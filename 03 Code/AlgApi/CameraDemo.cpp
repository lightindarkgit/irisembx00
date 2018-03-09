/******************************************************************************************
** �ļ���:   CameraDemo.cpp
���� ��Ҫ��:   CCameraDemo
**
** Copyright (c) �пƺ�����޹�˾
** ������:   lizhl
** ��  ��:   2014-01-15
** �޸���:
** ��  ��:
** ��  ��:   IKEMBX00��Ŀ��������ͷ����Demo
 *���� ��Ҫģ��˵��: ��������ͷ����Demo
**
** ��  ��:   1.0.0
** ��  ע:
**
*****************************************************************************************/
#include <QtGui>
#include "CameraDemo.h"

/*****************************************************************************
*                         ��������ͷDemo���캯��
*  �� �� ����CameraDemo
*  ��    �ܣ����캯��
*  ˵    ����
*  ��    ����
*
*  �� �� ֵ��
*
*  �� �� �ˣ�lizhl
*  ����ʱ�䣺2014-01-16
*  �� �� �ˣ�
*  �޸�ʱ�䣺
*****************************************************************************/
CameraDemo::CameraDemo(QWidget *parent):QWidget(parent)
{
    pCCameraApi = new CCameraApi();
    pCCameraApi->Init();//��ʼ��

    pp = (unsigned char *)malloc(640 * 480* 3 * sizeof(char));//��ͼǰ����ռ�

    //����Ϊ������ʾ���
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
*                         ��������ͷDemo��������
*  �� �� ����CameraDemo
*  ��    �ܣ���������
*  ˵    ����
*  ��    ����
*
*  �� �� ֵ��
*
*  �� �� �ˣ�lizhl
*  ����ʱ�䣺2014-01-16
*  �� �� �ˣ�
*  �޸�ʱ�䣺
*****************************************************************************/
CameraDemo::~CameraDemo()
{
    pCCameraApi->Release();//��������ͷ�����Դ�ͷ�
    pCCameraApi = NULL;
    delete pCCameraApi;
}

/*****************************************************************************
*                         ��������ͷDemo��ͼ�¼�
*  �� �� ����paintEvent
*  ��    �ܣ���ͼ������
*  ˵    ����
*  ��    ����
*
*  �� �� ֵ��
*
*  �� �� �ˣ�lizhl
*  ����ʱ�䣺2014-01-16
*  �� �� �ˣ�
*  �޸�ʱ�䣺
*****************************************************************************/
void CameraDemo::paintEvent(QPaintEvent *)
{
    pCCameraApi->CapImg(pp);//�ɼ�ͼ��
    frame->loadFromData(pp, 640 * 480 * 3 * sizeof(char));
    label->setPixmap(QPixmap::fromImage(*frame,Qt::AutoColor));
}



