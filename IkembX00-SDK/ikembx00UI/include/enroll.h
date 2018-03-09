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
#include <QLabel>
#include <QMovie>
#include "dialogenroll.h"
#include "dialogfeatureexist.h"

#include <mutex>
#include <thread>
#include <condition_variable>

#include "interaction.h"

//^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
//增加数据库操作相关头文件
#include <ctime>
#include <soci.h>
#include "../IrisAppLib/IrisManager/uuidControl.h"
#include "../ikIOStream/interface.h"
#include "../IrisAppLib/IrisManager/irisManager.h"
#include "../IrisAppLib/IrisManager/singleControl.h"
//vvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvv

#include "IKUSBSDK.h"


namespace Ui {
class Enroll;
}

//绘图事件类
class ImagePaintEvent : public QObject
{
    Q_OBJECT
public:
    //绘制实时采集图像事件
    void RaisePaintCapImgEvent(int result);
    //绘制注册结果事件
    void RaisePaintEnrollResultEvent(int result, IKSDKIrisInfo* enrIrisL, int numEnrL, IKSDKIrisInfo* enrIrisR, int numEnrR, IKResultFlag IKflag);

signals:
    //发送绘制实时采集图像信号
    void SigPaintCapImg(int result);
    //发送绘制注册结果信号
    void SigPaintEnrollResult(int result, IKSDKIrisInfo* enrIrisL, int numEnrL, IKSDKIrisInfo* enrIrisR, int numEnrR, IKResultFlag IKflag);
};

//注册类
class Enroll : public QDialog
{
    Q_OBJECT
    
public:
    Enroll(QWidget *parent = 0);
    ~Enroll();

private:
    //    int _captureImageTimerId;
    int _frameNum;

public:
    //设置人员信息
    void SetPersonInfo(PersonInfo *personInfomation);
    //退出注册界面，由其上一层调用，用于系统强行退出注册界面，而不是通过界面操作退出
    void ExitEnrollUI(bool isAutoExit = false);


    bool GetEnrollStatus();

    void SetEnrollStatus(bool isStoped);

    int IsPthreadAlive(pthread_t tid);

private:

    //从数据库中导入已注册虹膜特征
    void LoadExistedFeature();

    //检查用户选择的眼睛模式并进行设置
    bool CheckAndSetEyeMode();

    //去使能眼睛模式选择按钮
    void DisableEyeModeRbtn();

    //使能眼睛模式选择按钮
    void EnableEyeModeRbtn();

    //20141208
    void InitLabelMask();//初始化_labMask
    void InitLabelStatus();//初始化_labStatus
    void InitTraceGif();//初始化_labTraceL, _labTraceR
    void SetTraceVisible(bool isvisible);
    void DrawStatusLabel(StatusLabelType statusType);

    //@ 重载for sdk
    //人眼跟踪
    void DrawTraceGif(IKEnrIdenStatus enrIdenStatus);

    //停止注册
    bool StopEnroll();

    //将摄像头采集到的数据赋给成员变量 m_enrIdenStatus
    void SetCameraData(const IKEnrIdenStatus enrIdenStatus);

    //实时获取摄像头数据
    IKEnrIdenStatus GetCameraData();
    void InitEnrollResultPic();

    void InitEnrollWindow();

    bool IsDataSaved;

    
signals:
    void sigEnrollResult(bool isSucceed);
    void sigCloseDialogFeatureExist();

private slots:
    void on_btnEnroll_clicked();
    void on_btnEnrollReturn_clicked();
    void on_btnSaveEnrollResult_clicked();
    void on_btnIrisImgLeft1_clicked();
    void on_btnIrisImgLeft2_clicked();
    void on_btnIrisImgLeft3_clicked();
    void on_btnIrisImgRight1_clicked();
    void on_btnIrisImgRight2_clicked();
    void on_btnIrisImgRight3_clicked();
    void on_rbtn_clicked();

    //绘制实时采集图像
    void CapImageUpdate(IKEnrIdenStatus enrIdenStatus);

    //绘制注册结果图像
    void EnrollResultImageUpdate(int result, IKSDKIrisInfo* enrIrisL, int numEnrL,
                                             IKSDKIrisInfo* enrIrisR, int numEnrR,
                                             IKResultFlag lrEnrSucFailFlag         );

private:
    Ui::Enroll *ui;
    DialogEnroll *_dialogEnroll;              //注册对话框

    static const  int PPMHeaderLength = 18;   //成图的数据包头
    static unsigned char _grayCapImgBuffer[IK_DISPLAY_IMG_SIZE + PPMHeaderLength];//采集图像buffer，ppm格式
    int _distanceValue;                       //距离值
    bool _btnEnrollClicked;                   //注册按钮是否被点击标志
    bool _isStartEnroll;

    //虹膜管理
    IrisManager     & _enrIrisManager;        //虹膜管理，用于从数据库中导出虹膜注册特征，用于验证虹膜是否重复注册
    unsigned char   *_enrCodeListL;           //已注册左眼普通人员虹膜特征
    int             _enrCodeNumL;             //已注册左眼普通人员虹膜特征数量
    unsigned char   *_enrCodeListR;           //已注册右眼普通人员虹膜特征
    int             _enrCodeNumR;             //已注册右眼普通人员虹膜特征数量
    unsigned char   *_enrAdminCodeListL;      //已注册左眼管理员虹膜特征
    int             _enrAdminCodeNumL;        //已注册左眼管理员虹膜特征数量
    unsigned char   *_enrAdminCodeListR;      //已注册右眼管理员虹膜特征
    int             _enrAdminCodeNumR;        //已注册右眼管理员虹膜特征数量

    bool    _isLeftFeatureExisted;            //用于标记左眼虹膜是否重复注册
    bool    _isRightFeatureExisted;           //用于标记右眼虹膜是否重复注册
    int     _indexOfLeftExistedFeature;       //已存在特征的序号，左眼
    int     _indexOfRightExistedFeature;      //已存在特征的序号，右眼

    //20140930
    static int  s_leftEnrNum;
    static int  s_rightEnrNum;

    int _noticeCount;


    //注册相关参数
    static  IKSDKIrisInfo   *s_enrIrisL;
    static  IKSDKIrisInfo   *s_enrIrisR;
    int     _numEnrL;                       //左眼注册图像个数
    int     _numEnrR;                       //右眼注册图像个数
    static  IKResultFlag     s_enrLrFlag;   //注册结果标志
    IKEnrIdenStatus         m_enrIdenStatus;//传入摄像头采集信息
    static  IKIrisMode     _irisMode       ;//识别或注册时眼睛模式

    //注册线程
    pthread_t enrollThread;
    static void *ThreadEnroll(void* arg);
    bool _isEnrollThreadCreated;


    //传进来的人员信息
    PersonInfo _personInfo;

    //人机交互类
    static Interaction *s_enrollInteraction;


    //20140929
    QPoint _imGlobalPoint;
    QLabel *_labStatus;
    QLabel *_labMask;
    QMovie *_movTraceL;
    QLabel *_labTraceL;
    QMovie *_movTraceR;
    QLabel *_labTraceR;
    QLabel *_enrollWindowLabel;
    QPixmap* IdentWindow;//dj for initEnrollWindow

    bool _isClearL, _isClearR, _isLastClearL, _isLastClearR;
    bool ifUpdatePosition;          //是否更新位置信息的标志
    static bool s_isEnrollStoped;           //注册是否停止



public:
    static bool _signaledFlag;
    int Lx,Ly,Rx,Ry,Lr,Rr;
    bool isFeatureCheckUIOpen;

};

#endif // ENROLL_H
