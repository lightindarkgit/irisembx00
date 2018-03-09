/******************************************************************************************
** 文件名:   alarm.h
×× 主要类:   (无)
**
** Copyright (c) 中科虹霸有限公司
** 创建人:   Wang.L
** 日  期:   2015-01-28
** 修改人:
** 日  期:
** 描  述:   闹铃模块声明
** ×× 主要模块说明：
**
** 版  本:   1.0.0
** 备  注:
**
******************************************************************************************/
#ifndef ALARM_H
#define ALARM_H
#include <QObject>
#include <QTimer>
#include <QStringList>

#include <../SerialMuxDemux/SerialMuxDemux/include/serialmuxdemux.h>


class Alarm : public QObject
{
    Q_OBJECT
public:
    static Alarm* getInstance();
    static void release();


    static void updateAlarms();

private slots:
    void slotTimeout();

private:
    Alarm();
    ~Alarm();

    bool init();
    bool startRinging();
    bool stopRinging();

    bool getAlarmDuration(int& dur);


    //闹铃回调函数
    static void alarmCallback(const unsigned char* data, const int size);

    //响铃线程
    static void* ringAlarm(void* arg);



private:
    static Alarm* alarmer;
    static int refCounter;

//    QStringList alarmTimers;
    QTimer timer;

    SerialMuxDemux* sm;
    int alarmDuration;
    const int maxTry = 60;

//    bool optOpenOK;
//    bool optCloseOK;
};

#endif // ALARM_H
