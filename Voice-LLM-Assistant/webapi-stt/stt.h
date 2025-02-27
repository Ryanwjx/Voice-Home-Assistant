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
    void abandonstt();
private:    
    // WebSocket
    QWebSocket *webSocket;

    //timer
    QTimer *timer;
private:
    QString g_audioFileName;
    QFile g_audioFile;
    QString g_FullText;
    int g_status;

    QString APPID;
    QString APIKey;
    QString APISecret;

    QUrl getURL();

private slots:
    // WebSocket
    void webSocketConnected();
    void webSocketDisconnected();
    void onTextMessageReceived(const QString &message);

    //timer
    void sendAudioFrame();

signals:
    void sttReadyData(QString);
};
#endif // WEBAPI_H
