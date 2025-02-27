#include "MQTT.h"
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonValue>
#include <QByteArray>
#include <QDebug>
#include <QThread>
#include <QFile>
#include <QTextStream>
#include <QJsonArray>

// QString mqttTopicPublish  = "wjx/esp32/command"; // 订阅的主题
// QString mqttTopicSubscribe = "wjx/esp32/status";   // 发布的主题

MQTT::MQTT(QObject *parent) 
{
    this->setParent(parent);

    mqtt_client = new QMqttClient();

    connect(mqtt_client, SIGNAL(connected()),     //-websokets连接成功，调用连接成功函数
            this, SLOT(MQTTConnected()));
    connect(mqtt_client, SIGNAL(disconnected()),     //-websokets连接成功，调用连接成功函数
            this, SLOT(MQTTDisonnected()));
}

MQTT::~MQTT()
{
    delete mqtt_client;
    mqtt_client = nullptr;
}
void MQTT::Connect(QString thost, quint16 tport, QString tclientid)
{
    host = thost;            //"broker.emqx.io";
    port = tport;            //1883;                //好像这个端口才行
    clientid = tclientid;    //"mqttx_15590b49";
    mqtt_client->setHostname(host);
    mqtt_client->setPort(port);
    mqtt_client->setClientId(clientid);
    mqtt_client->setProtocolVersion(QMqttClient::MQTT_3_1_1);
    mqtt_client->setKeepAlive(10000);

    mqtt_client->connectToHost();
}

void MQTT::MQTTConnected()
{
    qDebug() << "MQTT connected!";
    connect(mqtt_client, SIGNAL(messageReceived(QByteArray,QMqttTopicName)), 
            this, SLOT(onMessageReceived(QByteArray,QMqttTopicName))); 

    for(QMqttTopicFilter Topic : subscribe_devices)
        mqtt_client->subscribe(Topic);
}

void MQTT::MQTTDisonnected()
{
    qDebug() << "MQTT Disconnected!";
}

void MQTT::onMessageReceived(const QByteArray & message,const QMqttTopicName & topic)
{
    qDebug() <<"Topic:" << topic.name() << " Received text message: " << QString(message);
    emit mqttMessageReceivedReady(message,topic);
}

void MQTT::SetSubscribeDevice(QMqttTopicName topic)
{
    qDebug() << "Topic:" << topic.name() << " is Subscribed";
    subscribe_devices.push_back(QMqttTopicFilter(topic.name()));
}

void MQTT::SendCommand(QByteArray device_command,QMqttTopicName topic)
{
    qDebug() << "Topic:" << topic.name() << " Send Command: " << QString(device_command);
    mqtt_client->publish(topic.name(), device_command);
}