/******************************************************************
Copyright © Deng Zhimao Co., Ltd. 1990-2021. All rights reserved.
* @projectName   audiorecorder
* @brief         audiorecorder.h
* @author        Deng Zhimao
* @email         1252699831@qq.com
* @net           www.openedv.com
* @date          2021-06-04
*******************************************************************/
#ifndef AUDIORECORDER_H
#define AUDIORECORDER_H

#include <QAudioRecorder>
#include <QAudioProbe>
#include <QAudioBuffer>

class AudioRecorder : public QObject
{
    Q_OBJECT

public:
    AudioRecorder(QWidget *parent = nullptr);
    ~AudioRecorder();

    void startRecorder(QString filename = "16k.wav");    
    void stopRecorder(); 
private:   
    /* 录音类 */
    QAudioRecorder *m_audioRecorder = nullptr;

    /* 用于探测缓冲区的level */
    QAudioProbe *m_probe = nullptr;

    /* 录音设置容器，保存录音设备的可用信息 */
    QList<QVariant>devicesVar;
    QList<QVariant>codecsVar;
    QList<QVariant>containersVar;
    QList<QVariant>sampleRateVar;
    QList<QVariant>channelsVar;
    QList<QVariant>qualityVar;
    QList<QVariant>bitratesVar;

    QString filename = "16k.wav";

    qint64 g_nosie_time;
    qint64 g_record_time;
    qreal audio_noise_thred = 0.4;
    qint64 nosie_time_thred = 20;
    qreal unavaliable_record_thred = 0.9;

private slots:   
    /* 更新录音时长 */
    void updateProgress(qint64);    //--录音时长槽函数

    /* 更新录音level */
    void processBuffer(const QAudioBuffer&);    //--录音音量等级槽函数

signals:
    void audioReadyData(QString);
};
#endif // AUDIORECORDER_H
