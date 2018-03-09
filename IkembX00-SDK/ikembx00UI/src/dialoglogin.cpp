/*****************************************************************************
 ** 文 件 名：DialogLogin.cpp
 ** 主 要 类：DialogLogin
 **
 ** Copyright (c) 中科虹霸有限公司
 **
 ** 创 建 人：刘中昌
 ** 创建时间：20013-10-15
 **
 ** 修 改 人：
 ** 修改时间：
 ** 描  述:   登陆对话框
 ** 主要模块说明: 管理员登陆验证
 **
 ** 版  本:   1.0.0
 ** 备  注:
 **
 *****************************************************************************/
#include "dialoglogin.h"
#include "ui_dialoglogin.h"


//#include "qmessagebox.h"
#include <QMessageBox>
#include "ikmessagebox.h"
#include "../ikIOStream/interface.h"
#include "../Common/Exectime.h"

static const int g_maxCountDown = 30;
DialogLogin::DialogLogin(QWidget *parent) :
	QDialog(parent),
	ui(new Ui::DialogLogin)
{
	
	ui->setupUi(this);
	ui->labError->hide();

	connect(ui->btnCancel,SIGNAL(clicked()),this,SLOT(cancel()));
	connect(ui->txtUserName, SIGNAL(textChanged(QString)), this, SLOT(GetFocused()));
	connect(ui->txtPassWord, SIGNAL(textChanged(QString)), this, SLOT(GetFocused()));


	//隐藏对话框标题栏及对话框位置调整
	setWindowFlags(Qt::FramelessWindowHint);
	setGeometry(QRect(0, 0, 1024, 600));

	_countdown = g_maxCountDown;

	//Fill background
	ui->centralWgt->setAutoFillBackground(true);
	ui->centralWgt->setBackgroundColor(QColor(230,230,230));

	//ui->labCountDown->setText(QString::number(_countdown));
	this->startTimer(1000);
	//ui->btnResetPw->hide();

}

/*****************************************************************************
 *                        关闭函数
 *  函 数 名：closeDialog
 *  功    能：界面消失倒计时
 *  说    明：
 *  参    数：
 *  返 回 值：
 *  创 建 人：刘中昌
 *  创建时间：2014-03-26
 *  修 改 人：
 *  修改时间：
 *****************************************************************************/
void DialogLogin::closeDialog()
{
	
	//设置焦点，防止软件盘不关闭
	ui->btnCancel->setFocus();
	cancel();
}

/*****************************************************************************
 *                        时间事件函数
 *  函 数 名：timerEvent
 *  功    能：界面消失倒计时
 *  说    明：
 *  参    数：
 *  返 回 值：
 *  创 建 人：刘中昌
 *  创建时间：2014-03-26
 *  修 改 人：
 *  修改时间：
 *****************************************************************************/
void DialogLogin::timerEvent(QTimerEvent *)
{
	
	if(_countdown>= 0)
	{
		ui->labCountDown->setText(QString::number(_countdown));
		if(_countdown == 0)
		{
			cancel();
		}
		_countdown--;
	}
}

DialogLogin::~DialogLogin()
{
	
	delete ui;
}

/*****************************************************************************
 *                        判断用户登陆
 *  函 数 名：judgeUser
 *  功    能：判断是否为管理员登陆
 *  说    明：
 *  参    数：
 *  返 回 值：
 *  创 建 人：刘中昌
 *  创建时间：2013-10-16
 *  修 改 人：
 *  修改时间：
 *****************************************************************************/
void DialogLogin::judgeUser()
{
	
	Operator user;
	_dbAccess.Query("select * from operator; ",user);
	if(this->ui->txtUserName->text().toStdString() == user.name
			&& this->ui->txtPassWord->text().toStdString() == user.password)
	{
		accept();
		ui->btnCancel->setFocus();
	}
	else
	{
		//        ui->labError->show();
		SetStyleSheet(false);
		//QMessageBox::information(this,tr("失败"), tr("用户名或密码错误！请确认！"), QMessageBox::Yes, QMessageBox::Yes);
		this->ui->txtPassWord->text().clear();
		this->ui->txtUserName->text().clear();
	}
}

void DialogLogin::cancel()
{
	
	reject();
	ui->btnCancel->setFocus();
}

/*****************************************************************************
 *                        重新记时函数
 *  函 数 名：reStartTime
 *  功    能：重置倒数秒数
 *  说    明：
 *  参    数：
 *  返 回 值：
 *  创 建 人：刘中昌
 *  创建时间：2013-10-16
 *  修 改 人：
 *  修改时间：
 *****************************************************************************/
void DialogLogin::reStartTime(QString )
{
	
	ui->labError->hide();
	//_countdown = 30;
}

void DialogLogin::showEvent(QShowEvent *)
{
	
	this->activateWindow();
	ui->txtUserName->setFocus();
}


void DialogLogin::GetFocused()
{
	
	SetStyleSheet(true);
}


void DialogLogin::SetStyleSheet(bool normalStyle)
{
	
	if(normalStyle)
	{
		ui->txtPassWord->setStyleSheet("background-color: #FFFFFF;");
		ui->txtUserName->setStyleSheet("background-color: #FFFFFF;");
	}
	else
	{
		ui->txtPassWord->setStyleSheet("background-color: #FF0000;");
		ui->txtUserName->setStyleSheet("background-color: #FF0000;");
	}
}


void DialogLogin::on_btnResetPw_clicked()
{
	
	this->hide();
	dialogResetPW *dlg = new dialogResetPW (this->parent());
	dlg->exec();
	// if(QDialog::Accepted != dlg.exec());
	if(dlg->m_returnType == 0)
	{
		return ;
	}
	delete dlg;

	DatabaseAccess dbAc;
	Operator opt;
	opt.password = "123456";
	std::string query("UPDATE operator SET password = :password WHERE name = 'admin'");


	if(dbSuccess == dbAc.Update(query, opt))
	{
		IKMessageBox infoNote(this->parent(), QString::fromUtf8("提示"), QString::fromUtf8("恢复出厂密码成功"), informationMbx);
		infoNote.exec();
		// QMessageBox::warning(this,QString::fromLocal8Bit("提示"),QString::fromLocal8Bit("恢复出厂密码成功"));
	}
	else
	{
		IKMessageBox infoNote(this->parent(), QString::fromUtf8("提示"), QString::fromUtf8("数据库读写失败"), informationMbx);
		infoNote.exec();
		// QMessageBox::warning(this,QString::fromLocal8Bit("提示"),QString::fromLocal8Bit("恢复出厂密码失败"));

	}


}

