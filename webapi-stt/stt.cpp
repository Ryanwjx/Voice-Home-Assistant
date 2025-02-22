#include "stt.h"
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonValue>
#include <QByteArray>
#include <QDebug>
#include <QThread>
#include <QFile>
#include <QTextStream>
#include <QJsonArray>


#define STATUS_FIRST_FRAME 0
#define STATUS_CONTINUE_FRAME 1
#define STATUS_LAST_FRAME 2

STT::STT(QObject *parent): m_status(STATUS_FIRST_FRAME)
{
    this->setParent(parent);

    webSocket = new QWebSocket();
    /* 需要加一些安全配置才能访问https */
    QSslConfiguration config;
    config.setPeerVerifyMode(QSslSocket::VerifyNone);
    config.setProtocol(QSsl::TlsV1SslV3);
    webSocket->setSslConfiguration(config);

    connect(webSocket, SIGNAL(connected()),     //-websokets连接成功，调用连接成功函数
            this, SLOT(webSocketConnected()));
    connect(webSocket, SIGNAL(binaryMessageReceived(QByteArray)),   //-websokets收到二进制信息，调用处理函数
            this, SLOT(onBinaryMessageReceived(QByteArray)));
    connect(webSocket, SIGNAL(textMessageReceived(QString)), 
            this, SLOT(onTextMessageReceived(QString)));


    // webSocket->sendTextMessage(cmd);
}

STT::~STT()
{
    delete webSocket;
    webSocket = nullptr;
}


void STT::startstt(QString fileName)
{
    qDebug() << fileName;

    if (m_audioFile.isOpen()) {
        m_audioFile.close();
        qDebug() << "close file:" <<fileName;
    }

    m_audioFile.setFileName(fileName);
    if (!m_audioFile.open(QIODevice::ReadOnly)) {
        qDebug() << "Failed to open audio file!";
        return;
    }

    // 启动定时器，每 40ms 发送一次音频数据
    connect(&m_timer, SIGNAL(timeout()), this, SLOT(sendAudioFrame()));

    webSocket->open(QUrl(QString("ws://iat-api.xfyun.cn/v2/iat?authorization=YXBpX2tleT0iNDZmNjE0MmQwM2QxOTEyYTI0NmRkOWI2OTZhMTdjYmYiLCBhbGdvcml0aG09ImhtYWMtc2hhMjU2IiwgaGVhZGVycz0iaG9zdCBkYXRlIHJlcXVlc3QtbGluZSIsIHNpZ25hdHVyZT0iZ3lYRUtBdXdIMGFPL2g1R3dSQzBvNVVKdXZwcGVXMEo1eFRGOEJzWVQyaz0i&date=Sun%2C+09+Feb+2025+05%3A05%3A34+GMT&host=iat-api.xfyun.cn")));  //开始连接服务器
}

void STT::webSocketConnected()
{
    qDebug() << "stt WebSocket connected!";
    m_timer.start(40);
    qDebug() << "m_timer start";
    // QString path = "./iat_pcm_8k.pcm";
    // startstt(path);
}

void STT::sendAudioFrame()
{
    QByteArray buf = m_audioFile.read(16000);
    // qDebug() << buf;
    if (buf.isEmpty()) {
        m_status = STATUS_LAST_FRAME;
        m_timer.stop();
    }
    qDebug() << "m_status"<<m_status;
    // 进行 Base64 编码
    QString encodedAudio = QString::fromUtf8(buf.toBase64());

    // 组织 JSON 数据
    QJsonObject json;
    QJsonObject data;
    data["status"] = m_status;
    data["format"] = "audio/L16;rate=16000";
    data["audio"] = encodedAudio;
    data["encoding"] = "raw";

    if (m_status == STATUS_FIRST_FRAME) {
        QJsonObject common;
        common["app_id"] = "165f0835";

        QJsonObject business;
        business["domain"] = "iat";
        business["language"] = "zh_cn";
        business["accent"] = "mandarin";

        json["common"] = common;
        json["business"] = business;
        json["data"] = data;

        m_status = STATUS_CONTINUE_FRAME;
    } else {
        json["data"] = data;
    }

    // 发送 JSON 数据
    QJsonDocument jsonDoc(json);
    QString jsonString = QString::fromUtf8(jsonDoc.toJson(QJsonDocument::Compact));
    webSocket->sendTextMessage(jsonString);

    // 最后一帧，关闭连接
    if (m_status == STATUS_LAST_FRAME) {
        m_status = STATUS_FIRST_FRAME;
        qDebug() << "STT over";
    }
}

void STT::onTextMessageReceived(const QString &message)
{
    qDebug() << "Received text message: " << message;
    // emit requestllm(message);

    // 解析 JSON 消息
    QJsonDocument jsonDoc = QJsonDocument::fromJson(message.toUtf8());
    if (!jsonDoc.isObject()) {
        qDebug() << "Invalid JSON format";
        return;
    }

    QJsonObject jsonObj = jsonDoc.object();

    // 提取状态码
    int code = jsonObj["code"].toInt();
    QString msg = jsonObj["message"].toString();
    QString sid = jsonObj["sid"].toString();

    if (code != 0) {
        qDebug() << "Error: " << msg << ", Code: " << code;
        return;
    }

    // 解析 `data` 部分
    QJsonObject dataObj = jsonObj["data"].toObject();
    if (!dataObj.contains("result")) {
        qDebug() << "No result data found!";
        return;
    }

    // 解析 `result`
    QJsonObject resultObj = dataObj["result"].toObject();
    QJsonArray wsArray = resultObj["ws"].toArray();

    // 解析 status
    int status = dataObj["status"].toInt();
    // 拼接解析后的文本
    for (const QJsonValue &wsValue : wsArray) {
        QJsonObject wsObj = wsValue.toObject();
        QJsonArray cwArray = wsObj["cw"].toArray();
        for (const QJsonValue &cwValue : cwArray) {
            QJsonObject cwObj = cwValue.toObject();
            QString word = cwObj["w"].toString();
            fullText += word;  // 拼接单词
        }
    }

    qDebug() << "recevied status: " << status;

    if(status == 2)
    {
        qDebug() << "full text result: " << fullText;
        emit sttReadyData(fullText);
        fullText.clear(); 

        webSocket->close();
    }    

}

void STT::onBinaryMessageReceived(const QByteArray &message)
{
    qDebug() << "Received binary message: " << message;
}