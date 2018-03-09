/******************************************************************************************
** 文件名:   AlgBase.cpp
×× 主要类:   CAPIBase
**
** Copyright (c) 中科虹霸有限公司
** 创建人:   lizhl
** 日  期:   2013-11-14
** 修改人:
** 日  期:
** 描  述:   IKEMBX00项目算法逻辑API内部实现
 *×× 主要模块说明: API功能内部具体实现
**
** 版  本:   1.0.0
** 备  注:
**
*****************************************************************************************/
#include <iostream>
#include "AlgApi.h"
#include "ApiBase.h"

#ifdef ALGAPITEST
#include "VirtualReadImg.h"
#endif

using namespace std;

mutex CAPIBase::s_instanceMutex;

//shared_ptr<CAPIBase> CAPIBase::s_pSingletonCAPIBase;
CAPIBase* CAPIBase::s_pSingletonCAPIBase = NULL;

/*****************************************************************************
*                         CAPIBase构造函数
*  函 数 名：CAPIBase
*  功    能：构造函数
*  说    明：
*  参    数：NULL
*  返 回 值：NULL
*  创 建 人：lizhl
*  创建时间：2013-11-16
*  修 改 人：
*  修改时间：
*****************************************************************************/
CAPIBase::CAPIBase()
{

}

/*****************************************************************************
*                         CAPIBase析构函数
*  函 数 名：~CAPIBase
*  功    能：析构函数
*  说    明：
*  参    数：NULL
*  返 回 值：NULL
*  创 建 人：lizhl
*  创建时间：2013-11-16
*  修 改 人：
*  修改时间：
*****************************************************************************/
CAPIBase::~CAPIBase()
{
//    delete _pWakeMainThreadCond;
//    delete _pXferBestImgCond;
//    delete _pXferCurrentImgCond;
}

/*****************************************************************************
*                         获取CAPIBase实例
*  函 数 名：GetInstance
*  功    能：获取CAPIBase实例
*  说    明：CAPIBase采用单件模式，且多线程安全
*  参    数：NULL
*
*  返 回 值：!NULL: CAPIBase实例指针
*			NULL：获取失败
*  创 建 人：lizhl
*  创建时间：2013-11-16
*  修 改 人：
*  修改时间：
*****************************************************************************/
//shared_ptr<CAPIBase> CAPIBase::GetInstance()
CAPIBase* CAPIBase::GetInstance()
{
    // 双重检查，提高性能
    if (NULL == s_pSingletonCAPIBase)
    {
        lock_guard<mutex> lck (s_instanceMutex);

        if (NULL == s_pSingletonCAPIBase)
        {
            s_pSingletonCAPIBase = new CAPIBase();
            s_pSingletonCAPIBase->_instanceNum = 0;
            s_pSingletonCAPIBase->_mainThreadFlag = false;//初始值
            s_pSingletonCAPIBase->_isWake = false;//初始值            
        }

    }

    return s_pSingletonCAPIBase;
}

/*****************************************************************************
*                         算法逻辑API初始化
*  函 数 名：Init
*  功    能：初始化算法逻辑API使用的相关资源
*  说    明：
*  参    数：strFunRts：输出参数，以字符串形式表示本次函数调用结果，由用户分配
*			 内存空间
*
*  返 回 值：0：初始化成功
*			 <0：初始化失败
*  创 建 人：lizhl
*  创建时间：2013-11-16
*  修 改 人：
*  修改时间：
*****************************************************************************/
int CAPIBase::Init(OUT char *strFunRts)
{
    int funResult = E_ALGAPI_DEFAUL_ERROR;
    //如果是第一次初始化，则初始化相关资源，初始化USB摄像头，开启采图线程
    if(0 == _instanceNum)
    {
        _algHandle = IKIR_HandleInit(strFunRts);
        if(NULL == _algHandle)
        {
            SetFunRt(strFunRts,E_ALGAPI_INIT_FAILED, "Get algorithm's handle failed");
            return E_ALGAPI_INIT_FAILED;
        }

        //打开USB模块
#ifndef ALGAPITEST
        _syncUsbApi = new SyncUSBAPI();
        funResult = _syncUsbApi->Open();
        if(E_ALGAPI_OK != funResult)
        {
            SetFunRt(strFunRts,E_ALGAPI_INIT_FAILED, "Init usb failed");
            return E_ALGAPI_INIT_FAILED;
        }
#endif
        //初始化虹膜图像信息结构体
        CASIAGenInitIrisInfo(&_leftCurImg);
        CASIAGenInitIrisInfo(&_rightCurImg);
        CASIAGenInitIrisInfo(&_leftBestImg);
        CASIAGenInitIrisInfo(&_rightBestImg);

        _keepIdenFlag = false;//初始值
        _outIdenFlag = true;//初始值
        _keepEnrFlag = false;//初始值
        _outEnrFlag = true;//初始值

        _getNewImgFlag = false;//初始值
        _getLeftNewGoodImgFlag = false;//初始值
        _getRightNewGoodImgFlag = false;//初始值

//        _asyncCapImgFlag = nullptr;
        _asyncCapImgFlag = false;
        _asyncEnrFlag = false;
        _asyncIdenFlag = false;

        //初始化回调相关指针
        InitCapCBSrc();
        InitEnrCBSrc();
        InitIdenCBSrc();

        _mainThreadFlag = true;
        _isWake = true;

        _tThreadMain = thread(ThreadMain, s_pSingletonCAPIBase);
        _tAsyncCapImg = thread(ThreadAsyncCaptureImg, s_pSingletonCAPIBase);
        _tAsyncEnroll = thread(ThreadAsyncEnroll, s_pSingletonCAPIBase);
        _tAsyncIden = thread(ThreadAsyncIden, s_pSingletonCAPIBase);

    }

    if(!_isWake)
    {
        funResult = Wake(strFunRts);
    }

    if(_keepIdenFlag)
    {
        funResult = StopIden(strFunRts);
        if(E_ALGAPI_OK != funResult)
        {
            SetFunRt(strFunRts,E_ALGAPI_INIT_FAILED, "Stop iden failed");
            return E_ALGAPI_INIT_FAILED;
        }
    }

    if(_keepEnrFlag)
    {
        funResult = StopEnroll(strFunRts);
        if(E_ALGAPI_OK != funResult)
        {
            SetFunRt(strFunRts,E_ALGAPI_INIT_FAILED, "Stop enroll failed");
            return E_ALGAPI_INIT_FAILED;
        }
    }

    //是否需要对采图功能进行初始化？？
    if(_cbCapIrisImg)
    {
        InitCapCBSrc();
    }


    _qualityMode = IR_WORK_MODE_REC;//默认为识别模式
    _algApiCtrlParam.EyeMode = AnyEye;//默认为任意眼
    _algApiCtrlParam.FindMode = g_constFindModeExhaust;//遍历搜索，默认值
    _algApiCtrlParam.IfSpoofDetect = g_constIKNoSpoofDetect;		//不执行假眼检测，默认值
    _algApiCtrlParam.MaxENRTime = g_constMaxENRTime;		//目前实现方式的单位是秒，如果注册过程超过这个时间，则返回失败
    _algApiCtrlParam.MaxRECTime = g_constMaxRECTime;		//单位秒，如果识别过程超过这个时间，则返回失败

    _instanceNum++;//必须是初始化成功后再+1
    SetFunRt(strFunRts,E_ALGAPI_OK, "Init success");
    return E_ALGAPI_OK;
}

/*****************************************************************************
*                         算法逻辑API释放
*  函 数 名：Release
*  功    能：释放算法逻辑API中使用的相关资源
*  说    明：
*  参    数：strFunRts：输出参数，以字符串形式表示本次函数调用结果，由用户分配
*			 内存空间
*
*  返 回 值：0：释放成功
*			 <0：释放失败
*  创 建 人：lizhl
*  创建时间：2013-11-16
*  修 改 人：
*  修改时间：
*****************************************************************************/
//int CAPIBase::Release(OUT char *strFunRts)
//{
////    s_instanceNum--;//必须是必要步骤完成后再-1，
////    if(0 == s_instanceNum)
////    {
////        //释放USB驱动及相关资源
//////        CAPIBase::~CAPIBase();
////        s_pSingletonCAPIBase.reset();
//////        ~CAPIBase();
////    }
//    return E_ALGAPI_OK;
//}

/*****************************************************************************
*                         算法逻辑API系统休眠
*  函 数 名：Sleep
*  功    能：使算法逻辑API进入休眠状态，不采集虹膜图像，但不释放相关资源
*  说    明：
*  参    数：strFunRts：输出参数，以字符串形式表示本次函数调用结果，由用户分配
*			 内存空间
*
*  返 回 值：0：系统休眠成功
*    		<0：系统休眠失败，根据返回值可以查询失败原因，或者参考strFunRts
*  创 建 人：lizhl
*  创建时间：2013-11-05
*  修 改 人：
*  修改时间：
*****************************************************************************/
int CAPIBase::Sleep(OUT char *strFunRts)
{
    if(!_isWake)
    {
        SetFunRt(strFunRts,E_ALGAPI_SLEEP_FAILED,"System is already sleep");
        return E_ALGAPI_SLEEP_FAILED;
    }

    int funResult;
    if(_keepIdenFlag)
    {
        funResult = StopIden(strFunRts);
    }

    if(_keepEnrFlag)
    {
        funResult = StopEnroll(strFunRts);
    }

    lock_guard<mutex> lck(_wakeMainThreadMutex);
    _isWake = false;
    SetFunRt(strFunRts,E_ALGAPI_OK, "Sleep success");
    return E_ALGAPI_OK;
}

/*****************************************************************************
*                         算法逻辑API系统唤醒
*  函 数 名：Wake
*  功    能：使算法逻辑API从休眠状态唤醒，恢复正常工作状态
*  说    明：
*  参    数：strFunRts：输出参数，以字符串形式表示本次函数调用结果，由用户分配
*			 内存空间
*
*  返 回 值：0：系统唤醒成功
*    		<0：系统唤醒失败，根据返回值可以查询失败原因，或者参考strFunRts
*  创 建 人：lizhl
*  创建时间：2013-11-05
*  修 改 人：
*  修改时间：
*****************************************************************************/
int CAPIBase::Wake(OUT char *strFunRts)
{
//    _pWakeMainThreadCond->Lock();
//    _pWakeMainThreadCond->Signal();
//    _pWakeMainThreadCond->Unlock();
//    while(!_isWake)
//    {
//        usleep(0);
//    }

    if(_isWake)
    {
        SetFunRt(strFunRts,E_ALGAPI_WAKE_FAILED,"System is already wake");
        return E_ALGAPI_WAKE_FAILED;
    }

    unique_lock <mutex> lck(_wakeMainThreadMutex);
    _isWake = true;
    _wakeMainThreadCV.notify_one();
    SetFunRt(strFunRts,E_ALGAPI_OK, "Wake success");
    return E_ALGAPI_OK;
}

/*****************************************************************************
*                         算法逻辑API控制参数获取
*  函 数 名：GetParam
*  功    能：获取算法逻辑API控制参数
*  说    明：
*  参    数：paramStruct：输出参数，算法逻辑API控制参数
*            strFunRts：输出参数，以字符串形式表示本次函数调用结果，由用户分配
*			 内存空间
*
*  返 回 值：0：参数获取成功
*    		<0：参数获取失败，根据返回值可以查询失败原因，或者参考strFunRts
*  创 建 人：lizhl
*  创建时间：2013-11-05
*  修 改 人：
*  修改时间：
*****************************************************************************/
int CAPIBase::GetParam(OUT AlgApiCtrlParam *paramStruct, OUT char *strFunRts)
{
    if(!_isWake)
    {
        SetFunRt(strFunRts,E_ALGAPI_SYSTEM_IS_SLEEP,"System is sleep");
        return E_ALGAPI_SYSTEM_IS_SLEEP;
    }

    if( sizeof(*paramStruct) != sizeof(_algApiCtrlParam) )
    {
        SetFunRt(strFunRts,E_ALGAPI_INVALID_PARAMETER, "Get algorithm's control parameter failed");
        return E_ALGAPI_INVALID_PARAMETER;
    }

    memcpy(paramStruct, &_algApiCtrlParam, sizeof(_algApiCtrlParam));
    SetFunRt(strFunRts,E_ALGAPI_OK, "Get algorithm's control parameter success");
    return E_ALGAPI_OK;
}

/*****************************************************************************
*                         算法逻辑API控制参数设置
*  函 数 名：SetParam
*  功    能：设置算法逻辑API控制参数
*  说    明：
*  参    数：paramStruct：输出参数，算法逻辑API控制参数
*            strFunRts：输出参数，以字符串形式表示本次函数调用结果，由用户分配
*			 内存空间
*
*  返 回 值：0：参数获取成功
*    		<0：参数获取失败，根据返回值可以查询失败原因，或者参考strFunRts
*  创 建 人：lizhl
*  创建时间：2013-11-05
*  修 改 人：
*  修改时间：
*****************************************************************************/
int CAPIBase::SetParam(IN AlgApiCtrlParam* paramStruct, OUT char* strFunRts)
{
    if(!_isWake)
    {
        SetFunRt(strFunRts,E_ALGAPI_SYSTEM_IS_SLEEP,"System is sleep");
        return E_ALGAPI_SYSTEM_IS_SLEEP;
    }

    if( sizeof(*paramStruct) != sizeof(_algApiCtrlParam) )
    {
        SetFunRt(strFunRts,E_ALGAPI_INVALID_PARAMETER, "Set algorithm's control parameter failed");
        return E_ALGAPI_INVALID_PARAMETER;
    }

    if( (paramStruct->EyeMode < BothEye) || (paramStruct->EyeMode > AnyEye) )
    {
        SetFunRt(strFunRts,E_ALGAPI_INVALID_EYEMODE, "Invalid eyemode! Set algorithm's control parameter failed");
        return E_ALGAPI_INVALID_EYEMODE;
    }

    if( (g_constFindModeExhaust != paramStruct->FindMode) && (g_constFindModeFirstMatch != paramStruct->FindMode) )
    {
        SetFunRt(strFunRts,E_ALGAPI_INVALID_FINDMODE, "Invalid findmode! Set algorithm's control parameter failed");
        return E_ALGAPI_INVALID_FINDMODE;
    }

    if( (g_constIKNoSpoofDetect != paramStruct->IfSpoofDetect) && (g_constIKDoSpoofDetect != paramStruct->IfSpoofDetect) )
    {
        SetFunRt(strFunRts,E_ALGAPI_INVALID_IFSPOOFDETECT, "Invalid ifspoofdetect! Set algorithm's control parameter failed");
        return E_ALGAPI_INVALID_IFSPOOFDETECT;
    }

    if( g_constMinENRTime > paramStruct->MaxENRTime)
    {
        SetFunRt(strFunRts,E_ALGAPI_INVALID_TIME,"Invalid maxenrtime! Set algorithm's control parameter failed");
        return E_ALGAPI_INVALID_TIME;
    }

    if(g_constMaxRECTime < paramStruct->MaxRECTime)
    {
        SetFunRt(strFunRts,E_ALGAPI_INVALID_TIME,"Invalid maxrectime! Set algorithm's control parameter failed");
        return E_ALGAPI_INVALID_TIME;
    }

    memcpy(&_algApiCtrlParam,paramStruct,sizeof(_algApiCtrlParam));
    SetFunRt(strFunRts,E_ALGAPI_OK, "Set algorithm's control parameter success");
    return E_ALGAPI_OK;
}

/*****************************************************************************
*                         算法逻辑API采集虹膜图像
*  函 数 名：SyncCapIrisImg
*  功    能：同步方式采集虹膜图像，采集到的图像可用于实时显示
*  说    明：
*  参    数：pIrisImg：输出参数，采集到的图像，由用户分配内存空间
*			 lrIrisClearFlag：输出参数，表明本次采集到的图像中是否有合格的虹膜图像，该标志可用于指示界面显示红、绿提示框
*				LAndRImgBlur	=	0,	//左眼、右眼图像都不合格；
*				LImgClear		=	1,	//左眼图像合格
*				RImgClear		=	2,	//右眼图像合格
*				LAndRImgClear	=	3	//左眼、右眼图像都合格
*	   		 irisPositionFlag：输出参数，提示远近、左右、上下
*				Far				=	0,
*				Near			=	1,
*				Left			=	2,
*				Right			=	3,
*				Up				=	4,
*				Down			=	5,
*				OK				=	6
*            strFunRts：输出参数，以字符串形式表示本次函数调用结果，由用户分配
*			 内存空间
*
*  返 回 值：0：采集虹膜图像成功
*    		<0：采集虹膜图像失败，根据返回值可以查询失败原因，或者参考strFunRts
*  创 建 人：lizhl
*  创建时间：2013-11-05
*  修 改 人：
*  修改时间：
*****************************************************************************/
int CAPIBase::SyncCapIrisImg(OUT unsigned char *pIrisImg, OUT LRIrisClearFlag &lrIrisClearFlag, OUT IrisPositionFlag &irisPositionFlag, OUT char *strFunRts)
{
    if(!_isWake)
    {
        SetFunRt(strFunRts,E_ALGAPI_SYSTEM_IS_SLEEP,"System is sleep");
        return E_ALGAPI_SYSTEM_IS_SLEEP;
    }

    if(pIrisImg)
    {
        unique_lock<mutex> lck(_xferCurrentImgMutex);
        _pSwapImg = pIrisImg;
        _swapNewImgFlag = true;//通知主线程可以交换图像
        while(_swapNewImgFlag)
        {
//            _xferCurrentImgCV.wait(lck);
            if(_xferCurrentImgCV.wait_for(lck,chrono::seconds(1)) == cv_status::timeout)
            {
                _pSwapImg = NULL;
                _swapNewImgFlag = false;
                SetFunRt(strFunRts,E_ALGAPI_CAP_IMG_FAILED, "Capture image failed");
                return E_ALGAPI_CAP_IMG_FAILED;
            }
        }

        _pSwapImg = NULL;
        lrIrisClearFlag = _lrIrisClearFlag;
        irisPositionFlag = _irisPositionFlag;

        SetFunRt(strFunRts,E_ALGAPI_OK, "Capture image success");
        return E_ALGAPI_OK;
    }
    else
    {
        SetFunRt(strFunRts,E_ALGAPI_CAP_IMG_FAILED, "Capture image failed");
        return E_ALGAPI_CAP_IMG_FAILED;
    }


}

/*****************************************************************************
*                         算法逻辑API采集虹膜图像
*  函 数 名：AsyncCapIrisImg
*  功    能：异步方式采集虹膜图像，采集到的图像可用于实时显示
*  说    明：
*  参    数：appCapIrisImg：输入参数，用户注册的回调函数
*--------------------------------------------------------------------
*以下参数无用
*            pIrisImg：输出参数，采集到的图像，由用户分配内存空间
*			 lrIrisClearFlag：输出参数，表明本次采集到的图像中是否有合格的虹膜图像，该标志可用于指示界面显示红、绿提示框
*				LAndRImgBlur	=	0,	//左眼、右眼图像都不合格；
*				LImgClear		=	1,	//左眼图像合格
*				RImgClear		=	2,	//右眼图像合格
*				LAndRImgClear	=	3	//左眼、右眼图像都合格
*	   		 irisPositionFlag：输出参数，提示远近、左右、上下
*				Far				=	0,
*				Near			=	1,
*				Left			=	2,
*				Right			=	3,
*				Up				=	4,
*				Down			=	5,
*				OK				=	6
*--------------------------------------------------------------------
*            strFunRts：输出参数，以字符串形式表示本次函数调用结果，由用户分配
*			 内存空间
*
*  返 回 值：0：函数调用成功
*    		<0：函数调用失败，根据返回值可以查询失败原因，或者参考strFunRts
*  创 建 人：lizhl
*  创建时间：2013-11-05
*  修 改 人：
*  修改时间：
*****************************************************************************/
int CAPIBase::AsyncCapIrisImg(IN CBAlgApiCapIrisImg appCapIrisImg, OUT char *strFunRts)
{
    if(!_isWake)
    {
        SetFunRt(strFunRts,E_ALGAPI_SYSTEM_IS_SLEEP,"System is sleep");
        return E_ALGAPI_SYSTEM_IS_SLEEP;
    }

    unique_lock <mutex> lck(_asyncCapImgMutex);

//    _cbIrisImg = pIrisImg;
    _cbCapIrisImg = appCapIrisImg;
//    _cbIrisPositionFlag = irisPositionFlag;
//    _cbLRIrisClearFlag = lrIrisClearFlag;

    _asyncCapImgFlag = true;
    _asyncCapImgCV.notify_one();//发送信号给异步采图线程

    SetFunRt(strFunRts,E_ALGAPI_OK, "Async capture image success");
    return E_ALGAPI_OK;
}

/*****************************************************************************
*                         算法逻辑API开始注册
*  函 数 名：SyncStartEnroll
*  功    能：同步方式虹膜注册
*  说    明：
*  参    数：enrIrisL，enrIrisR：输出参数，注册得到的左眼、右眼虹膜图像及相关信息
*			 numEnrL，numEnrR：输入参数，注册得到的左眼、右眼虹膜图像个数，由调用者指定
*			 lrFlag：输出参数，左眼、右眼虹膜注册成功标志
*				EnrRecBothFailed	=	-3,		//EnrRecBothFailed：注册、识别时左右眼均注册或识别失败
*				EnrRecRightFailed	=	-2,		//EnrRecRightFailed:右眼注册、识别失败导致注册或识别失败,左眼情况未知
*				EnrRecLeftFailed	=	-1,		//EnrRecLeftFailed:左眼注册、识别失败导致注册或识别失败,右眼情况未知
*				EnrRecLeftSuccess	=	1,		//EnrRecLeftSuccess:注册、识别时仅左眼注册或识别成功
*				EnrRecRightSuccess	=	2,		//EnrRecRightSuccess:注册、识别时仅右眼注册或识别成功
*				EnrRecBothSuccess	=	3		//EnrRecBothSuccess:注册、识别时左右眼均注册成功
*			 strFunRts：输出参数，以字符串形式表示本次函数调用结果，由用户分配
*			 内存空间内存空间
*
*  返 回 值：0：注册成功
*			 1：人为停止注册
*    		<0：注册失败，根据返回值可以查询失败原因，或者参考strFunRts
*  创 建 人：lizhl
*  创建时间：2013-11-05
*  修 改 人：
*  修改时间：
*****************************************************************************/
int CAPIBase::SyncStartEnroll(OUT APIIrisInfo *enrIrisL, IN int numEnrL, OUT APIIrisInfo *enrIrisR, IN int numEnrR, OUT LRSucFailFlag &lrFlag, OUT char *strFunRts)
{
    int maxEnrollTime = _algApiCtrlParam.MaxENRTime;//超过该时间如果还没有注册成功，则认为注册失败
    bool leftEyeNeed = false;
    bool rightEyeNeed = false;
    bool leftEyeActive = false;
    bool rightEyeActive = false;


    {
        //检查函数调用合法性并设置标志位
        if(!_isWake)
        {
            SetFunRt(strFunRts,E_ALGAPI_SYSTEM_IS_SLEEP,"System is sleep");
            return E_ALGAPI_SYSTEM_IS_SLEEP;
        }

        lock_guard<mutex> lck(_setEnrRecMutex);

        if(_keepIdenFlag || _keepEnrFlag)
        {
            SetFunRt(strFunRts,E_ALGAPI_ALGORITHM_CONFLICT, "Start enroll failed, iden is running");
            return E_ALGAPI_ALGORITHM_CONFLICT;
        }

        if( ((numEnrL < 2) || (numEnrL > g_constMaxEnrollImNum)) || ((numEnrR < 2) || (numEnrR > g_constMaxEnrollImNum)) )
        {
            SetFunRt(strFunRts,E_ALGAPI_INVALID_ENR_INPUT,"Invalid enroll number");
            return E_ALGAPI_INVALID_ENR_INPUT;
        }

        if(maxEnrollTime < g_constMinENRTime)
        {
            SetFunRt(strFunRts,E_ALGAPI_INVALID_TIME,"Invalid maxenrtime! Set algorithm's control parameter failed");
            return E_ALGAPI_INVALID_TIME;
        }

        switch(_algApiCtrlParam.EyeMode)
        {
        case BothEye:
            leftEyeNeed = true;
            rightEyeNeed = true;
            break;

        case LeftEye:
            leftEyeNeed = true;
            rightEyeNeed = false;
            break;

        case RightEye:
            leftEyeNeed = false;
            rightEyeNeed = true;
            break;

        case AnyEye:
            leftEyeNeed = true;
            rightEyeNeed = true;
            break;

        default:
            SetFunRt(strFunRts,E_ALGAPI_INVALID_EYEMODE,"Invalid eyemode! Set algorithm's control parameter failed");
            return E_ALGAPI_INVALID_EYEMODE;
        }

        if( ((true == leftEyeNeed) && (NULL == enrIrisL)) || ((true == rightEyeNeed) && (NULL == enrIrisR))  )
        {
            SetFunRt(strFunRts,E_ALGAPI_INVALID_ENR_INPUT,"Invalid enroll iris list");
            return E_ALGAPI_INVALID_ENR_INPUT;
        }

        _keepEnrFlag = true;
        _outEnrFlag = false;
    }

    //开始注册
    _qualityMode = IR_WORK_MODE_ENR;

    int leftExistedNum = 0;
    int rightExistedNum = 0;

    int funResult = E_ALGAPI_DEFAUL_ERROR;
    lrFlag = EnrRecUnknown;
    int enrResult = 0;

    //和超时判断相关变量
    int durationTime = 0;
    struct timeval tvEnrStart, tvEnrEnd;
    bool flagOverTime = false;//超时的标志
    bool flagOverMinTime = false;//超过最小注册时间的标志，超过最小注册时间才能有注册结果

    //初始化输出结果
    for (int count = 0; count < numEnrL;count++)
    {
        IKInitAPIIrisInfo(enrIrisL+count);
    }

    for (int count = 0; count < numEnrR; count++)
    {
        IKInitAPIIrisInfo(enrIrisR+count);
    }

    for(int count = 0; count <= g_constMaxEnrollImNum; count++)
    {
        CASIAGenInitIrisInfo(&_leftEnrollImg[count]);
        CASIAGenInitIrisInfo(&_rightEnrollImg[count]);
    }

    gettimeofday(&tvEnrStart,NULL);
    while(_keepEnrFlag)
    {
        gettimeofday(&tvEnrEnd,NULL);

        durationTime = tvEnrEnd.tv_sec - tvEnrStart.tv_sec;
        if(durationTime > g_constMinENRTime)
        {
            flagOverMinTime = true;

            if(durationTime > maxEnrollTime)
            {
                flagOverTime = true;
            }
        }

        {
            //交换采集到的好图像并注册特征
            unique_lock<mutex> lck(_xferBestImgMutex);
            _swapNewGoodImgFlag = true;//通知主线程可以交换图像
            while(_swapNewGoodImgFlag)
            {
//                _xferBestImgCV.wait(lck);
                if(_xferBestImgCV.wait_for(lck,chrono::seconds(1)) == cv_status::timeout)
                {
                    break;
                }

            }

            if( _getLeftNewGoodImgFlag && leftEyeNeed )
            {
                leftEyeActive = true;
                _getLeftNewGoodImgFlag = false;
            }

            if( _getRightNewGoodImgFlag && rightEyeNeed )
            {
                rightEyeActive = true;
                _getRightNewGoodImgFlag = false;
            }

            //分别对左右眼进行注册
            if(leftEyeActive)
            {
                funResult = IKEnrollLRFeature(_algHandle, _algApiCtrlParam.IfSpoofDetect, _leftEnrollImg, &leftExistedNum,
                                              &_leftEnrollImg[g_constIREnrollTimeNum-1], _leftEnrMatchImgInfo, &_curBestEnrMatchImgInfo, strFunRts);
                leftEyeActive = false;//为下一轮赋初值
            }

            if(rightEyeActive)
            {
                funResult = IKEnrollLRFeature(_algHandle, _algApiCtrlParam.IfSpoofDetect, _rightEnrollImg, &rightExistedNum,
                                              &_rightEnrollImg[g_constIREnrollTimeNum-1], _rightEnrMatchImgInfo, &_curBestEnrMatchImgInfo, strFunRts);
                rightEyeActive = false;//为下一轮赋初值
            }

        }

        //判断是否注册成功
        if( (leftExistedNum >= g_constMaxEnrollImNum) && flagOverMinTime && leftEyeNeed)
        {
            for(int count = 0; count < numEnrL; count++)
            {
                IKCopyIRIrisInfo2APIIrisInfo(enrIrisL+count, &_leftEnrollImg[count]);
            }

            enrResult += EnrRecLeftSuccess;
            leftEyeNeed = false;
        }

        if( (rightExistedNum >= g_constMaxEnrollImNum) && flagOverMinTime && rightEyeNeed)
        {

            for(int count = 0; count < numEnrR; count++)
            {
                IKCopyIRIrisInfo2APIIrisInfo(enrIrisR+count, &_rightEnrollImg[count]);
            }

            enrResult += EnrRecRightSuccess;
            rightEyeNeed = false;
        }

        //判断输出结果
        if(AnyEye == _algApiCtrlParam.EyeMode)
        {
            if(!leftEyeNeed || !rightEyeNeed)
            {
                rightEyeNeed = false;
                leftEyeNeed = false;
            }
        }

        if( (false == leftEyeNeed) && (false == rightEyeNeed) )
        {
            //success
            lock_guard<mutex> lck(_setEnrRecMutex);
            lrFlag = (LRSucFailFlag)enrResult;

            _keepEnrFlag = false;
            _outEnrFlag = true;
            SetFunRt(strFunRts,E_ALGAPI_OK, "Enroll success");
            return E_ALGAPI_OK;
        }
        else if(flagOverTime)
        {
            lock_guard<mutex> lck(_setEnrRecMutex);
            switch(_algApiCtrlParam.EyeMode)
            {
            case BothEye:
                enrResult -= EnrRecBothSuccess;
                break;

            case LeftEye:
                enrResult -= EnrRecLeftSuccess;
                break;

            case RightEye:
                enrResult -= EnrRecRightSuccess;
                break;

            case AnyEye:
                enrResult -= EnrRecBothSuccess;
                break;

            default:
                lrFlag = (LRSucFailFlag)enrResult;
                _keepEnrFlag = false;
                _outEnrFlag = true;
                SetFunRt(strFunRts,E_ALGAPI_INVALID_EYEMODE, "Invalid eyemode");
                return E_ALGAPI_INVALID_EYEMODE;
            }

            lrFlag = (LRSucFailFlag)enrResult;
            _keepEnrFlag = false;
            _outEnrFlag = true;
            SetFunRt(strFunRts,E_ALGAPI_ENR_IDEN_FAILED, "Enroll failed");
            return E_ALGAPI_ENR_IDEN_FAILED;
        }

    }//end while

    lock_guard<mutex> lck(_setEnrRecMutex);
    lrFlag = (LRSucFailFlag)enrResult;
    _keepEnrFlag = false;
    _outEnrFlag = true;
    SetFunRt(strFunRts,E_ALGAPI_ENR_IDEN_INTERRUPT, "Enroll process is stopped");
    return E_ALGAPI_ENR_IDEN_INTERRUPT;
}

/*****************************************************************************
*                         算法逻辑API开始注册
*  函 数 名：AsyncStartEnroll
*  功    能：异步方式虹膜注册
*  说    明：
*  参    数：appEnroll：输入参数，用户注册的注册函数
*			 numEnrL，numEnrR：输入参数，注册得到的左眼、右眼虹膜图像个数，由调用者指定
*-----------------------------------------------------------------------------------------
*以下参数无用
*            enrIrisL，enrIrisR：输出参数，注册得到的左眼、右眼虹膜图像及相关信息
*			 lrFlag：输出参数，左眼、右眼虹膜注册成功标志
*				EnrRecBothFailed	=	-3,		//EnrRecBothFailed：注册、识别时左右眼均注册或识别失败
*				EnrRecRightFailed	=	-2,		//EnrRecRightFailed:右眼注册、识别失败导致注册或识别失败,左眼情况未知
*				EnrRecLeftFailed	=	-1,		//EnrRecLeftFailed:左眼注册、识别失败导致注册或识别失败,右眼情况未知
*				EnrRecLeftSuccess	=	1,		//EnrRecLeftSuccess:注册、识别时仅左眼注册或识别成功
*				EnrRecRightSuccess	=	2,		//EnrRecRightSuccess:注册、识别时仅右眼注册或识别成功
*				EnrRecBothSuccess	=	3		//EnrRecBothSuccess:注册、识别时左右眼均注册成功
*-------------------------------------------------------------------------------------------
*			 strFunRts：输出参数，以字符串形式表示本次函数调用结果，由用户分配
*			 内存空间内存空间
*
*  返 回 值：0：函数调用成功
*    		<0：函数调用失败，根据返回值可以查询失败原因，或者参考strFunRts
*  创 建 人：lizhl
*  创建时间：2013-11-05
*  修 改 人：
*  修改时间：
*****************************************************************************/
int CAPIBase::AsyncStartEnroll(IN CBAlgApiEnroll appEnroll, IN int numEnrL, IN int numEnrR, OUT char *strFunRts)
{
    if(!_isWake)
    {
        SetFunRt(strFunRts,E_ALGAPI_SYSTEM_IS_SLEEP,"System is sleep");
        return E_ALGAPI_SYSTEM_IS_SLEEP;
    }

    _cbEnroll = appEnroll;
//    _cbEnrIrisL = enrIrisL;
    _cbNumEnrL = numEnrL;
//    _cbEnrIrisR = enrIrisR;
    _cbNumEnrR = numEnrR;
//    _cbEnrLRFlag = lrFlag;

    unique_lock <mutex> lck(_asyncEnrMutex);
    _asyncEnrFlag = true;
    _asyncEnrCV.notify_one();//发送信号给异步enroll线程
    SetFunRt(strFunRts,E_ALGAPI_OK, "Async start enroll success");
    return E_ALGAPI_OK;
}

/*****************************************************************************
*                         算法逻辑API停止注册
*  函 数 名：AlgApiStopEnroll
*  功    能：停止虹膜注册
*  说    明：
*  参    数：strFunRts：输出参数，以字符串形式表示本次函数调用结果，由用户分配
*			 内存空间
*
*  返 回 值：0：停止注册成功
*    		<0：停止注册失败，根据返回值可以查询失败原因，或者参考strFunRts
*  创 建 人：lizhl
*  创建时间：2013-11-05
*  修 改 人：
*  修改时间：
*****************************************************************************/
int CAPIBase::StopEnroll(OUT char *strFunRts)
{
    if(!_isWake)
    {
        SetFunRt(strFunRts,E_ALGAPI_SYSTEM_IS_SLEEP,"System is sleep");
        return E_ALGAPI_SYSTEM_IS_SLEEP;
    }

    lock_guard<mutex> lck(_setEnrRecMutex);
    if(!_keepEnrFlag)
    {
        SetFunRt(strFunRts,E_ALGAPI_FAIL2STOP, "Stop enroll failed, enroll is not running");
        return E_ALGAPI_FAIL2STOP;
    }
    _keepEnrFlag = false;

    //等待enroll函数结束
    while(!_outEnrFlag)
    {
        usleep(0);
    }

    if(_cbEnroll)
    {
        InitEnrCBSrc();
    }

    SetFunRt(strFunRts,E_ALGAPI_OK, "Stop enroll success");
    return E_ALGAPI_OK;
}

/*****************************************************************************
*                         算法逻辑API开始识别
*  函 数 名：SyncStartIden
*  功    能：同步方式虹膜识别
*  说    明：
*  参    数：codeListL，codeListR：输入参数，左眼、右眼虹膜注册特征
*			 codeNumL，codeNumR：输入参数，左眼、右眼虹膜注册特征个数
*			 recIrisL，recIrisR：输出参数，本次识别得到的左眼、右眼虹膜图像及相关信息
*			 lrFlag：输出参数，左眼、右眼虹膜识别成功标志
*				EnrRecBothFailed	=	-3,		//EnrRecBothFailed：注册、识别时左右眼均注册或识别失败
*				EnrRecRightFailed	=	-2,		//EnrRecRightFailed:右眼注册、识别失败导致注册或识别失败,左眼情况未知
*				EnrRecLeftFailed	=	-1,		//EnrRecLeftFailed:左眼注册、识别失败导致注册或识别失败,右眼情况未知
*				EnrRecLeftSuccess	=	1,		//EnrRecLeftSuccess:注册、识别时仅左眼注册或识别成功
*				EnrRecRightSuccess	=	2,		//EnrRecRightSuccess:注册、识别时仅右眼注册或识别成功
*				EnrRecBothSuccess	=	3		//EnrRecBothSuccess:注册、识别时左右眼均注册成功
*			 pLeftMatchOutputParam，pRightMatchOutputParam：输出参数，左眼、右眼识别输出信息
*            strFunRts：输出参数，以字符串形式表示本次函数调用结果，由用户分配
*			 内存空间
*
*  返 回 值：0：识别成功
*			 1：人为停止识别
*    		<0：识别失败，根据返回值可以查询失败原因，或者参考strFunRts
*  创 建 人：lizhl
*  创建时间：2013-11-05
*  修 改 人：
*  修改时间：
*****************************************************************************/
int CAPIBase::SyncStartIden(IN unsigned char *codeListL, IN int codeNumL, IN unsigned char *codeListR, IN int codeNumR,
                        OUT APIIrisInfo *recIrisL, OUT APIIrisInfo *recIrisR, OUT LRSucFailFlag &lrFlag,
                        OUT APIMatchOutput *pLeftMatchOutputParam, OUT  APIMatchOutput *pRightMatchOutputParam, OUT char *strFunRts)
{
    int maxRecTime = _algApiCtrlParam.MaxRECTime;//超过该时间如果还没有注册成功，则认为注册失败
    bool leftEyeNeed = false;
    bool rightEyeNeed = false;
    bool leftEyeActive = false;
    bool rightEyeActive = false;

    {
        //检查函数调用合法性并设置标志位
        if(!_isWake)
        {
            SetFunRt(strFunRts,E_ALGAPI_SYSTEM_IS_SLEEP,"System is sleep");
            return E_ALGAPI_SYSTEM_IS_SLEEP;
        }

        lock_guard<mutex> lck(_setEnrRecMutex);

        if(_keepIdenFlag || _keepEnrFlag)
        {
            SetFunRt(strFunRts,E_ALGAPI_ALGORITHM_CONFLICT, "Start iden failed, enroll is running");
            return E_ALGAPI_ALGORITHM_CONFLICT;
        }

        if(maxRecTime > g_constMaxRECTime)
        {
            SetFunRt(strFunRts,E_ALGAPI_INVALID_TIME,"Invalid maxenrtime! Set algorithm's control parameter failed");
            return E_ALGAPI_INVALID_TIME;
        }

        switch(_algApiCtrlParam.EyeMode)
        {
        case BothEye:
            leftEyeNeed = true;
            rightEyeNeed = true;
            break;

        case LeftEye:
            leftEyeNeed = true;
            rightEyeNeed = false;
            break;

        case RightEye:
            leftEyeNeed = false;
            rightEyeNeed = true;
            break;

        case AnyEye:
            leftEyeNeed = true;
            rightEyeNeed = true;
            break;

        default:
            SetFunRt(strFunRts,E_ALGAPI_INVALID_EYEMODE,"Invalid eyemode! Set algorithm's control parameter failed");
            return E_ALGAPI_INVALID_EYEMODE;
        }

        if( ((true == leftEyeNeed) && (NULL == codeListL)) || ((true == rightEyeNeed) && (NULL == codeListR))  )
        {
            SetFunRt(strFunRts,E_ALGAPI_INVALID_REC_INPUT,"Invalid iden code list");
            return E_ALGAPI_INVALID_REC_INPUT;
        }

        _keepIdenFlag = true;
        _outIdenFlag = false;
    }

    //开始识别
    lrFlag = EnrRecUnknown;
    int recResult = 0;
    _qualityMode = IR_WORK_MODE_REC;
    int idenFailedCount = 0;//识别失败计数
    int funRetL = E_ALGAPI_DEFAUL_ERROR;
    int funRetR = E_ALGAPI_DEFAUL_ERROR;

    MatchOutput outputMtPara;
    MatchInput inputPara;

    inputPara.angRotate[0]=IR_MAX_NEG_ROTATION_ANGLE;	//默认：left rotate
    inputPara.angRotate[1]=IR_MAX_POS_ROTATION_ANGLE;	//默认：right rotate
    inputPara.matchStrategy =IR_CASCADE_CLASSIFIER;		//默认：Not defined
    //20130704比对阈值不对用户开放
    inputPara.matchTh = IR_USE_DEAULT_MATCHTH;

    //通过APP设置的参数（用户设置的部分）
    //inputPara.matchTh = pIKIRInfo->irPara.matchTh;		//APP 设置
    inputPara.findMode = _algApiCtrlParam.FindMode;	//APP 设置

    //用于识别的算法结构体
    pIrisImageInfo pImgInfoL = NULL;
    pIrisImageInfo pImgInfoR = NULL;

    //和超时判断相关变量
    int durationTime = 0;
    struct timeval tvRecStart, tvRecEnd;
    bool flagOverTime = false;//超时的标志

    //初始化输出结果,设置默认值
    IKInitAPIMatchOutput(pLeftMatchOutputParam);
    IKInitAPIMatchOutput(pRightMatchOutputParam);

    gettimeofday(&tvRecStart,NULL);
    while(_keepIdenFlag)
    {
        gettimeofday(&tvRecEnd,NULL);

        //为本次识别赋初值
        leftEyeActive = false;
        rightEyeActive = false;

        durationTime = tvRecEnd.tv_sec - tvRecStart.tv_sec;
        if(durationTime > maxRecTime)
        {
            flagOverTime = true;
            lock_guard<mutex> lck(_setEnrRecMutex);
            lrFlag = (LRSucFailFlag)recResult;

            _keepIdenFlag = false;
            _outIdenFlag = true;
            SetFunRt(strFunRts,E_ALGAPI_IDEN_OVERTIME, "Iden overtime");
            return E_ALGAPI_IDEN_OVERTIME;
        }

        //交换采集到的好图像并识别
        {
            unique_lock<mutex> lck(_xferBestImgMutex);
            _swapNewGoodImgFlag = true;//通知主线程可以交换图像
            while(_swapNewGoodImgFlag)
            {
//                _xferBestImgCV.wait(lck);
                if(_xferBestImgCV.wait_for(lck,chrono::seconds(1)) == cv_status::timeout)
                {
                    break;
                }
            }

            if( _getLeftNewGoodImgFlag && leftEyeNeed )
            {
                leftEyeActive = true;
                _getLeftNewGoodImgFlag = false;
            }

            if( _getRightNewGoodImgFlag && rightEyeNeed )
            {
                rightEyeActive = true;
                _getRightNewGoodImgFlag = false;
            }

            //开始特征提取与比对
            funRetL = E_ALGAPI_DEFAUL_ERROR;
            funRetR = E_ALGAPI_DEFAUL_ERROR;

            if(leftEyeActive)
            {
                pImgInfoL = &(_leftEnrollImg[g_constIREnrollTimeNum-1]);
                //识别特征
                funRetL = IKIdenFeature(_algHandle, codeListL, codeNumL, pImgInfoL, inputPara, &outputMtPara, strFunRts);
                IKCopyIRIrisInfo2APIIrisInfo(recIrisL, pImgInfoL);

                if( (IR_FUN_SUCCESS == funRetL) && leftEyeNeed)
                {
                    IKInsertIRMtOutput2APIMtOutput(pLeftMatchOutputParam, &outputMtPara, g_constMtIsLeftEye);
                    leftEyeNeed = false;
                    recResult += EnrRecLeftSuccess;
                }

            }

            if(rightEyeActive)
            {
                pImgInfoR = &(_rightEnrollImg[g_constIREnrollTimeNum-1]);
                //识别特征
                funRetR = IKIdenFeature(_algHandle, codeListR, codeNumR, pImgInfoR, inputPara, &outputMtPara, strFunRts);
                IKCopyIRIrisInfo2APIIrisInfo(recIrisR, pImgInfoR);
                if( (IR_FUN_SUCCESS == funRetR) && rightEyeNeed)
                {
                    IKInsertIRMtOutput2APIMtOutput(pRightMatchOutputParam, &outputMtPara, g_constMtIsRightEye);
                    rightEyeNeed = false;
                    recResult += EnrRecRightSuccess;
                }
            }
        }

        //判断输出结果
        if(AnyEye == _algApiCtrlParam.EyeMode)
        {
            if(!leftEyeNeed || !rightEyeNeed)
            {
                rightEyeNeed = false;
                leftEyeNeed = false;
            }
        }

        if( (false == leftEyeNeed) && (false == rightEyeNeed) )
        {
            //success
            lock_guard<mutex> lck(_setEnrRecMutex);
            lrFlag = (LRSucFailFlag)recResult;

            _keepIdenFlag = false;
            _outIdenFlag = true;
            SetFunRt(strFunRts,E_ALGAPI_OK, "Iden success");
            return E_ALGAPI_OK;
        }
        else
        {
            //识别失败计数加1
            idenFailedCount++;
            //如果是双眼识别模式，只要有一只眼睛识别通过，则失败计数不增加
            if(BothEye == _algApiCtrlParam.EyeMode)
            {
                if( (false == leftEyeNeed) || (false == rightEyeNeed) )
                {
                    idenFailedCount--;
                }
            }

            if(idenFailedCount >= g_constIdenFailedTolerantNum)
            {
                lock_guard<mutex> lck(_setEnrRecMutex);
                switch(_algApiCtrlParam.EyeMode)
                {
                case BothEye:
                    recResult -= EnrRecBothSuccess;
                    break;

                case LeftEye:
                    recResult -= EnrRecLeftSuccess;
                    break;

                case RightEye:
                    recResult -= EnrRecRightSuccess;
                    break;

                case AnyEye:
                    recResult -= EnrRecBothSuccess;
                    break;

                default:
                    lrFlag = (LRSucFailFlag)recResult;
                    _keepIdenFlag = false;
                    _outIdenFlag = true;
                    SetFunRt(strFunRts,E_ALGAPI_INVALID_EYEMODE, "Invalid eyemode");
                    return E_ALGAPI_INVALID_EYEMODE;
                }

                lrFlag = (LRSucFailFlag)recResult;
                _keepIdenFlag = false;
                _outIdenFlag = true;
                SetFunRt(strFunRts,E_ALGAPI_ENR_IDEN_FAILED, "Iden failed");
                return E_ALGAPI_ENR_IDEN_FAILED;
            }
        }

    }

    lock_guard<mutex> lck(_setEnrRecMutex);
    lrFlag = (LRSucFailFlag)recResult;
    _keepIdenFlag = false;
    _outIdenFlag = true;
    SetFunRt(strFunRts,E_ALGAPI_ENR_IDEN_INTERRUPT, "Iden process is stopped");
    return E_ALGAPI_ENR_IDEN_INTERRUPT;
}

/*****************************************************************************
*                         算法逻辑API开始识别
*  函 数 名：AsyncStartIden
*  功    能：异步方式虹膜识别
*  说    明：
*  参    数：appIden：输入参数，用户注册的识别回调函数
*            codeListL，codeListR：输入参数，左眼、右眼虹膜注册特征
*			 codeNumL，codeNumR：输入参数，左眼、右眼虹膜注册特征个数
*----------------------------------------------------------------------------------------------
*以下参数无用
*			 recIrisL，recIrisR：输出参数，本次识别得到的左眼、右眼虹膜图像及相关信息
*			 lrFlag：输出参数，左眼、右眼虹膜识别成功标志
*				EnrRecBothFailed	=	-3,		//EnrRecBothFailed：注册、识别时左右眼均注册或识别失败
*				EnrRecRightFailed	=	-2,		//EnrRecRightFailed:右眼注册、识别失败导致注册或识别失败,左眼情况未知
*				EnrRecLeftFailed	=	-1,		//EnrRecLeftFailed:左眼注册、识别失败导致注册或识别失败,右眼情况未知
*				EnrRecLeftSuccess	=	1,		//EnrRecLeftSuccess:注册、识别时仅左眼注册或识别成功
*				EnrRecRightSuccess	=	2,		//EnrRecRightSuccess:注册、识别时仅右眼注册或识别成功
*				EnrRecBothSuccess	=	3		//EnrRecBothSuccess:注册、识别时左右眼均注册成功
*			 pLeftMatchOutputParam，pRightMatchOutputParam：输出参数，左眼、右眼识别输出信息
*-----------------------------------------------------------------------------------------------
*            strFunRts：输出参数，以字符串形式表示本次函数调用结果，由用户分配
*			 内存空间
*
*  返 回 值：0：函数调用成功
*    		<0：函数调用失败，根据返回值可以查询失败原因，或者参考strFunRts
*  创 建 人：lizhl
*  创建时间：2013-11-05
*  修 改 人：
*  修改时间：
*****************************************************************************/
int CAPIBase::AsyncStartIden(IN CBAlgApiIden appIden, IN unsigned char *codeListL, IN int codeNumL, IN unsigned char *codeListR, IN int codeNumR, OUT char *strFunRts)
{
    if(!_isWake)
    {
        SetFunRt(strFunRts,E_ALGAPI_SYSTEM_IS_SLEEP,"System is sleep");
        return E_ALGAPI_SYSTEM_IS_SLEEP;
    }

    _cbIden = appIden;
    _cbCodeListL = codeListL;
    _cbCodeNumL = codeNumL;
    _cbCodeListR = codeListR;
    _cbCodeNumR = codeNumR;

//    _cbRecIrisL = recIrisL;
//    _cbRecIrisR = recIrisR;
//    _cbIdenLRFlag = lrFlag;
//    _cbLeftMatchOutputParam = pLeftMatchOutputParam;
//    _cbRightMatchOutputParam = pRightMatchOutputParam;

    unique_lock <mutex> lck(_asyncIdenMutex);
    _asyncIdenFlag = true;
    _asyncIdenCV.notify_one();//发送信号给异步iden线程
    SetFunRt(strFunRts,E_ALGAPI_OK, "Async start iden success");
    return E_ALGAPI_OK;
}

/*****************************************************************************
*                         算法逻辑API停止识别
*  函 数 名：StopIden
*  功    能：停止虹膜识别
*  说    明：
*  参    数：strFunRts：输出参数，以字符串形式表示本次函数调用结果，由用户分配
*			 内存空间
*
*  返 回 值：0：停止识别成功
*    		<0：停止识别失败，根据返回值可以查询失败原因，或者参考strFunRts
*  创 建 人：lizhl
*  创建时间：2013-11-05
*  修 改 人：
*  修改时间：
*****************************************************************************/
int CAPIBase::StopIden(OUT char *strFunRts)
{
    if(!_isWake)
    {
        SetFunRt(strFunRts,E_ALGAPI_SYSTEM_IS_SLEEP,"System is sleep");
        return E_ALGAPI_SYSTEM_IS_SLEEP;
    }

    lock_guard<mutex> lck(_setEnrRecMutex);
    if(!_keepIdenFlag)
    {
        SetFunRt(strFunRts,E_ALGAPI_FAIL2STOP, "Stop iden failed, iden is not running");
        return E_ALGAPI_FAIL2STOP;
    }
    _keepIdenFlag = false;

    //等待iden函数结束
    while(!_outIdenFlag)
    {
        usleep(0);
    }

    if(_cbIden)
    {
        InitIdenCBSrc();
    }

    SetFunRt(strFunRts,E_ALGAPI_OK, "Stop iden success");
    return E_ALGAPI_OK;
}

/*****************************************************************************
*                         算法逻辑API虹膜比对
*  函 数 名：Match
*  功    能：虹膜识别特征模板比对
*  说    明：在StartEnroll成功后调用本函数，将注册得到的识别特征模板与
*			 已注册特征模板进行比对，验证被注册者是否重复注册虹膜
*  参    数：recCode，recNum：输入参数，注册得到的用户的识别特征模板及个数
*			 codeList，codeNum：输入参数，虹膜注册特征及个数
*			 pMatchOutputParam：输出参数，比对输出信息
*            strFunRts：输出参数，以字符串形式表示本次函数调用结果，由用户分配
*			 内存空间
*
*  返 回 值：0：比对成功
*    		<0：比对失败，根据返回值可以查询失败原因，或者参考strFunRts
*  创 建 人：lizhl
*  创建时间：2013-11-05
*  修 改 人：
*  修改时间：
*****************************************************************************/
int CAPIBase::Match(IN unsigned char *recCode, IN int recNum, IN unsigned char *codeList, IN int codeNum, OUT  APIMatchOutput *pMatchOutputParam, OUT char *strFunRts)
{
    if(!_isWake)
    {
        SetFunRt(strFunRts,E_ALGAPI_SYSTEM_IS_SLEEP,"System is sleep");
        return E_ALGAPI_SYSTEM_IS_SLEEP;
    }

    if( (recNum <= 0) || (codeNum <= 0) || (NULL == recCode) || (NULL == codeList) )
    {
        SetFunRt(strFunRts,E_ALGAPI_INVALID_MATCH_INPUT, "Invalid match input");
        return E_ALGAPI_INVALID_MATCH_INPUT;
    }

    for(int count = 0; count < recNum; count++)
    {
        //设置比对参数
        MatchInput inputPara;
        inputPara.angRotate[0]=IR_MAX_NEG_ROTATION_ANGLE;	//默认：left rotate
        inputPara.angRotate[1]=IR_MAX_POS_ROTATION_ANGLE;	//默认：right rotate
        inputPara.matchStrategy =IR_CASCADE_CLASSIFIER;		//默认：Not defined

        //20130704比对阈值不对用户开放
        inputPara.matchTh = IR_USE_DEAULT_MATCHTH;
        inputPara.findMode = _algApiCtrlParam.FindMode;	//APP 设置

        MatchOutput outputMtPara;
        int funResult = IR_DEFAULT_ERR;
        funResult = IKIR_Match(_algHandle, recCode + count*IR_REC_FEATURE_LENGTH, codeList, codeNum, inputPara, &outputMtPara, strFunRts);

        if(IR_FUN_SUCCESS == funResult)
        {
            IKInsertIRMtOutput2APIMtOutput(pMatchOutputParam, &outputMtPara, g_constMtUnknowEye);
            SetFunRt(strFunRts,E_ALGAPI_OK, "Match success");
            return E_ALGAPI_OK;
        }
    }

    SetFunRt(strFunRts,E_ALGAPI_MATCH_FAILED, "Match failed");
    return E_ALGAPI_MATCH_FAILED;
}

/*****************************************************************************
*                         初始化异步采图函数相关资源
*  函 数 名：InitCapCBSrc
*  功    能：初始化异步采图函数相关资源
*  说    明：
*  参    数：NULL
*
*  返 回 值：NULL
*  创 建 人：lizhl
*  创建时间：2013-12-04
*  修 改 人：
*  修改时间：
*****************************************************************************/
void  CAPIBase::InitCapCBSrc(void)
{
    _cbCapIrisImg = NULL;
//    _cbIrisImg = NULL;
//    _myIrisImg = NULL;
//    _cbLRIrisClearFlag = NULL;
//    _cbIrisPositionFlag = NULL;
}

/*****************************************************************************
*                         初始化异步注册函数相关资源
*  函 数 名：InitEnrCBSrc
*  功    能：初始化异步注册函数相关资源
*  说    明：
*  参    数：NULL
*
*  返 回 值：NULL
*  创 建 人：lizhl
*  创建时间：2013-12-04
*  修 改 人：
*  修改时间：
*****************************************************************************/
void  CAPIBase::InitEnrCBSrc(void)
{
    _cbEnroll = NULL;
//    _cbEnrIrisL = NULL;
    _cbNumEnrL = 0;
//    _cbEnrIrisR = NULL;
    _cbNumEnrR = 0;
//    _cbEnrLRFlag = NULL;
}

/*****************************************************************************
*                         初始化异步识别函数相关资源
*  函 数 名：InitIdenCBSrc
*  功    能：初始化异步识别函数相关资源
*  说    明：
*  参    数：NULL
*
*  返 回 值：NULL
*  创 建 人：lizhl
*  创建时间：2013-12-04
*  修 改 人：
*  修改时间：
*****************************************************************************/
void  CAPIBase::InitIdenCBSrc(void)
{
    _cbIden = NULL;
    _cbCodeListL = NULL;
    _cbCodeNumL = 0;
    _cbCodeListR = NULL;
    _cbCodeNumR = 0;
//    _cbIdenLRFlag = NULL;

//    _cbRecIrisL = NULL;
//    _cbRecIrisR = NULL;
//    _cbLeftMatchOutputParam = NULL;
//    _cbRightMatchOutputParam = NULL;
}

/*****************************************************************************
*                         主线程
*  函 数 名：ThreadMain
*  功    能：从USB设备采图
*  说    明：
*  参    数：pParam：输入参数，CAPIBase指针
*
*  返 回 值：NULL
*  创 建 人：lizhl
*  创建时间：2013-11-19
*  修 改 人：
*  修改时间：
*****************************************************************************/
void* CAPIBase::ThreadMain(IN CAPIBase* pParam)
{
    char strFunRts[g_constLengthOfStrFunRts];
    int funResult = E_ALGAPI_DEFAUL_ERROR;
    pParam->_lrIrisClearFlag = LAndRImgBlur;
    pParam->_irisPositionFlag = Unknown;

    int leftQualifiedImgNum = 0;
    int rightQualifiedImgNum = 0;
    int leftQualifiedImgScore = 0;
    int rightQualifiedImgScore = 0;
    int savedScoreL = 0;
    int savedScoreR = 0;

    int imgInformCount  = 0;//通知计数，用于通知APP的采图函数
    int imgSelectCount  = 0;//图像选择计数，用于注册或识别时挑选图像计数

    unsigned int getImgLen = 0;

    CAPIGetFlag getFlag;

    while(pParam->_mainThreadFlag)
    {
        if(pParam->_isWake)
        {            
            //采图，放到_pCapImg中            

 #ifdef ALGAPITEST
            //++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
            //单元测试时没有相关的采图函数和虹膜检测算法，因此对源代码进行了修改，改为从文件读图
//            CASIAGenInitIrisInfo(&pParam->_leftCurImg);
//            CASIAGenInitIrisInfo(&pParam->_rightCurImg);
            funResult = TestAlgApiReadImg(pParam->_pCapImg, &pParam->_leftCurImg, &pParam->_rightCurImg);
            if(E_ALGAPI_OK != funResult)
            {
                continue;
            }
            //不进行虹膜检测，直接赋值
            funResult = E_ALGAPI_OK;
            pParam->_eyeDetectRet.lImFlag = IR_IM_EYE_LEFF;
            pParam->_eyeDetectRet.rImFlag = IR_IM_EYE_RIGHT;

            //++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
#else
            //TO DO
            funResult = pParam->_syncUsbApi->GetImage(pParam->_pCapImg, getImgLen);
            if(E_ALGAPI_OK != funResult)
            {
                continue;
            }

            /////////////////////////

            //单元测试
            //从_pSwapImg中检测虹膜
            funResult = IKIR_IrisDetection(pParam->_algHandle, pParam->_pCapImg, g_constIrisImgHeight, g_constIrisImgWidth,
                                           &pParam->_leftCurImg, &pParam->_rightCurImg, &pParam->_eyeDetectRet, strFunRts);

#endif


            if(E_ALGAPI_OK == funResult)
            {
                //判断左眼图像质量
                if( (IR_IM_EYE_LEFF == pParam->_eyeDetectRet.lImFlag) && (RightEye != pParam->_algApiCtrlParam.EyeMode) )
                {
                    pParam->_leftCurImg.processStatus = IR_STATUS_BEGIN;

                    funResult = IKIR_JudgeImQuality(pParam->_algHandle, &pParam->_leftCurImg, pParam->_qualityMode, strFunRts);
                    if ( (IR_FUN_SUCCESS == funResult) && (pParam->_leftCurImg.QualityLevel >= IR_IMAGE_QUALITY_FAIR) )
                    {
                        savedScoreL = pParam->_leftCurImg.QualityScore;
                        leftQualifiedImgNum++;
                    }
                    else
                    {
                        savedScoreL = 0;
                    }
                }

                //判断右眼图像质量
                if( (IR_IM_EYE_RIGHT == pParam->_eyeDetectRet.rImFlag) && (LeftEye != pParam->_algApiCtrlParam.EyeMode) )
                {
                    pParam->_rightCurImg.processStatus = IR_STATUS_BEGIN;

                    funResult = IKIR_JudgeImQuality(pParam->_algHandle, &pParam->_rightCurImg, pParam->_qualityMode, strFunRts);
                    if ( (IR_FUN_SUCCESS == funResult) && (pParam->_rightCurImg.QualityLevel >= IR_IMAGE_QUALITY_FAIR) )
                    {
                        savedScoreR = pParam->_rightCurImg.QualityScore;
                        rightQualifiedImgNum++;
                    }
                    else
                    {
                        savedScoreR = 0;
                    }
                }

            }

            //交换当前采集到的图像
            if(pParam->_swapNewImgFlag)
            {
                unique_lock <mutex> lck(pParam->_xferCurrentImgMutex);

                imgInformCount++;
                memcpy(pParam->_pSwapImg, pParam->_pCapImg, g_constIrisImgSize);
                getFlag.GetDistanceFlag(imgInformCount, g_constImgNumToInform, &pParam->_irisPositionFlag);
                getFlag.GetClearFlag(imgInformCount, g_constImgNumToInform, leftQualifiedImgNum, rightQualifiedImgNum, &pParam->_lrIrisClearFlag);

                pParam->_swapNewImgFlag = false;
                pParam->_xferCurrentImgCV.notify_one();
            }


            //如果不处于注册或识别，则清空相关数据并重新开始
            if( (!pParam->_keepEnrFlag) && (!pParam->_keepIdenFlag) )
            {
                CASIAGenInitIrisInfo(&pParam->_leftBestImg);
                CASIAGenInitIrisInfo(&pParam->_rightBestImg);

                imgSelectCount = 0;
                leftQualifiedImgNum = 0;
                rightQualifiedImgNum = 0;
                leftQualifiedImgScore = 0;
                rightQualifiedImgScore = 0;
                continue;
            }

            imgSelectCount++;
            if(savedScoreL > leftQualifiedImgScore)
            {
                CASIAGenCopyIrisInfo(&pParam->_leftBestImg,&pParam->_leftCurImg);
                leftQualifiedImgScore = savedScoreL;
            }

            if(savedScoreR > rightQualifiedImgScore)
            {
                CASIAGenCopyIrisInfo(&pParam->_rightBestImg,&pParam->_rightCurImg);
                rightQualifiedImgScore = savedScoreR;
            }

            //如果需要交换最佳图像并且已经选择了足够多的图像，则交换采集到的最佳图像给注册或识别函数
            if( (pParam->_swapNewGoodImgFlag) && (imgSelectCount >= g_constImgNumToSelect) )
            {
                unique_lock<mutex> lck(pParam->_xferBestImgMutex);

                if(leftQualifiedImgNum >= g_constImgNumOfClear)
                {
                    CASIAGenCopyIrisInfo(&pParam->_leftEnrollImg[g_constIREnrollTimeNum-1], &pParam->_leftBestImg);
                    pParam->_getLeftNewGoodImgFlag = true;
                    CASIAGenInitIrisInfo(&pParam->_leftBestImg);
                }

                if(rightQualifiedImgNum >= g_constImgNumOfClear)
                {
                    CASIAGenCopyIrisInfo(&pParam->_rightEnrollImg[g_constIREnrollTimeNum-1], &pParam->_rightBestImg);
                    pParam->_getRightNewGoodImgFlag = true;
                    CASIAGenInitIrisInfo(&pParam->_rightBestImg);
                }

                if( (pParam->_getLeftNewGoodImgFlag) || (pParam->_getRightNewGoodImgFlag))
                {
                    imgSelectCount = 0;
                    leftQualifiedImgNum = 0;
                    rightQualifiedImgNum = 0;
                    leftQualifiedImgScore = 0;
                    rightQualifiedImgScore = 0;
                    pParam->_swapNewGoodImgFlag = false;
                    pParam->_xferBestImgCV.notify_one();
                }

            }
            //////////////////

        }
        else
        {
            //休眠，等待唤醒
            unique_lock <mutex> lck(pParam->_wakeMainThreadMutex);
            while(!pParam->_isWake)
            {
                pParam->_wakeMainThreadCV.wait(lck);
            }

        }

        usleep(0);
    }

    return ((void*)0);
}

/*****************************************************************************
*                         异步采图线程
*  函 数 名：ThreadAsyncCaptureImg
*  功    能：异步方式采图，通过调用用户传进来的回调函数通知用户采集图像成功
*  说    明：
*  参    数：pParam：输入参数，CAPIBase指针
*
*  返 回 值：NULL
*  创 建 人：lizhl
*  创建时间：2013-11-19
*  修 改 人：
*  修改时间：
*****************************************************************************/
void* CAPIBase::ThreadAsyncCaptureImg(IN CAPIBase* pParam)
{
    char strFunRts[g_constLengthOfStrFunRts];
    int funResult = E_ALGAPI_DEFAUL_ERROR;

    while(pParam->_mainThreadFlag)
    {
        if(pParam->_isWake)
        {
            unique_lock <mutex> lck(pParam->_asyncCapImgMutex);
            while(!pParam->_asyncCapImgFlag)
            {
                pParam->_asyncCapImgCV.wait(lck);
            }

            //等到条件信号后，调用同步采图函数
            funResult = pParam->SyncCapIrisImg(pParam->_cbIrisImg,
                                               pParam->_cbLRIrisClearFlag,
                                               pParam->_cbIrisPositionFlag, strFunRts);
            if(pParam->_cbCapIrisImg)
            {
                pParam->_cbCapIrisImg(funResult, pParam->_cbIrisImg, pParam->_cbLRIrisClearFlag, pParam->_cbIrisPositionFlag);
                pParam->InitCapCBSrc();
            }

            pParam->_asyncCapImgFlag = false;
        }

        usleep(0);//此处加sleep主要是防止出现休眠状态下while不断死循环造成cpu占用，但是几乎不可能出现该情况
    }

    //如果需要销毁本线程，需要加入额外的判断


    return ((void*)0);
}

/*****************************************************************************
*                         异步注册线程
*  函 数 名：ThreadAsyncEnroll
*  功    能：异步方式注册，通过调用用户传进来的回调函数通知用户注册结果
*  说    明：
*  参    数：pParam：输入参数，CAPIBase指针
*
*  返 回 值：NULL
*  创 建 人：lizhl
*  创建时间：2013-11-19
*  修 改 人：
*  修改时间：
*****************************************************************************/
void* CAPIBase::ThreadAsyncEnroll(IN CAPIBase* pParam)
{
    char strFunRts[g_constLengthOfStrFunRts];
    int funResult = E_ALGAPI_DEFAUL_ERROR;

    while(pParam->_mainThreadFlag)
    {
        if(pParam->_isWake)
        {
            unique_lock <mutex> lck(pParam->_asyncEnrMutex);
            while(!pParam->_asyncEnrFlag)
            {
                pParam->_asyncEnrCV.wait(lck);
            }

            //等到条件信号后，调用同步注册函数
            funResult = pParam->SyncStartEnroll(pParam->_cbEnrIrisL, pParam->_cbNumEnrL, pParam->_cbEnrIrisR, pParam->_cbNumEnrR, pParam->_cbEnrLRFlag, strFunRts);
            if(pParam->_cbEnroll)
            {
                pParam->_cbEnroll(funResult, pParam->_cbEnrIrisL, pParam->_cbNumEnrL, pParam->_cbEnrIrisR, pParam->_cbNumEnrR, pParam->_cbEnrLRFlag);
                pParam->InitEnrCBSrc();
            }

            pParam->_asyncEnrFlag = false;
        }

        usleep(0);//此处加sleep主要是防止出现休眠状态下while不断死循环造成cpu占用，但是几乎不可能出现该情况
    }

    //如果需要销毁本线程，需要加入额外的判断


    return ((void*)0);

}

/*****************************************************************************
*                         异步识别线程
*  函 数 名：ThreadAsyncIden
*  功    能：异步方式识别，通过调用用户传进来的回调函数通知用户识别结果
*  说    明：
*  参    数：pParam：输入参数，CAPIBase指针
*
*  返 回 值：NULL
*  创 建 人：lizhl
*  创建时间：2013-11-19
*  修 改 人：
*  修改时间：
*****************************************************************************/
void* CAPIBase::ThreadAsyncIden(IN CAPIBase* pParam)
{
    char strFunRts[g_constLengthOfStrFunRts];
    int funResult = E_ALGAPI_DEFAUL_ERROR;

    while(pParam->_mainThreadFlag)
    {
        if(pParam->_isWake && pParam->_asyncIdenFlag)
        {
            unique_lock <mutex> lck(pParam->_asyncIdenMutex);
            while(!pParam->_asyncIdenFlag)
            {
                pParam->_asyncIdenCV.wait(lck);
            }

            //等到条件信号后，调用同步识别函数
            funResult = pParam->SyncStartIden(pParam->_cbCodeListL, pParam->_cbCodeNumL, pParam->_cbCodeListR, pParam->_cbCodeNumR,
                                              &pParam->_cbRecIrisL, &pParam->_cbRecIrisR, pParam->_cbIdenLRFlag,
                                              &pParam->_cbLeftMatchOutputParam, &pParam->_cbRightMatchOutputParam,strFunRts);
            if(pParam->_cbIden)
            {
                pParam->_cbIden(funResult, &pParam->_cbRecIrisL, &pParam->_cbRecIrisR, pParam->_cbIdenLRFlag,
                                &pParam->_cbLeftMatchOutputParam, &pParam->_cbRightMatchOutputParam);
                pParam->InitIdenCBSrc();
            }

            pParam->_asyncIdenFlag = false;
        }

        usleep(0);//此处加sleep主要是防止出现休眠状态下while不断死循环造成cpu占用，但是几乎不可能出现该情况
    }

    //如果需要销毁本线程，需要加入额外的判断


    return ((void*)0);
}


