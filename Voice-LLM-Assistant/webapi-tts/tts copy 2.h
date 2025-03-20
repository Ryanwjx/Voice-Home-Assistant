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
    void abandonTTS();
private:
    QWebSocket *webSocket;
    QString pathstr = "./demo.pcm";

    QFile g_audioFile;
    QString g_ttstext;

    QString APPID;
    QString APIKey;
    QString APISecret;

    void TTSConnect();
    QUrl getURL();
private slots:

    // WebSocket
    void webSocketConnected();
    void webSocketDisconnected();
    void onTextMessageReceived(const QString &message);

signals:
    void ttsReadyData(QString);
};
#endif // WEBAPI_H
