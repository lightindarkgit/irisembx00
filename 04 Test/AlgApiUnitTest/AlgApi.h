/******************************************************************************************
** 文件名:   AlgApi.h
×× 主要类:   CAlgApi,CCameraApi
**  
** Copyright (c) 中科虹霸有限公司
** 创建人:   lizhl
** 日  期:   2013-11-05
** 修改人:   
** 日  期:
** 描  述:   IKEMBX00项目算法逻辑API,为该项目提供虹膜图像采集,虹膜注册,虹膜识别等功能
 *×× 主要模块说明: AlgApi.h                               算法逻辑类,人脸图像采集类                     
**
** 版  本:   1.0.0
** 备  注:   
**
*****************************************************************************************/
#pragma once

//#include <tr1/memory>
#include "common.h"

//为单元测试定义的宏
#define ALGAPITEST_NO

const int g_constIrisImgHeight  = 704;//1536;//待修改
const int g_constIrisImgWidth   = 2048;//待修改
const int g_constIrisImgSize    = ((g_constIrisImgHeight)*(g_constIrisImgWidth));

//"P5\n2048 704\n255\n";
const int g_constIrisPPMHeaderLength = 16;

const int g_constFaceImgHeigth  = 400;//待修改
const int g_constFaceImgWidth   = 300;//待修改
const int g_constFaceImgSize    = ((g_constFaceImgHeigth)*(g_constFaceImgWidth));

//算法处理的标准虹膜尺寸信息
const int g_constIKImgHeight        =   480;		//虹膜图像的高度
const int g_constIKImgWidth         =   640;		//虹膜图像的宽度
const int g_constIKImgSize          =   307200;		//虹膜图像大小
const int g_constIKEnrFeatureLen    =	512;        //虹膜注册特征的大小
const int g_constIKRecFeatureLen    =   1024;       //虹膜识别特征的大小

//注册、识别成功失败标志
enum LRSucFailFlag
{
	EnrRecBothFailed	=	-3,		//EnrRecBothFailed：注册、识别时左右眼均注册或识别失败
	EnrRecRightFailed	=	-2,		//EnrRecRightFailed:右眼注册、识别失败导致注册或识别失败,左眼情况未知
	EnrRecLeftFailed	=	-1,		//EnrRecLeftFailed:左眼注册、识别失败导致注册或识别失败,右眼情况未知
    EnrRecUnknown       =   0,      //EnrRecUnknown:注册、识别结果未知
    EnrRecLeftSuccess	=	1,		//EnrRecLeftSuccess:注册、识别时仅左眼注册或识别成功
	EnrRecRightSuccess	=	2,		//EnrRecRightSuccess:注册、识别时仅右眼注册或识别成功
	EnrRecBothSuccess	=	3		//EnrRecBothSuccess:注册、识别时左右眼均注册成功
};

//图像清晰标志
enum LRIrisClearFlag
{
	LAndRImgBlur	=	0,	//左眼、右眼图像都不合格；
	LImgClear		=	1,	//左眼图像合格
	RImgClear		=	2,	//右眼图像合格
	LAndRImgClear	=	3	//左眼、右眼图像都合格
};

//位置信息标志
enum IrisPositionFlag
{
    Far				=	0,	//位置偏远
    Near			=	1,  //位置偏近
    Left			=	2,  //位置偏左
    Right			=	3,  //位置偏右
    Up				=	4,  //位置偏上
    Down			=	5,  //位置偏下
    OK				=	6,  //位置合适
    Unknown         =   7   //位置未知，不用提示
};

//注册图像个数
const int g_constMaxEnrollImNum =	3;		//注册过程中算法允许的最大注册图像数

//设备 算法逻辑控制的输入参数
//参数1. eyeMode——采图、注册、识别时的眼睛模式
enum APIEyeMode
{
    BothEye         =   0,      //双眼
    LeftEye         =   1,      //左眼
    RightEye        =   2,      //右眼
    AnyEye          =   3       //任意眼，注册模式下，任意眼无效
};

//参数2. 注册时允许的最小时间、识别时允许的最大时间，单位：s，建议使用默认值
const int g_constMinENRTime         =	10;		//目前实现方式的单位是秒，注册过程必须超过这个时间才会有注册成功，用户设置的注册时间必须不小于该值。
                                                //举例：用户设置了15，在0-10秒这段时间，不会有注册结果；10-15秒之间，如果注册成功则返回成功，不成功则继续；超过15秒还没有成功则返回失败
const int g_constMaxRECTime         =	5;		//单位秒，如果识别过程超过这个时间，则返回失败

//参数3. findMode——是“遍历搜索”还是“找到即停”,建议使用默认值 g_constFindModeExhaust
const int g_constFindModeExhaust    =	0;		//遍历搜索，默认值
const int g_constFindModeFirstMatch =   1;		//找到即停

//参数4.是否执行假眼检测
const int g_constIKNoSpoofDetect    =	0;		//不执行假眼检测，默认值
const int g_constIKDoSpoofDetect    = 	1;		//执行假眼检测

//比对输出参数
const int g_constMaxNumOfMatch      =	100;	//比对过程算法最多返回的比对结果数
//left2right
const int g_constMtIsLeftEye        =   01;     //比对或者识别结果中序号为左眼的序号
const int g_constMtIsRightEye       =   02;     //比对或者识别结果中序号为右眼的序号
const int g_constMtUnknowEye        =   0;      //对比对图像的左眼或者右眼信息不明确,此情况在用户输入IK_AnyEye时可能出现

//函数调用错误码

const int E_ALGAPI_ERROR_BASE               =       0;      //基础错误码

//与系统相关的定义，以1***开头
const int E_ALGAPI_INIT_FAILED              =	E_ALGAPI_ERROR_BASE	- 1001;	//API初始化失败或者尚未初始化
const int E_ALGAPI_HANDLE_ERROR             =   E_ALGAPI_ERROR_BASE	- 1002;	//在与Handle相关的函数中失败
const int E_ALGAPI_INVALID_DEV_STATUS       =   E_ALGAPI_ERROR_BASE	- 1003;	//设备状态出错,设备初始化失败或停止工作或设备被拔出

//与注册或识别算法逻辑相关的以21**开头
const int E_ALGAPI_ENR_IDEN_FAILED          =	E_ALGAPI_ERROR_BASE	- 2101;	//注册或识别失败
const int E_ALGAPI_FAIL2STOP                =	E_ALGAPI_ERROR_BASE	- 2102;	//调用停止注册或停止识别函数时失败
const int E_ALGAPI_ALGORITHM_CONFLICT       =   E_ALGAPI_ERROR_BASE	- 2103;	//算法冲突，注册、识别不能同时进行
const int E_ALGAPI_INVALID_ENR_INPUT        =   E_ALGAPI_ERROR_BASE - 2104; //注册时无效的输入,如注册图像数目，注册时间，EyeMode等
const int E_ALGAPI_INVALID_REC_INPUT        =   E_ALGAPI_ERROR_BASE - 2105; //识别时无效的输入,如识别时间，EyeMode等
const int E_ALGAPI_INVALID_MATCH_INPUT      =   E_ALGAPI_ERROR_BASE - 2106; //比对时无效的输入
const int E_ALGAPI_IDEN_OVERTIME            =   E_ALGAPI_ERROR_BASE - 2107; //识别时overtime

//特别的，当人为调用停止注册或识别函数时，之前调用的注册函数或识别函数将返回1
const int E_ALGAPI_ENR_IDEN_INTERRUPT       =   1;                          //注册、识别时，APP中止了注册或识别过程；也用于采集合格图像时APP中止了采集合格图像

//与比对算法逻辑相关的以22**开头
const int E_ALGAPI_MATCH_FAILED             = 	E_ALGAPI_ERROR_BASE	- 2201;  //比对失败

//与采图相关的以23**开头
const int E_ALGAPI_CAP_IMG_FAILED           =	E_ALGAPI_ERROR_BASE	- 2301;	//图像采集失败

//与算法控制参数相关的以30**开头，
const int E_ALGAPI_INVALID_EYEMODE          =   E_ALGAPI_ERROR_BASE	- 3001;	//无效的Eyemode
const int E_ALGAPI_INVALID_TIME             =   E_ALGAPI_ERROR_BASE	- 3002;	//无效的注册时间或识别时间
const int E_ALGAPI_INVALID_FINDMODE         =   E_ALGAPI_ERROR_BASE	- 3003;	//无效的FindMode
const int E_ALGAPI_INVALID_IFSPOOFDETECT    =   E_ALGAPI_ERROR_BASE	- 3004; //无效的IfSpoofDetect
const int E_ALGAPI_INVALID_PARAMETER        =   E_ALGAPI_ERROR_BASE	- 3005; //无效的算法控制参数

//与休眠、唤醒相关的以40**开头
const int E_ALGAPI_SLEEP_FAILED             =   E_ALGAPI_ERROR_BASE - 4001; //休眠失败，系统已处于休眠状态
const int E_ALGAPI_WAKE_FAILED              =   E_ALGAPI_ERROR_BASE - 4002; //唤醒失败，系统已处于唤醒状态
const int E_ALGAPI_SYSTEM_IS_SLEEP          =   E_ALGAPI_ERROR_BASE - 4003; //系统处于休眠状态，相关函数调用失败

//函数调用通用返回值
const int E_ALGAPI_OK                       =	0;                          //函数调用成功，并且完成了函数功能
const int E_ALGAPI_DEFAUL_ERROR             =	E_ALGAPI_ERROR_BASE	- 8000;	//函数调用缺省错误，一般为函数调用结果初始值，一般不可能返回该值

//算法逻辑控制参数
struct AlgApiCtrlParam
{
    APIEyeMode  EyeMode;		//注册或者识别时的眼睛选择模式，如任意眼，左眼，右眼或者双眼模式
    int         MaxENRTime;     //注册时允许的最大时间，单位：s，超过该时间报注册超时，默认值为20
    int         MaxRECTime;     //识别时允许的最大时间，单位：s
	//以下为识别过程中可能用到的参数
    int         FindMode;		//是“遍历搜索”还是“找到即停”
	//g_constIKNoSpoofDetect : Do not perform spoof eye detection 默认值
	//g_constIKDoSpoofDetect : Perform spoof eye detection
    int         IfSpoofDetect; //是否进行假眼检测，该功能尚未正式发布，结果可能不准确。
};

//识别输出信息,对用户可见
struct APIMatchOutput
{
	//Decision threshold that has been used in current iris match, 
	//above which a match is considered true.
	float MatchTh; 

	//The number of matched iris templates in database.
	int MatchNum;

	//Index of matched template in the enrolled template list. 
	long MatchIndex[g_constMaxNumOfMatch];

	//Indicator of left or right eye
    int Left2Right[g_constMaxNumOfMatch];//暂不用关注

	//Match score of each match, ranging from 0 to 1, 
	//and the higher the more confidence of the match.
	float MatchScore[g_constMaxNumOfMatch];
};

//API虹膜图像结构信息
struct APIIrisInfo
{  
	//Height of the iris image in pixels. It must be fixed to 480 currently.
	int ImgHeight;  

	//Width of the iris image in pixels. It must be fixed to 640 currently.
	int ImgWidth;  

	//1D array of pixel intensity values of the original iris image
    unsigned char ImgData[g_constIKImgSize];

    //1D array of pixel intensity values of the demo iris image. Pupil/iris and iris/sclera boundary points are marked with white pixels.
    unsigned char LocImage[g_constIKImgSize];

	//The row index of pupil center in iris image, ranging from 1 to IR_IMAGE_HEIGHT.
	int PupilRow;

	//The column index of pupil center in iris image,ranging from 1 toIR_IMAGE_WIDTH
	int PupilCol;

	//The radius of pupil in iris image
	int PupilRadius;

	//The row index of iris center in iris image, ranging from 1 to IR_IMAGE_HEIGHT
	int IrisRow;

	//The column index of iris center in iris image, ranging from 1 to IR_IMAGE_WIDTH
	int IrisCol;

	//The radius of iris in iris image
	int IrisRadius;

    //Indicator of the focus level of the iris image, ranging from 0 to 100. High value of focusScore means clear iris image and
    //low value of focusScore means defocused iris image.
	int FocusScore; 

	//Percentage of the visible part of the iris., ranging from 0 to 100. 100 means no occlusion while 65 means 65 percent of the iris are occluded.
	int PercentVisible;

	// Confidence of the input iris image is obtained from spoof eye. 
	// 0 means the image is obtained from genuine eye, while 100 means the image is a spoof one.
	int SpoofValue; 

	// Indicator of the how much the image is interlaced. Higher value indicates the iris image is heavier interlaced.
	int InterlaceValue; 

	// The overall quality of the iris image, can be one of the following levels.
	//IR_IMAGE_QUALITY_BAD      
	//IR_IMAGE_QUALITY_POOR     
	//IR_IMAGE_QUALITY_FAIR     
	//IR_IMAGE_QUALITY_GOOD     
	//IR_IMAGE_QUALITY_EXCELLENT
	int	QualityLevel;

	//综合了focus，occlusion，blur等因素得出的最终质量分数
	int QualityScore;

	//Iris image feature template enrolled into database, with fixed length 512 bytes.
	unsigned char IrisEnrTemplate[g_constIKEnrFeatureLen];

	//Iris image feature template recogonized into database, with fixed length 1024 bytes.
	unsigned char IrisRecTemplate[g_constIKRecFeatureLen];

	//hancj建议不用的删掉
	//其他备用的扩展信息
	//数据结构中预留的信息为，为后期扩展使用.是应用程序和算法API的接口媒介
//	unsigned char reserveSpace[IK_Reserve_Size];
};

//虹膜摄像头采集到的图像尺寸信息
struct ImgSizeInfo
{
	int ImgHeight;	//图像高度
	int ImgWidth;	//图像宽度
	int ImgSize;	//图像尺寸
};

// 采集虹膜图像回调函数(funResult,pIrisImg,lrIrisClearFlag,lrIrisPositionFlag)
typedef int (*CBAlgApiCapIrisImg)(int , unsigned char* , LRIrisClearFlag , IrisPositionFlag);
//虹膜注册回调函数(funResult, enrIrisL, numEnrL, enrIrisR, numEnrR, lrFlag)
typedef int (*CBAlgApiEnroll)(int , APIIrisInfo* , int , APIIrisInfo* , int , LRSucFailFlag);
//虹膜识别回调函数(funResult, recIrisL,recIrisR,lrFlag,pLeftMatchOutputParam,pRightMatchOutputParam)
typedef int (*CBAlgApiIden)(int , APIIrisInfo* , APIIrisInfo* , LRSucFailFlag , APIMatchOutput* , APIMatchOutput* );

//前置申明
class CAPIBase;

//算法逻辑API类
class CAlgApi
{
public:
    CAlgApi(void);
    ~CAlgApi(void);

public:
	//获取设备获得的虹膜图像信息的尺寸信息
    int		GetImgInfo(OUT ImgSizeInfo *imgSizeInfo);
    //初始化算法逻辑API使用的相关资源
    int		Init();
    //释放算法逻辑API使用的相关资源
//    int 	Release(char *strFunRts);
	//使算法逻辑API进入休眠状态，不采集虹膜图像，但不释放相关资源
    int		Sleep();
	//使算法逻辑API从休眠状态唤醒，恢复正常工作状态
    int		Wake();

	//获取算法逻辑API控制参数
    int		GetParam(OUT AlgApiCtrlParam *paramStruct);
	//设置算法逻辑API控制参数
    int		SetParam(IN AlgApiCtrlParam *paramStruct);
	//同步方式采集虹膜图像
    int		SyncCapIrisImg(OUT unsigned char *pIrisImg, OUT LRIrisClearFlag &lrIrisClearFlag, OUT IrisPositionFlag &irisPositionFlag);
	//异步方式采集虹膜图像
    int     AsyncCapIrisImg(IN CBAlgApiCapIrisImg appCapIrisImg);

	//同步方式虹膜注册
    int		SyncStartEnroll(OUT APIIrisInfo *enrIrisL, IN int numEnrL, OUT APIIrisInfo *enrIrisR, IN int numEnrR, OUT LRSucFailFlag &lrFlag);
	//异步方式虹膜注册
    int		AsyncStartEnroll(IN CBAlgApiEnroll appEnroll, IN int numEnrL, IN int numEnrR);
	//停止虹膜注册
    int		StopEnroll();

	//同步方式虹膜识别
    int		SyncStartIden(IN unsigned char *codeListL, IN int codeNumL, IN unsigned char *codeListR, IN int codeNumR,
                      OUT APIIrisInfo *recIrisL, OUT APIIrisInfo *recIrisR, OUT LRSucFailFlag &lrFlag,
                      OUT APIMatchOutput *pLeftMatchOutputParam, OUT APIMatchOutput *pRightMatchOutputParam);
	//异步方式虹膜识别
   int		AsyncStartIden(IN CBAlgApiIden appIden, IN unsigned char *codeListL, IN int codeNumL, IN unsigned char *codeListR, IN int codeNumR);

	//停止虹膜识别
    int		StopIden();
	//虹膜识别特征模板比对
    int		Match(IN unsigned char *recCode, IN int recNum, IN unsigned char *codeList, IN int codeNum, OUT APIMatchOutput *pMatchOutputParam);

private:
    CAPIBase* _pCAPIBase;
//    std::tr1::shared_ptr<CAPIBase> _pCAPIBase;//API内部使用资源
    bool _initFlag; //是否初始化标志
	CAlgApi(const CAlgApi&);//禁止拷贝赋值
    CAlgApi& operator=(const CAlgApi&);
};

//摄像头API类，用于采集人脸图像，该部分推后实现
class CCameraApi
{
public:
    CCameraApi(void);
    ~CCameraApi(void);
public:
	//获取设备获得的人脸图像信息的尺寸信息
    int		GetImgInfo(OUT ImgSizeInfo *imgSizeInfo);
	//初始化采集人脸图像摄像头
    int		Init();
	//采集人脸图像
    int		CapImg(OUT unsigned char *pFaceImg);
	//释放采集人脸图像摄像头相关资源
    void	Release();
private:
//	Handle _pHandle;//类句柄
};
