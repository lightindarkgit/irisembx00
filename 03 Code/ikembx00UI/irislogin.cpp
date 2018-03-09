///*****************************************************************************
//** 文 件 名： IrisLogin.cpp
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
//#include "irislogin.h"
//#include "ui_irislogin.h"
//#include "ui_dialogidentify.h"
//#include "time.h"
//#include "QDateTime"
//#include "QTime"
//#include <QtCore>
//#include "QThread"
//#include <sys/time.h>

//ReDrawEvent drawEvent;
//IdentEvent identEvent;
//CommonFunction commonFunction;
//IrisLogin::IrisLogin(QWidget *parent) :
//    QWidget(parent),
//    ui(new Ui::IrisLogin)
//{
//    ui->setupUi(this);
////    connect(ui->toolBtnGoBack,SIGNAL(clicked()),this,SIGNAL(sigGoBack()));
//    _identApi.Init();

//    /**********界面时间实时显示*****************/
//    //获取系统现在的时间
////    QDateTime time = QDateTime::currentDateTime();
////    QString strTime = time.toString("yyyy-MM-dd hh:mm:ss");//设置系统时间显示格式
////    ui->labTime->setText(QString::fromUtf8("时间：")+ strTime);

//    _personName.append(QString::fromUtf8("张三"));
//    _personName.append(QString::fromUtf8("李斯"));
//    _personName.append(QString::fromUtf8("王二"));
//    _personName.append(QString::fromUtf8("董经"));
//    _personName.append(QString::fromUtf8("武郈"));
//    _personName.append(QString::fromUtf8("蔷维"));

//    //新建定时器 关联当前时间
////    QTimer *timer = new QTimer(this);
////    connect(timer,SIGNAL(timeout()),this,SLOT(TimerUpdate()));
////    timer->start(1000);

//    connect(&drawEvent,SIGNAL(sigDrawIdent()),this,SLOT(ImageUpdate()));
//    connect(&identEvent,SIGNAL(sigResult(long)),this,SLOT(IdentResult(long)));
//    //设置界面背景
//    QPalette palette;
//    palette.setBrush(QPalette::Background, QBrush(QPixmap(":/image/back1.jpg")));
//    ui->widgetAdmin->setPalette(palette);

//    //初始化算法模块
//    _identApi.Init();


//    //设置图片头
//    commonFunction.SetShowImageHeader(_grayImgBuffer,sizeof(_grayImgBuffer));


//    _signaledFlag = true;
//    _identCapImgFlag = true;
//    _identCapImgThread = std::thread(IdentCapImgThread, (void*)this);

//    //启动异步识别识别
//    StartIdent();
//    _isStartfFlag = true;

//}

//IrisLogin::~IrisLogin()
//{
//    _exitIdentThread = false;
//    _identCapImgFlag = false;
//    _identCapImgThread.join();
//    delete ui;
//    delete _qIdentify;
//}



///*****************************************************************************
//*                        开始识别
//*  函 数 名： StartIdent
//*  功    能：启动识别
//*  说    明：
//*  参    数：
//*  返 回 值：
//*  创 建 人：刘中昌
//*  创建时间：2013-12-23
//*  修 改 人：
//*  修改时间：
//*****************************************************************************/
//void IrisLogin::StartIdent()
//{
//    _isStartfFlag = true;
//    _identApi.AsyncStartIden(IdentCallBack,_codeListL,_codeNumL,_codeListR,_codeNumR);
//}


///*****************************************************************************
//*                        识别回调函数
//*  函 数 名： IdentCallBack
//*  功    能：识别结果处理
//*  说    明：
//*  参    数：
//*  返 回 值：
//             EnrRecBothFailed	=	-3,		//EnrRecBothFailed：注册、识别时左右眼均注册或识别失败
//             EnrRecRightFailed	=	-2,		//EnrRecRightFailed:右眼注册、识别失败导致注册或识别失败,左眼情况未知
//             EnrRecLeftFailed	=	-1,		//EnrRecLeftFailed:左眼注册、识别失败导致注册或识别失败,右眼情况未知
//             EnrRecUnknown       =   0,      //EnrRecUnknown:注册、识别结果未知
//             EnrRecLeftSuccess	=	1,		//EnrRecLeftSuccess:注册、识别时仅左眼注册或识别成功
//             EnrRecRightSuccess	=	2,		//EnrRecRightSuccess:注册、识别时仅右眼注册或识别成功
//             EnrRecBothSuccess	=	3		//EnrRecBothSuccess:注册、识别时左右眼均注册成功
//*  创 建 人：刘中昌
//*  创建时间：2013-12-23
//*  修 改 人：
//*  修改时间：
//*****************************************************************************/
//int IrisLogin::IdentCallBack(int funResult, APIIrisInfo* recIrisL, APIIrisInfo* recIrisR, LRSucFailFlag lrFlag,
//                                 APIMatchOutput* pLeftMatchOutputParam, APIMatchOutput* pRightMatchOutputParam)
//{
//    if(funResult ==1)
//    {//人为停止
//        return -1;
//    }else if(funResult == E_OK)
//    {
//        switch(lrFlag)
//        {
//        case EnrRecBothFailed://左右眼都失败
//            identEvent.onResult(-1);
//            commonFunction.IKSaveGrayFile("FR",recIrisR->ImgData,recIrisR->ImgWidth,recIrisR->ImgHeight);
//            commonFunction.IKSaveGrayFile("FR",recIrisR->LocImage,recIrisR->ImgWidth,recIrisR->ImgHeight);
//            commonFunction.IKSaveGrayFile("FL",recIrisL->ImgData,recIrisL->ImgWidth,recIrisL->ImgHeight);
//            commonFunction.IKSaveGrayFile("FL",recIrisL->LocImage,recIrisL->ImgWidth,recIrisL->ImgHeight);
//            break;
//        case EnrRecRightFailed:
//            identEvent.onResult(-1);
//            commonFunction.IKSaveGrayFile("FR",recIrisR->ImgData,recIrisR->ImgWidth,recIrisR->ImgHeight);
//            commonFunction.IKSaveGrayFile("FR",recIrisR->LocImage,recIrisR->ImgWidth,recIrisR->ImgHeight);
//            break;
//        case EnrRecLeftFailed:
//            identEvent.onResult(-1);
//            commonFunction.IKSaveGrayFile("FL",recIrisL->ImgData,recIrisL->ImgWidth,recIrisL->ImgHeight);
//            commonFunction.IKSaveGrayFile("FL",recIrisL->LocImage,recIrisL->ImgWidth,recIrisL->ImgHeight);
//            break;
//        case EnrRecUnknown:
//            identEvent.onResult(0);
//            break;
//        case EnrRecLeftSuccess:
//            identEvent.onResult(pLeftMatchOutputParam->MatchIndex[0]);
//            commonFunction.IKSaveGrayFile("TL",recIrisL->ImgData,recIrisL->ImgWidth,recIrisL->ImgHeight);
//            commonFunction.IKSaveGrayFile("TL",recIrisL->LocImage,recIrisL->ImgWidth,recIrisL->ImgHeight);
//            break;
//        case EnrRecRightSuccess:
//            identEvent.onResult(pRightMatchOutputParam->MatchIndex[0]);
//            commonFunction.IKSaveGrayFile("TR",recIrisR->ImgData,recIrisR->ImgWidth,recIrisR->ImgHeight);
//            commonFunction.IKSaveGrayFile("TR",recIrisR->LocImage,recIrisR->ImgWidth,recIrisR->ImgHeight);
//            break;
//        case EnrRecBothSuccess:
//            if(pRightMatchOutputParam->MatchIndex[0] == pLeftMatchOutputParam->MatchIndex[0])
//            {
//                identEvent.onResult(pRightMatchOutputParam->MatchIndex[0]);
//            }
//            else
//            {
//                pRightMatchOutputParam->MatchScore[0]>pLeftMatchOutputParam->MatchScore[0]?
//                            identEvent.onResult(pRightMatchOutputParam->MatchIndex[0]):
//                            identEvent.onResult(pLeftMatchOutputParam->MatchIndex[0]);
//            }
//            commonFunction.IKSaveGrayFile("TR",recIrisR->ImgData,recIrisR->ImgWidth,recIrisR->ImgHeight);
//            commonFunction.IKSaveGrayFile("TR",recIrisR->LocImage,recIrisR->ImgWidth,recIrisR->ImgHeight);
//            commonFunction.IKSaveGrayFile("TL",recIrisL->ImgData,recIrisL->ImgWidth,recIrisL->ImgHeight);
//            commonFunction.IKSaveGrayFile("TL",recIrisL->LocImage,recIrisL->ImgWidth,recIrisL->ImgHeight);
//            break;
//        }
//    }
//    else
//    {
//        sleep(2);
//        identEvent.onResult(0);

//    }
//    return 0;
//}


///*****************************************************************************
//*                        显示识别结果函数
//*  函 数 名：StopIdent
//*  功    能：识别结果
//*  说    明：
//*  参    数：
//*  返 回 值：
//*  创 建 人：刘中昌
//*  创建时间：2013-12-27
//*  修 改 人：
//*  修改时间：
//*****************************************************************************/
//void IrisLogin::IdentResult(long personIndex)
//{
//    if(personIndex<0)
//    {
//        ui->labStatu->setText(QString::fromUtf8("识别失败"));
//    }
//    else if(personIndex == 0)
//    {
//        ui->labStatu->setText(QString::fromUtf8("识别中..."));
//    }
//    else
//    {
//        if(personIndex<_personName.size())
//        {
//            ui->labStatu->setText(_personName[personIndex]);
//        }
//    }
//    std::lock_guard<std::mutex> lck (_identCapImgMutex, std::adopt_lock);

//    if(_isStartfFlag)
//    {
//        //异步识别识别
//        StartIdent();
//    }

//}

///*****************************************************************************
//*                        停止识别函数
//*  函 数 名：StopIdent
//*  功    能：停止识别
//*  说    明：
//*  参    数：
//*  返 回 值：
//*  创 建 人：刘中昌
//*  创建时间：2013-12-23
//*  修 改 人：
//*  修改时间：
//*****************************************************************************/
//void IrisLogin::StopIdent()
//{
//    _identApi.StopIden();
//    _isStartfFlag = false;
//}

/////*****************************************************************************
////*                        界面时间更新显示
////*  函 数 名：TimerUpdate
////*  功    能：界面时间更新显示
////*  说    明：
////*  参    数：
////*  返 回 值：
////*  创 建 人：刘中昌
////*  创建时间：2013-10-22
////*  修 改 人：
////*  修改时间：
////*****************************************************************************/
////void IrisLogin::TimerUpdate()
////{
////    //获取系统现在的时间
////    QDateTime time = QDateTime::currentDateTime();
////    //设置系统时间显示格式
////    QString strTime = time.toString("yyyy-MM-dd hh:mm:ss");
////    //界面显示
////    ui->labTime->setText(QString::fromUtf8("时间：")+ strTime);
////}
///*****************************************************************************
//*                         注册模块将采集到的图像显示到界面
//*  函 数 名：ImageUpdate
//*  功    能：采图
//*  说    明：
//*  参    数：pParam：输入参数
//*
//*  返 回 值：NULL
//*  创 建 人：liuzhch
//*  创建时间：2013-12-25
//*  修 改 人：
//*  修改时间：
//*****************************************************************************/
//void IrisLogin::ImageUpdate()
//{

//    std::lock_guard<std::mutex> lck (_identCapImgMutex, std::adopt_lock);
//    QImage imageBig;
//    imageBig = QImage::fromData(_grayImgBuffer, g_constIrisImgSize + g_constIrisPPMHeaderLength, "PPM");
//    if(!imageBig.isNull())
//    {
//        imageBig = imageBig.scaled (65535,ui->labEye->width(),
//                                    Qt::KeepAspectRatio);
//        //std::cout << "(" << imageBig.width() << ", " << imageBig.height() << "), (" << ui->labEye->width() << ", " << ui->labEye->height() << ")" << std::endl;
//        ui->labEye->setPixmap(QPixmap::fromImage(imageBig));
//    }
//    _signaledFlag = true;
//}

///*****************************************************************************
//*                           识别模块采图线程
//*  函 数 名：IdentCapImgThread
//*  功    能：采图
//*  说    明：
//*  参    数：pParam：输入参数
//*
//*  返 回 值：NULL
//*  创 建 人：liuzhch
//*  创建时间：2013-12-25
//*  修 改 人：
//*  修改时间：
//*****************************************************************************/
//void* IrisLogin::IdentCapImgThread(void* arg)
//{
//    IrisLogin *pCGDlg = static_cast <IrisLogin*> (arg);

//    int funResult = E_ALGAPI_ERROR_BASE;

//    while(pCGDlg->_identCapImgFlag)
//    {
//        std::lock_guard<std::mutex> lck (pCGDlg->_identCapImgMutex, std::adopt_lock);

//        funResult = pCGDlg->_identApi.SyncCapIrisImg(pCGDlg->_grayImgBuffer+ g_constIrisPPMHeaderLength, pCGDlg->_lrIrisClearFlag, pCGDlg->_irisPositionFlag);
//        if(E_ALGAPI_OK == funResult)
//        {
//            if(pCGDlg->_signaledFlag)
//            {
//                drawEvent.OnDrawIdent();
//            }

//            pCGDlg->_signaledFlag = false;
//        }
//        usleep(20);
//    }
//    return (void*)0;
//}

