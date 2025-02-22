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