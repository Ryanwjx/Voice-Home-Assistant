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
#include <QMessageAuthenticationCode>
#include <QUrlQuery>

#define STATUS_FIRST_FRAME 0
#define STATUS_CONTINUE_FRAME 1
#define STATUS_LAST_FRAME 2

STT::STT(QObject *parent): g_status(STATUS_FIRST_FRAME)
{
    this->setParent(parent);

    webSocket = new QWebSocket();
    /* 需要加一些安全配置才能访问https */
    QSslConfiguration config;
    config.setPeerVerifyMode(QSslSocket::VerifyNone);
    config.setProtocol(QSsl::TlsV1SslV3);
    webSocket->setSslConfiguration(config);

    timer = new QTimer();

    connect(webSocket, SIGNAL(connected()),     //-websokets连接成功，调用连接成功函数
            this, SLOT(webSocketConnected()));
    connect(webSocket, SIGNAL(textMessageReceived(QString)), 
            this, SLOT(onTextMessageReceived(QString)));
    connect(webSocket, SIGNAL(disconnected()),
            this, SLOT(webSocketDisconnected()));

    connect(timer, SIGNAL(timeout()), this, SLOT(sendAudioFrame()));
    // webSocket->sendTextMessage(cmd);

    APPID = "165f0835";
    APIKey = "46f6142d03d1912a246dd9b696a17cbf";
    APISecret = "ZGFlMzhmYjk2ZDczN2RkZWI4MjJlZGI5";

    // startstt("/mnt/linux_app/llm/iat_pcm_8k.pcm");
}

STT::~STT()
{
    delete webSocket;
    webSocket = nullptr;
    delete timer;
    timer = nullptr;
}

void STT::startstt(QString fileName)
{
    qDebug() << fileName;
    g_audioFileName = fileName; //初始化 设置输入

    // 启动定时器，每 40ms 发送一次音频数据
    QUrl qurl = getURL();
    webSocket->open(qurl);  //开始连接服务器
}

void STT::abandonstt()
{
    webSocket->close(); 
}

void STT::webSocketConnected()
{
    qDebug() << "stt WebSocket connected!";

    g_audioFile.setFileName(g_audioFileName);   //初始化 文件
    if (!g_audioFile.open(QIODevice::ReadOnly)) {
        qDebug() << "Failed to open audio file!";
        return;
    }

    g_status = STATUS_FIRST_FRAME;  //初始化 发送状态

    g_FullText.clear(); //初始化 接收信息

    timer->start(40);
    qDebug() << "timer start";
    // QString path = "./iat_pcm_8k.pcm";
    // startstt(path);
}

void STT::webSocketDisconnected()
{
    qDebug() << "stt WebSocket disconnected!";
    g_audioFile.close();    //清理 关闭文件
}

void STT::sendAudioFrame()
{
    QByteArray buf = g_audioFile.read(16000);
    if (buf.isEmpty()) {
        g_status = STATUS_LAST_FRAME;
    }
    // 进行 Base64 编码
    QString encodedAudio = QString::fromUtf8(buf.toBase64());

    // 组织 JSON 数据
    QJsonObject json;
    QJsonObject data;
    data["status"] = g_status;
    data["format"] = "audio/L16;rate=16000";
    data["audio"] = encodedAudio;
    data["encoding"] = "raw";

    if (g_status == STATUS_FIRST_FRAME) {
        QJsonObject common;
        common["app_id"] = APPID;

        QJsonObject business;
        business["domain"] = "iat";
        business["language"] = "zh_cn";
        business["accent"] = "mandarin";

        json["common"] = common;
        json["business"] = business;
        json["data"] = data;

        g_status = STATUS_CONTINUE_FRAME;
    } else {
        json["data"] = data;
    }

    // 发送 JSON 数据
    QJsonDocument jsonDoc(json);
    QString jsonString = QString::fromUtf8(jsonDoc.toJson(QJsonDocument::Compact));
    int sendnum = webSocket->sendTextMessage(jsonString);

    // 最后一帧，关闭连接
    if (g_status == STATUS_LAST_FRAME || sendnum == 0) {
        timer->stop();
        qDebug() << "STT over";
    }
}

void STT::onTextMessageReceived(const QString &message)
{
    qDebug() << "Received text message: " << message;

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
            g_FullText += word;  // 拼接单词
        }
    }

    // qDebug() << "recevied status: " << status;

    if(status == 2)
    {
        qDebug() << "full text result: " << g_FullText;
        emit sttReadyData(g_FullText);

        webSocket->close();
    }    

}

QUrl STT::getURL()
{
    QString url = "ws://iat-api.xfyun.cn/v2/iat";
    QString host = "iat-api.xfyun.cn";
    QDateTime dateTime; 
    QString dateTime_str = dateTime.currentDateTime().toString("ddd, dd MMM yyyy HH:mm:ss 'GMT'");

    QString signature_origin =  QString("host: ") + host + '\n'+
                                QString("date: ") + dateTime_str + '\n'+
                                QString("GET /v2/iat HTTP/1.1");
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

    qDebug() << qurl;
    return qurl;
}