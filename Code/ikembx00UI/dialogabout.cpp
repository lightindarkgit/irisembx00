/*****************************************************************************
 ** 文 件 名：DialogAbout.cpp
 ** 主 要 类：DialogAbout
 **
 ** Copyright (c) 中科虹霸有限公司
 **
 ** 创 建 人：刘中昌
 ** 创建时间：20013-10-15
 **
 ** 修 改 人：
 ** 修改时间：
 ** 描  述:   关于界面
 ** 主要模块说明: 显示版本信息、公司信息等
 **
 ** 版  本:   1.0.0
 ** 备  注:
 **
 *****************************************************************************/
#include "dialogabout.h"
#include "ui_dialogabout.h"
#include "sysnetinterface.h"
#include <QMouseEvent>
#include "../Common/Global.h"
#include "../Common/Exectime.h"

DialogAbout::DialogAbout(QWidget *parent) :
	QWidget(parent),
	ui(new Ui::DialogAbout)
{
	Exectime etm(__FUNCTION__);
	ui->setupUi(this);
	//    this->setFocusPolicy(Qt::StrongFocus);
	//隐藏标题栏
	//setWindowFlags(Qt::CustomizeWindowHint);



	//ui->toolBtnGoBack->setStyleSheet("background-color:transparent");
	QString MAC = QString::fromStdString(SysNetInterface::GetDevSN());
	ui->label_SN->setText(MAC);

	ui->labVerValue->setText(SysNetInterface::GetSoftVer().c_str());
	connect(ui->toolBtnGoBack,SIGNAL(clicked()),this,SIGNAL(sigGoBack()));
}

DialogAbout::~DialogAbout()
{
	Exectime etm(__FUNCTION__);
	delete ui;
}



/*****************************************************************************
 *                      对话框失去焦点
 *  函 数 名：focusOutEvent
 *  功    能：对话框失去焦点后关闭
 *  说    明：
 *  参    数：
 *  返 回 值：
 *  创 建 人：刘中昌
 *  创建时间：2013-10-22
 *  修 改 人：
 *  修改时间：
 *****************************************************************************/
//void DialogAbout::focusOutEvent(QFocusEvent*)
//{
//    this->close();
//}

