/******************************************************************
Copyright © Deng Zhimao Co., Ltd. 1990-2021. All rights reserved.
* @projectName   02_asr_demo
* @brief         mainwindow.cpp
* @author        Deng Zhimao
* @email         1252699831@qq.com
* @net           www.openedv.com
* @date          2021-06-04
*******************************************************************/
#include "mainwindow.h"
#include <QDebug>
#include <QSound>
#include <unistd.h>
#include <QProcess>

int  aplay_playwav(const QString &wavFile) {
    QStringList args;
    args << wavFile;
    return QProcess::execute("aplay", args);
}

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    this->setGeometry(0, 0, 800, 480);
    this->setStyleSheet("background:#14161a");

    mainWidget = new QWidget();
    movieWidget = new QWidget();

    movieLabel = new QLabel();
    textLabel = new QLabel();

    movieWidget->setMinimumWidth(240);
    textLabel->setMinimumSize(240, 50);
    textLabel->setStyleSheet("color: white; font-size: 20px");
    textLabel->setText("请点击，开始说话...");
    textLabel->setAlignment(Qt::AlignCenter);

    /* 安装事件过滤器 */
    movieLabel->installEventFilter(this);   //触摸时,启动画面，播放音频，启动time2
    movieLabel->setFixedSize(240, 240);

    vBoxLayout = new QVBoxLayout();
    hBoxLayout = new QHBoxLayout();

    vBoxLayout->addWidget(movieWidget);
    vBoxLayout->addWidget(textLabel);
    vBoxLayout->setAlignment(Qt::AlignCenter);

    mainWidget->setLayout(vBoxLayout);  //-mainWidget通过vBox存 movieWidget textLabel

    hBoxLayout->addWidget(movieLabel);
    movieWidget->setLayout(hBoxLayout);//-hBox通过hBox存 movieLabel

    setCentralWidget(mainWidget);

    myMovie = new QMovie(":/gif/voice_effect.gif");
    /* 设置播放速度，值越大越快 */
    myMovie->setSpeed(50);
    movieLabel->setMovie(myMovie);
    movieLabel->show();
    /* 设置设置化时显示第一帧 */
    myMovie->jumpToNextFrame();

    movieLabel->setAlignment(Qt::AlignHCenter);

    timer1 = new QTimer(this);
    timer2 = new QTimer(this);
    timer3 = new QTimer(this);

    connect(timer1, SIGNAL(timeout()), this, SLOT(onTimer1TimeOut()));  //请求转换，启动time3
    connect(timer2, SIGNAL(timeout()), this, SLOT(onTimer2TimeOut()));  //记录声音，启动Time1
    connect(timer3, SIGNAL(timeout()), this, SLOT(onTimer3TimeOut()));  //恢复

    /* 自定义的录音类 */
    myAudioRecorder = new AudioRecorder(this);

    /* 自定义的百度云识别录音类 */
    // myAsr = new Asr(this);
    mytts = new TTS(this);
    mystt = new STT(this);

    myllm = new LLM(this);

    /* LED */
    myLed = new Led(this);


    mediaplayer = new QMediaPlayer(this);

    // connect(myAsr, SIGNAL(asrReadyData(QString)), this, SLOT(onAsrReadyData(QString))); //根据asr发送的信号，进行结果显示
    connect(mystt, SIGNAL(sttReadyData(QString)), this, SLOT(onsttReadyData(QString))); //根据stt发送的信号，进行结果显示
    connect(myllm, SIGNAL(llmReadyData(QString)), this, SLOT(onllmReadyData(QString))); //根据llm发送的信号，进行结果显示
    connect(mytts, SIGNAL(ttsReadyData(QString)), this, SLOT(onttsReadyData(QString))); //根据llm发送的信号，进行结果显示
}

MainWindow::~MainWindow()
{
}

bool MainWindow::eventFilter(QObject *watched, QEvent *event){

    if (watched == movieLabel && event->type() == QEvent::MouseButtonPress) {
        QSound::play(":/audio/sound.wav");
        if (myMovie->state() != QMovie::Running) {
            /* 等待QSound播放完,1.5s后再录音 */
            timer2->start(1500);
            textLabel->setText("正在听您说话，请继续...");
            myMovie->start();
        }
    }

    return QWidget::eventFilter(watched, event);
}

void MainWindow::onTimer1TimeOut()
{
    /* 停止录音，8s钟的短语音 */
    myAudioRecorder->stopRecorder();
    textLabel->setText("正在识别，请稍候...");
    timer1->stop();
    myMovie->stop();
    QString fileName = QCoreApplication::applicationDirPath() + "/16k.wav";
    // myAsr->getTheResult(fileName);


    mystt->startstt(fileName);
    // myllm->sendMessage(textdata);
    timer3->start(30000);
}

void MainWindow::onTimer2TimeOut()
{
    timer1->start(3000);
    /* 开始录音 */
    myAudioRecorder->startRecorder();
    timer2->stop();
}

void MainWindow::onTimer3TimeOut()
{
    textLabel->setText("请点击，开始说话...");
    timer3->stop();
}

// void MainWindow::onAsrReadyData(QString str)
// {
//     if (str.contains("开灯"))
//         myLed->setLedState(true);
//     else if (str.contains("关灯"))
//         myLed->setLedState(false);

//     textLabel->setText("识别结果是:\n" + str);
//     textLabel->adjustSize();
// }

void MainWindow::onsttReadyData(QString str)
{
    // if (str.contains("开灯"))
    //     myLed->setLedState(true);
    // else if (str.contains("关灯"))
    //     myLed->setLedState(false);

    textLabel->setText("识别结果是:\n" + str);
    textLabel->adjustSize();

    myllm->sendMessage(str);
}

void MainWindow::onllmReadyData(QString str)
{
    textLabel->setText("LLM回答结果是:\n" + str);
    textLabel->adjustSize();

    mytts->startTTS(str);
}

void MainWindow::onttsReadyData(QString filepath)
{
    if (filepath.isEmpty() || !QFile::exists(filepath)) {
        qDebug() << "文件路径无效：" << filepath;
        return;
    }

    // mediaplayer->setMedia(QUrl::fromLocalFile(filepath));
    // mediaplayer->setVolume(100); 
    // mediaplayer->play();

    aplay_playwav(filepath);

    // QString path = "./16k.wav";
    // mystt->startstt(path);
}
