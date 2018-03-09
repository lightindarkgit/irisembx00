///*****************************************************************************
//** 文 件 名： IrisLogin.h
//** 主 要 类： IrisLogin
//**
//** Copyright (c) 中科虹霸有限公司
//**
//** 创 建 人：刘中昌
//** 创建时间：20013-10-24
//**
//** 修 改 人：
//** 修改时间：
//** 描  述:   虹膜登陆
//** 主要模块说明: 应用虹膜识别登陆
//**
//** 版  本:   1.0.0
//** 备  注:  本类目前已经不再使用 2014-12-25 fjf
//**
//*****************************************************************************/
#ifndef IRISLOGIN_H
#define IRISLOGIN_H

//#include <QWidget>
//#include "qidentify.h"
//#include "../../Code/USBAPI/simplelock.h"
//#include "AlgApi.h"
//#include "Common/interface.h"
//#include <QVector>

//#include <mutex>
//#include <thread>
//#include <condition_variable>
#include "../../Code/USBAPI/simplelock.h"
#include "AlgApi.h"
#include "Common/interface.h"
#include <QVector>
#include <QString>
#include <mutex>
#include <thread>
#include <condition_variable>
#include "FeatureManageForDemo.h"
#include "interaction.h"
#include "../IrisAppLib/IrisManager/irisManager.h"
#include "../IrisAppLib/IrisManager/singleControl.h"
#include "dialogidentify.h"

namespace Ui {
class IrisLogin;
}

//enum IdentResultState
//{
//    SuccessState=0,
//    FailedState,
//    ContinueState ,
//    KeepState,
//    UnKnownState
//};

//enum class WiganOutMode : unsigned int
//{
//    OutWorkSN =0,
//    OutCard,
//    NoOut,
//};


class IrisLogin : public QWidget
{
    Q_OBJECT
    public:
        explicit IrisLogin(QWidget *parent = 0);
        ~IrisLogin();

    signals:

        void sigGoBack();              //返回考勤识别
        void sigLoginSucceed(bool isSucceed); //信号，登陆是否成功
        void sigUpdateImage();

    public:
        void StartAttendIdent();       //开始考勤识别
        void StartIdent();             //开始识别
        int  SetIdentParam();           //设置识别模式
        void StopIdent();
        bool InitCamera();
        void StopCapturing();
    private:
        void StartLoginIdent();        //开始管理员登陆识别
        void SetStatus(bool isStop);

        void InitSerialMuxDemux();
        void SetSerialCallBack();
        void ReleaseSerialMuxDemux();
        void GetConfig();

        QString CalcValue(int nValue,int nTotal);
        void SetProgressBarValue(IrisPositionFlag irisPositionFlag);
    public slots:

        void sendBell();   //门铃
        void closeBell();  //关闭门铃

        void timerEvent(QTimerEvent *event);//界面时间更新
        void imageUpdate();                 //界面图像更新
        void imageUpdateForFace();          //界面图像更新
        void quitIdent();                   //停止识别

        void isStartAttendent();      //是否开启识别
        void isStartLoginIdent();     //是否开启 管理员登陆识别

        void identResult(long personIndex, int flag);         //考勤识别结果
        void loginIdentResult(long personIndex, int flag);    //管理员登陆识别结果

        void interactionStatus(bool flag);

        void progressBarValueChanged(int value);

    private:
        QString GetFacePhotoPath();
        QString GetFacePhotoSonPath(int index);


        void SetIdentMode();                        //根据配置设置识别模式

        void LoadCodeList();

        static void * IdentCapImgThread(void* arg); //上传图片数据线程

        static void *  GetFacePhoto(void* arg);     //获取人脸图像

        //回调函数funResult, recIrisL,recIrisR,lrFlag,pLeftMatchOutputParam,pRightMatchOutputParam
        static int IdentCallBack(int funResult, APIIrisInfo* recIrisL, APIIrisInfo* recIrisR, LRSucFailFlag lrFlag,
                                 APIMatchOutput* pLeftMatchOutputParam, APIMatchOutput* pRightMatchOutputParam);



        //管理员识别登陆回调
        static int LoginIdentCallBack(int funResult, APIIrisInfo* recIrisL, APIIrisInfo* recIrisR, LRSucFailFlag lrFlag,
                                      APIMatchOutput* pLeftMatchOutputParam, APIMatchOutput* pRightMatchOutputParam);
        //检测是当前否有人
        static void IdentInteractionStatusCallBack(IfHasPerson hasPerson);

        //刷卡事件回调
        static void RecvCardCallBack(const unsigned char* data, const int size);

        //将识别记录存储到数据库
        void SaveRecogToDB(PeopleInfo& person);

        //将人脸信息存储图片
        void SavePersonFaceImage(QString name, bool isSuccess);
        //向串口发送韦根、继电器信号
        void SendSingnalToSerial(PeopleInfo& person);

    private:

        Ui::IrisLogin *ui;
        //正常考勤数据
        unsigned char *_codeListL;//IN
        int _codeNumL;            //IN
        unsigned char *_codeListR;//IN
        int _codeNumR;            //IN


        //管理员考勤数据
        unsigned char *_loginCodeListL;//IN
        int _loginCodeNumL;//IN
        unsigned char *_loginCodeListR;//IN
        int _loginCodeNumR;//IN

        //算法API
        CAlgApi * _identApi;


        //灰度图像
        unsigned char _grayImgBuffer[g_constIrisImgSize + g_constBMPHeaderLength];


        //CFeatureManageForDemo _cFeatureManage;
        QVector<QString>  _personName;             //考勤人员信息
        QVector<QString>  _loginPersonName;        //管理员人员信息

        std::thread _identCapImgThread;            //识别模块图像显示线程
        std::mutex _identCapImgMutex;              //识别模块图像采集mutex
        std::condition_variable _identCapImgCV;    //识别模块图像采集condition_variable
        //    bool _identCapImgFlag;                     //识别模块图像采集flag

        int _count;                                //帧率
        static bool g_isClose;                             //是否停止识别
        bool  _isLoginIdent;                      //是否开始识别

        //人机交互类
        static Interaction *_identInteraction;
        //串口类
        SerialMuxDemux* _sm;
        LRIrisClearFlag  _irisClearFlag;
        IdentResultState  _identResultState;           //识别结果
        QTimer *_timerStartIdent;                      //识别成功或失败时显示框显示1s钟
        InteractionResultType _interactionResultType;  //绘制什么框
        AlgApiCtrlParam _idenParamStruct;//算法逻辑控制参数
    //    bool   _stopIdent;


        //虹膜管理
        IrisManager& _irisManager;
        //数据库操作类
        DatabaseAccess _dbAccess;

        //人脸API
        CCameraApi* _faceCamer;
        ImgSizeInfo  _facephotoSize;
        unsigned char* _facephotoBuf;

        //门铃定时关闭
        QTimer _bellTimer;
        int _bellTimes;      //门铃时间


        bool  _isOutWigan;   //韦根输出
        //WiganOutMode  _outMode; //输出类型---韦根

        bool  _isOutRalay;   //继电器输出
        int   _secOutRalay;
        int   _minOutRalay;


        bool  _defaultIdentMode; //默认为  非刷卡识别


        std::thread _identFaceImagThread;
        volatile bool _enableCapture;

    };

#endif // IRISLOGIN_H
