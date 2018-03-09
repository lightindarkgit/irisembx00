/******************************************************************************************
** 文件名:   CameraDemo.h
×× 主要类:   CameraDemo
**
** Copyright (c) 中科虹霸有限公司
** 创建人:   lizhl
** 日  期:   2014-01-16
** 修改人:
** 日  期:
** 描  述:   IKEMBX00项目人脸摄像头类调用示例
 *×× 主要模块说明: CameraDemo.h
**
** 版  本:   1.0.0
** 备  注:
**
*****************************************************************************************/
#ifndef CAMERADEMO_H
#define CAMERADEMO_H

#include <QtGui>
#include "SRC/AlgApi.h"

class CameraDemo : public QWidget
{
    Q_OBJECT
public:
    CameraDemo(QWidget *parent=0);
    ~CameraDemo();

public:
    CCameraApi *pCCameraApi;//人脸摄像头类
    //以下为界面等相关资源
    QPainter *painter;
    QLabel *label;
    QImage *frame;
    QTimer *timer;    
    int rs;//返回值

    unsigned char *pp;//获取图像指针，需要在调用采图前分配内存

private slots:
    void paintEvent(QPaintEvent *);
};

#endif
