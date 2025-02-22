#ifndef STT_H
#define STT_H

#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QWebSocket>
#include <unistd.h>
#include <QFile>
#include <QTimer>

class STT : public QObject
{
    Q_OBJECT

public:
    STT(QObject *parent = nullptr);
    ~STT();

    void startstt(QString fileName);

private:

    QString fullText;
    
    QTimer *timer;

    /* 返回的字符串信息 */
    QString dataString;

    // WebSocket
    QWebSocket *webSocket;

    // QString pathstr = "/mnt/linux_app/llm/iat_pcm_8k.pcm";
    QFile m_audioFile;
    QTimer m_timer;
    int m_status;
private slots:

    // WebSocket
    void webSocketConnected();
    void sendAudioFrame();

    void onTextMessageReceived(const QString &message);
    void onBinaryMessageReceived(const QByteArray &message);
public:
    void whichDeviceNameSendCmd(QString name, QString message);

signals:
    void sttReadyData(QString);
};
#endif // WEBAPI_H
