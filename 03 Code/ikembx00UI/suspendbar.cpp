/*****************************************************************************
** 文 件 名： SuspendBar.cpp
** 主 要 类： SuspendBar
**
** Copyright (c) 中科虹霸有限公司
**
** 创 建 人：刘中昌
** 创建时间：20013-10-24
**
** 修 改 人：
** 修改时间：
** 描  述:   弹出管理员登陆按钮
** 主要模块说明: 弹出管理员登陆按钮
**
** 版  本:   1.0.0
** 备  注:
**
*****************************************************************************/
#include "suspendbar.h"
#include "ui_suspendbar.h"

SuspendBar::SuspendBar(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::SuspendBar)
{
    ui->setupUi(this);
    ui->widget->setBackgroundColor(QColor(70,70,35));
    ui->btnIdent->hide();

    connect(ui->btnAdmin,SIGNAL(clicked()),this,SIGNAL(sigAdminLogin()));
    connect(ui->btnIrisLogin,SIGNAL(clicked()),this,SIGNAL(sigIrisLogin()));
    connect(ui->btnIdent,SIGNAL(clicked()),this,SIGNAL(sigIdent()));
    connect(ui->btnIrisLogin,SIGNAL(clicked()),this,SLOT(showBtnIdent()));
    connect(ui->btnIdent,SIGNAL(clicked()),this,SLOT(hideBtnIdent()));
}

SuspendBar::~SuspendBar()
{
    delete ui;
}

/*****************************************************************************
*                        显示回退button
*  函 数 名：showBtnIdent
*  功    能：显示回退button
*  说    明：
*  参    数：
*  返 回 值：
*  创 建 人：刘中昌
*  创建时间：2013-10-26
*  修 改 人：
*  修改时间：
*****************************************************************************/
void SuspendBar::showBtnIdent()
{
    ui->btnAdmin->hide();
    ui->btnIrisLogin->hide();
    ui->btnIdent->show();
}


/*****************************************************************************
*                        隐藏回退button
*  函 数 名：hidBtnIdent
*  功    能：隐藏回退button
*  说    明：
*  参    数：
*  返 回 值：
*  创 建 人：刘中昌
*  创建时间：2013-10-26
*  修 改 人：
*  修改时间：
*****************************************************************************/
void SuspendBar::hideBtnIdent()
{
    ui->btnAdmin->show();
    ui->btnIdent->hide();
    ui->btnIrisLogin->show();
}
