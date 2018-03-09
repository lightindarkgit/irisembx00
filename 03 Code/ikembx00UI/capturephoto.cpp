/*****************************************************************************
** 文 件 名：capturephoto.cpp
** 主 要 类：CapturePhoto
**
** Copyright (c) 中科虹霸有限公司
**
** 创 建 人：王磊
** 创建时间：20013-10-18
**
** 修 改 人：
** 修改时间：
** 描  述:   人脸图像采集界面
** 主要模块说明: 人脸图像采集过程中实时显示， 采集完成， 返回
**
** 版  本:   1.0.0
** 备  注:
**
*****************************************************************************/
#include <QImage>
#include <QPixmap>
#include <QMessageBox>
#include <unistd.h>

#include "capturephoto.h"
#include "ui_capturephoto.h"
//人脸图像API
#include "../AlgApi/SRC/AlgApi.h"

const int IMAGE_WIDTH = 480;
const int IMAGE_HEIGHT = 640;
const int IMAGE_SIZE = IMAGE_WIDTH * IMAGE_HEIGHT;

/*****************************************************************************
*                        人脸图像采集处理函数
*  函 数 名：GetFacePhoto
*  功    能：调用人脸采集图像接口，采集到图像后发射信号，供图像处理类使用
*  说    明：
*  参    数：
*  返 回 值：
*  创 建 人：王磊
*  创建时间：2014-01-20
*  修 改 人：
*  修改时间：
*****************************************************************************/
void *GetFacePhoto(void* arg)
{
    //TO DO
    if(NULL == arg)
    {
        return (void *)0;
    }

    CapturePhoto* cp = static_cast<CapturePhoto *>(arg);

    while(cp->enableCapture)
    {
        //TO DO
        if(E_ALGAPI_OK == cp->faceCamer->CapImg(cp->photoBuffer))
        {
            emit cp->sigUpdateImage();
        }
        //防止系统误误判死循环
        sleep(0);
    }

    return (void *)0;
}

CapturePhoto::CapturePhoto(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::CapturePhoto),
    faceCamer(NULL),
    photoBuffer(NULL),
    pImage(NULL),
    enableCapture(false)
{
    ui->setupUi(this);

    //图像缓暂存冲区
    photoBuffer = (unsigned char*)malloc(IMAGE_SIZE * 3 * sizeof(unsigned char));
    pImage = new QImage(photoBuffer, IMAGE_HEIGHT, IMAGE_WIDTH, QImage::Format_RGB888);

    //去除对话框标题栏，坐标重定位和填充背景
    setWindowFlags(Qt::FramelessWindowHint);
    setGeometry(QRect(0, 0, 800, 600));
    setBackgroundColor(QColor(242,241,240));


    //连接返回和保存信号和对应的槽
    connect(ui->btnBack, SIGNAL(clicked()), this, SLOT(slotBack()));
    connect(ui->btnSave, SIGNAL(clicked()), this, SLOT(slotSave()));
    connect(this, SIGNAL(sigUpdateImage()), this, SLOT(slotGetFaceImage()));
}

CapturePhoto::~CapturePhoto()
{
    delete ui;
}

bool CapturePhoto::setFileNameInfo(QString personInfo)
{
    persInfo = personInfo;

    return true;
}

bool CapturePhoto::initCamera()
{
    faceCamer = new CCameraApi();
    if(E_ALGAPI_OK !=  faceCamer->Init())
    {
        QMessageBox::critical(this, QString::fromUtf8("初始化人脸图像采集设备"), QString::fromUtf8("API初始化失败或未初始化"));
        return false;
    }


    enableCapture = true;
    pthread_create(&thrID, NULL, GetFacePhoto, this);

    return true;
}

void CapturePhoto::slotGetFaceImage()
{
    //TO DO
    pImage->loadFromData(photoBuffer, IMAGE_SIZE * 3 * sizeof(unsigned char));
    QImage img(IMAGE_WIDTH, IMAGE_HEIGHT, QImage::Format_RGB888);
    int height = ui->labFaceImageStream->height();
    int width = ui->labFaceImageStream->width();

    if(pImage->height() > pImage->width())
    {
        img = pImage->copy(0, pImage->height() / 4, pImage->width(), pImage->width() * height / width);
    }
    else
    {
        img = pImage->copy(pImage->width() / 4, 0, pImage->height() * width / height, pImage->height());
    }

    QMatrix matrix;
    matrix.rotate(180);
    img = img.transformed(QMatrix().rotate(180));
    img = img.scaled(width, height, Qt::KeepAspectRatio);


    pixMap = QPixmap::fromImage(img, Qt::AutoColor);

    ui->labFaceImageStream->setPixmap(pixMap);
}

/*****************************************************************************
*                        返回操作处理函数
*  函 数 名：slotBack
*  功    能：槽函数，处理返回操作
*  说    明：
*  参    数：
*  返 回 值：
*  创 建 人：王磊
*  创建时间：2013-10-18
*  修 改 人：
*  修改时间：
*****************************************************************************/
void CapturePhoto::slotBack()
{
    //TO DO
    stopCapturing();
    emit sigImgaeCaptured(QImage());
    close();
}

/*****************************************************************************
*                        保存采集到的当前人脸图像
*  函 数 名：slotSave
*  功    能：槽函数，存储当前采集的人脸图像
*  说    明：
*  参    数：
*  返 回 值：
*  创 建 人：王磊
*  创建时间：2013-10-18
*  修 改 人：
*  修改时间：
*****************************************************************************/
void CapturePhoto::slotSave()
{
    //TO DO
    //发射图像采集结束信号
    emit sigImgaeCaptured(pixMap.toImage());


    stopCapturing();
    //关闭当前窗口
    close();
}


void CapturePhoto::stopCapturing()
{
    enableCapture = false;

    void *rtn_status;
    pthread_join(thrID, &rtn_status);
    faceCamer->Release();
}
