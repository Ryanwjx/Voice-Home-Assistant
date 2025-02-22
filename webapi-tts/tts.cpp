#include "tts.h"
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonValue>
#include <QByteArray>
#include <QDebug>
#include <QThread>
#include <QFile>
#include <QTextStream>
#include <QProcess>

#define STATUS_FIRST_FRAME 0
#define STATUS_CONTINUE_FRAME 1
#define STATUS_LAST_FRAME 2

int  convertPCMtoWAV_ffmpeg(const QString &pcmFile, const QString &wavFile) {
    QStringList args;
    args << "-f" << "s16le"    // PCM 格式 (signed 16-bit little-endian)
         << "-ar" << "16000"   // 采样率 16kHz
         << "-ac" << "1"       // 单声道
         << "-i" << pcmFile
         << wavFile;

    return QProcess::execute("ffmpeg", args);
}

TTS::TTS(QObject *parent):m_audioFile(pathstr), m_status(STATUS_FIRST_FRAME)
{
    this->setParent(parent);

    webSocket = new QWebSocket();
    /* 需要加一些安全配置才能访问https */
    QSslConfiguration config;
    config.setPeerVerifyMode(QSslSocket::VerifyNone);
    config.setProtocol(QSsl::TlsV1SslV3);
    webSocket->setSslConfiguration(config);

    connect(webSocket, SIGNAL(connected()), this, SLOT(webSocketConnected()));//-websokets连接成功，调用连接成功函数
    connect(webSocket, SIGNAL(textMessageReceived(QString)), this, SLOT(onTextMessageReceived(QString)));

    // webSocket->sendTextMessage(cmd);
}

TTS::~TTS()
{
    delete webSocket;
    webSocket = nullptr;
}

void TTS::startTTS(QString text)
{   
    ttstext.clear();
    ttstext = text;
    webSocket->open(QUrl(QString("wss://tts-api.xfyun.cn/v2/tts?authorization=YXBpX2tleT0iNDZmNjE0MmQwM2QxOTEyYTI0NmRkOWI2OTZhMTdjYmYiLCBhbGdvcml0aG09ImhtYWMtc2hhMjU2IiwgaGVhZGVycz0iaG9zdCBkYXRlIHJlcXVlc3QtbGluZSIsIHNpZ25hdHVyZT0iMThHampteFVQMjNvRE5mekRUSHN5Wk5XQlA5T1FSMzhNT0IyaURzT1FHZz0i&date=Sun%2C+09+Feb+2025+07%3A12%3A24+GMT&host=ws-api.xfyun.cn")));  //开始连接服务器
}

void TTS::webSocketConnected()
{
    qDebug() << "tts WebSocket connected!";

    QFile file1(pathstr);
    file1.remove();
    file1.close();

    QFile file2(wavpathstr);
    file2.remove();
    file2.close();

    qDebug() << ttstext;
    // 组织 JSON 数据
    QJsonObject json;

    QJsonObject common;
    common["app_id"] = "165f0835";

    QJsonObject business;
    business["aue"] = "raw";
    business["auf"] = "audio/L16;rate=16000";
    business["vcn"] = "xiaoyan";
    business["tte"] = "utf8";

    // 进行 Base64 编码
    QByteArray buf = ttstext.toLocal8Bit();
    // QByteArray buf("这是语音");
    QString encodedAudio = QString::fromUtf8(buf.toBase64());
    QJsonObject data;
    data["status"] = 2;
    data["text"] = encodedAudio;

    json["common"] = common;
    json["business"] = business;
    json["data"] = data;

    // 发送 JSON 数据
    QJsonDocument jsonDoc(json);
    QString jsonString = QString::fromUtf8(jsonDoc.toJson(QJsonDocument::Compact));
    webSocket->sendTextMessage(jsonString);

    // // 最后一帧，关闭连接
    // if (m_status == STATUS_LAST_FRAME) {
    //     QThread::sleep(1);
    //     webSocket->close();
    // }
}

void TTS::onTextMessageReceived(const QString &message)
{
    qDebug() << "Received voice message ";

    QJsonDocument jsonDoc = QJsonDocument::fromJson(message.toUtf8());
    if (!jsonDoc.isObject()) {
        qDebug() << "Invalid JSON format";
        return;
    }

    QJsonObject jsonObj = jsonDoc.object();
    int code = jsonObj["code"].toInt();
    QString sid = jsonObj["sid"].toString();
    QJsonObject dataObj = jsonObj["data"].toObject();
    QString audioBase64 = dataObj["audio"].toString();
    int status = dataObj["status"].toInt();

    // 处理错误信息
    if (code != 0) {
        QString errMsg = jsonObj["message"].toString();
        qDebug() << QString("sid:%1 call error:%2 code:%3").arg(sid, errMsg).arg(code);
        return;
    }

    // 解码 Base64
    QByteArray audioData = QByteArray::fromBase64(audioBase64.toUtf8());

    // 将音频数据追加写入文件
    QFile file(pathstr);
    if (file.open(QIODevice::Append)) {
        file.write(audioData);
        file.close();
    } else {
        qDebug() << "Failed to open demo.pcm for writing!";
        return;
    }

    // // 处理 WebSocket 关闭逻辑
    // if (status == 2) {
    //     qDebug() << "WebSocket is closing...";
    //     webSocket->close();
    // }

    if(status == 2)
    {
        qDebug() << "语音获取成功";
        int exitCode = convertPCMtoWAV_ffmpeg(pathstr, wavpathstr);
        if(exitCode == -2 || exitCode == -1)
        {
            qDebug() << "PCM 转 WAV 失败!";
            return;
        }
        qDebug() << "PCM 转 WAV 完成!";
        emit ttsReadyData(wavpathstr); 

        webSocket->close(); 
    }
    
}