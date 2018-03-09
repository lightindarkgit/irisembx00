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
#include <math.h>   //为了在distanceCB中使用pow
#include <unistd.h>

#include "AlgApi.h"
#include "ApiBase.h"

#include "../Common/Logger.h"
#include "../Common/Exectime.h"

#ifdef WIN32
//#include <opencv.hpp>
//using namespace cv;
#include "imdebug.h"
#endif

//#include <QDebug>

#ifdef ALGAPITEST
#include "VirtualReadImg.h"
#endif

using namespace std;

#define USE_LCK//_NO

#ifdef WIN32
SYSTEMTIME systemTime;
#endif

void WriteLog(const char *log)
{
	Exectime etm(__FUNCTION__);
    FILE *logFile = fopen("AlgApiLog.txt","ab");

#ifdef WIN32
    //SYSTEMTIME systemTime;
    ::GetSystemTime(&systemTime);
    fprintf(logFile, "Time: %02d_%02d_%02d_%02d_%02d_%04ld \r\n",
            systemTime.wMonth, systemTime.wDay, systemTime.wHour + 8, systemTime.wMinute, systemTime.wSecond, systemTime.wMilliseconds);
#else
    struct timeval tvRestart;
    struct tm *ptmNowTime;
    time_t lt;
    lt =time(NULL);
    ptmNowTime = localtime(&lt);
    gettimeofday(&tvRestart,NULL);
    fprintf(logFile, "Time: %02d_%02d_%02d_%02d_%02d_%04ld \r\n",
            ptmNowTime->tm_mon+1,ptmNowTime->tm_mday,ptmNowTime->tm_hour,ptmNowTime->tm_min, ptmNowTime->tm_sec,tvRestart.tv_usec);
#endif

    fprintf(logFile, " %s \r\n", log);
    fclose(logFile);
}

#ifndef WIN32
//输出函数运行时间的宏定义
#define OUTPUT_FUN_RUNTIME_NO
#define DETECTION_RUNTIME_NO

#define THREAD_TEST_NO

#ifndef WIN32
    //20140321增加输出查看各函数运行时间
    struct timeval tvFunStart, tvFunEnd;
    unsigned int totalTime;
#endif

//输出算法API线程ID的宏定义
#define OUTPUT_ALGAPI_THREAD_ID

#ifdef OUTPUT_ALGAPI_THREAD_ID

#include <sys/types.h>
#include <sys/syscall.h>

void WriteAlgApiPID(const char *name)
{
	Exectime etm(__FUNCTION__);
	FILE *fp = fopen("PID_OF_ALGAPI.txt","a");
	if(nullptr != fp)
	{
        fprintf(fp, "PID[%d],TID[%ld] %s\n", getpid(), syscall(SYS_gettid), name);
		fclose(fp);
	}
}

#endif

void safeSleep(int sec, long nsec)
{
	Exectime etm(__FUNCTION__);
    struct timespec ts;
    ts.tv_sec = sec;
    ts.tv_nsec = nsec;
    int ret;
    while(1)
    {
        ret = nanosleep(&ts, &ts);
        if(0 == ret)
        {
//            std::cout << "nanosleep success" << std::endl;
            return;
        }
        else if(E_INTR == ret)
        {
//            std::cout << "nanosleep is interupt,remain:" << ts.tv_sec <<"_s," << ts.tv_nsec << "_ns" << std::endl;
            WriteLog("nanosleep is interupt");
            continue;
        }
        else
        {
//            std::cout << "nanosleep is error,remain:" << ts.tv_sec <<"_s," << ts.tv_nsec << "_ns" << std::endl;
            WriteLog("nanosleep is error");
            return;
        }
    }
}

#endif

mutex CAPIBase::s_instanceMutex;

//shared_ptr<CAPIBase> CAPIBase::s_pSingletonCAPIBase;
CAPIBase* CAPIBase::s_pSingletonCAPIBase = NULL;

//与串口相关的定义
//定义一个全局的取红外距离值的变量
int g_distanceValue = 0;
bool g_isLEDOn = false;

#ifndef WIN32
//定义一个全局的时间变量1微秒，代替usleep(0)
struct timespec g_ts1us;
#endif


//const unsigned char g_adCMD[4] =            {0x0C, 0x00, 0x00, 0x00};   //发送给串口的获取红外测距命令
//const unsigned char g_redAllOnCMD[4] =      {0x07, 0x03, 0x00, 0x00};   //控制红外灯全亮
//const unsigned char g_redLeftOnCMD[4] =     {0x07, 0x01, 0x00, 0x00};   //控制左红外灯亮
//const unsigned char g_redRightOnCMD[4] =    {0x07, 0x02, 0x00, 0x00};   //控制右红外灯亮
//const unsigned char g_redAllOffCMD[4] =     {0x07, 0x00, 0x00, 0x00};   //控制红外灯全灭

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
	Exectime etm(__FUNCTION__);

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
	Exectime etm(__FUNCTION__);
//    delete _pWakeMainThreadCond;
//    delete _pXferBestImgCond;
//    delete _pXferCurrentImgCond;
}

/*****************************************************************************
*                         串口获取红外测距值的回调函数
*  函 数 名：distanceCB
*  功    能：串口获取红外测距值的回调函数
*  说    明：
*  参    数：data：输入参数，串口数据
*			size：输入参数，串口长度
*
*  返 回 值：NULL
*
*  创 建 人：lizhl
*  创建时间：2014-01-17
*  修 改 人：
*  修改时间：
*****************************************************************************/
//void distanceCB(const unsigned char* data, const int size)
//{
////    qWarning("distance callback");
//    g_distanceValue = 0;
//    int factor = 10;

//    if(NULL != data)
//    {
//        for(int num = 0; num < size; num++)
//        {
//            //TO DO
//            g_distanceValue += data[num] * pow(factor, num);
//        }

//    }
//}


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
	Exectime etm(__FUNCTION__);
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
#ifndef WIN32
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
	Exectime etm(__FUNCTION__);
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
#ifndef WIN32
        _syncUsbApi = new SyncUSBAPI();
        funResult = _syncUsbApi->Open();
        if(E_ALGAPI_OK != funResult)
        {
            std::cout << "Init: open iriscamera failed, try again" << std::endl;
            WriteLog("Init: open iriscamera failed, try again");

            safeSleep(3,0);

            if(E_ALGAPI_OK != _syncUsbApi->Open())
            {
                std::cout << "Init: open iriscamera failed again" << std::endl;
                WriteLog("Init: open iriscamera failed again");
                SetFunRt(strFunRts,E_ALGAPI_INIT_FAILED, "Init usb failed");
                return E_ALGAPI_INIT_FAILED;
            }
        }
#else
		funResult = InitUSBDevice(0);
		if (E_ALGAPI_OK != funResult)
		{
			SetFunRt(strFunRts, E_ALGAPI_INIT_FAILED, "Init usb failed");
			return E_ALGAPI_INIT_FAILED;
		}
#endif
#endif

        //获取串口测距
//        _serialDistance = SerialMuxDemux::GetInstance();

//        //设置回调，在distanceCB中取距离值
//        if( false == _serialDistance->AddCallback(0x0c, distanceCB))
//        {
//            SetFunRt(strFunRts,E_ALGAPI_INIT_FAILED, "Set serial's callback failed");
//            return E_ALGAPI_INIT_FAILED;
//        }
//        qWarning("serial add callback success");

//        funResult = _serialDistance->Init("ttys1");//ttyUSB0
//        if(-20000 == funResult)
//        {
//            qWarning("serial init success");
//        }
//        else
//        {
//            qWarning("serial init failed");
//        }

//        if(0 != funResult)
//        {
//            SetFunRt(strFunRts,E_ALGAPI_INIT_FAILED, "Init serial failed");
//            return E_ALGAPI_INIT_FAILED;
//        }



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

//        _asyncCapImgFlag = false;
        _asyncCapImgLck.bFlag = false;
//        _asyncEnrFlag = false;
        _asyncEnrLck.bFlag = false;
//        _asyncIdenFlag = false;
        _asyncIdenLck.bFlag = false;

        //初始化回调相关指针
        InitCapCBSrc();
        InitEnrCBSrc();
        InitIdenCBSrc();

        _mainThreadFlag = true;
        _isWake = true;


        _tThreadMain = thread(ThreadMain, s_pSingletonCAPIBase);
        _tThreadCapDetectedIrisImg = thread(ThreadCapDetectedIrisImg, s_pSingletonCAPIBase);

        _tAsyncCapImg = thread(ThreadAsyncCaptureImg, s_pSingletonCAPIBase);
        _tAsyncEnroll = thread(ThreadAsyncEnroll, s_pSingletonCAPIBase);
        _tAsyncIden = thread(ThreadAsyncIden, s_pSingletonCAPIBase);



#ifndef WIN32
        _mutexXfer2Detect = PTHREAD_MUTEX_INITIALIZER;
        _condXfer2Detect = PTHREAD_COND_INITIALIZER;
#endif

#ifdef WIN32
		_tOperateSuccLamp = thread(ThreadOperateSuccLamp, s_pSingletonCAPIBase);
		_tOperateFailLamp = thread(ThreadOperateFailLamp, s_pSingletonCAPIBase);
#endif
    }

    if(!_isWake)
    {
		funResult = WakeAlgApi(strFunRts);
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

    _ifSaveImg = false;//默认不保存图像
    _ifUseAppName = false;//默认在保存图像时不使用用户传入的名字
    _saveImgIndex = 0;//图像保存序列号从0开始

    _instanceNum++;//必须是初始化成功后再+1
	
	    //定义一个全局的时间变量1微秒，代替usleep(0)
    g_ts1us.tv_sec = 0;
    g_ts1us.tv_nsec = 1000;      //1000 ns = 1 us,最小休眠时间为1 us??

	_isInitFlag = true;//初始化成功
	
    SetFunRt(strFunRts,E_ALGAPI_OK, "Init success");
    return E_ALGAPI_OK;
}

#else 

int CAPIBase::Init(checkDevStCallBack myCallBack, OUT char *strFunRts)
{
	Exectime etm(__FUNCTION__);
	int funResult = E_ALGAPI_DEFAUL_ERROR;
	//如果是第一次初始化，则初始化相关资源，初始化USB摄像头，开启采图线程
	if (0 == _instanceNum)
	{
		_algHandle = IKIR_HandleInit(strFunRts);
		if (NULL == _algHandle)
		{
			SetFunRt(strFunRts, E_ALGAPI_INIT_FAILED, "Get algorithm's handle failed");
			return E_ALGAPI_INIT_FAILED;
		}

		//打开USB模块
#ifndef ALGAPITEST
#ifndef WIN32
		_syncUsbApi = new SyncUSBAPI();
		funResult = _syncUsbApi->Open();
        if(E_ALGAPI_OK != funResult)
        {
            std::cout << "Init: open iriscamera failed, try again" << std::endl;
            WriteLog("Init: open iriscamera failed, try again");

            safeSleep(3,0);

            if(E_ALGAPI_OK != _syncUsbApi->Open())
            {
                std::cout << "Init: open iriscamera failed again" << std::endl;
                WriteLog("Init: open iriscamera failed again");
                SetFunRt(strFunRts,E_ALGAPI_INIT_FAILED, "Init usb failed");
                return E_ALGAPI_INIT_FAILED;
            }
        }
#else
		funResult = InitUSBDevice(0);
		if (E_ALGAPI_OK != funResult)
		{
			SetFunRt(strFunRts, E_ALGAPI_INIT_FAILED, "Init usb failed");
			return E_ALGAPI_INIT_FAILED;
		}

		SetDevCallBack(DevCallBack, (void *)s_pSingletonCAPIBase);
		appCallBack = myCallBack;

#endif
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

//		_asyncCapImgFlag = false;
        _asyncCapImgLck.bFlag = false;
//		_asyncEnrFlag = false;
        _asyncEnrLck.bFlag = false;
//		_asyncIdenFlag = false;
        _asyncIdenLck.bFlag = false;

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

        _tThreadCapDetectedIrisImg = thread(ThreadCapDetectedIrisImg, s_pSingletonCAPIBase);

#ifdef WIN32
		_tOperateSuccLamp = thread(ThreadOperateSuccLamp, s_pSingletonCAPIBase);
		_tOperateFailLamp = thread(ThreadOperateFailLamp, s_pSingletonCAPIBase);
#endif
	}

	if (!_isWake)
	{
		funResult = WakeAlgApi(strFunRts);
	}

	if (_keepIdenFlag)
	{
		funResult = StopIden(strFunRts);
		if (E_ALGAPI_OK != funResult)
		{
			SetFunRt(strFunRts, E_ALGAPI_INIT_FAILED, "Stop iden failed");
			return E_ALGAPI_INIT_FAILED;
		}
	}

	if (_keepEnrFlag)
	{
		funResult = StopEnroll(strFunRts);
		if (E_ALGAPI_OK != funResult)
		{
			SetFunRt(strFunRts, E_ALGAPI_INIT_FAILED, "Stop enroll failed");
			return E_ALGAPI_INIT_FAILED;
		}
	}

	//是否需要对采图功能进行初始化？？
	if (_cbCapIrisImg)
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

	_isInitFlag = true;//初始化成功

	SetFunRt(strFunRts, E_ALGAPI_OK, "Init success");
	return E_ALGAPI_OK;
}


void __stdcall DevCallBack(void *p)
{
	Exectime etm(__FUNCTION__);
	CAPIBase *apiBase = (CAPIBase*)p;
	if (nullptr == apiBase)
	{
		return;
	}

	checkDevStCallBack APPCallBack = (checkDevStCallBack)apiBase->appCallBack;
	if (nullptr != APPCallBack)
	{
		char strFunRts[g_constLengthOfStrFunRts];
		apiBase->StopEnroll(strFunRts);
		apiBase->StopIden(strFunRts);
		apiBase->_mainThreadFlag = false;
		APPCallBack(nullptr);
		apiBase->Release(strFunRts);
		WriteLog("设备拔出");
	}
}

#endif

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
int CAPIBase::Release(OUT char *strFunRts)
{
	Exectime etm(__FUNCTION__);
	//stopUsbTransfer();
    return E_ALGAPI_OK;
}

/*****************************************************************************
*                         算法逻辑API系统休眠
*  函 数 名：SleepAlgApi
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
int CAPIBase::SleepAlgApi(OUT char *strFunRts)
{
	Exectime etm(__FUNCTION__);
    if(!_isWake)
    {
        SetFunRt(strFunRts,E_ALGAPI_SLEEP_FAILED,"System is already sleep");
        std::cout << "CAPIBase::SleepAlgApi failed, System is already sleep" << std::endl;
        WriteLog("休眠AlgApi失败");
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

    {
        lock_guard<mutex> lck(_wakeMainThreadMutex);
        _isWake = false;
    }

#ifndef WIN32
    safeSleep(5,0);
    _syncUsbApi->Close();//关闭虹膜摄像头，停止图像采集
#else
    Sleep(5000);
    stopUsbTransfer();
#endif

    SetFunRt(strFunRts,E_ALGAPI_OK, "SleepAlgApi success");
    std::cout << "CAPIBase::SleepAlgApi success" << std::endl;
    WriteLog("休眠AlgApi成功");
    return E_ALGAPI_OK;
}

/*****************************************************************************
*                         算法逻辑API系统唤醒
*  函 数 名：WakeAlgApi
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
int CAPIBase::WakeAlgApi(OUT char *strFunRts)
{
	Exectime etm(__FUNCTION__);
    if(_isWake)
    {
        SetFunRt(strFunRts,E_ALGAPI_WAKE_FAILED,"System is already wake");
        std::cout << "CAPIBase::WakeAlgApi failed, system is already wake" << std::endl;
        return E_ALGAPI_WAKE_FAILED;
    }

#ifndef WIN32
    safeSleep(3,0);
#else
    Sleep(3000);
#endif

    //李言虹膜摄像头通电后需要1s钟的时间进行程序加载，在此之前不能保证成功打开摄像头
    //打开虹膜摄像头，重新进行图像采集
#ifndef WIN32
    if(0 != _syncUsbApi->Open())
#else
    if (0 != InitUSBDevice(0))
#endif
    {
        std::cout << "Wake: open iriscamera failed, try again" << std::endl;
        WriteLog("Wake: open iriscamera failed, try again");

#ifndef WIN32
        safeSleep(5,0);
#else
        Sleep(5000);
#endif

#ifndef WIN32
        if(0 != _syncUsbApi->Open())
#else
        if (0 != InitUSBDevice(0))
#endif
        {
            std::cout << "Wake: open iriscamera failed again" << std::endl;
            WriteLog("Wake: open iriscamera failed again");
            return E_ALGAPI_DEFAUL_ERROR;
        }
    }

    unique_lock <mutex> lck(_wakeMainThreadMutex);
    _isWake = true;
    _wakeMainThreadCV.notify_all();
    SetFunRt(strFunRts,E_ALGAPI_OK, "WakeAlgApi success");
    std::cout << "CAPIBase::WakeAlgApi success" << std::endl;
    WriteLog("唤醒成功");
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
	Exectime etm(__FUNCTION__);
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
	Exectime etm(__FUNCTION__);
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
*  参    数： distanceValue：输入参数，当前用户与设备间的距离值
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
int CAPIBase::SyncCapIrisImg(IN int distanceValue, OUT unsigned char *pIrisImg, OUT LRIrisClearFlag &lrIrisClearFlag, OUT IrisPositionFlag &irisPositionFlag, OUT char *strFunRts)
{
	Exectime etm(__FUNCTION__);
    if(!_isWake)
    {
        SetFunRt(strFunRts,E_ALGAPI_SYSTEM_IS_SLEEP,"System is sleep");
        return E_ALGAPI_SYSTEM_IS_SLEEP;
    }

    if(pIrisImg)
    {
        if(NoSoundDistanceValue == distanceValue)
        {
            _hasPerson = false;
        }
        else if(distanceValue >= LEDOnDistanceValue)
        {
            _hasPerson = false;
        }
        else
        {
            _hasPerson = true;
        }


        unique_lock<mutex> lck(_xferCurrentImgMutex);
        _pSwapImg = pIrisImg;


        _swapNewImgFlag = true;//通知主线程可以交换图像
        g_distanceValue = distanceValue;
        while(_swapNewImgFlag)
        {
//            _xferCurrentImgCV.wait(lck);
            if(_xferCurrentImgCV.wait_for(lck,chrono::seconds(1)) == cv_status::timeout)
            {
				_swapNewImgFlag = false;
                _pSwapImg = NULL;
                SetFunRt(strFunRts,E_ALGAPI_CAP_IMG_FAILED, "Capture image failed");
				WriteLog("采集图像超时");
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
*                         算法逻辑API采集检测到的虹膜图像
*  函 数 名：SyncCapDetectedIrisImg
*  功    能：同步方式采集检测到的虹膜图像，采集到的图像可用于实时显示
*  说    明：
*  参    数： haveIrisImg：输出参数，表明是否检测到虹膜图像
*            pLeftIrisImg：输出参数，检测到的左眼虹膜图像，由用户分配内存空间
*            pRightIrisImg：输出参数，检测到的右眼虹膜图像，由用户分配内存空间
*            strFunRts：输出参数，以字符串形式表示本次函数调用结果，由用户分配
*			 内存空间
*
*  返 回 值：0：采集虹膜图像成功
*    		<0：采集虹膜图像失败，根据返回值可以查询失败原因，或者参考strFunRts
*  创 建 人：lizhl
*  创建时间：2014-05-07
*  修 改 人：
*  修改时间：
*****************************************************************************/
int CAPIBase::SyncCapDetectedIrisImg(HaveIrisImg &haveIrisImg, unsigned char *pLeftIrisImg, unsigned char *pRightIrisImg, char *strFunRts)
{
	Exectime etm(__FUNCTION__);
    if(!_isWake)
    {
        SetFunRt(strFunRts,E_ALGAPI_SYSTEM_IS_SLEEP,"System is sleep");
        return E_ALGAPI_SYSTEM_IS_SLEEP;
    }

    if(pLeftIrisImg && pRightIrisImg)
    {
        unique_lock<mutex> lck(_swapDetectedIrisImgLck.stdMutex);
        _pSwapLeftIrisImg = pLeftIrisImg;
        _pSwapRightIrisImg = pRightIrisImg;
        _swapDetectedIrisImgLck.bFlag = true;//通知虹膜检测线程可以交换图像

        while(_swapDetectedIrisImgLck.bFlag)
        {
//            _xferCurrentImgCV.wait(lck);
            if(_swapDetectedIrisImgLck.stdCV.wait_for(lck,chrono::seconds(1)) == cv_status::timeout)
            {
                _swapDetectedIrisImgLck.bFlag = false;
                _pSwapLeftIrisImg = NULL;
                _pSwapRightIrisImg = NULL;

                SetFunRt(strFunRts,E_ALGAPI_CAP_IMG_FAILED, "Capture image failed");
                return E_ALGAPI_CAP_IMG_FAILED;
            }
        }

        _pSwapLeftIrisImg = NULL;
        _pSwapRightIrisImg = NULL;
        haveIrisImg = _haveIrisImg;

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
*  参    数：distanceValue：输入参数，当前用户与设备之间的距离值
*           appCapIrisImg：输入参数，用户注册的回调函数
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
int CAPIBase::AsyncCapIrisImg(IN int distanceValue, IN CBAlgApiCapIrisImg appCapIrisImg, OUT char *strFunRts)
{
	Exectime etm(__FUNCTION__);
    if(!_isWake)
    {
        SetFunRt(strFunRts,E_ALGAPI_SYSTEM_IS_SLEEP,"System is sleep");
        return E_ALGAPI_SYSTEM_IS_SLEEP;
    }

//    unique_lock <mutex> lck(_asyncCapImgMutex);
    unique_lock <mutex> lck(_asyncCapImgLck.stdMutex);

//    _cbIrisImg = pIrisImg;
    _cbCapIrisImg = appCapIrisImg;
    _cbDistanceValue = distanceValue;
//    _cbIrisPositionFlag = irisPositionFlag;
//    _cbLRIrisClearFlag = lrIrisClearFlag;

//    _asyncCapImgFlag = true;
    _asyncCapImgLck.bFlag = true;
//    _asyncCapImgCV.notify_one();//发送信号给异步采图线程
    _asyncCapImgLck.stdCV.notify_one();//发送信号给异步采图线程

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
	Exectime etm(__FUNCTION__);
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

    _leftExistedNum = 0;
    _rightExistedNum = 0;

    int funResult = E_ALGAPI_DEFAUL_ERROR;
    lrFlag = EnrRecUnknown;
    int enrResult = 0;

    //和超时判断相关变量
    int durationTime = 0;
#ifndef WIN32
    struct timeval tvEnrStart, tvEnrEnd;
#else
	clock_t tvEnrStart, tvEnrEnd;
#endif
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

    //初始化最佳图像
    CASIAGenInitIrisInfo(&_leftCurImg);
    CASIAGenInitIrisInfo(&_leftBestImg);

    CASIAGenInitIrisInfo(&_rightCurImg);
    CASIAGenInitIrisInfo(&_rightBestImg);

    for (int n = 0; n < g_constIREnrollTimeNum; n++)
    {
        CASIAGenInitIrisInfo(&_leftEnrollImg[n]);
        CASIAGenInitIrisInfo(&_rightEnrollImg[n]);
    }

#ifndef WIN32
    gettimeofday(&tvEnrStart,NULL);
#else
	tvEnrStart = clock();
#endif
    while(_keepEnrFlag)
    {

#ifndef WIN32
        gettimeofday(&tvEnrEnd,NULL);
		durationTime = tvEnrEnd.tv_sec - tvEnrStart.tv_sec;
#else
		tvEnrEnd = clock();
		durationTime = (int)(tvEnrEnd - tvEnrStart) / CLOCKS_PER_SEC;
#endif

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
                funResult = IKEnrollLRFeature(_algHandle, _algApiCtrlParam.IfSpoofDetect, _leftEnrollImg, &_leftExistedNum,
                                              &_leftEnrollImg[g_constIREnrollTimeNum-1], _leftEnrMatchImgInfo, &_curBestEnrMatchImgInfo, strFunRts);
                leftEyeActive = false;//为下一轮赋初值
            }

            if(rightEyeActive)
            {
                funResult = IKEnrollLRFeature(_algHandle, _algApiCtrlParam.IfSpoofDetect, _rightEnrollImg, &_rightExistedNum,
                                              &_rightEnrollImg[g_constIREnrollTimeNum-1], _rightEnrMatchImgInfo, &_curBestEnrMatchImgInfo, strFunRts);
                rightEyeActive = false;//为下一轮赋初值
            }

        }

        //判断是否注册成功
        if( (_leftExistedNum >= g_constMaxEnrollImNum) && flagOverMinTime && leftEyeNeed)
        {
            for(int count = 0; count < numEnrL; count++)
            {
                IKCopyIRIrisInfo2APIIrisInfo(enrIrisL+count, &_leftEnrollImg[count]);
            }

            enrResult += EnrRecLeftSuccess;
            leftEyeNeed = false;
        }

        if( (_rightExistedNum >= g_constMaxEnrollImNum) && flagOverMinTime && rightEyeNeed)
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

#ifdef WIN32
			//发信号点亮成功指示灯1秒
//			unique_lock <mutex> lckOfLamp(_operateSuccMutex);
            unique_lock <mutex> lckOfLamp(_operateSuccLck.stdMutex);
//			_operateSuccFlag = true;
            _operateSuccLck.bFlag = true;
//			_operateSuccCV.notify_one();
            _operateSuccLck.stdCV.notify_one();
#endif

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

#ifdef WIN32
			//发信号点亮失败指示灯1秒
//			unique_lock <mutex> lckOfLamp(_operateFailMutex);
            unique_lock <mutex> lckOfLamp(_operateFailLck.stdMutex);
//			_operateFailFlag = true;
            _operateFailLck.bFlag = true;
//			_operateFailCV.notify_one();
            _operateFailLck.stdCV.notify_one();
#endif

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
	Exectime etm(__FUNCTION__);
    if(!_isWake)
    {
        SetFunRt(strFunRts,E_ALGAPI_SYSTEM_IS_SLEEP,"System is sleep");
        return E_ALGAPI_SYSTEM_IS_SLEEP;
    }

//    unique_lock <mutex> lck(_asyncEnrMutex);
    unique_lock <mutex> lck(_asyncEnrLck.stdMutex);
    _cbEnroll = appEnroll;
//    _cbEnrIrisL = enrIrisL;
    _cbNumEnrL = numEnrL;
//    _cbEnrIrisR = enrIrisR;
    _cbNumEnrR = numEnrR;
//    _cbEnrLRFlag = lrFlag;


//    _asyncEnrFlag = true;
    _asyncEnrLck.bFlag = true;
//    _asyncEnrCV.notify_one();//发送信号给异步enroll线程
    _asyncEnrLck.stdCV.notify_one();//发送信号给异步enroll线程
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
	Exectime etm(__FUNCTION__);
    if(!_isWake)
    {
        SetFunRt(strFunRts,E_ALGAPI_SYSTEM_IS_SLEEP,"System is sleep");
        return E_ALGAPI_SYSTEM_IS_SLEEP;
    }

    //如果有回调函数存在，将其清空
    if(_cbEnroll)
    {
        InitEnrCBSrc();
    }

    {
        //设置停止注册标志位
        unique_lock <mutex> lck(_setEnrRecMutex);
        _keepEnrFlag = false;
    }

    //等待enroll函数结束
    while(!_outEnrFlag)
    {
#ifndef WIN32
        safeSleep(0,1000);//休眠1微秒
#else
		Sleep(0);
#endif
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
	Exectime etm(__FUNCTION__);
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

        if( ((true == leftEyeNeed) && (NULL == codeListL)) && ((true == rightEyeNeed) && (NULL == codeListR))  )//为了测试方便，将或关系改为与关系
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
#ifndef WIN32
    struct timeval tvRecStart, tvRecEnd;
#else
	clock_t tvRecStart, tvRecEnd;
#endif
//    bool flagOverTime = false;//超时的标志
    bool xferBestImgOverTime = false;//等待交换最佳图像超时标志，在1s中时间内没有从采图线程获得最佳图像用于识别

    //初始化输出结果,设置默认值
    IKInitAPIMatchOutput(pLeftMatchOutputParam);
    IKInitAPIMatchOutput(pRightMatchOutputParam);

    //初始化最佳图像
    CASIAGenInitIrisInfo(&_leftCurImg);
    CASIAGenInitIrisInfo(&_leftBestImg);

    CASIAGenInitIrisInfo(&_rightCurImg);
    CASIAGenInitIrisInfo(&_rightBestImg);

    for (int n = 0; n < g_constIREnrollTimeNum; n++)
    {
        CASIAGenInitIrisInfo(&_leftEnrollImg[n]);
        CASIAGenInitIrisInfo(&_rightEnrollImg[n]);
    }

#ifndef WIN32
    gettimeofday(&tvRecStart,NULL);
#else
	tvRecStart = clock();
#endif

    while(_keepIdenFlag)
    {
		//为本次识别赋初值
		leftEyeActive = false;
		rightEyeActive = false;

#ifndef WIN32
        gettimeofday(&tvRecEnd,NULL);
        durationTime = tvRecEnd.tv_sec - tvRecStart.tv_sec;
#else
		tvRecEnd = clock();
		durationTime = (int)(tvRecEnd - tvRecStart) / CLOCKS_PER_SEC;
#endif

        if(durationTime > maxRecTime)
        {
//            flagOverTime = true;
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
                    xferBestImgOverTime = true;
                    break;
                }
            }

            //如果获取最佳图像超时，则进入下一次循环
            if(xferBestImgOverTime)
            {
                xferBestImgOverTime = false;
                continue;
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

            if(leftEyeActive /*&& (nullptr != codeListL) && (codeNumL > 0)*/)
            {
                pImgInfoL = &(_leftEnrollImg[g_constIREnrollTimeNum-1]);
                //识别特征
                funRetL = IKIdenFeature(_algHandle, codeListL, codeNumL, pImgInfoL, inputPara, &outputMtPara, strFunRts);
//                std::cout << "Left: IrisRadius: " << pImgInfoL->irisRadius << "PupilRadius: " << pImgInfoL->pupilRadius << std::endl;
                IKCopyIRIrisInfo2APIIrisInfo(recIrisL, pImgInfoL);

                if( (IR_FUN_SUCCESS == funRetL) && leftEyeNeed)
                {
                    IKInsertIRMtOutput2APIMtOutput(pLeftMatchOutputParam, &outputMtPara, g_constMtIsLeftEye);
                    leftEyeNeed = false;
                    recResult += EnrRecLeftSuccess;
                }

            }

            if(rightEyeActive /*&& (nullptr != codeListR) && (codeNumR > 0)*/)
            {
                pImgInfoR = &(_rightEnrollImg[g_constIREnrollTimeNum-1]);
                //识别特征
                funRetR = IKIdenFeature(_algHandle, codeListR, codeNumR, pImgInfoR, inputPara, &outputMtPara, strFunRts);
//                std::cout << "Right: IrisRadius: " << pImgInfoR->irisRadius << "PupilRadius: " << pImgInfoR->pupilRadius << std::endl;
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

#ifdef WIN32
			//发信号点亮成功指示灯1秒
//			unique_lock <mutex> lckOfLamp(_operateSuccMutex);
            unique_lock <mutex> lckOfLamp(_operateSuccLck.stdMutex);
//			_operateSuccFlag = true;
            _operateSuccLck.bFlag = true;
//			_operateSuccCV.notify_one();
            _operateSuccLck.stdCV.notify_one();
#endif

            return E_ALGAPI_OK;
        }
        else
        {
            //识别失败计数加1
            if( (leftEyeActive && funRetL >= IR_MATCH_BAD_MATCHPARA && funRetL <= IR_FM_MATCH_ERR)
                    || (rightEyeActive && funRetR >= IR_MATCH_BAD_MATCHPARA && funRetR <= IR_FM_MATCH_ERR) )
            {
                idenFailedCount++;
            }
            //如果是双眼识别模式，只要有一只眼睛识别通过，则失败计数不增加
            //if(BothEye == _algApiCtrlParam.EyeMode)
            //{
            //    if( (false == leftEyeNeed) || (false == rightEyeNeed) )
            //    {
            //        idenFailedCount--;
            //    }
            //}

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

#ifdef WIN32
				//发信号点亮失败指示灯1秒
//				unique_lock <mutex> lckOfLamp(_operateFailMutex);
                unique_lock <mutex> lckOfLamp(_operateFailLck.stdMutex);
//				_operateFailFlag = true;
                _operateFailLck.bFlag = true;
//				_operateFailCV.notify_one();
                _operateFailLck.stdCV.notify_one();
#endif

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
	Exectime etm(__FUNCTION__);
    if(!_isWake)
    {
        SetFunRt(strFunRts,E_ALGAPI_SYSTEM_IS_SLEEP,"System is sleep");
        std::cout << "CAPIBase::AsyncStartIden failed, system is sleep" << std::endl;
        return E_ALGAPI_SYSTEM_IS_SLEEP;
    }

//    unique_lock <mutex> lck(_asyncIdenMutex);
    unique_lock <mutex> lck(_asyncIdenLck.stdMutex);

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


//    _asyncIdenFlag = true;
    _asyncIdenLck.bFlag = true;
//    _asyncIdenCV.notify_one();//发送信号给异步iden线程
    _asyncIdenLck.stdCV.notify_one();//发送信号给异步iden线程
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
	Exectime etm(__FUNCTION__);
    if(!_isWake)
    {
        SetFunRt(strFunRts,E_ALGAPI_SYSTEM_IS_SLEEP,"System is sleep");
        std::cout << "CAPIBase::StopIden failed, system is sleep" << std::endl;
        return E_ALGAPI_SYSTEM_IS_SLEEP;
    }

    //如果有回调函数存在，将其清空
    if(_cbIden)
    {
        InitIdenCBSrc();
    }

    {
        //设置停止识别标志位
        unique_lock <mutex> lck(_setEnrRecMutex);
        _keepIdenFlag = false;
    }

    //等待iden函数结束
    while(!_outIdenFlag)
    {
#ifndef WIN32
        safeSleep(0,1000);//休眠1微秒
#else
		Sleep(0);
#endif
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
	Exectime etm(__FUNCTION__);
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

        LOG_INFO("codeNum: %d,outputMtPara: %d , score:%f",codeNum,outputMtPara.matchIndex[0],outputMtPara.matchScore[0]);

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

//为了调试增加的存图函数，后期不开放接口
void CAPIBase::SaveImg(IN bool ifSaveImg, IN bool ifUseAppName, IN char *userName, IN char *pathOrigName, IN char *pathLeftName, IN char * pathRightName)
{
	Exectime etm(__FUNCTION__);
    _ifSaveImg = ifSaveImg;
    _ifUseAppName = ifUseAppName;

    if(_ifUseAppName)
    {
#ifdef WIN32
        sprintf_s(_userName, sizeof(_userName), "%s", userName);
        sprintf_s(_pathOrigName, sizeof(_pathOrigName), "%s", pathOrigName);
        sprintf_s(_pathLeftName, sizeof(_pathLeftName), "%s", pathLeftName);
        sprintf_s(_pathRightName, sizeof(_pathRightName), "%s", pathRightName);
#else
        snprintf(_userName, sizeof(_userName), "%s", userName);
        snprintf(_pathOrigName, sizeof(_pathOrigName), "%s", pathOrigName);
        snprintf(_pathLeftName, sizeof(_pathLeftName), "%s", pathLeftName);
        snprintf(_pathRightName, sizeof(_pathRightName), "%s", pathRightName);
#endif
    }
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
	Exectime etm(__FUNCTION__);
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
	Exectime etm(__FUNCTION__);
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
	Exectime etm(__FUNCTION__);
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
*  修 改 人：lizhl
*  修改时间：2014-05-07
*  修改说明：在主线程中只做采图，不做虹膜检测及质量判断，采到图后交给ThreadCapDetectedIrisImg线程做
*****************************************************************************/
//#ifdef WIN32
//采集到的图像,即送入算法的图像是"正"的(左下角是原点,但是在windows平台上显示,保存是"反"的,左上角是原点),
//因此在图像交换显示,采图保存时上下翻转一下,方便观看,
//未翻转图像与算法打交道
//翻转图像与送到界面显示,保存打交道
unsigned char algCapMirrorImg[g_constIrisImgWidth*g_constIrisImgHeight];
unsigned char algLeftMirrorImg[g_constIrisImgWidth*g_constIrisImgHeight];
unsigned char algRightMirrorImg[g_constIrisImgWidth*g_constIrisImgHeight];

unsigned char alg2IrisDetectImg[g_constIrisImgWidth*g_constIrisImgHeight];
unsigned char alg2IrisDetectMirrorImg[g_constIrisImgWidth*g_constIrisImgHeight];
//#endif

//由于ThreadMain和ThreadCapDetectedIrisImg两个线程都需要使用，因此将它定义为全局变量
unsigned int imgInformCount  = 0;//通知计数，用于通知APP的采图函数

#ifndef THREAD_TEST

void* CAPIBase::ThreadMain(IN CAPIBase* pParam)
{
	Exectime etm(__FUNCTION__);
#ifndef WIN32
    WriteAlgApiPID(__FUNCTION__);
#endif

    int funResult = E_ALGAPI_DEFAUL_ERROR;
    unsigned int getImgLen = 0;
    CAPIGetFlag getFlag;

    struct tm *ptmNowTime;
    struct timeval tv_NowTime;
    time_t lt;
    struct tm *ptmEndTime;
    struct timeval tv_EndTime;
    int usedTime;

    while(pParam->_mainThreadFlag)
    {
#ifndef WIN32
        safeSleep(0,1000);//休眠1微秒
#else
        Sleep(0);
#endif

        if(pParam->_isWake)
        {
            //采图，放到_pCapImg中

            //从USB设备中采集图像
#ifndef WIN32

#ifdef OUTPUT_FUN_RUNTIME
            std::cout << "^^^^^^^^In GetImage" << std::endl;
            gettimeofday(&tvFunStart,nullptr);
#endif

//            lt =time(NULL);
//            ptmNowTime = localtime(&lt);
//            gettimeofday(&tv_NowTime,NULL);

            funResult = pParam->_syncUsbApi->GetImage(pParam->_pCapImg, getImgLen);
            if(E_TIME_OVER == funResult)
            {
                WriteLog("采集大图超时");
            }

//            lt =time(NULL);
//            ptmEndTime = localtime(&lt);
//            gettimeofday(&tv_EndTime,NULL);
//            usedTime = (tv_EndTime.tv_sec - tv_NowTime.tv_sec)*1000000 + tv_EndTime.tv_usec - tv_NowTime.tv_usec;
//            std::cout << "GetImage use" << usedTime << "_usec"  << std::endl;







#ifdef OUTPUT_FUN_RUNTIME
            gettimeofday(&tvFunEnd,nullptr);
            totalTime = (tvFunEnd.tv_sec - tvFunStart.tv_sec)*1000*1000 + tvFunEnd.tv_usec-tvFunStart.tv_usec;
            std::cout << "USB get image need(us): " << totalTime << std::endl;
            std::cout << "vvvvvvvvvvOut GetImage" << std::endl;
#endif

#else
            //添加windows平台下采图函数调用
            funResult = getImage(pParam->_pCapImg);

#endif

            if(E_ALGAPI_OK != funResult)
            {
				LOG_ERROR("GetImage failed. Ret: %d",funResult);
				usleep(5000000);
                continue;
            }
            else
            {
                //采集图像成功，将图像返回给应用
                //上下翻转一下,方便显示
                ImageMirror(algCapMirrorImg, pParam->_pCapImg, g_constIrisImgHeight, g_constIrisImgWidth, false);

                //交换当前采集到的图像
                if(pParam->_swapNewImgFlag)
                {
#ifdef USE_LCK
                    unique_lock <mutex> lck(pParam->_xferCurrentImgMutex);
#endif

                    imgInformCount++;

#ifdef OUTPUT_FUN_RUNTIME
                    std::cout << "^^^^^^^^In ConvertRaw2Bmp" << std::endl;
                    gettimeofday(&tvFunStart,nullptr);
#endif

                    //送去显示的图像是已经上下翻转的图像
                    ConvertRaw2Bmp(pParam->_pSwapImg, algCapMirrorImg, g_constIrisImgHeight, g_constIrisImgWidth);


#ifdef OUTPUT_FUN_RUNTIME
                    gettimeofday(&tvFunEnd,nullptr);
                    totalTime = (tvFunEnd.tv_sec - tvFunStart.tv_sec)*1000*1000 + tvFunEnd.tv_usec-tvFunStart.tv_usec;
                    std::cout << "Raw2Bmp image need(us): " << totalTime << std::endl;
                    std::cout << "vvvvvvvvvvOut ConvertRaw2Bmp" << std::endl;
#endif
                    if (pParam->_ifSaveImg)
                    {
                        //20140730在主线程中保存原始图像
                        //增加linux平台的存图功能
                        char cTimeName[g_constArrayLen];
                        struct tm *ptmNowTime;
                        struct timeval tv_NowTime;
                        time_t lt;
                        lt =time(NULL);
                        ptmNowTime = localtime(&lt);
                        gettimeofday(&tv_NowTime,NULL);

                        if(!pParam->_ifUseAppName)
                        {
                            snprintf(cTimeName, sizeof(cTimeName),
                                     "./%02d_%02d_%02d_%02d_%02d_%04ld_Orig.bmp",
                                     ptmNowTime->tm_mon+1,ptmNowTime->tm_mday,ptmNowTime->tm_hour,ptmNowTime->tm_min, ptmNowTime->tm_sec,tv_NowTime.tv_usec);
                        }
                        else
                        {
                            snprintf(cTimeName, sizeof(cTimeName),
                                     "./%s/%s%02d_%02d_%02d_%02d_%02d_%04ld_Orig.bmp",
                                     pParam->_pathOrigName,pParam->_userName,
                                     ptmNowTime->tm_mon+1,ptmNowTime->tm_mday,ptmNowTime->tm_hour,ptmNowTime->tm_min, ptmNowTime->tm_sec,tv_NowTime.tv_usec);
                        }
                        SaveGrayBMPFile(cTimeName, algCapMirrorImg, g_constIrisImgHeight, g_constIrisImgWidth);
                    }

                    getFlag.GetDistanceFlag(imgInformCount, g_constImgNumToInform, &pParam->_irisPositionFlag);
                    //本线程只能得到位置信息，图像是否清晰信息要在ThreadCapDetectedIrisImg线程中得到
//                    getFlag.GetClearFlag(imgInformCount, g_constImgNumToInform, leftQualifiedImgNum, rightQualifiedImgNum, &pParam->_lrIrisClearFlag);

                    pParam->_swapNewImgFlag = false;
#ifdef USE_LCK
                    pParam->_xferCurrentImgCV.notify_one();
#endif
                }

                //20140916 endof

                if( (pParam->_keepEnrFlag || pParam->_keepIdenFlag) && pParam->_hasPerson)
                {
                    {
                    //如果处于注册或者识别
                    //将采集到的图像传递给ThreadCapDetectedIrisImg线程，做虹膜检测及质量判断
#ifdef USE_LCK
                        unique_lock <mutex> lck(pParam->_xfer2IrisDetectImgMutex);
#endif

                        pParam->_hasNewDetImg = true;
                        memcpy(alg2IrisDetectImg, pParam->_pCapImg, g_constIrisImgWidth*g_constIrisImgHeight);



#ifdef USE_LCK
                        pParam->_xfer2IrisDetectImgCV.notify_one();
#endif
                    }

                    CASIAGenInitIrisInfo(&pParam->_leftCurImg);
                    CASIAGenInitIrisInfo(&pParam->_rightCurImg);

                }
                else
                {
                    pParam->_lrIrisClearFlag = LAndRImgBlur;

                    //20140827增加虹膜追踪信息
                    pParam->InitTraceInfo(pParam->_leftTraceInfo);

                    //20140827增加虹膜追踪信息
                    pParam->InitTraceInfo(pParam->_rightTraceInfo);
                }

                //
            }
        }
        else
        {
            //休眠，等待唤醒
            std::cout << "ThreadMain:sleep" << std::endl;
            unique_lock <mutex> lck(pParam->_wakeMainThreadMutex);
            while(!pParam->_isWake)
            {
                pParam->_wakeMainThreadCV.wait(lck);
            }

        }

#ifndef WIN32
        safeSleep(0,1000000);//休眠1微秒
#else
        Sleep(0);
#endif
    }

    return (void*)0;

}

#endif

/*****************************************************************************
*                         采集虹膜线程
*  函 数 名：ThreadCapDetectedIrisImg
*  功    能：主线程采集到图像后传给本线程做虹膜检测和质量判断
*  说    明：
*  参    数：pParam：输入参数，CAPIBase指针
*
*  返 回 值：NULL
*  创 建 人：lizhl
*  创建时间：2014-05-07
*  修 改 人：
*  修改时间：
*****************************************************************************/
#ifndef THREAD_TEST

void* CAPIBase::ThreadCapDetectedIrisImg(IN CAPIBase* pParam)
{
	Exectime etm(__FUNCTION__);
#ifndef WIN32
    WriteAlgApiPID(__FUNCTION__);
#endif
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

    int imgSelectCount  = 0;//图像选择计数，用于注册或识别时挑选图像计数
    CAPIGetFlag getFlag;

    //保存检测到的左右眼虹膜图像的标志
    bool saveLeftImg = false;
    bool saveRightImg = false;

    //////////////////////////////////////
    //与图像保存相关的结构体
    char cTimeName[g_constArrayLen];
    char cTimeNameLeft[g_constArrayLen];
    char cTimeNameRight[g_constArrayLen];

#ifdef WIN32
    //SYSTEMTIME systemTime;
    //SYSTEMTIME systemEndTime;

#else
    struct tm *ptmNowTime;
    struct timeval tv_NowTime;
    time_t lt;
    static int n=0;
#endif
    //////////////////////////////////////

    if (0 != IKIR_ClsInit())
    {
        return ((void*)0);
    }


    while(pParam->_mainThreadFlag)
    {
#ifndef WIN32
        safeSleep(0,1000000);//休眠1微秒
#else
        Sleep(0);
#endif
	
        if(pParam->_isWake)
        {
//            gettimeofday(&tvFunStart,nullptr);

            {
                //等待MainThread交换采集到的图像，并进行虹膜检测
#ifdef USE_LCK
                unique_lock <mutex> lck(pParam->_xfer2IrisDetectImgMutex);
                pParam->_xfer2IrisDetectImgCV.wait(lck);
#endif
            }
//                if(pParam->_xfer2IrisDetectImgCV.wait_for(lck,chrono::seconds(1)) == cv_status::timeout)
//                {
//                    //如果不处于注册或识别，则清空相关数据并重新开始
//                    if( (!pParam->_keepEnrFlag) && (!pParam->_keepIdenFlag) )
//                    {
//                        CASIAGenInitIrisInfo(&pParam->_leftBestImg);
//                        CASIAGenInitIrisInfo(&pParam->_leftEnrollImg[g_constIREnrollTimeNum-1]);
//                        CASIAGenInitIrisInfo(&pParam->_rightBestImg);
//                        CASIAGenInitIrisInfo(&pParam->_rightEnrollImg[g_constIREnrollTimeNum-1]);

//                        imgSelectCount = 0;
//                        leftQualifiedImgNum = 0;
//                        rightQualifiedImgNum = 0;
//                        leftQualifiedImgScore = 0;
//                        rightQualifiedImgScore = 0;

//                        pParam->_lrIrisClearFlag = LAndRImgBlur;
//                    }
//                    continue;
//                }

//            if(!pParam->_hasNewDetImg)
//            {
//                continue;
//            }

//            gettimeofday(&tvFunEnd,nullptr);
//            totalTime = (tvFunEnd.tv_sec - tvFunStart.tv_sec)*1000*1000 + tvFunEnd.tv_usec-tvFunStart.tv_usec;
//            std::cout << "From lock to before of ImageMirror need(us): " << totalTime << std::endl;


                ImageMirror(alg2IrisDetectMirrorImg, alg2IrisDetectImg, g_constIrisImgHeight, g_constIrisImgWidth, false);

//                gettimeofday(&tvFunEnd,nullptr);
//                totalTime = (tvFunEnd.tv_sec - tvFunStart.tv_sec)*1000*1000 + tvFunEnd.tv_usec-tvFunStart.tv_usec;
//                std::cout << "From lock to end of ImageMirror need(us): " << totalTime << std::endl;

//                lt =time(NULL);
//                ptmNowTime = localtime(&lt);
//                gettimeofday(&tv_NowTime,NULL);
//                std::cout << "Swap Iris image: min" << ptmNowTime->tm_min << "_sec" << ptmNowTime->tm_sec << "_usec" << tv_NowTime.tv_usec << std::endl;

                if( (pParam->_keepEnrFlag || pParam->_keepIdenFlag) /*&& pParam->_hasPerson*/ /*&& pParam->_hasNewDetImg*/)
                {

#ifdef DETECTION_RUNTIME
                    std::cout << "^^^^^^^^In IrisDetection" << std::endl;
                    gettimeofday(&tvFunStart,nullptr);
#endif

                    pParam->_hasNewDetImg = false;

//                    funResult = IKIR_IrisDetection_Eye(pParam->_algHandle, alg2IrisDetectImg, g_constIrisImgHeight, g_constIrisImgWidth, &pParam->_eyeDetectRet, strFunRts);
                    //20140708 新的人眼检测算法定义左上角为原点,而原来的定义是左下角是原点
                    funResult = IKIR_IrisDetection_Eye(pParam->_algHandle, alg2IrisDetectMirrorImg, g_constIrisImgHeight, g_constIrisImgWidth, &pParam->_eyeDetectRet, strFunRts);

//                    funResult = 0;
//                    pParam->_eyeDetectRet.lImFlag = IR_IM_EYE_LEFF;
//                    pParam->_eyeDetectRet.lBgR = 300;
//                    pParam->_eyeDetectRet.lBgC = 400;


//                    pParam->_eyeDetectRet.rImFlag = IR_IM_EYE_RIGHT;
//                    pParam->_eyeDetectRet.rBgR = 300;
//                    pParam->_eyeDetectRet.rBgC = 1400;


#ifdef DETECTION_RUNTIME
                    gettimeofday(&tvFunEnd,nullptr);
                    totalTime = (tvFunEnd.tv_sec - tvFunStart.tv_sec)*1000*1000 + tvFunEnd.tv_usec-tvFunStart.tv_usec;
                    std::cout << "IrisDetection need(us): " << totalTime << std::endl;
                    std::cout << "vvvvvvvvvvOut IrisDetection" << std::endl;
#endif

//                    lt =time(NULL);
//                    ptmNowTime = localtime(&lt);
//                    gettimeofday(&tv_NowTime,NULL);
//                    std::cout << "After Iris detect: min" << ptmNowTime->tm_min << "_sec" << ptmNowTime->tm_sec << "_usec" << tv_NowTime.tv_usec << std::endl;
                }
                else
                {
                    funResult = E_ALGAPI_DEFAUL_ERROR;
                }

//            }

            if (pParam->_ifSaveImg)
            {
#ifdef WIN32
                ::GetSystemTime(&systemTime);
                if(pParam->_ifUseAppName)
                {
                    pParam->_saveImgIndex++;//实际保存图像序列号从1开始
                    sprintf_s(cTimeName, sizeof(cTimeName), "%s%s_%05d_%02d_%02d_%02d_%06ld_Orig.bmp",
                              pParam->_pathOrigName, pParam->_userName,
                              pParam->_saveImgIndex, systemTime.wHour + 8, systemTime.wMinute, systemTime.wSecond, systemTime.wMilliseconds);
                }
                else
                {
                    sprintf_s(cTimeName, sizeof(cTimeName), "C:\\IKUSB200\\OriginalImgs\\%02d_%02d_%02d_%02d_%02d_%04ld_Orig.bmp",
                              systemTime.wMonth, systemTime.wDay, systemTime.wHour + 8, systemTime.wMinute, systemTime.wSecond, systemTime.wMilliseconds);
                }
                //0711 原始图像在主线程中保存
                //SaveGrayBMPFile(cTimeName, alg2IrisDetectMirrorImg, g_constIrisImgHeight, g_constIrisImgWidth);
#else
                //增加linux平台的存图功能
                lt =time(NULL);
                ptmNowTime = localtime(&lt);
                gettimeofday(&tv_NowTime,NULL);

                snprintf(cTimeName, sizeof(cTimeName), "./%02d_%02d_%02d_%02d_%02d_%04ld_Orig.bmp",ptmNowTime->tm_mon+1,ptmNowTime->tm_mday,ptmNowTime->tm_hour,ptmNowTime->tm_min, ptmNowTime->tm_sec,tv_NowTime.tv_usec);
                //0711 原始图像在主线程中保存
                //SaveGrayBMPFile(cTimeName, alg2IrisDetectMirrorImg, g_constIrisImgHeight, g_constIrisImgWidth);
				
#endif
            }

            //funResult = idIrisDetection(pParam->_pCapImg, g_constIrisImgHeight, g_constIrisImgWidth, &pParam->_eyeDetectRet, strFunRts);
            if (0 == funResult)
            {
                //funResult=0只是代表函数调用成功,不代表检测到虹膜
#ifdef WIN32
                //imdebug("lum w=%d h=%d %p", IMAGE_WIDTH, IMAGE_HEIGHT, &pParam->_leftCurImg.imageData);
                //imdebug("lum w=%d h=%d %p", IMAGE_WIDTH, IMAGE_HEIGHT, &pParam->_rightCurImg.imageData);
#endif

                if ( (IR_IM_EYE_UNDEF != pParam->_eyeDetectRet.lImFlag) && (IR_IM_EYE_LEFF != pParam->_eyeDetectRet.lImFlag) )
                {
                    pParam->_eyeDetectRet.lBgR = 0;
                    //pParam->_eyeDetectRet.lEdR = 0;
                    pParam->_eyeDetectRet.lBgC = 0;
                    saveLeftImg = false;

                    //20140827增加虹膜追踪信息
                    pParam->InitTraceInfo(pParam->_leftTraceInfo);
                }
                else
                {
                    saveLeftImg = true;

                    //20140827增加虹膜追踪信息
                    pParam->_leftTraceInfo.bgC = pParam->_eyeDetectRet.lBgC/4;
                    pParam->_leftTraceInfo.bgR = pParam->_eyeDetectRet.lBgR/4;
                    pParam->_leftTraceInfo.centC = pParam->_eyeDetectRet.lCentC/4;
                    pParam->_leftTraceInfo.centR = pParam->_eyeDetectRet.lCentR/4;

                    //20140708人眼检测算法修改以后
                    pParam->_eyeDetectRet.lBgR = g_constIrisImgHeight * 4 - pParam->_eyeDetectRet.lBgR - g_constIrisImgHeight;
                }

                if ((IR_IM_EYE_UNDEF != pParam->_eyeDetectRet.rImFlag) && (IR_IM_EYE_RIGHT != pParam->_eyeDetectRet.rImFlag))
                {
                    pParam->_eyeDetectRet.rBgR = 0;
                    //pParam->_eyeDetectRet.rEdR = 0;
                    pParam->_eyeDetectRet.rBgC = 0;
                    saveRightImg = false;

                    //20140827增加虹膜追踪信息
                    pParam->InitTraceInfo(pParam->_rightTraceInfo);
                }
                else
                {
                    saveRightImg = true;

                    //20140827增加虹膜追踪信息
                    pParam->_rightTraceInfo.bgC = pParam->_eyeDetectRet.rBgC/4;
                    pParam->_rightTraceInfo.bgR = pParam->_eyeDetectRet.rBgR/4;
                    pParam->_rightTraceInfo.centC = pParam->_eyeDetectRet.rCentC/4;
                    pParam->_rightTraceInfo.centR = pParam->_eyeDetectRet.rCentR/4;

                    //20140708人眼检测算法修改以后
                    pParam->_eyeDetectRet.rBgR = g_constIrisImgHeight * 4 - pParam->_eyeDetectRet.rBgR - g_constIrisImgHeight;
                }

                if ((pParam->_eyeDetectRet.lBgR + pParam->_eyeDetectRet.lBgC + pParam->_eyeDetectRet.rBgC + pParam->_eyeDetectRet.rBgR) > 0)
                {
#ifdef WIN32
                    ::GetSystemTime(&systemTime);
                    std::cout << "Befor gerIrisImg:sec" << systemTime.wSecond << "_msec_" << systemTime.wMilliseconds << std::endl;

                    funResult = getIrisImg(pParam->_eyeDetectRet.lBgC, pParam->_eyeDetectRet.lBgR, (BYTE*)&pParam->_leftCurImg.imageData,
                        pParam->_eyeDetectRet.rBgC, pParam->_eyeDetectRet.rBgR, (BYTE*)&pParam->_rightCurImg.imageData);

                    ::GetSystemTime(&systemEndTime);
                    std::cout << "After gerIrisImg:sec" << systemEndTime.wSecond << "_msec_" << systemEndTime.wMilliseconds << std::endl;

                    //funResult = getIrisImg(pParam->_eyeDetectRet.lBgC, pParam->_eyeDetectRet.lEdR, (BYTE*)&pParam->_leftCurImg.imageData,
                    //	pParam->_eyeDetectRet.rBgC, pParam->_eyeDetectRet.rEdR, (BYTE*)&pParam->_rightCurImg.imageData);

                    //imdebug("lum w=%d h=%d %p", IMAGE_WIDTH, IMAGE_HEIGHT, pParam->_pCapImg);
                    //imdebug("lum w=%d h=%d %p", IMAGE_WIDTH, IMAGE_HEIGHT, &pParam->_leftCurImg.imageData);
                    //imdebug("lum w=%d h=%d %p", IMAGE_WIDTH, IMAGE_HEIGHT, &pParam->_rightCurImg.imageData);
					
#else
                    //增加linux平台的下发取图功能
                    unsigned int len1=0,len2=0;
//                    Command cmd(10,10,1000,10);
                    Command cmd(pParam->_eyeDetectRet.lBgC, pParam->_eyeDetectRet.lBgR,
                                pParam->_eyeDetectRet.rBgC, pParam->_eyeDetectRet.rBgR);

                    lt =time(NULL);
                    ptmNowTime = localtime(&lt);
                    gettimeofday(&tv_NowTime,NULL);
//                    std::cout << "Before GetPictureTaken: min" << ptmNowTime->tm_min << "_sec" << ptmNowTime->tm_sec << "_usec" << tv_NowTime.tv_usec << std::endl;


                    if (E_TIME_OVER == pParam->_syncUsbApi->GetPictureTaken(cmd,pParam->_leftCurImg.imageData,len1,
                                                         pParam->_rightCurImg.imageData,len2) )
                    {
                        WriteLog("采集两幅小图像超时");
                    }

					
#endif

//                    lt =time(NULL);
//                    struct tm *ptmEndTime;
//                    struct timeval tv_EndTime;
//                    ptmEndTime = localtime(&lt);
//                    gettimeofday(&tv_EndTime,NULL);
//                    int usedTime = (tv_EndTime.tv_sec - tv_NowTime.tv_sec)*1000000 + tv_EndTime.tv_usec - tv_NowTime.tv_usec;
//                    std::cout << "GetPictureTaken use" << usedTime << "_usec"  << std::endl;



                    ImageMirror(algLeftMirrorImg, pParam->_leftCurImg.imageData, g_constIrisImgHeight, g_constIrisImgWidth, false);
                    ImageMirror(algRightMirrorImg, pParam->_rightCurImg.imageData, g_constIrisImgHeight, g_constIrisImgWidth, false);

                    if ( (IR_IM_EYE_UNDEF == pParam->_eyeDetectRet.lImFlag) && (IR_IM_EYE_NONE == pParam->_eyeDetectRet.rImFlag) )
                    {
                        //如果左眼检测结果未知并且右眼没检测到，有可能是将右眼的图像给了左眼，因此复制一份给右眼
                        memcpy(pParam->_rightCurImg.imageData, pParam->_leftCurImg.imageData, g_constIrisImgSize);
                        pParam->_eyeDetectRet.rImFlag = IR_IM_EYE_UNDEF;
                    }


//                    lt =time(NULL);
//                    ptmNowTime = localtime(&lt);
//                    gettimeofday(&tv_NowTime,NULL);
//                    std::cout << "After ImageMirror: min" << ptmNowTime->tm_min << "_sec" << ptmNowTime->tm_sec << "_usec" << tv_NowTime.tv_usec << std::endl;


                    //////////////////////////////////////////////
                    //20140411 虹膜检测到后，将用于检测的原始640*480图像保存下来，并将检测到的左右眼虹膜图像640*480保存下来，在文件名中写入坐标值
                    if (pParam->_ifSaveImg)
                    {
#ifdef WIN32

                        if (saveLeftImg)
                        {
                            if(pParam->_ifUseAppName)
                            {
                                sprintf_s(cTimeNameLeft, sizeof(cTimeNameLeft), "%s%s_%05d_%02d_%02d_%02d_%06ld_L.bmp",
                                          pParam->_pathLeftName, pParam->_userName,
                                          pParam->_saveImgIndex, systemTime.wHour + 8, systemTime.wMinute, systemTime.wSecond, systemTime.wMilliseconds);
                            }
                            else
                            {
                                sprintf_s(cTimeNameLeft, sizeof(cTimeNameLeft), "C:\\IKUSB200\\DetectedImgs\\%02d_%02d_%02d_%02d_%02d_%04ld_Left_BgR_%04ld_BgC_%04d_Score_%03d.bmp",
                                          systemTime.wMonth, systemTime.wDay, systemTime.wHour + 8, systemTime.wMinute, systemTime.wSecond, systemTime.wMilliseconds,
                                          pParam->_eyeDetectRet.lBgR, pParam->_eyeDetectRet.lBgC, pParam->_eyeDetectRet.lQualityScore);
                            }

                            SaveGrayBMPFile(cTimeNameLeft, algLeftMirrorImg, g_constIrisImgHeight, g_constIrisImgWidth);
                        }

                        if (saveRightImg)
                        {
                            if(pParam->_ifUseAppName)
                            {
                                sprintf_s(cTimeNameRight, sizeof(cTimeNameRight), "%s%s_%05d_%02d_%02d_%02d_%06ld_R.bmp",
                                          pParam->_pathRightName, pParam->_userName,
                                          pParam->_saveImgIndex, systemTime.wHour + 8, systemTime.wMinute, systemTime.wSecond, systemTime.wMilliseconds);
                            }
                            else
                            {
                                sprintf_s(cTimeNameRight, sizeof(cTimeNameRight), "C:\\IKUSB200\\DetectedImgs\\%02d_%02d_%02d_%02d_%02d_%04ld_Righ_BgR_%04ld_BgC_%04d_Score_%03d.bmp",
                                          systemTime.wMonth, systemTime.wDay, systemTime.wHour + 8, systemTime.wMinute, systemTime.wSecond, systemTime.wMilliseconds,
                                          pParam->_eyeDetectRet.rBgR, pParam->_eyeDetectRet.rBgC, pParam->_eyeDetectRet.rQualityScore);
                            }

                            SaveGrayBMPFile(cTimeNameRight, algRightMirrorImg, g_constIrisImgHeight, g_constIrisImgWidth);
                        }
#else
                        //增加linux平台的存图功能
                        //if( n < 30 )
                        {
                            if(!pParam->_ifUseAppName)
                            {
                                snprintf(cTimeNameLeft, sizeof(cTimeNameLeft), "./Left%02d_%02d_%02d_%02d_%02d_%04ld_Left_BgR_%04d_BgC_%04d_Score_%03d.bmp",
                                         ptmNowTime->tm_mon+1,ptmNowTime->tm_mday,ptmNowTime->tm_hour,ptmNowTime->tm_min, ptmNowTime->tm_sec,tv_NowTime.tv_usec,
                                         pParam->_eyeDetectRet.lBgR, pParam->_eyeDetectRet.lBgC, pParam->_eyeDetectRet.lQualityScore);
                            }
                            else
                            {
                                snprintf(cTimeNameLeft, sizeof(cTimeNameLeft),
                                         "./%s/%s%02d_%02d_%02d_%02d_%02d_%04ld_Left.bmp",
                                         pParam->_pathLeftName,pParam->_userName,
                                         ptmNowTime->tm_mon+1,ptmNowTime->tm_mday,ptmNowTime->tm_hour,ptmNowTime->tm_min, ptmNowTime->tm_sec,tv_NowTime.tv_usec);

                            }

                            SaveGrayBMPFile(cTimeNameLeft, algLeftMirrorImg, g_constIKImgHeight, g_constIKImgWidth);


                            if(!pParam->_ifUseAppName)
                            {
                                snprintf(cTimeNameRight, sizeof(cTimeNameRight), "./Right%02d_%02d_%02d_%02d_%02d_%04ld_Righ_BgR_%04d_BgC_%04d_Score_%03d.bmp",
                                         ptmNowTime->tm_mon+1,ptmNowTime->tm_mday,ptmNowTime->tm_hour,ptmNowTime->tm_min, ptmNowTime->tm_sec,tv_NowTime.tv_usec,
                                         pParam->_eyeDetectRet.rBgR, pParam->_eyeDetectRet.rBgC, pParam->_eyeDetectRet.rQualityScore);
                            }
                            else
                            {
                                snprintf(cTimeNameRight, sizeof(cTimeNameRight),
                                         "./%s/%s%02d_%02d_%02d_%02d_%02d_%04ld_Right.bmp",
                                         pParam->_pathRightName,pParam->_userName,
                                         ptmNowTime->tm_mon+1,ptmNowTime->tm_mday,ptmNowTime->tm_hour,ptmNowTime->tm_min, ptmNowTime->tm_sec,tv_NowTime.tv_usec);

                            }
                            SaveGrayBMPFile(cTimeNameRight, algRightMirrorImg, g_constIKImgHeight, g_constIKImgWidth);

                        }

                        //n++;
#endif
                    }
                    //////////////////////////////////////////////
                }
                else
                {
                    //没检测到虹膜,使funResult不等于0,不调用质量判断
                    funResult = -1;
                }
				

            }
            else
            {
                //20141223 虹膜检测函数失败
                pParam->InitTraceInfo(pParam->_leftTraceInfo);
                pParam->InitTraceInfo(pParam->_rightTraceInfo);
            }

            /////////////////////////
//            funResult = -1;


            //检测到虹膜,进行质量判断
            if(E_ALGAPI_OK == funResult)
            {
                //判断左眼图像质量
                if( (IR_IM_EYE_LEFF == pParam->_eyeDetectRet.lImFlag || IR_IM_EYE_UNDEF == pParam->_eyeDetectRet.lImFlag) && (RightEye != pParam->_algApiCtrlParam.EyeMode) )
                {
                    pParam->_leftCurImg.processStatus = IR_STATUS_BEGIN;
                    //imdebug("lum w=%d h=%d %p", IMAGE_WIDTH, IMAGE_HEIGHT, &pParam->_leftCurImg.imageData);
					
#ifdef OUTPUT_FUN_RUNTIME
                    std::cout << "^^^^^^^^In JudgeImQuality" << std::endl;
                    gettimeofday(&tvFunStart,nullptr);
#endif
					
                    funResult = IKIR_JudgeImQuality(pParam->_algHandle, &pParam->_leftCurImg, pParam->_qualityMode, strFunRts);
					
#ifdef OUTPUT_FUN_RUNTIME
                    gettimeofday(&tvFunEnd,nullptr);
                    totalTime = (tvFunEnd.tv_sec - tvFunStart.tv_sec)*1000*1000 + tvFunEnd.tv_usec-tvFunStart.tv_usec;
                    std::cout << "JudgeImQualit need(us): " << totalTime << std::endl;
                    std::cout << "vvvvvvvvvvOut JudgeImQuality" << std::endl;
#endif

//                    lt =time(NULL);
//                    ptmNowTime = localtime(&lt);
//                    gettimeofday(&tv_NowTime,NULL);
//                    std::cout << "After Left IKIR_JudgeImQuality: min" << ptmNowTime->tm_min << "_sec" << ptmNowTime->tm_sec << "_usec" << tv_NowTime.tv_usec << std::endl;

					
                    if ( (IR_FUN_SUCCESS == funResult) && (pParam->_leftCurImg.QualityLevel >= IR_IMAGE_QUALITY_FAIR) )
                    {
                        savedScoreL = pParam->_leftCurImg.QualityScore;
                        leftQualifiedImgNum++;

                        //20140827增加虹膜追踪信息
                        pParam->_leftTraceInfo.irisRadius = pParam->_leftCurImg.irisRadius/4;
                        pParam->_leftTraceInfo.pupilRadius = pParam->_leftCurImg.pupilRadius/4;
                    }
                    else
                    {
                        savedScoreL = 0;
                    }
                }

                //判断右眼图像质量
                if( (IR_IM_EYE_RIGHT == pParam->_eyeDetectRet.rImFlag || IR_IM_EYE_UNDEF == pParam->_eyeDetectRet.rImFlag) && (LeftEye != pParam->_algApiCtrlParam.EyeMode) )
                {
                    pParam->_rightCurImg.processStatus = IR_STATUS_BEGIN;
                    //imdebug("lum w=%d h=%d %p", IMAGE_WIDTH, IMAGE_HEIGHT, &pParam->_rightCurImg.imageData);
                    funResult = IKIR_JudgeImQuality(pParam->_algHandle, &pParam->_rightCurImg, pParam->_qualityMode, strFunRts);

//                    lt =time(NULL);
//                    ptmNowTime = localtime(&lt);
//                    gettimeofday(&tv_NowTime,NULL);
//                    std::cout << "After Right IKIR_JudgeImQuality: min" << ptmNowTime->tm_min << "_sec" << ptmNowTime->tm_sec << "_usec" << tv_NowTime.tv_usec << std::endl;

                    if ( (IR_FUN_SUCCESS == funResult) && (pParam->_rightCurImg.QualityLevel >= IR_IMAGE_QUALITY_FAIR) )
                    {
                        savedScoreR = pParam->_rightCurImg.QualityScore;
                        rightQualifiedImgNum++;

                        //20140827增加虹膜追踪信息
                        pParam->_rightTraceInfo.irisRadius = pParam->_rightCurImg.irisRadius/4;
                        pParam->_rightTraceInfo.pupilRadius = pParam->_rightCurImg.pupilRadius/4;
                    }
                    else
                    {
                        savedScoreR = 0;
                    }
                }

            }

            //获取图像是否清晰的信息
            getFlag.GetClearFlag(imgInformCount, g_constImgNumToInform, leftQualifiedImgNum, rightQualifiedImgNum, &pParam->_lrIrisClearFlag);

            //交换当前检测到的虹膜图像
            if(pParam->_swapDetectedIrisImgLck.bFlag)
            {
#ifdef USE_LCK
                unique_lock <mutex> lck(pParam->_swapDetectedIrisImgLck.stdMutex);
#endif
                //////////////////////////
                //20140410
                int haveIrisImg = 0;
                if ((IR_IM_EYE_UNDEF == pParam->_eyeDetectRet.lImFlag) || (IR_IM_EYE_LEFF == pParam->_eyeDetectRet.lImFlag))
                {

                    ConvertRaw2Bmp(pParam->_pSwapLeftIrisImg, algLeftMirrorImg, g_constIrisImgHeight, g_constIrisImgWidth);

                    haveIrisImg += LeftIris;
                }

                if ((IR_IM_EYE_UNDEF == pParam->_eyeDetectRet.rImFlag) || (IR_IM_EYE_RIGHT == pParam->_eyeDetectRet.rImFlag))
                {

                    ConvertRaw2Bmp(pParam->_pSwapRightIrisImg, algRightMirrorImg, g_constIrisImgHeight, g_constIrisImgWidth);

                    haveIrisImg += RightIris;
                }
                pParam->_haveIrisImg = (HaveIrisImg)haveIrisImg;
                //////////////////////////

                pParam->_swapDetectedIrisImgLck.bFlag = false;
#ifdef USE_LCK
                pParam->_swapDetectedIrisImgLck.stdCV.notify_one();
#endif
            }

            //如果不处于注册或识别，则清空相关数据并重新开始
            if( (!pParam->_keepEnrFlag) && (!pParam->_keepIdenFlag) )
            {
                CASIAGenInitIrisInfo(&pParam->_leftCurImg);
                CASIAGenInitIrisInfo(&pParam->_leftBestImg);
                CASIAGenInitIrisInfo(&pParam->_leftEnrollImg[g_constIREnrollTimeNum-1]);

                CASIAGenInitIrisInfo(&pParam->_rightCurImg);
                CASIAGenInitIrisInfo(&pParam->_rightBestImg);
                CASIAGenInitIrisInfo(&pParam->_rightEnrollImg[g_constIREnrollTimeNum-1]);

                imgSelectCount = 0;
                savedScoreL = 0;
                savedScoreR = 0;
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
#ifdef USE_LCK
                unique_lock<mutex> lck(pParam->_xferBestImgMutex);
#endif
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
                    CASIAGenInitIrisInfo(&pParam->_leftCurImg);
                    CASIAGenInitIrisInfo(&pParam->_leftBestImg);

                    CASIAGenInitIrisInfo(&pParam->_rightCurImg);
                    CASIAGenInitIrisInfo(&pParam->_rightBestImg);

                    imgSelectCount = 0;
                    savedScoreL = 0;
                    savedScoreR = 0;
                    leftQualifiedImgNum = 0;
                    rightQualifiedImgNum = 0;
                    leftQualifiedImgScore = 0;
                    rightQualifiedImgScore = 0;

                    pParam->_swapNewGoodImgFlag = false;
#ifdef USE_LCK
                    pParam->_xferBestImgCV.notify_one();
#endif
                }

            }
            //////////////////

        }
        else
        {
            std::cout << "ThreadCapDetectedIrisImg:sleep" << std::endl;
            //休眠，等待唤醒
            unique_lock <mutex> lck(pParam->_wakeMainThreadMutex);
            while(!pParam->_isWake)
            {
                pParam->_wakeMainThreadCV.wait(lck);
            }

        }

#ifndef WIN32
        safeSleep(0,1000000);//休眠1微秒
#else
        Sleep(0);
#endif

    }

    IKIR_ClsRelease();

    return ((void*)0);
}

#endif


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
	Exectime etm(__FUNCTION__);
#ifndef WIN32
	WriteAlgApiPID(__FUNCTION__);
#endif

    char strFunRts[g_constLengthOfStrFunRts];
    int funResult = E_ALGAPI_DEFAUL_ERROR;

    while(pParam->_mainThreadFlag)
    {
        if(pParam->_isWake)
        {
//            unique_lock <mutex> lck(pParam->_asyncCapImgMutex);
            unique_lock <mutex> lck(pParam->_asyncCapImgLck.stdMutex);
//            while(!pParam->_asyncCapImgFlag)
            while(!pParam->_asyncCapImgLck.bFlag)
            {
//                pParam->_asyncCapImgCV.wait(lck);
                pParam->_asyncCapImgLck.stdCV.wait(lck);
            }

            //等到条件信号后，调用同步采图函数
            funResult = pParam->SyncCapIrisImg(pParam->_cbDistanceValue,
                                               pParam->_cbIrisImg,
                                               pParam->_cbLRIrisClearFlag,
                                               pParam->_cbIrisPositionFlag, 										   
											   strFunRts);
            if(pParam->_cbCapIrisImg)
            {
                pParam->_cbCapIrisImg(funResult, pParam->_cbIrisImg, pParam->_cbLRIrisClearFlag, pParam->_cbIrisPositionFlag,
                                      pParam->_leftTraceInfo, pParam->_rightTraceInfo,
                                      pParam->_leftExistedNum, pParam->_rightExistedNum);

                pParam->InitCapCBSrc();
            }

//            pParam->_asyncCapImgFlag = false;
            pParam->_asyncCapImgLck.bFlag = false;
        }
#ifndef WIN32
        safeSleep(0,1000);//休眠1微秒
#else
		Sleep(0);
#endif
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
	Exectime etm(__FUNCTION__);
#ifndef WIN32
	WriteAlgApiPID(__FUNCTION__);
#endif
    char strFunRts[g_constLengthOfStrFunRts];
    int funResult = E_ALGAPI_DEFAUL_ERROR;

    while(pParam->_mainThreadFlag)
    {
//        unique_lock <mutex> lck(pParam->_asyncEnrMutex);
        unique_lock <mutex> lck(pParam->_asyncEnrLck.stdMutex);
        if(pParam->_isWake)
        {

//            while(!pParam->_asyncEnrFlag)
            while(!pParam->_asyncEnrLck.bFlag)
            {
//                pParam->_asyncEnrCV.wait(lck);
                pParam->_asyncEnrLck.stdCV.wait(lck);
            }

            //等到条件信号后，调用同步注册函数
            funResult = pParam->SyncStartEnroll(pParam->_cbEnrIrisL, pParam->_cbNumEnrL, pParam->_cbEnrIrisR, pParam->_cbNumEnrR, pParam->_cbEnrLRFlag, strFunRts);
            if(pParam->_cbEnroll)
            {
                pParam->_cbEnroll(funResult, pParam->_cbEnrIrisL, pParam->_cbNumEnrL, pParam->_cbEnrIrisR, pParam->_cbNumEnrR, pParam->_cbEnrLRFlag);
                pParam->InitEnrCBSrc();
            }

//            pParam->_asyncEnrFlag = false;
            pParam->_asyncEnrLck.bFlag = false;
        }
#ifndef WIN32
        safeSleep(0,1000);//休眠1微秒
#else
		Sleep(0);
#endif
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
	Exectime etm(__FUNCTION__);
#ifndef WIN32
	WriteAlgApiPID(__FUNCTION__);
#endif
    char strFunRts[g_constLengthOfStrFunRts];
    int funResult = E_ALGAPI_DEFAUL_ERROR;

    while(pParam->_mainThreadFlag)
    {
        if(pParam->_isWake)
        {
//			unique_lock <mutex> lck(pParam->_asyncIdenMutex);
            unique_lock <mutex> lck(pParam->_asyncIdenLck.stdMutex);
//            while(!pParam->_asyncIdenFlag)
            while(!pParam->_asyncIdenLck.bFlag)
            {
//                pParam->_asyncIdenCV.wait(lck);
                pParam->_asyncIdenLck.stdCV.wait(lck);
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

//            pParam->_asyncIdenFlag = false;
            pParam->_asyncIdenLck.bFlag = false;
        }
#ifndef WIN32
        safeSleep(0,1000);//休眠1微秒
#else
		Sleep(0);
#endif
    }

    //如果需要销毁本线程，需要加入额外的判断


    return ((void*)0);
}

//20141223
void CAPIBase::InitTraceInfo(IrisTraceInfo &traceInfo)
{
	Exectime etm(__FUNCTION__);
    traceInfo.bgC = 0;
    traceInfo.bgR = 0;
    traceInfo.centC = 0;
    traceInfo.centR = 0;
}

#ifdef WIN32
/*****************************************************************************
*                         操作成功指示灯线程
*  函 数 名：ThreadOperateSuccLamp
*  功    能：操作成功指示灯线程
*  说    明：
*  参    数：pParam：输入参数，CAPIBase指针
*
*  返 回 值：NULL
*  创 建 人：lizhl
*  创建时间：2014-03-25
*  修 改 人：
*  修改时间：
*****************************************************************************/
void* CAPIBase::ThreadOperateSuccLamp(IN CAPIBase* pParam)
{
	Exectime etm(__FUNCTION__);
	char strFunRts[g_constLengthOfStrFunRts];
	int funResult = E_ALGAPI_DEFAUL_ERROR;

	while (pParam->_mainThreadFlag)
	{
//		unique_lock <mutex> lck(pParam->_operateSuccMutex);
		unique_lock <mutex> lckOfLamp(pParam->_operateSuccLck.stdMutex);
		if (pParam->_isWake)
		{

//			while (!pParam->_operateSuccFlag)
            while(!pParam->_operateSuccLck.bFlag)
			{
//				pParam->_operateSuccCV.wait(lck);
                pParam->_operateSuccLck.stdCV.wait(lckOfLamp);
			}

			if (pParam->_isInitFlag)
			{
				bool ret = OperateLamp(SuccessOn);
				Sleep(2000);
				OperateLamp(SuccessOff);
			}

//			pParam->_operateSuccFlag = false;
            pParam->_operateSuccLck.bFlag = false;
		}

	}

	//如果需要销毁本线程，需要加入额外的判断

	return ((void*)0);
}


/*****************************************************************************
*                         操作失败指示灯线程
*  函 数 名：ThreadOperateFailLamp
*  功    能：操作失败指示灯线程
*  说    明：
*  参    数：pParam：输入参数，CAPIBase指针
*
*  返 回 值：NULL
*  创 建 人：lizhl
*  创建时间：2014-03-25
*  修 改 人：
*  修改时间：
*****************************************************************************/
void* CAPIBase::ThreadOperateFailLamp(IN CAPIBase* pParam)
{
	Exectime etm(__FUNCTION__);
	char strFunRts[g_constLengthOfStrFunRts];
	int funResult = E_ALGAPI_DEFAUL_ERROR;

	while (pParam->_mainThreadFlag)
	{
//		unique_lock <mutex> lck(pParam->_operateFailMutex);
		unique_lock <mutex> lckOfLamp(pParam->_operateFailLck.stdMutex);
		if (pParam->_isWake)
		{

//			while (!pParam->_operateFailFlag)
            while (!pParam->_operateFailLck.bFlag)
			{
//				pParam->_operateFailCV.wait(lck);
                pParam->_operateFailLck.stdCV.wait(lckOfLamp);
			}		

			if (pParam->_isInitFlag)
			{
				OperateLamp(FailedOn);
				Sleep(2000);
				OperateLamp(FailedOff);
			}

//			pParam->_operateFailFlag = false;
            pParam->_operateFailLck.bFlag = false;
		}

	}

	//如果需要销毁本线程，需要加入额外的判断

	return ((void*)0);
}

#endif
