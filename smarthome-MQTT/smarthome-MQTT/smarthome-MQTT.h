/******************************************************************
Copyright © Deng Zhimao Co., Ltd. 1990-2021. All rights reserved.
* @projectName   01_smarthome
* @brief         mainwindow.h
* @author        Deng Zhimao
* @email         1252699831@qq.com
* @net           www.openedv.com
* @date          2021-05-26
*******************************************************************/
#ifndef smarthome_MQTT_H
#define smarthome_MQTT_H

#include <QMainWindow>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QGridLayout>
#include <QListWidget>
#include <QLayout>
#include "switchbutton.h"
#include "../webapi-MQTT/MQTT.h"
#include <QMap>

class smarthomeMQTT : public QWidget
{
    Q_OBJECT

public:
    smarthomeMQTT(QWidget *parent = nullptr);
    ~smarthomeMQTT();

private:
    MQTT *MQTT_Client;

    QLabel *label[4];
    QPushButton *cloudPushButton;
    QWidget *widget[11];
    QVBoxLayout *vBoxLayout[4];
    QHBoxLayout *hBoxLayout[6];
    QPushButton *pushButton[3];
    QListWidget *listWidget;
    QLabel *itemLabel[9];
    QGridLayout *gridLayout[3];
    SwitchButton *switchButton[3];
    QVBoxLayout *mainLayout;

    QMap<QString,QString> place_ch2en = {
        {"客厅","meetingroom"},
        {"主卧","mainbedroom"},
        {"次卧","secondbedroom"}
    };
    QMap<QString,QString> device_ch2en = {
        {"灯","light"},
        {"空调","airconditioner"}
    };
    /* 界面布局 */
    void layoutInit();
    void loadStyle();

private slots:
    void onToggled(bool);
    void deviceState(const QByteArray &message,const QMqttTopicName &topic);
};
#endif // smarthome_MQTT_H
