/******************************************************************************************
 ** 文件名:   buffering.cpp
 ×× 主要类:   (无)
 **
 ** Copyright (c) 中科虹霸有限公司
 ** 创建人:   Wang.L
 ** 日  期:   2015-01-28
 ** 修改人:
 ** 日  期:
 ** 描  述:   等待动画实现
 ** ×× 主要模块说明：
 **
 ** 版  本:   1.0.0
 ** 备  注:
 **
 ******************************************************************************************/
#include "buffering.h"
#include "ui_buffering.h"
#include "../Common/Exectime.h"

Buffering::Buffering(QWidget *parent,QString showText) :
	QDialog(parent),
	ui(new Ui::Buffering),
	gifPlayer(":/image/loading.gif")
{
	
	ui->setupUi(this);
	InitPlayer();

	//liuzhch
	ui->labShowText->setText(showText);
	this->setWindowFlags(Qt::FramelessWindowHint| Qt::WindowMinimizeButtonHint| Qt::WindowSystemMenuHint);
	setGeometry(QRect(0, 0, 1024, 600));
	setAttribute(Qt::WA_TranslucentBackground, true);
}

Buffering::~Buffering()
{
	
	delete ui;
}


void Buffering::InitPlayer()
{
	
	//TO DO
	gifPlayer.setScaledSize(QSize(ui->labGif->size().width(), ui->labGif->size().width()));
	gifPlayer.setSpeed(200);

	ui->labGif->setMovie(&gifPlayer);
	gifPlayer.start();
}


void Buffering::SetNotionText(const QString &info)
{
	
	ui->labShowText->setText(info);
}

