/*****************************************************************************
 ** 文 件 名：shutdown.cpp
 ** 主 要 类：Shutdown
 **
 ** Copyright (c) 中科虹霸有限公司
 **
 ** 创 建 人：L.Wang
 ** 创建时间：20013-10-18
 **
 ** 修 改 人：
 ** 修改时间：
 ** 描  述:   关机操作界面
 ** 主要模块说明: 关机， 取消关机， 重启
 **
 ** 版  本:   1.0.0
 ** 备  注:
 **
 *****************************************************************************/
#include "shutdown.h"
#include "ui_shutdown.h"
#include "buffering.h"
#include "thread"
#include "../Common/Logger.h"
#include "../Common/Exectime.h"
#include "interaction.h"


Shutdown::Shutdown(QWidget *parent) :
	QDialog(parent),
	ui(new Ui::Shutdown),
	po(PowerOpt::NONE),
	timeOut(5000),    //Wait at most 5 seconds without recieving any reply
	waitingUIPlayer(nullptr),
	waitingUIDisper(nullptr),
	operInfoDisper(nullptr)
{
	Exectime etm(__FUNCTION__);
	ui->setupUi(this);

	//Set this window's display style
	setWindowFlags(Qt::FramelessWindowHint);
	setAttribute(Qt::WA_TranslucentBackground);


	//Init waiting flash UI widget
	initWaitingUI();


	connect(ui->btnBack, SIGNAL(clicked()), this, SIGNAL(sigGoBack()));
	connect(ui->btnReboot, SIGNAL(clicked()), this, SLOT(slotReboot()));
	connect(ui->btnShutdown, SIGNAL(clicked()), this, SLOT(slotShutdown()));
	connect(eIns.GetIKEvent(), SIGNAL(sigInfraredClosed()), this, SLOT(slotManagePower()));
	connect(&timeOutTimer, SIGNAL(timeout()), this, SLOT(slotManagePower()));
	//connect(this, SIGNAL(sigCloseInfrared()), this, SLOT(slotCloseInfrared()));


	//关闭红外灯等待画面
	connect(eIns.GetIKEvent(), SIGNAL(sigInfraredClosed()), waitingUIPlayer, SLOT(stop()));
	connect(&timeOutTimer, SIGNAL(timeout()), waitingUIPlayer, SLOT(stop()));
}

Shutdown::~Shutdown()
{
	Exectime etm(__FUNCTION__);
	//Deal with QMovie, stop and destroy
	waitingUIPlayer->stop();
	delete waitingUIPlayer;

	delete ui;
}


void Shutdown::initWaitingUI()
{
	Exectime etm(__FUNCTION__);
	//初始化等待界面动画
	waitingUIPlayer = new QMovie(":/image/loading.gif");
	waitingUIPlayer->setScaledSize(QSize(100, 100));
	waitingUIPlayer->setSpeed(100);


	//初始化等待界面
	waitingUIDisper = new QLabel(this);
	waitingUIDisper->setMovie(waitingUIPlayer);
	waitingUIDisper->setAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
	waitingUIDisper->setGeometry(0, 0, 1024, 600);


	//初始化操作提示信息
	operInfoDisper = new QLabel(this);
	operInfoDisper->setAlignment(Qt::AlignHCenter | Qt::AlignVCenter);

	//设置字体
	QFont fontFormat(this->font().family(), 18);
	operInfoDisper->setStyleSheet("color: rgb(255, 255, 255)");
	operInfoDisper->setFont(fontFormat);
	operInfoDisper->setGeometry(0, 400, 1024, 32);


	waitingUIDisper->hide();
	operInfoDisper->hide();
}

/*****************************************************************************
 *                        重启系统
 *  函 数 名：slotReboot
 *  功    能：槽函数，重启系统
 *  说    明：
 *  参    数：
 *  返 回 值：
 *  创 建 人：L.Wang
 *  创建时间：2013-10-18
 *  修 改 人：
 *  修改时间：
 *****************************************************************************/
void Shutdown::slotReboot()
{
	Exectime etm(__FUNCTION__);
	//TO DO
	//设置并显示等待界面
	dispWaitingUI(QString("正在重启设备"));

	pwMgr.Reboot();

	//    //设置操作选项
	//    po = PowerOpt::REBOOT;

	//    //启动定时器，如果在timeout时间内仍未收到红外灯已关闭信号，则仍将继续执行关机或重启
	//    timeOutTimer.start(timeOut, true);
	//    //发送关闭红外灯信号
	//    eIns.GetIKEvent()->CloseInfrared();
}

/*****************************************************************************
 *                        关机
 *  函 数 名：slotShutdown
 *  功    能：槽函数，关闭设备
 *  说    明：
 *  参    数：
 *  返 回 值：
 *  创 建 人：L.Wang
 *  创建时间：2013-10-18
 *  修 改 人：
 *  修改时间：
 *****************************************************************************/
void Shutdown::slotShutdown()
{
	Exectime etm(__FUNCTION__);
	//TO DO
	//设置并显示等待界面
	dispWaitingUI(QString("正在关闭设备"));

	pwMgr.Shutdown();
	//    //设置操作选项
	//    po = PowerOpt::SHUTDOWN;

	//    //启动定时器，如果在timeout时间内仍未收到红外灯已关闭信号，则仍将继续执行关机或重启
	//    timeOutTimer.start(timeOut, true);
	//    //发送关闭红外灯信号
	//    eIns.GetIKEvent()->CloseInfrared();
}


/*****************************************************************************
 *                        屏蔽Esc键信号
 *  函 数 名：reject
 *  功    能：
 *  说    明：
 *  参    数：
 *  返 回 值：
 *  创 建 人：L.Wang
 *  创建时间：2014-08-14
 *  修 改 人：
 *  修改时间：
 *****************************************************************************/
void Shutdown::reject()
{
	Exectime etm(__FUNCTION__);
	//DO NOTHING
}


/*****************************************************************************
 *                        屏蔽Esc键信号
 *  函 数 名：slotManagePower
 *  功    能：
 *  说    明：
 *  参    数：
 *  返 回 值：
 *  创 建 人：L.Wang
 *  创建时间：2015-01-12
 *  修 改 人：
 *  修改时间：
 *****************************************************************************/
void Shutdown::slotManagePower()
{
	Exectime etm(__FUNCTION__);
	switch(po)
	{
		case PowerOpt::SHUTDOWN:
			ShutdownImp();
			break;
		case PowerOpt::REBOOT:
			RebootImp();
			break;
	}

	//Reinit member variable po
	po = PowerOpt::NONE;
}

/*****************************************************************************
 *                        屏蔽Esc键信号
 *  函 数 名：slotManagePower
 *  功    能：
 *  说    明：
 *  参    数：
 *  返 回 值：
 *  创 建 人：L.Wang
 *  创建时间：2015-01-12
 *  修 改 人：
 *  修改时间：
 *****************************************************************************/
void Shutdown::ShutdownImp()
{
	Exectime etm(__FUNCTION__);
	//TODO
	Interaction::GetInteractionInstance()->CloseUsb();
	LOG_INFO("shutdown -hf now");
	system("shutdown -hf now");
}

/*****************************************************************************
 *                        屏蔽Esc键信号
 *  函 数 名：slotManagePower
 *  功    能：
 *  说    明：
 *  参    数：
 *  返 回 值：
 *  创 建 人：L.Wang
 *  创建时间：2015-01-12
 *  修 改 人：
 *  修改时间：
 *****************************************************************************/
void Shutdown::RebootImp()
{
	Exectime etm(__FUNCTION__);
	//TO DO
	Interaction::GetInteractionInstance()->CloseUsb();
	system("shutdown -rf now");
	LOG_INFO("shutdown -rf now");
}

/*****************************************************************************
 *                        显示关机等待动画
 *  函 数 名：dispWaitingUI
 *  功    能：
 *  说    明：
 *  参    数：
 *  返 回 值：
 *  创 建 人：L.Wang
 *  创建时间：2015-01-13
 *  修 改 人：
 *  修改时间：
 *****************************************************************************/
void Shutdown::dispWaitingUI(const QString &notice)
{
	Exectime etm(__FUNCTION__);
	operInfoDisper->setText(notice);
	waitingUIPlayer->start();
	waitingUIDisper->show();
	operInfoDisper->show();
}

/*****************************************************************************
 *                        隐藏关机等待动画
 *  函 数 名：hideWaitingUI
 *  功    能：
 *  说    明：
 *  参    数：
 *  返 回 值：
 *  创 建 人：L.Wang
 *  创建时间：2015-01-13
 *  修 改 人：
 *  修改时间：
 *****************************************************************************/
void Shutdown::hideWaitingUI()
{
	Exectime etm(__FUNCTION__);
	waitingUIPlayer->stop();
	waitingUIDisper->hide();
	operInfoDisper->hide();
}

