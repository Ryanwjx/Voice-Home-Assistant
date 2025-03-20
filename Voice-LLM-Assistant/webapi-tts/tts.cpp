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
#include <QMessageAuthenticationCode>
#include <QUrlQuery>

#define STATUS_FIRST_FRAME 0
#define STATUS_CONTINUE_FRAME 1
#define STATUS_LAST_FRAME 2

TTS::TTS(QObject *parent):g_audioFile(pathstr)
{
    this->setParent(parent);

    webSocket = new QWebSocket();
    /* 需要加一些安全配置才能访问https */
    QSslConfiguration config;
    config.setPeerVerifyMode(QSslSocket::VerifyNone);
    config.setProtocol(QSsl::TlsV1SslV3);
    webSocket->setSslConfiguration(config);

    connect(webSocket, SIGNAL(connected()), this, SLOT(webSocketConnected()));//-websokets连接成功，调用连接成功函数
    connect(webSocket, SIGNAL(disconnected()), this, SLOT(webSocketDisconnected()));
    connect(webSocket, SIGNAL(textMessageReceived(QString)), this, SLOT(onTextMessageReceived(QString)));

    APPID = "165f0835";
    APIKey = "46f6142d03d1912a246dd9b696a17cbf";
    APISecret = "ZGFlMzhmYjk2ZDczN2RkZWI4MjJlZGI5";
    // webSocket->sendTextMessage(cmd);
}

TTS::~TTS()
{
    delete webSocket;
    webSocket = nullptr;
}

void TTS::FillBufTTS(QString text)
{
    g_ttstext.enqueue(text);
    startTTS();
}

void TTS::startTTS()
{   
    if(!g_audioprocessflag && g_ttstext.size() != 0)
    {
        QUrl qurl = getURL();
        webSocket->open(qurl);  //开始连接服务器
    }
}

void TTS::abandonTTS()
{
    webSocket->close(); 
}

void TTS::webSocketConnected()
{
    QDateTime dateTime= QDateTime::currentDateTime();//获取系统当前的时间
    QString datestr = dateTime .toString("hh:mm:ss.zzz");

    qDebug() << datestr <<'\t' << "tts WebSocket connected!";

    g_audioFile.open(QIODevice::Append);    //初始化 打开文件

    g_audioprocessflag = true;      //初始化 设置启动tts标志

    QString str = g_ttstext.dequeue();
    qDebug() << str;
    // 组织 JSON 数据
    QJsonObject json;

    QJsonObject common;
    common["app_id"] = APPID;

    QJsonObject business;
    business["aue"] = "raw";
    business["auf"] = "audio/L16;rate=16000";
    business["vcn"] = "xiaoyan";
    business["tte"] = "utf8";

    // 进行 Base64 编码
    QByteArray buf = str.toLocal8Bit();
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
}

void TTS::webSocketDisconnected()
{
    QDateTime dateTime= QDateTime::currentDateTime();//获取系统当前的时间
    QString str = dateTime .toString("hh:mm:ss.zzz");

    qDebug() << str <<'\t' << "tts WebSocket disconnected!";
    g_audioFile.close();  //清理 关闭文件
    g_audioprocessflag = false;      //清理 重置启动tts标志

    startTTS(); //重启 
}

void TTS::onTextMessageReceived(const QString &message)
{
    // qDebug() << "Received voice message ";

    // while(1);

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
    g_audioFile.write(audioData);
    // qDebug() << "追加后pos变换：" <<g_audioFile.pos();

    if(status == STATUS_LAST_FRAME)
    {
        qDebug() << "PCM语音获取成功";
        emit ttsReadyData(pathstr);  
        webSocket->close(); 
    }
}

QUrl TTS::getURL()
{
    QString url = "ws://tts-api.xfyun.cn/v2/tts";
    QString host = "ws-api.xfyun.cn";
    QDateTime dateTime; 
    QString dateTime_str = dateTime.currentDateTime().toString("ddd, dd MMM yyyy HH:mm:ss 'GMT'");

    QString signature_origin =  QString("host: ") + host + '\n'+
                                QString("date: ") + dateTime_str + '\n'+
                                QString("GET /v2/tts HTTP/1.1");
    QByteArray message = signature_origin.toUtf8();
    QByteArray key = APISecret.toUtf8();
    QByteArray signature_byte = QMessageAuthenticationCode::hash(message,key,QCryptographicHash::Sha256).toBase64();
    QString signature = QString(signature_byte);

    QString authorization_origin = QString("api_key=\"%1\", algorithm=\"%2\", headers=\"%3\", signature=\"%4\"").arg(APIKey).arg("hmac-sha256").arg("host date request-line").arg(signature);
    QByteArray authorization_byte = authorization_origin.toUtf8().toBase64();
    QString authorization = QString(authorization_byte);

    QUrl qurl(url);
    QUrlQuery query;
    query.addQueryItem("authorization",authorization);
    query.addQueryItem("date",dateTime_str);
    query.addQueryItem("host",host);
    qurl.setQuery(query);

    // qDebug() <<qurl.toString().toStdString().data();
    return qurl;
}