#ifndef TTS_H
#define TTS_H

#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QWebSocket>
#include <unistd.h>
#include <QFile>
#include <QTimer>
#include <QQueue>

class TTS : public QObject
{
    Q_OBJECT

public:
    TTS(QObject *parent = nullptr);
    ~TTS();

    void FillBufTTS(QString text);
    void abandonTTS();
private:
    QWebSocket *webSocket;

    QString pathstr = "./demo.pcm";

    QFile g_audioFile;
    QQueue<QString> g_ttstext;
    bool g_audioprocessflag = false;

    QString APPID;
    QString APIKey;
    QString APISecret;

    QUrl getURL();
    void startTTS();
private slots:

    // WebSocket
    void webSocketConnected();
    void webSocketDisconnected();
    void onTextMessageReceived(const QString &message);

signals:
    void ttsReadyData(QString);
};
#endif // WEBAPI_H
