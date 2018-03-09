/******************************************************************************************
** 文件名:   ikevent.h
×× 主要类:   (无)
**
** Copyright (c) 中科虹霸有限公司
** 创建人:   fjf
** 日  期:   2015-01-26
** 修改人:
** 日  期:
** 描  述:   全局事件类定义，用于发送和接收event通知和处理event事件
** ×× 主要模块说明：
**    串口数据传输：Serialvent事件
**    识别事件： IKEventIns事件
** 版  本:   1.0.0
** 备  注:
**
******************************************************************************************/
#pragma once
#include <QObject>

class SerialEvent : public QObject
{
    Q_OBJECT
public:
    SerialEvent();
    void onGetICCardNum(QString);

signals:
    void GetICCrad(QString);
};


//全局事件类，用于线程间同步
class IKEvent : public QObject
{
    Q_OBJECT
    friend class IKEventIns;
public:
    //广播要关闭红外灯
    void CloseInfrared();
    //回应关闭红外灯操作，仅在红外灯已关闭时调用
    void ReplyInfraredClosed();

private:
    IKEvent();
    ~IKEvent(){}
    IKEvent(const IKEvent &e) = delete;
    IKEvent& operator =(const IKEvent& e) = delete;

signals:
    void sigCloseInfrared();
    void sigInfraredClosed();
};


//
class IKEventIns
{
public:
    IKEventIns();
    ~IKEventIns();
    IKEvent* GetIKEvent();

private:
   static IKEvent *e;
};
