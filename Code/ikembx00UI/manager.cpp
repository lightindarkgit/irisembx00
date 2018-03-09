/*****************************************************************************
 ** 文 件 名： Manager.cpp
 ** 主 要 类： Manager
 **
 ** Copyright (c) 中科虹霸有限公司
 **
 ** 创 建 人：刘中昌
 ** 创建时间：20013-10-24
 **
 ** 修 改 人：
 ** 修改时间：
 ** 描  述:    管理员操作菜单
 ** 主要模块说明: 管理员操作菜单
 **
 ** 版  本:   1.0.0
 ** 备  注:
 **
 *****************************************************************************/
#include <QDateTime>      //AddedBy:Wang.l 2014-12-04
#include "manager.h"
#include "ui_manager.h"
#include "shutdown.h"
#include "../common.h"
#include "commononqt.h"
#include "../Common/Exectime.h"

Manager::Manager(QWidget *parent) :
	QWidget(parent),
	ui(new Ui::Manager),
	_timer(new QTimer()),     //AddedBy:Wang.l 2014-12-04
	_weekDay(weekDay)         //AddedBy:Wang.l 2014-12-04
{
	Exectime etm(__FUNCTION__);
	ui->setupUi(this);
	ui->labCopyright->setText(tr("<b>Copyright &copy; 2014 </b>IrisKing Co. Ltd"));

	//    ui->centralWidget->setStyleSheet("background-image:url(:/images/menus_bkground.jpg);");

	//    ui->toolBtnDataManager->setStyleSheet("background-color:transparent");
	//    ui->toolBtnHelp->setStyleSheet("background-color:transparent");
	//    ui->toolBtnIdentify->setStyleSheet("background-color:transparent");
	//    ui->toolBtnPerson->setStyleSheet("background-color:transparent");
	//    ui->toolBtnShutdown->setStyleSheet("background-color:transparent");
	//    ui->toolBtnSysterm->setStyleSheet("background-color:transparent");


	connect(ui->toolBtnDataManager,SIGNAL(clicked()),this,SIGNAL(sigDataManger()));
	connect(ui->toolBtnHelp,SIGNAL(clicked()),this,SIGNAL(sigHelp()));
	connect(ui->toolBtnIdentify,SIGNAL(clicked()),this,SIGNAL(sigIdentify()));
	connect(ui->toolBtnPerson,SIGNAL(clicked()),this,SIGNAL(sigPerson()));
	connect(ui->toolBtnShutdown,SIGNAL(clicked()),this,SIGNAL(sigShutdown()));
	connect(ui->toolBtnSystem,SIGNAL(clicked()),this,SIGNAL(sigSysterm()));
	connect(_timer, SIGNAL(timeout()), this, SLOT(slotUpdateDateTime()));

	_timer->start(1000, false);     //AddedBy:Wang.l 2014-12-04
}

Manager::~Manager()
{
	Exectime etm(__FUNCTION__);
	delete ui;
}



void Manager::slotPowerManager()
{
	Exectime etm(__FUNCTION__);
	Shutdown* sd = new Shutdown(this);
	sd->exec();
}

//AddedBy:Wang.l 2014-12-04
void Manager::slotUpdateDateTime()
{
	Exectime etm(__FUNCTION__);
	std::string week = QDateTime::currentDateTime().toString("dddd").toStdString();
	if(_weekDay.count(week) > 0)
	{
		ui->labCurDateTime->setText(QString("%1  %2   %3").arg((_weekDay[week]).c_str(),
					QDateTime::currentDateTime().toString("yyyy-MM-dd"),
					QDateTime::currentDateTime().toString("hh:mm:ss")));
	}
	else
	{
		// ui->labCurDateTime->setText(QString("%1").arg(QDateTime::currentDateTime().toString("dddd yyyy-MM-dd    hh:mm:ss")));                                                          );
	}
}

