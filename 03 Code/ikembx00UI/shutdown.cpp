/*****************************************************************************
** 文 件 名：shutdown.cpp
** 主 要 类：Shutdown
**
** Copyright (c) 中科虹霸有限公司
**
** 创 建 人：王磊
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

Shutdown::Shutdown(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Shutdown)
{
    ui->setupUi(this);
    connect(ui->btnBack, SIGNAL(clicked()), this, SIGNAL(sigGoBack()));
    connect(ui->btnReboot, SIGNAL(clicked()), this, SLOT(slotReboot()));
    connect(ui->btnShutdown, SIGNAL(clicked()), this, SLOT(slotShutdown()));
}

Shutdown::~Shutdown()
{
    delete ui;
}

/*****************************************************************************
*                        重启系统
*  函 数 名：slotReboot
*  功    能：槽函数，重启系统
*  说    明：
*  参    数：
*  返 回 值：
*  创 建 人：王磊
*  创建时间：2013-10-18
*  修 改 人：
*  修改时间：
*****************************************************************************/
void Shutdown::slotReboot()
{
    //TO DO
    int rtnVal = system("shutdown -rf now");
    if(!rtnVal)
    {
        int i = 0;
    }
}

/*****************************************************************************
*                        关机
*  函 数 名：slotShutdown
*  功    能：槽函数，关闭设备
*  说    明：
*  参    数：
*  返 回 值：
*  创 建 人：王磊
*  创建时间：2013-10-18
*  修 改 人：
*  修改时间：
*****************************************************************************/
void Shutdown::slotShutdown()
{
    //TO DO
    int rtnVal = system("shutdown -hf now");
    if(!rtnVal)
    {
        int i = 0;
    }
}
