/*****************************************************************************
** 文 件 名：capturephoto.h
** 主 要 类：CapturePhoto
**
** Copyright (c) 中科虹霸有限公司
**
** 创 建 人：L.Wang
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
#pragma once
#include <QDialog>
#include <QImage>
#include <pthread.h>
//人脸图像API
#include "CCameraApi.h"

extern const int IMAGE_WIDTH;
extern const int IMAGE_HEIGHT;
extern const int IMAGE_SIZE;

void *GetFacePhoto(void* arg);

//人脸图像采集API类声明
class CCameraApi;


namespace Ui {
class CapturePhoto;
}

class CapturePhoto : public QDialog
{
    friend void *GetFacePhoto(void* arg);

    Q_OBJECT    
public:
    explicit CapturePhoto(QWidget *parent = 0);
    ~CapturePhoto();
    //设置人脸图像文件名信息
    bool setFileNameInfo(QString personInfo);

    bool initCamera();
signals:
    void sigImgaeCaptured(QImage);
    void sigUpdateImage();

protected slots:
    void slotGetFaceImage();
    void slotBack(bool isAutoBack=false);
    void slotSave();

protected:
    void stopCapturing(bool isAutoBack = false);

private:
    Ui::CapturePhoto *ui;
    CCameraApi* faceCamer;
    unsigned char* photoBuffer;
    QImage* pImage;
    QPixmap pixMap;

    pthread_t thrID;
    volatile bool enableCapture;
    QString persInfo;
    bool _isDataEdited;
public:
    bool IsPhotoSaved();
};
