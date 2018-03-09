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
#include <math.h>   //为了在distanceCB中使用pow

#include <QDir>
//#include <QDebug>
#include "../AlgApi/SRC/AlgApi.h"

//播放语音的MediaObject
Phonon::MediaObject* Interaction:: s_sound = nullptr;


//Phonon::MediaObject* s_sound = nullptr;

struct timeval Interaction::s_tvSoundStart;//发声开始时间
bool Interaction::s_updateTvStart = true;//是否更新发声开始时间，初始化为true

//全局变量，红外测距值
int g_infraredDistanceValue = 0;
int Interaction::s_infraredDistanceValue = 0; //红外测距值

std::mutex Interaction::s_interactionInstanceMutex;
Interaction* Interaction::s_pSingletonInteraction = NULL;
std::map<std::string, InteractionCallBack> Interaction::s_interactionCallBacks;//回调函数

const unsigned char g_adCMD[4] =            {0x0C, 0x00, 0x00, 0x00};   //发送给串口的获取红外测距命令
const unsigned char g_redAllOnCMD[4] =      {0x07, 0x03, 0x00, 0x00};   //控制红外灯全亮
const unsigned char g_redLeftOnCMD[4] =     {0x07, 0x01, 0x00, 0x00};   //控制左红外灯亮
const unsigned char g_redRightOnCMD[4] =    {0x07, 0x02, 0x00, 0x00};   //控制右红外灯亮
const unsigned char g_redAllOffCMD[4] =     {0x07, 0x00, 0x00, 0x00};   //控制红外灯全灭

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

    if(NULL != data)
    {
        for(int num = 0; num < size; num++)
        {
            //TO DO
            distanceValue += data[num] * pow(factor, num);
        }
        g_infraredDistanceValue = distanceValue;
        Interaction::s_infraredDistanceValue = distanceValue;
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
    _resultFlag = false;//初始化时没有结果
    _infraredLEDOnFlag = false;//初始化时红外灯处于关闭状态
    s_sound = Phonon::createPlayer(Phonon::MusicCategory,
                                  Phonon::MediaSource("./sounds/chs/004thanks.wav"));
//    s_sound = new Phonon::MediaObject();
//    s_sound = new Phonon::MediaObject(this);
//    s_sound->setCurrentSource(Phonon::MediaSource("./sounds/chs/004thanks.wav"));
    //获取串口测距
    _serialDistance = SerialMuxDemux::GetInstance();
    //设置回调，在distanceCB中取距离值
    if( false == _serialDistance->AddCallback(0x0c, distanceCB))
    {
//        qWarning("serial add callback failed");
    }
//    qWarning("serial add callback success");

    int funResult = _serialDistance->Init("ttyUSB0");//ttys1//ttyUSB0
    if(0 == funResult)
    {
        qWarning("serial init success");
    }
    else
    {
        qWarning("serial init failed");
    }

    _getDistanceThread = std::thread(GetDistanceThread, (void*)this);
    _timerForInfraredLED = new QTimer();
    //关联定时器超时信号槽函数
    connect(_timerForInfraredLED,SIGNAL(timeout()),this,SLOT(TimerForInfraredLEDTimeOut()));
    _timerForInfraredLED->start(30*1000);//此处可以不开启定时器

    s_interactionCallBacks.clear();//清除回调函数
}

Interaction::~Interaction()
{
    _getDistanceThread.join();
}

/*****************************************************************************
*                         执行人机交互，包括语音及界面绘图
*  函 数 名：PlayInteraction
*  功    能：执行人机交互，包括语音及界面绘图
*  说    明：如果还没有结果，一定要将传入的resultType设置为ResultUnknown
*  参    数：resultType：输入参数，注册、识别的结果类型
*           lrIrisClearFlag：输入参数，左右眼是否清晰标志
*           irisPositionFlag：输入参数，位置信息标志
*           paintImg：输出参数，待绘制的图像，其格式为彩色，否则报错；
*           如果不是，使用converToFormat函数进行转换
*           建议使用QImage::Format_ARGB32_Premultiplied
*
*
*  返 回 值：NULL
*  创 建 人：lizhl
*  创建时间：2014-01-27
*  修 改 人：
*  修改时间：
*****************************************************************************/
void Interaction::PlayInteraction(IN InteractionResultType resultType, IN LRIrisClearFlag lrIrisClearFlag, IN IrisPositionFlag irisPositionFlag, OUT QImage &paintImg)
{
//    //如果没有结果，处理人机交互过程中的语音及提示
//    if((ResultUnknown == resultType) && (false == _resultFlag))
//    {
//        DisplayClearBlur(paintImg, lrIrisClearFlag);   //显示图像是否清晰提示框
//        PlayInteractionSound(resultType, irisPositionFlag);        //播放人机交互语音提示，靠近、靠远等
//    }
//    else
//    {
//        //如果有结果，处理相应的结果
//        ProcessResult(resultType, paintImg);
//        PlayInteractionSound(resultType, irisPositionFlag);        //播放人机交互语音提示，靠近、靠远等
//        _resultFlag = true;
//    }

    PlayInteractionSound(resultType, irisPositionFlag);        //播放人机交互语音提示，靠近、靠远等
    switch(resultType)
    {
    case ResultUnknown:
    case EnrollPerformig:
    case IdenPerforming:
        DisplayClearBlur(paintImg, lrIrisClearFlag);   //显示图像是否清晰提示框
        break;
    case EnrollSuccess:
    case EnrollFailed:
    case IdenSuccess:
    case IdenFailed:
    case IdenKeepLastStatus:
        ProcessResult(resultType, paintImg);
        break;
    default:
        break;
    }


    return;
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

/*****************************************************************************
*                         显示图像是否清晰提示框
*  函 数 名：DisplayClearBlur
*  功    能：显示图像是否清晰提示框
*  说    明：
*  参    数：paintImg：输出参数，待绘制的图像，其格式为彩色，否则报错；
*           如果不是，使用converToFormat函数进行转换
*           建议使用QImage::Format_ARGB32_Premultiplied
*           lrIrisClearFlag：输入参数，左右眼是否清晰标志
*
*  返 回 值：NULL
*  创 建 人：lizhl
*  创建时间：2014-01-08
*  修 改 人：
*  修改时间：
*****************************************************************************/
void Interaction::DisplayClearBlur(OUT QImage &paintImg, IN LRIrisClearFlag lrIrisClearFlag)
{
//    QImage desImage = QImage(g_constIrisImgWidth,g_constIrisImgHeight,QImage::Format_ARGB32_Premultiplied); //RGB32
//    desImage = qImg.convertToFormat(QImage::Format_ARGB32_Premultiplied);

    QImage imgCover(g_constIrisImgWidth, g_constIrisImgHeight, QImage::Format_ARGB32_Premultiplied);
//    imgCover.fill(0);//QColor(100,100,100,10)
//    imgCover.fill(Qt::transparent);
//    imgCover.fill(qRgba(255, 255, 255, 0));

    //坐标申明，x轴对应图像宽度所在，y轴对应图像高度所在
    //左眼图像起始坐标
    int xLeft = 24;
    int yLeft = 112;

    //右眼图像起始坐标
    int xRight = 1384;
    int yRight = 112;

    //叉的坐标
//    int crossSize = 200;//画叉的尺寸
    //左叉的起始坐标
//    int crossXLeftOrigin = xLeft + (g_constIKImgWidth-crossSize)/2;
//    int crossXLeftEnd = xLeft + g_constIKImgWidth - (g_constIKImgWidth-crossSize)/2;
//    int crossYLeftOrigin = yLeft + (g_constIKImgHeight-crossSize)/2;
//    int crossYLeftEnd = yLeft + + g_constIKImgHeight - (g_constIKImgHeight-crossSize)/2;

    //右叉的起止坐标
//    int crossXRightOrigin = xRight + (g_constIKImgWidth-crossSize)/2;
//    int crossXRightEnd = xRight + g_constIKImgWidth - (g_constIKImgWidth-crossSize)/2;
//    int crossYRightOrigin = yRight + (g_constIKImgHeight-crossSize)/2;
//    int crossYRightEnd = yRight + + g_constIKImgHeight - (g_constIKImgHeight-crossSize)/2;


    //圆的相关信息
    //圆的直径
//    int diameter = 200;

    //左圆的起始坐标
//    int roundXLeft = xLeft + (g_constIKImgWidth-diameter)/2;
//    int roundYLeft = yLeft + (g_constIKImgHeight-diameter)/2;

    //右圆的起始坐标
//    int roundXRight = xRight + (g_constIKImgWidth-diameter)/2;
//    int roundYRight = yRight + (g_constIKImgHeight-diameter)/2;

    QPainter painterRectangle(&paintImg);
//    QPainter painterRectangle(&imgCover);

    painterRectangle.setRenderHint(QPainter::Antialiasing,true);
    if(LAndRImgBlur == lrIrisClearFlag)
    {
        painterRectangle.setPen(QPen(Qt::red, 5, Qt::SolidLine, Qt::RoundCap));

//        painterRectangle.drawLine(244,252,444,452);
//        painterRectangle.drawLine(244,452,444,252);


//        painterResult.drawRect(xLeft,yLeft,g_constIKImgWidth,g_constIKImgHeight);//24,112
//        painterResult.drawLine(244,252,444,452);
//        painterResult.drawLine(244,452,444,252);
//        painterRectangle.drawLine(crossXLeftOrigin,crossYLeftOrigin,crossXLeftEnd,crossYLeftEnd);
//        painterRectangle.drawLine(crossXLeftOrigin,crossYLeftEnd,crossXLeftEnd,crossYLeftOrigin);


//        painterRectangle.drawLine(crossXRightOrigin,crossYRightOrigin,crossXRightEnd,crossYRightEnd);
//        painterRectangle.drawLine(crossXRightOrigin,crossYRightEnd,crossXRightEnd,crossYRightOrigin);

//        painterRectangle.drawEllipse(roundXLeft,roundYLeft,diameter,diameter);
//        painterRectangle.drawEllipse(roundXRight,roundYRight,diameter,diameter);

    }
    else
    {
        painterRectangle.setPen(QPen(Qt::green, 5, Qt::SolidLine, Qt::RoundCap));
    }

//    painterRectangle.drawRect(24,112,640,480);//24,112
//    painterRectangle.drawRect(1384,112,640,480);//1384,112
    painterRectangle.drawRect(xLeft,yLeft,g_constIKImgWidth,g_constIKImgHeight);//24,112
    painterRectangle.drawRect(xRight,yRight,g_constIKImgWidth,g_constIKImgHeight);//1384,112

//    QPainter finalPainter(&paintImg);
//    finalPainter.setCompositionMode(QPainter::CompositionMode_Xor);//CompositionMode_SourceOver
//    finalPainter.drawImage(0, 0, imgCover);
}

/*****************************************************************************
*                         播放人机交互语音提示，靠近、靠远等
*  函 数 名：PlayInteractionSound
*  功    能：播放人机交互语音提示，靠近、靠远等
*  说    明：
*  参    数：resultType：输入参数，注册、识别的结果类型
*           irisPositionFlag：输入参数，位置信息标志
*
*  返 回 值：NULL
*  创 建 人：lizhl
*  创建时间：2014-01-08
*  修 改 人：
*  修改时间：
*****************************************************************************/
void Interaction::PlayInteractionSound(IN InteractionResultType resultType, IN IrisPositionFlag irisPositionFlag)
{
//    qDebug()<<QDir::currentPath();

    struct timeval tvSoundEnd;
    int durationTime = 0;//声音间隔

    if(s_updateTvStart)
    {
        //更新声音播放开始时间
        gettimeofday(&s_tvSoundStart,NULL);
        s_updateTvStart = false;
    }

    //获取当前时间
    gettimeofday(&tvSoundEnd,NULL);
    durationTime = tvSoundEnd.tv_sec - s_tvSoundStart.tv_sec;

    if( (durationTime < g_constSoundInterval) && (ResultUnknown == resultType) )
    {
        //当前时间与声音播放开始时间间隔太小，并且没有注册或识别结果，则不播放声音
        return;
    }
    else
    {
        //达到间隔时间，播放声音，将更新声音播放时间标志设为true
        s_updateTvStart = true;
    }

    if(ResultUnknown == resultType)
    {
        switch(irisPositionFlag)
        {
        case Far:
            s_sound->setCurrentSource(Phonon::MediaSource("./sounds/chs/010closer.wav"));
            break;

        case Near:
            s_sound->setCurrentSource(Phonon::MediaSource("./sounds/chs/018farther.wav"));
            break;

        case Left:
            s_sound->setCurrentSource(Phonon::MediaSource("./sounds/chs/104right.wav"));
            break;

        case Right:
            s_sound->setCurrentSource(Phonon::MediaSource("./sounds/chs/105left.wav"));
            break;

        case Up:
                s_sound->setCurrentSource(Phonon::MediaSource("./sounds/chs/102down.wav"));
            break;

        case Down:
                s_sound->setCurrentSource(Phonon::MediaSource("./sounds/chs/103up.wav"));
            break;

        case OK:
            s_sound->setCurrentSource(Phonon::MediaSource("./sounds/chs/100keep.wav"));
            break;

        default:
            break;
        }
    }
    else
    {
//        if((s_sound) && (false == _resultFlag))
        if((s_sound) && (IdenKeepLastStatus != resultType))
        {
            s_sound->clear();
        }

        switch(resultType)
        {
        case EnrollSuccess:
            s_sound->setCurrentSource(Phonon::MediaSource("./sounds/chs/003enrollsucc.wav"));
            break;
        case EnrollFailed:
            s_sound->setCurrentSource(Phonon::MediaSource("./sounds/chs/006enrollstop.wav"));
            break;
        case IdenSuccess:
            s_sound->setCurrentSource(Phonon::MediaSource("./sounds/chs/002idensucc.wav"));
            break;
        case IdenFailed:
            s_sound->setCurrentSource(Phonon::MediaSource("./sounds/chs/005retry.wav"));
            break;
        default:
            break;
        }

    }

    if((s_sound) && (false == _resultFlag) && (IdenKeepLastStatus != resultType))
    {
        s_sound->play();
    }

}

/*****************************************************************************
*                         处理注册、识别结果的输出
*  函 数 名：ProcessResult
*  功    能：处理注册、识别结果的输出
*  说    明：
*  参    数：resultType：输入参数，注册、识别的结果类型
*           paintImg：输出参数，待绘制的图像，其格式为彩色，否则报错；
*           如果不是，使用converToFormat函数进行转换
*           建议使用QImage::Format_ARGB32_Premultiplied
*
*  返 回 值：NULL
*  创 建 人：lizhl
*  创建时间：2014-01-08
*  修 改 人：
*  修改时间：
*****************************************************************************/
void Interaction::ProcessResult(IN InteractionResultType resultType, OUT QImage &paintImg)
{
    static InteractionResultType s_lastInteractionResultType = ResultUnknown;

    if(IdenKeepLastStatus == resultType)
    {
        resultType = s_lastInteractionResultType;
    }
    s_lastInteractionResultType = resultType;
//    QImage imgCover(g_constIrisImgWidth, g_constIrisImgHeight, QImage::Format_ARGB32_Premultiplied);
    //坐标申明，x轴对应图像宽度所在，y轴对应图像高度所在
    //左眼图像起始坐标
    int xLeft = 24;
    int yLeft = 112;

    //右眼图像起始坐标
    int xRight = 1384;
    int yRight = 112;

    //叉的坐标
    int crossSize = 200;//画叉的尺寸
    //左叉的起始坐标
    int crossXLeftOrigin = xLeft + (g_constIKImgWidth-crossSize)/2;
    int crossXLeftEnd = xLeft + g_constIKImgWidth - (g_constIKImgWidth-crossSize)/2;
    int crossYLeftOrigin = yLeft + (g_constIKImgHeight-crossSize)/2;
    int crossYLeftEnd = yLeft + + g_constIKImgHeight - (g_constIKImgHeight-crossSize)/2;

    //右叉的起止坐标
    int crossXRightOrigin = xRight + (g_constIKImgWidth-crossSize)/2;
    int crossXRightEnd = xRight + g_constIKImgWidth - (g_constIKImgWidth-crossSize)/2;
    int crossYRightOrigin = yRight + (g_constIKImgHeight-crossSize)/2;
    int crossYRightEnd = yRight + + g_constIKImgHeight - (g_constIKImgHeight-crossSize)/2;

    //圆的相关信息
    //圆的直径
    int diameter = 200;

    //左圆的起始坐标
    int roundXLeft = xLeft + (g_constIKImgWidth-diameter)/2;
    int roundYLeft = yLeft + (g_constIKImgHeight-diameter)/2;

    //右圆的起始坐标
    int roundXRight = xRight + (g_constIKImgWidth-diameter)/2;
    int roundYRight = yRight + (g_constIKImgHeight-diameter)/2;

    //首先清除之前的语音文件
//    if(s_sound)
//    {
//        s_sound->clear();
//    }

    QPainter painterResult(&paintImg);

    painterResult.setRenderHint(QPainter::Antialiasing,true);

    switch(resultType)
    {
    case EnrollSuccess:
//        s_sound = Phonon::createPlayer(Phonon::MusicCategory,
//                                       Phonon::MediaSource("./sounds/chs/003enrollsucc.wav"));
//        s_sound->setCurrentSource(Phonon::MediaSource("./sounds/chs/003enrollsucc.wav"));
        break;

    case EnrollFailed:
//        s_sound = Phonon::createPlayer(Phonon::MusicCategory,
//                                       Phonon::MediaSource("./sounds/chs/006enrollstop.wav"));
//        s_sound->setCurrentSource(Phonon::MediaSource("./sounds/chs/006enrollstop.wav"));
        break;

    case IdenSuccess:
//        s_sound = Phonon::createPlayer(Phonon::MusicCategory,
//                                       Phonon::MediaSource("./sounds/chs/002idensucc.wav"));
//        s_sound->setCurrentSource(Phonon::MediaSource("./sounds/chs/002idensucc.wav"));

        painterResult.setPen(QPen(Qt::green, 5, Qt::SolidLine, Qt::RoundCap));
        painterResult.drawRect(xLeft,yLeft,g_constIKImgWidth,g_constIKImgHeight);//24,112
        painterResult.drawEllipse(roundXLeft,roundYLeft,diameter,diameter);

//        painterResult.drawLine(crossXRightOrigin,crossYRightEnd,crossXRightEnd,crossYRightOrigin);
        painterResult.drawRect(xRight,yRight,g_constIKImgWidth,g_constIKImgHeight);//1384,112
        painterResult.drawEllipse(roundXRight,roundYRight,diameter,diameter);
        break;

    case IdenFailed:
//        s_sound = Phonon::createPlayer(Phonon::MusicCategory,
//                                       Phonon::MediaSource("./sounds/chs/005retry.wav"));
//        s_sound->setCurrentSource(Phonon::MediaSource("./sounds/chs/005retry.wav"));

        painterResult.setPen(QPen(Qt::red, 5, Qt::SolidLine, Qt::RoundCap));
        painterResult.drawRect(xLeft,yLeft,g_constIKImgWidth,g_constIKImgHeight);//24,112
//        painterResult.drawLine(244,252,444,452);
//        painterResult.drawLine(244,452,444,252);
        painterResult.drawLine(crossXLeftOrigin,crossYLeftOrigin,crossXLeftEnd,crossYLeftEnd);
        painterResult.drawLine(crossXLeftOrigin,crossYLeftEnd,crossXLeftEnd,crossYLeftOrigin);

        painterResult.drawRect(xRight,yRight,g_constIKImgWidth,g_constIKImgHeight);//1384,112
        painterResult.drawLine(crossXRightOrigin,crossYRightOrigin,crossXRightEnd,crossYRightEnd);
        painterResult.drawLine(crossXRightOrigin,crossYRightEnd,crossXRightEnd,crossYRightOrigin);
        break;

    default:
        break;
    }

//    if(s_sound)
//    {
//        s_sound->play();
//    }
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
    Interaction *pCGDlg = static_cast <Interaction*> (pParam);
    while(1)
    {
        //发送命令给串口，在回调中取距离值
//        qWarning("send ad CMD");
        pCGDlg->_serialDistance->Send(g_adCMD, 4);

        usleep(100000);//每100ms发送一次
        //判断距离值
//        if(NoPersonDistanceValue > g_infraredDistanceValue)
        if(NoPersonDistanceValue > s_infraredDistanceValue)
        {
            //有人
            if(false == pCGDlg->_infraredLEDOnFlag)
            {
                //红外灯处于关闭状态，则打开红外灯
                pCGDlg->_serialDistance->Send(g_redAllOnCMD, 4);
                pCGDlg->_infraredLEDOnFlag = true;

            }

            //关闭定时器，并重新启动定时器计时
            pCGDlg->_timerForInfraredLED->stop();
            pCGDlg->_timerForInfraredLED->start(30*1000);//每次开启红外灯后，维持30秒
        }
    }
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
    //定时到，关闭红外灯
    if(true == _infraredLEDOnFlag)
    {
        _serialDistance->Send(g_redAllOffCMD, 4);
        _infraredLEDOnFlag = false;

        for( std::map<std::string, InteractionCallBack>::iterator iter = s_interactionCallBacks.begin();
             iter != s_interactionCallBacks.end(); ++ iter )
        {
            (iter->second)(NoPerson);
        }

    }

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
    if(s_interactionCallBacks.count(strName) > 0)
    {
        s_interactionCallBacks.erase(strName);
    }
}
