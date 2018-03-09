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
#include "manager.h"
#include "ui_manager.h"

Manager::Manager(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Manager)
{
    ui->setupUi(this);
    connect(ui->toolBtnDataManager,SIGNAL(clicked()),this,SIGNAL(sigDataManger()));
    connect(ui->toolBtnHelp,SIGNAL(clicked()),this,SIGNAL(sigHelp()));
    connect(ui->toolBtnIdentify,SIGNAL(clicked()),this,SIGNAL(sigIdentify()));
    connect(ui->toolBtnPerson,SIGNAL(clicked()),this,SIGNAL(sigPerson()));
    connect(ui->toolBtnShutdown,SIGNAL(clicked()),this,SIGNAL(sigShutdown()));
    connect(ui->toolBtnSysterm,SIGNAL(clicked()),this,SIGNAL(sigSysterm()));

}

Manager::~Manager()
{
    delete ui;
}
