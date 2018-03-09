/*****************************************************************************
** 文 件 名：enroll.h
** 主 要 类：Enroll
**
** Copyright (c) 中科虹霸有限公司
**
** 创 建 人：lizhl
** 创建时间：2013-10-18
**
** 修 改 人：
** 修改时间：
** 描  述:   注册模块
** 主要模块说明: 注册类
**
** 版  本:   1.0.0
** 备  注:
**
*****************************************************************************/
#ifndef ENROLL_H
#define ENROLL_H

#include <QDialog>
#include <QtCore/QTextCodec>
#include <QElapsedTimer>
#include <QPainter>
#include "dialogenroll.h"

#include <mutex>
#include <thread>
#include <condition_variable>

#include "../../03 Code/AlgApi/SRC/AlgApi.h"
#include "../AlgApi/SRC/BRCCommon.h"
#include "interaction.h"


//extern unsigned char _grayCapImgBuffer[g_constIrisImgSize + g_constIrisPPMHeaderLength];   //采集图像buffer
//extern unsigned char _grayCapImgBuffer[g_constIrisImgSize + g_constBMPHeaderLength];//采集图像buffer，bmp格式


namespace Ui {
class Enroll;
}

//绘图事件类
class ImagePaintEvent : public QObject
{
    Q_OBJECT
public:
    //绘制实时采集图像事件
    void RaisePaintCapImgEvent();
    //绘制注册结果事件
    void RaisePaintEnrollResultEvent(int result, APIIrisInfo* enrIrisL, int numEnrL, APIIrisInfo* enrIrisR, int numEnrR, LRSucFailFlag lrSucFailFlag);

signals:
    //发送绘制实时采集图像信号
    void SigPaintCapImg();
    //发送绘制注册结果信号
    void SigPaintEnrollResult(int result, APIIrisInfo* enrIrisL, int numEnrL, APIIrisInfo* enrIrisR, int numEnrR, LRSucFailFlag lrSucFailFlag);
};

//注册类
class Enroll : public QDialog
{
    Q_OBJECT
    
public:
    Enroll(QWidget *parent = 0);
    ~Enroll();

    //^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
    //待删除
public:
    virtual void timerEvent(QTimerEvent *event);
private:
    int _captureImageTimerId;
    int _frameNum;
    //vvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvv

public:
    //设置人员信息
    void SetPersonInfo(PersonInfo *personInfomation);

private:
    //异步注册回调函数
    static int CBAsyncEnroll(int funResult, APIIrisInfo* enrIrisL, int numEnrL, APIIrisInfo* enrIrisR, int numEnrR, LRSucFailFlag lrFlag);

    //显示注册图像对应的原始图像
    void ShowOriginImg();

    
signals:
    void SigEnrollResult(bool isSucceed);

private slots:
    void on_btnEnroll_clicked();

    void on_btnEnrollReturn_clicked();

    void on_btnSaveEnrollResult_clicked();

    //绘制实时采集图像
    void CapImageUpdate();

    //绘制注册结果图像
    void EnrollResultImageUpdate(int result, APIIrisInfo* enrIrisL, int numEnrL, APIIrisInfo* enrIrisR, int numEnrR, LRSucFailFlag lrEnrSucFailFlag);

    void on_rbtnBothEye_clicked();

    void on_rbtnLeftEye_clicked();

    void on_rbtnRightEye_clicked();

    void on_rbtnAnyEye_clicked();

    void on_btnIrisImgLeft1_clicked();

    void on_btnIrisImgLeft2_clicked();

    void on_btnIrisImgLeft3_clicked();

    void on_btnIrisImgRight1_clicked();

    void on_btnIrisImgRight2_clicked();

    void on_btnIrisImgRight3_clicked();

private:
    static void* CapImgThread(void* pParam);    //采图线程


private:
    Ui::Enroll *ui;
    DialogEnroll *_dialogEnroll;                //注册对话框
    CAlgApi *_enrollAlgApi;                     //注册模块算法API实例
//    unsigned char _grayCapImgBuffer[g_constIrisImgSize + g_constIrisPPMHeaderLength];   //采集图像buffer
    unsigned char _grayCapImgBuffer[g_constIrisImgSize + g_constBMPHeaderLength];//采集图像buffer，bmp格式
    int _distanceValue;//距离值
    char _grayCapPpmHeader[g_constIrisPPMHeaderLength];                                 //存储采集图像ppm头的buffer

//    unsigned char _grayResultImgBuffer[g_constIKImgSize + g_constIKPPMHeaderLength];    //注册图像buffer
//    char _grayResultPpmHeader[g_constIKPPMHeaderLength];                                //存储注册图像ppm头的buffer

    unsigned char _grayResultImgBuffer[g_constIKImgSize + g_constBMPHeaderLength];      //注册图像buffer，bmp格式

    bool _btnEnrollClicked;                                     //注册按钮是否被点击标志

    static LRIrisClearFlag s_lrIrisClearFlag;                           //图像是否清晰标志
    static IrisPositionFlag s_irisPositionFlag;                         //图像位置信息标志

    std::thread _capImgThread;                                  //注册模块图像显示线程
    std::mutex _capImgMutex;                                    //注册模块图像采集mutex
//    std::condition_variable _capImgCV;                          //注册模块图像采集condition_variable
    bool _capImgFlag;                                           //注册模块图像采集flag
    bool _signaledFlag;

//    std::thread _enrollThread;                                  //注册模块注册线程
//    std::mutex  _enrollMutex;                                   //注册模块注册mutex
//    std::condition_variable _enrollCV;                          //注册模块注册condition_variable
//    bool _enrollFlag;                                           //注册模块注册flag

    //注册相关参数
    static APIIrisInfo *s_enrIrisL;  //左眼注册图像
    static APIIrisInfo *s_enrIrisR;  //右眼注册图像
    int _numEnrL;                   //左眼注册图像个数
    int _numEnrR;                   //右眼注册图像个数
    static LRSucFailFlag s_enrLrFlag;//注册结果标志

    //传进来的人员信息
    PersonInfo _personInfo;

    //人机交互类
    static Interaction *s_enrollInteraction;

    AlgApiCtrlParam _enrollParamStruct;//算法逻辑控制参数

};

#endif // ENROLL_H
