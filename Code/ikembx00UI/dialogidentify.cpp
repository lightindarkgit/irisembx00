
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
#include "Global.h"
#include "../Common/Exectime.h"

#define SUPER_ON

//信号槽链接类
ReDrawEvent drawEvent;
IdentEvent identEvent;
CommonFunction commonFunction;

LRIrisClearFlag s_lrIrisClearFlag;
IrisPositionFlag s_irisPositionFlag;
IrisTraceInfo s_leftTraceInfo;
IrisTraceInfo s_rightTraceInfo;
const long RecogMax = 30000;//识别记录超过3万进行表备份
//add by hatao 2015.4.22
WeatherEvent weatherEvent;


//静态声明
Interaction *DialogIdentify::_identInteraction;
bool DialogIdentify::_defaultIdentMode = true;
bool DialogIdentify::_cardoririsMode   = false; //默认为false,保持原来的识别模式 fjf
bool DialogIdentify::g_isQuitIdent     = false ;
IfHasPerson DialogIdentify::_hasPerson = HasPerson;
IfHasPerson DialogIdentify::_lastPersonStatus;//20141205 lizhl
bool DialogIdentify::g_isStopIdent =true;
unsigned char DialogIdentify::_grayImgBuffer[g_constIrisImgSize + g_constBMPHeaderLength];

bool DialogIdentify::_isThreadRun = false;

//For LiYan's test
volatile bool saveImgFlag = false;

CAlgApi * DialogIdentify::_identApi;
CCameraApi* DialogIdentify::_faceCamer;

QMovie *DialogIdentify::_loadMovie;
QLabel *DialogIdentify::_loadLabel;
QLabel *DialogIdentify::_labMask;
int isCarded = 0 ; /// 0:未刷卡 ；1：已刷卡

int G_BellTime = -1;
int G_MaxTempt = 60;
QString g_IdentName = "";



DialogIdentify::DialogIdentify(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::DialogIdentify),

    _sm(SerialMuxDemux::GetInstance()),
    _irisManager(SingleControl<IrisManager>::CreateInstance()),
    _outMode(WiganOutMode::OutCard),
    _isReloadiris(false),
    _weekDay(weekDay)
{
    Exectime etm(__FUNCTION__);
    ui->setupUi(this);

    //Loading logo infor from local logo file
    updateLogo();

    isCanTracEyes = true;


    //Added by:wangl@20150506
    //关联网络状态变化信号与左边的控件显示
    connect(this, SIGNAL(sigNetworkStatusChanged(bool)), this, SLOT(slotInitLeftWidget(bool)));
    //关联网络变态变化信号与更新网络状态图标
    connect(this, SIGNAL(sigNetworkStatusChanged(bool)), this, SLOT(slotUpdateNetWorkState(bool)));

    _mac = SysNetInterface::GetDevSN();

    _isLoginIdent = false;
    _isOutWigan = true;
    _isOutRelay = true;
    _minOutRalay = 0;
    _secOutRalay = 3;
    _countDown = -1;
    //ui->labCountDown->hide();

    //The following codes for hiding labels was added by Wang.l 2014-09-23
    ui->labIsLogin->hide();
    ui->labCount->hide();
    //ui->labLogo->hide();

    /*********获取配置信息*****************/
    GetConfig();
    //  std::cout << "toWeigand: " << _isOutWigan << "\t toRelay: " << _isOutRelay << std::endl;

    //新建定时器 关联当前时间
    this->startTimer(1000);

    connect(&drawEvent,SIGNAL(sigDrawIdent()),this,SLOT(imageUpdate()));
    connect(&identEvent,SIGNAL(sigResult(long,int)),this,SLOT(identResult(long,int)));
    connect(&identEvent,SIGNAL(sigLoginResult(long,int)),this,SLOT(loginIdentResult(long,int)));
    connect(&identEvent,SIGNAL(sigInteractionStatus(bool)),this,SLOT(interactionStatus(bool)));
    connect(&identEvent,SIGNAL(sigIdentCardID(QString)),this,SLOT(StartIdentCardID(QString)));

    //For LiYan's Test
    QString testImgPath("testimages");
    QDir curDir(QCoreApplication::applicationDirPath());
    ui->btnSaveImage->hide();

    if(!curDir.exists(testImgPath))
    {
        curDir.mkpath(testImgPath);
    }
    ui->saveImageStatus->hide();
    connect(ui->btnSaveImage, SIGNAL(clicked()), this, SLOT(slotSaveImg()));

    //20140822lizhil
    connect(&identEvent,SIGNAL(sigSetUIImg(bool)),this,SLOT(SetUIImg(bool)));


    //人机交互
    _identInteraction = Interaction::GetInteractionInstance();
    _irisClearFlag = LAndRImgBlur;
    _identResultState = ContinueState;

    //初始化算法模块
    _identApi = new CAlgApi();
    _identApi->Init();

    //人脸初始化
    _faceCamer = new CCameraApi();
    _faceCamer->Init();
    _faceCamer->GetImgInfo(&_facephotoSize);
    _facephotoBuf = new unsigned char[_facephotoSize.ImgSize * 2 * sizeof(unsigned char)];

    AddSeriseCallback();
    _countCardMode = -1;



    //新建定时器
    _timerStartIdent = new QTimer();
    //关联定时器计满信号和相应的槽函数--识别成功失败需要在屏幕显示结果1s
    connect(_timerStartIdent,SIGNAL(timeout()),this,SLOT(isStartIdent()));

    //初始化门铃定时器
    InitTimers();

    //20141217 lizhl
    connect(&identEvent, SIGNAL(sigUIofIden()),
            this, SLOT(slotUIofIden()));

    connect(&identEvent, SIGNAL(sigUIofWait()),
            this, SLOT(slotUIofWait()));

    connect(&identEvent, SIGNAL(sigUIofSleep()),
            this, SLOT(slotUIofSleep()));

    //采集人脸独立为一个线程，李志林
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



    /*改成上面大军从新疆发来的快报
    Lx = g_defaultTraceLeftBeginX + g_defaultTraceSize/2;                   //170
    Ly = g_imgHeight - (g_defaultTraceLeftBeginY + g_defaultTraceSize/2)-30; //268
    Lr = 80;
    Rx = g_defaultTraceRightBeginX + g_defaultTraceSize/2;
    Ry = g_imgHeight - (g_defaultTraceRightBeginY + g_defaultTraceSize/2)-30; //424
    Rr = 80;
    */

    //@ weather show
    m_weather = new HBWeather("101010300");

    _ptimerUpate = new QTimer();
    // connect(_ptimerUpate,SIGNAL(timeout()),this,SLOT(slotTimerWeather()));
    // _ptimerUpate->start(2000);

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
    Exectime etm(__FUNCTION__);
    //TODO

    ShowLeftWgt(true);
}


void DialogIdentify::ShowLeftWgt(bool isShow)
{
    //TODO 判断网络模式，如果是联网则显示天气预报，非联网时则显示logo
    if(isShow)
    {
        if(IsConnectedToNetwork())    //如果是联网模式
            //if(false)    //如果是联网模式
        {
            ShowWeather();
        }
        else  //非联网模式
        {
            //@ ShowLogo();
        }

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
    //    if(isNetOK)
    //        if(IsConnectedToNetwork())    //如果是联网模式
    //        {
    //            ShowWeather();
    //        }
    //        else  //非联网模式
    //        {
    //            ShowLogo();
    //        }
    ShowLogo();
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

void DialogIdentify::CheckCPUAndMem()  //检测cpu使用率和内存使用率
{
    char* cmd = "ps  aux   |grep ikembx00  |grep -v  grep  |awk \'{print $3,$4,$5,$6}\'";
    char out[1024*1024];
    int nret = exec_cmd(cmd,out);
    if(nret != 0)
    {
        LOG_ERROR("Get CPU and MEM failed!");
        return nret;
    }

    string result(out);
    int dex = result.find(",");

    // 获取CPU使用率
    string strcpu = result.substr(0,dex);
    float fcpu = atof(strcpu.c_str());
    if(fcpu < 100.f)
    {
        // 内存使用率
        result = result.substr(dex + 1);
        dex = result.find(",");

        result = result.substr(dex + 1);
        dex = result.find(",");
        // 内存分配大小
        string strmem = result.substr(0,dex);
        int memsize = atof(strmem.c_str());
        if(memsize < 809532)
        {
            // 内存和CPU属于正常状态
            return 0;
        }
    }

    {
        LOG_ERROR("CPU&MEM: %s",out);
    }

}

void DialogIdentify::CheckCPUTemp()
{
    char* cmd = "sensors |grep Core";
    char out[1024*1024]={0};
    exec_cmd(cmd,out);
    QString result = QString(QLatin1String(out));

    int firstT = result.indexOf("Core 0:");
    int firstIndex = result.indexOf("+",firstT)+1;
    int tempt1 = result.mid(firstIndex,2).toInt();

    int secondT = result.indexOf("Core 1:");
    int firstIndex2 = result.indexOf("+",secondT)+1;
    int tempt2 = result.mid(firstIndex2,2).toInt();

    if(tempt1 >= G_MaxTempt || tempt2 >= G_MaxTempt)
    {
        LOG_ERROR("CPU 温度超过警示温度，core0：%d    core1：%d",tempt1,tempt2);
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
            _defaultIdentMode = true;
            _cardoririsMode   = false;
        }

        qDebug()<<"---_cardoririsMode:"<<_cardoririsMode<<"  --_defaultIdentMode:"<<_defaultIdentMode<<endl;
        LOG_INFO("---_cardoririsMode:%d",_cardoririsMode);
        LOG_INFO("--_defaultIdentMode:%d",_defaultIdentMode);

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
            std::cout<<"人机交互注册函数失败！"<<std::endl;
        }
        ifRegisterCB = true;
    }
}
void DialogIdentify::AddSeriseCallback()
{
    Exectime etm(__FUNCTION__);
    //增加新的识别模式的判断 2015-08-26 fjf
    if(_cardoririsMode)
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
    _faceCamer->Release();
    delete _facephotoBuf;
    delete _timerStartIdent;
    delete _identApi;
    delete _identInteraction;
    delete ui;
    // add by hatao 2015.4.22
    delete _ptimerUpate;
    delete m_weather;
    _ptimerUpate = NULL;
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
            qDebug()<<"in case DialogSelectLogin::Iris: "<<endl;
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
    _countDown = 30;
    _isLoginIdent = true;
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
        emit sigLoginSucceed(true);
        quitIdent();
    }
    delete login;
}


void DialogIdentify::WakeInteraction()
{
    Exectime etm(__FUNCTION__);
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
    LOG_INFO("in indent timer :g_isQuitIdent = %d  _isLoginIdent=%d",g_isQuitIdent,_isLoginIdent);
    g_isStopIdent = false; //@dj
    _identResultState = ContinueState;
    _timerStartIdent->stop();
    //20141223 lizhl 开始识别，不再显示上次识别结果
    DrawStatusLabel(StatusIdenWait);

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
          LOG_INFO("停止识别中...g_isQuitIdent=%d,_isLoginIdent = %d",g_isQuitIdent,_isLoginIdent);
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
void DialogIdentify::LoadCodeList()
{
    Exectime etm(__FUNCTION__);
    if(_isReloadiris)
    {
        ReleseCodeList();
//        //初始化
        _irisManager.Init();
    }

    //初始化
    //初始化库中注释了管理员特征加载 旧版放开管理员虹膜登录造成识别卡顿，
    //后期需要修改，暂针对屏蔽管理员登录功能注释，管理员虹膜加载，防止管理员虹膜数量超出限定造成程序崩溃
    //_irisManager.Init();

    //加载普通人员特征
    _irisManager.GetIrisFeature(IdentifyType::Left,_codeListL,_codeNumL);
    _irisManager.GetIrisFeature(IdentifyType::Right,_codeListR,_codeNumR);
    LOG_INFO("GetIrisFeature L: %d, R: %d",_codeNumL,_codeNumR);


    #ifdef SUPER_ON
    //加载管理员特征
    _irisManager.GetSuperFeature(IdentifyType::Left,_loginCodeListL,_loginCodeNumL);
    _irisManager.GetSuperFeature(IdentifyType::Right,_loginCodeListR,_loginCodeNumR);
    #endif

    _isReloadiris = false;
}

void DialogIdentify::ReleseCodeList()
{
    Exectime etm(__FUNCTION__);

    _irisManager.ReleaseFeature();

    #ifdef SUPER_ON
    _irisManager.ReleaseSuperData();
    #endif
}

void DialogIdentify::SetReLoadIris(bool isReLoad)
{
    Exectime etm(__FUNCTION__);
    _isReloadiris = isReLoad;
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
    Exectime etm(__FUNCTION__);
    int funResult = E_ALGAPI_DEFAUL_ERROR;

    funResult = _identApi->GetParam(&_idenParamStruct);
    std::cout<<funResult<<std::endl;
    if(E_ALGAPI_OK == funResult)
    {
        _idenParamStruct.EyeMode = AnyEye;
        funResult = _identApi->SetParam(&_idenParamStruct);
    }
    return funResult;
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
    SetIdentParam();

    //重新获取设备信息
    GetConfig();
    AddSeriseCallback();

    //初始化人脸
    //_faceCamer->Init();

    //ui->labIP->setText( QString(NetInfo::GetIP().c_str()));

    //设置初始状态和标志位---防止返回识别界面时仍然绘制成功或失败
    _identResultState = UnKnownState;

    g_isQuitIdent = false;
    //初始化识别设置
    ReIdent();

    _identApi->AsyncCapIrisImg(_identInteraction->s_infraredDistanceValue,
                               CapIrisImgCallBack);

    if(_defaultIdentMode || _cardoririsMode )
    {
        //@dj------------
       // isStartIdent();
       _timerStartIdent->start(1000);

    }

#ifdef DEBUG
    ///测试输出
    QDateTime time = QDateTime::currentDateTime();
    QString strTime = time.toString("hh:mm");
    std::cout<<strTime.toStdString() + "进入识别识别！ "<<std::endl;
#endif

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
    LOG_INFO("启动识别....")
    ui->labIsLogin->setText(QString::fromUtf8("考勤识别"));

    _isLoginIdent = false;
    //if(_isReloadiris)
    {
        std::cout<<"loading irir feature..."<<std::endl;
        LoadCodeList();
    }
    std::cout<<"开始识别StartAttendIdent函数时间:"
            <<QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss").toStdString()<<"  "
           <<std::endl;
//    if(_codeNumL == 0 && _codeNumR==0)
//    {
//        quitIdent();
//        StartIdent();
//        return;
//    }


    int error = _identApi->AsyncStartIden(IdentCallBack,
                                          _codeListL, _codeNumL,_codeListR, _codeNumR);

#ifdef DEBUG
    std::cout<<"开始识别 _codeNumL: "<<_codeNumL<<" _codeNumR :"<<_codeNumR<<std::endl;
#endif

    if(error!=0)
    {
        LOG_INFO("<------开始识别失败！------->");
        std::cout<<"开始识别失败！"<<std::endl;
    }

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

    //if(_isReloadiris)
    {
        //_irisManager.ReleaseSuperData();
        //_irisManager.LoadSuperData();
        LoadCodeList();
    }

    int error = _identApi->AsyncStartIden(LoginIdentCallBack,
                                          _loginCodeListL, _loginCodeNumL,_loginCodeListR,_loginCodeNumR);

    if(error!=0)
    {
        std::cout<<"管理员开始识别失败！"<<std::endl;
    }
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
        return;
    }
    g_isQuitIdent = true;
    LOG_INFO("退出识别");

    //停止识别
    StopIdent();
    DellSeriseCallback();
    //释放人脸API
    //_faceCamer->Release();
    _identResultState = UnKnownState;

    // common by liushu
    //删除虹膜特征
    //ReleseCodeList();

#ifdef DEBUG
    QDateTime time = QDateTime::currentDateTime();
    QString strTime = time.toString("yyyy-MM-dd hh:mm:ss");
    std::cout<<strTime.toStdString() + "退出识别！ "<<std::endl;
    std::cout<<"卸载i虹膜特征 _codeNumL: "<<_codeNumL<<" _codeNumR :"<<_codeNumR<<std::endl;
#endif



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
    int error = _identApi->StopIden();
    if(error != 0)
    {
        // std::cout<<"停止识别失败！ "<<std::endl;
        LOG_ERROR("停止识别失败！")
    }
    else
    {
        g_isStopIdent = true;
    }

    //20141215 lizhl
    SetTraceVisible(false);

    DrawStatusLabel(StatusIdenStop);


#ifdef DEBUG
    QDateTime time = QDateTime::currentDateTime();
    QString strTime = time.toString("yyyy-MM-dd hh:mm:ss");
    std::cout<<strTime.toStdString() + "停止识别！ "<<std::endl;
#endif

}

/*****************************************************************************
 *                        识别回调函数
 *  函 数 名： IdentCallBack
 *  功    能：识别结果处理
 *  说    明：
 *  参    数：
 *  返 回 值：
 EnrRecBothFailed	=	-3,		//EnrRecBothFailed：注册、识别时左右眼均注册或识别失败
 EnrRecRightFailed	=	-2,		//EnrRecRightFailed:右眼注册、识别失败导致注册或识别失败,左眼情况未知
 EnrRecLeftFailed	=	-1,		//EnrRecLeftFailed:左眼注册、识别失败导致注册或识别失败,右眼情况未知
 EnrRecUnknown       =   0,      //EnrRecUnknown:注册、识别结果未知
 EnrRecLeftSuccess	=	1,		//EnrRecLeftSuccess:注册、识别时仅左眼注册或识别成功
 EnrRecRightSuccess	=	2,		//EnrRecRightSuccess:注册、识别时仅右眼注册或识别成功
 EnrRecBothSuccess	=	3		//EnrRecBothSuccess:注册、识别时左右眼均注册成功
 *  创 建 人：刘中昌
 *  创建时间：2013-12-23
 *  修 改 人：
 *  修改时间：
 *****************************************************************************/
int DialogIdentify::IdentCallBack(int funResult, APIIrisInfo* recIrisL, APIIrisInfo* recIrisR, LRSucFailFlag lrFlag,
                                  APIMatchOutput* pLeftMatchOutputParam, APIMatchOutput* pRightMatchOutputParam)
{
    Exectime etm(__FUNCTION__);
    if(funResult ==1)
    {//人为停止
        std::cout<<"人为停止 funResult 为："<< funResult <<std::endl;
        return -1;
    }else if (E_ALGAPI_ENR_IDEN_FAILED == funResult)
    {
        switch(lrFlag)
        {
        case EnrRecBothFailed://左右眼都失败
            identEvent.onResult(-1,3);
            break;
        case EnrRecRightFailed:
            identEvent.onResult(-1,3);
            break;
        case EnrRecLeftFailed:
            identEvent.onResult(-1,3);
            break;
        default:
            identEvent.onResult(0,3);
            std::cout<<"识别失败但是funResult 为："<< funResult <<std::endl;
            break;
        }
    }else if(funResult == E_OK)
    {
        switch(lrFlag)
        {
        case EnrRecLeftSuccess:
            identEvent.onResult(pLeftMatchOutputParam->MatchIndex[0],0);
            break;
        case EnrRecRightSuccess:
            identEvent.onResult(pRightMatchOutputParam->MatchIndex[0],1);
            break;
        case EnrRecBothSuccess:
            pRightMatchOutputParam->MatchScore[0]>pLeftMatchOutputParam->MatchScore[0]?
                        identEvent.onResult(pRightMatchOutputParam->MatchIndex[0],1):
                        identEvent.onResult(pLeftMatchOutputParam->MatchIndex[0],0);
            break;
        default:
            std::cout<<"识别成功 ！但是funResult 为："<< funResult <<std::endl;
        }
    }
    else
    {
        identEvent.onResult(0,3);
    }
    return 0;
}

/*****************************************************************************
 *                        管理员识别回调函数
 *  函 数 名： LoginIdentCallBack
 *  功    能：识别结果处理
 *  说    明：
 *  参    数：
 *  返 回 值：
 EnrRecBothFailed	=	-3,		//EnrRecBothFailed：注册、识别时左右眼均注册或识别失败
 EnrRecRightFailed	=	-2,		//EnrRecRightFailed:右眼注册、识别失败导致注册或识别失败,左眼情况未知
 EnrRecLeftFailed	=	-1,		//EnrRecLeftFailed:左眼注册、识别失败导致注册或识别失败,右眼情况未知
 EnrRecUnknown      =    0,     //EnrRecUnknown:注册、识别结果未知
 EnrRecLeftSuccess	=	 1,		//EnrRecLeftSuccess:注册、识别时仅左眼注册或识别成功
 EnrRecRightSuccess	=	 2,		//EnrRecRightSuccess:注册、识别时仅右眼注册或识别成功
 EnrRecBothSuccess	=	 3		//EnrRecBothSuccess:注册、识别时左右眼均注册成功
 *  创 建 人：刘中昌
 *  创建时间：2013-12-23
 *  修 改 人：
 *  修改时间：
 *****************************************************************************/
int DialogIdentify::LoginIdentCallBack(int funResult, APIIrisInfo* recIrisL, APIIrisInfo* recIrisR, LRSucFailFlag lrFlag,
                                       APIMatchOutput* pLeftMatchOutputParam, APIMatchOutput* pRightMatchOutputParam)
{
    Exectime etm(__FUNCTION__);
    if(funResult ==1)
    {//人为停止
        return -1;
    }else if (E_ALGAPI_ENR_IDEN_FAILED == funResult)
    {
        switch(lrFlag)
        {
        case EnrRecBothFailed://左右眼都失败
            identEvent.onLoginResult(-1,0);
            break;
        case EnrRecRightFailed:
            identEvent.onLoginResult(-1,1);
            break;
        case EnrRecLeftFailed:
            identEvent.onLoginResult(-1,2);
            break;
        case EnrRecUnknown:
        default:
            break;
        }
    }else if(funResult == E_OK)
    {
        switch(lrFlag)
        {
        case EnrRecLeftSuccess:
            identEvent.onLoginResult(pLeftMatchOutputParam->MatchIndex[0],0);
            break;
        case EnrRecRightSuccess:
            identEvent.onLoginResult(pRightMatchOutputParam->MatchIndex[0],1);
            break;
        case EnrRecBothSuccess:
            pRightMatchOutputParam->MatchScore[0]>pLeftMatchOutputParam->MatchScore[0]?
                        identEvent.onLoginResult(pRightMatchOutputParam->MatchIndex[0],1):
                        identEvent.onLoginResult(pLeftMatchOutputParam->MatchIndex[0],0);
            break;
        default:
            break;
        }
    }
    else
    {
        identEvent.onLoginResult(0,3);

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
    /// @dj 非登录
    if(!_isLoginIdent)
    {
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

                //如果刷卡识别则要判断_personCardID是否与person的CardID相同 以后还要加是否禁止识别功能能
                //---之后要写个函数
                // if(_personCardID.toStdString() !=person.CardID)
                QByteArray dbPersoncardID  = QString::fromStdString(person.CardID).toAscii();
                QByteArray curCardIDDec    = ConvertCardNumToDec(_personCardID).toAscii();

                char* card1 = dbPersoncardID.data();
                char* card2 = curCardIDDec.data();

                bool bsame = (0 == strcmp(card1,card2));

                LOG_INFO("_defaultIdentMode: %d,dbPersoncardID: [%s], curCardIDDec: [%s] result: %d",
                         _defaultIdentMode,card1,card2,bsame);

                if(_defaultIdentMode || (!_defaultIdentMode && bsame))
                {
                    //20141215 lizhl
                    DrawStatusLabel(StatusIdenSuccess, QString(person.Name.c_str()));

                    _identResultState = SuccessState;


                    //将数据存储的数据库
                    SaveRecogToDB(person);

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
                    if(!_defaultIdentMode && bsame )
                    {
                        _countCardMode = 1;
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
                std::cout<<e.what()<<endl;
            }
        }
    }
    if(!g_isQuitIdent && _identResultState == ContinueState)
    {
        //异步识别识别
        //@dj
        //isStartIdent();
        _timerStartIdent->stop();
        _timerStartIdent->start(1000);
    }
    else
    {
        //std::cout<<"停止识别  _isClose 为："<< _isClose <<" _isLoginIdent 为："<< _isLoginIdent<<std::endl <<" _identResultState 为："<< _identResultState <<std::endl;
    }

//    timeval tmv1,tmv2;
//    gettimeofday(&tmv1,NULL);
    bool uploadStatus = IkSocket::GetIkSocketInstance()->IsUpLoading();
    //如果自动上传线程正在上传，则跳过，防止主线程阻塞
    if(!uploadStatus)
    {
        IkSocket::GetIkSocketInstance()->UploadTemp();
    }
    else
    {
        LOG_INFO("自动上传线程正在上传");

    }

//    gettimeofday(&tmv2,NULL);
//    int64_t  usec = (tmv2.tv_sec - tmv1.tv_sec) * 1000000 + (tmv2.tv_usec - tmv1.tv_usec);

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

    personRec.recogTime =QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss").toStdString();// QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss");

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
        int err = _sm->Send(relay,4,PriorityLevels::HighPriority);

        std::cout << GetLogHead(__FUNCTION__) <<  "向继电器发送信号完成(return-value:" << err << ")！"<<std::endl;
    }

    //向韦根发信号
    if(_isOutWigan)
    {
        if(!QString::fromStdString(person.CardID).trimmed().isEmpty())
        {
            //只截取十六进制卡号的最后六个字节
            //string vaildCardIDSec = person.CardID.substr(person.CardID.length() - 6);
            QString vaildCardIDSec = ConvertCardNumToHex(person.CardID.c_str());
            ulong cardID = vaildCardIDSec.toULong(NULL, 16);

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
                int err = _sm->Send(wigan,4,PriorityLevels::HighPriority);

                printf("%s向韦根发卡号信息完成！cardID: %d.%d.%d\n", GetLogHead(__FUNCTION__).c_str(), wigan[1], wigan[2], wigan[3]);

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
    if(_isLoginIdent)
    {
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

            emit sigLoginSucceed(true);
            //停止识别
            quitIdent();

            return;

        }
    }

    if(!g_isQuitIdent && _identResultState == ContinueState )
    {
        //异步识别识别
        _timerStartIdent->stop();
        _timerStartIdent->start(1000);
        // isStartIdent();
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

    //@dj 20161205 SetStatus(!flag);

    //add by dj 20161205
    //睡眠唤醒时，重新开始识别，防止休眠唤醒后不识别的问题
    if(flag)
    {
        quitIdent();
        usleep(500);
        StartIdent();
    }
    else
    {
        SetStatus(!flag);
    }

}

void DialogIdentify::SetUIImg(bool isSleep)
{
    Exectime etm(__FUNCTION__);
    if(isSleep)
    {

    }
    else
    {
        _identInteraction->ResetDisplay();

        //20141204 lizhl
        _loadMovie->start();
        _loadLabel->setVisible(true);
    }

    ////Comment off by Wangl @20150115
    //    ui->labelSleep->setVisible(true);

    //20141204 lizhl
    _waitMovie->stop();
    //_waitLabel->setVisible(false);

    //20150121 Wang.L
    ui->labelSleep->setVisible(false);

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
 *  修 改 人：
 *  修改时间：
 *****************************************************************************/
void DialogIdentify::IdentInteractionStatusCallBack(IfHasPerson hasPerson)
{
    Exectime etm(__FUNCTION__);
    //20141205 lizhl
    //如果是刷卡识别和识别退出则直接返回
    //    if(!_defaultIdentMode || g_isQuitIdent)
    //    {
    //        std::cout<<"是否有人进入识别回调->IfHasPerson :"<<hasPerson<< "刷卡识别和识别退出则直接返回" << std::endl;
    //        return;
    //    }

    std::cout<<"是否有人进入识别回调->IfHasPerson :"<<hasPerson<<std::endl;

    _hasPerson = hasPerson;

    if( hasPerson == HasPerson )
    {
        if(_lastPersonStatus == NoPerson)
        {
            _hasPerson = HasPerson;
            identEvent.onSetUIImg(false);
            int wakeResult = _identApi->WakeAlgApi();
            if(wakeResult != 0)
            {
                LOG_ERROR("AlgApi唤醒失败！");
            }
            _identApi->AsyncCapIrisImg(_identInteraction->s_infraredDistanceValue,
                                       CapIrisImgCallBack);
            _faceCamer = new CCameraApi();
            _faceCamer->Init();
            identEvent.onInteractionStatus(true);
            identEvent.onUIofIden();
        }
        else
        {
            identEvent.onUIofIden();
        }

    }
    else if(hasPerson == WaitPerson)
    {
        //20141215
        identEvent.onUIofWait();
    }
    else if(NoPerson == hasPerson)
    {
        _hasPerson = NoPerson;
        identEvent.onInteractionStatus(false);
        struct timespec slptm;
        slptm.tv_sec = 2;
        slptm.tv_nsec = 1000;      //1000 ns = 1 us
        nanosleep(&slptm,0);

        _faceCamer->Release(true);
        _identApi->SleepAlgApi();
        identEvent.onSetUIImg(true);
        identEvent.onUIofSleep();    //Added by: wangl @20150115
    }
    _lastPersonStatus = _hasPerson;//记录本次状态 20141205 lizhl
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

    }
    else
    {
        std::cout<<"StartIdent() 识别开始！"<<std::endl;
        if(!g_isStopIdent)
        {
            LOG_ERROR("StartIdent() 识别开始！");
            return;
        }

        _timerStartIdent->start(1000);
        //        isStartIdent();

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
        LOG_ERROR("初始化串口失败！");
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
            LOG_INFO("向串口注册cardID回调函数成功！");
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
 *  参    数：
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
 *  参    数：personCardID-十六进制IC卡号
 *  返 回 值：
 *  创 建 人：liuzhch
 *  创建时间：2014-04-02
 *  修 改 人：gdj
 *  修改时间：2016-11-17
 *****************************************************************************/
void DialogIdentify::StartIdentCardID(QString personCardID)
{
    Exectime etm(__FUNCTION__);
    std::cout<<"enter StartIdentCardID------cardNUM is:"<<personCardID.toStdString()<<std::endl;
    if (_cardoririsMode)
    {

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
                //SetStatus(true);
                _personCardID = personCardID;
                //SetStatus(false);
            }
            if(!_isLoginIdent)
                _countCardMode = 30;
        }
    }
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
 *                        气象数据回调函数
 *  函 数 名：RecvWeatherInfoCallBack
 *  功    能：
 *  说    明：
 *  参    数：
 *  返 回 值：
 *  创 建 人：hatao
 *  创建时间：2014-04-22
 *  修 改 人：
 *  修改时间：
 *****************************************************************************/

void DialogIdentify::RecvWeatherInfoCallBack(WEATHERINFO &wInfo)
{
    Exectime etm(__FUNCTION__);
    //std::cout<<"city = "<<wInfo.CityName<<endl;
    //std::cout<<"Temperature: "<<wInfo.TotalTemp<<endl;



    QString strLine = QString::fromStdString(wInfo.CityName) + ",";
    strLine += QString::fromStdString(wInfo.TotalTemp) + ",";
    strLine += QString ::fromStdString(wInfo.WeatherInfo) + ",";
    strLine += QString ::fromStdString(wInfo.Winp) + ",";
    strLine += QString ::fromStdString(wInfo.WeatherICO);

    weatherEvent.SetWeatherInfo(strLine);


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
 *  修 改 人：
 *  修改时间：
 *****************************************************************************/
void DialogIdentify::timerEvent(QTimerEvent *event)
{
    Exectime etm(__FUNCTION__);

    //CheckCPUTemp();    //CPU 温度检测
    //CheckCPUAndMem();  //CPU占用率检测
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
    ui->labCount->setText(QString::fromUtf8("帧率：") + QString::number(_count));
    _count =0;

    /// 登录模式则不计刷卡 isCard=0默认为没有刷卡
    if(_isLoginIdent)
    {
        isCarded = 0;
    }

    //虹膜登陆
    /// 计时为0时停止识别
    if(_countDown == 0)
    {
        ui->labCountDown->setText(QString::number(_countDown));
        ReIdent();
    }
    /// 倒计时时开启识别
    else if(_countDown>0)
    {
        ui->labCountDown->setText(QString::number(_countDown));
        _countDown--;
        _countCardMode = 0;
        if( g_isStopIdent)
        {

           _timerStartIdent->start(1000);
        }
    }
    /// 登录界面直接返回
    else if(_countDown == -1)
    {
        if(!_defaultIdentMode)
        {
            StopIdent();
        }
        _countCardMode = 0;
        _countDown --;
    }


    //For LiYan's Test@20150803
    if(saveImgFlag)
    {
        ui->saveImageStatus->setChecked(!ui->saveImageStatus->isChecked());
    }

    //更新网络状态
    CheckNetworkStatus();


    //    LOG_DEBUG("**********************");
    //    LOG_DEBUG("_countDown     ----  %d",_countDown);
    //    LOG_DEBUG("g_isStopIdent  ----  %d",g_isStopIdent);
    //    LOG_DEBUG("_countCardMode ----  %d",_countCardMode);
    //    LOG_DEBUG("_hasPerson     ----  %d",_hasPerson);
    //    LOG_DEBUG("_isLoginIdent  ----  %d",_isLoginIdent);

    if(_countDown  > 0)
        return;


    if(!_defaultIdentMode && !_isLoginIdent)
    {

        /// 如果已经刷卡
        if(isCarded == 1  )
        {
            _countCardMode = 30;
            isCarded = 0;
        }

        if( _countCardMode <= 0 )
        {
            StopIdent();


            ui->labStatu->setVisible(true);
            ui->labStatu->setStyleSheet("QLabel{border-image:url(:/image/ui_bar_note.png)}");
            QString labText = QString::fromUtf8("请刷卡") ;
            ui->labStatu->setText(labText);

        }
        else if(_countCardMode> 0 )
        {
            _countCardMode--;
            if(_countCardMode <2 )
                return ;
            ui->labStatu->setVisible(true);
            ui->labStatu->setStyleSheet("QLabel{border-image:url(:/image/ui_bar_note.png)}");
            QString labText = QString::fromUtf8("请在") + QString::number(_countCardMode)+QString::fromUtf8("s内虹膜识别");
            ui->labStatu->setText(labText);

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
    //状态判断与执行类型处理
    if(_identResultState == SuccessState)
    {
        _identResultState = KeepState;
        _timerStartIdent->start(1000);
        _interactionResultType = IdenSuccess;
    }
    else if(_identResultState == FailedState)
    {
        _identResultState = KeepState;
        _timerStartIdent->start(1000);
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


    repaint();
    //20141204 lizhl
    if(HasPerson == _hasPerson)
    {
        if(QMovie::NotRunning != _waitMovie->state())
        {
            _waitMovie->stop();
        }

        //20141218 lizhl
        _identInteraction->noticeCount++;
        if(_identInteraction->noticeCount % g_noticeNum == 0)
        {
            ifUpdatePosition = true;
        }
        else
        {
            ifUpdatePosition = false;
        }


        QImage imageBig;
        imageBig = QImage::fromData(_grayImgBuffer, g_constIrisImgSize + g_constBMPHeaderLength, "BMP");

        if(!imageBig.isNull())
        {
            QImage paintImg = imageBig.convertToFormat(QImage::Format_ARGB32_Premultiplied);

            _identInteraction->PlayInteraction(_interactionResultType,s_lrIrisClearFlag,s_irisPositionFlag, ifUpdatePosition, paintImg,
                                               s_leftTraceInfo, s_rightTraceInfo);

            //For LiYan's Test
            if(saveImgFlag)
            {
                QString imgPath("%1/testimages/IK_Face_Image_%2.bmp");
                imgPath = imgPath.arg(QCoreApplication::applicationDirPath(),
                                      QDateTime::currentDateTime().toString("yyyy_MM_dd_hh_mm_ss_zzz"));
                paintImg.save(imgPath, "bmp", 100);
            }

            //20141215 lizhl
            DrawTraceGif(s_irisPositionFlag, s_lrIrisClearFlag, s_leftTraceInfo, s_rightTraceInfo);

            if(ifUpdatePosition && ResultUnknown == _interactionResultType)
            {
                //如果需要更新位置信息并且没有结果，则提示状态信息
                DrawStatusLabel(StatusAlgIsPerform);
            }
            //检测不到人眼睛
         //   if(!isCanTracEyes)
            {


                QPainter painter(&paintImg);
               // QPen pen(Qt::gray, 3, Qt::DashDotLine, Qt::RoundCap, Qt::RoundJoin);

                 QPen pen(Qt::green, 3, Qt::DashDotLine, Qt::RoundCap, Qt::RoundJoin);
                 //  加载特征为0时，圆圈显示为灰色
                 if(_codeNumL*_codeNumR <= 0)
                 {
                     pen.setColor(Qt::black);
                 }

                 // 退出识别状态时 显示为红色
                 if(g_isQuitIdent)
                 {
                     pen.setColor(Qt::red);
                 }

                 // 登录识别时显示为蓝色
                 if(_isLoginIdent)
                 {
                     pen.setColor(Qt::blue);
                 }

                 if(isCanTracEyes)
                 {
                     pen.setColor(Qt::gray);
                 }


                painter.setPen(pen);

                QPoint centL(Lx,Ly);
                painter.drawEllipse(centL,Lr,Lr);
                QPoint centR(Rx,Ry);
                painter.drawEllipse(centR,Rr,Rr);
                painter.end();

            }



            ui->labEye->setPixmap(QPixmap::fromImage(paintImg));
            _count ++;
        }
        else
        {
            std::cout<<"识别显示未获得图像！"<<std::endl;
        }

        if(!ui->labEye->isVisible())
        {
            //_waitLabel->setVisible(false);
            ui->labelSleep->setVisible(false);     //Added By:Wang.L @20150121
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


    if(!g_isQuitIdent)
    {
        _identApi->AsyncCapIrisImg(_identInteraction->s_infraredDistanceValue,
                                   CapIrisImgCallBack);
    }
}


/*****************************************************************************
 *                         异步采图回调函数
 *  函 数 名：
 *  功    能：异步采图回调函数
 *  说    明：在回调函数中取异步采图结果
 *  参    数：
 *  返 回 值：
 *  创 建 人：lizhl
 *  创建时间：2014-03-13
 *  修 改 人：
 *  修改时间：
 *****************************************************************************/
int DialogIdentify::CapIrisImgCallBack(int funResult, unsigned char* pIrisImg, LRIrisClearFlag lrIrisClearFlag, IrisPositionFlag irisPositionFlag, IrisTraceInfo leftTraceInfo, IrisTraceInfo rightTraceInfo)
{
    Exectime etm(__FUNCTION__);
    if(E_ALGAPI_OK == funResult)
    {
        //        _grayImgBuffer = pIrisImg;
        memcpy(_grayImgBuffer, pIrisImg, g_constIrisImgSize + g_constBMPHeaderLength);
        s_lrIrisClearFlag = lrIrisClearFlag;
        s_irisPositionFlag = irisPositionFlag;
        s_leftTraceInfo = leftTraceInfo;
        s_rightTraceInfo = rightTraceInfo;
    }

    //绘图
    //    if(HasPerson == _hasPerson)
    if(NoPerson != _hasPerson)//20141205 lizhl
    {
        drawEvent.OnDrawIdent();
    }
    return 0;
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
                //std::cout << GetLogHead(__FUNCTION__) << "saving face photos to directory "  << pParam->GetFacePhotoPath().toStdString() << std::endl;

                image = image.mirrored(false, true);
                image.save(pParam->GetFacePhotoPath() + "/"
                           + QDateTime::currentDateTime().toString("yyyy-MM-dd_hh_mm_ss") + resultName + ".jpg","JPG");

            }
            else
            {
                std::cout << GetLogHead(__FUNCTION__) << "人脸图像采集获取图片失败！" << std::endl;
                LOG_ERROR("人脸图像采集获取图片失败！");
            }
        }
    }

    return (void *)0;
}

//20141212 lizhl 增加识别界面的人机交互函数
void DialogIdentify::InitLabelStatus()
{
    Exectime etm(__FUNCTION__);
    //20141215
    ui->labStatu->setVisible(false);
}

void DialogIdentify::InitLabelMask()
{
    Exectime etm(__FUNCTION__);
    _labMask = new QLabel(this,/*Qt::SplashScreen | */Qt::WindowStaysOnTopHint/* | Qt::X11BypassWindowManagerHint*/ );
    _labMask->setGeometry(g_imgBeginX, g_imgBeginY, g_imgWidth, g_imgHeight);//178,104,664,498

    QImage image(":/image/ui_mask_green.png");
    image.scaled(g_imgWidth, g_imgHeight);
    QPixmap pic1(QPixmap::fromImage(image));

    //设置图片透明度
    //    QPixmap* alpic1=new QPixmap(pic1.size());
    //    alpic1->fill(QColor(128,128,0,200));
    //    pic1.setAlphaChannel(*alpic1);
    _labMask->setPixmap(pic1);
}

void DialogIdentify::InitTraceGif()
{
    Exectime etm(__FUNCTION__);

    Lx = g_defaultTraceLeftBeginX + g_defaultTraceSize/2;                   //170
    // Ly = g_imgHeight - (g_defaultTraceLeftBeginY + g_defaultTraceSize/2)-30; //268
    Ly = g_imgHeight*2/3 ;
    Lr = 80;
    Rx = g_defaultTraceRightBeginX + g_defaultTraceSize/2;
    Ry = g_imgHeight*2/3 ;
    // Ry = g_imgHeight - (g_defaultTraceRightBeginY + g_defaultTraceSize/2)-30; //424
    Rr = 80;


    _movTraceL = new QMovie(":/image/ui_trace_r.gif");
    _movTraceL->setScaledSize(QSize(g_defaultTraceSize, g_defaultTraceSize));

    _labTraceL = new QLabel(this);
    _labTraceL->setMovie(_movTraceL);

   // _labTraceL->setGeometry(g_defaultTraceLeftBeginX + g_imgBeginX, g_defaultTraceLeftBeginY + g_imgBeginY, g_defaultTraceSize, g_defaultTraceSize);
    _labTraceL->setGeometry(int(Lx*g_scaling) - g_defaultTraceSize/2  + g_imgBeginX,
                            int(Ly*g_scaling) - g_defaultTraceSize/2  + g_imgBeginY,
                            g_defaultTraceSize,
                            g_defaultTraceSize);


    _movTraceR = new QMovie(":/image/ui_trace_r.gif");
    _movTraceR->setScaledSize(QSize(g_defaultTraceSize, g_defaultTraceSize));

    _labTraceR = new QLabel(this);
    _labTraceR->setMovie(_movTraceR);
  //  _labTraceR->setGeometry(g_defaultTraceRightBeginX + g_imgBeginX, g_defaultTraceRightBeginY + g_imgBeginY, g_defaultTraceSize, g_defaultTraceSize);
    _labTraceR->setGeometry(int(Rx*g_scaling) - g_defaultTraceSize/2  + g_imgBeginX,
                            int(Ry*g_scaling) - g_defaultTraceSize/2  + g_imgBeginY,
                            g_defaultTraceSize,
                            g_defaultTraceSize);

    //20141210
    _isClearL = _isClearR = _isLastClearL = _isLastClearR = false;
    SetTraceVisible(true);
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
    switch(type)
    {
    case StatusAlgIsPerform:
        if(Far == s_irisPositionFlag)
        {
            ui->labStatu->setVisible(true);
            ui->labStatu->setStyleSheet("QLabel{border-image:url(:/image/ui_bar_get_closer.png)}");
            ui->labStatu->setText("请靠近一点");
        }
        else if(Near == s_irisPositionFlag)
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

void DialogIdentify::DrawTraceGif(IrisPositionFlag irisPositionFlag, LRIrisClearFlag lrIrisClearFlag, IrisTraceInfo leftTraceInfo, IrisTraceInfo rightTraceInfo)
{
    Exectime etm(__FUNCTION__);

    if(Unknown == irisPositionFlag)
    {
//        leftTraceInfo.centC = g_defaultTraceLeftBeginX + g_defaultTraceSize/2;
//        leftTraceInfo.centR = g_defaultTraceLeftBeginY + g_defaultTraceSize/2;

//        rightTraceInfo.centC = g_defaultTraceRightBeginX + g_defaultTraceSize/2;
//        rightTraceInfo.centR = g_defaultTraceRightBeginY + g_defaultTraceSize/2;

        leftTraceInfo.centC = Lx;
        leftTraceInfo.centR = Ly;

        rightTraceInfo.centC = Rx;
        rightTraceInfo.centR = Ry;

    }

    if(leftTraceInfo.centC + leftTraceInfo.centR > 0)
    {
        isCanTracEyes = true;
        if(LImgClear == lrIrisClearFlag || LAndRImgClear == lrIrisClearFlag)
        {
            _isClearL = true;
        }
        else
        {
            _isClearL = false;
        }

        if(_isClearL != _isLastClearL)
        {
            if(_isClearL)
            {
                _movTraceL->setFileName(":/image/ui_trace_g.gif");
            }
            else
            {
                _movTraceL->setFileName(":/image/ui_trace_r.gif");
            }
            _movTraceL->restart();
            _labTraceL->setMovie(_movTraceL);
        }

        _labTraceL->setGeometry((int)(leftTraceInfo.centC * g_scaling) - g_defaultTraceSize/2 + g_imgBeginX,
                                (int)(leftTraceInfo.centR * g_scaling) - g_defaultTraceSize/2 + g_imgBeginY,
                                g_defaultTraceSize, g_defaultTraceSize);

        if(!_labTraceL->isVisible())
        {
            _labTraceL->setVisible(true);
        }
    }
    else
    {
        //repaint();
        //未检测到眼睛则不显示追踪gif
        _labTraceL->setVisible(false);
        isCanTracEyes = false;
    }

    if(rightTraceInfo.centC + rightTraceInfo.centR > 0)
    {
        isCanTracEyes = true;
        if(RImgClear == lrIrisClearFlag || LAndRImgClear == lrIrisClearFlag)
        {
            _isClearR = true;
        }
        else
        {
            _isClearR = false;
        }


        if(_isClearR != _isLastClearR)
        {
            if(_isClearR)
            {
                _movTraceR->setFileName(":/image/ui_trace_g.gif");
            }
            else
            {
                _movTraceR->setFileName(":/image/ui_trace_r.gif");
            }
            _movTraceR->restart();
            _labTraceR->setMovie(_movTraceR);
        }

        _labTraceR->setGeometry((int)(rightTraceInfo.centC * g_scaling) - g_defaultTraceSize/2 + g_imgBeginX,
                                (int)(rightTraceInfo.centR * g_scaling) - g_defaultTraceSize/2 + g_imgBeginY,
                                g_defaultTraceSize, g_defaultTraceSize);

        if(!_labTraceR->isVisible())
        {
            _labTraceR->setVisible(true);
        }
    }
    else
    {
        //未检测到眼睛则不显示追踪gif
        _labTraceR->setVisible(false);
        isCanTracEyes = false;
    }

    _isLastClearL = _isClearL;
    _isLastClearR = _isClearR;
}

void DialogIdentify::slotUIofIden()
{
    Exectime etm(__FUNCTION__);
    _loadMovie->stop();
    _loadLabel->setVisible(false);
    _labMask->setVisible(true);
    //ui->label->setVisible(true);
    //@wangl 20150110
    WakeFromSleep();
    //@wangl end
}

void DialogIdentify::slotUIofWait()
{
    Exectime etm(__FUNCTION__);
    //Commented off By: Wang.L @20150121
    //ui->labelSleep->setVisible(false);
    DrawStatusLabel(StatusIdenWait);
    _labMask->setVisible(false);
    ui->labEye->setVisible(false);
    //ui->label->setVisible(false);

    //@wangl 20150110
    SetOnSleep();
    //@wangl end
}

void DialogIdentify::slotUIofSleep()
{
    Exectime etm(__FUNCTION__);
    //ui->labelSleep->setVisible(true);   //@zhil
    //@wangl 20150110
    setStyleSheet("QFrame#centralWgt"
                  "{"
                  "   background-image: url(:/image/ui_background_on_sleep.png);"
                  "}");
    //@wangl end
}

//Added by wang.l @2015-01-10
void DialogIdentify::SetOnSleep()
{
    Exectime etm(__FUNCTION__);
    //Hide invisible widgets
    ShowLeftWgt(false);

    ui->wgtRight->hide();
    ui->labBorder->hide();

    // 大军从新疆发来的快报 begin
    //ui->label->setVisible(false);
    //    ui->label_2->setVisible(false);
    // 大军从新疆发来的快报 end

    /*// 修改为上面大军从新疆发来的快报
    ui->label->setVisible(false);
    ui->label_2->setVisible(false);
    */

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

void DialogIdentify::WakeFromSleep()
{
    Exectime etm(__FUNCTION__);
    //Show hidden widgets
    ShowLeftWgt(true);


    ui->wgtRight->show();
    ui->labBorder->show();

    // 大军从新疆发来的快报  begin
    //ui->label->setVisible(true);
    //  ui->label_2->setVisible(true);
    // 大军从新疆发来的快报  begin

    /*修改为上面大军从新疆发来的快报
    ui->label->setVisible(true);
    ui->label_2->setVisible(true);
    */
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
    Exectime etm(__FUNCTION__);
    // 获取气象数据
    weatherdata data = m_weather->GetWeatherData();

    // 显示气象数据
    QString picPath = data.pic;
    ui->labWeatherIMG->setPixmap( QPixmap(picPath) );
    ui->labCityName->setText( data.city );
    ui->labWeather->setText( data.weather1 );
    ui->labTemp->setText( data.temp1 + "~" + data.temp2 );
    ui->labWinp->setText( data.wind1 );

    if(data.weather1 != "")
    {
        _ptimerUpate->stop();
        _ptimerUpate->start(1000*3600*12);

    }

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
    _ptimerUpate->stop();
    _ptimerUpate->start(1000);

}


void * DialogIdentify::ThreadSound(void *arg)
{
    if(NULL == arg)
    {
        return (void *)0;
    }
    DialogIdentify *pWnd = static_cast<DialogIdentify* >(arg);
    _isThreadRun = true;


    while(1)
    {
        if(g_isStopIdent && g_isQuitIdent)
        {
            break;
        }
        bool isNeedUpdate = true; //pWnd->ifUpdatePosition;
        qDebug()<< int(pWnd->_interactionResultType)<<endl;
        if( IdenSuccess == pWnd->_interactionResultType)
        {

            QString cmd = "ekho -v \"Mandarin\" -s 0  $"+(g_IdentName+"识别通过");
            system(cmd.toStdString().c_str());
            qDebug()<<"播放声音 step 1"<<endl;

        }

        //sleep(1);


    }



    //

}

