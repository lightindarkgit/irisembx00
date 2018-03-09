/*****************************************************************************
** 文 件 名：interaction.cpp
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
#include "interaction.h"
#include "../XML/xml/IKXmlApi.h"
#include "ikevent.h"
#include <math.h>   //为了在distanceCB中使用pow
#include "../Common/Logger.h"

#include <QDir>

#include <QDateTime>
#include <QtDebug>
//#include <QSound>
#include "../Common/Exectime.h"

#define DISPLAY_2_RECTANGLE_NO
//显示一个框时的框尺寸
//30,140,580,320  //30,30,580,420
const int g_1_rec_leftx = 30;
const int g_1_rec_lefty = 30;//30 //140
const int g_1_rec_width = 580;
const int g_1_rec_height = 420;//420 //320

//全局变量，红外测距值
//int g_infraredDistanceValue = 0;
int Interaction::s_infraredDistanceValue = 0; //红外测距值
const int LEDOnDistanceValue = 50; //@for sdk  用户设定红外判断是否有人的距离

std::mutex Interaction::s_interactionInstanceMutex;
Interaction* Interaction::s_pSingletonInteraction = NULL;
std::map<std::string, InteractionCallBack> Interaction::s_interactionCallBacks;//回调函数
//20150113
IKEventIns ikEventIns;
IKEvent *ikEvent;

//显示的是小图时的相关尺寸
const int g_constSmallRectWidth     = 160;    //矩形框宽度
const int g_constSmallRectHeight    = 120;    //矩形框高度
const int g_constSmallCrossSize     = 80;     //叉的尺寸
const int g_constSmallCircleDiameter= 80;     //圆的直径
const int g_constSmallStart_L_Y     = 200;    //左框的左上角纵坐标 150
const int g_constSmallStart_R_Y     = 200;    //右框的左上角纵坐标 150
const int g_constSmallStart_L_X     = 100;    //左框的左上角横坐标
const int g_constSmallStart_R_X     = 380;   //右框的左上角横坐标

const int num = 42;//扫描框在整幅图像中移动的次数//30
const int step = 420/num;//扫描框移动的步长

const QLineF linef[12];

const int lineLen = 50;
const int X0 = 30;
const int X1 = X0 + lineLen;
const int X2 = IK_IRIS_IMG_WIDTH/2 - lineLen/2;
const int X3 = IK_IRIS_IMG_WIDTH/2 + lineLen/2;
const int X5 = IK_IRIS_IMG_WIDTH - 30;
const int X4 = X5 - lineLen;

const int Y0 = 30;
const int Y1 = Y0 + lineLen;
const int Y2 = IK_IRIS_IMG_HEIGHT/2 - lineLen/2;
const int Y3 = IK_IRIS_IMG_HEIGHT/2 + lineLen/2;
const int Y5 = IK_IRIS_IMG_HEIGHT - 30;
const int Y4 = Y5 - lineLen;

QLineF g_leftLineF[7];
QLineF g_rightLineF[7];
const int g_lineLen = 50;

const int g_lineX0 = 30;
const int g_lineX1 = g_lineX0 + g_lineLen;
const int g_lineX2 = IK_IRIS_IMG_WIDTH/2 - g_lineLen/2;
const int g_lineX3 = IK_IRIS_IMG_WIDTH/2;
const int g_lineX4 = IK_IRIS_IMG_WIDTH/2 + g_lineLen/2;
const int g_lineX6 = IK_IRIS_IMG_WIDTH - 30;
const int g_lineX5 = g_lineX6 - g_lineLen;

const int g_lineY0 = 30;
const int g_lineY1 = g_lineY0 + lineLen;
const int g_lineY2 = IK_IRIS_IMG_HEIGHT/2 - g_lineLen/2;
const int g_lineY3 = IK_IRIS_IMG_HEIGHT/2 + g_lineLen/2;
const int g_lineY5 = IK_IRIS_IMG_HEIGHT - 30;
const int g_lineY4 = g_lineY5 - g_lineLen;

const int g_irisRadius = 27;//虹膜直径约为180-250，取中间值后除以四
const int g_pupilRadius = 9;
//const int g_outerRadius = 55;
const int g_arrowLen = 13;
const int g_arrowSize = 3;
const int g_distanceBtArcAndArr = 5;

//默认的左眼追踪框中心坐标
const int g_defaultLeftCenterX = 180;
const int g_defaultLeftCenterY = 240;

//默认的右眼追踪框中心坐标
const int g_defaultRightCenterX = 460;
const int g_defaultRightCenterY = 240;


const unsigned char g_adCMD[4]          =    {0x0C, 0x00, 0x00, 0x00};   //发送给串口的获取红外测距命令
const unsigned char g_redAllOnCMD[4]    =    {0x07, 0x03, 0x00, 0x00};   //控制红外灯全亮
const unsigned char g_redLeftOnCMD[4]   =    {0x07, 0x01, 0x00, 0x00};   //控制左红外灯亮
const unsigned char g_redRightOnCMD[4]  =    {0x07, 0x02, 0x00, 0x00};   //控制右红外灯亮
const unsigned char g_redAllOffCMD[4]   =    {0x07, 0x00, 0x00, 0x00};   //控制红外灯全灭
const unsigned char g_irisCamOffCMD[4]  =    {0x05, 0x00, 0x00, 0x00};   //控制关闭虹膜摄像头
const unsigned char g_irisCamOnCMD[4]   =    {0x05, 0x01, 0x00, 0x00};   //控制打开虹膜摄像头
const unsigned char g_shutDownCMD[4]    =    {0xff, 0x00, 0x00, 0x00};   //控制关闭USB模块
const unsigned char g_startUpCMD[4]     =    {0xfe, 0x00, 0x00, 0x00};   //控制打开USB模块

extern std::mutex g_keepMain;
extern int g_isMainAlive;

/*****************************************************************************
*                         串口获取红外测距值的回调函数
*  函 数 名：distanceCB
*  功    能：串口获取红外测距值的回调函数
*  说    明：
*  参    数：data：输入参数，串口数据
*			size：输入参数，串口长度
*
*  返 回 值：NULL
*
*  创 建 人：lizhl
*  创建时间：2014-01-17
*  修 改 人：
*  修改时间：
*****************************************************************************/
void distanceCB(const unsigned char* data, const int size)
{
    //    qWarning("distance callback");
    int distanceValue = 0;
    int factor = 16;

    //liu-做修改  距离只有data[0] 有效
    if(NULL != data)
    {

        //liu -改
        distanceValue += data[0];

        Interaction::s_infraredDistanceValue = distanceValue;
        if(distanceValue<15)
        {
            static int count =0;
            count ++;
            if(count >100)
            {
                LOG_DEBUG("测距回调工作状态distanceValue%d",distanceValue);
                count = 0;
            }

        }

    }
}



/*****************************************************************************
*                         获取Interaction实例
*  函 数 名：GetInteractionInstance
*  功    能：获取CAPIBase实例
*  说    明：Interaction采用单件模式
*  参    数：NULL
*
*  返 回 值：!NULL: Interaction实例指针
*			NULL：获取失败
*  创 建 人：lizhl
*  创建时间：2014-02-14
*  修 改 人：
*  修改时间：
*****************************************************************************/
Interaction* Interaction::GetInteractionInstance()
{
    qDebug()<<"in GetInteractionInstance func"<<endl;
    // 双重检查，提高性能
    if (NULL == s_pSingletonInteraction)
    {
        std::lock_guard<std::mutex> lck (s_interactionInstanceMutex);

        if (NULL == s_pSingletonInteraction)
        {
            s_pSingletonInteraction = new Interaction();
        }
    }

    return s_pSingletonInteraction;
}


Interaction::Interaction()
{
    Exectime etm(__FUNCTION__);
    _resultFlag = false;//初始化时没有结果
    _sleepTime = GetSleepTime();
    connect(&_timerTestWake,SIGNAL(timeout()),this,SLOT(TimerForTest()));
   //_timerTestWake.start(120*1000);//@dj 60s 触发一次


    ap = new AudioPlay();

    if(NULL == ap)
    {
        LOG_ERROR("AudioPlay new failed");
    }

    //获取串口测距
    _serialDistance = SerialMuxDemux::GetInstance();


    if(_serialDistance == nullptr)
    {
        LOG_ERROR("获取串口单例失败");
    }

    //设置回调，在distanceCB中取距离值
    if( false == _serialDistance->AddCallback(0x0c, distanceCB))
    {
        LOG_ERROR("serial add callback failed");
    }
    else
    {
        LOG_DEBUG("serial add callback success");
    }


    int funResult = _serialDistance->Init(GetSerialPortName());//ttys1//ttyUSB0
    if(0 == funResult)
    {
        qWarning("serial init success");
    }
    else
    {
        LOG_ERROR("serial init failed");
        // TODO  打开串口失败就不要继续了
        return;
    }


    _irisCameraOnFlag = true;//初始化时虹膜摄像头处于开启状态

    //20150113
    ifClose = false;
    ikEvent = ikEventIns.GetIKEvent();
    connect(ikEvent, SIGNAL(sigCloseInfrared()),
            this, SLOT(CloseInfrared()));

    _getDistanceThread = std::thread(GetDistanceThread, (void*)this);


    _serialDistance->Send(g_redAllOnCMD, 4);
    _infraredLEDOnFlag = true;//初始化时红外灯处于开启状态


    gettimeofday(&_tvHasPerson,nullptr);
    gettimeofday(&_tvNoPerson,nullptr);

    qRegisterMetaType<InteractionResultType>("InteractionResultType");

    s_interactionCallBacks.clear(); //清除回调函数
    _hasPerson = HasPerson;         //初始化时认为有人
    _screenPressed = true;
    _updateTvStart = true;


    //20141218
    noticeCount = 1;

}

Interaction::~Interaction()
{
   // delete _timerForInfraredLED;

    _serialDistance->Send(g_redAllOffCMD, 4);
    _infraredLEDOnFlag = false;

    struct timespec ts100ms;
    ts100ms.tv_sec = 0;
    ts100ms.tv_nsec = 100*1000*1000;      //100ms
    nanosleep(&ts100ms, NULL);//防止串口丢失命令

    _serialDistance->Send(g_irisCamOffCMD, 4);
    _irisCameraOnFlag = false;

    if(ap)
    {
        delete ap;
        ap = NULL;
    }

    _getDistanceThread.join();

    std::lock_guard<std::mutex> lck (s_interactionInstanceMutex);
    delete s_pSingletonInteraction ;


}

// 关闭USB模块
void Interaction::CloseUsb()
{
    Exectime etm(__FUNCTION__);
    struct timespec ts100ms;
    _serialDistance->Send(g_shutDownCMD, 4);
    ts100ms.tv_sec = 1;
    ts100ms.tv_nsec = 100*1000*1000;      //100ms
    nanosleep(&ts100ms, NULL);//防止串口丢失命令
}

// 打开USB模块
void Interaction::StartUsb()
{
    Exectime etm(__FUNCTION__);
    struct timespec ts100ms;
    _serialDistance->Send(g_startUpCMD, 4);
    ts100ms.tv_sec = 2;
    ts100ms.tv_nsec = 100*1000*1000;      //100ms
    nanosleep(&ts100ms, NULL);//防止串口丢失命令
}


//@dj  李言、利刚让添加的超级指令，满足硬件逻辑需求
void Interaction::SendOffSuperCmd()
{
    unsigned char superCmd[4]={0xFF,0x00,0x00,0x00};
    _serialDistance->Send(superCmd, 4);

}

void Interaction::SendOnSuperCmd()
{
    unsigned char superCmd[4]={0xFE,0x00,0x00,0x00};
    _serialDistance->Send(superCmd, 4);
}

//20150113
void Interaction::CloseInfrared()
{
    ifClose = true;
}

//@for sdk 重载
void Interaction::PlayInteraction(IN InteractionResultType resultType,IN IKIrisDistFlag distEstimation,IN bool isUpdatePos)
{
    Exectime etm(__FUNCTION__);
    bool ifUpdatePosition = isUpdatePos;

    if(ResultUnknown == resultType && !ifUpdatePosition)
    {
        //如果没有识别或者注册结果，并且不需要更新位置提示，则返回
        return;
    }


    if((resultType == EnrollSuccess) || (resultType == EnrollFailed) || (resultType == IdenSuccess) || (resultType == IdenFailed))
    {
	    distEstimation = DistSuitable;
    }

    if((IdenKeepLastStatus != resultType) && (DistUnknown != distEstimation) && (NULL != ap))
    {

        if(ResultUnknown == resultType)
        {
            switch(distEstimation)
            {
            case DistTooFar:
                ap->PlaySound(CLOSER);
                break;
            case DistTooClose:
                ap->PlaySound(FARTHER);
                break;
            case DistSuitable:
                ap->PlaySound(KEEP);
                break;
            default:
                break;
            }
        }
        else
        {
            switch(resultType)
            {
            case EnrollSuccess:
                ap->PlaySound(ENROLLSUCC);
                break;
            case EnrollFailed:
                ap->PlaySound(ENROLLSTOP);
                break;
            case IdenSuccess:
                ap->PlaySound(IDENSUCC);
                break;
            case IdenFailed:
                ap->PlaySound(RETRY);
                break;
            default:
                break;
            }

        }
    }
    return ;
}
/*****************************************************************************
*                         清除结果
*  函 数 名：ClearResult
*  功    能：清除结果
*  说    明：
*  参    数：NULL
*
*  返 回 值：NULL
*  创 建 人：lizhl
*  创建时间：2014-01-27
*  修 改 人：
*  修改时间：
*****************************************************************************/
void Interaction::ClearResult()
{
    _resultFlag = false;
}


void Interaction::ResetDisplay()
{
//    _linePos = 0;
//    _variableRadius = g_outerRadius;
}


//@dj 安全期间，从子线程中剥离出来，在主线成中实现，
//后续考虑动态修改配置生效
int Interaction::GetSleepTime()
{
    Exectime etm(__FUNCTION__);
    int sleepTime = 5;
    IKXml::IKXmlDocument doc;
    if(doc.LoadFromFile("ikembx00.xml"))
    {
        //读取休眠时间
        if(!doc.GetElementValue("/IKEMBX00/configures/identify/belltime").empty())
        {
            sleepTime = QString(doc.GetElementValue("/IKEMBX00/configures/systemsleep").c_str()).toInt();
            if(sleepTime < 5)
            {
                sleepTime = 5;
            }
            else if(sleepTime > 120)
            {
                sleepTime = 120;
            }
        }
    }
    else
    {
        std::cout<<"配置文件 ikembx00.xml 未打开！"<<std::endl;
        LOG_ERROR("获取休眠时间时，打开配置文件错误");
    }

    return sleepTime;
}

/*****************************************************************************
*                         获取红外测距值线程
*  函 数 名：GetDistanceThread
*  功    能：获取红外测距值
*  说    明：
*  参    数：pParam：输入参数
*
*  返 回 值：NULL
*  创 建 人：lizhl
*  创建时间：2014-02-12
*  修 改 人：
*  修改时间：
*****************************************************************************/
void* Interaction::GetDistanceThread(void* pParam)
{
    Exectime etm(__FUNCTION__);
    Interaction *pInteractionParam = static_cast <Interaction*> (pParam);
    struct timespec ts100ms;
    ts100ms.tv_sec = 0;
    ts100ms.tv_nsec = 100*1000*1000;     //++++/100ms

    int infraredTime;
    int cameraTime;

    int sleepTime = pInteractionParam->_sleepTime ;
    //sleepTime = 3;
    std::cout << "省电模式时间:" << sleepTime << "min" << std::endl;


    while(!pInteractionParam->ifClose)
    {
        //发送命令给串口，在回调中取距离值

        nanosleep(&ts100ms, NULL);//每100ms发送一次
        static int i=0;
        i++;

        // ui线程卡死后，系统重启
        if(i>300)
        {
            i =0;
            LOG_DEBUG("GetDistanceThread is run");
            lock_guard<mutex> lock(g_keepMain);
            static int count = 0;
            if(g_isMainAlive == 1)
            {
                g_isMainAlive = 0;
                count = 0;

            }
            else if(g_isMainAlive == 0)
            {
                count ++;
                LOG_ERROR("dev is hung%d",count);
            }

            if(count >=3)
            {
                LOG_ERROR("dev is hung,system need reboot");
                 system("reboot");
            }

        }

        //判断距离值
          //      if(NoPersonDistanceValue > s_infraredDistanceValue)

        {
            //@FOR pc
            //s_infraredDistanceValue  = 10000;
            std::lock_guard<std::mutex> lck(pInteractionParam->_setHasPersonMutex);
            if(LEDOnDistanceValue > s_infraredDistanceValue || pInteractionParam->_screenPressed)
            {
                pInteractionParam->_hasPerson = HasPerson;
                pInteractionParam->_screenPressed = false;
            }
            else if(s_infraredDistanceValue >= LEDOnDistanceValue)
            {

                pInteractionParam->_hasPerson = WaitPerson;
            }
            else
            {
                pInteractionParam->_hasPerson = NoPerson;
            }
        }


        if(pInteractionParam->_hasPerson == HasPerson)
        {
            //有人
            if(false == pInteractionParam->_infraredLEDOnFlag)
            {
                qDebug()<< "有人 -》打开红外灯"<<endl;
                //红外灯处于关闭状态，则打开红外灯
                pInteractionParam->_serialDistance->Send(g_redAllOnCMD, 4);//20140320为调试程序崩溃，把本cpp中的timer去掉
                pInteractionParam->_infraredLEDOnFlag = true;

                std::cout << "Has person, send cmd to power on infrared LED" << std::endl;
                LOG_DEBUG("Has person, send cmd to power on infrared LED");
                nanosleep(&ts100ms, NULL);//防止串口丢失命令

                //为了防止串口丢命令，再发一次
                LOG_DEBUG("Has person, send cmd to power on infrared LED again");
                pInteractionParam->_serialDistance->Send(g_redAllOnCMD, 4);
                //supercmd
//                unsigned char superCmd[4]={0xff,0x00,0x00,0x00};
//                pInteractionParam->_serialDistance->Send(superCmd, 4);

                nanosleep(&ts100ms, NULL);//防止串口丢失命令

                if(false == pInteractionParam->_irisCameraOnFlag)
                {
                    //打开虹膜摄像头
                    //虹膜摄像头通电后，李言需要1s钟时间加载虹膜摄像头程序，在此时间内不要打开摄像头采图
                    pInteractionParam->_serialDistance->Send(g_irisCamOnCMD, 4);
                    pInteractionParam->_irisCameraOnFlag = true;

                    std::cout << "Has person, send cmd to power on iris camera" << std::endl;
                    LOG_DEBUG("Has person, send cmd to power on iris camera");
                    nanosleep(&ts100ms, NULL);//防止串口丢失命令

                    //为了防止串口丢命令，再发一次
                    pInteractionParam->_serialDistance->Send(g_irisCamOnCMD, 4);
                    nanosleep(&ts100ms, NULL);//防止串口丢失命令

                }

                for( std::map<std::string, InteractionCallBack>::iterator iter = s_interactionCallBacks.begin();
                     iter != s_interactionCallBacks.end(); ++ iter )
                {
                    (iter->second)(HasPerson);
                }
            }

            {
                std::lock_guard<std::mutex> lck(pInteractionParam->_setHasPersonMutex);
                gettimeofday(&pInteractionParam->_tvHasPerson, nullptr);
            }

        }
        else
        {
            gettimeofday(&pInteractionParam->_tvNoPerson, nullptr);
            {
                std::lock_guard<std::mutex> lck(pInteractionParam->_setHasPersonMutex);
                infraredTime = pInteractionParam->_tvNoPerson.tv_sec - pInteractionParam->_tvHasPerson.tv_sec;
            }


            //@ for test 原本 30 非8
            if(infraredTime > 10 )//10//30
            {
                //如果当前无人时间距离上次有人时间超过30秒，则关闭红外灯
                if(true == pInteractionParam->_infraredLEDOnFlag)
                {
                    {
                        std::lock_guard<std::mutex> lck(pInteractionParam->_setHasPersonMutex);
                        pInteractionParam->_hasPerson = WaitPerson;//wait 20141204
                    }

                    //关闭红外灯
                    pInteractionParam->_serialDistance->Send(g_redAllOffCMD, 4);//20140320为调试程序崩溃，把本cpp中的timer去掉
                    pInteractionParam->_infraredLEDOnFlag = false;

                    std::cout << "No person, send cmd to power off infrared LED" << std::endl;
                    LOG_DEBUG("No person, send cmd to power off infrared LED");
                    nanosleep(&ts100ms, NULL);//防止串口丢失命令

                    //为了防止串口丢命令，再发一次
                    pInteractionParam->_serialDistance->Send(g_redAllOffCMD, 4);
                    nanosleep(&ts100ms, NULL);//防止串口丢失命令

                    //回调，通知应用程序，准备关闭红外灯 20141204 lizhl
                    for( std::map<std::string, InteractionCallBack>::iterator iter = s_interactionCallBacks.begin();
                         iter != s_interactionCallBacks.end(); ++ iter )
                    {
                        std::cout << "No person, 进入回调通知识别模块" << std::endl;
                        LOG_DEBUG("No person, 进入回调通知识别模块");
                        (iter->second)(WaitPerson);
                    }

                }

                //如果无人时间超过30min，关闭虹膜摄像头
                {
                    std::lock_guard<std::mutex> lck(pInteractionParam->_setHasPersonMutex);
                    cameraTime = pInteractionParam->_tvNoPerson.tv_sec - pInteractionParam->_tvHasPerson.tv_sec;
                }

                //sleepTime = 1;

                if(cameraTime > sleepTime*60)//60
                {
                    LOG_DEBUG("cameraTime=%d;sleepTime=%d",cameraTime,sleepTime);

                    if(true == pInteractionParam->_irisCameraOnFlag)
                    {


                        //回调，通知应用程序，准备关闭摄像头

                        for( std::map<std::string, InteractionCallBack>::iterator iter = s_interactionCallBacks.begin();
                             iter != s_interactionCallBacks.end(); ++ iter )
                        {

                            (iter->second)(NoPerson);
                        }

                    }

                }

            }

        }
    }

    pInteractionParam->_serialDistance->Send(g_redAllOffCMD, 4);
    nanosleep(&ts100ms, NULL);
    return (void*)0;
}


/*****************************************************************************
*                         处理红外灯点亮状态定时器事件
*  函 数 名：TimerForInfraredLEDTimeOut
*  功    能：处理红外灯点亮状态定时器事件
*  说    明：
*  参    数：NULL
*
*  返 回 值：NULL
*  创 建 人：lizhl
*  创建时间：2014-02-12
*  修 改 人：
*  修改时间：
*****************************************************************************/
void Interaction::TimerForInfraredLEDTimeOut()
{
    Exectime etm(__FUNCTION__);
    //定时到，关闭红外灯
    if(true == _infraredLEDOnFlag)
    {
        //_serialDistance->Send(g_redAllOffCMD, 4);//20140320为调试程序崩溃，把本cpp中的timer去掉
        _infraredLEDOnFlag = false;

        for( std::map<std::string, InteractionCallBack>::iterator iter = s_interactionCallBacks.begin();
             iter != s_interactionCallBacks.end(); ++ iter )
        {
            (iter->second)(NoPerson);
        }

    }

}

//20161121测试
void Interaction::TimerForTest()
{
    LOG_INFO("in TimerForTest for wake SDK");

    ScreenPressResponse();
    LOG_INFO("模拟有人完毕");


}

void Interaction::ScreenPressResponse()
{
    Exectime etm(__FUNCTION__);
    std::lock_guard<std::mutex> lck(_setHasPersonMutex);
    gettimeofday(&_tvHasPerson, nullptr);
    _screenPressed = true;
}

/*****************************************************************************
*                         注册回调函数
*  函 数 名：RegisterCallBack
*  功    能：注册回调函数
*  说    明：
*  参    数：NULL
*
*  返 回 值：true：成功；
*           false：失败
*  创 建 人：lizhl
*  创建时间：2014-02-14
*  修 改 人：
*  修改时间：
*****************************************************************************/
bool Interaction::RegisterCallBack(std::string strName, InteractionCallBack callBack)
{
    Exectime etm(__FUNCTION__);
    s_interactionCallBacks[strName] = callBack;
    return s_interactionCallBacks.count(strName) > 0;
}

/*****************************************************************************
*                         删除回调函数
*  函 数 名：DeleteCallBack
*  功    能：删除回调函数
*  说    明：
*  参    数：NULL
*
*  返 回 值：NULL
*  创 建 人：lizhl
*  创建时间：2014-02-14
*  修 改 人：
*  修改时间：
*****************************************************************************/
void Interaction::DeleteCallBack(std::string strName)
{
    Exectime etm(__FUNCTION__);
    if(s_interactionCallBacks.count(strName) > 0)
    {
        s_interactionCallBacks.erase(strName);
    }
}

