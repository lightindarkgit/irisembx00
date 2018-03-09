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
#include "../XML/xml/IKXmlApi.h"
#include "commononqt.h"
#include "Logger.h"
#include "ikconfig.h"

DialogAbout::DialogAbout(QWidget *parent) :
	QWidget(parent),
	ui(new Ui::DialogAbout)
{
	
	ui->setupUi(this);
	//    this->setFocusPolicy(Qt::StrongFocus);
	//隐藏标题栏
	//setWindowFlags(Qt::CustomizeWindowHint);



	//ui->toolBtnGoBack->setStyleSheet("background-color:transparent");
	QString MAC = QString::fromStdString(SysNetInterface::GetDevSN());
	ui->label_SN->setText(MAC);

	ui->labVerValue->setText(SysNetInterface::GetSoftVer().c_str());
	connect(ui->toolBtnGoBack,SIGNAL(clicked()),this,SIGNAL(sigGoBack()));
    ShowInfo();
}

DialogAbout::~DialogAbout()
{
	
    delete ui;
}

void DialogAbout::ShowInfo()
{

    int logo= IkConfig::GetDevLogoCfg();

     QString logofile;

    if(logo == 1)
    {
        QString name = QString::fromLocal8Bit("  中科唯实科技(北京)有限公司");
        ui->label_14->setText(name);
        ui->label_11->setText("86-10-62620046-8002");
        ui->label_12->setText("北京市海淀区北四环西路37号");
        ui->label_13->setText("kefu@vistek.cn");
        ui->label->setText(name);

        logofile = ":/image/vistek.png";
    }
    else
    {
        QString name = QString::fromLocal8Bit(" 北京中科虹霸科技有限公司");
        ui->label_14->setText(name);
        ui->label_11->setText("010-62529086-8001/8006");
        ui->label_12->setText("北京市海淀区北四环西路银谷大厦709室");
        ui->label_13->setText("business@irisking.com");
        ui->label->setText(name);
        logofile = ":image/iriskingLog.ico";
    }
    QPixmap pic(logofile);
    pic = pic.scaled(ui->label_5->width(),ui->label_5->height(),Qt::IgnoreAspectRatio);
    ui->label_5->setPixmap(pic);
    ui->label_5->show();
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

