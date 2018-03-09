/******************************************************************************************
** 文件名:   ApiBase.h
×× 主要类:   CAPIBase
**  
** Copyright (c) 中科虹霸有限公司
** 创建人:   lizhl
** 日  期:   2013-11-11
** 修改人:   
** 日  期:
** 描  述:   IKEMBX00项目算法逻辑API内部使用的资源及相关定义，对外不可见
 *×× 主要模块说明: ApiBase.h
**
** 版  本:   1.0.0
** 备  注:   
**
*****************************************************************************************/
#pragma once

#include "IK_IR_DLL.h"
#include "BRCCommon.h"
#include "AlgApi.h"
#include "ApiGetFlag.h"

//#include "IR_DETECT_DLL.h"
#include "IK_IR_DLL_ID.h"

#ifndef WIN32
#include "../USBAPI/syncusbapi.h"
#include <sys/time.h>
#include <unistd.h>
//增加串口类，实现从红外测距模块得到距离值
#include "../SerialMuxDemux/SerialMuxDemux/include/serialmuxdemux.h"
#else
#include "common.h"
#include "usbInterface.h"

#include <time.h>
#include <Windows.h>
#endif

#include <mutex>
#include <thread>
#include <condition_variable>

//using namespace std;
//using std::tr1::shared_ptr;


const int g_constMaxENRTime         =	60;//20;		//目前实现方式的单位是秒，注册过程超过该时间会报注册超时，AlgApiCtrlParam中MaxENRTime的默认值
const int g_constImgNumToSelect =       1;//3;      //从几幅图像中选择一幅合格的虹膜图像用于算法
const int g_constImgNumToInform =       1;//3;      //每当得到几幅图像后，综合这几幅图像的位置信息，图像是否清晰信息，用于通知上层APP
const int g_constImgNumToSetLED =       20;     //每隔多少图像去设置红外LED的状态，开还是关，20幅图像大致为2秒
const int g_constImgNumOfClear =        1;      //清晰图像个数
const int g_constIdenFailedTolerantNum =3;      //识别失败允许个数，超过该值报识别失败

//定义一个全局的取红外距离值的变量
extern int g_distanceValue;
//void distanceCB(const unsigned char* data, const int size);

#ifdef WIN32
void __stdcall DevCallBack(void *p);
#endif

//将mutex，condition_variable，以及一个flag封装起来，方便使用
struct CAPILck
{
    std::mutex stdMutex;
    std::condition_variable stdCV;
    bool bFlag;
};


class CAPIBase
{
public:    
    ~CAPIBase();

    //获取单实例指针
    static CAPIBase* GetInstance();
//    static shared_ptr<CAPIBase> GetInstance();
    //获取设备获得的虹膜图像信息的尺寸信息
    int		GetImgInfo(OUT ImgSizeInfo *imgSizeInfo, OUT char *strFunRts);

#ifndef WIN32
    //初始化算法逻辑API使用的相关资源
    int		Init(OUT char *strFunRts);
#else
	//初始化算法逻辑API使用的相关资源
	int		Init(checkDevStCallBack myCallBack, OUT char *strFunRts);
#endif
    //释放算法逻辑API使用的相关资源
    int 	Release(OUT char *strFunRts);

    //使算法逻辑API进入休眠状态，不采集虹膜图像，但不释放相关资源
	int		SleepAlgApi(OUT char *strFunRts);
    //使算法逻辑API从休眠状态唤醒，恢复正常工作状态
	int		WakeAlgApi(OUT char *strFunRts);
    //获取算法逻辑API控制参数
    int		GetParam(OUT AlgApiCtrlParam *paramStruct, OUT char *strFunRts);
    //设置算法逻辑API控制参数
    int		SetParam(IN AlgApiCtrlParam *paramStruct, OUT char *strFunRts);

    //同步方式采集虹膜图像
    int		SyncCapIrisImg(IN int distanceValue, OUT unsigned char *pIrisImg, OUT LRIrisClearFlag &lrIrisClearFlag, OUT IrisPositionFlag &irisPositionFlag, OUT char *strFunRts);
    //异步方式采集虹膜图像
    int		AsyncCapIrisImg(IN int distanceValue, IN CBAlgApiCapIrisImg appCapIrisImg, OUT char *strFunRts);

    //同步方式虹膜注册
    int		SyncStartEnroll(OUT APIIrisInfo *enrIrisL, IN int numEnrL, OUT APIIrisInfo *enrIrisR,  IN int numEnrR, OUT LRSucFailFlag &lrFlag, OUT char *strFunRts);
    //异步方式虹膜注册
    int		AsyncStartEnroll(IN CBAlgApiEnroll appEnroll, IN int numEnrL, IN int numEnrR, OUT char *strFunRts);

    //停止虹膜注册
    int		StopEnroll(OUT char *strFunRts);

    //同步方式虹膜识别
    int		SyncStartIden(IN unsigned char *codeListL, IN int codeNumL,  IN unsigned char *codeListR, IN int codeNumR,
                      OUT APIIrisInfo *recIrisL, OUT APIIrisInfo *recIrisR, OUT LRSucFailFlag &lrFlag,
                      OUT APIMatchOutput *pLeftMatchOutputParam, OUT APIMatchOutput *pRightMatchOutputParam, OUT char *strFunRts);
    //异步方式虹膜识别
    int		AsyncStartIden(IN CBAlgApiIden appIden, IN unsigned char *codeListL, IN int codeNumL, IN unsigned char *codeListR, IN int codeNumR, OUT char *strFunRts);

    //停止虹膜识别
    int		StopIden(OUT char *strFunRts);
    //虹膜识别特征模板比对
    int		Match(IN unsigned char *recCode, IN int recNum, IN unsigned char *codeList, IN int codeNum, OUT APIMatchOutput *pMatchOutputParam, OUT char *strFunRts);

    //为了调试增加的存图函数，后期不开放接口
    void    SaveImg(IN bool ifSaveImg, IN bool ifUseAppName, IN char *userName, IN char *pathOrigName, IN char *pathLeftName, IN char * pathRightName);
    //同步模式采集虹膜图像，虹膜检测后的左、右眼图像，后期不开放接口
    int     SyncCapDetectedIrisImg(OUT HaveIrisImg &haveIrisImg, OUT unsigned char *pLeftIrisImg, OUT unsigned char *pRightIrisImg, OUT char *strFunRts);

private:
    CAPIBase();
    CAPIBase(const CAPIBase&);//禁止拷贝赋值
    CAPIBase& operator=(const CAPIBase&);

    void    InitCapCBSrc(void);                        //初始化异步采图函数相关资源
    void    InitEnrCBSrc(void);                        //初始化异步注册函数相关资源
    void    InitIdenCBSrc(void);                       //初始化异步识别函数相关资源

    //20141223
    void InitTraceInfo(IrisTraceInfo &traceInfo);

    static void* ThreadMain(IN CAPIBase* pParam);               //主线程
    static void* ThreadAsyncCaptureImg(IN CAPIBase* pParam);    //采图线程，异步采图时使用
    static void* ThreadAsyncEnroll(IN CAPIBase* pParam);    //注册线程，异步注册时使用
    static void* ThreadAsyncIden(IN CAPIBase* pParam);      //识别线程，异步识别时使用

    //增加一个采集虹膜图像线程，针对眼睛检测算法检测到虹膜后,原有的ThreadMain线程只用于采集全幅图像
    //在ThreadCapDetectedIrisImg中做眼睛检测、质量判断
    static void* ThreadCapDetectedIrisImg(IN CAPIBase* pParam);

#ifdef WIN32
	static void* ThreadOperateSuccLamp(CAPIBase* pParam);	//操作成功指示灯的线程
	static void* ThreadOperateFailLamp(CAPIBase* pParam);	//操作失败指示灯的线程
#endif

#ifdef WIN32
public:
	checkDevStCallBack appCallBack;						//应用程序注册的回调函数
#endif

private:
//    static shared_ptr<CAPIBase> s_pSingletonCAPIBase;
    static CAPIBase *s_pSingletonCAPIBase;
//    static APILocker s_instanceLocker;              //获取实例锁
    static std::mutex s_instanceMutex;                   //获取实例mutex

    std::mutex _xferCurrentImgMutex;                     //交换当前采集图像mutex
    std::condition_variable _xferCurrentImgCV;           //交换当前采集图像condition_variable

    std::mutex _xferBestImgMutex;                        //交换采集到最好图像mutex
    std::condition_variable _xferBestImgCV;              //交换采集到最好图像condition_variable

    std::mutex _wakeMainThreadMutex;                     //唤醒主线程mutex
    std::condition_variable _wakeMainThreadCV;           //唤醒主线程condition_variable

    std::mutex _wakeIrisDetThreadMutex;					 //唤醒虹膜检测线程mutex
    std::condition_variable _wakeIrisDetThreadCV;		 //唤醒虹膜检测线程condition_varible

    std::mutex _setEnrRecMutex;                          //设置注册、识别标志位mutex
    std::condition_variable _setEnrRecCV;                 //设置注册、识别标志位condition_variable

    CAPILck _asyncCapImgLck;
//    std::mutex _asyncCapImgMutex;                        //异步采图线程mutex
//    std::condition_variable _asyncCapImgCV;              //异步采图线程condition_variable
//    bool _asyncCapImgFlag;

    CAPILck _asyncEnrLck;
//    std::mutex _asyncEnrMutex;                           //异步注册线程mutex
//    std::condition_variable _asyncEnrCV;                 //异步注册线程condition_variable
//    bool _asyncEnrFlag;                                  //异步注册线程flag

    CAPILck _asyncIdenLck;                               //异步识别线程锁
//    std::mutex _asyncIdenMutex;                          //异步识别线程mutex
//    std::condition_variable _asyncIdenCV;                //异步识别线程condition_variable
//    bool _asyncIdenFlag;                                 //异步识别线程flag

    //异步采集虹膜图像线程锁，针对眼睛检测算法检测到虹膜图像后
    CAPILck _asyncCapDetectedIrisImgLck;
    bool _swapDetectedIrisImgFlag;                          //表示是否交换检测到的虹膜图像
    CAPILck _swapDetectedIrisImgLck;

    std::mutex _xfer2IrisDetectImgMutex;                     //交换当前采集虹膜图像mutex
    std::condition_variable _xfer2IrisDetectImgCV;           //交换当前采集虹膜图像condition_variable

#ifndef WIN32
    pthread_mutex_t _mutexXfer2Detect;
    pthread_cond_t _condXfer2Detect;
#endif


#ifdef WIN32
    CAPILck _operateSuccLck;
//	std::mutex _operateSuccMutex;						//操作成功指示灯的mutex
//	std::condition_variable _operateSuccCV;				//操作成功指示灯的condition_variable
//	bool _operateSuccFlag;								//操作成功指示灯的flag

    CAPILck _operateFailLck;
//	std::mutex _operateFailMutex;						//操作失败指示灯的mutex
//	std::condition_variable _operateFailCV;				//操作失败指示灯的condition_variable
//	bool _operateFailFlag;								//操作失败指示灯的flag
#endif

private:
    //和回调相关参数
    CBAlgApiCapIrisImg _cbCapIrisImg;               //采图回调函数
//    unsigned char *_cbIrisImg;                      //用于接收异步采图函数传进来的图像指针
//    LRIrisClearFlag *_cbLRIrisClearFlag;            //用于接收异步采图函数传进来的图像是否清晰标志
//    IrisPositionFlag *_cbIrisPositionFlag;          //用于接收异步采图函数传进来的图像位置信息标志
    int _cbDistanceValue;                           //用于异步采图的距离值
    unsigned char _cbIrisImg[g_constIrisImgSize+g_constBMPHeaderLength];   //用于异步采图接收图像，bmp格式

	unsigned char _cbLeftIrisImg[g_constIrisImgSize + g_constBMPHeaderLength];
	unsigned char _cbRightIrisImg[g_constIrisImgSize + g_constBMPHeaderLength];
	HaveIrisImg _cbHaveIrisImg;	

    LRIrisClearFlag _cbLRIrisClearFlag;             //用于异步采图的图像是否清晰标志
    IrisPositionFlag _cbIrisPositionFlag;           //用于异步采图的图像位置信息标志

    CBAlgApiEnroll _cbEnroll;                       //注册回调函数
//    APIIrisInfo *_cbEnrIrisL;                       //用于接收异步注册函数传进来的左眼注册虹膜信息结构体指针
    APIIrisInfo _cbEnrIrisL[g_constMaxEnrollImNum]; //用于异步注册函数的左眼注册虹膜信息结构体
    int _cbNumEnrL;                                 //用于接收异步注册函数传进来的左眼虹膜数量
//    APIIrisInfo *_cbEnrIrisR;                       //用于接收异步注册函数传进来的右眼注册虹膜信息结构体指针
    APIIrisInfo _cbEnrIrisR[g_constMaxEnrollImNum]; //用于异步注册函数的右眼注册虹膜信息结构体
    int _cbNumEnrR;                                 //用于接收异步注册函数传进来的右眼虹膜数量
//    LRSucFailFlag *_cbEnrLRFlag;                    //用于接收异步注册函数传进来的成功失败标志
    LRSucFailFlag _cbEnrLRFlag;                     //用于异步注册函数的成功失败标志

    CBAlgApiIden _cbIden;                           //识别回调函数
    unsigned char *_cbCodeListL;                    //用于接收异步识别函数传进来的左眼注册特征模板
    int _cbCodeNumL;                                //用于接收异步识别函数传进来的左眼注册特征模板数量
    unsigned char *_cbCodeListR;                    //用于接收异步识别函数传进来的右眼注册特征模板
    int _cbCodeNumR;                                //用于接收异步识别函数传进来的右眼注册特征模板数量
//    LRSucFailFlag *_cbIdenLRFlag;                   //用于接收异步识别函数传进来的成功失败标志
    LRSucFailFlag _cbIdenLRFlag;                    //用于异步识别函数的成功失败标志

//    APIIrisInfo *_cbRecIrisL;                       //用于接收异步识别函数传进来的左眼识别虹膜信息结构体指针
//    APIIrisInfo *_cbRecIrisR;                       //用于接收异步识别函数传进来的右眼识别虹膜信息结构体指针
//    APIMatchOutput *_cbLeftMatchOutputParam;        //用于接收异步识别函数传进来的左眼输出比对信息
//    APIMatchOutput *_cbRightMatchOutputParam;       //用于接收异步识别函数传进来的右眼输出比对信息
    APIIrisInfo _cbRecIrisL;                       //用于异步识别函数的左眼识别虹膜信息结构体指针
    APIIrisInfo _cbRecIrisR;                       //用于异步识别函数的右眼识别虹膜信息结构体指针
    APIMatchOutput _cbLeftMatchOutputParam;        //用于异步识别函数的左眼输出比对信息
    APIMatchOutput _cbRightMatchOutputParam;       //用于异步识别函数的右眼输出比对信息

private:
#ifndef WIN32
    SyncUSBAPI *_syncUsbApi;                        //USB同步采集图像类
//    SerialMuxDemux *_serialDistance;                //串口测距类
#endif

private:
    HANDLE _algHandle;                              //算法使用的句柄
    AlgApiCtrlParam _algApiCtrlParam;               //算法逻辑控制参数
    int _qualityMode;                               //系统使用时对图像质量的要求，可以理解为原来的注册模式或者识别模式
    int _instanceNum;                               //实例计数

    unsigned char _pCapImg[g_constIrisImgSize];     //为采集到的图像分配空间，从usb驱动取数据，裸数据，不包括文件格式头
    unsigned char *_pSwapImg;                       //交换给用户的图像指针

	unsigned char *_pSwapLeftIrisImg;				//交换给用户的检测到的左眼虹膜图像指针
	unsigned char *_pSwapRightIrisImg;				//交换给用户的检测到的右眼虹膜图像指针

    HaveIrisImg _haveIrisImg;                       //是否检测到虹膜的标志
    bool _ifSaveImg;                                //是否保存图像标志，默认为false，不保存
    bool _ifUseAppName;                             //是否使用应用程序输入的名字和路径，默认为false，使用时间命名；
                                                    //如果使用应用程序传入，相关文件夹必须在调用前创建完毕
    unsigned int _saveImgIndex;                     //保存图像序列号

    char _pathOrigName[g_constArrayLen];                            //原始图像保存路径
    char _pathLeftName[g_constArrayLen];                            //左眼图像保存路径
    char _pathRightName[g_constArrayLen];                           //右眼图像保存路径
    char _userName[g_constArrayLen];                                //保存图像的用户名

    EyeDetectionInfo2 _eyeDetectRet;                 //为算法分配的眼睛检测结构体
    LRIrisClearFlag _lrIrisClearFlag;               //左右眼图像是否合格标志位
    IrisPositionFlag _irisPositionFlag;             //虹膜位置信息标志位
    bool _hasPerson;                                //取距离标志位时是否有人标志
    bool _hasNewDetImg;

    IrisTraceInfo _leftTraceInfo;
    IrisTraceInfo _rightTraceInfo;

    bool _keepIdenFlag;                             //处于识别状态
    bool _outIdenFlag;                              //识别函数退出标志
    bool _keepEnrFlag;                              //处于注册状态
    bool _outEnrFlag;                               //注册函数退出标志

	bool _isInitFlag;								//是否初始化标志

#ifndef WIN32
    bool _mainThreadFlag;                           //表示主线程是否处于工作状态
#else
	public:
		bool _mainThreadFlag;
#endif
    bool _isWake;                                   //表示是否处于wake状态

    bool _swapNewImgFlag;                           //表示是否交换图像
    bool _swapNewGoodImgFlag;                       //表示是否交换合格图像
    bool _getNewImgFlag;                            //表示是否获取到一幅新的图像
    bool _getLeftNewGoodImgFlag;                    //表示左眼是否有新的合格图像
    bool _getRightNewGoodImgFlag;                   //表示右眼是否有新的合格图像

    //20140928
    int _leftExistedNum;
    int _rightExistedNum;


    IrisImageInfo _leftCurImg;                      //最新采集的左眼图像
    IrisImageInfo _leftBestImg;                     //从左眼图像序列中选取的最好的图像（等待或正被算法进行识别或注册）
    IrisImageInfo _leftEnrollImg[g_constIREnrollTimeNum];//左眼注册时选取的三幅最佳图像
    IrisImageInfo _rightCurImg;                     //最新采集的右眼图像
    IrisImageInfo _rightBestImg;                    //从右眼图像序列中选取的最好的图像（等待或正被算法进行识别或注册）
    IrisImageInfo _rightEnrollImg[g_constIREnrollTimeNum];//右眼注册时选取的三幅最佳图像

    IKIREnrMatchImInfo _leftEnrMatchImgInfo[g_constIREnrollImgNum+1];//左眼注册比对虹膜图像
    IKIREnrMatchImInfo _rightEnrMatchImgInfo[g_constIREnrollImgNum+1];//右眼注册比对虹膜图像
    IKIREnrMatchImInfo _curBestEnrMatchImgInfo;     //当前注册比对虹膜图像


    std::thread _tThreadMain;                            //主线程
    std::thread _tAsyncCapImg;                           //异步采图线程
    std::thread _tAsyncEnroll;                           //异步注册线程
    std::thread _tAsyncIden;                             //异步识别线程

    //增加一个采集虹膜图像线程，针对眼睛检测算法检测到虹膜后
    std::thread _tThreadCapDetectedIrisImg;

#ifdef WIN32
	std::thread _tOperateSuccLamp;
	std::thread _tOperateFailLamp;
#endif

};
