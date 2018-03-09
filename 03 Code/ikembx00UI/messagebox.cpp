/*****************************************************************************
** 文 件 名：QMessageBoxDefine.h
** 主 要 类：QMessageBoxDefine
**
** Copyright (c) 中科虹霸有限公司
**
** 创 建 人：刘中昌
** 创建时间：20013-10-26
**
** 修 改 人：
** 修改时间：
** 描  述:   串口通信数据处理类
** 主要模块说明: 自定义提示对话框--解决button无法显示中文问题
**
** 版  本:   1.0.0
** 备  注:
**
*****************************************************************************/
#include "messagebox.h"
#include "ui_messagebox.h"

MessageBox::MessageBox(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::MessageBox)
{
    ui->setupUi(this);

    setWindowFlags(Qt::CustomizeWindowHint);
    ui->widgetBack->setBackgroundColor(QColor(220,220,220));
    ui->widgetTitle->setBackgroundColor(QColor(71,71,71));
    ui->btnCancel->hide();
    ui->btnOK->hide();
    ui->btnSave->hide();
}

MessageBox::~MessageBox()
{
    delete ui;
}

QMessageBox::StandardButton MessageBox::information ( const QString & title,
                                           const QString & text, QMessageBox::StandardButtons buttons,
                                                             QMessageBox::StandardButton defaultButton )
{
    ui->labTitle->text() = title;
    ui->labContext->text() = text;


    if(buttons & QMessageBox::Ok)
    {
        ui->btnOK->show();
    }

    if(buttons & QMessageBox::Cancel)
    {
        ui->btnCancel->show();
    }

    if(buttons & QMessageBox::Close)
    {
//        QPushButton *btnCancel=new QPushButton(QObject::tr("关闭"));
        ui->btnCancel->text() = tr("关闭");
        ui->btnCancel->show();
    }

    if(buttons & QMessageBox::Yes)
    {
        //QPushButton *btnYes=new QPushButton(QObject::tr("是"));
        ui->btnOK->text() = tr("是");
        ui->btnOK->show();
    }

    if(buttons & QMessageBox::No)
    {
       // QPushButton *btnNo=new QPushButton(QObject::tr("否"));
        ui->btnCancel->text() = tr("否");
        ui->btnCancel->show();
    }
    return QMessageBox::Ok;

}
