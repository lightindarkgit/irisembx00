/*****************************************************************************
** 文 件 名：interaction.h
** 主 要 类：Interaction
**
** Copyright (c) 中科虹霸有限公司
**
** 创 建 人：lizhl
** 创建时间：2014-01-08
**
** 修 改 人：
** 修改时间：
** 描  述:   人机交互，负责语音提示、界面提示等
** 主要模块说明: 人机交互类
**
** 版  本:   1.0.0
** 备  注:
**
*****************************************************************************/
#ifndef INTERACTION_H
#define INTERACTION_H

#include <phonon>
#include <sys/time.h>
#include <unistd.h>
#include <QPainter>

#include <thread>
#include <mutex>
#include <QTimer>
//#include <QTimerEvent>

#include "../../03 Code/AlgApi/SRC/AlgApi.h"
#include "../common.h"
//增加串口类，实现从红外测距模块得到距离值
#include "../bin/serialmuxdemux.h"

const int g_constSoundInterval = 5;//发声间隔，只对注册中、识别中播放语音有效

//全局变量，红外测距值
extern int g_infraredDistanceValue;

enum IfHasPerson
{
    NoPerson,   //无人
    HasPerson   //有人
};

//人机交互类注册回调函数定义，用于通知注册者当前设备前是否有人
typedef void (*InteractionCallBack)(IN IfHasPerson ifHasPerson);

//人机交互结果类型
enum InteractionResultType
{
    EnrollSuccess,  //注册成功
    EnrollFailed,   //注册失败
    EnrollPerformig,//注册中
    IdenSuccess,    //识别成功
    IdenFailed,     //识别失败
    IdenKeepLastStatus,//保存识别上一次结果状态，只界面显示，不播放语音
    IdenPerforming, //识别中
    ResultUnknown   //结果未知或者目前没有结果

};

class Interaction : public QObject
{
    Q_OBJECT
public:
    static Interaction* GetInteractionInstance();
    ~Interaction();

    //注册回调函数
    bool RegisterCallBack(IN std::string strName, IN InteractionCallBack callBack);
    //删除回调函数
    void DeleteCallBack(IN std::string strName);

    //人机交互显示，如是否清晰，绘制结果；播放结果语音
    void PlayInteraction(IN InteractionResultType resultType, IN LRIrisClearFlag lrIrisClearFlag, IN IrisPositionFlag irisPositionFlag, OUT QImage &paintImg);
    void ClearResult();//清除人机交互的注册、识别结果信息。当有识别结果后，界面会一直保持识别结果的相关信息，直到调用本函数后才进行下一次处理

private:
    Interaction();
    Interaction(const Interaction&);//禁止拷贝赋值
    Interaction& operator=(const Interaction&);

    void PlayInteractionSound(IN InteractionResultType resultType, IN IrisPositionFlag irisPositionFlag);        //播放人机交互语音提示，靠近、靠远等
    void DisplayClearBlur(OUT QImage &paintImg, IN LRIrisClearFlag lrIrisClearFlag);              //显示图像是否清晰提示框
    void ProcessResult(IN InteractionResultType resultType, OUT QImage &paintImg);  //处理注册、识别结果的输出

private:
    static void* GetDistanceThread(void* pParam);    //获取红外测距值线程
    static std::map<std::string, InteractionCallBack> s_interactionCallBacks;//回调函数

//    virtual void timerEvent(QTimerEvent *event);    //处理红外灯点亮状态定时器事件
private slots:
    void TimerForInfraredLEDTimeOut();

public:
    static int s_infraredDistanceValue; //红外测距值

private:
    static Interaction *s_pSingletonInteraction;
    static std::mutex s_interactionInstanceMutex;                   //获取Interaction实例mutex

    static Phonon::MediaObject *s_sound;//播放语音的MediaObject
    static struct timeval s_tvSoundStart; //发声开始时间
    static bool s_updateTvStart;         //是否更新发声开始时间
    bool _resultFlag;               //结果标志位，表明目前是否有结果

    bool _infraredLEDOnFlag;             //红外灯是否开启标志
//    int _infraredLEDTimerID;            //红外灯点亮状态定时器ID
    QTimer *_timerForInfraredLED;       //红外灯点亮状态定时器



    std::thread _getDistanceThread; //获取红外测距值线程
    SerialMuxDemux *_serialDistance;//串口测距类
};

void distanceCB(const unsigned char* data, const int size);
#endif // INTERACTION_H
