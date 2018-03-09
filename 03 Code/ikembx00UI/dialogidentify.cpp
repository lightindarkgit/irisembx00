
/*****************************************************************************
** 文 件 名：Identity.cpp
** 主 要 类：Identity
**
** Copyright (c) 中科虹霸有限公司
**
** 创 建 人：刘中昌
** 创建时间：20013-10-13
**
** 修 改 人：
** 修改时间：
** 描  述:   识别对话框
** 主要模块说明: 识别界面的显示
**
** 版  本:   1.0.0
** 备  注:
**
*****************************************************************************/
#include <unistd.h>
#include <iostream>
#include "dialogidentify.h"
#include "ui_dialogidentify.h"
#include "time.h"
#include "QDateTime"
#include "QTime"
#include <QtCore>
#include "QThread"
#include <sys/time.h>
#include <QMessageBox>


ReDrawEvent drawEvent;
IdentEvent identEvent;
CommonFunction commonFunction;

LRIrisClearFlag lrIrisClearFlag;
IrisPositionFlag irisPositionFlag;

Interaction *DialogIdentify::_identInteraction;

DialogIdentify::DialogIdentify(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::DialogIdentify)
{
    ui->setupUi(this);

    /**********界面时间实时显示*****************/
    //获取系统现在的时间
    QDateTime time = QDateTime::currentDateTime();
    QString strTime = time.toString("yyyy-MM-dd hh:mm:ss");//设置系统时间显示格式
    ui->labTime->setText(QString::fromUtf8("时间：")+ strTime);

    _cFeatureManage.InitParam();
    _cFeatureManage.LoadTemplate();


    //新建定时器 关联当前时间
    this->startTimer(1000);

    connect(&drawEvent,SIGNAL(sigDrawIdent()),this,SLOT(ImageUpdate()));
    connect(&identEvent,SIGNAL(sigResult(long,int)),this,SLOT(IdentResult(long,int)));
    connect(&identEvent,SIGNAL(sigLoginResult(long,int)),this,SLOT(LoginIdentResult(long,int)));

    //设置界面背景
    QPalette palette;
    palette.setBrush(QPalette::Background, QBrush(QPixmap(":/image/back1.jpg")));
    ui->widgetInfo->setPalette(palette);

    //初始化算法模块
     _identApi = new CAlgApi();
    _identApi->Init();

    //启动线程显示图像
    _identCapImgFlag = true;
    _identCapImgThread = std::thread(IdentCapImgThread, (void*)this);

    _isClose = false;
    _isLoginIdent = false;

    _identInteraction = Interaction::GetInteractionInstance();
    _irisClearFlag = LAndRImgBlur;
    _identResultState = ContinueState;

    //新建定时器
    _timerStartIdent = new QTimer();
    //关联定时器计满信号和相应的槽函数
    connect(_timerStartIdent,SIGNAL(timeout()),this,SLOT(StartAttendIdent()));

    //启动异步识别识别
    //StartAttendIdent();
}

DialogIdentify::~DialogIdentify()
{
    _exitIdentThread = false;
    _identCapImgFlag = false;
    //线程join
    _identCapImgThread.join();

    delete _timerStartIdent;
    delete _identApi;
    delete _identInteraction;
    delete ui;
}


/*****************************************************************************
*                        开始识别
*  函 数 名： StartIdent
*  功    能：启动识别
*  说    明：
*  参    数：
*  返 回 值：
*  创 建 人：刘中昌
*  创建时间：2013-12-23
*  修 改 人：
*  修改时间：
*****************************************************************************/
void DialogIdentify::StartAttendIdent()
{
    _identResultState = UnKnownState;
    _timerStartIdent->stop();

    ui->labIsLogin->setText(QString::fromUtf8("考勤识别"));

    _isLoginIdent = false;

    int error = _identApi->AsyncStartIden(IdentCallBack,_cFeatureManage.s_enrollCodesL,
                                         _cFeatureManage.s_enrNumL,_cFeatureManage.s_enrollCodesR,
                                         _cFeatureManage.s_enrNumR);

    if(error!=0)
    {
        std::cout<<"开始识别失败！"<<std::endl;
       // QMessageBox::information(this, QString::fromUtf8("识别"), QString::fromUtf8("识别失败！"));
    }
    else
    {
        _isClose = false;
    }
}


/*****************************************************************************
*                        管理员登陆开始识别
*  函 数 名： StartIdent
*  功    能：启动识别
*  说    明：
*  参    数：
*  返 回 值：
*  创 建 人：刘中昌
*  创建时间：2013-12-23
*  修 改 人：
*  修改时间：
*****************************************************************************/
void DialogIdentify::StartLoginIdent()
{
    ui->labIsLogin->setText(QString::fromUtf8("管理员登陆识别"));
    _isClose = false;
    _isLoginIdent =true;

    int error = _identApi->AsyncStartIden(LoginIdentCallBack,_cFeatureManage.s_enrollCodesL,
                                         _cFeatureManage.s_enrNumL,_cFeatureManage.s_enrollCodesR,
                                         _cFeatureManage.s_enrNumR);
////    if(error!=0)
//    {
//        QMessageBox::information(this, QString::fromUtf8("识别"), QString::fromUtf8("识别失败！"));
//    }
}

/*****************************************************************************
*                        识别回调函数
*  函 数 名： IdentCallBack
*  功    能：识别结果处理
*  说    明：
*  参    数：
*  返 回 值：
             EnrRecBothFailed	=	-3,		//EnrRecBothFailed：注册、识别时左右眼均注册或识别失败
             EnrRecRightFailed	=	-2,		//EnrRecRightFailed:右眼注册、识别失败导致注册或识别失败,左眼情况未知
             EnrRecLeftFailed	=	-1,		//EnrRecLeftFailed:左眼注册、识别失败导致注册或识别失败,右眼情况未知
             EnrRecUnknown       =   0,      //EnrRecUnknown:注册、识别结果未知
             EnrRecLeftSuccess	=	1,		//EnrRecLeftSuccess:注册、识别时仅左眼注册或识别成功
             EnrRecRightSuccess	=	2,		//EnrRecRightSuccess:注册、识别时仅右眼注册或识别成功
             EnrRecBothSuccess	=	3		//EnrRecBothSuccess:注册、识别时左右眼均注册成功
*  创 建 人：刘中昌
*  创建时间：2013-12-23
*  修 改 人：
*  修改时间：
*****************************************************************************/
int DialogIdentify::IdentCallBack(int funResult, APIIrisInfo* recIrisL, APIIrisInfo* recIrisR, LRSucFailFlag lrFlag,
                                  APIMatchOutput* pLeftMatchOutputParam, APIMatchOutput* pRightMatchOutputParam)
{
    QImage img;//处理注册结果时目前只是为了适应ProcessResult的接口参数，实际没有使用
    if(funResult ==1)
    {//人为停止
        return -1;
    }else if (E_ALGAPI_ENR_IDEN_FAILED == funResult)
    {
        switch(lrFlag)
        {
        case EnrRecBothFailed://左右眼都失败
            identEvent.onResult(-1,0);
            //            commonFunction.IKSaveGrayFile("FR",recIrisR->ImgData,recIrisR->ImgWidth,recIrisR->ImgHeight);
            //            commonFunction.IKSaveGrayFile("FR",recIrisR->LocImage,recIrisR->ImgWidth,recIrisR->ImgHeight);
            //            commonFunction.IKSaveGrayFile("FL",recIrisL->ImgData,recIrisL->ImgWidth,recIrisL->ImgHeight);
            //            commonFunction.IKSaveGrayFile("FL",recIrisL->LocImage,recIrisL->ImgWidth,recIrisL->ImgHeight);
            break;
        case EnrRecRightFailed:
            identEvent.onResult(-1,0);
            //            commonFunction.IKSaveGrayFile("FR",recIrisR->ImgData,recIrisR->ImgWidth,recIrisR->ImgHeight);
            //            commonFunction.IKSaveGrayFile("FR",recIrisR->LocImage,recIrisR->ImgWidth,recIrisR->ImgHeight);
            break;
        case EnrRecLeftFailed:
            identEvent.onResult(-1,0);
            //            commonFunction.IKSaveGrayFile("FL",recIrisL->ImgData,recIrisL->ImgWidth,recIrisL->ImgHeight);
            //            commonFunction.IKSaveGrayFile("FL",recIrisL->LocImage,recIrisL->ImgWidth,recIrisL->ImgHeight);
            break;
        }
    }else if(funResult == E_OK)
    {
        switch(lrFlag)
        {
        case EnrRecLeftSuccess:
            identEvent.onResult(pLeftMatchOutputParam->MatchIndex[0],0);
            //            commonFunction.IKSaveGrayFile("TL",recIrisL->ImgData,recIrisL->ImgWidth,recIrisL->ImgHeight);
            //            commonFunction.IKSaveGrayFile("TL",recIrisL->LocImage,recIrisL->ImgWidth,recIrisL->ImgHeight);
            break;
        case EnrRecRightSuccess:
            identEvent.onResult(pRightMatchOutputParam->MatchIndex[0],1);
            //            commonFunction.IKSaveGrayFile("TR",recIrisR->ImgData,recIrisR->ImgWidth,recIrisR->ImgHeight);
            //            commonFunction.IKSaveGrayFile("TR",recIrisR->LocImage,recIrisR->ImgWidth,recIrisR->ImgHeight);
            break;
        case EnrRecBothSuccess:
            pRightMatchOutputParam->MatchScore[0]>pLeftMatchOutputParam->MatchScore[0]?
                        identEvent.onResult(pRightMatchOutputParam->MatchIndex[0],1):
                        identEvent.onResult(pLeftMatchOutputParam->MatchIndex[0],0);

            //            commonFunction.IKSaveGrayFile("TR",recIrisR->ImgData,recIrisR->ImgWidth,recIrisR->ImgHeight);
            //            commonFunction.IKSaveGrayFile("TR",recIrisR->LocImage,recIrisR->ImgWidth,recIrisR->ImgHeight);
            //            commonFunction.IKSaveGrayFile("TL",recIrisL->ImgData,recIrisL->ImgWidth,recIrisL->ImgHeight);
            //            commonFunction.IKSaveGrayFile("TL",recIrisL->LocImage,recIrisL->ImgWidth,recIrisL->ImgHeight);
            break;
        }
    }
    else
    {
        identEvent.onResult(0,0);

    }
    return 0;
}


/*****************************************************************************
*                        管理员识别回调函数
*  函 数 名： LoginIdentCallBack
*  功    能：识别结果处理
*  说    明：
*  参    数：
*  返 回 值：
             EnrRecBothFailed	=	-3,		//EnrRecBothFailed：注册、识别时左右眼均注册或识别失败
             EnrRecRightFailed	=	-2,		//EnrRecRightFailed:右眼注册、识别失败导致注册或识别失败,左眼情况未知
             EnrRecLeftFailed	=	-1,		//EnrRecLeftFailed:左眼注册、识别失败导致注册或识别失败,右眼情况未知
             EnrRecUnknown       =   0,      //EnrRecUnknown:注册、识别结果未知
             EnrRecLeftSuccess	=	1,		//EnrRecLeftSuccess:注册、识别时仅左眼注册或识别成功
             EnrRecRightSuccess	=	2,		//EnrRecRightSuccess:注册、识别时仅右眼注册或识别成功
             EnrRecBothSuccess	=	3		//EnrRecBothSuccess:注册、识别时左右眼均注册成功
*  创 建 人：刘中昌
*  创建时间：2013-12-23
*  修 改 人：
*  修改时间：
*****************************************************************************/
int DialogIdentify::LoginIdentCallBack(int funResult, APIIrisInfo* recIrisL, APIIrisInfo* recIrisR, LRSucFailFlag lrFlag,
                                       APIMatchOutput* pLeftMatchOutputParam, APIMatchOutput* pRightMatchOutputParam)
{
     QImage img;//处理注册结果时目前只是为了适应ProcessResult的接口参数，实际没有使用
    if(funResult ==1)
    {//人为停止
        return -1;
    }else if (E_ALGAPI_ENR_IDEN_FAILED == funResult)
    {
        switch(lrFlag)
        {
        case EnrRecBothFailed://左右眼都失败
            identEvent.onLoginResult(-1,2);
            //            commonFunction.IKSaveGrayFile("FR",recIrisR->ImgData,recIrisR->ImgWidth,recIrisR->ImgHeight);
            //            commonFunction.IKSaveGrayFile("FR",recIrisR->LocImage,recIrisR->ImgWidth,recIrisR->ImgHeight);
            //            commonFunction.IKSaveGrayFile("FL",recIrisL->ImgData,recIrisL->ImgWidth,recIrisL->ImgHeight);
            //            commonFunction.IKSaveGrayFile("FL",recIrisL->LocImage,recIrisL->ImgWidth,recIrisL->ImgHeight);
            break;
        case EnrRecRightFailed:
            identEvent.onLoginResult(-1,2);
            //            commonFunction.IKSaveGrayFile("FR",recIrisR->ImgData,recIrisR->ImgWidth,recIrisR->ImgHeight);
            //            commonFunction.IKSaveGrayFile("FR",recIrisR->LocImage,recIrisR->ImgWidth,recIrisR->ImgHeight);
            break;
        case EnrRecLeftFailed:
            identEvent.onLoginResult(-1,2);
            //            commonFunction.IKSaveGrayFile("FL",recIrisL->ImgData,recIrisL->ImgWidth,recIrisL->ImgHeight);
            //            commonFunction.IKSaveGrayFile("FL",recIrisL->LocImage,recIrisL->ImgWidth,recIrisL->ImgHeight);
            break;
        }
    }else if(funResult == E_OK)
    {
        switch(lrFlag)
        {
        case EnrRecLeftSuccess:
            identEvent.onLoginResult(pLeftMatchOutputParam->MatchIndex[0],0);
            //            commonFunction.IKSaveGrayFile("TL",recIrisL->ImgData,recIrisL->ImgWidth,recIrisL->ImgHeight);
            //            commonFunction.IKSaveGrayFile("TL",recIrisL->LocImage,recIrisL->ImgWidth,recIrisL->ImgHeight);
            break;
        case EnrRecRightSuccess:
            identEvent.onLoginResult(pRightMatchOutputParam->MatchIndex[0],1);
            //            commonFunction.IKSaveGrayFile("TR",recIrisR->ImgData,recIrisR->ImgWidth,recIrisR->ImgHeight);
            //            commonFunction.IKSaveGrayFile("TR",recIrisR->LocImage,recIrisR->ImgWidth,recIrisR->ImgHeight);
            break;
        case EnrRecBothSuccess:
            pRightMatchOutputParam->MatchScore[0]>pLeftMatchOutputParam->MatchScore[0]?
                        identEvent.onLoginResult(pRightMatchOutputParam->MatchIndex[0],1):
                        identEvent.onLoginResult(pLeftMatchOutputParam->MatchIndex[0],0);

            //            commonFunction.IKSaveGrayFile("TR",recIrisR->ImgData,recIrisR->ImgWidth,recIrisR->ImgHeight);
            //            commonFunction.IKSaveGrayFile("TR",recIrisR->LocImage,recIrisR->ImgWidth,recIrisR->ImgHeight);
            //            commonFunction.IKSaveGrayFile("TL",recIrisL->ImgData,recIrisL->ImgWidth,recIrisL->ImgHeight);
            //            commonFunction.IKSaveGrayFile("TL",recIrisL->LocImage,recIrisL->ImgWidth,recIrisL->ImgHeight);
            break;
        }
    }
    else
    {
        identEvent.onLoginResult(0,3);

    }
    return 0;
}


/*****************************************************************************
*                        显示识别结果函数
*  函 数 名：IdentResult
*  功    能：识别结果
*  说    明：
*  参    数：
*  返 回 值：
*  创 建 人：刘中昌
*  创建时间：2013-12-27
*  修 改 人：
*  修改时间：
*****************************************************************************/
void DialogIdentify::IdentResult(long personIndex,int flag)
{
    if(personIndex<0)
    {
        ui->labStatu->setText(QString::fromUtf8("识别失败"));
        _identResultState = FailedState;
    }
    else if(personIndex == 0)
    {
        ui->labStatu->setText(QString::fromUtf8("识别中..."));
        _identResultState = ContinueState;
    }
    else
    {
        _identResultState = SuccessState;
        if(0==flag)
        {
            //左眼
            ui->labStatu->setText(_cFeatureManage.s_strNameLeft[personIndex]+ QString::fromUtf8("  识别成功"));
        }
        else
        {
            //右眼
            ui->labStatu->setText(_cFeatureManage.s_strNameRight[personIndex]+ QString::fromUtf8("  识别成功"));
        }

    }
    std::lock_guard<std::mutex> lck (_identCapImgMutex, std::adopt_lock);

    if(!_isClose && !_isLoginIdent && _identResultState == ContinueState)
    {
        //异步识别识别
        StartAttendIdent();
    }

}



/*****************************************************************************
*                        显示识别结果函数
*  函 数 名：LoginIdentResult
*  功    能：识别结果
*  说    明：
*  参    数：
*  返 回 值：
*  创 建 人：刘中昌
*  创建时间：2013-12-27
*  修 改 人：
*  修改时间：
*****************************************************************************/
void DialogIdentify::LoginIdentResult(long personIndex,int flag)
{
    if(personIndex<0)
    {
        ui->labStatu->setText(QString::fromUtf8("识别失败"));
    }
    else if(personIndex == 0)
    {
        ui->labStatu->setText(QString::fromUtf8("识别中..."));
    }
    else
    {
        if(personIndex<_personName.size())
        {
            ui->labStatu->setText(_loginPersonName[personIndex] + QString::fromUtf8("识别成功"));
            emit sigLoginSucceed(true);
            //停止识别
            StopIdent();
            return;
        }
    }
    std::lock_guard<std::mutex> lck (_identCapImgMutex, std::adopt_lock);

    if(!_isClose && _isLoginIdent)
    {
        //异步识别识别
        StartLoginIdent();
    }
}

/*****************************************************************************
*                        停止识别函数
*  函 数 名：StopIdent
*  功    能：停止识别
*  说    明：
*  参    数：
*  返 回 值：
*  创 建 人：刘中昌
*  创建时间：2013-12-23
*  修 改 人：
*  修改时间：
*****************************************************************************/
void DialogIdentify::StopIdent()
{
    if(_isClose)
    {
        return;
    }
    _isClose = true;
    int error = _identApi->StopIden();
    if(error != 0)
    {
        std::cout<<"停止识别失败！ "<<std::endl;
    }
}

/*****************************************************************************
*                        界面时间更新显示
*  函 数 名：timerEvent
*  功    能：界面时间更新显示
*  说    明：
*  参    数：
*  返 回 值：
*  创 建 人：刘中昌
*  创建时间：2013-10-22
*  修 改 人：
*  修改时间：
*****************************************************************************/
void DialogIdentify::timerEvent(QTimerEvent *event)
{
    //获取系统现在的时间
    QDateTime time = QDateTime::currentDateTime();
    //设置系统时间显示格式
    QString strTime = time.toString("yyyy-MM-dd hh:mm:ss");
    //界面显示
    ui->labTime->setText(QString::fromUtf8("时间：")+ strTime + QString::fromUtf8("  帧率： ") + QString::number(_count));
    _count =0;
}

/*****************************************************************************
*                         注册模块将采集到的图像显示到界面
*  函 数 名：ImageUpdate
*  功    能：采图
*  说    明：
*  参    数：pParam：输入参数
*
*  返 回 值：NULL
*  创 建 人：liuzhch
*  创建时间：2013-12-25
*  修 改 人：
*  修改时间：
*****************************************************************************/
void DialogIdentify::ImageUpdate()
{
    std::lock_guard<std::mutex> lck (_identCapImgMutex, std::adopt_lock);
    QImage imageBig;
    imageBig = QImage::fromData(_grayImgBuffer, g_constIrisImgSize + g_constBMPHeaderLength, "BMP");

    if(!imageBig.isNull())
    {
        QImage paintImg = imageBig.convertToFormat(QImage::Format_ARGB32_Premultiplied);

        //状态判断与执行类型处理
        if(_identResultState == SuccessState)
        {
            _identResultState = KeepState;
            _timerStartIdent->start(1000);
            _interactionResultType = IdenSuccess;
        }
        else if(_identResultState == FailedState)
        {
            _identResultState = KeepState;
            _timerStartIdent->start(1000);
            _interactionResultType = IdenFailed;
        }
        else if(_identResultState == KeepState)
        {
             _interactionResultType = IdenKeepLastStatus;
        }
        else
        {
            _interactionResultType = ResultUnknown;
        }

//        if(paintImg.isNull())
//        {
//            return;
//        }
//        std::cout<<"ident : "<<lrIrisClearFlag<<std::endl;
        //绘框
        _identInteraction->PlayInteraction(_interactionResultType,lrIrisClearFlag,irisPositionFlag,paintImg);
        paintImg = paintImg.scaled (65535,ui->labEye->width(),
                                    Qt::KeepAspectRatio);

        ui->labEye->setPixmap(QPixmap::fromImage(paintImg));
        _count ++;
    }
    else
    {
        std::cout<<"识别显示未获得图像！"<<std::endl;
    }
}

/*****************************************************************************
*                           识别模块采图线程
*  函 数 名：IdentCapImgThread
*  功    能：采图
*  说    明：
*  参    数：pParam：输入参数
*
*  返 回 值：NULL
*  创 建 人：liuzhch
*  创建时间：2013-12-25
*  修 改 人：
*  修改时间：
*****************************************************************************/
void* DialogIdentify::IdentCapImgThread(void* arg)
{
    DialogIdentify *pCGDlg = static_cast <DialogIdentify*> (arg);

    int errorResult = E_ALGAPI_ERROR_BASE;

    while(pCGDlg->_identCapImgFlag)
    {
        if(!pCGDlg->_isClose)
        {
            std::lock_guard<std::mutex> lck (pCGDlg->_identCapImgMutex, std::adopt_lock);

            errorResult = pCGDlg->_identApi->SyncCapIrisImg(pCGDlg->_identInteraction->s_infraredDistanceValue, pCGDlg->_grayImgBuffer,
                                                            lrIrisClearFlag, irisPositionFlag);
            if(E_ALGAPI_OK == errorResult)
            {
                drawEvent.OnDrawIdent();
            }
        }
        else
        {
            sleep(0);
        }
    }
    return (void*)0;
}

