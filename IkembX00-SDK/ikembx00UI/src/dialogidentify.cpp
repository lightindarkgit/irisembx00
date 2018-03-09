/*****************************************************************************
** 文 件 名：Identity.cpp
** 主 要 类：Identity
**
** Copyright (c) 中科虹霸有限公司
**
** 创 建 人：刘中昌
** 创建时间：20013-10-13
**
** 修 改 人：
** 修改时间：
** 描  述:   识别对话框
** 主要模块说明: 识别界面的显示
**
** 版  本:   1.0.0
** 备  注:
**
*****************************************************************************/
#include <unistd.h>
#include <iostream>
#include "dialogidentify.h"
#include "ui_dialogidentify.h"
#include "time.h"
#include "QDateTime"
#include "QTime"
#include <QtCore>
#include <QDebug>
#include "QThread"
#include <QNetworkConfigurationManager>
//#include <QNetworkInterface>
#include <sys/time.h>
#include <QMessageBox>
#include "../ikIOStream/interface.h"
#include "../common.h"
//#include "../XML/xml/IKXmlApi.h"
#include "../IrisAppLib/IrisManager/uuidControl.h"
#include "sysnetinterface.h"
#include <QList>
#include "dialogselectlogin.h"
#include "dialoglogin.h"
#include "commononqt.h"
#include "ikxmlhelper.h"
#include <iostream>
#include "iksocket.h"
#include "../Common/Logger.h"
#include "drawtracgif.h"   //add for sdk
#include "Global.h"
#include <QSound>
#include "../Common/Exectime.h"
#include"ikmessagebox.h"
#include "buffering.h"
#include "ikconfig.h"



//信号槽链接类
ReDrawEvent drawEvent;
IdentEvent identEvent;


IKResultFlag ad; //@sdk
const long RecogMax = 30000;//识别记录超过3万进行表备份


//静态声明
Interaction *DialogIdentify::_identInteraction;
bool DialogIdentify::_defaultIdentMode= true;
bool DialogIdentify::_cardoririsMode = false; //默认为false,保持原来的识别模式 fjf
bool DialogIdentify::g_isQuitIdent ;
IfHasPerson DialogIdentify::_hasPerson = HasPerson;
IfHasPerson DialogIdentify::_lastPersonStatus;//20141205 lizhl
bool DialogIdentify::g_isStopIdent = true;
bool DialogIdentify::_isLoginIdent = false;

unsigned char faceImgBuf[IK_DISPLAY_IMG_SIZE];

//For LiYan's test
volatile bool saveImgFlag = false;

//CAlgApi * DialogIdentify::_identApi;
//@@@CCameraApi* DialogIdentify::_faceCamer;

QMovie *DialogIdentify::_loadMovie;
QLabel *DialogIdentify::_loadLabel;
QLabel *DialogIdentify::_labMask;
int isCarded = 0 ; /// 0:未刷卡 ；1：已刷卡

int G_BellTime = -1;
IKEnrIdenStatus g_enrIdenStatus;

bool g_isIdentThreadEnd = true;  //识别线程是否退出标志
bool g_isSdkWorking = false;            //检测sdk是否有输出
int  G_IsLoadingCodeList = 1; //1 正在加载特征 0 提示特征加载完成 2不再提示特征完成
std::mutex g_sdkCallBackMutex;

std::mutex g_keepMain;
int g_isMainAlive;

//guowen添加 2018-01-22
volatile bool g_isSDKReInited = true;

//@dj for sdk-------------------------------------------------------------------
void IrisStatusInfoCallback(IKEnrIdenStatus* enrIdenStatus)
{
    Exectime etm(__FUNCTION__);

    if(nullptr == enrIdenStatus)
    {
        LOG_ERROR("enrIdenStatus is null");
        return ;
    }

    {
        lock_guard<mutex> lock(g_sdkCallBackMutex);
        memmove(&g_enrIdenStatus,enrIdenStatus,sizeof(IKEnrIdenStatus));
    }

    //    if(g_isLogInSDKCallback)
    //    {
    //        LOG_INFO("SDK回调函数输出 distEstimation = %d;Lx = %d, Ly = %d, Rx = %d, Ry = %d",
    //                 enrIdenStatus->distEstimation,
    //                 enrIdenStatus->irisPos.leftIrisPos.xPixel,
    //                 enrIdenStatus->irisPos.leftIrisPos.yPixel,
    //                 enrIdenStatus->irisPos.rightIrisPos.xPixel,
    //                 enrIdenStatus->irisPos.rightIrisPos.yPixel
    //                 );
    //    }

    g_isSdkWorking = true;
    drawEvent.OnDrawIdent();

}
//-------------------------------------------------------------------------------

DialogIdentify::DialogIdentify(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::DialogIdentify),
    _sm(SerialMuxDemux::GetInstance()),
    _irisManager(SingleControl<IrisManager>::CreateInstance()),
    _outMode(WiganOutMode::OutCard),
    //_isReloadiris(false),
    _weekDay(weekDay)
{
    ui->setupUi(this);

    //Loading logo infor from local logo file
    updateLogo();


    //Added by:wangl@20150506
    //关联网络状态变化信号与左边的控件显示
    connect(this, SIGNAL(sigNetworkStatusChanged(bool)), this, SLOT(slotInitLeftWidget(bool)));
    //关联网络变态变化信号与更新网络状态图标
    connect(this, SIGNAL(sigNetworkStatusChanged(bool)), this, SLOT(slotUpdateNetWorkState(bool)));

    _mac = SysNetInterface::GetDevSN();


    _isLoginIdent = false;
    _isLoginSuccess = false;
    _isOutWigan = false;
//    _isOutWigan26 = false;
//    _isOutWigan34 = false;
    _isOutRelay = true;
    _minOutRalay = 0;
    _secOutRalay = 3;
    _countDown = -1;
    _varStartY = g_imgHeight/3;
    _isReloadiris = true;
    _isSlept = false;  //初始化为非睡眠状态


    ui->labIsLogin->hide();
    ui->labCount->hide();
    //ui->labLogo->hide();

    /*********获取配置信息*****************/
    GetConfig();

    rebootTime = IkConfig::GetRebootTime();

    //    //新建定时器 关联当前时间
    //    this->startTimer(500);
    //新建定时器
    _timerStartIdent = new QTimer(this);
    //关联定时器计满信号和相应的槽函数--识别成功失败需要在屏幕显示结果1s

    connect(_timerStartIdent,SIGNAL(timeout()),this,SLOT(isStartIdent()));
    connect(&drawEvent,SIGNAL(sigDrawIdent()),this,SLOT(imageUpdate()));
    connect(&identEvent,SIGNAL(sigResult(long,int)),this,SLOT(identResult(long,int)));
    connect(&identEvent,SIGNAL(sigLoginResult(long,int)),this,SLOT(loginIdentResult(long,int)));
    connect(&identEvent,SIGNAL(sigInteractionStatus(bool)),this,SLOT(interactionStatus(bool)));
    connect(&identEvent,SIGNAL(sigIdentCardID(QString)),this,SLOT(StartIdentCardID(QString)));
    connect(&identEvent,SIGNAL(sigInitSdkOutTime()),this,SLOT(slotInitSdkOuttime()));

    ui->btnSaveImage->hide();

    ui->saveImageStatus->hide();
    connect(ui->btnSaveImage, SIGNAL(clicked()), this, SLOT(slotSaveImg()));

    //20140822lizhil
    connect(&identEvent,SIGNAL(sigSetUIImg(bool)),this,SLOT(SetUIImg(bool)));

    connect(&identEvent,SIGNAL(sigSleepSDK()),this,SLOT(slotReleaseDevice()));
    connect(&identEvent,SIGNAL(sigWakeSDK()),this,SLOT(slotWakeDevice()));

    //连接重新初始化SDK结果信号槽(guowen 2018-01-22)
    connect(this, SIGNAL(sigReInitSDKResult(bool)), this, SLOT(slotReInitSDKResult(bool)));

    //人机交互初始化串口
    _identInteraction = Interaction::GetInteractionInstance();
    _identResultState = ContinueState;

    //@dj for sdk -----------------------------------------------------------------------------

    ReInitSDK();

    // ahnan 20161026
    _m_isSDKWake = true;

    //人脸初始化t
    //@ for hang 为排查采集人脸图导致死机bug注释
    //@@@
    //    _faceCamer = new CCameraApi();
    //    _faceCamer->Init();
    //    _faceCamer->GetImgInfo(&_facephotoSize);
    //    _facephotoBuf = new unsigned char[_facephotoSize.ImgSize * 2 * sizeof(unsigned char)];
    //------------------------------------------------------------------------------------------
    AddSeriseCallback();
    _countCardMode = -1;

    _noticeCount = 1;

    //初始化门铃定时器
    InitTimers();

    //20141217 lizhl
    connect(&identEvent, SIGNAL(sigUIofIden()),this, SLOT(slotUIofIden()));

    connect(&identEvent, SIGNAL(sigUIofWait()), this, SLOT(slotUIofWait()));

    connect(&identEvent, SIGNAL(sigUIofSleep()),this, SLOT(slotUIofSleep()));

    //采集人脸独立为一个线程，李志林
    //@ for crash
    _capFaceFlag = false;
    _capFaceThread = std::thread(ThreadCapFaceImg, this);

    //20140822
    _uiImg = new QImage();
    _loadMovie = new QMovie(":/image/loading.gif");
    _loadMovie->setScaledSize(QSize(100,100));
    //    _loadMovie->setScaledSize(QSize(664,498));
    _loadMovie->setSpeed(100);

    _loadLabel = new QLabel(this);
    _loadLabel->setGeometry(438,282,100,100);
    _loadLabel->setMovie(_loadMovie);
    _loadLabel->setVisible(false);

    //guowen添加2018-01-23
    _waitSdkInitMovie = new QMovie(":/image/loading.gif");
    _waitSdkInitMovie->setScaledSize(QSize(300,300));
    _waitSdkInitMovie->setSpeed(100);
    _waitSdkInitLable = new QLabel(this);
    _waitSdkInitLable->setGeometry(362, 150, 300, 300);
    _waitSdkInitLable->setMovie(_waitSdkInitMovie);
    _waitSdkInitLable->setVisible(false);

    //20150113 wangl
    _waitMovie = new QMovie(":/image/ui_scanning_eyes_664_498.gif");
    _waitMovie->setScaledSize(QSize(498,373));
    _waitMovie->setSpeed(150);

    ui->labelSleep->setMovie(_waitMovie);
    ui->labelSleep->setVisible(false);
    ui->labelSleep->setVisible(false);


    initNetStateDisplay();
    //Added BY: Wang.L @2015-01-21
    connect(&_timeCoordinateTimer, SIGNAL(timeout()), this, SLOT(SetTimecoordinate()));

    //20141212 lizhl
    InitLabelStatus();//识别界面在ui中已经存在labStatu，不需要重新创建一个_labStatus

    //20141202
    InitLabelMask();

    //20141208
    InitTraceGif();

    InitIdentWindow();

    //_isInSelectLogin = false;

    //@ weather show
    m_weather = new HBWeather("101010300");


    //    _ptimerUpate = new QTimer(this);
    //    connect(_ptimerUpate,SIGNAL(timeout()),this,SLOT(slotTimerWeather()));
    //@ 暂时注释天气 _ptimerUpate->start(2000);

    //新建定时器 关联当前时间
    this->startTimer(500);
    LOG_INFO("this->startTimer(500)");
}

//Added By: Wang.L @20150123
//Target: Get all relative timers initialized here, e.g. time interval, singal shot;
void DialogIdentify::InitTimers()
{
    Exectime etm(__FUNCTION__);
    //门铃响后定时关闭
    _bellTimer.setInterval(_bellRingTimeLen * 1000);
    _bellRingCtrlTimer.setInterval(500);
    _bellRingCtrlTimer.setSingleShot(false);

    connect(&_bellTimer, SIGNAL(timeout()), this, SLOT(closeBell()));
    connect(&_bellRingCtrlTimer, SIGNAL(timeout()),this, SLOT(PlayDoorbell()));
}

void DialogIdentify::initNetStateDisplay()
{
    Exectime etm(__FUNCTION__);
    //_netStatePlayer.setScaledSize(QSize(ui->labNetStateVal->size().width(), ui->labNetStateVal->size().width()));

    _netStatePlayer.setFileName(":/image/ui_network_offline.gif");
    _netStatePlayer.setScaledSize(QSize(45, 45));
    _netStatePlayer.setSpeed(100);

    ui->labNetStateVal->setMovie(&_netStatePlayer);
    _netStatePlayer.start();
}


void DialogIdentify::slotInitLeftWidget(bool isOK)
{
    //TODO

    Exectime etm(__FUNCTION__);
    ShowLeftWgt(true);
}


void DialogIdentify::ShowLeftWgt(bool isShow)
{
    Exectime etm(__FUNCTION__);
    //TODO 判断网络模式，如果是联网则显示天气预报，非联网时则显示logo
    if(isShow)
    {
        //        if(IsConnectedToNetwork())    //如果是联网模式
        //            //if(false)    //如果是联网模式
        //        {
        //            ShowWeather();
        //        }
        //        else  //非联网模式
        //        {
        //            ShowLogo();
        //        }
        ShowLogo();
    }
    else
    {
        ui->wgtLeftLogo->hide();
        ui->wgtLeftWeatherInfo->hide();
    }
}


void DialogIdentify::ShowOnLeftWgt(bool isNetOK)   //Added BY: Wang.L @20160218
{
    Exectime etm(__FUNCTION__);
    if(isNetOK)
        if(IsConnectedToNetwork())    //如果是联网模式
        {
            ShowWeather();
        }
        else  //非联网模式
        {
            ShowLogo();
        }
}


void DialogIdentify::ShowLogo()     //Added BY: Wang.L @20160218
{
    Exectime etm(__FUNCTION__);
    ui->wgtLeftWeatherInfo->hide();
    ui->wgtLeftLogo->show();
}

void DialogIdentify::ShowWeather()   //Added BY: Wang.L @20160218
{
    Exectime etm(__FUNCTION__);
    ui->wgtLeftWeatherInfo->show();
    ui->wgtLeftLogo->hide();
}

void DialogIdentify::ShowClock()
{
    Exectime etm(__FUNCTION__);
    //获取系统现在的时间
    QDateTime time = QDateTime::currentDateTime();
    //设置系统时间显示格式
    QString strTime = time.toString("hh:mm:ss");
    std::string week = QDateTime::currentDateTime().toString("dddd").toStdString();
    //Edited By wang.l 2014-09-23
    QString strDate;   // = time.toString("MM") + tr("月") + time.toString("dd") + tr("日");
    if(_weekDay.count(week) > 0)
    {
        strDate = QString(" %1   %2").arg((_weekDay[week]).c_str(),
                                          QDateTime::currentDateTime().toString("yyyy-MM-dd"));
    }
    else
    {
        strDate = QDateTime::currentDateTime().toString("yyyy-MM-dd   dddd");
    }

    //界面显示
    ui->labDate->setText(strDate);
    QString styleSheet("QLabel#labHour{"
                       "  background-image: url(:/image/ui_digtal_%1.png);"
                       "}"
                       "QLabel#labMinute{"
                       "  background-image: url(:/image/ui_digtal_%2.png);"
                       "}"
                       "QLabel#labSecond{"
                       "  background-image: url(:/image/ui_digtal_%3.png);"
                       "}");
    ui->wgtTime->setStyleSheet(styleSheet.arg(QDateTime::currentDateTime().toString("hh"),
                                              QDateTime::currentDateTime().toString("mm"),
                                              QDateTime::currentDateTime().toString("ss")));
}

void DialogIdentify::StartIdentTimer(int msec)
{
    Exectime etm(__FUNCTION__);
    if(msec < 0)
        msec = 2000;
    LOG_INFO("启动timer");
    _timerStartIdent->start(msec); ///2000
    if(_timerStartIdent->timerId() == -1)
    {
        LOG_ERROR("启动识别timer失败");
    }
}


//如果客户定制了logo显示，则显示客户定制的logo
void DialogIdentify::updateLogo()
{
    Exectime etm(__FUNCTION__);
    QString logoFilePath("%1%2ui_iden_logo.png");
    logoFilePath = logoFilePath.arg(QApplication::applicationDirPath(), QDir::separator());
    QDir appDir(QApplication::applicationDirPath());

    std::cout << GetLogHead(__FUNCTION__) << "logo file path: " << logoFilePath.toStdString() << std::endl;
    //如果用户定制了logo则显示用户logo
    qDebug()<<"logo-lujingwei"<<logoFilePath<<endl;
    if(appDir.exists(logoFilePath))
    {
        QString logoStyle("	background-image: url(%1);");

        ui->labLogo->setStyleSheet(logoStyle.arg(logoFilePath));
    }
    else
    {
        std::cout << GetLogHead(__FUNCTION__) << "unexist logo file path: " << logoFilePath.toStdString() << std::endl;
    }

    //不显示天气，而是显示logo
    ShowLeftWgt(true);
}

//使用新的获取配置信息方式 Added By:wangl@20150514
void DialogIdentify::GetConfig()
{
    Exectime etm(__FUNCTION__);
    IKXmlHelper sysCfg(GetXmlFilePath());
    if(sysCfg.IsValid())
    {
        //识别模式
        //2015-08-26 增加新的识别方式即：刷卡或者虹膜都可以通过
        if (IKXmlHelper::IDenMode::IRIS_OR_CARD == sysCfg.GetIdenMode())
        {
            _cardoririsMode = true;
            _defaultIdentMode = true;
        }
        else if(IKXmlHelper::IDenMode::IRIS_AND_CARD == sysCfg.GetIdenMode())  //除“刷卡与虹膜”模式外都作”仅虹膜识别“模式处理
        {
            _defaultIdentMode = false;
            _cardoririsMode   = false;
        }
        else
        {
            _cardoririsMode   = false;
            _defaultIdentMode = true;
        }
        qDebug()<<"_cardoririsMode:"<<_cardoririsMode<<"  _defaultIdentMode:"<<_defaultIdentMode<<endl;

        //门铃振铃时间
        if((_bellRingTimeLen = sysCfg.GetDoorBellDuration()) < 0)
        {

            _bellRingTimeLen = 5;       //默认为5秒
        }

        // gdj
        if(G_BellTime != -1)
        {
            _bellRingTimeLen = G_BellTime;
        }



        //默认都不输出
        _isOutWigan = false;
//        _isOutWigan26 = false;
//        _isOutWigan34 = false;
        _isOutRelay = false;

        //识别结果输出配置信息
        switch(sysCfg.GetOutputTo())
        {
        case IKXmlHelper::OutputInfo::TO_WIEGAND:   //仅输出韦根
            _isOutWigan = true;
            break;
        case IKXmlHelper::OutputInfo::TO_RELAY:     //仅输出继电器设备
            _isOutRelay = true;
            break;
        case IKXmlHelper::OutputInfo::TO_WIEGAND_AND_RELAY:   //同时输出到韦根和继电器设备
            _isOutWigan = true;
            _isOutRelay = true;
            break;
        case IKXmlHelper::OutputInfo::TO_NONE:    //不输出
        default:
            break;
        }
    }
    else
    {
        std::cout<<"配置文件 ikembx00.xml 未打开！"<<std::endl;
    }
}

void DialogIdentify::TestWakeAndSleep()
{


    //usleep();
}

void DialogIdentify::clearSysLog()
{
    static int timecount = 0;
    if(timecount < 1800)
    {
        timecount ++;
    }
    else
    {
        timecount = 0;
        system("cat /dev/null > /var/log/syslog");
        system("cat /dev/null > /var/log/syslog.1");
        system("cat /dev/null > /var/log/kern.log");
    }
}
/*****************************************************************************
*                        注册人机交互函数
*  函 数 名： InterationRegCB
*  功    能： 注册人机交互函数
*  说    明：
*  参    数：
*  返 回 值：
*  创 建 人：fjf
*  创建时间：2015-08-26
*  修 改 人：
*  修改时间：
*****************************************************************************/
void DialogIdentify::InterationRegCB()
{
    Exectime etm(__FUNCTION__);
    //20141205 lizhl
    static bool ifRegisterCB = false;
    if(!ifRegisterCB)
    {
        if(!(_identInteraction->RegisterCallBack("DialogIdentify",IdentInteractionStatusCallBack)))
        {
            LOG_ERROR("人机交互注册函数失败！");
            std::cout<<"人机交互注册函数失败！"<<std::endl;
        }
        ifRegisterCB = true;
    }
}
void DialogIdentify::AddSeriseCallback()
{
    Exectime etm(__FUNCTION__);
    //增加新的识别模式的判断 2015-08-26 fjf
    if (_cardoririsMode)
    {
        //同时处理两种回调
        this->InterationRegCB();
        this->SetSerialCallBack(true);
    }
    else
    {
        if(_defaultIdentMode)
        {
            //fjf 调用抽出函数
            this->InterationRegCB();
        }
        else
        {
            SetSerialCallBack(true);
        }
    }

    unsigned char cmd = 0x0b;
    if(_sm && !_sm->AddCallback(cmd, RecvRalayCallBack))
    {
        std::cout<<"串口分发类注册回调函数失败！"<<std::endl;
        LOG_ERROR("串口分发类注册回调函数失败！");
        return;
    }
    else
    {
        std::cout<<"向串口注册继电器回调函数成功！"<<std::endl;
    }

    cmd = 0x0a;
    if(_sm && !_sm->AddCallback(cmd, RecvWiganCallBack))
    {
        std::cout<<"串口分发类注册回调函数失败！"<<std::endl;
        LOG_ERROR("串口分发类注册回调函数失败！");
        return;
    }
    else
    {
        std::cout<<"向串口注册韦根回调函数成功！"<<std::endl;
    }

}
/*****************************************************************************
*                        删除注册的回调函数
*  函 数 名： DellSeriseCallback
*  功    能： 将注册的回调函数删除
*  说    明： 问题：1、是否需要把新增加OR功能注册的回调函数清空
*                 2、是否单独处理抽出函数
*  参    数：
*  返 回 值：
*  创 建 人：fjf
*  创建时间：2015-08-26
*  修 改 人：
*  修改时间：
*****************************************************************************/
void DialogIdentify::DellSeriseCallback()
{
    Exectime etm(__FUNCTION__);
    if(_defaultIdentMode)
    {
        //        _identInteraction->DeleteCallBack("DialogIdentify");
    }
    else
    {
        SetSerialCallBack(false);
    }

    unsigned char cmd = 0x0b;
    if(_sm && !_sm->RemoveCallback(cmd))
    {
        std::cout<<"串口分发类取消注册回调函数失败！"<<std::endl;
        LOG_ERROR("串口分发类取消注册回调函数失败！");
        return;
    }
    else
    {
        std::cout<<"向串口取消注册继电器回调函数成功！"<<std::endl;
    }

    cmd = 0x0a;
    if(_sm && !_sm->RemoveCallback(cmd))
    {
        std::cout<<"串口分发类取消注册回调函数失败！"<<std::endl;
        LOG_ERROR("串口分发类取消注册回调函数失败！");
        return;
    }
    else
    {
        std::cout<<"向串口取消注册韦根回调函数成功！"<<std::endl;
    }

}

DialogIdentify::~DialogIdentify()
{
    Exectime etm(__FUNCTION__);
    //线程join
    //    _identCapImgThread.join();
    //    @@@_faceCamer->Release(true);
    //    delete _faceCamer;
    //    _faceCamer = nullptr;

    delete []_facephotoBuf; //dj 20170301
    _facephotoBuf = nullptr;

    delete _timerStartIdent;
    _timerStartIdent = nullptr;

    delete _identInteraction;
    _identInteraction = nullptr;

    delete m_weather;
    m_weather = nullptr;

    delete _uiImg;
    _uiImg = nullptr;

    delete _loadMovie ;
    _loadMovie = nullptr;

    delete _loadLabel;
    _loadLabel = nullptr;

    delete _tipLabel;
    _tipLabel = nullptr;

    delete _waitMovie;//在红外测距未检测到人时显示一个gif
    _waitMovie = nullptr;

    delete _waitLabel;//
    _waitLabel = nullptr;

    delete _bellRing;
    _bellRing = nullptr;

    delete _labMask;
    _labMask = nullptr;

    delete _movTraceL;
    _movTraceL = nullptr;

    delete _labTraceL;
    _labTraceL = nullptr;

    delete _movTraceR;
    _movTraceR = nullptr;

    delete _labTraceR;
    _labTraceR = nullptr;

    delete ui;

    //_ptimerUpate = NULL;
}


/*****************************************************************************
*                        门铃事件
*  函 数 名： sendBell
*  功    能： 启动门铃
*  说    明：
*  参    数：
*  返 回 值：
*  创 建 人：刘中昌
*  创建时间：2014-04-01
*  修 改 人：
*  修改时间：
*****************************************************************************/
void DialogIdentify::sendBell()
{
    Exectime etm(__FUNCTION__);
    //启动定时器
    _bellTimer.setInterval(_bellRingTimeLen * 1000);
    std::cout<<"<<<<<<<<<<_bellRingTimeLen = "<<_bellRingTimeLen<<std::endl;
    _bellTimer.start();
    _bellRingCtrlTimer.start();


    //设置门铃振铃效果
    ui->toolBtnBell->setStyleSheet(QString("border-top-left-radius: 3px;"
                                           "border-bottom-left-radius: 3px;"
                                           "background-image: url(:/image/ui_doorbell_on.png);"));

    unsigned char cmd[] = {0x0d,0x01,0x00,0x00};
    _sm->Send(cmd,4);
    std::cout<<"发送门铃事件"<<std::endl;
}

/*****************************************************************************
*                        响铃动画
*  函 数 名： PlayDoorbell
*  功    能： 响铃动画实现
*  说    明：
*  参    数：
*  返 回 值：
*  创 建 人：Wang.L
*  创建时间：2014-09-28
*  修 改 人：
*  修改时间：
*****************************************************************************/
void DialogIdentify::PlayDoorbell()
{
    Exectime etm(__FUNCTION__);

    static int frameIndex = 0;
    QString ringBell("border-top-left-radius: 3px;"
                     "border-bottom-left-radius: 3px;"
                     "background-image: url(:/image/%1);");
    if(++frameIndex%2)
    {
        ui->toolBtnBell->setStyleSheet(ringBell.arg("ui_doorbell_off.png"));
    }
    else
    {
        ui->toolBtnBell->setStyleSheet(ringBell.arg("ui_doorbell_on.png"));
    }

    if(frameIndex>=1000)
        frameIndex = 0;
}


/*****************************************************************************
*                        选择登陆类型
*  函 数 名： selectLogin
*  功    能： 选择登陆类型
*  说    明：
*  参    数：
*  返 回 值：
*  创 建 人：刘中昌
*  创建时间：2014-05-13
*  修 改 人：
*  修改时间：
*****************************************************************************/
void DialogIdentify::selectLogin()
{
    Exectime etm(__FUNCTION__);
    //_isInSelectLogin = true;
    if(!_isLoginIdent)
    {
        DialogSelectLogin selectLog(this);
        selectLog.exec();
        switch(selectLog.result())
        {
        case DialogSelectLogin::Account:
            AccountLogin();
            break;
        case DialogSelectLogin::Iris:
            IrisLogin();
            break;
        default:
            break;
        }
    }else
    {
        ReIdent();
    }
}


void DialogIdentify::IrisLogin()
{
    Exectime etm(__FUNCTION__);
    if(_isLoginIdent)
    {
        return;
    }
    _countDown = 30;
    //设置登录按钮的外观和行为，使其显示可取消虹膜登录
    ui->toolBtnLogin->setStyleSheet("QToolButton#toolBtnLogin{"
                                    "  border-top-left-radius: 3px;"
                                    "  border-bottom-left-radius: 3px;"
                                    "  background-image: url(:/image/ui_cancel_normal.png);"
                                    "}"
                                    "QToolButton#toolBtnLogin:pressed{"
                                    "  background-image: url(:/image/ui_cancel_pressed.png);"
                                    "}");

    ui->labCountDown->show();

    _isLoginIdent = true;
    // ahnan 20161025
    if(0 != IKUSBSDK_StopIden())
    {
        LOG_DEBUG("ahnan DialogIdentify::IrisLogin IKUSBSDK_StopIden failed");

        usleep(1000000);
        if(0 != IKUSBSDK_StopIden())
        {
            LOG_DEBUG("ahnan after usleep 1s DialogIdentify::IrisLogin IKUSBSDK_StopIden failed");

        }


    }
    /// @dj
    //isStartIdent();
}

void DialogIdentify::CancelLogin()
{
    Exectime etm(__FUNCTION__);
    if(!_isLoginIdent)
    {
        return;
    }


    //恢复登录按钮的默认外观和行为
    ui->toolBtnLogin->setStyleSheet("QToolButton#toolBtnLogin{"
                                    "  border-top-left-radius: 3px;"
                                    "  border-bottom-left-radius: 3px;"
                                    "  background-image: url(:/image/ui_login_normal.png);"
                                    "}"
                                    "QToolButton#toolBtnLogin:pressed{"
                                    "  background-image: url(:/image/ui_login_pressed.png);"
                                    "}");


    ui->labCountDown->hide();
    _countDown = -1;
    _isLoginIdent = false;
    _countCardMode = 0;
    //isStartIdent();
}

void DialogIdentify::AccountLogin()
{
    Exectime etm(__FUNCTION__);
    DialogLogin* login = new DialogLogin(this);
    if(QDialog::Accepted == login->exec())
    {
        _isLoginSuccess = true;
        emit sigLoginSucceed(_isLoginSuccess);
        quitIdent();
    }

    delete login;
}


void DialogIdentify::WakeInteraction()
{
    _identInteraction->ScreenPressResponse();
}

/*****************************************************************************
*                        关闭门铃事件
*  函 数 名： sendBell
*  功    能： 启动门铃
*  说    明：
*  参    数：
*  返 回 值：
*  创 建 人：刘中昌
*  创建时间：2014-04-01
*  修 改 人：
*  修改时间：
*****************************************************************************/
void DialogIdentify::closeBell()
{

    Exectime etm(__FUNCTION__);

    _bellTimer.stop();
    _bellRingCtrlTimer.stop();

    unsigned char cmd[] = {0x0d,0x00,0x00,0x00};
    _sm->Send(cmd,4);
    QString ringBell("border-top-left-radius: 3px;"
                     "border-bottom-left-radius: 3px;"
                     "background-image: url(:/image/ui_bell_normal.png);");
    ui->toolBtnBell->setStyleSheet(ringBell);//AQUA BELL.png

    std::cout<<"发送关闭门铃事件"<<std::endl;
}


/*****************************************************************************
*                        是否开始识别
*  函 数 名： IsStartIdent
*  功    能：启动识别
*  说    明：
*  参    数：
*  返 回 值：
*  创 建 人：刘中昌
*  创建时间：2013-12-23
*  修 改 人：
*  修改时间：
*****************************************************************************/
void DialogIdentify::isStartIdent()
{
    Exectime etm(__FUNCTION__);
    g_isStopIdent = false ;
    _identResultState = ContinueState;
    _timerStartIdent->stop();
    //20141223 lizhl 开始识别，不再显示上次识别结果
    DrawStatusLabel(StatusIdenWait);

    LOG_INFO("is in Ident timer :g_isQuitIdent = %d;_isLoginIdent=%d",g_isQuitIdent,_isLoginIdent);

    if(!g_isQuitIdent && !_isLoginIdent)
    {
        ui->labIsLogin->setText(QString::fromUtf8("虹膜考勤"));
        StartAttendIdent();
    }
    else if(!g_isQuitIdent && _isLoginIdent)
    {
        ui->labIsLogin->setText(QString::fromUtf8("虹膜登陆"));

        StartLoginIdent();
    }
    else
    {
        std::cout<<"停止识别  ： _isClose :"<<g_isQuitIdent <<"_isLoginIdent :"<<_isLoginIdent <<std::endl;
        LOG_INFO("停止识别中...g_isQuitIdent:%d",g_isQuitIdent);
    }
}

/*****************************************************************************
*                        加载虹膜特征
*  函 数 名： LoadCodeList
*  功    能： 加载特征
*  说    明：
*  参    数：
*  返 回 值：
*  创 建 人：刘中昌
*  创建时间： 2013-03- 07
*  修 改 人：
*  修改时间：
*****************************************************************************/
void DialogIdentify::LoadCodeList(bool isIrisLogin)
{
    Exectime etm(__FUNCTION__);
    if(_isReloadiris)  //caotao 20161108, 修改从管理界面回来，加载特征数目不正确问题,
    {

        int result = pthread_create(&loadDataThread,NULL,ThreadLoadData,this);
        pthread_detach(loadDataThread);
        LOG_INFO("pthread_loaddata_result = %d",result);

        //        ReleseCodeList();
        //        //初始化
        //        _irisManager.Init(); // hzf 20161026
    }
    //@dj
    if(isIrisLogin)
    {
        _irisManager.ReleaseSuperData();
        _irisManager.LoadSuperData();


    }


    //加载普通人员特征
    _irisManager.GetIrisFeature(IdentifyType::Left,_codeListL,_codeNumL);
    _irisManager.GetIrisFeature(IdentifyType::Right,_codeListR,_codeNumR);
    LOG_INFO("左右眼特征数量：_codeNumL %d, _codeNumR %d",_codeNumL,_codeNumR);

    //加载管理员特征
    _irisManager.GetSuperFeature(IdentifyType::Left,_loginCodeListL,_loginCodeNumL);
    _irisManager.GetSuperFeature(IdentifyType::Right,_loginCodeListR,_loginCodeNumR);
    _isReloadiris = false;

}

void DialogIdentify::ReleseCodeList()
{
    Exectime etm(__FUNCTION__);
    _irisManager.ReleaseFeature();
    _irisManager.ReleaseSuperData();
}

void DialogIdentify::SetReLoadIris(bool isReLoad)
{
    //@dj _isReloadiris = isReLoad;
}

/*****************************************************************************
*                        设置识别模式
*  函 数 名： SetIdentParam
*  功    能： 启动识别
*  说    明：
*  参    数：
*  返 回 值：
*  创 建 人：刘中昌
*  创建时间：2013-03-08
*  修 改 人：
*  修改时间：
*****************************************************************************/
int DialogIdentify::SetIdentParam()
{
    //int funResult = E_ALGAPI_DEFAUL_ERROR;

    //    funResult = _identApi->GetParam(&_idenParamStruct);
    //    std::cout<<funResult<<std::endl;
    //    if(E_ALGAPI_OK == funResult)
    //    {
    //        _idenParamStruct.EyeMode = AnyEye;
    //        funResult = _identApi->SetParam(&_idenParamStruct);
    //    }
    //return funResult;
    return 0;
}

/*****************************************************************************
*                        识别相关初始化
*  函 数 名： InitIdent
*  功    能：
*  说    明：
*  参    数：
*  返 回 值：
*  创 建 人：刘中昌
*  创建时间：2014-05-16
*  修 改 人：
*  修改时间：
*****************************************************************************/
void DialogIdentify::InitIdent()
{
    Exectime etm(__FUNCTION__);
    //加载虹膜特征
    LoadCodeList();
    std::cout<<"加载虹膜特征 _codeNumL: "<<_codeNumL<<" _codeNumR :"<<_codeNumR<<std::endl;
}

/*****************************************************************************
*                        开始识别
*  函 数 名： StartIdent
*  功    能：启动识别
*  说    明：
*  参    数：
*  返 回 值：
*  创 建 人：刘中昌
*  创建时间：2013-03-08
*  修 改 人：
*  修改时间：
*****************************************************************************/
void DialogIdentify::StartIdent()
{
    Exectime etm(__FUNCTION__);
    //加载虹膜与人员信息
    InitIdent();

    //设置识别模式
    //@ 任意眼模式 SetIdentParam();


    //重新获取设备信息
    GetConfig();
    AddSeriseCallback();

    //设置初始状态和标志位---防止返回识别界面时仍然绘制成功或失败
    _identResultState = UnKnownState;


    //初始化识别设置
    ReIdent();

    if(_defaultIdentMode || _cardoririsMode )
    {
        isStartIdent();
        //StartIdentTimer();
    }

}

/*****************************************************************************
*                        开始识别
*  函 数 名： StartAttendIdent
*  功    能：启动识别
*  说    明：
*  参    数：
*  返 回 值：
*  创 建 人：刘中昌
*  创建时间：2013-12-23
*  修 改 人：
*  修改时间：
*****************************************************************************/
void DialogIdentify::StartAttendIdent()
{
    Exectime etm(__FUNCTION__);
    //fjf add debug cout
    std::cout<<"启动识别...."<<std::endl;
    ui->labIsLogin->setText(QString::fromUtf8("考勤识别"));

    _isLoginIdent = false;
    //if(_isReloadiris)
    {
        // LOG_INFO("StartAttendIdent _isReloadiris: %d", _isReloadiris);
        LoadCodeList();
    }
#ifdef DEBUG
    std::cout<<"开始识别StartAttendIdent函数时间:"
            <<QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss").toStdString()<<"  "
           <<std::endl;
#endif
    LOG_DEBUG("开始考勤识别 _isLoginIdent: %d", _isLoginIdent);
    //@ dj
    if(!g_isIdentThreadEnd)
    {
        LOG_ERROR("开启识别线程时候，发现上次识别线程并未结束");
    }
    //啓動sdk識別線程
    int pthread_create_result =  pthread_create(&idenThread, NULL, ThreadIden, this);
    LOG_INFO("pthread_create_result = %d",pthread_create_result);
    pthread_detach(idenThread);
}

/*****************************************************************************
*                        管理员登陆开始识别
*  函 数 名： StartLoginIdent
*  功    能：启动识别
*  说    明：
*  参    数：
*  返 回 值：
*  创 建 人：刘中昌
*  创建时间：2013-12-23
*  修 改 人：
*  修改时间：
*****************************************************************************/
void DialogIdentify::StartLoginIdent()
{
    Exectime etm(__FUNCTION__);

#ifdef DEBUG
    std::cout<<"开始管理员登陆识别 _loginCodeNumL: "<<_loginCodeNumL<<" _loginCodeNumR :"<<_loginCodeNumR<<std::endl;
#endif
    LOG_DEBUG("开始管理员登陆识别 _isLoginIdent: %d", _isLoginIdent);

    bool isIrisLogin = true;
    LoadCodeList(isIrisLogin);
    // ahnan 20161025
    int pthread_create_result = pthread_create(&idenThread, NULL, ThreadIden, this);
    LOG_INFO("pthread_create_result = %d",pthread_create_result);
    pthread_detach(idenThread);

}

/*****************************************************************************
*                        退出识别函数
*  函 数 名：QuitIdent
*  功    能：退出识别
*  说    明：
*  参    数：
*  返 回 值：
*  创 建 人：刘中昌
*  创建时间：2013-12-23
*  修 改 人：liuzhch
*  修改时间：2014-04-01
*****************************************************************************/
void DialogIdentify::quitIdent()
{
    Exectime etm(__FUNCTION__);
    if(g_isQuitIdent)
    {
        LOG_WARN("设备已经退出识别");
        return;
    }
    g_isQuitIdent = true;
    _countDown = -1;


    //停止识别
    StopIdent();

    DellSeriseCallback();
    //释放人脸API
    //_faceCamer->Release();
    _identResultState = UnKnownState;



    //删除虹膜特征
    //ReleseCodeList();


}

/*****************************************************************************
*                        停止识别函数
*  函 数 名：StopIdent
*  功    能：停止识别
*  说    明：
*  参    数：
*  返 回 值：
*  创 建 人：刘中昌
*  创建时间：2013-12-23
*  修 改 人：liuzhch
*  修改时间：2014-04-01
*****************************************************************************/
void DialogIdentify::StopIdent()
{
    Exectime etm(__FUNCTION__);

    _timerStartIdent->stop(); //@add for sdk

    LOG_INFO("StopIdent IKUSBSDK_StopIden _isLoginIdent: %d", _isLoginIdent);

    if(g_isStopIdent)
    {
        LOG_ERROR("停止识别时，SDK识别已经停止 g_isStopIdent:%d",g_isStopIdent);
        return ;

    }
    if(!g_isQuitIdent)
    {
        int result = IKUSBSDK_StopIden();
        LOG_INFO("StopIdent IKUSBSDK_StopIden result: %d",result);
        if(result != 0)
        {
            LOG_ERROR("SDK停止识别失败！%d",result);
            return;

        }

        //@dj
        //pthread_join(idenThread,NULL);
        g_isStopIdent = true;
    }
    else
    {
        LOG_ERROR("退出识别时有错，当前已经时退出识别状态g_isQuitIdent:%d",g_isQuitIdent);
    }

    //20141215 lizhl
    SetTraceVisible(false);
    DrawStatusLabel(StatusIdenStop);
    LOG_INFO("------Out StopIdent------");
}

int DialogIdentify::ShowIdentResult(int funResult, IKResultFlag lrFlag, IKSDKMatchOutput matchOutputL, IKSDKMatchOutput matchOutputR)
{
    Exectime etm(__FUNCTION__);
    if(funResult == E_USBSDK_ENR_IDEN_INTERRUPT)
    {//人为停止
        //std::cout<<"人为停止 funResult 为："<< funResult <<std::endl;
        // StopIdent();//@
        if(_isLoginIdent)
        {
            StartIdentTimer();
            //isStartIdent();
        }
        return -1;
    }

    //@ 跟新错误码 对应新sdk
    if (E_USBSDK_ENR_IDEN_FAILED == funResult)
    {
        switch(lrFlag)
        {
        case EnrRecBothFailed://左右眼都失败
            //@dj
            //            identEvent.onResult(-1,3,_isLoginIdent);
            //            break;
        case EnrRecRightFailed:
            //            identEvent.onResult(-1,3,_isLoginIdent);
            //            break;
        case EnrRecLeftFailed:
            identEvent.onResult(-1,3,_isLoginIdent);
            break;
        default:
            identEvent.onResult(0,3,_isLoginIdent);
            std::cout<<"识别失败但是funResult 为："<< funResult <<std::endl;
            break;
        }
    }
    else if(funResult == E_USBSDK_OK)
    {
        switch(lrFlag)
        {
        case EnrRecLeftSuccess:
            identEvent.onResult(matchOutputL.MatchIndex[0],0,_isLoginIdent);
            break;
        case EnrRecRightSuccess:
            identEvent.onResult(matchOutputR.MatchIndex[0],1,_isLoginIdent);
            break;
        case EnrRecBothSuccess:
            //问题：识别结果没有返回质量分数，如何判断发送那个比对index
            //暂时 发送左眼
            identEvent.onResult(matchOutputL.MatchIndex[0],0,_isLoginIdent);
            break;
        default:
            std::cout<<"识别成功 ！但是funResult 为："<< funResult <<std::endl;
        }
        //	//caotao 20161127, 修改登录识别成功后注册图像不更新问题
        //	if(_isLoginIdent)
        //	{
        //		_countDown = -2;
        //	}
    }
    else
    {
        identEvent.onResult(0,3,_isLoginIdent);
    }


    return 0;


}


/*****************************************************************************
*                        显示识别结果函数
*  函 数 名：IdentResult
*  功    能：识别结果
*  说    明：
*  参    数：
*  返 回 值：
*  创 建 人：刘中昌
*  创建时间：2013-12-27
*  修 改 人：
*  修改时间：
*****************************************************************************/
void DialogIdentify::identResult(long personIndex,int flag)
{

    Exectime etm(__FUNCTION__);


    if(personIndex<0)
    {
        //20141215 lizhl
        DrawStatusLabel(StatusIdenFailed);
        _identResultState = FailedState;
        //人脸图像采集独立为一个线程，李志林
        _idenName = "";
        _isIdenSuccess = false;
        {
            std::unique_lock <std::mutex> lck(_capFaceImgMutex);
            _capFaceFlag = true;
            _capFaceImgCV.notify_one();
        }


    }
    else if(3 == flag)
    {
        _identResultState = ContinueState;
    }
    else
    {

        std::cout<<"识别成功! 时间:"
                <<QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss").toStdString()<<"  "
               <<std::endl;

        PeopleInfo person;

        try
        {
            if(0 == flag)
            {
                _irisManager.SelectInfoFromFeature(personIndex,person,IdentifyType::Left);
            }
            else
            {
                _irisManager.SelectInfoFromFeature(personIndex,person,IdentifyType::Right);
            }
            LOG_INFO("personInde=%d",personIndex);
            std::cout<<"personIndex="<<personIndex<<endl;

            // LOG_INFO("1name:%s,FeatureID:%s",person.Name.c_str(),person.FeatureID);


            //如果刷卡识别则要判断_personCardID是否与person的CardID相同 以后还要加是否禁止识别功能能
            //---之后要写个函数
            // if(_personCardID.toStdString() !=person.CardID)
            QString dbPersoncardID  = QString::fromStdString(person.CardID);
            QString curCardIDDec    = ConvertCardNumToDec(_personCardID);

            LOG_INFO("MemPersonCardID=%s"  ,dbPersoncardID.toStdString().c_str());
            LOG_DEBUG("curCardIDDec =%s"    ,curCardIDDec.toStdString().c_str());

            if(_defaultIdentMode || (!_defaultIdentMode && (dbPersoncardID == curCardIDDec)))
            {
                //20141215 lizhl
                DrawStatusLabel(StatusIdenSuccess, QString(person.Name.c_str()));

                _identResultState = SuccessState;

                LOG_INFO("2ame:%s,FeatureID:%s",person.Name.c_str(),person.FeatureID);
                //将数据存储的数据库
                SaveRecogToDB(person);
                LOG_INFO("3name:%s,FeatureID:%s",person.Name.c_str(),person.FeatureID);

                //保存人脸
                // emit sigSavePersonImage(QString(person.Name.c_str()),true);
                //人脸图像采集独立为一个线程，李志林
                _idenName = QString(person.Name.c_str());

                _isIdenSuccess = true;
                {
                    std::unique_lock <std::mutex> lck(_capFaceImgMutex);
                    _capFaceFlag = true;
                    _capFaceImgCV.notify_one();
                }

                //向串口发信号


                SendSingnalToSerial(person);
                if(!_defaultIdentMode && (dbPersoncardID == curCardIDDec) )
                {
                    //@_countCardMode = 1;
                    _countCardMode = 0;
                    //_personCardID = "0";
                }
            }

            else
            {

                //20141215 lizhl
                DrawStatusLabel(StatusIdenFailed);

                std::cout<<"识别成功！但卡号与人不匹配!"<<std::endl;
                //识别成功但卡号与人不匹配
                _identResultState = FailedState;
            }

        }catch(exception e)
        {
            LOG_INFO("识别结果异常%s",e.what());
            std::cout<<e.what()<<endl;
        }
    }
    if(!g_isQuitIdent && _identResultState == ContinueState)
    {
        //异步识别识别
        //isStartIdent();
        StartIdentTimer(0);   //?
    }
    else
    {
        //std::cout<<"停止识别  _isClose 为："<< _isClose <<" _isLoginIdent 为："<< _isLoginIdent<<std::endl <<" _identResultState 为："<< _identResultState <<std::endl;
    }

    bool uploadStatus = IkSocket::GetIkSocketInstance()->IsUpLoading();
    //如果自动上传线程正在上传，则跳过，防止主线程阻塞
    if(!uploadStatus)
    {
        if(!IkSocket::GetIkSocketInstance()->isSocketConnet())
        {
            LOG_INFO("网络未连接，不实时上传识别记录")
                    return ;
        }
        //LOG_INFO("befor UploadTemp");
        IkSocket::GetIkSocketInstance()->UploadTemp();
        //LOG_INFO("after UploadTemp");
    }
    else
    {
        LOG_INFO("自动上传线程正在上传");

    }

}

/*****************************************************************************
*                       将识别结果存储到数据库函数
*  函 数 名：SaveRecogToDB
*  功    能：将识别结果存储到数据库
*  说    明：
*  参    数：
*  返 回 值：
*  创 建 人：刘中昌
*  创建时间：2014-04-02
*  修 改 人：
*  修改时间：
*****************************************************************************/
void DialogIdentify::SaveRecogToDB(PeopleInfo& person,int recType)
{
    Exectime etm(__FUNCTION__);
    int personRecMaxID = 0;

    Transaction trn(*(_dbAccess.GetSession()));

    _dbAccess.Query("select max(reclogid) from personreclog; ",personRecMaxID);
    if(personRecMaxID>0 && (personRecMaxID%RecogMax == 0))
    {
        std::ostringstream backup;
        backup<<" select * into ";
        backup<<" recog";
        backup<<QDateTime::currentDateTime().toString("yyyyMMdd").toStdString();
        backup<<" from personreclog;  delete from personreclog;";
        _dbAccess.Query(backup.str());
    }
    PersonRecLog personRec;
    personRec.recLogID = personRecMaxID + 1;
    personRec.irisDataID = person.FeatureID;
    personRec.personID = person.Id;
    personRec.devType = 6;//上下班
    personRec.uploadstatus =0;//为上传记录
    personRec.recogType = recType;//虹膜识别
    personRec.devSN = _mac;//暂时为提供mac
    //    personRec.devSeq =1;

    QDateTime recDate = QDateTime::currentDateTime();

    //有点变态，有“：”则存储的图片有的优盘无法到处。
    //用_代替： 则数据库识别记录无法存储，所以按照两个变量处理
    _identDate  = recDate.toString("yyyy-MM-dd_hh_mm_ss");
    //    QString recDate = QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss");
    personRec.recogTime = recDate.toString("yyyy-MM-dd hh:mm:ss").toStdString();

    std::ostringstream oss;
    oss<<" INSERT INTO personreclog ( reclogid,  personid,  irisdataid, recogtime, recogtype,  atpostion, ";
    oss<<"  devsn, devtype,uploadstatus)  values ( :reclogid,  :personid,  :irisdataid, :recogtime,";
    oss<<" :recogtype,  :atpostion,  :devsn, :devtype,:uploadstatus); ";

    if(dbSuccess != _dbAccess.Insert(oss.str(),personRec))
    {
        trn.Rollback();
        std::cout<< GetLogHead(__FUNCTION__) <<"识别记录存储数据库错误！识别姓名： "<< person.Name << "设备序列号: "  << _mac <<std::endl;
    }
    else
    {
        trn.Commit();
    }
}
/*****************************************************************************
*                       人员工号SQL查询函数
*  函 数 名：WorkSnQueryFromIris
*  功    能：动态获得人员工号的SQL查询语句－－通过人员信息
*  说    明：
*  参    数：
*  返 回 值：
*  创 建 人：fjf
*  创建时间：2015-08-27
*  修 改 人：
*  修改时间：
*****************************************************************************/
QString DialogIdentify::WorkSnQueryFromIris(PeopleInfo &person)
{
    Exectime etm(__FUNCTION__);
    QString query = "";

    //此处为原有的识别模式的人员工号的方法
    query += "select * from person where personid = \'";
    UuidControl uc;
    query += uc.StringFromUuid(person.Id).c_str();
    query +="\' ; ";

    return query;
}
/*****************************************************************************
*                       获得人员工号函数
*  函 数 名：WorkSnQueryFromCard
*  功    能：动态获得人员工号的SQL查询语句－－通过卡号
*  说    明：
*  参    数：
*  返 回 值：
*  创 建 人：fjf
*  创建时间：2015-08-27
*  修 改 人：
*  修改时间：
*****************************************************************************/
QString DialogIdentify::WorkSnQueryFromCard()
{
    Exectime etm(__FUNCTION__);
    QString query = "";
    query = "select * from person where cardid = \'";
    query += _curCardID;
    query +="\' ; ";

    return query;
}
/*****************************************************************************
*                       发送韦根26
*  函 数 名：SendWigan26
*  功    能：发送韦根26－－通过卡号
*  说    明：
*  参    数：
*  返 回 值：
*  创 建 人：gdj
*  创建时间：2017-08-18
*  修 改 人：
*  修改时间：
*****************************************************************************/
bool DialogIdentify::SendWigan26(std::string cardid)
{
    if(QString::fromStdString(cardid).trimmed().isEmpty())
    {
        LOG_ERROR("发送韦根26时，卡号为空");
        return false ;
    }

    QString vaildCardIDSec = ConvertCardNumToHex(cardid.c_str());
    ulong cardID = vaildCardIDSec.toULong(NULL, 16);

    LOG_INFO("发送韦根卡号%s", cardid.c_str());

    printf("%scardid to wigan: (Dec) %d   (Hex)%s\n",
           GetLogHead(__FUNCTION__).c_str(), cardID, vaildCardIDSec.toStdString().c_str());
    try
    {
        unsigned char wigan[4];
        wigan[0] = 0x0a;
        wigan[1] = (cardID & 0x00FF0000)>>16;
        wigan[2] = (cardID & 0x0000FF00)>>8;
        wigan[3] = (cardID & 0x000000FF);
        //usleep(50000);
        //向韦根发卡号信息
        //jlg 改动了优先级，所以将原来的高优先级换为低优先级
        int err = _sm->Send(wigan,4,PriorityLevels::LowPriority);

        LOG_INFO("%s向韦根26发卡号信息完成！cardID: %d.%d.%d\n", GetLogHead(__FUNCTION__).c_str(), wigan[1], wigan[2], wigan[3]);

    }catch(exception e)
    {
        LOG_ERROR("卡号转换失败");
        std::cout <<" 卡号转换失败！"<<std::endl;
    }


}
/*****************************************************************************
*                       发送韦根34
*  函 数 名：SendWigan34
*  功    能：发送韦根34－－通过卡号
*  说    明：
*  参    数：
*  返 回 值：
*  创 建 人：gdj
*  创建时间：2017-08-18
*  修 改 人：
*  修改时间：
*****************************************************************************/
bool DialogIdentify::SendWigan34(std::string cardid)
{
    if(QString::fromStdString(cardid).trimmed().isEmpty())
    {
        LOG_ERROR("发送韦根34时，卡号为空");
        return false ;
    }

    //只截取十六进制卡号的最后六个字节
    //string vaildCardIDSec = person.CardID.substr(person.CardID.length() - 6);
    QString vaildCardIDSec = ConvertCardNumToHex(cardid.c_str());
    ulong cardID = vaildCardIDSec.toULong(NULL, 16);

    LOG_INFO("发送韦根卡号%s", cardid.c_str());

    printf("%scardid to wigan: (Dec) %d   (Hex)%s\n",
           GetLogHead(__FUNCTION__).c_str(), cardID, vaildCardIDSec.toStdString().c_str());
    try
    {
        unsigned char wigan[5];
        wigan[0] = 0x0a;
        wigan[1] = (cardID & 0x00FF0000)>>16;
        wigan[2] = (cardID & 0x0000FF00)>>8;
        wigan[3] = (cardID & 0x000000FF);
        //usleep(50000);
        //向韦根发卡号信息
        //jlg 改动了优先级，所以将原来的高优先级换为低优先级
        int err = _sm->Send(wigan,4,PriorityLevels::LowPriority);

        LOG_INFO("%s向韦根34发卡号信息完成！cardID: %d.%d.%d\n", GetLogHead(__FUNCTION__).c_str(), wigan[1], wigan[2], wigan[3]);

    }catch(exception e)
    {
        LOG_ERROR("卡号转换失败");
        std::cout <<" 卡号转换失败！"<<std::endl;
    }
}

/*****************************************************************************
*                       获得人员工号函数
*  函 数 名：GetPersonWorkSn
*  功    能：通过卡号或者普通人员信息得到当前人员的工号
*  说    明：
*  参    数：
*  返 回 值：
*  创 建 人：fjf
*  创建时间：2015-08-26
*  修 改 人：wangl
*  修改时间：2016-01-06
*****************************************************************************/
long long DialogIdentify::GetPersonWorkSn(PeopleInfo &person)
{
    Exectime etm(__FUNCTION__);
    Person detailPerson;
    QString query = "";
    long long workSn = 0;

    //处理直接刷卡通过和其它识别方式的处理不同情况 2015-08-26
    if (_cardoririsMode)
    {
        if ("" == _curCardID || NULL == _curCardID)
        {
            query = this->WorkSnQueryFromIris(person);
        }
        else
        {
            query = this->WorkSnQueryFromCard();
        }
    }
    else
    {
        query = this->WorkSnQueryFromIris(person);
    }

    _dbAccess.Query( query.toStdString(),detailPerson);
    workSn = QString(detailPerson.workSn.c_str()).toLongLong();

    return workSn;
}
/*****************************************************************************
*                       向串口发信号函数
*  函 数 名：SendSingnalToSerial
*  功    能：向串口发送韦根、继电器信号
*  说    明：
*  参    数：
*  返 回 值：
*  创 建 人：刘中昌
*  创建时间：2014-04-02
*  修 改 人：
*  修改时间：
*****************************************************************************/
void DialogIdentify::SendSingnalToSerial(PeopleInfo& person)
{
    Exectime etm(__FUNCTION__);
    //向继电器发信号
    if(_isOutRelay)
    {
        unsigned char relay[4];
        relay[0] = 0x0b;
        relay[1] = 0x03;
        relay[2] = (_minOutRalay & 0x000000FF);
        relay[3] = (_secOutRalay & 0x000000FF);
        //usleep(50000);
        //int err = _sm->Send(relay,4,PriorityLevels::HighPriority);
        int err = _sm->Send(relay,4,PriorityLevels::LowPriority);

        std::cout << GetLogHead(__FUNCTION__) <<  "向继电器发送信号完成(return-value:" << err << ")！"<<std::endl;
    }



    //向韦根发信号
    if(_isOutWigan)
    {
       // SendWigan26(person.CardID);
        if(!QString::fromStdString(person.CardID).trimmed().isEmpty())
        {
            //只截取十六进制卡号的最后六个字节
            //string vaildCardIDSec = person.CardID.substr(person.CardID.length() - 6);
            QString vaildCardIDSec = ConvertCardNumToHex(person.CardID.c_str());
            ulong cardID = vaildCardIDSec.toULong(NULL, 16);

            LOG_INFO("发送韦根卡号%s", person.CardID.c_str());

            printf("%scardid to wigan: (Dec) %d   (Hex)%s\n",
                   GetLogHead(__FUNCTION__).c_str(), cardID, vaildCardIDSec.toStdString().c_str());
            try
            {
                //2015-08-26 fjf 处理当前卡号
                //long cardID = _curCardID.toLong();//strtol(detailPerson.cardID.c_str(),0,16);

                unsigned char wigan[4];
                wigan[0] = 0x0a;

                wigan[1] = (cardID & 0x00FF0000)>>16;
                wigan[2] = (cardID & 0x0000FF00)>>8;
                wigan[3] = (cardID & 0x000000FF);
                //usleep(50000);
                //向韦根发卡号信息
                //jlg 改动了优先级，所以将原来的高优先级换为低优先级
                int err = _sm->Send(wigan,4,PriorityLevels::LowPriority);

                LOG_INFO("%s向韦根发卡号信息完成！cardID: %d.%d.%d\n", GetLogHead(__FUNCTION__).c_str(), wigan[1], wigan[2], wigan[3]);

            }catch(exception e)
            {
                //std::cout<<detailPerson.name <<" 卡号转换失败！"<<std::endl;
                std::cout <<" 卡号转换失败！"<<std::endl;
            }
        }

    }

}

/*****************************************************************************
*                        显示识别结果函数
*  函 数 名：LoginIdentResult
*  功    能：识别结果
*  说    明：
*  参    数：
*  返 回 值：
*  创 建 人：刘中昌
*  创建时间：2013-12-27
*  修 改 人：
*  修改时间：
*****************************************************************************/
void DialogIdentify::loginIdentResult(long personIndex,int flag)
{
    Exectime etm(__FUNCTION__);

    if(personIndex<0)
    {
        //20141215 lizhl
        DrawStatusLabel(StatusIdenFailed);

        _identResultState = FailedState;
    }
    else if(3 == flag)
    {
        _identResultState = ContinueState;
    }
    else
    {
        _identResultState = SuccessState;

        _isLoginSuccess = true;
        emit sigLoginSucceed(_isLoginSuccess);

        //停止识别
        quitIdent();
        return;

    }

    if(!g_isQuitIdent && _identResultState == ContinueState )
    {
        //异步识别识别
        //isStartIdent();
        StartIdentTimer(0);  //??
    }
}



/*****************************************************************************
*                        通过红外结果设置是否有人
*  函 数 名： InteractionStatus
*  功    能：设置是否有人
*  说    明：
*  参    数：
*  返 回 值：
*  创 建 人：刘中昌
*  创建时间：2013-04-01
*  修 改 人：
*  修改时间：
*****************************************************************************/
void DialogIdentify::interactionStatus(bool flag)
{
    Exectime etm(__FUNCTION__);

    SetStatus(!flag);

}

void DialogIdentify::SetUIImg(bool isSleep)
{

    Exectime etm(__FUNCTION__);
    if(isSleep)
    {

    }
    else
    {
        //@comment by dj
        //_identInteraction->ResetDisplay();

        //20141204 lizhl
        _loadMovie->start();
        _loadLabel->setVisible(true);
    }


    //20141204 lizhl
    _waitMovie->stop();
//    _waitLabel->setVisible(false);

    //20150121 Wang.L
    ui->labelSleep->setVisible(false);

}
void doAPS(void)
{
    int re = system("/usr/bin/killall -9 APS &>/dev/null");
    int ret = system("/usr/bin/APS");
}

/*****************************************************************************
*                        根据红外测距是否停止识别函数
*  函 数 名：IdentInteractionStatusCallBack
*  功    能：是否识别
*  说    明：
*  参    数：
*  返 回 值：
*  创 建 人：刘中昌
*  创建时间：2014-3-20
*  修 改 人：guodj
*  修改时间：2016-10-13
*****************************************************************************/
void DialogIdentify::IdentInteractionStatusCallBack(IfHasPerson hasPerson)
{

    Exectime etm(__FUNCTION__);
    _hasPerson = hasPerson;

    if( hasPerson == HasPerson )
    {
        if(_lastPersonStatus == NoPerson)
        {

            //@唤醒SDK
            LOG_INFO("唤醒SDK操作");
            // _isSlept = false ;
            identEvent.onWakeSDK();   //add for sdk


            // for sdk
            //@ for crash
            //跟曹涛沟通过，人脸摄像头休眠时候不会断电，所以唤醒时候无须再创建实例 初始化。
            //            _faceCamer = new CCameraApi();
            //            _faceCamer->Init();

        }
        else
        {
            identEvent.onUIofIden();
            LOG_INFO("close APS");
            std::thread a(doAPS);
            a.detach();
            LOG_INFO("open APS");
        }

    }
    else if(hasPerson == WaitPerson)
    {
        //20141215

        identEvent.onUIofWait();
    }
    else if(hasPerson == NoPerson )
    {
        //_hasPerson = NoPerson; //ahnan 20161027, 多余
        identEvent.onInteractionStatus(false);
        struct timespec slptm;
        slptm.tv_sec = 2;
        slptm.tv_nsec = 1000;      //1000 ns = 1 us
        nanosleep(&slptm,0);
        //@_faceCamer->Release(true);

        //@休眠sdk
        LOG_INFO("休眠SDK操作");
        identEvent.onSleepSDK();

        identEvent.onSetUIImg(true);
        identEvent.onUIofSleep();    //Added by: wangl @20150115
    }
    _lastPersonStatus = _hasPerson;//记录本次状态 20141205 lizhl


}
//ahnan 20161031, 将IKUSBSDK_Init()作为线程
void SDKInit(void *arg)
{
    Exectime etm(__FUNCTION__);
    DialogIdentify *pWnd = static_cast<DialogIdentify* >(arg);
    LOG_INFO("in SDKInit");
    if(NULL != pWnd)
    {
        LOG_INFO("sdk init step 1");
        if(!pWnd->_m_isSDKWake)
        {
            //caotao, 20170510, cpu置为ondemand模式
            cpuFreqSet("ondemand");
            LOG_INFO("sdk init step 2");
            //画面更新 从睡眠到唤醒
            identEvent.onSetUIImg(false);
            LOG_INFO("sdk init step 3");
            int initCount = 0;
            int result  = -1;
            while( (result = IKUSBSDK_Init(&IrisStatusInfoCallback))!= 0)
            {
                LOG_ERROR("初始化失败，尝试再次初始化 result%d",result);
                //释放SDK（）；
                IKUSBSDK_Release();
                initCount ++;
                if(initCount>3)
                {
                    LOG_ERROR("SDK尝试初始化超过3次，停止继续尝试初始化");
                    //向主线成发送信号提示用户
                    identEvent.onInitSDKOutTime();
                    break;
                }
                sleep(2000);
            }
            LOG_INFO("sdk init step 4");
            if(result == 0)
            {
                pWnd->_m_isSDKWake = true;
                //ahnan 20161026, 设置led及camera状态标致
                pWnd->_identInteraction->_irisCameraOnFlag = true;
                pWnd->_identInteraction->_infraredLEDOnFlag = true;
                qDebug()<< "唤醒设备时，SDK 初始化成功！"<<endl;
                LOG_INFO("唤醒设备时，SDK 初始化成功！");
                //ahnan 20161027, 逻辑错误,放在这里更合理
                pWnd->g_isStopIdent = false;
                pWnd->g_isQuitIdent = false;
//                pWnd->isStartIdent();//dj
                LOG_INFO("启动主线成识别函数isStartIdent");
            }
            else
            {
                qDebug()<< "唤醒设备时，SDK 初始化失败！"<<endl;
                LOG_ERROR("唤醒设备时，SDK 初始化失败！result = %d",result);
            }
            LOG_INFO("sdk init step 5");
            identEvent.onUIofIden();
            LOG_INFO("sdk init step 6");
            system("/usr/bin/killall -9 APS &>/dev/null");
            system("/usr/bin/APS");

        }
    }

}

/*****************************************************************************
*                        重新初始化SDK
*  函 数 名：ReinitSDKInKeepSDK
*  功    能：在KeepSDK监测到SDK不工作后重新初始化SDK
*  说    明：
*  参    数：
*  返 回 值：
*  创 建 人：guowen
*  创建时间：2018-01-22
*  修 改 人：
*  修改时间：
*****************************************************************************/
void ReInitSDKInKeepSDK(void *arg)
{
    LOG_INFO("In ReInitSDKInKeepSDK");
    Exectime etm(__FUNCTION__);
    DialogIdentify *pWnd = static_cast<DialogIdentify* >(arg);
    int funFlag, i;

    funFlag = IKUSBSDK_Release();
    if(funFlag == 0) {
        LOG_INFO("SDK释放成功");
    }
    else {
        LOG_ERROR("SDK释放失败ret: %d", funFlag);
    }

    for( i = 0 ; i < 3 ; i ++)
    {
        funFlag = IKUSBSDK_Init( IrisStatusInfoCallback);
        if(funFlag == 0)
        {
            LOG_INFO("SDK初始化成功");
            break;
        }
        else
        {
            LOG_INFO("SDK初始化失败(%d)ret: %d", i+1, funFlag);
            IKUSBSDK_Release();
        }
    }

    if(i >= 3)
    {
        pWnd->sendReInitSDKResult(false);
    }
    else
    {
        //初始化SDK完成标志置位
        g_isSDKReInited = true;
        pWnd->sendReInitSDKResult(true);
    }
}

/*****************************************************************************
*                        重新初始化SDK处理槽函数
*  函 数 名：slotReInitSDKResult
*  功    能：处理重新初始化SDK返回结果
*  说    明：
*  参    数：
*  返 回 值：
*  创 建 人：guowen
*  创建时间：2018-01-22
*  修 改 人：
*  修改时间：
*****************************************************************************/
void DialogIdentify::slotReInitSDKResult(bool result)
{
    if(result) {
        //关闭等待界面
        _waitSdkInitMovie->stop();
        _waitSdkInitLable->setVisible(false);
        StartIdent();
        LOG_INFO("启动识别");
    }
    else {
        LOG_INFO("提示用户摄像头异常，断电重启");
        IKMessageBox erro(this,QString::fromLocal8Bit("设备异常！"),QString::fromLocal8Bit("摄像头连接异常，请按确定重新启动设备"),questionMbx);
        erro.exec();
        system("reboot");
    }
}

/*****************************************************************************
*                        发送重新初始化SDK结果信号函数
*  函 数 名：sendReInitSDKResult
*  功    能：发送重新初始化SDK结果
*  说    明：
*  参    数：
*  返 回 值：
*  创 建 人：guowen
*  创建时间：2018-01-22
*  修 改 人：
*  修改时间：
*****************************************************************************/
void DialogIdentify::sendReInitSDKResult(bool result)
{
    emit sigReInitSDKResult(result);
}

/*****************************************************************************
*                        唤醒设备
*  函 数 名：WakeDevice
*  功    能：将设备、sdk相关算法、摄像头唤醒，进入工作状态
*  说    明：
*  参    数：
*  返 回 值：
*  创 建 人：guodj
*  创建时间：2016-10-13
*  修 改 人：caotao
*  修改时间：20161031
*****************************************************************************/
void DialogIdentify::slotWakeDevice()
{
    Exectime etm(__FUNCTION__);

    _isSlept = false;
    LOG_INFO("befor creat wake sdk thread");
    std::thread sdkInit(SDKInit, this);
    sdkInit.detach();
}

/*****************************************************************************
*                        释放设备
*  函 数 名：slotReleaseDevice
*  功    能：将设备、sdk相关算法、摄像头释放，进入睡眠
*  说    明：
*  参    数：
*  返 回 值：
*  创 建 人：guodj
*  创建时间：2016-10-13
*  修 改 人：
*  修改时间：
*****************************************************************************/
void DialogIdentify::slotReleaseDevice()
{
    Exectime etm(__FUNCTION__);
    LOG_DEBUG("slotReleaseDevice 1 _m_isSDKWake: %d", _m_isSDKWake);

    if(_m_isSDKWake)
    {
        LOG_DEBUG("slotReleaseDevice 2 _m_isSDKWake: %d", _m_isSDKWake);

        StopIdent();
        int ret = -1;
        if(0 == (ret = IKUSBSDK_Release()))
        {
            qDebug()<<"休眠设备时 SDK 释放成功！"<<endl;
            LOG_INFO("休眠设备时 SDK 释放成功！");
            _m_isSDKWake = false;
            _identInteraction->_irisCameraOnFlag = false;
            _identInteraction->_infraredLEDOnFlag = false;
            //caotao, 20170510, cpu降频
            cpuFreqSet("powersave");
        }
        else
        {
            LOG_ERROR("IKUSBSDK_Release failed ret: %d", ret);
            usleep(2000000);
            if(0 == IKUSBSDK_Release())
            {
                LOG_INFO("休眠设备时 SDK 释放成功！");
                _m_isSDKWake = false;
                _identInteraction->_irisCameraOnFlag = false;
                _identInteraction->_infraredLEDOnFlag = false;
                //caotao, 20170510, cpu降频
                cpuFreqSet("powersave");
            }

            else
            {
                LOG_WARN("休眠设备时 SDK 释放失败！");
            }
        }
    }

}
/*****************************************************************************
*                        设置是否进入识别
*  函 数 名：SetStatus
*  功    能：设置是否进入识别状态
*  说    明：
*  参    数：
*  返 回 值：
*  创 建 人：刘中昌
*  创建时间：2014-3-28
*  修 改 人：
*  修改时间：
*****************************************************************************/
void DialogIdentify::SetStatus(bool isStop)
{
    Exectime etm(__FUNCTION__);
    //_stopIdent = isStop;

    if(isStop)
    {
        StopIdent();
        std::cout<<"识别关闭！"<<std::endl;
        LOG_INFO("识别关闭");


    }
    else
    {
        std::cout<<"StartIdent() 识别开始！"<<std::endl;
        if(!g_isStopIdent)
        {
            LOG_INFO("StartIdent() 正在识别中！");
            return;
        }

        isStartIdent();

        //StartIdentTimer();

    }
}

/*****************************************************************************
*                        初始化串口函数
*  函 数 名：InitSerialMuxDemux
*  功    能：初始化串口
*  说    明：
*  参    数：
*  返 回 值：
*  创 建 人：liuzhch
*  创建时间：2014-04-02
*  修 改 人：
*  修改时间：
*****************************************************************************/
void DialogIdentify::InitSerialMuxDemux()
{
    Exectime etm(__FUNCTION__);

    if(_sm && !_sm->Init(GetSerialPortName()))
    {
        std::cout<<"初始化串口失败！"<<std::endl;
        LOG_DEBUG("初始化串口失败！");
        return;
    }
}

/*****************************************************************************
*                        设置串口回调函数
*  函 数 名：SetSerialCallBack
*  功    能：注册读取IC卡的回调函数
*  说    明：
*  参    数：
*  返 回 值：
*  创 建 人：liuzhch
*  创建时间：2014-04-02
*  修 改 人：
*  修改时间：
*****************************************************************************/
void DialogIdentify::SetSerialCallBack(bool isAdd)
{
    Exectime etm(__FUNCTION__);
    unsigned char cmd = 0x0f;

    if(isAdd)
    {

        if(_sm && !_sm->AddCallback(cmd, RecvCardCallBack))
        {
            std::cout<<"串口分发类注册回调函数失败！"<<std::endl;
            LOG_ERROR("串口分发类注册回调函数失败！");
            return;
        }
        else
        {

            std::cout<<"向串口注册cardID回调函数成功！"<<std::endl;
        }
    }
    else
    {
        if(_sm && !_sm->RemoveCallback(cmd))
        {
            std::cout<<"串口分发类取消回调函数失败！"<<std::endl;
            LOG_ERROR("串口分发类取消回调函数失败！");
            return;
        }
        else
        {
            std::cout<<"向串口取消cardID回调函数成功！"<<std::endl;
        }
    }
}
/*****************************************************************************
*                        发送格式化的卡号函数
*  函 数 名：SendFormatCardNum
*  功    能：格式卡号并发送信号
*  说    明：
*  参    数：
*  返 回 值：
*  创 建 人：fjf
*  创建时间：2015-08-26
*  修 改 人：
*  修改时间：
*****************************************************************************/
void DialogIdentify::SendFormatCardNum(const unsigned char* data, const int size)
{
    Exectime etm(__FUNCTION__);
    QString tempt = QString(QLatin1String(data));
    QString val, s;
    if(nullptr != data)
    {
        for(int i = 0; i < size; i++)
        {
            //TO DO
            val += s.sprintf("%02x", data[i]);
        }
    }
    std::cout<<"进入_cardoririsMode刷卡识别回调-> data:"<<val.toStdString()<<std::endl;
    identEvent.onIdentCardID(val);
}
/*****************************************************************************
*                        串口回调函数
*  函 数 名：RecvCardCallBack
*  功    能：注册读取IC卡的回调函数
*  说    明：
*  参    数：
*  返 回 值：
*  创 建 人：liuzhch
*  创建时间：2014-04-02
*  修 改 人：
*  修改时间：
*****************************************************************************/
void DialogIdentify::RecvCardCallBack(const unsigned char* data, const int size)
{
    Exectime etm(__FUNCTION__);
    // QString sdata = QString(QLatin1String(data));
    std::cout<<"IC卡的回调函数时间:"
            <<QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss").toStdString()
           <<"  "<<std::endl;


    /// card or iris
    if (_cardoririsMode && !g_isQuitIdent && nullptr != data && size >0)
    {
        SendFormatCardNum(data,size);

    }

    /// card and iris
    else if(!_defaultIdentMode && !g_isQuitIdent && nullptr != data && size >0)
    {
        SendFormatCardNum(data,size);
        isCarded = 1;

    }
    else
    {
        isCarded = 0;
        std::cout<<"_defaultIdentMode:"<<_defaultIdentMode<<" g_isQuitIdent :"<<g_isQuitIdent<<" size : "<<size<<std::endl;
    }
}
/*****************************************************************************
*                        通过CardId识别后直接打开门禁函数
*  函 数 名：AllowAccess
*  功    能：在人员刷卡成功后可以直接通过门禁
*  说    明：
*  参    数：personCardID-十进制IC卡号
*  返 回 值：
*  创 建 人：fjf
*  创建时间：2015-08-26
*  修 改 人：
*  修改时间：
*****************************************************************************/
void DialogIdentify::AllowAccess(QString personCardID)
{
    Exectime etm(__FUNCTION__);
    //保存当前人员的卡号
    _curCardID = personCardID;
    if (_cardoririsMode)
    {
        std::cout<< GetLogHead(__FUNCTION__) << "enter AllowAccess------(CardID): " << personCardID.toStdString() <<std::endl;
        PeopleInfo person;//空变量，只为符合接口函数
        person.CardID = personCardID.toStdString();
        this->SendSingnalToSerial(person);

    }
}
/*****************************************************************************
*                        通过CardId识别函数
*  函 数 名：StartIdentCardID
*  功    能：
*  说    明：
*  参    数：
*  返 回 值：
*  创 建 人：liuzhch
*  创建时间：2014-04-02
*  修 改 人：
*  修改时间：
*****************************************************************************/
void DialogIdentify::StartIdentCardID(QString personCardID)
{
    Exectime etm(__FUNCTION__);
    std::cout<<"enter StartIdentCardID------cardNUM is:"<<personCardID.toStdString()<<std::endl;
    if (_cardoririsMode)
    {
        //-----------------------------------------------------------
        Person detailPerson;
        QString cardidstr = ConvertCardNumToDec(personCardID);
        QString query = "";
        query = "select * from person where cardid = \'";
        query += cardidstr;
        query +="\' ; ";
        _dbAccess.Query( query.toStdString(),detailPerson);
        if(detailPerson.cardID == cardidstr.toStdString())
        {
            DrawStatusLabel(StatusIdenSuccess, QString(detailPerson.name.c_str()));
            this->AllowAccess( cardidstr);
            // _identResultState = SuccessState;

        }
        PeopleInfo recPerson;
        recPerson.Name   = detailPerson.name;
        recPerson.CardID = detailPerson.cardID;
        recPerson.Sex    = detailPerson.sex;
        recPerson.WorkSn = detailPerson.workSn;
        recPerson.Depart = detailPerson.departName;
        detailPerson.personID.Get(recPerson.Id);
        int recType = 2; //识别类型 1 虹膜识别 2 刷卡识别
        SaveRecogToDB(recPerson,recType);

    }
    else
    {
        if(g_isStopIdent)
        {
            //正常识别模式
            _personCardID = personCardID;
            SetStatus(false);
            _countCardMode = 0;

        }
        else
        {
            //刷卡频繁时操作  如果是同一个卡则重新记时
            //如果是另一个人时则先停止识别，重新开启下一次识别
            if(personCardID != _personCardID)
            {
                _personCardID = personCardID;
            }
            if(!_isLoginIdent)
                _countCardMode = 30;
        }
    }
}

// 槽函数，响应sdk初始化失败后给用户提示
void DialogIdentify::slotInitSdkOuttime()
{
    IKMessageBox warnNote(this, QString::fromUtf8("USB初始化失败"), QString::fromUtf8("尝试初始化SDK失败！"), warningMbx);

    warnNote.exec();

}

/*****************************************************************************
*                        串口回调函数
*  函 数 名：RecvRalayCallBack
*  功    能：注册读取继电器回调函数
*  说    明：
*  参    数：
*  返 回 值：
*  创 建 人：liuzhch
*  创建时间：2014-04-14
*  修 改 人：
*  修改时间：
*****************************************************************************/
void DialogIdentify::RecvRalayCallBack(const unsigned char* data, const int size)
{
    Exectime etm(__FUNCTION__);
    std::cout<<"继电器回调函数时间:"
            <<QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss").toStdString()
           <<"  "<<std::endl;
    if(nullptr != data && size >0)
    {
        std::cout<<"继电器回调成功！"<<std::endl;
    }
    else
    {
        std::cout<<"继电器回调失败！"<<std::endl;
        LOG_ERROR("继电器回调失败！");
    }
}

/*****************************************************************************
*                        串口回调函数
*  函 数 名：RecvWiganCallBack
*  功    能：注册韦根回调函数
*  说    明：
*  参    数：
*  返 回 值：
*  创 建 人：liuzhch
*  创建时间：2014-04-14
*  修 改 人：
*  修改时间：
*****************************************************************************/
void DialogIdentify::RecvWiganCallBack(const unsigned char* data, const int size)
{
    Exectime etm(__FUNCTION__);
    std::cout<<"韦根回调函数时间:"
            <<QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss").toStdString()
           <<"  "<<std::endl;
    if(nullptr != data && size >0)
    {
        std::cout<<"韦根别回调成功！"<<std::endl;
    }
    else
    {
        std::cout<<"韦根回调失败！"<<std::endl;
        LOG_ERROR("韦根回调失败！");
    }
}

/*****************************************************************************
*                        释放串口分发类
*  函 数 名：ReleaseSerialMuxDemux
*  功    能：释放串口类
*  说    明：
*  参    数：
*  返 回 值：
*  创 建 人：liuzhch
*  创建时间：2014-04-02
*  修 改 人：
*  修改时间：
*****************************************************************************/
void DialogIdentify::ReleaseSerialMuxDemux()
{
    Exectime etm(__FUNCTION__);
    if(_sm)
    {
        _sm->ReleaseInstance();
        _sm = nullptr;
    }
}


/*****************************************************************************
*                        界面时间更新显示
*  函 数 名：timerEvent
*  功    能：界面时间更新显示
*  说    明：
*  参    数：
*  返 回 值：
*  创 建 人：刘中昌
*  创建时间：2013-10-22
*  修 改 人：gdj
*  修改时间：
*****************************************************************************/
void DialogIdentify::timerEvent(QTimerEvent *event)
{
    Exectime etm(__FUNCTION__);
//    LOG_ERROR("timerEvent...................");
    static int oneSec = 0;
    static int timecnt = 0;
    static int sleeptmcnt = 0;
    //显示时钟数字
    ShowClock();

    //显示加载特征进度条
    ShowLoadProcess();

    //如果休眠则不要显示左边log
    if(_isSlept)
    {
        ShowLeftWgt(false);

    }


    if(oneSec >= 1)
    {
        oneSec = 0;
        return;
    }

    oneSec ++ ;
    {
        lock_guard<mutex> lock(g_keepMain);
        g_isMainAlive = 1;

    }

    //监测重新初始化SDK线程是否完成(guowen添加2018-01-22)
    if(!g_isSDKReInited) {
        if(timecnt >= 60) {
            LOG_INFO("提示用户摄像头异常，断电重启");
            IKMessageBox erro(this,QString::fromLocal8Bit("设备异常！"),QString::fromLocal8Bit("摄像头连接异常，请按确定重新启动设备"),questionMbx);
            erro.exec();
            system("reboot");
        }
        timecnt++;
    }
    else {
        timecnt = 0;
    }

    //监测休眠时间(guowen添加20180124)
    if(!_m_isSDKWake)
        sleeptmcnt++;
    else
        sleeptmcnt = 0;

    static time_t startTime = time(NULL);
    time_t        stopTime  = time(NULL);
    int           elapsed   = stopTime - startTime;
    static int count = 0;
    count ++;

    //连续工作24小时并进入休眠30分钟后重启系统(guowen修改20180124)
    if((elapsed >= 3600*24 || count>=3600*24) && (!_m_isSDKWake) && (sleeptmcnt >= 1800))
    {
        LOG_INFO("system reboot, elapsed=%d, count=%d",elapsed,count);
        system("reboot");
//        qApp->quit();
    }


    ///加个开关，开闹铃的时候不做sdk监控，为了方面调试
    //休眠的时候不再进行sdk守护
    if( _hasPerson == HasPerson && g_isSDKReInited)
    {
        KeepSdk();
    }

    //更新网络状态
    CheckNetworkStatus();

    ui->labCount->setText(QString::fromUtf8("帧率：") + QString::number(_count));
    _count =0;

    //@dj 虹膜登录识别模式则不计刷卡 isCard=0默认为没有刷卡
    if(_isLoginIdent)
    {
        ShowIrisLoginCount(_countDown,_isLoginIdent);
        return ;
    }

    //@dj_defaultIdentMode = true默认为非刷卡模式 false 刷卡模式
    if(!_defaultIdentMode)
    {
        ShowIrisAddCardUI();
        return ;
    }

//    //更新数据K
//    bool uploadStatus = IkSocket::GetIkSocketInstance()->IsUpLoading();
//    //如果自动上传线程正在上传，则跳过，防止主线程阻塞
//    if(!uploadStatus)
//    {
//        if(!IkSocket::GetIkSocketInstance()->isSocketConnet())
//        {
//            LOG_INFO("网络未连接，不实时上传识别记录")
//                    return ;
//        }
//        //LOG_INFO("befor UploadTemp");
//        IkSocket::GetIkSocketInstance()->UploadTemp();
//        //LOG_INFO("after UploadTemp");
//    }
//    else
//    {
//        LOG_INFO("自动上传线程正在上传");

//    }
}


//@dj
void DialogIdentify::ShowIrisLoginCount(int &count,bool isIrisLogin)
{
    if(!isIrisLogin)
    {
        ui->labCountDown->setText(QString::number(30));
        return;
    }
    isCarded = 0;
    //如果计数器为0则返回识别界面；
    if(count == 0 )
    {
        ReIdent();
    }
    else if(count>0)
    {
        ui->labCountDown->setText(QString::number(count));
        count--;
        _countCardMode = 0;
        if( g_isStopIdent)
        {
            isStartIdent();
        }

    }

}

//@dj
void DialogIdentify::ShowIrisAddCardUI()
{
    /// 如果已经刷卡
    if(isCarded == 1  )
    {
        _countCardMode = 30;
        isCarded = 0;
    }

    if( _countCardMode <= 0 )
    {
        if(g_isStopIdent == false)
        {
            StopIdent();
        }

        ui->labStatu->setVisible(!_isSlept);
        ui->labStatu->setStyleSheet("QLabel{border-image:url(:/image/ui_bar_note.png)}");
        QString labText = QString::fromUtf8("请刷卡") ;
        ui->labStatu->setText(labText);

    }
    else if(_countCardMode> 0 )
    {
        _countCardMode--;
        if(_countCardMode <2 )
            return ;
        ui->labStatu->setVisible(!_isSlept);
        ui->labStatu->setStyleSheet("QLabel{border-image:url(:/image/ui_bar_note.png)}");
        QString labText = QString::fromUtf8("请在") + QString::number(_countCardMode)+QString::fromUtf8("s内虹膜识别");
        ui->labStatu->setText(labText);

    }

}

void DialogIdentify::ShowLoadProcess()
{
    static int value = 0;
    if(G_IsLoadingCodeList == 1)
    {
        _loadDataBar->setValue(value%100);
        value +=10;
        if(value >100)
        {
            value = 0;
        }

    }
    else if(G_IsLoadingCodeList == 0)
    {

        _loadDataBar->setValue(100);
        G_IsLoadingCodeList = 2;
    }
    else
    {
        _loadDataBar->setVisible(false);

    }

}

void DialogIdentify::KeepSdk()
{
    static int checkcount = 0;
    if(g_isSdkWorking)
    {
        g_isSdkWorking = false;
        checkcount = 0;
    }
    else
    {
        checkcount ++;
        LOG_ERROR("检测到sdk不工作，第 %d 次",checkcount);
    }
    if(checkcount > 10)
    {
        LOG_ERROR("SDK超过 %d 秒无响应，将重新初始化sdk");
        StopIdent();

        g_isSDKReInited = false;
        LOG_INFO("创建重新初始化SDK线程");
        std::thread sdkInit(ReInitSDKInKeepSDK, this);
        sdkInit.detach();

        checkcount = 0;
        //打开等待界面
        _waitSdkInitMovie->start();
        _waitSdkInitLable->setVisible(true);
        LOG_INFO("打开等待界面");
    }
}

void DialogIdentify::ReInitSDK()
{
    IKUSBSDK_Release();
    int i = 0;
    for( i = 0 ; i < 3 ; i ++)
    {
        int funFlag = IKUSBSDK_Init( IrisStatusInfoCallback);
        LOG_INFO("设备初始化sdk,结果funflag = %d",funFlag);
        if(funFlag == 0)
        {
            break;
        }
        else
        {
            IKUSBSDK_Release();
        }
    }
    if(i>=3)
    {
        //提示用户摄像头异常，断电重启
        IKMessageBox erro(this,QString::fromLocal8Bit("设备异常！"),QString::fromLocal8Bit("摄像头连接异常，请按确定关机，并断开电源，等待10s后连接电源，重新开机"),questionMbx);
        erro.exec();
        if(QDialog::Accepted == erro.result())
        {
            system("poweroff");
        }
        else
        {
            return;
        }
    }
}


void DialogIdentify::CheckNetworkStatus()
{
    Exectime etm(__FUNCTION__);
    //TODO
    static bool preNetworkStatus = false;   //Storage old network status

    //If network status has changed, notify those who care this
    if(IsConnectedToNetwork() != preNetworkStatus)
    {
        preNetworkStatus = !preNetworkStatus;  //更新网络状态
        emit sigNetworkStatusChanged(preNetworkStatus);
    }
}


void DialogIdentify::slotUpdateNetWorkState(bool isOK)
{
    Exectime etm(__FUNCTION__);
    //TODO
    _netStatePlayer.stop();
    if(isOK)
    {
        _netStatePlayer.setFileName(":/image/ui_network_online.gif");
    }
    else
    {
        _netStatePlayer.setFileName(":/image/ui_network_offline.gif");
    }

    ui->labNetStateVal->setMovie(&_netStatePlayer);
    _netStatePlayer.start();

}


/*****************************************************************************
*                        从新返回识别界面
*  函 数 名：ReIdent
*  功    能：界面时间更新显示
*  说    明：
*  参    数：
*  返 回 值：
*  创 建 人：刘中昌
*  创建时间：2014-05-14
*  修 改 人：
*  修改时间：
*****************************************************************************/
void DialogIdentify::ReIdent()
{
    Exectime etm(__FUNCTION__);
    _countDown =-1;
    ui->labCountDown->hide();
    //恢复登录按钮的默认外观和行为
    ui->toolBtnLogin->setStyleSheet("QToolButton#toolBtnLogin{"
                                    "  border-top-left-radius: 3px;"
                                    "  border-bottom-left-radius: 3px;"
                                    "  background-image: url(:/image/ui_login_normal.png);"
                                    "}"
                                    "QToolButton#toolBtnLogin:pressed{"
                                    "  background-image: url(:/image/ui_login_pressed.png);"
                                    "}");
    _isLoginIdent = false;
    g_isQuitIdent = false;
    _isLoginSuccess = false;

}

/*****************************************************************************
*                         注册模块将采集到的图像显示到界面
*  函 数 名：ImageUpdate
*  功    能：采图
*  说    明：
*  参    数：pParam：输入参数
*
*  返 回 值：NULL
*  创 建 人：liuzhch
*  创建时间：2013-12-25
*  修 改 人：
*  修改时间：
*****************************************************************************/
void DialogIdentify::imageUpdate()
{
    Exectime etm(__FUNCTION__);
    if(_isLoginSuccess)
    {
        std::lock_guard<mutex> lock(g_sdkCallBackMutex);
        emit sigShowEnrollCamera(g_enrIdenStatus);

        QImage img;
        ui->labEye->setPixmap(QPixmap::fromImage(img));
        return;
    }

    //状态判断与执行类型处理
    if(_identResultState == SuccessState)
    {
        _identResultState = KeepState;
        StartIdentTimer();
        _interactionResultType = IdenSuccess;
    }
    else if(_identResultState == FailedState)
    {
        _identResultState = KeepState;
        StartIdentTimer();
        _interactionResultType = IdenFailed;
    }
    else if(_identResultState == KeepState)
    {
        _interactionResultType = IdenKeepLastStatus;
    }
    else
    {
        _interactionResultType = ResultUnknown;
    }

    //20141204 lizhl
    if(HasPerson == _hasPerson)
    {
        if(QMovie::NotRunning != _waitMovie->state())
        {
            _waitMovie->stop();
        }

        //@dj
        ifUpdatePosition = (_noticeCount++ % g_noticeNum == 0) ? true : false;


        QImage imageBig ;
        {
            std::lock_guard<mutex> lock (g_sdkCallBackMutex);
            imageBig = DrawTracGif::GetImg(g_enrIdenStatus.imageData, (unsigned int)IK_DISPLAY_IMG_SIZE);
            _distEstimation = g_enrIdenStatus.distEstimation;

        }

        if(!imageBig.isNull())
        {
            ui->labEye->setPixmap(QPixmap::fromImage(imageBig));
            _identInteraction->PlayInteraction(_interactionResultType, _distEstimation,ifUpdatePosition);

            //@重载for sdk guodj
            {
                std::lock_guard<mutex> lock(g_sdkCallBackMutex);
                DrawTraceGif(g_enrIdenStatus);

            }

            //LOG_INFO("ifUpdatePosition%d",ifUpdatePosition);
            if(ifUpdatePosition && ResultUnknown == _interactionResultType)
            {
                //如果需要更新位置信息并且没有结果，则提示状态信息ada
                DrawStatusLabel(StatusAlgIsPerform);
            }
        }
        else
        {
            std::cout<<"识别显示未获得图像！"<<std::endl;
            LOG_ERROR("识别显示未获得图像！");

        }

        if(!ui->labEye->isVisible())
        {
            //_waitLabel->setVisible(false);
            ui->labelSleep->setVisible(false);
            ui->labEye->setVisible(true);
            //20141215 lizhl
            SetTraceVisible(true);
        }

    }
    else
    {
        if(QMovie::NotRunning == _waitMovie->state())
        {
            _waitMovie->start();
        }
        if(!ui->labelSleep->isVisible()/*_waitLabel->isVisible()*/)
        {
            ui->labEye->setVisible(false);

            //Replaced By: Wang.L @20150121
            ui->labelSleep->setVisible(true); // _waitLabel->setVisible(true);

            //20141215 lizhl
            SetTraceVisible(false);
        }

    }

}



/*****************************************************************************
*                        获取人脸照片路径
*  函 数 名：getFacePhotoPath
*  功    能：获取人脸照片存储路径
*  说    明：
*  参    数：
*  返 回 值：
*  创 建 人：liuzhch
*  创建时间：2014-02-18
*  修 改 人：
*  修改时间：
*****************************************************************************/
QString DialogIdentify::GetFacePhotoPath()
{
    Exectime etm(__FUNCTION__);
    QDir dir(QCoreApplication::applicationDirPath());
    QString facePhotoDir("recogface");

    if(!dir.exists(facePhotoDir))
    {
        if(!dir.mkpath(facePhotoDir))
        {
            QMessageBox::warning(this, QString::fromUtf8("创建目录"), QString::fromUtf8("无法创建保存识别成功人员头像照片的目录"));
            std::cout << GetLogHead(__FUNCTION__) << "cannot create face photo storaging directory" << std::endl;
            return QString("");
        }

        int cmdRes = 0;
        cmdRes = system("sudo chmod 777 *");
    }

    return dir.absolutePath() + QDir::separator() + facePhotoDir;
}


/*****************************************************************************
*                        获取人脸照片路径
*  函 数 名：getFacePhotoPath
*  功    能：获取人脸照片存储路径
*  说    明：
*  参    数：
*  返 回 值：
*  创 建 人：liuzhch
*  创建时间：2014-02-18
*  修 改 人：
*  修改时间：
*****************************************************************************/
QString DialogIdentify::GetFacePhotoPath(const QString& subDir)
{
    Exectime etm(__FUNCTION__);
    QDir parDir(QCoreApplication::applicationDirPath());
    QString facePhotoDir("%1%2%3");

    facePhotoDir = facePhotoDir.arg("recogface", QDir::separator(), subDir.isEmpty() ? "unknown" : subDir);
    if(!parDir.exists(facePhotoDir))
    {
        if(!parDir.mkpath(facePhotoDir))
        {
            QMessageBox::warning(this, QString::fromUtf8("创建目录"), QString::fromUtf8("无法创建保存识别成功人员头像照片的目录"));
            std::cout << GetLogHead(__FUNCTION__) << "cannot create face photo storaging directory" << std::endl;
            return QString("");
        }

        int cmdRes = 0;
        cmdRes = system("sudo chmod 777 *");
    }


    return parDir.absolutePath() + QDir::separator() + facePhotoDir;
}


/*****************************************************************************
*                        获取人脸照片路径
*  函 数 名：getFacePhotoPath
*  功    能：获取人脸照片存储路径
*  说    明：未启用
*  参    数：
*  返 回 值：
*  创 建 人：liuzhch
*  创建时间：2014-02-18
*  修 改 人：
*  修改时间：
*  备注    ： 该函数已经取消使用
*****************************************************************************/
QString DialogIdentify::GetFacePhotoSonPath(int index)
{
    Exectime etm(__FUNCTION__);
    QString facePhotoDir1("Monday");
    QString facePhotoDir2("Tuesday");
    QString facePhotoDir3("wednessday");
    QString facePhotoDir4("Thursday");
    QString facePhotoDir5("Friday");
    QString facePhotoDir6("Saturday");
    QString facePhotoDir7("Sunday");

    QDir dir(QDir::currentPath());

    QDate date;
    date.currentDate().dayOfWeek();
    dir.cd(GetFacePhotoPath());
    switch(index)
    {
    case 1:
        if(!dir.exists(facePhotoDir1))
        {
            if(!dir.mkdir(facePhotoDir1))
            {
                QMessageBox::warning(this, QString::fromUtf8("创建目录"), QString::fromUtf8("无法创建保存识别成功人员头像照片的目录"));
            }
        }
        dir.cd(facePhotoDir1);
        return dir.dirName();
        break;
    case 2:
        if(!dir.exists(facePhotoDir2))
        {
            if(!dir.mkdir(facePhotoDir2))
            {
                QMessageBox::warning(this, QString::fromUtf8("创建目录"), QString::fromUtf8("无法创建保存识别成功人员头像照片的目录"));
            }
        }

        dir.cd(facePhotoDir2);
        return dir.dirName();
        break;
    case 3:
        if(!dir.exists(facePhotoDir3))
        {
            if(!dir.mkdir(facePhotoDir3))
            {
                QMessageBox::warning(this, QString::fromUtf8("创建目录"), QString::fromUtf8("无法创建保存识别成功人员头像照片的目录"));
            }
        }

        dir.cd(facePhotoDir3);
        return dir.dirName();
        break;
    case 4:
        if(!dir.exists(facePhotoDir4))
        {
            if(!dir.mkdir(facePhotoDir4))
            {
                QMessageBox::warning(this, QString::fromUtf8("创建目录"), QString::fromUtf8("无法创建保存识别成功人员头像照片的目录"));
            }
        }

        dir.cd(facePhotoDir4);
        return dir.dirName();
        break;
    case 5:
        if(!dir.exists(facePhotoDir5))
        {
            if(!dir.mkdir(facePhotoDir5))
            {
                QMessageBox::warning(this, QString::fromUtf8("创建目录"), QString::fromUtf8("无法创建保存识别成功人员头像照片的目录"));
            }
        }

        dir.cd(facePhotoDir5);
        return dir.dirName();
        break;
    case 6:
        if(!dir.exists(facePhotoDir6))
        {
            if(!dir.mkdir(facePhotoDir6))
            {
                QMessageBox::warning(this, QString::fromUtf8("创建目录"), QString::fromUtf8("无法创建保存识别成功人员头像照片的目录"));
            }
        }

        dir.cd(facePhotoDir6);
        return dir.dirName();
        break;
    case 7:
        if(!dir.exists(facePhotoDir7))
        {
            if(!dir.mkdir(facePhotoDir7))
            {
                QMessageBox::warning(this, QString::fromUtf8("创建目录"), QString::fromUtf8("无法创建保存识别成功人员头像照片的目录"));
            }
        }

        dir.cd(facePhotoDir7);
        return dir.dirName();
        break;
    default:
        break;
    }
    return dir.dirName();
}


/*****************************************************************************
*                        获取人脸照片备份路径
*  函 数 名：GetFacePhotoBackUpPath
*  功    能：获取人脸照片备份目录存储路径
*  说    明：未启用
*  参    数：
*  返 回 值：
*  创 建 人：wangl
*  创建时间：2015-03-13
*  修 改 人：
*  修改时间：
*  备注    ： 该函数已经取消使用
*****************************************************************************/
QString DialogIdentify::GetFacePhotoBackUpPath()
{
    Exectime etm(__FUNCTION__);
    QDir parDir(QCoreApplication::applicationDirPath());
    std::cout << GetLogHead(__FUNCTION__) << "parent directory: " << parDir.dirName().toStdString() << std::endl;

    QString targetDir("recogFaceBK/%1");

    targetDir = targetDir.arg(QDateTime::currentDateTime().toString("before_yyyy_MM_dd"));
    if(!parDir.exists(targetDir))
    {
        if(!parDir.mkpath(targetDir))
        {
            std::cout << GetLogHead(__FUNCTION__) << "cannot create face photo backup directory" << std::endl;
            return QString("");
        }
    }

    //Without the following, this new directory cannot get written access
    int cmdRes = 0;
    cmdRes = system("sudo chmod 777 *");

    return parDir.absolutePath() + QDir::separator() + targetDir;
}

/*****************************************************************************
*                        备份人脸照片
*  函 数 名：BackUpFacePhoto
*  功    能：将人脸图像备份到备份目录中
*  说    明：如果人脸图像目录中的照片超过3000幅，则将该目录中的所有照片移至备份目录中
*  参    数：
*  返 回 值：
*  创 建 人：wangl
*  创建时间：2015-03-13
*  修 改 人：
*  修改时间：
*  备    注：
*****************************************************************************/
bool DialogIdentify::BackUpFacePhoto()
{
    Exectime etm(__FUNCTION__);
    QDir dir(GetFacePhotoPath());
    if(dir.entryList(QDir::Files).size() >= 3000)
    {
        QString dstDir(GetFacePhotoBackUpPath());
        if(dstDir.isEmpty())
        {
            std::cout << GetLogHead(__FUNCTION__) << "can not get the backup directory of face photos  "  << dstDir.toStdString() << std::endl;
            LOG_ERROR("can not get the backup directory of face photos");
            return false;
        }

        std::cout << GetLogHead(__FUNCTION__) << "moving face photos to backup directory "  << dstDir.toStdString() << std::endl;
        int cmdRes = 0;
        cmdRes = system(QString("sudo chown -hR irisking %1/*;mv -f %1/* %2").arg(dir.dirName(), dstDir).toStdString().c_str());
    }
    //    else
    //    {
    //        std::cout << GetLogHead(__FUNCTION__) << "no need to backup face photos" << std::endl;
    //    }

    return true;
}

//采集人脸图像独立为一个线程，李志林
void* DialogIdentify::ThreadCapFaceImg(DialogIdentify *pParam)
{
    /*
    Exectime etm(__FUNCTION__);
    int funResult = E_ALGAPI_DEFAUL_ERROR;
    QString resultName;

    while(1)
    {
        {
            std::unique_lock <std::mutex> lck(pParam->_capFaceImgMutex);
            while(!pParam->_capFaceFlag)
            {
                pParam->_capFaceImgCV.wait(lck);
            }
            pParam->_capFaceFlag = false;
        }


        //识别通过后保存人脸图像
        if(nullptr != pParam->_faceCamer)
        {
            //等到条件信号后，调用采图函数
            //@??为什么 5次？
            for(int n = 0; n < 5; n++)
            {
                funResult = pParam->_faceCamer->CapImg(pParam->_facephotoBuf);
            }

            if(!funResult)
            {
                pParam->BackUpFacePhoto();     //转储人脸图像 Added by:wanl @20150312

                resultName = "F";
                if(pParam->_isIdenSuccess)
                {
                    resultName = pParam->_idenName + "T";

                }


                QImage image = QImage(pParam->_facephotoBuf, pParam->_facephotoSize.ImgWidth,
                                      pParam->_facephotoSize.ImgHeight, QImage::Format_RGB16);

                image = image.mirrored(false, true);
                image.save(pParam->GetFacePhotoPath() + "/"
                           + pParam->_identDate + resultName + ".jpg","JPG");
                QString imagename = pParam->GetFacePhotoPath() + "/"
                        + pParam->_identDate + resultName;
                LOG_INFO("imagename = %s",imagename.toStdString().c_str());

            }
            else
            {
                std::cout << GetLogHead(__FUNCTION__) << "人脸图像采集获取图片失败！" << std::endl;
                LOG_ERROR("人脸图像采集获取图片失败！");
            }
        }
    }
    */

    return (void *)0;
}

//20141212 lizhl 增加识别界面的人机交互函数
void DialogIdentify::InitLabelStatus()
{
    //20141215
    //ui->labStatu->setVisible(false);
    ui->labStatu->setText(QString::fromLocal8Bit("特征加载中.."));
    ui->labStatu->setVisible(true);

}

void DialogIdentify::InitLabelMask()
{
    Exectime etm(__FUNCTION__);

    //@dj  初始化识别界面
    ui->labBorder->setGeometry(QRect(g_imgBeginX, g_imgBeginY, g_imgWidth, g_imgHeight));
    ui->labEye->setGeometry(QRect(g_imgBeginX, g_imgBeginY, g_imgWidth, g_imgHeight));

    _labMask = new QLabel(this,/*Qt::SplashScreen | */Qt::WindowStaysOnTopHint/* | Qt::X11BypassWindowManagerHint*/ );
    //    _labMask->setGeometry(g_imgBeginX, g_imgBeginY, g_imgWidth, g_imgHeight);//178,104,664,498

    _labMask->setGeometry(g_imgBeginX, g_imgBeginY, g_imgWidth, g_imgHeight);//178,104,664,498

    QImage image(":/image/ui_mask_green.png");
    image.scaled(g_imgWidth, g_imgHeight);
    QPixmap pic1(QPixmap::fromImage(image));

    //设置图片透明度
    //        QPixmap* alpic1=new QPixmap(pic1.size());
    //        alpic1->fill(QColor(128,128,0,200));
    //        pic1.setAlphaChannel(*alpic1);
    _labMask->setPixmap(pic1);
}

void DialogIdentify::InitTraceGif()
{
    Exectime etm(__FUNCTION__);

    // 大军从新疆发来的快报  begin
    Lx = g_defaultTraceLeftBeginX + g_defaultTraceSize/2;                   //170
    // Ly = g_imgHeight - (g_defaultTraceLeftBeginY + g_defaultTraceSize/2)-30; //268
    Ly = g_imgHeight/2 ;
    Lr = 120;
    Rx = g_defaultTraceRightBeginX + g_defaultTraceSize/2;
    Ry = g_imgHeight/2 ;
    Rr = 120;
    // 大军从新疆发来的快报  end

    _movTraceL = new QMovie(":/image/ui_trace_r.gif");
    _movTraceL->setScaledSize(QSize(g_defaultTraceSize, g_defaultTraceSize));

    _labTraceL = new QLabel(this);
    _labTraceL->setMovie(_movTraceL);
    _labTraceL->setGeometry(int(Lx*g_scaling) - g_defaultTraceSize/2  + g_imgBeginX,
                            int(Ly*g_scaling) - g_defaultTraceSize/2  + g_imgBeginY,
                            g_defaultTraceSize,
                            g_defaultTraceSize);

    _movTraceR = new QMovie(":/image/ui_trace_r.gif");
    _movTraceR->setScaledSize(QSize(g_defaultTraceSize, g_defaultTraceSize));

    _labTraceR = new QLabel(this);
    _labTraceR->setMovie(_movTraceR);
    _labTraceR->setGeometry(int(Rx*g_scaling) - g_defaultTraceSize/2  + g_imgBeginX,
                            int(Ry*g_scaling) - g_defaultTraceSize/2  + g_imgBeginY,
                            g_defaultTraceSize,
                            g_defaultTraceSize);
    //20141210
    _isClearL = _isClearR = _isLastClearL = _isLastClearR = false;
    SetTraceVisible(true);
}

void DialogIdentify::InitIdentWindow()
{
    Exectime etm(__FUNCTION__);
    QPixmap* IdentWindow =new QPixmap(g_imgWidth,g_imgHeight);
    IdentWindow->fill(QColor(255,255,255,0));
    QPainter painter(IdentWindow);

    QPen pen(QColor(255,255,255,0), 6, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin);
    painter.setPen(pen);

    QPoint centL(int(Lx*g_scaling) ,int(Ly*g_scaling) );
    QPoint centR(int(Rx*g_scaling) ,int(Ry*g_scaling) );

    QPainterPath path;
    path.addEllipse(centL,Lr,Lr);
    path.addEllipse(centR,Rr,Rr);
    path.addRect(0,0,g_imgWidth-4,g_imgHeight-6);
    //painter.setBrush(QBrush(QColor(0,123,247,180)));
    //painter.setBrush(QBrush(QColor(0,123,247,180),Qt::Dense5Pattern));
    //painter.setBrush(QBrush(Qt::black,Qt::Dense3Pattern));
    painter.setBrush(QBrush(QColor(230,243,206,80),Qt::Dense2Pattern));
    path.setFillRule(Qt::OddEvenFill);
    painter.drawPath(path);
    painter.end();
    qDebug()<<"Lxy="<<Lx*g_scaling <<","<<Ly*g_scaling<<";"<<" Rxy= "<<Rx*g_scaling<<", "<<Ry*g_scaling<<endl;

    ui->label->setGeometry(g_imgBeginX, g_imgBeginY, g_imgWidth, g_imgHeight);
    ui->label->setPixmap(*IdentWindow);

    //show loaddata progressbar
    G_IsLoadingCodeList = 2;
    _loadDataBar = new QProgressBar(this);
    _loadDataBar->setGeometry(g_imgBeginX+100,g_imgBeginY+g_imgHeight - 50,g_imgWidth-200,20);

    _loadDataBar->setStyleSheet("QProgressBar{color:black;font:16px;text-align:center;}"
                                "QProgressBar::chunk{background-color:blue}");
    _loadDataBar->setFormat("Iris Loading··· ");

}

//20141208
void DialogIdentify::SetTraceVisible(bool isvisible)
{
    Exectime etm(__FUNCTION__);
    if(isvisible)
    {
        _movTraceL->setFileName(":/image/ui_trace_r.gif");
        _movTraceL->restart();
        _labTraceL->setVisible(true);

        _movTraceR->setFileName(":/image/ui_trace_r.gif");
        _movTraceR->restart();
        _labTraceR->setVisible(true);
    }
    else
    {
        _movTraceL->stop();
        _labTraceL->setVisible(false);
        _movTraceR->stop();
        _labTraceR->setVisible(false);
    }
}

void DialogIdentify::DrawStatusLabel(StatusLabelType type, QString name/* = ""*/)
{
    Exectime etm(__FUNCTION__);

    IKIrisDistFlag irisPosFlag =  _distEstimation;
#ifdef IKUseDistSensorForSelfPos
    int reddistance = Interaction::s_infraredDistanceValue;
    if(reddistance > maxRecSelfPosDist)
    {
        irisPosFlag = DistTooFar;
    }
    else if(reddistance < minRecSelfPosDist)
    {
        irisPosFlag = DistTooClose;
    }
#endif
    switch(type)
    {
    case StatusAlgIsPerform:
        if(DistTooFar == irisPosFlag)
        {
            ui->labStatu->setVisible(true);
            ui->labStatu->setStyleSheet("QLabel{border-image:url(:/image/ui_bar_get_closer.png)}");
            ui->labStatu->setText("请靠近一点");
        }
        else if(DistTooClose == irisPosFlag)
        {
            ui->labStatu->setVisible(true);
            ui->labStatu->setStyleSheet("QLabel{border-image:url(:/image/ui_bar_get_further.png)}");
            ui->labStatu->setText("请远一点");
        }
        else
        {
            ui->labStatu->setVisible(false);
        }
        break;

    case StatusIdenSuccess:
        ui->labStatu->setVisible(true);
        ui->labStatu->setStyleSheet("QLabel{border-image:url(:/image/ui_bar_ok.png)}");
        ui->labStatu->setText(name);
        break;

    case StatusIdenFailed:
        ui->labStatu->setVisible(true);
        ui->labStatu->setStyleSheet("QLabel{border-image:url(:/image/ui_bar_error.png)}");
        ui->labStatu->setText(QString::fromUtf8("请再试一次"));
        break;
    case StatusCardAndIri:

    case StatusIdenStop:
    case StatusIdenWait:
    default:
        ui->labStatu->setVisible(false);
        break;
    }
}


void DialogIdentify::DrawTraceGif(IKEnrIdenStatus enrIdenStatus)
{
    Exectime etm(__FUNCTION__);
    int Lx,Ly,Rx,Ry;                         //定义左右眼xy坐标
    //获取左右眼虹膜坐标
    DrawTracGif::GetTracPosition(enrIdenStatus,Lx,Ly,Rx,Ry);

    // 存在的问题：没有单眼是否清晰的标志，所以左右眼是否清晰会显示效果会互相影响
    //显示左眼跟踪
    DrawTracGif::ShowTraceGift(enrIdenStatus,_movTraceL,_labTraceL,Lx,Ly,_isLastClearL);
    //显示右眼跟踪
    DrawTracGif::ShowTraceGift(enrIdenStatus,_movTraceR,_labTraceR,Rx,Ry,_isLastClearR);

}


void DialogIdentify::slotUIofIden()
{
    Exectime etm(__FUNCTION__);
   // isStartIdent();
    StartIdentTimer(100);
    _loadMovie->stop();
    _loadLabel->setVisible(false);
    _labMask->setVisible(true);

    _isSlept = false;

    //喚醒界面
    WakeFromSleep();

}

void DialogIdentify::slotUIofWait()
{
    //Exectime etm(__FUNCTION__);
    //Commented off By: Wang.L @20150121
    //ui->labelSleep->setVisible(false);

    StopIdent(); //停止识别

    DrawStatusLabel(StatusIdenWait);

    _labMask->setVisible(false);

    if(ui->labEye->isVisible())
    {

        ui->labEye->setVisible(false);
    }


    SetTraceVisible(false);
    _isSlept = true ;
    //@wangl 20150110
    SetOnSleep();
    //@wangl end
}

void DialogIdentify::slotUIofSleep()
{
    SetTraceVisible(false);
    setStyleSheet("QFrame#centralWgt"
                  "{"
                  "   background-image: url(:/image/ui_background_on_sleep.png);"
                  "}");

}

//Added by wang.l @2015-01-10
void DialogIdentify::SetOnSleep()
{

    //Hide invisible widgets
    ShowLeftWgt(false);

    ui->wgtRight->hide();

    ui->labBorder->hide();
    ui->label->setVisible(false);
    //Set backgroud image
    setStyleSheet("QFrame#centralWgt"
                  "{"
                  "   background-image: url(:/image/ui_background_on_wait.png);"
                  "}");

    //Start timer for hipping and hopping time widgets
    _timeCoordinateTimer.setSingleShot(false);
    _timeCoordinateTimer.start(30);

    //Relayout date time widgets
    ui->wgtTime->setGeometry(90, 7, 380, 94);
    ui->labDate->setGeometry(821, 560, 190, 30);
}


//喚醒界面
void DialogIdentify::WakeFromSleep()
{
    Exectime etm(__FUNCTION__);
    LOG_DEBUG("-----------------WakeFromSleep------------------");
    //Show hidden widgets
    ShowLeftWgt(true);

    ui->wgtRight->show();
    ui->labBorder->show();
    ui->label->setVisible(true);
    SetTraceVisible(true);
    //ui->labStatu->setVisible(true);

    //Restore background image
    setStyleSheet("QFrame#centralWgt"
                  "{"
                  "   background-image: url(:/image/ui_background_on_iden.png);"
                  "}");

    //Stop time widget moving controller timer
    _timeCoordinateTimer.stop();

    //Restore date time widgets' layout
    ui->wgtTime->setGeometry(90, 7, 380, 94);
    ui->labDate->setGeometry(480, 68, 190, 30);
}

//设置设备处于识别状态以外时钟游动效果
void DialogIdentify::SetTimecoordinate()
{
    Exectime etm(__FUNCTION__);
    //TODO
    static bool xIncrease = true;
    static bool yIncrease = true;

    if(ui->wgtTime->geometry().x() >= 644)
    {
        xIncrease = false;
    }
    else if(ui->wgtTime->geometry().x() <= 0)
    {
        xIncrease = true;
    }


    if(ui->wgtTime->geometry().y() >= 506)
    {
        yIncrease = false;
    }
    else if(ui->wgtTime->geometry().y() <= 0)
    {
        yIncrease = true;
    }


    int x = xIncrease ? (ui->wgtTime->geometry().x() + 1) : (ui->wgtTime->geometry().x() - 1);
    int y = yIncrease ? (ui->wgtTime->geometry().y() + 1) : (ui->wgtTime->geometry().y() - 1);

    ui->wgtTime->setGeometry(x, y, 380, 94);
}

/*****************************************************************************
*                        气象接口定时器
*  函 数    slotTimerWeather
*  功    能：定时获取气象数据
*  说    明：
*  参    数：
*  返 回 值：
*  创 建 人：hatao
*  创建时间：2014-04-23
*  修 改 人：guodj
*  修改时间：2016-6
*****************************************************************************/
void DialogIdentify::slotTimerWeather()
{
    //    Exectime etm(__FUNCTION__);
    //    // 获取气象数据
    //    weatherdata data = m_weather->GetWeatherData();

    //    // 显示气象数据
    //    QString picPath = data.pic;
    //    ui->labWeatherIMG->setPixmap( QPixmap(picPath) );
    //    ui->labCityName->setText( data.city );
    //    ui->labWeather->setText( data.weather1 );
    //    ui->labTemp->setText( data.temp1 + "~" + data.temp2 );
    //    ui->labWinp->setText( data.wind1 );

    //    if(data.weather1 != "")
    //    {
    //        _ptimerUpate->start(1000*3600*12);

    //    }

}


//For LiYan's Test
void DialogIdentify::slotSaveImg()
{
    Exectime etm(__FUNCTION__);
    if(ui->btnSaveImage->text().contains("存图"))
    {
        saveImgFlag = true;
        ui->saveImageStatus->show();
        ui->btnSaveImage->setText("停止");
    }
    else
    {
        saveImgFlag = false;
        ui->saveImageStatus->hide();
        ui->btnSaveImage->setText("存图");
    }
}

void DialogIdentify::slotUpdateCityName(bool isFromNet,QString cityName)
{
    Exectime etm(__FUNCTION__);
    m_weather->SetCityName(isFromNet,cityName);

    //_ptimerUpate->start(1000);
    //StartTimer(_ptimerUpate,1000);

}

void * DialogIdentify::ThreadIden(void *arg)
{
    g_isIdentThreadEnd = false;
    Exectime etm(__FUNCTION__);

    if(NULL == arg)
    {
       // LOG_ERROR("输入指针为空");
        return (void *)0;
    }
    DialogIdentify *pWnd = static_cast<DialogIdentify* >(arg);
    bool          isLoginIndet = pWnd->_isLoginIdent ;
    unsigned char *codeListL;
    unsigned char *codeListR;
    int           codeNumL;
    int           codeNumR;

    if(isLoginIndet)
    {
        codeListL = pWnd->_loginCodeListL;
        codeNumL  = pWnd->_loginCodeNumL;
        codeListR = pWnd->_loginCodeListR;
        codeNumR  = pWnd->_loginCodeNumR;
    }
    else
    {
        codeListL = pWnd->_codeListL;
        codeNumL  = pWnd->_codeNumL;
        codeListR = pWnd->_codeListR;
        codeNumR  = pWnd->_codeNumR;
    }

    IKResultFlag flag;
    IKAlgConfigStruct algConfig;
    algConfig.minEnrIdenTime = IK_MIN_REC_TIME;
    algConfig.maxEnrIdenTime = IK_MAX_REC_TIME;
    algConfig.reservedPara[IK_RESERVEDPARA_BLINK] = 0;
    algConfig.reservedPara[IK_RESERVEDPARA_I2FMODE] = I2FModeIris;

    IKSDKMatchOutput matchOutputL, matchOutputR;

    IKIrisMode irisMode = IrisModeUndef;
    //    IKIrisMode irisMode = IrisModeBoth;
    int nFunFlag = E_USBSDK_DEFAULT_ERROR;

    nFunFlag = IKUSBSDK_StartIrisIden(irisMode, codeListL, codeNumL, &matchOutputL,
                                      codeListR, codeNumR, &matchOutputR, flag, &algConfig);


    LOG_INFO("IKUSBSDK_StartIrisIden return, nFunFlag: %d,codeNumL:%d,codeNumR:%d", nFunFlag,codeNumL,codeNumR);

    pWnd->ShowIdentResult(nFunFlag,flag,matchOutputL,matchOutputR);
    g_isIdentThreadEnd = true;

}

void *DialogIdentify::ThreadLoadData(void *arg)
{

    DialogIdentify *pWnd = static_cast<DialogIdentify*> (arg);
    LOG_INFO("IrisData is Loading now");

    G_IsLoadingCodeList = 1;

    pWnd->ReleseCodeList();
    //初始化
    pWnd->_irisManager.Init();

    G_IsLoadingCodeList = 0;


    //pWnd->InitIdent();

}
