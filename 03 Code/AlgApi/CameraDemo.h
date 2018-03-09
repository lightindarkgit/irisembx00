/******************************************************************************************
** �ļ���:   CameraDemo.h
���� ��Ҫ��:   CameraDemo
**
** Copyright (c) �пƺ�����޹�˾
** ������:   lizhl
** ��  ��:   2014-01-16
** �޸���:
** ��  ��:
** ��  ��:   IKEMBX00��Ŀ��������ͷ�����ʾ��
 *���� ��Ҫģ��˵��: CameraDemo.h
**
** ��  ��:   1.0.0
** ��  ע:
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
    CCameraApi *pCCameraApi;//��������ͷ��
    //����Ϊ����������Դ
    QPainter *painter;
    QLabel *label;
    QImage *frame;
    QTimer *timer;    
    int rs;//����ֵ

    unsigned char *pp;//��ȡͼ��ָ�룬��Ҫ�ڵ��ò�ͼǰ�����ڴ�

private slots:
    void paintEvent(QPaintEvent *);
};

#endif
