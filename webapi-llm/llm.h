#ifndef LLM_H
#define LLM_H

#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QWebSocket>
#include <unistd.h>
#include <QFile>
#include <QTimer>

class LLM : public QObject
{
    Q_OBJECT

public:
    LLM(QObject *parent = nullptr);
    ~LLM();

    void startLLM(QString tuserInput);
    void abandonLLM();
private:    
    // WebSocket
    QWebSocket *webSocket;

    //timer
    QTimer *timer;

private:
    /* 输入的字符 */
    QString g_UserInput;
    /* 返回的字符串信息 */
    QString g_FullText;
    QString APPID;
    QString APIKey;
    QString APISecret;

    QUrl getURL();

private slots:
    // WebSocket
    void webSocketConnected();
    void webSocketDisconnected();
    void onTextMessageReceived(const QString &message);
    void webSocketError(QAbstractSocket::SocketError error);
signals:
    void LLMReadyData(QString);
};
#endif // WEBAPI_H
