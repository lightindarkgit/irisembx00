/******************************************************************************************
** 文件名:   CameraDemo.cpp
×× 主要类:   CCameraDemo
**
** Copyright (c) 中科虹霸有限公司
** 创建人:   lizhl
** 日  期:   2014-01-15
** 修改人:
** 日  期:
** 描  述:   IKEMBX00项目人脸摄像头调用Demo
 *×× 主要模块说明: 人脸摄像头调用Demo
**
** 版  本:   1.0.0
** 备  注:
**
*****************************************************************************************/
#include <QtGui>
#include "CameraDemo.h"

/*****************************************************************************
*                         人脸摄像头Demo构造函数
*  函 数 名：CameraDemo
*  功    能：构造函数
*  说    明：
*  参    数：
*
*  返 回 值：
*
*  创 建 人：lizhl
*  创建时间：2014-01-16
*  修 改 人：
*  修改时间：
*****************************************************************************/
CameraDemo::CameraDemo(QWidget *parent):QWidget(parent)
{
    pCCameraApi = new CCameraApi();
    pCCameraApi->Init();//初始化

    pp = (unsigned char *)malloc(640 * 480* 3 * sizeof(char));//采图前分配空间

    //以下为界面显示相关
    painter = new QPainter(this);
    frame = new QImage(pp,640,480,QImage::Format_RGB888);
    label = new QLabel();

    timer = new QTimer(this);
    connect(timer,SIGNAL(timeout()),this,SLOT(update()));
    timer->start(30);

    QHBoxLayout *hLayout = new QHBoxLayout();
    hLayout->addWidget(label);
    setLayout(hLayout);
    setWindowTitle(tr("Capture"));
}

/*****************************************************************************
*                         人脸摄像头Demo析构函数
*  函 数 名：CameraDemo
*  功    能：析构函数
*  说    明：
*  参    数：
*
*  返 回 值：
*
*  创 建 人：lizhl
*  创建时间：2014-01-16
*  修 改 人：
*  修改时间：
*****************************************************************************/
CameraDemo::~CameraDemo()
{
    pCCameraApi->Release();//人脸摄像头相关资源释放
    pCCameraApi = NULL;
    delete pCCameraApi;
}

/*****************************************************************************
*                         人脸摄像头Demo绘图事件
*  函 数 名：paintEvent
*  功    能：采图并绘制
*  说    明：
*  参    数：
*
*  返 回 值：
*
*  创 建 人：lizhl
*  创建时间：2014-01-16
*  修 改 人：
*  修改时间：
*****************************************************************************/
void CameraDemo::paintEvent(QPaintEvent *)
{
    pCCameraApi->CapImg(pp);//采集图像
    frame->loadFromData(pp, 640 * 480 * 3 * sizeof(char));
    label->setPixmap(QPixmap::fromImage(*frame,Qt::AutoColor));
}



