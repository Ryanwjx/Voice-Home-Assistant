#ifndef LLM_H
#define LLM_H

#include <QWidget>
#include <QTextBrowser>
#include <QTextEdit>
#include <QPushButton>
#include <QVBoxLayout>
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>

class LLM : public QObject {
    Q_OBJECT

public:
    LLM(QWidget *parent = nullptr);
    ~LLM();

    void sendMessage(QString);

private slots:
    void onNetworkfinished(QNetworkReply *reply);

private:
    QNetworkAccessManager *networkManager;
    
signals:
    void llmReadyData(QString);
    
};

#endif // CHATWINDOW_H
