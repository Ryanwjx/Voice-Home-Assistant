#ifndef MQTT_H
#define MQTT_H

#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QWebSocket>
#include <unistd.h>
#include <QFile>
#include <QTimer>

#include <QCoreApplication>
#include <QtMqtt/QMqttClient>
#include <QtMqtt/QMqttSubscription>
#include <QtWebSockets/QWebSocketProtocol>

class MQTT : public QObject
{
    Q_OBJECT

public:
    MQTT(QObject *parent = nullptr);
    ~MQTT();

    void Connect(QString thost, quint16 tport, QString tclientid);
    void SendCommand(QByteArray device_command,QMqttTopicName topic);
    void SetSubscribeDevice(QMqttTopicName Topic);
private:
    
    // WebSocket
    QMqttClient *mqtt_client;

    QString host;
    QString clientid;
    quint16 port;
    QVector<QMqttTopicFilter> subscribe_devices;

private slots:

    // WebSocket
    void MQTTConnected();
    void MQTTDisonnected();
    void onMessageReceived(const QByteArray & message,const QMqttTopicName & topic);

signals:
    void mqttMessageReceivedReady(const QByteArray &message,const QMqttTopicName &topic);
};
#endif // WEBAPI_H
