///*****************************************************************************
//** 文 件 名： IrisLogin.cpp
//** 主 要 类： IrisLogin
//**
//** Copyright (c) 中科虹霸有限公司
//**
//** 创 建 人：刘中昌
//** 创建时间：20013-10-24
//**
//** 修 改 人：
//** 修改时间：
//** 描  述:   虹膜登陆
//** 主要模块说明: 应用虹膜识别登陆
//**
//** 版  本:   1.0.0
//** 备  注:  测试人脸虹膜一起显示时使用    实际应用已经去掉 本类目前已经不再使用 2014-12-25 fjf
//**
//*****************************************************************************/
#include "irislogin.h"

#include <unistd.h>
#include <iostream>
//#include "IrisLogin.h"
//#include "ui_dialogidentify.h"
#include "ui_irislogin.h"
#include "time.h"
#include "QDateTime"
#include "QTime"
#include <QtCore>
#include "QThread"
#include <sys/time.h>
#include <QMessageBox>
#include "../ikIOStream/interface.h"
#include "../common.h"
#include "../XML/xml/IKXmlApi.h"
#include "../IrisAppLib/IrisManager/uuidControl.h"
#include "../Common/Logger.h"
#include "../Common/Exectime.h"

//信号槽链接类
static ReDrawEvent drawEvent;
static IdentEvent identEvent;
static CommonFunction commonFunction;

static LRIrisClearFlag lrIrisClearFlag;
static IrisPositionFlag irisPositionFlag;

static IrisTraceInfo s_leftTraceInfo;
static IrisTraceInfo s_rightTraceInfo;

bool IrisLogin::g_isClose;

static int s_faceOK = 0;
static int s_faceError = 0;
Interaction *IrisLogin::_identInteraction;

//测试
static time_t  _timeBegin;



IrisLogin::IrisLogin(QWidget *parent) :
	QWidget(parent),
	ui(new Ui::IrisLogin),
	_sm(SerialMuxDemux::GetInstance()),
    _irisManager(SingleControl<IrisManager>::CreateInstance())
{
	Exectime etm(__FUNCTION__);
	ui->setupUi(this);

	/**********界面设置*****************/
	//获取系统现在的时间
	// QDateTime time = QDateTime::currentDateTime();
	// QString strTime = time.toString("yyyy-MM-dd hh:mm:ss");//设置系统时间显示格式
	//ui->labTime->setText(strTime);
	//ui->labIP->setText(QString(NetInfo::GetIP().c_str()));
	ui->toolBtnBell->setStyleSheet("background-color:transparent");
	//设置界面背景
	QPalette palette;
	palette.setBrush(QPalette::Background, QBrush(QPixmap(":/image/back3.jpg")));
	ui->widgetInfo->setPalette(palette);


	//_isClose = false;
	_isLoginIdent = false;
	_isOutWigan = true;
	_isOutRalay = true;
	_defaultIdentMode = true;
	_minOutRalay = 0;
	_secOutRalay = 30;

	/*********获取配置信息*****************/
	GetConfig();

	//新建定时器 关联当前时间
	this->startTimer(1000);

	connect(&drawEvent,SIGNAL(sigDrawIdent()),this,SLOT(imageUpdate()));
	connect(&identEvent,SIGNAL(sigResult(long,int)),this,SLOT(identResult(long,int)));
	connect(&identEvent,SIGNAL(sigLoginResult(long,int)),this,SLOT(loginIdentResult(long,int)));
	connect(&identEvent,SIGNAL(sigInteractionStatus(bool)),this,SLOT(interactionStatus(bool)));
	connect(this,SIGNAL(sigUpdateImage()),this,SLOT(imageUpdateForFace()));

	//初始化算法模块
	_identApi = new CAlgApi();
	_identApi->Init();

	//人脸初始化
	//    _faceCamer = new CCameraApi();
	//    _faceCamer->Init();
	_faceCamer->GetImgInfo(&_facephotoSize);
	_facephotoBuf = new unsigned char[_facephotoSize.ImgSize * 2 * sizeof(unsigned char)];
	InitCamera();

	//人机交互
	_identInteraction = Interaction::GetInteractionInstance();
	_irisClearFlag = LAndRImgBlur;
	_identResultState = ContinueState;

	SetIdentMode();

	//    _stopIdent =false;

	//新建定时器
	_timerStartIdent = new QTimer();
	//关联定时器计满信号和相应的槽函数--识别成功失败需要在屏幕显示结果1s
	connect(_timerStartIdent,SIGNAL(timeout()),this,SLOT(isStartAttendent()));
	//门铃响后定时关闭
	connect(&_bellTimer,SIGNAL(timeout()),this,SLOT(closeBell()));

	//启动线程显示图像
	_identCapImgThread = std::thread(IdentCapImgThread, (void*)this);
}

void IrisLogin::GetConfig()
{
	Exectime etm(__FUNCTION__);

	//    bool  _isOutWigan;
	//    bool  _isOutRalay;

	//    bool  _defaultIdentMode;
	//    std::string temp;
	IKXml::IKXmlDocument doc;
	if(doc.LoadFromFile("ikembx00.xml"))
	{
		if(doc.GetElementValue("/IKEMBX00/configures/identify/mode") == "false")
		{
			_defaultIdentMode = false;
		}
		else
		{
			_defaultIdentMode = true;
		}

		if(doc.GetElementValue("/IKEMBX00/configures/identify/bellTime").empty())
		{
			_bellTimes =5;
		}
		else
		{
			_bellTimes = QString(doc.GetElementValue("/IKEMBX00/configures/identify/bellTime").c_str()).toInt();
		}
	}
}

void IrisLogin::SetIdentMode()
{
	Exectime etm(__FUNCTION__);
	if(_defaultIdentMode)
	{
		//        StartAttendIdent();
		if(!(_identInteraction->RegisterCallBack("IrisLogin",IdentInteractionStatusCallBack)))
		{
			std::cout<<"人机交互注册函数失败！"<<std::endl;
		}
	}
	else
	{
		SetSerialCallBack();
	}
}

IrisLogin::~IrisLogin()
{
	Exectime etm(__FUNCTION__);
	//线程join
	_identCapImgThread.join();

	// _faceCamer->Release();
	delete _facephotoBuf;
	delete _timerStartIdent;
	delete _identApi;
	delete _identInteraction;
	delete ui;
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
void IrisLogin::sendBell()
{
	Exectime etm(__FUNCTION__);

	_bellTimer.start(_bellTimes * 1000);
	ui->toolBtnBell->setIcon(QIcon(":/image/loading.gif"));//AQUA BELL.png
	unsigned char *cmd = "0x0d0e0000";
	_sm->Send(cmd,4);
	std::cout<<"发送门铃事件****************"<<std::endl;
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
void IrisLogin::closeBell()
{
	Exectime etm(__FUNCTION__);
	_bellTimer.stop();
	unsigned char *cmd = "0x0d0f0000";
	_sm->Send(cmd,4);
	ui->toolBtnBell->setIcon(QIcon(":image/GOLD BELL.png"));
	std::cout<<"发送关闭门铃事件"<<std::endl;
}

/*****************************************************************************
 *                        是否开启 管理员登陆识别
 *  函 数 名： IsStartLoginIdent
 *  功    能：  是否开启 管理员登陆识别
 *  说    明：
 *  参    数：
 *  返 回 值：
 *  创 建 人：刘中昌
 *  创建时间：2014-04-01
 *  修 改 人：
 *  修改时间：
 *****************************************************************************/
void IrisLogin::isStartLoginIdent()
{
	Exectime etm(__FUNCTION__);

	ui->labIsLogin->setText(QString::fromUtf8("管理员登陆识别"));
	g_isClose = false;
	_isLoginIdent =true;
	StartLoginIdent();
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
void IrisLogin::isStartAttendent()
{
	Exectime etm(__FUNCTION__);
	_identResultState = ContinueState;
	_timerStartIdent->stop();
	ui->labStatu->setText(QString::fromUtf8("识别中..."));

	if(!g_isClose && !_isLoginIdent)
	{
		StartIdent();
	}
	else if(!g_isClose && _isLoginIdent)
	{
		StartLoginIdent();
	}
	else
	{
		std::cout<<"停止识别  ： _isClose :"<<g_isClose <<"_isLoginIdent :"<<_isLoginIdent <<std::endl;
	}
	//_timerStartIdent->stop();
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
void IrisLogin::LoadCodeList()
{
	Exectime etm(__FUNCTION__);

    // common by liushu
	//初始化
    //_irisManager.Init();

	//加载普通人员特征
	_irisManager.GetIrisFeature(IdentifyType::Left,_codeListL,_codeNumL);
	_irisManager.GetIrisFeature(IdentifyType::Right,_codeListR,_codeNumR);


	//加载管理员特征
	_irisManager.GetSuperFeature(IdentifyType::Left,_loginCodeListL,_loginCodeNumL);
	_irisManager.GetSuperFeature(IdentifyType::Right,_loginCodeListR,_loginCodeNumR);

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
int IrisLogin::SetIdentParam()
{
	Exectime etm(__FUNCTION__);
	int funResult = E_ALGAPI_DEFAUL_ERROR;

	funResult = _identApi->GetParam(&_idenParamStruct);
	if(E_ALGAPI_OK == funResult)
	{
		_idenParamStruct.EyeMode = AnyEye;
		funResult = _identApi->SetParam(&_idenParamStruct);
	}
	return funResult;
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
void IrisLogin::StartAttendIdent()
{
	Exectime etm(__FUNCTION__);

	//加载虹膜特征
    LoadCodeList();
	//设置识别模式
	SetIdentParam();
	//初始化人脸
	//_faceCamer->Init();

	// ui->labIP->setText( QString(NetInfo::GetIP().c_str()));

	//设置初始状态和标志位---防止返回识别界面时仍然绘制成功或失败
	_identResultState = UnKnownState;

	//    _timerStartIdent->stop();
	ui->labIsLogin->setText(QString::fromUtf8("虹膜识别"));

	g_isClose = false;
	_isLoginIdent = false;

	//由于刷新图片为之前存储的，为防止直接被识别通过设置此延时，
	//延时时间可以测试确定
	//    struct timespec slptm;
	//    slptm.tv_sec = 0;
	//    slptm.tv_nsec = 1000 * 100 * 1000;      //1000 ns = 1 us
	//    nanosleep(&slptm,0);
	//    _timerStartIdent->start(500);

	StartIdent();
	QDateTime time = QDateTime::currentDateTime();
	QString strTime = time.toString("yyyy-MM-dd hh:mm:ss");
	std::cout<<strTime.toStdString() + "进入识别识别！ "<<std::endl;
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
void IrisLogin::StartIdent()
{
	Exectime etm(__FUNCTION__);
	//    if(_stopIdent )
	//    {
	//        return;
	//    }
	_isLoginIdent = false;

	int error = _identApi->AsyncStartIden(IdentCallBack,
			_codeListL, _codeNumL,_codeListR, _codeNumR);

	if(error!=0)
	{
		std::cout<<"开始识别失败！"<<std::endl;
		LOG_ERROR("Ident Failed");
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
void IrisLogin::StartLoginIdent()
{
	Exectime etm(__FUNCTION__);
	int error = _identApi->AsyncStartIden(LoginIdentCallBack,
			_loginCodeListL, _loginCodeNumL,_loginCodeListR,_loginCodeNumR);
	if(error!=0)
	{
		std::cout<<"管理员开始识别失败！"<<std::endl;
		LOG_ERROR("Admin Ident Failed");
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
void IrisLogin::quitIdent()
{
	Exectime etm(__FUNCTION__);
	if(g_isClose)
	{
		LOG_INFO("Ident is not run.");
		return;
	}
	g_isClose = true;

	//std::cout<<"_isClose :"<<_isClose <<std::endl;
	//停止识别
	StopIdent();
	//释放人脸API
	//_faceCamer->Release();
	_identResultState = UnKnownState;

    // common by liushu
	//删除虹膜特征
    //_irisManager.ReleaseFeature();
    //_irisManager.ReleaseSuperData();

	QDateTime time = QDateTime::currentDateTime();
	QString strTime = time.toString("yyyy-MM-dd hh:mm:ss");
	std::cout<<strTime.toStdString() + "退出识别！ "<<std::endl;

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
void IrisLogin::StopIdent()
{
	Exectime etm(__FUNCTION__);


	int error = _identApi->StopIden();
	ui->labStatu->setText(QString::fromUtf8("停止识别"));
	if(error != 0)
	{
		std::cout<<"停止识别失败！ "<<std::endl;
	}

	QDateTime time = QDateTime::currentDateTime();
	QString strTime = time.toString("yyyy-MM-dd hh:mm:ss");
	std::cout<<strTime.toStdString() + "停止识别！ "<<std::endl;
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
int IrisLogin::IdentCallBack(int funResult, APIIrisInfo* recIrisL, APIIrisInfo* recIrisR, LRSucFailFlag lrFlag,
		APIMatchOutput* pLeftMatchOutputParam, APIMatchOutput* pRightMatchOutputParam)
{
	Exectime etm(__FUNCTION__);
	if(funResult ==1)
	{//人为停止
		std::cout<<"人为停止 funResult 为："<< funResult <<std::endl;
		LOG_ERROR("IdentCallBack stop. funResult:%d",funResult);
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
int IrisLogin::LoginIdentCallBack(int funResult, APIIrisInfo* recIrisL, APIIrisInfo* recIrisR, LRSucFailFlag lrFlag,
		APIMatchOutput* pLeftMatchOutputParam, APIMatchOutput* pRightMatchOutputParam)
{
	Exectime etm(__FUNCTION__);
	if(funResult ==1)
	{//人为停止
		LOG_ERROR("LoginIdentCallBack stop. funResult:%d",funResult);
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
			default:
				std::cout << "[IrisLogin.LoginIdentCallBack]unhandled iden flag(LRSucFailFlag) case, value: " << lrFlag << std::endl;
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
				std::cout << "[IrisLogin.LoginIdentCallBack]unhandled iden flag(LRSucFailFlag) case, value: " << lrFlag << std::endl;
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
void IrisLogin::identResult(long personIndex,int flag)
{
	Exectime etm(__FUNCTION__);
	if(personIndex<0)
	{
		ui->labStatu->setText(QString::fromUtf8("识别失败"));
		_identResultState = FailedState;
		SavePersonFaceImage("",false);
	}
	else if(3 == flag)
	{
		ui->labStatu->setText(QString::fromUtf8("识别中..."));
		_identResultState = ContinueState;
	}
	else
	{

		PeopleInfo person;
		if(0 == flag)
		{
			_irisManager.SelectInfoFromFeature(personIndex,person,IdentifyType::Left);
			//左眼
			ui->labStatu->setText(QString(person.Name.c_str()) + QString::fromUtf8("  识别成功"));
		}
		else
		{
			_irisManager.SelectInfoFromFeature(personIndex,person,IdentifyType::Right);

			//右眼
			ui->labStatu->setText(QString(person.Name.c_str())  + QString::fromUtf8("  识别成功"));
		}
		_identResultState = SuccessState;

		//将数据存储的数据库
		SaveRecogToDB(person);
		//保存人脸
		//SavePersonFaceImage(QString(person.Name.c_str()),true);

		SendSingnalToSerial(person);
	}

	if(!g_isClose && !_isLoginIdent && _identResultState == ContinueState)
	{
		//异步识别识别
		isStartAttendent();
		//        StartIdent();
	}
	else
	{
		// std::cout<<"停止识别  _isClose 为："<< _isClose <<" _isLoginIdent 为："<< _isLoginIdent<<std::endl <<" _identResultState 为："<< _identResultState <<std::endl;
	}
}

/*****************************************************************************
 *                       存储人脸图片函数
 *  函 数 名：SavePersonFaceImage
 *  功    能：存储识别 成功 或 失败 时人员人脸图片
 *  说    明：
 *  参    数：
 *  返 回 值：
 *  创 建 人：刘中昌
 *  创建时间：2014-04-02
 *  修 改 人：
 *  修改时间：
 *****************************************************************************/
void IrisLogin::SavePersonFaceImage(QString name, bool isSuccess)
{
	Exectime etm(__FUNCTION__);
	int funResult = E_ALGAPI_DEFAUL_ERROR;
	QString resultName = "F";

	if(isSuccess)
	{
		resultName = name + "T";
	}
	//识别通过后保存人脸图像
	//    if(nullptr == _faceCamer)
	//    {
	//        return;
	//        //        _faceCamer->Init();
	//    }
	//    else
	//    {
	//        funResult = _faceCamer->CapImg(_facephotoBuf);
	//        if(!funResult)
	//        {
	//            QImage image = QImage(_facephotoBuf, _facephotoSize.ImgWidth,_facephotoSize.ImgHeight, QImage::Format_RGB888);
	//            //QString str = GetFacePhotoPath() + "/" + GetFacePhotoSonPath(QDate::currentDate().dayOfWeek()) +"/"
	//            //+ QString(person.Name.c_str())+ QDateTime::currentDateTime()
	//            //.toString("yyyy-MM-dd hh:mm:ss") +".jpg";
	//            //std::cout<<str.toStdString()<<endl;
	//            //"/" +  GetFacePhotoSonPath(QDate::currentDate().dayOfWeek()) +
	//            image.save(GetFacePhotoPath() + "/"
	//                       + QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss") + resultName + ".jpg","JPG");
	//        }
	//        else
	//        {
	//            std::cout<<"人脸图像采集获取图片失败！"<<std::endl;
	//        }
	//    }
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
void IrisLogin::SaveRecogToDB(PeopleInfo& person)
{
	Exectime etm(__FUNCTION__);
	int personRecMaxID = 0;
	_dbAccess.Query("select max(reclogid) from personreclog; ",personRecMaxID);
	PersonRecLog personRec;
	personRec.recLogID = personRecMaxID + 1;
	personRec.irisDataID = person.FeatureID;
	personRec.personID = person.Id;
	personRec.devType = 6;//上下班
	personRec.recogType = 1;//虹膜识别
	personRec.devSN = "dgsfdsg";//暂时为提供mac
	//    personRec.devSeq =1;

	personRec.recogTime =QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss").toStdString();// QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss");

	std::ostringstream oss;
	oss<<" INSERT INTO personreclog ( reclogid,  personid,  irisdataid, recogtime, recogtype,  atpostion, ";
	oss<<"  devsn, devtype)  values ( :reclogid,  :personid,  :irisdataid, :recogtime,";
	oss<<" :recogtype,  :atpostion, :devsn, :devtype); ";

	if(dbSuccess != _dbAccess.Insert(oss.str(),personRec))
	{
		std::cout<<"识别记录存储数据库错误！"<<std::endl;
	}
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
void IrisLogin::SendSingnalToSerial(PeopleInfo& person)
{
	Exectime etm(__FUNCTION__);
	//    Person detailPerson;
	//    QString query = "select * from person where personid = \'";
	//    UuidControl uc;
	//    query += uc.StringFromUuid(person.Id).c_str();
	//    query +="\' ; ";
	//    _dbAccess.Query( query.toStdString(),detailPerson);

	//    //向韦根发信号
	//    if(_isOutWigan)
	//    {
	//        if(_outMode == WiganOutMode::OutWorkSN)
	//        {
	//            //向韦根发工号信息
	//            int wigan = 0;
	//            wigan |= (0x0a <<24);
	//            wigan |= (QString(detailPerson.workSn.c_str()).toInt() & 0x00FFFFFF);

	//            _sm->Send((unsigned char *)&wigan,4);
	//        }
	//        else
	//        {
	//            //向韦根发卡号信息
	//            int wigan = 0;
	//            wigan |= (0x0b <<24);
	//            wigan |= (strtol(detailPerson.IDcard.c_str(),0,16) & 0x00FFFFFF);

	//            _sm->Send((unsigned char *)&wigan,4);
	//        }
	//    }
	//    //向继电器发信号
	//    if(_isOutRalay)
	//    {
	//        int ralay;
	//        ralay |= (0x0b << 24);
	//        ralay |= (0x03 << 16);
	//        ralay |= (_minOutRalay & 0x000000FF) << 8;
	//        ralay |= (_secOutRalay & 0x000000FF);
	//        _sm->Send((unsigned char *)& ralay,4);
	//    }

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
void IrisLogin::loginIdentResult(long personIndex,int flag)
{
	Exectime etm(__FUNCTION__);
	if(personIndex<0)
	{
		ui->labStatu->setText(QString::fromUtf8("识别失败"));
		_identResultState = FailedState;
	}
	else if(3 == flag)
	{
		ui->labStatu->setText(QString::fromUtf8("识别中..."));
		_identResultState = ContinueState;
	}
	else
	{
		_identResultState = SuccessState;

		//ui->labStatu->setText(_loginPersonName[personIndex] + QString::fromUtf8("识别成功"));
		emit sigLoginSucceed(true);
		//停止识别
		quitIdent();
		LOG_ERROR("Ident quit.");
		return;

	}
	//    std::lock_guard<std::mutex> lck (_identCapImgMutex, std::adopt_lock);

	if(!g_isClose && _isLoginIdent && _identResultState == ContinueState )
	{
		//异步识别识别
		StartLoginIdent();
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
void IrisLogin::interactionStatus(bool flag)
{
	Exectime etm(__FUNCTION__);
	//std::cout<<"InteractionStatus(bool flag) :"<<flag <<std::endl;
	SetStatus(!flag);
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
void IrisLogin::IdentInteractionStatusCallBack(IfHasPerson hasPerson)
{
	Exectime etm(__FUNCTION__);
	std::cout<<"IfHasPerson :"<<hasPerson<<std::endl;
	if(g_isClose)
	{
		LOG_INFO("Ident not run.");
		return;
	}
	if( hasPerson != NoPerson)
	{
		identEvent.onInteractionStatus(true);
	}
	else
	{
		identEvent.onInteractionStatus(false);
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
void IrisLogin::SetStatus(bool isStop)
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
		if(g_isClose)
		{
			LOG_INFO("Ident not run.");
			return;
		}
		StartIdent();
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
void IrisLogin::InitSerialMuxDemux()
{
	Exectime etm(__FUNCTION__);

	if(_sm && !_sm->Init(GetSerialPortName()))
	{
		std::cout<<"初始化串口失败！"<<std::endl;
		LOG_ERROR("SerialPort Init failed. %s",GetSerialPortName());
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
void IrisLogin::SetSerialCallBack()
{
	Exectime etm(__FUNCTION__);
	unsigned char cmd = 0x0f;
	if(_sm && !_sm->AddCallback(cmd, RecvCardCallBack))
	{
		std::cout<<"串口分发类注册回调函数失败！"<<std::endl;
		LOG_ERROR("SerialPort AddCallback failed. %02X",cmd);
		return;
	}
}

/*****************************************************************************
 *                        串口回调函数
 *  函 数 名：initSerialMuxDemux
 *  功    能：初始化串口分发类，并注册读取IC卡的回调函数
 *  说    明：
 *  参    数：
 *  返 回 值：
 *  创 建 人：liuzhch
 *  创建时间：2014-04-02
 *  修 改 人：
 *  修改时间：
 *****************************************************************************/
void IrisLogin::RecvCardCallBack(const unsigned char* data, const int size)
{
	Exectime etm(__FUNCTION__);
	if( !g_isClose && nullptr != data && size >0)
	{
		identEvent.onInteractionStatus(true);
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
void IrisLogin::ReleaseSerialMuxDemux()
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
void IrisLogin::timerEvent(QTimerEvent *event)
{
	Exectime etm(__FUNCTION__);
	//获取系统现在的时间
	//   QDateTime time = QDateTime::currentDateTime();
	//设置系统时间显示格式
	//  QString strTime = time.toString("yyyy-MM-dd hh:mm:ss");
	//界面显示
	//ui->labTime->setText(strTime);// + QString::fromUtf8("  帧率： ") + QString::number(_count));
	ui->labIris->setText(QString::fromUtf8("虹膜帧率：") + QString::number(_count));
	ui->labFaceOK->setText(QString::fromUtf8("人脸帧率：") + QString::number(s_faceOK));
	ui->labFaceError->setText(QString::fromUtf8("人脸错误帧率：") + QString::number(s_faceError));
	_count =0;
	s_faceOK = 0;
	s_faceError = 0;
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
void IrisLogin::imageUpdate()
{
	Exectime etm(__FUNCTION__);

	std::lock_guard<std::mutex> lck (_identCapImgMutex, std::adopt_lock);

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
	SetProgressBarValue( irisPositionFlag);
	QImage imageBig;
	imageBig = QImage::fromData(_grayImgBuffer, g_constIrisImgSize + g_constBMPHeaderLength, "BMP");
	if(!imageBig.isNull())
	{
		QImage paintImg = imageBig.convertToFormat(QImage::Format_ARGB32_Premultiplied);


		//绘框
		_identInteraction->PlayInteraction(_interactionResultType,lrIrisClearFlag,irisPositionFlag, false, paintImg,
				s_leftTraceInfo, s_rightTraceInfo);


		ui->labEye->setPixmap(QPixmap::fromImage(paintImg));
		_count ++;
	}
	else
	{
		std::cout<<"识别显示未获得图像！"<<std::endl;
		LOG_ERROR("Ident Get Img  failed.");
	}
}

/*****************************************************************************
 *                           识别模块采图线程
 *  函 数 名：IdentCapImgThread
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
void* IrisLogin::IdentCapImgThread(void* arg)
{
	Exectime etm(__FUNCTION__);
	// pthread_t tid = pthread_self();
	//std::cout<<"线程 _identCapImgThread TID:"<<tid<<"线程 _identCapImgThread PID :"<<(unsigned)getpid()<<std::endl;
	IrisLogin *pCGDlg = static_cast <IrisLogin*> (arg);

	int errorResult = E_ALGAPI_ERROR_BASE;

	while(true)
	{
		if(!pCGDlg->g_isClose)
		{
			//std::lock_guard<std::mutex> lck (pCGDlg->_identCapImgMutex, std::adopt_lock);

			errorResult = pCGDlg->_identApi->SyncCapIrisImg(pCGDlg->_identInteraction->s_infraredDistanceValue, pCGDlg->_grayImgBuffer,
					lrIrisClearFlag, irisPositionFlag);
			if(E_ALGAPI_OK == errorResult)
			{
				//更新界面图像信号
				drawEvent.OnDrawIdent();
				// std::cout<<" 更新界面图像信号-----"<<std::endl;
			}
		}
		else
		{
			//防止CPU占用率过高
			struct timespec slptm;
			slptm.tv_sec = 0;
			slptm.tv_nsec = 1000;      //1000 ns = 1 us
			nanosleep(&slptm,0);
		}
	}
	return (void*)0;
}

void IrisLogin::imageUpdateForFace()
{
	Exectime etm(__FUNCTION__);
	QImage* pImage = new QImage(_facephotoBuf, _facephotoSize.ImgWidth, _facephotoSize.ImgHeight , QImage::Format_RGB888);
	pImage->loadFromData(_facephotoBuf, _facephotoSize.ImgSize * 2 * sizeof(unsigned char));


	QImage img(_facephotoSize.ImgWidth , _facephotoSize.ImgHeight , QImage::Format_RGB16);
	img = pImage->copy(0, 0, pImage->width(), pImage->height());
	img = img.scaled(ui->labFace->width(), ui->labFace->height(), Qt::KeepAspectRatio);

	QPixmap pixMap;
	pixMap = QPixmap::fromImage(img.mirror(true,true), Qt::AutoColor);

	QPainter painterRectangle(&pixMap);
	painterRectangle.setRenderHint(QPainter::Antialiasing,true);
	//    painterRectangle.setPen(QPen(Qt::green, 5, Qt::SolidLine, Qt::RoundCap));
	//    painterRectangle.drawRect(xLeft,yLeft,g_constIKImgWidth,g_constIKImgHeight);//24,112
	//    painterRectangle.drawRect(xRight,yRight,g_constIKImgWidth,g_constIKImgHeight);//1384,112

	ui->labFace->setPixmap(pixMap);


	////    QImage pImage;// = new QImage(_facephotoBuf, _facephotoSize.ImgHeight, _facephotoSize.ImgWidth, QImage::Format_RGB888);
	////    pImage = QImage::fromData(_facephotoBuf, _facephotoSize.ImgSize * 3 * sizeof(unsigned char));
	//    QImage img(_facephotoSize.ImgWidth , _facephotoSize.ImgHeight , QImage::Format_RGB888);
	//    img = QImage::fromData(_facephotoBuf, _facephotoSize.ImgSize * 3 * sizeof(unsigned char));
	//    ui->labEye->setPixmap(QPixmap::fromImage(img));
}

bool IrisLogin::InitCamera()
{
	Exectime etm(__FUNCTION__);
	_faceCamer = new CCameraApi();
	if(E_ALGAPI_OK !=  _faceCamer->Init())
	{
		//        QMessageBox::critical(this, QString::fromUtf8("初始化人脸图像采集设备"), QString::fromUtf8("API初始化失败或未初始化"));
		//        IKMessageBox errorNote(this, QString::fromUtf8("初始化人脸图像采集设备"), QString::fromUtf8("API初始化失败或未初始化"), errorMbx);

		//        errorNote.exec();
		std::cout << "[ERROR@" << __FUNCTION__ << "]:" << "init facecamer failed" << std::endl;
		LOG_ERROR("Init Facecamer failed.");
		return false;
	}


	_enableCapture = true;
	//启动线程显示图像
	_identFaceImagThread = std::thread(GetFacePhoto, (void*)this);
	return true;
}

void IrisLogin::StopCapturing()
{
	Exectime etm(__FUNCTION__);
	_enableCapture = false;
	_identFaceImagThread.join();
	_faceCamer->Release();
}

void IrisLogin::progressBarValueChanged(int value)
{
	Exectime etm(__FUNCTION__);

	ui->progressBar->setValue(value);
	ui->progressBar->setStyleSheet(QString(""
				"    QProgressBar {"
				"    border: 2px solid grey;"
				"    border-radius: 5px;"
				""
				"}"

				"QProgressBar::chunk {"
				"    background-color: "
				"%1;"
				"}").arg(CalcValue(value,ui->progressBar->maximum())));
}

void IrisLogin::SetProgressBarValue(IrisPositionFlag positionFlag)
{
	Exectime etm(__FUNCTION__);
	switch(positionFlag)
	{
		case Far:
			ui->progressBar->setValue(3);
			break;
		case Near:
			ui->progressBar->setValue(1);
			break;
		case OK:
			ui->progressBar->setValue(2);
			break;
		default:
			ui->progressBar->setValue(0);
			break;
	}
}

QString IrisLogin::CalcValue(int nValue,int nTotal)
{
	Exectime etm(__FUNCTION__);
	QString strStyle = "";
	int value1 = (float)nTotal/(float)3;
	int value2 = (float)nTotal/(float)3;

	if (nValue > value1)
	{
		QString str1;
		QString str2;
		float scale1 = (float)value1 / (float)nValue;
		float scale2 = scale1 + 0.000004;
		str1 = str1.setNum(scale1,'f',6);
		str2 = str2.setNum(scale2,'f',6);

		if (nValue > (value1 + value2))
		{
			QString str3;
			QString str4;

			float scale3 =(float)(value1+value2)/nValue;// 2.0 * scale1;
			float scale4 = scale3 + 0.000004;
			str3 = str3.setNum(scale3,'f',6);
			str4 = str4.setNum(scale4,'f',6);
			strStyle = QString("qlineargradient(spread:pad, x1:0, y1:1, x2:0, y2:0, \
					   stop:0 rgba(255, 255, 0, 255), stop:%1 rgba(255, 255, 0, 255), \
					stop:%2 rgba(0, 210, 60, 255), stop:%3 rgba(0, 210, 60, 255) \
					stop:%4 rgba(255, 0, 0, 255), stop:1 rgba(255, 0, 0, 255))")
					   .arg(str1)
					   .arg(str2)
					   .arg(str3)
					   .arg(str4);
		}
		else
		{
			strStyle = QString("qlineargradient(spread:pad, x1:0, y1:1, x2:0, y2:0, stop:0 rgba(255, 255, 0, 255), stop:%1 rgba(255, 255, 0, 255), stop:%2 rgba(0, 210, 60, 255), stop:1 rgba(0, 210, 60, 255))")
				.arg(str1)
				.arg(str2);
		}
	}
	else
	{
		strStyle = "rgba(255, 255, 0, 255)";
	}
	return strStyle;
}




/*****************************************************************************
 *                        人脸图像采集处理函数
 *  函 数 名：GetFacePhoto
 *  功    能：调用人脸采集图像接口，采集到图像后发射信号，供图像处理类使用
 *  说    明：
 *  参    数：
 *  返 回 值：
 *  创 建 人：liuzhch
 *  创建时间：2014-04-08
 *  修 改 人：
 *  修改时间：
 *****************************************************************************/
void* IrisLogin::GetFacePhoto(void* arg)
{
	Exectime etm(__FUNCTION__);
	//TO DO
	if(nullptr == arg)
	{
		LOG_ERROR("arg is null.");
		return (void *)0;
	}

	IrisLogin* cp = static_cast<IrisLogin *>(arg);

	//防止CPU占用率过高
	struct timespec slptm;
	slptm.tv_sec = 0;
	slptm.tv_nsec = 30*1000*1000;      //1000 ns = 1 us
	while(cp->_enableCapture)
	{
		struct timeval tvEnrStart, tvEnrEnd;
		gettimeofday(&tvEnrStart,nullptr);
		//TO DO
		if(E_ALGAPI_OK == cp->_faceCamer->CapImg(cp->_facephotoBuf))
		{
			gettimeofday(&tvEnrEnd,nullptr);
			std::cout << "人脸获取成功时间：" << tvEnrEnd.tv_usec - tvEnrStart.tv_usec << std::endl;
			emit cp->sigUpdateImage();
			std::cout<<"CapImg() 调用成功！"<<std::endl;
			s_faceOK++;


		}
		else
		{
			gettimeofday(&tvEnrEnd,nullptr);
			std::cout << "人脸获取失败时间：" << tvEnrEnd.tv_usec - tvEnrStart.tv_usec << std::endl;
			s_faceError++;
			// std::cout<<"cp->_faceCamer->CapImg() 调用失败！"<<std::endl;
		}

		//防止系统误误判死循环
		nanosleep(&slptm,0);

	}
	std::cout<<"线程GetFacePhoto退出！cp->_enableCapture : "<<cp->_enableCapture<<std::endl;
	return (void *)0;
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
QString IrisLogin::GetFacePhotoPath()
{
	Exectime etm(__FUNCTION__);
	QString facePhotoDir("recogface");
	QDir dir(QDir::currentPath());

	if(!dir.exists(facePhotoDir))
	{
		if(!dir.mkdir(facePhotoDir))
		{
			QMessageBox::warning(this, QString::fromUtf8("创建目录"), QString::fromUtf8("无法创建保存识别成功人员头像照片的目录"));
		}

		int cmdRes = 0;
		cmdRes = system("sudo chmod 777 *");
	}
	dir.cd(facePhotoDir);
	return dir.dirName();
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
QString IrisLogin::GetFacePhotoSonPath(int index)
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
			//        if(dir.exists(facePhotoDir2))
			//        {
			//             system("sudo chmod 777 *");
			////            dir.rmdir(facePhotoDir2);
			//            dir.cd(facePhotoDir2);
			//            dir.remove("/*.jpg");
			//            dir.cdUp();
			//        }

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
defaut:
			break;
	}
	return dir.dirName();
}

