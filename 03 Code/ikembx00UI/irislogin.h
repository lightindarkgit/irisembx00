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
//** 备  注:
//**
//*****************************************************************************/
//#ifndef IRISLOGIN_H
//#define IRISLOGIN_H

//#include <QWidget>
//#include "qidentify.h"
//#include "../../03 Code/USBAPI/simplelock.h"
//#include "../../03 Code/AlgApi/SRC/AlgApi.h"
//#include "Common/interface.h"
//#include <QVector>

//#include <mutex>
//#include <thread>
//#include <condition_variable>

//namespace Ui {
//class IrisLogin;
//}

//class IrisLogin : public QWidget
//{
//    Q_OBJECT
    
//public:
//    explicit IrisLogin(QWidget *parent = 0);
//    ~IrisLogin();

//    void StartIdent();
//    void StopIdent();

//public slots:
//    //void TimerUpdate();
//    void ImageUpdate();
//    void IdentResult(long personIndex);
//private:

//    unsigned char *_codeListL;//IN
//    int _codeNumL;//IN
//    unsigned char *_codeListR;//IN
//    int _codeNumR;//IN

//    //上传图片数据线程
//    static void *IdentThreadLoop(void *arg);
//    static void * IdentCapImgThread(void* arg);

//    //    //funResult, recIrisL,recIrisR,lrFlag,pLeftMatchOutputParam,pRightMatchOutputParam
//    static int IdentCallBack(int funResult, APIIrisInfo* recIrisL, APIIrisInfo* recIrisR, LRSucFailFlag lrFlag,
//                     APIMatchOutput* pLeftMatchOutputParam, APIMatchOutput* pRightMatchOutputParam);

//private:

//    Ui::IrisLogin *ui;
//    QIdentify   * _qIdentify;
//    CAlgApi _identApi;
//    //是否开始识别
//    static bool  _isIdent;
//    //退出线程
//    bool  _exitIdentThread;

//    //线程等待
//    //ThreadWaiter  _threadWaiter;
//    //识别类型
//    //identifyType _currentIdentifyType;

//    unsigned char _grayImgBuffer[g_constIrisImgSize + g_constIrisPPMHeaderLength];

//    QVector<QString>  _personName;
//    LRIrisClearFlag _lrIrisClearFlag;
//    IrisPositionFlag _irisPositionFlag;

//    std::thread _identCapImgThread;                                //识别模块图像显示线程
//    std::mutex _identCapImgMutex;                                  //识别模块图像采集mutex
//    std::condition_variable _identCapImgCV;                        //识别模块图像采集condition_variable
//    bool _identCapImgFlag;                                         //识别模块图像采集flag
//    bool _signaledFlag;
//    bool _isStartfFlag;                                            //是否开始识别


//};

//#endif // IRISLOGIN_H
