/******************************************************************
Copyright © Deng Zhimao Co., Ltd. 1990-2021. All rights reserved.
* @projectName   03_appmainview
* @brief         widget.h
* @author        Deng Zhimao
* @email         1252699831@qq.com
* @net           www.openedv.com
* @date          2021-06-09
*******************************************************************/
#ifndef WIDGET_H
#define WIDGET_H
#include <QWidget>
#include <QMainWindow>

#include "../slidepage/slidepage.h"
#include "../smarthome-MQTT/smarthome-MQTT/smarthome-MQTT.h"
#include "../Voice-LLM-Assistant/Voice-LLM-Assistant/Voice-LLM-Assistant.h" 

// class SlidePage;
//为什么要声明上述？ :可不需要包含上述，但是当pri等包含错误，通过这种方法可很快解决。注意：RESOURCES += \ 的\加上后续才有效
//pri包含用 pwd会好些 ：用$$PWD可以很方便构建依赖
//同名类会不会冲突？ ：会冲突，避免
//当函数输入A类，A类的衍生类也行？：是的，输入指针时，会隐式转换为基类，如果函数内只使用基类信息是不会出现问题的

class AppMainView : public QWidget
{
    Q_OBJECT

public:
    AppMainView(QWidget *parent = nullptr);
    ~AppMainView();

private:
    /* 背景 */
    QWidget *bgWidget ;

    /* 滑动页面管理对象 */
    SlidePage *mySlidePage;

    VoiceLLMAssistant *myLLM;

    smarthomeMQTT *myMQTT;
    /* 重设大小 */
    void resizeEvent(QResizeEvent *event);


};
#endif // WIDGET_H
