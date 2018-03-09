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

#include "AlgApi.h"
#include "BRCCommon.h"
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
    void RaisePaintCapImgEvent(int result);
    //绘制注册结果事件
    void RaisePaintEnrollResultEvent(int result, APIIrisInfo* enrIrisL, int numEnrL, APIIrisInfo* enrIrisR, int numEnrR, LRSucFailFlag lrSucFailFlag);

signals:
    //发送绘制实时采集图像信号
    void SigPaintCapImg(int result);
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
private slots:
    void ShowTime();
private:
//    int _captureImageTimerId;
    int _frameNum;
    QTimer *_showTime;
    //vvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvv

public:
    //设置人员信息
    void SetPersonInfo(PersonInfo *personInfomation);
    //退出注册界面，由其上一层调用，用于系统强行退出注册界面，而不是通过界面操作退出
    void ExitEnrollUI();


private:
    //异步注册回调函数
    static int CBAsyncEnroll(int funResult, APIIrisInfo* enrIrisL, int numEnrL, APIIrisInfo* enrIrisR, int numEnrR, LRSucFailFlag lrFlag);
    static int CBAlgApiCapIrisImg(int funResult, unsigned char* pIrisImg, LRIrisClearFlag lrIrisClearFlag, IrisPositionFlag irisPositionFlag, IrisTraceInfo leftTraceInfo, IrisTraceInfo rightTraceInfo,
                                  int leftEnrNum, int rightEnrNum);
    //显示注册图像对应的原始图像
    void ShowOriginImg();

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

    //20141212
    void DrawStatusLabel(StatusLabelType statusType);
    void DrawTraceGif(IrisPositionFlag irisPositionFlag, LRIrisClearFlag lrIrisClearFlag, IrisTraceInfo leftTraceInfo, IrisTraceInfo rightTraceInfo);
    
signals:
    void sigEnrollResult(bool isSucceed);

private slots:
    void on_btnEnroll_clicked();

    void on_btnEnrollReturn_clicked();

    void on_btnSaveEnrollResult_clicked();

    //绘制实时采集图像
    void CapImageUpdate(int result);

    //绘制注册结果图像
    void EnrollResultImageUpdate(int result, APIIrisInfo* enrIrisL, int numEnrL, APIIrisInfo* enrIrisR, int numEnrR, LRSucFailFlag lrEnrSucFailFlag);

    void on_btnIrisImgLeft1_clicked();

    void on_btnIrisImgLeft2_clicked();

    void on_btnIrisImgLeft3_clicked();

    void on_btnIrisImgRight1_clicked();

    void on_btnIrisImgRight2_clicked();

    void on_btnIrisImgRight3_clicked();

    void on_rbtn_clicked();

private:
    static void* CapImgThread(void* pParam);    //采图线程

private:
    Ui::Enroll *ui;
    DialogEnroll *_dialogEnroll;                //注册对话框
    static CAlgApi *_enrollAlgApi;                     //注册模块算法API实例
//    unsigned char _grayCapImgBuffer[g_constIrisImgSize + g_constIrisPPMHeaderLength];   //采集图像buffer
    static unsigned char _grayCapImgBuffer[g_constIrisImgSize + g_constBMPHeaderLength];//采集图像buffer，bmp格式
    int _distanceValue;//距离值
    char _grayCapPpmHeader[g_constIrisPPMHeaderLength];                                 //存储采集图像ppm头的buffer

//    unsigned char _grayResultImgBuffer[g_constIKImgSize + g_constIKPPMHeaderLength];    //注册图像buffer
//    char _grayResultPpmHeader[g_constIKPPMHeaderLength];                                //存储注册图像ppm头的buffer

    unsigned char _grayResultImgBuffer[g_constIKImgSize + g_constBMPHeaderLength];      //注册图像buffer，bmp格式

    bool _btnEnrollClicked;                                             //注册按钮是否被点击标志

    //虹膜管理
    IrisManager& _enrIrisManager;                                       //虹膜管理，用于从数据库中导出虹膜注册特征，用于验证虹膜是否重复注册
    unsigned char *_enrCodeListL;                                       //已注册左眼普通人员虹膜特征
    int _enrCodeNumL;                                                   //已注册左眼普通人员虹膜特征数量
    unsigned char *_enrCodeListR;                                       //已注册右眼普通人员虹膜特征
    int _enrCodeNumR;                                                   //已注册右眼普通人员虹膜特征数量

    unsigned char *_enrAdminCodeListL;                                  //已注册左眼管理员虹膜特征
    int _enrAdminCodeNumL;                                              //已注册左眼管理员虹膜特征数量
    unsigned char *_enrAdminCodeListR;                                  //已注册右眼管理员虹膜特征
    int _enrAdminCodeNumR;                                              //已注册右眼管理员虹膜特征数量

    bool _isLeftFeatureExisted;                                         //用于标记左眼虹膜是否重复注册
    bool _isRightFeatureExisted;                                        //用于标记右眼虹膜是否重复注册

    int _indexOfLeftExistedFeature;                                     //已存在特征的序号，左眼
    int _indexOfRightExistedFeature;                                    //已存在特征的序号，右眼

    static LRIrisClearFlag s_lrIrisClearFlag;                           //图像是否清晰标志
    static IrisPositionFlag s_irisPositionFlag;                         //图像位置信息标志
    static IrisTraceInfo s_leftTraceInfo;
    static IrisTraceInfo s_rightTraceInfo;

    //20140930
    static int s_leftEnrNum;
    static int s_rightEnrNum;

    std::thread _capImgThread;                                  //注册模块图像显示线程
    std::mutex _capImgMutex;                                    //注册模块图像采集mutex
//    std::condition_variable _capImgCV;                          //注册模块图像采集condition_variable
    bool _capImgFlag;                                           //注册模块图像采集flag
//    static bool _signaledFlag;

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

    //20140929
    QPoint _imGlobalPoint;
    QLabel *_labStatus;

    //20141202
    QLabel *_labMask;

    //20141208
    QMovie *_movTraceL;
    QLabel *_labTraceL;

    QMovie *_movTraceR;
    QLabel *_labTraceR;

    bool _isClearL, _isClearR, _isLastClearL, _isLastClearR;

    bool ifUpdatePosition;//是否更新位置信息的标志

public:
    static bool _signaledFlag;
    int Lx,Ly,Rx,Ry,Lr,Rr;

};

#endif // ENROLL_H
