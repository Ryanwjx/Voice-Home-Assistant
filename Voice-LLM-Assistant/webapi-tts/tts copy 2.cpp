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

    TTSConnect();

    //         webSocket2 = new QWebSocket();
    // QUrl qurl = getURL();
    TTSConnect();

    //     connect(webSocket2, SIGNAL(connected()), this, SLOT(webSocketConnected()));//-websokets连接成功，调用连接成功函数
    // connect(webSocket2, SIGNAL(disconnected()), this, SLOT(webSocketDisconnected()));
    // connect(webSocket2, SIGNAL(textMessageReceived(QString)), this, SLOT(onTextMessageReceived(QString)));
     g_audioFile.open(QIODevice::Append);
        //初始化 打开文件
        g_audioFile.resize(0);
}

TTS::~TTS()
{
    delete webSocket;
    webSocket = nullptr;
}

void TTS::TTSConnect()
{
    QUrl qurl = getURL();
    webSocket->open(qurl); 
}

void TTS::abandonTTS()
{
    webSocket->close(); 
}

void TTS::webSocketConnected()
{
    QDateTime dateTime= QDateTime::currentDateTime();//获取系统当前的时间
    QString str = dateTime .toString("hh:mm:ss.zzz");

    qDebug() << str <<'\t' << "tts WebSocket connected!";

  

    static int time = 0;
    if(time == 0)
    {
        startTTS("你好");
         qDebug() << "你好";
    }
    else
       {
         startTTS("中国");
    qDebug() << "中国";
       }

    time++;
}

void TTS::webSocketDisconnected()
{
    QDateTime dateTime= QDateTime::currentDateTime();//获取系统当前的时间
    QString str = dateTime .toString("hh:mm:ss.zzz");

    qDebug() << str <<'\t' << "tts WebSocket disconnected!";
    g_audioFile.close();  //清理 关闭文件

    qDebug() << str <<'\t' << "tts WebSocket try connect!";
    // TTSConnect();
}

void TTS::startTTS(QString text)
{   
    g_ttstext = text;   //初始化 设置输入
    qDebug() << "语音text: "<< g_ttstext;
    
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
    QByteArray buf = g_ttstext.toLocal8Bit();
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


    static int time = 0;
    if(time == 0)
    {
        webSocket->sendTextMessage(jsonString);
         qDebug() << text;
    }
    else
       {
        webSocket2->sendTextMessage(jsonString);
    qDebug() << text;
       }

    time++;
    

    
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
    g_audioFile.write(audioData);

    if(status == STATUS_LAST_FRAME)
    {
        qDebug() << "PCM语音获取成功";
        emit ttsReadyData(pathstr);  
        // webSocket->close(); 
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

    qDebug() <<qurl.toString().toStdString().data();
    return qurl;
}