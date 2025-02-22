#ifndef TTS_H
#define TTS_H

#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QWebSocket>
#include <unistd.h>
#include <QFile>
#include <QTimer>

class TTS : public QObject
{
    Q_OBJECT

public:
    TTS(QObject *parent = nullptr);
    ~TTS();

    void startTTS(QString text);

private:
    /* 返回的字符串信息 */
    QString dataString;

    // WebSocket
    QWebSocket *webSocket;

    QString pathstr = "./demo.pcm";
    QString wavpathstr = "./output.wav";

    QFile m_audioFile;
    QTimer m_timer;

    QString ttstext;

    int m_status;
private slots:

    // WebSocket
    void webSocketConnected();
    void onTextMessageReceived(const QString &message);

signals:
    void ttsReadyData(QString);
};
#endif // WEBAPI_H
