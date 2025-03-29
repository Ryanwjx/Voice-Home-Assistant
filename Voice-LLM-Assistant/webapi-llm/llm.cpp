#include "llm.h"
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
#include <QVector>

#define STATUS_FIRST_FRAME 0
#define STATUS_CONTINUE_FRAME 1
#define STATUS_LAST_FRAME 2

LLM::LLM(QObject *parent)
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
    connect(webSocket, SIGNAL(textMessageReceived(QString)), 
            this, SLOT(onTextMessageReceived(QString)));
    connect(webSocket, SIGNAL(disconnected()),
            this, SLOT(webSocketDisconnected()));
    connect(webSocket, SIGNAL(error(QAbstractSocket::SocketError)), this, SLOT(webSocketError(QAbstractSocket::SocketError)));

    APPID = "xxx";
    APIKey = "xxx";
    APISecret = "xxx";

    // startLLM("现在几点了");
}

LLM::~LLM()
{
    delete webSocket;
    webSocket = nullptr;
}

void LLM::startLLM(QString tuserInput)
{
    qDebug() << tuserInput;
    g_UserInput = tuserInput;

    QUrl qurl = getURL();
    // qDebug() <<qurl;
    webSocket->open(qurl);  //开始连接服务器
}

void LLM::abandonLLM()
{
    webSocket->close(); 
}

void LLM::webSocketConnected()
{
    QDateTime dateTime= QDateTime::currentDateTime();//获取系统当前的时间
    QString str = dateTime .toString("hh:mm:ss.zzz");

    qDebug() << str <<'\t' << "LLM WebSocket connected!";

    g_FullText.clear(); //初始化 接收信息
    
    // 构造 JSON 数据
    QJsonObject header;
    header["app_id"] = APPID;
    // header["uid"] = "12345";

    QJsonObject chat;
    chat["domain"] = "4.0Ultra";
    chat["temperature"] = 0.5;
    chat["max_tokens"] = 4096;

    QJsonObject parameter;
    parameter["chat"] = chat;

    QJsonObject userMessage;
    userMessage["role"] = "user";
    userMessage["content"] =  g_UserInput;

    QJsonArray textArray;
    textArray.append(userMessage);

    QJsonObject message;
    message["text"] =  textArray;

    QJsonObject payload;
    payload["message"] = message;

    QJsonObject full_message;
    full_message["header"] = header;
    full_message["parameter"] = parameter;
    full_message["payload"] = payload;

    // 将 JSON 对象转换为字符串
    QJsonDocument doc(full_message);
    QByteArray json = doc.toJson();
    QString jsonString = QString::fromUtf8(json);
    qDebug() << jsonString.toStdString().data();
    // 发送 JSON 字符串
    webSocket->sendTextMessage(jsonString);
}

void LLM::webSocketError(QAbstractSocket::SocketError error) {
        qDebug() << "LLM WebSocket error occurred!";
        qDebug() << "LLM Error code:" << error;

        // 你可以根据不同的错误代码来进行更详细的错误处理
        switch (error) {
            case QAbstractSocket::HostNotFoundError:
                qDebug() << "Host not found";
                break;
            case QAbstractSocket::ConnectionRefusedError:
                qDebug() << "Connection refused";
                break;
            case QAbstractSocket::RemoteHostClosedError:
                qDebug() << "Remote host closed the connection";
                break;
            // case QAbstractSocket::TimeoutError:
            //     qDebug() << "Connection timed out";
            //     break;
            default:
                qDebug() << "Other error: " << error;
        }
}

void LLM::webSocketDisconnected()
{
    QDateTime dateTime= QDateTime::currentDateTime();//获取系统当前的时间
    QString str = dateTime .toString("hh:mm:ss.zzz");

    qDebug() << str <<'\t' << "LLM WebSocket disconnected!";
}

void LLM::onTextMessageReceived(const QString &ret_message)
{
    qDebug() << "Received text message: " << ret_message;
    // emit requestllm(message);

    // 解析 JSON 消息
    QJsonDocument jsonDoc = QJsonDocument::fromJson(ret_message.toUtf8());
    if (!jsonDoc.isObject()) {
        qDebug() << "Invalid JSON format";
        return;
    }

    QJsonObject jsonObj = jsonDoc.object();

    // 解析 header 部分
    QJsonObject header = jsonObj["header"].toObject();
    int code = header["code"].toInt();
    QString message = header["message"].toString();
    QString sid = header["sid"].toString();
    int status = header["status"].toInt();

    // 处理错误信息
    if (code != 0) {
        qDebug() << QString("sid:%1 call error:%2 code:%3").arg(sid, message).arg(code);
        return;
    }

    // 解析 payload 部分
    QJsonObject payload = jsonObj["payload"].toObject();
    
    // 解析 choices 部分
    QJsonObject choices = payload["choices"].toObject();
    int choiceStatus = choices["status"].toInt();
    int seq = choices["seq"].toInt();
    QJsonArray textArray = choices["text"].toArray();
    
    // // 输出 text 中的内容
    // for (int i = 0; i < textArray.size(); ++i) {
    //     QJsonObject textObj = textArray[i].toObject();
    //     QString content = textObj["content"].toString();
    //     QString role = textObj["role"].toString();
    //     int index = textObj["index"].toInt();

    //     g_FullText += content;
    // }

    // if(status == STATUS_LAST_FRAME)
    // {
    //     qDebug() << "full text result: " << g_FullText;
    //     emit LLMReadyData(g_FullText);

    //     webSocket->close();
    // }    

    // 输出 text 中的内容
    for (int i = 0; i < textArray.size(); ++i) {
        QJsonObject textObj = textArray[i].toObject();
        QString content = textObj["content"].toString();
        QString role = textObj["role"].toString();
        int index = textObj["index"].toInt();

        g_FullText += content;     
    }

    int maxlen = 25;
    if(g_FullText.size() >= maxlen) //大于maxlen的结果，
    {
        int lastPeriodIndex = g_FullText.lastIndexOf("。");
        QString answer("");
        if (lastPeriodIndex != -1)      //如果有。则提取句子。然后申请转换
        {
            answer = g_FullText.mid(0, lastPeriodIndex + 1);    //很奇怪，”。“好像是两个字符，但只+1少一半
            g_FullText = g_FullText.mid(lastPeriodIndex + 1);   //很奇怪，”。“好像是两个字符，但+1多一半
            emit LLMReadyData(answer);
        }
        else                        // 否则退而求其次找其余符号。然后申请转换
        {
            const QVector<QString> chinesePunctuation = {"？","，","：","；",".",",","!"};
            int lastChineseSentenceIndex = -1;
            for (int i = 0; i < chinesePunctuation.size(); ++i)
            {
                int tmpIndex = g_FullText.lastIndexOf(chinesePunctuation[i]);
                lastChineseSentenceIndex = lastChineseSentenceIndex < tmpIndex ? tmpIndex : lastChineseSentenceIndex;
            }
            if (lastChineseSentenceIndex != -1)     //找到分段符号，则按照句子划分tts
            {
                answer = g_FullText.mid(0, lastChineseSentenceIndex + 1);
                g_FullText = g_FullText.mid(lastChineseSentenceIndex + 1);
                emit LLMReadyData(answer);
            }
            else
            {
                answer = g_FullText.mid(0, maxlen);  //否则按照max个字符
                g_FullText = g_FullText.mid(maxlen + 1);
                emit LLMReadyData(answer);
            }
        }
    }
    if(status == STATUS_LAST_FRAME)
    {
        // qDebug() << "full text result: " << g_FullText;
        if(g_FullText.size() > 0){
            emit LLMReadyData(g_FullText);
        }
        webSocket->close();
    }  
}

QUrl LLM::getURL()
{
    QString url = "wss://spark-api.xf-yun.com/v4.0/chat";
    QString host = "spark-api.xf-yun.com";
    QDateTime dateTime; 
    // QString dateTime_str = dateTime.currentDateTime().toString("ddd, dd MMM yyyy HH:mm:ss 'GMT'"); 
    //其他没问题时候，需要考虑服务器时间 -+ 5 = 客户端时间
    //但是发现服务器时间 比正常时间慢8h!!!
    QString dateTime_str = dateTime.currentDateTime().addSecs(-8 * 3600).toString("ddd, dd MMM yyyy HH:mm:ss 'GMT'"); 

    QString signature_origin =  QString("host: ") + host + '\n'+
                                QString("date: ") + dateTime_str + '\n'+
                                QString("GET /v4.0/chat HTTP/1.1");
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

    return qurl;
}
