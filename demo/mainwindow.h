/******************************************************************
Copyright © Deng Zhimao Co., Ltd. 1990-2021. All rights reserved.
* @projectName   02_asr_demo
* @brief         mainwindow.h
* @author        Deng Zhimao
* @email         1252699831@qq.com
* @net           www.openedv.com
* @date          2021-06-04
*******************************************************************/
#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QVBoxLayout>
#include <QLabel>
#include <QMovie>
#include <QTimer>
#include <QHBoxLayout>
#include <QMediaPlayer>
#include <QAudioOutput>
#include "../audiorecorder/audiorecorder.h"
#include "../webapi-stt/stt.h"
#include "../webapi-tts/tts.h"
#include "../webapi-llm/llm.h"

class Asr;
class AudioRecorder;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private:   
    /* 主Widget */
    QWidget *mainWidget;
    /* gif底下的Widget */
    QWidget *movieWidget;
    /* gif动画画布 */
    QLabel *movieLabel;
    /* 用于显示识别结果 */
    QLabel *textLabel;
    /* 垂直布局 */
    QVBoxLayout *vBoxLayout;
    /* 水平布局 */
    QHBoxLayout *hBoxLayout;
    /* 用于显示GIF动画 */
    QMovie *myMovie;

    /* 事件过滤器 */
    bool eventFilter(QObject *watched, QEvent *event);  //重定义

    void setup();

private:
    /* 定时器 */
    QTimer *time;

    TTS *mytts;
    STT *mystt;
    LLM *myllm;

    /* 录音类 */
    AudioRecorder *myAudioRecorder;

    /* 播放类 */
    QFile g_sourceFile;   // class member.
    QAudioOutput* audio; // class member.

private slots:
    void onaudioReadyData(QString); //--触发结束录音、获取识别结果 槽函数
    void ontimeTimeOut(); //--触发启动录音，触发onTimer1TimeOut调用 槽函数 
   
    void onsttReadyData(QString);   //--触发结果更新，更新屏幕显示、触发LLM
    void onllmReadyData(QString);
    void onttsReadyData(QString);

    void audiohandleStateChanged(QAudio::State newState);
};
#endif // MAINWINDOW_H
