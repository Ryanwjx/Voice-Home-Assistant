#include "llm.h"
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>

LLM::LLM(QWidget *parent) {
    networkManager = new QNetworkAccessManager(this);

    // connect(sendButton, &QPushButton::clicked, this, &ChatWindow::sendMessage);
    connect(networkManager, &QNetworkAccessManager::finished, this, &LLM::onNetworkfinished);

    // sendMessage("你好。");
}
LLM::~LLM()
{

}

void LLM::sendMessage(QString userInput) {
    // QString userInput = "你好啊！";//inputField->toPlainText().trimmed();
    // if (userInput.isEmpty()) return;

    QNetworkRequest request(QUrl("https://spark-api-open.xf-yun.com/v1/chat/completions"));
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");  //说明
    request.setRawHeader("Authorization", "Bearer EqjkoSOdLnNactrzgXnR:eKTYVlqpsYOVnGUTylFq");

    QJsonObject json;
    json["model"] = "4.0Ultra";
    QJsonArray messages;
    QJsonObject message;
    message["role"] = "user";
    message["content"] = userInput;  // 这里替换成用户输入
    messages.append(message);
    json["messages"] = messages;
    json["stream"] = false;  // 是否启用流式传输


    QJsonDocument doc(json);
    QByteArray postData = doc.toJson();
    QNetworkReply *reply = networkManager->post(request, postData);

    // connect(reply, SIGNAL(finished()), this, SLOT(onNetworkfinished(QNetworkReply *reply)));
    // connect(reply, SIGNAL(readyRead()), this, SLOT(readyReadData()));
}

void LLM::onNetworkfinished(QNetworkReply *reply) {
    QByteArray responseData = reply->readAll();
    // qDebug() << "Raw Reply Data:" << responseData;  // 打印完整的 HTTP 响应

    if (reply->error() != QNetworkReply::NoError) {
        // chatDisplay->append("Error: " + reply->errorString());
        reply->deleteLater();
        return;
    }
    QJsonDocument doc = QJsonDocument::fromJson(responseData);
    QJsonObject obj = doc.object();
    // qDebug() << "Complete JSON Response: " << obj;

    QJsonArray choicesArray = obj["choices"].toArray();  // 获取choices数组
    if (!choicesArray.isEmpty()) {
        QJsonObject firstChoice = choicesArray.first().toObject();  // 获取第一个choice对象

        QJsonObject messageObj = firstChoice["message"].toObject();  // 获取message对象
        QString content = messageObj["content"].toString();  // 获取content字段的值

        // 在聊天界面显示Bot的回复
        // chatDisplay->append("Bot: " + content);
        qDebug() << "Receive Response: " << content;
        emit llmReadyData(content);
    } else {
        // 处理未找到choices的情况
        // chatDisplay->append("Bot: No response found");
    }   
    reply->deleteLater();
}

QUrl STT::getURL()
{
    QString url = "ws://spark-api.xf-yun.com/v4.0/chat";
    QString host = "spark-api.xf-yun.com";
    QDateTime dateTime; 
    QString dateTime_str = dateTime.currentDateTime().toString("ddd, dd MMM yyyy HH:mm:ss 'GMT'");

    QString signature_origin =  QString("host: ") + host + '\n'+
                                QString("date: ") + dateTime_str + '\n'+
                                QString("GET /v1.1/chat HTTP/1.1");
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