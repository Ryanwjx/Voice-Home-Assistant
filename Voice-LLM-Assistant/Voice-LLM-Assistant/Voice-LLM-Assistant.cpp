/******************************************************************
Copyright © Deng Zhimao Co., Ltd. 1990-2021. All rights reserved.
* @projectName   02_asr_demo
* @brief         mainwindow.cpp
* @author        Deng Zhimao
* @email         1252699831@qq.com
* @net           www.openedv.com
* @date          2021-06-04
*******************************************************************/
#include "Voice-LLM-Assistant.h"
#include <QDebug>
#include <QSound>
#include <unistd.h>
#include <QProcess>

#include <QAudioFormat>
#include <QAudioOutput>
#include <QApplication>

VoiceLLMAssistant::VoiceLLMAssistant(QWidget *parent)
    : QMainWindow(parent)
{
    setup();

    /* 自定义的录音类 */
    myAudioRecorder = new AudioRecorder(this);
    connect(myAudioRecorder, SIGNAL(audioReadyData(QString)), this, SLOT(onaudioReadyData(QString)));

    /* 定时器类，控制运行流 */
    time = new QTimer(this);

    connect(time, SIGNAL(timeout()), this, SLOT(ontimeTimeOut())); 

    /* 自定义的百度云识别录音类 */
    mytts = new TTS(this);
    mystt = new STT(this);
    myllm = new LLM(this);

    connect(mystt, SIGNAL(sttReadyData(QString)), this, SLOT(onsttReadyData(QString))); //根据stt发送的信号，进行结果显示
    connect(myllm, SIGNAL(LLMReadyData(QString)), this, SLOT(onllmReadyData(QString))); //根据llm发送的信号，进行结果显示
    connect(mytts, SIGNAL(ttsReadyData(QString)), this, SLOT(onttsReadyData(QString))); //根据llm发送的信号，进行结果显示

    /* 播放 */
    QAudioFormat fmt;
    fmt.setSampleRate(16000);   // 设置采样率
    fmt.setSampleSize(16);      // 设置样本大小
    fmt.setChannelCount(1);     // 设置使用双通道
    fmt.setCodec("audio/pcm");  // 设置编解码器
    fmt.setByteOrder(QAudioFormat::LittleEndian);   // 使用小端
    fmt.setSampleType(QAudioFormat::SignedInt); 
    QAudioDeviceInfo info(QAudioDeviceInfo::defaultOutputDevice());
    if (!info.isFormatSupported(fmt)) {
        qWarning() << "Raw audio format not supported by backend, cannot play audio.";
    }
    audio = new QAudioOutput(fmt, this);
    connect(audio, SIGNAL(stateChanged(QAudio::State)), this, SLOT(audiohandleStateChanged(QAudio::State)));

    // onttsReadyData("./demo.pcm");
}

VoiceLLMAssistant::~VoiceLLMAssistant()
{
}

void VoiceLLMAssistant::setup()
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
}

bool VoiceLLMAssistant::eventFilter(QObject *watched, QEvent *event){

    if (watched == movieLabel && event->type() == QEvent::MouseButtonPress) {
        QSound::play(":/audio/sound.wav");
        if (myMovie->state() != QMovie::Running) {
            /* 可能stt llm tts正在运行 */
            //stt 和 llm 获取了回答
            //tts 播放
            mystt->abandonstt();
            myllm->abandonLLM();
            mytts->abandonTTS();
            if(audio->state() != QAudio::IdleState)
            {
                audio->stop();
            }

            /* 等待QSound播放完,1.5s后再录音 */
            time->start(1);
            textLabel->setText("正在听您说话，请继续...");
            myMovie->start();
        }
        else
        {
            myMovie->stop();
            myAudioRecorder->stopRecorder();
            textLabel->setText("请点击，开始说话...");
        }
    }

    return QWidget::eventFilter(watched, event);
}

void VoiceLLMAssistant::ontimeTimeOut()
{
    /* 开始录音 */
    myAudioRecorder->startRecorder();
    time->stop();
}

void VoiceLLMAssistant::onaudioReadyData(QString filename)
{
    myMovie->stop();
    if(filename == nullptr)
    {
        textLabel->setText("请点击，开始说话...");
        return;
    }
    /* 停止录音，8s钟的短语音 */
    textLabel->setText("正在识别，请稍候...");
    
    QString fileName = QCoreApplication::applicationDirPath() + "/16k.wav";

    mystt->startstt(fileName);
    // myllm->sendMessage(textdata);
}

void VoiceLLMAssistant::onsttReadyData(QString str)
{
    // if (str.contains("开灯"))
    //     myLed->setLedState(true);
    // else if (str.contains("关灯"))
    //     myLed->setLedState(false);

    textLabel->setText("STT识别结果是:\n" + str);
    textLabel->adjustSize();

    myllm->startLLM(str);
}

void VoiceLLMAssistant::onllmReadyData(QString str)
{
    textLabel->setText("LLM回答结果是:\n" + str);
    textLabel->adjustSize();

    mytts->startTTS(str);
}

void VoiceLLMAssistant::onttsReadyData(QString filepath)
{
    if (filepath.isEmpty() || !QFile::exists(filepath)) {
        qDebug() << "文件路径无效：" << filepath;
        return;
    }

    g_sourceFile.setFileName(filepath); 
    g_sourceFile.open(QIODevice::ReadOnly);
    audio->start(&g_sourceFile);
}

void VoiceLLMAssistant::audiohandleStateChanged(QAudio::State newState)
{
    switch (newState) {
        case QAudio::IdleState:
            audio->stop();
            qDebug() << "audio over";
            textLabel->setText("请点击，开始说话...");
            break;
        
        case QAudio::StoppedState:
            // Stopped for other reasons
            if (audio->error() == QAudio::NoError) {
                g_sourceFile.close();
                qDebug() << "audio stop";
            }
            break;
        default:
            break;
    }
}