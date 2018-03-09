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

//#include <phonon>
#include <sys/time.h>
#include <unistd.h>
#include <QPainter>

#include <thread>
#include <mutex>
#include <QTimer>
//#include <QTimerEvent>

//@ for sdk #include "AlgApi.h"
#include "IKUSBSDK.h"  //for sdk
#include "../common.h"
//增加串口类，实现从红外测距模块得到距离值
#include "../SerialMuxDemux/SerialMuxDemux/include/serialmuxdemux.h"
#include "../Common/AudioPlay.h"

#include <QAudioOutput>
#include <QAudioFormat>
#include <QMediaPlayer>
#include <QFile>

#define TTS_NO

#ifdef TTS
#include "SVEnginePool.h"
#include "SVEngine.h"
#include "engine_output.h"

#include <unistd.h>
#include <QTextCodec>
#include <QString>
#include <iostream>
#include <iconv.h>

#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>
#include <pulse/simple.h>
#include <pulse/error.h>
#include <stdlib.h>



#define BUFSIZE 1024
#define OUTLEN 255
#define BUFSIZE 1024

#endif

const int g_constSoundInterval = 5;//发声间隔，只对注册中、识别中播放语音有效

const int g_ppmHeaderLength = 18;

//全局变量，红外测距值
extern int g_infraredDistanceValue;


//20141208
//默认的追踪框大小
const int g_defaultTraceSize = 100;

//屏幕画布大小
const int g_convasHeight = 600;
const int g_convasWidth = 1024;

//采集到的图像显示在界面上的起始坐标
const int g_imgBeginX = 178;//180
const int g_imgBeginY = 104;//100

//采集到的图像显示在界面上的尺寸
const int g_imgWidth = 664;
const int g_imgHeight = 498;


//以下坐标是相对于img而言
//默认的左眼追踪框起始坐标
const int g_defaultTraceLeftBeginX = 120;//160 //180
const int g_defaultTraceLeftBeginY = 150;//240

//默认的右眼追踪框起始坐标
const int g_defaultTraceRightBeginX = g_imgWidth - g_defaultTraceLeftBeginX - g_defaultTraceSize -20;//460
const int g_defaultTraceRightBeginY = 150;//240

//提示框的默认尺寸
const int g_statusWidth = 500;
const int g_statusHeight = 120;
const int g_statusBeginX = (g_imgWidth - g_statusWidth)/2;
const int g_statusBeginY = 300;


const int g_MinEnrollOcclusion    =  70;
const int g_MinEnrollQualityScore =  55;
//采集到的图像在界面上显示时的放大比例
const float g_scaling = (float)g_imgWidth / IK_IRIS_IMG_WIDTH;/*IK_IRIS_IMG_WIDTH/g_imgWidth;*///1;//g_imgWidth / IK_IRIS_IMG_WIDTH;

//每多少次播报一次状态提示，用于播放远近语音提示和显示远近状态条
const int g_noticeNum = 30;//大概3秒钟 //hzf 20161025


enum IfHasPerson
{
    NoPerson,   //无人
    WaitPerson, //大于红外灯开启距离时 20141204 lizhl
    HasPerson   //有人
};//状态变化是 有人 -》 wait -》 无人

enum ArrowDirection
{
    NOArrow,
    Point2Center,
    Point2Outer
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
    ResultUnknown,   //结果未知或者目前没有结果
    AlgorithmIdle   //空闲

};

//20141218 状态条（远近、识别状态、注册状态等）的类型
enum StatusLabelType
{
    StatusAlgIsPerform, //注册或者识别正在进行
    StatusEnrNotStart,  //注册尚未开始
    StatusEnrStart,     //注册刚刚开始
    StatusEnrStop,      //注册被人为停止
    StatusEnrFailed,    //注册失败
    StatusEnrSuccess,   //注册成功
    StatusIdenStop,     //识别停止
    StatusIdenSuccess,  //识别成功
    StatusIdenFailed,   //识别失败
    StatusIdenWait,     //等待识别状态
    StatusCardAndIri    //刷卡+虹膜
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
    // for sdk overloaded
    void PlayInteraction(IN InteractionResultType resultType,IN IKIrisDistFlag distEstimation,IN bool isUpdatePos);

    void ClearResult();//清除人机交互的注册、识别结果信息。当有识别结果后，界面会一直保持识别结果的相关信息，直到调用本函数后才进行下一次处理

    void ScreenPressResponse();

    void ResetDisplay();

    // 关闭USB模块
    void CloseUsb();
    // 打开USB模块
    void StartUsb();

    void SendOffSuperCmd();

    void SendOnSuperCmd();

private:
    Interaction();
    Interaction(const Interaction&);//禁止拷贝赋值
    Interaction& operator=(const Interaction&);       
    int  GetSleepTime();

private:
    static void* GetDistanceThread(void* pParam);    //获取红外测距值线程
    static std::map<std::string, InteractionCallBack> s_interactionCallBacks;//回调函数

private slots:
    void TimerForInfraredLEDTimeOut();
    //for wake sdk
    void TimerForTest();

    //20150113
    void CloseInfrared();

public:
    static int s_infraredDistanceValue;             //红外测距值
    int     _sleepTime;

    unsigned int noticeCount;                       //20141218 用于播放远近语音提示和显示远近状态条的计数值

private:

    static Interaction *s_pSingletonInteraction;
    static std::mutex s_interactionInstanceMutex;   //获取Interaction实例mutex
    std::mutex _setHasPersonMutex;

    //Phonon::MediaObject *_sound;        //播放语音的MediaObject
    AudioPlay *ap;
    //QFile _inputFile;
    //QAudioOutput *_audio;
    //QAudioFormat _format;

    struct timeval _tvSoundStart;       //发声开始时间
    bool _updateTvStart;                //是否更新发声开始时间

    struct timeval _tvHasPerson;        //上次有人的时间，因为要在线程中使用，因此定义为静态变量
    struct timeval _tvNoPerson;         //当前没人的时间，因为要在线程中使用，因此定义为静态变量

    IfHasPerson _hasPerson;                    //是否有人标志位
    bool _screenPressed;                //屏幕点击标志位

    bool _resultFlag;                   //结果标志位，表明目前是否有结果
public:
    bool _infraredLEDOnFlag;            //红外灯是否开启标志
    bool _irisCameraOnFlag;             //虹膜摄像头是否开启标志
    bool  testHasPerson;
private:

    //@dj for wake test
    QTimer _timerTestWake;
    QTimer _timerForInfraredLED;       //红外灯点亮状态定时器

    bool _testFlag;

    std::thread _getDistanceThread;     //获取红外测距值线程
    SerialMuxDemux *_serialDistance;    //串口测距类

//    int _linePos;
//    int _variableRadius;



    InteractionResultType _resultType;


    //20150113
    bool ifClose;

#ifdef TTS
private:
    static void* PlaySoundThread(void* pParam);
    std::thread _playSoundThread;
#endif
};

void distanceCB(const unsigned char* data, const int size);
#endif // INTERACTION_H
