/******************************************************************************************
** 文件名:   IKUSBSDK.h
**
** Copyright (c) 中科虹霸有限公司
** 创建人:   lizhl
** 日  期:   2016-06-23
** 修改人:
** 日  期:
** 描  述:   IKUSB设备SDK,提供设备初始化,虹膜图像采集,虹膜注册,虹膜识别等功能
**
** 版  本:   1.0.0
** 备  注:
**
*****************************************************************************************/
#pragma once

#ifndef _IKUSBSDK_H_
#define _IKUSBSDK_H_

#ifdef  IKUSBSDK_EXPORTS
#define  __IK_USBSDK_DLL_ __declspec(dllexport)
#else
#define  __IK_USBSDK_DLL_
#endif

#define __IK_STYLE_CALL
#define IN
#define OUT
#define INOUT


#define IKUSBSDK_100S_NO
#define IKUSBSDK_IKEMBX00
#define IKUSBSDK_OV580_NO
#define IKUSBSDK_OV580L_NO
#define IKUSBSDK_UVC_NO
#define IKUSBSDK_IMX132_NO
#define IKUSBSDK_AIO4060_NO

/**************************************
* 常量定义
***************************************/
//版本信息字符串长度
const int IK_VERSION_LENGTH = 260;
//设备ID字符串长度
const int IK_DEVICEID_LENGTH = 11;

//灰度图像BMP文件头大小
const int IK_BMP_BMP_HEADER_SIZE = 1078;

//预留空间长度
const int IK_RESERVEDINFO_LEN = 50;
const int IK_RESERVEDINFO_BLINK = 0;			//预留空间第0位与眨眼相关
const int IK_RESERVEDINFO_PANIC = 2;			//预留空间第2位与USB IO异常有关
const int IK_RESERVEDINFO_FACEPROGRESS = 1;		//预留空间第1位与眨眼相关

const int IK_RESERVEDPARA_LEN = 50;
const int IK_RESERVEDPARA_BLINK = 0;			//预留参数第0位表示是否需要眨眼
const int IK_RESERVEDPARA_I2FMODE = 1;			//预留参数第1位表示I2FMODE
const int IK_RESERVEDPARA_QUALITYMODE = 2;		//预留参数第2位表示质量模式

//原始图像尺寸大小
#if defined(IKUSBSDK_100S) || defined(IKUSBSDK_IKEMBX00) || defined(IKUSBSDK_AIO4060)
const int IK_IMG_HEIGHT = 960;
const int IK_IMG_WIDTH = 640;
const int IK_DISPLAY_IMG_HEIGHT = 480;			//回调函数中,所得到图像的高
const int IK_DISPLAY_IMG_WIDTH = 640;			//回调函数中,所得到图像的宽
const int IK_DISPLAY_IMG_SIZE = IK_DISPLAY_IMG_HEIGHT * IK_DISPLAY_IMG_WIDTH;
#else
const int IK_IMG_HEIGHT = 1080;
const int IK_IMG_WIDTH = 1920;
const int IK_DISPLAY_IMG_HEIGHT = 270;			//回调函数中,所得到图像的高
const int IK_DISPLAY_IMG_WIDTH = 480;			//回调函数中,所得到图像的宽
const int IK_DISPLAY_IMG_SIZE = IK_DISPLAY_IMG_HEIGHT * IK_DISPLAY_IMG_WIDTH;
#endif
const int IK_IMG_SIZE = IK_IMG_HEIGHT * IK_IMG_WIDTH;

//虹膜特征尺寸大小
const int IK_ENROLL_FEATURE_SIZE = 512;
const int IK_IDEN_FEATURE_SIZE = 1024;

//虹膜图像尺寸大小
const int IK_IRIS_IMG_HEIGHT = 480;
const int IK_IRIS_IMG_WIDTH = 640;
const long IK_IRIS_IMG_SIZE = IK_IRIS_IMG_HEIGHT * IK_IRIS_IMG_WIDTH;

//人脸图像尺寸大小
const long IK_FACE_IMG_SIZE = IK_IRIS_IMG_SIZE;
//人脸特征尺寸大小
const int IK_FACE_FEATURE_SIZE = 800;
const int IK_FACE_POS_SIZE = 4;

//最大匹配特征数量
const int IK_MAX_NUM_OF_MATCH = 100;

//最小注册时间,单位:秒,下同
const int IK_MIN_ENR_TIME = 0;
//最大注册时间,超过该时间超时退出
const int IK_MAX_ENR_TIME = 120;
//最小识别时间
const int IK_MIN_REC_TIME = 0;
//最大识别时间,超过该时间超时退出
const int IK_MAX_REC_TIME = 10;

//函数调用错误码
const int E_USBSDK_ERROR_BASE = 0;     //基础错误码

//与系统相关的定义，以1***开头
const int E_USBSDK_INIT_FAILED = E_USBSDK_ERROR_BASE - 1001;          //SDK初始化失败或者尚未初始化
const int E_USBSDK_HANDLE_ERROR = E_USBSDK_ERROR_BASE - 1002;         //在与Handle相关的函数中失败
const int E_USBSDK_INVALID_DEV_STATUS = E_USBSDK_ERROR_BASE - 1003;   //设备状态出错,设备初始化失败或停止工作或设备被拔出
const int E_USBSDK_INVALID_LICENSE = E_USBSDK_ERROR_BASE - 1004;	  //无效的授权,授权过期,需要申请新的授权SDK
const int E_USBSDK_READ_DEVICEID_ERROR = E_USBSDK_ERROR_BASE - 1005;  //读取设备ID出错

const int E_USBSDK_E_OPEN_SERAIL_FAILED = E_USBSDK_ERROR_BASE - 1006;  /*串口打开失败*/
const int E_USBSDK_E_DEVICE_NO_OPEN   = E_USBSDK_ERROR_BASE - 1007;  /*设备未打开*/
const int E_USBSDK_E_NO_ENDPOINT      = E_USBSDK_ERROR_BASE - 1008;  /*端点不存在*/
const int E_USBSDK_E_NO_DEVICE_HANDLE = E_USBSDK_ERROR_BASE - 1009;  /*设备句柄不存在*/
const int E_USBSDK_E_NO_TRANSFER      = E_USBSDK_ERROR_BASE - 1010;  /*传输句柄不存在*/
const int E_USBSDK_E_SUBMIT           = E_USBSDK_ERROR_BASE - 1011;  /*传输提交错误*/
const int E_USBSDK_E_NO_CONTEXT       = E_USBSDK_ERROR_BASE - 1012;  /*设备上下文为获取错误*/
const int E_USBSDK_E_CLAIM_INTERFACE  = E_USBSDK_ERROR_BASE - 1013;  /*设置端点接口错误*/
const int E_USBSDK_E_NO_TRANSFER_ERROR  = E_USBSDK_ERROR_BASE - 1014; /*传输设备无法获取数据*/
const int E_USBSDK_E_TIME_OVER  = E_USBSDK_ERROR_BASE -  1015; 	     /*USB传输超时*/
const int E_USBSDK_E_OPEN_CAMERA_FAILED = E_USBSDK_ERROR_BASE - 1016; /*虹膜摄像头打开失败*/

//与注册或识别算法逻辑相关的以21**开头
const int E_USBSDK_ENR_IDEN_FAILED = E_USBSDK_ERROR_BASE - 2101;      //注册、识别、获取虹膜特征信息失败
const int E_USBSDK_FAIL2STOP = E_USBSDK_ERROR_BASE - 2102;            //调用停止注册、停止识别、停止获取虹膜特征信息函数时失败
const int E_USBSDK_ALGORITHM_CONFLICT = E_USBSDK_ERROR_BASE - 2103;   //算法冲突，注册、识别、获取虹膜特征信息不能同时进行
const int E_USBSDK_INVALID_ENR_INPUT = E_USBSDK_ERROR_BASE - 2104;    //注册、获取虹膜特征信息时无效的输入,如图像数目，时间，EyeMode等
const int E_USBSDK_INVALID_REC_INPUT = E_USBSDK_ERROR_BASE - 2105;    //识别时无效的输入,如识别时间，EyeMode等
const int E_USBSDK_INVALID_MATCH_INPUT = E_USBSDK_ERROR_BASE - 2106;  //比对时无效的输入
const int E_USBSDK_IDEN_OVERTIME = E_USBSDK_ERROR_BASE - 2107;        //识别时overtime
const int E_USBSDK_IDEN_UNSUPPORT_FEATURE_NUM = E_USBSDK_ERROR_BASE - 2108;        //识别时输入特征数目超过授权数目
const int E_USBSDK_IDEN_NO_FEATURE = E_USBSDK_ERROR_BASE - 2109;        //识别时没有特征

//特别的，当人为调用停止注册、识别、获取虹膜特征信息函数时，之前调用的注册、识别、获取虹膜特征信息函数将返回1
const int E_USBSDK_ENR_IDEN_INTERRUPT = 1;

//与比对算法逻辑相关的以22**开头
const int E_USBSDK_MATCH_FAILED = E_USBSDK_ERROR_BASE - 2201;      //比对失败

//与采图相关的以23**开头
const int E_USBSDK_CAP_IMG_FAILED = E_USBSDK_ERROR_BASE - 2301;   //图像采集失败
const int E_USBSDK_CAP_IMG_INVALID = E_USBSDK_ERROR_BASE - 2302;   //所采集图像不是虹霸授权的图像

//与算法控制参数相关的以30**开头，
const int E_USBSDK_INVALID_IRISMODE = E_USBSDK_ERROR_BASE - 3001;  //无效的irismode
const int E_USBSDK_INVALID_TIME = E_USBSDK_ERROR_BASE - 3002;     //无效的注册时间或识别时间
const int E_USBSDK_INVALID_FINDMODE = E_USBSDK_ERROR_BASE - 3003;         //无效的FindMode
const int E_USBSDK_INVALID_IFSPOOFDETECT = E_USBSDK_ERROR_BASE - 3004;    //无效的IfSpoofDetect
const int E_USBSDK_INVALID_PARAMETER = E_USBSDK_ERROR_BASE - 3005;        //无效的算法控制参数

//与设备相关
const int E_USBSDK_CAPTURER_INVALID = E_USBSDK_ERROR_BASE - 4001;
const int E_USBSDK_OPEN_CAPTURER_FAILED = E_USBSDK_ERROR_BASE - 4002;

//函数调用通用返回值
const int E_USBSDK_OK = 0;                                       //函数调用成功，并且完成了函数功能
const int E_USBSDK_DEFAULT_ERROR = E_USBSDK_ERROR_BASE - 8000;    //函数调用缺省错误，一般为函数调用结果初始值，一般不可能返回该值

//工作模式，表示空闲/注册/识别
enum IKWorkMode
{
	IKWorkModeIdle =	-1,		//空闲模式，即未处于注册或者识别工作模式
	IKWorkModeEnroll =	0,		//注册模式
	IKWorkModeIden =	1,		//识别模式
};

//iris模式,表示虹膜注册/识别时,需要哪只眼睛进行注册/识别,如任意眼,左眼,右眼,双眼
enum IKIrisMode
{
	IrisModeNone = -1,	//无效
	IrisModeUndef = 0,	//任意眼
	IrisModeLeft = 1,	//左眼
	IrisModeRight = 2,	//右眼
	IrisModeBoth = 3,	//双眼
};

//模态模式,表示识别时,需要虹膜,人脸或虹膜+人脸进行识别
enum IKI2FMode
{
	I2FModeNone = - 1,	//无效
	I2FModeUndef = 0,	//未知,虹膜或人脸
	I2FModeIris = 1,	//仅虹膜 
	I2FModeFace = 2,	//仅人脸
	I2FModeBoth = 3		//虹膜+人脸	
};

//距离提示
enum IKIrisDistFlag
{
	DistTooFar = -1,
	DistSuitable = 0,
	DistTooClose = 1,
	DistUnknown = 100,
};

//注册、识别、获取特征时的成功,失败提示
enum IKResultFlag
{
	RecFaceBothIrisFailed = -7,	//识别时,人脸+双眼识别失败
	RecFaceRightIrisFailed = -6,//识别时,人脸+右眼识别失败	
	RecFaceLeftIrisFailed = -5,	//识别时,人脸+左眼识别失败
	RecFaceFailed = -4,			//识别时,人脸识别失败

	EnrRecBothFailed = -3,		//注册、识别、获取特征时左右眼均注册或识别失败
	EnrRecRightFailed = -2,		//右眼注册、识别、获取特征失败导致注册、识别、获取特征失败,左眼情况未知
	EnrRecLeftFailed = -1,		//左眼注册、识别、获取特征失败导致注册、识别、获取特征失败,右眼情况未知

	EnrRecUnknown = 0,			//注册、识别、获取特征结果未知

	EnrRecLeftSuccess = 1,		//注册、识别、获取特征时仅左眼注册、识别、获取特征成功
	EnrRecRightSuccess = 2,     //注册、识别、获取特征时仅右眼注册、识别、获取特征成功
	EnrRecBothSuccess = 3,      //注册、识别、获取特征时左右眼均注册、识别、获取特征成功

	RecFaceSuccess = 4,			//识别时,仅人脸识别成功
	RecFaceLeftIrisSuccess = 5,	//识别时,人脸+左眼识别成功
	RecFaceRightIrisSuccess = 6,//识别时,人脸+右眼识别成功
	RecFaceBothIrisSuccess = 7	//识别时,人脸+双眼识别成功
};

//UIUE提示信息，注释中为建议提示用语
enum IKUIUEMsgID {
	UIUE_MsgID_SUITABLE = 0,								//正在扫描，请保持稳定
	UIUE_MsgID_BAD_IMAGE_QUALITY = 1 << 0,					//请将双眼对准取景框
	UIUE_MsgID_EYE_NOT_FOUND = 1 << 1,						//请将双眼对准取景框
	UIUE_MsgID_EYE_TOO_CLOSE = 1 << 2,						//请离远一点
	UIUE_MsgID_EYE_TOO_FAR = 1 << 3,						//请靠近一点
	UIUE_MsgID_EYE_TOO_UP = 1 << 4,							//请将双眼对准取景框
	UIUE_MsgID_EYE_TOO_DOWN = 1 << 5,						//请将双眼对准取景框
	UIUE_MsgID_EYE_TOO_LEFT = 1 << 6,						//请将双眼对准取景框
	UIUE_MsgID_EYE_TOO_RIGHT = 1 << 7,						//请将双眼对准取景框
	UIUE_MsgID_MOTION_BLUR = 1 << 8,						//正在扫描，请保持稳定
	UIUE_MsgID_FOCUS_BLUR = 1 << 9,							//正在扫描，请保持稳定
	UIUE_MsgID_BAD_EYE_OPENNESS = 1 << 10,					//请睁大眼睛或调整下角度
	UIUE_MsgID_WITH_GLASS = 1 << 14,						//尝试调整下角度或摘掉眼镜
	UIUE_MsgID_OUTDOOR = 1 << 15,							//请离开室外强光环境或者请在室内环境中使用
	UIUE_MsgID_ATTACK = 1 << 16,							//请停止恶意攻击(建议不提示)
	UIUE_MsgID_BLINK = 1 << 17,								//请眨眼
	UIUE_MsgID_UNAUTHORIZED_ATTACK = 1 << 18,				//如果您是非注册用户，请停止使用(建议不提示)
};

//虹膜位置信息
typedef struct tagIKIrisPosStruct
{
	struct{
		int xPixel;
		int yPixel;
		int radius;
	}leftIrisPos, rightIrisPos;
	int dist;  //虹膜与摄像头之间的估算距离
}IKIrisPos;

//虹膜算法运行过程中的状态信息
typedef struct tagIKEnrIdenStatusStruct
{
	int workMode;			//工作模式,识别还是注册
	int irisMode;			//虹膜的图像类型：左眼、右眼、左右眼、任意眼、None
	int leftIrisProgress;			//左眼进度
	int rightIrisProgress;			//右眼进度

	IKUIUEMsgID uiueID;	//UIUE状态ID
	IKIrisPos irisPos;		//虹膜位置信息
	IKIrisDistFlag distEstimation;	//表示当前距离是否合适，远、近、合适、未知	
	unsigned char imageData[IK_DISPLAY_IMG_SIZE];//算法返回的图像(注：并不一定是原始采集的图像)
	int reservedInfo[IK_RESERVEDINFO_LEN]; //预留信息
}IKEnrIdenStatus, *pIKEnrIdenStatus;

//算法控制参数设置
typedef struct tagIKAlgConfigStruct
{
	int minEnrIdenTime;		//最小注册、获取特征时间,识别不需要设置,即使设置也不会生效
	int maxEnrIdenTime;		//最大注册、识别、获取特征时间,超过该时间还没有注册、识别、获取特征成功,注册、识别、获取特征超时退出
	int reservedPara[IK_RESERVEDPARA_LEN]; //预留控制参数
}IKAlgConfigStruct, *pIKAlgConfigStruct;

//SDK虹膜图像结构信息
typedef struct tagIKSDKIrisInfoStruct
{
	//Height of the iris image in pixels. It must be fixed to 480 currently.
	int ImgHeight;

	//Width of the iris image in pixels. It must be fixed to 640 currently.
	int ImgWidth;

	//虹膜图像类型，左眼、右眼OR未知
	int ImgType;

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

	//Saturation level of the iris image.
	int saturation;

	// Indicator of the how much the image is interlaced. Higher value indicates the iris image is heavier interlaced.
	int InterlaceValue;

	//综合了focus，occlusion，blur等因素得出的最终质量分数
	int QualityScore;

	//1D array of pixel intensity values of the original iris image.
	unsigned char imgData[IK_IRIS_IMG_SIZE];

	//Iris image feature template enrolled into database, with fixed length 512 bytes.
	unsigned char irisEnrTemplate[IK_ENROLL_FEATURE_SIZE];

	//Iris image feature template of recognition image, with fixed length 1024 bytes.
	unsigned char irisRecTemplate[IK_IDEN_FEATURE_SIZE];

}IKSDKIrisInfo;

//IK Cloud 虹膜图像结构信息,与IKSDKIrisInfo的区别是图像是jpeg格式的，便于网络传输
//保存原始图像中存储的图像信息
const int  IKCLOUD_UNKNOWN_INFO_LEN = 100;
typedef struct tagIKCloudIrisInfoStruct
{
	//Height of the iris image in pixels. It must be fixed to 480 currently.
	int ImgHeight;

	//Width of the iris image in pixels. It must be fixed to 640 currently.
	int ImgWidth;

	//虹膜图像类型，左眼、右眼OR未知
	int ImgType;

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

	//Saturation level of the iris image.
	int saturation;

	// Indicator of the how much the image is interlaced. Higher value indicates the iris image is heavier interlaced.
	int InterlaceValue;

	//综合了focus，occlusion，blur等因素得出的最终质量分数
	int QualityScore;

	//length of the jpeg image of the original iris image.
	int jpegImSize;

	//1D array of jpeg image of the original iris image.
	unsigned char imgData[IK_IRIS_IMG_SIZE];

	//1D array of iris unknown info
	unsigned char unknownInfo[IKCLOUD_UNKNOWN_INFO_LEN];

	//Iris image feature template enrolled into database, with fixed length 512 bytes.
	unsigned char irisEnrTemplate[IK_ENROLL_FEATURE_SIZE];

	//Iris image feature template of recognition image, with fixed length 1024 bytes.
	unsigned char irisRecTemplate[IK_IDEN_FEATURE_SIZE];

}IKCloudIrisInfo;


typedef struct tagIKSDKFaceInfoStruct
{
	int imageH;	//Height of the face image in pixels. It must be fixed to 480 currently.
	int imageW;	//Width of the face image in pixels. It must be fixed to 640 currently.
	int occlusionScore;	//occlusion score of the face
	int saturation;	//Saturation level of the iris image.
	int focusScore; //Focus level of the iris image.
	int eyePos[IK_FACE_POS_SIZE];	//eye postiongs
	int rotation1;
	int rotation2;
	int rotation3;
	int captureDist;
	int qualityScore;// The overall quality of the face image, can be one of the following levels.
	unsigned char imageData[IK_FACE_IMG_SIZE];	//1D array of pixel intensity values of the original face image
	unsigned char faceTemplate[IK_FACE_FEATURE_SIZE];	//Iris image feature template enrolled into database, with fixed length 800 bytes.
} IKSDKFaceInfo;

//识别输出信息,对用户可见
typedef struct tagIKSDKMatchOutputStruct
{
	//The number of matched iris templates in database.
	int MatchNum;

	//Index of matched template in the enrolled template list. 
	long MatchIndex[IK_MAX_NUM_OF_MATCH];
}IKSDKMatchOutput;

/*****************************************************************************
*                         回调函数
*  函 数 名：
*  功    能：
*  说    明：用户在应用程序端实现本回调函数,SDK将相关信息通过该回调函数传递给上层应用
*  参    数：IKEnrIdenStatus：输出参数，注册、识别、获取特征过程中的状态信息
*
*  返 回 值：
*  创 建 人：lizhl
*  创建时间：2016-06-24
*  修 改 人：
*  修改时间：
*****************************************************************************/
typedef void (__IK_STYLE_CALL *IKUSBSDK_IrisStatusInfoCallback)(IKEnrIdenStatus* enrIdenStatus);

/*****************************************************************************
*                         获取版本号
*  函 数 名：IKUSBSDK_GetVersion
*  功    能：获取版本号
*  说    明：即使不调用设备初始化,也可以调用本方法获取版本号
*  参    数：version：输出参数，由用户分配空间，长度260(IK_VERSION_LENGTH)
*
*  返 回 值：NULL
*  创 建 人：lizhl
*  创建时间：2016-06-24
*  修 改 人：
*  修改时间：
*****************************************************************************/
__IK_USBSDK_DLL_ void __IK_STYLE_CALL IKUSBSDK_GetVersion(char* version);

/*****************************************************************************
*                         获取设备ID
*  函 数 名：IKUSBSDK_GetDeviceID
*  功    能：获取设备ID
*  说    明：调用设备初始化后,才能调用该函数
*  参    数：deviceID：输出参数，由用户分配空间，长度260(IK_VERSION_LENGTH)
*
*  返 回 值：0：获取成功
*			 <0：获取失败，根据返回值可以查询失败原因,详见函数调用错误码
*  创 建 人：lizhl
*  创建时间：2016-09-14
*  修 改 人：
*  修改时间：
*****************************************************************************/
__IK_USBSDK_DLL_ int __IK_STYLE_CALL IKUSBSDK_GetDeviceID(char* deviceID);

/*****************************************************************************
*                         设备资源初始化
*  函 数 名：IKUSBSDK_Init
*  功    能：初始化设备及使用的相关资源
*  说    明：
*  参    数：statusInfoCB：输入参数，工作状态信息的回调
*
*  返 回 值：0：初始化成功
*			 <0：初始化失败,根据返回值可以查询失败原因,详见函数调用错误码
*  创 建 人：lizhl
*  创建时间：2016-06-24
*  修 改 人：
*  修改时间：
*****************************************************************************/
__IK_USBSDK_DLL_ int __IK_STYLE_CALL IKUSBSDK_Init(IN IKUSBSDK_IrisStatusInfoCallback statusInfoCB);

/*****************************************************************************
*                         算法逻辑API释放
*  函 数 名：IKUSBSDK_Release
*  功    能：释放设备及相关资源
*  说    明：
*  参    数：
*
*  返 回 值：0：释放成功
*			 <0：释放失败,根据返回值可以查询失败原因,详见函数调用错误码
*  创 建 人：lizhl
*  创建时间：2016-06-24
*  修 改 人：
*  修改时间：
*****************************************************************************/
__IK_USBSDK_DLL_ int __IK_STYLE_CALL IKUSBSDK_Release();

/*****************************************************************************
*                         开始虹膜注册
*  函 数 名：IKUSBSDK_StartIrisEnroll
*  功    能：虹膜注册
*  说    明：
*  参    数：enrIrisL，enrIrisR：输出参数，注册得到的左眼、右眼虹膜图像及相关信息
*			 numEnrL，numEnrR：输入参数，注册得到的左眼、右眼虹膜图像个数，由调用者指定,只能是3,6,9中的一个,推荐使用9,
*								如果注册过程较慢或者有网络传输/存储方面限制,可以适当改小
*			 irisMode：输入参数，指定需要指定哪只眼睛进行注册
*			 resultFlag：输出参数，左眼、右眼虹膜注册成功标志
*			 algConfig：输入参数，算法控制参数
*
*
*  返 回 值：0：注册成功
*			 1：人为停止注册
*    		<0：注册失败,根据返回值可以查询失败原因,详见函数调用错误码
*  创 建 人：lizhl
*  创建时间：2016-06-24
*  修 改 人：
*  修改时间：
*****************************************************************************/
__IK_USBSDK_DLL_ int __IK_STYLE_CALL IKUSBSDK_StartIrisEnroll(OUT IKSDKIrisInfo *enrIrisL, IN int numEnrL, OUT IKSDKIrisInfo *enrIrisR, IN int numEnrR, IN IKIrisMode irisMode, OUT IKResultFlag &resultFlag, IN IKAlgConfigStruct *algConfig = NULL);

/*****************************************************************************
*                         开始人脸注册
*  函 数 名：IKUSBSDK_StartFaceEnroll
*  功    能：人脸注册
*  说    明：
*  参    数：enrFace：输出参数，注册得到的人脸图像及相关信息
*			 numEnr：输入参数，注册得到的人脸图像个数，由调用者指定
*			 algConfig：输入参数，算法控制参数
*
*
*  返 回 值：0：注册成功
*			 1：人为停止注册
*    		<0：注册失败,根据返回值可以查询失败原因,详见函数调用错误码
*  创 建 人：lizhl
*  创建时间：2016-06-24
*  修 改 人：
*  修改时间：
*****************************************************************************/
__IK_USBSDK_DLL_ int   __IK_STYLE_CALL  IKUSBSDK_StartFaceEnroll(OUT IKSDKFaceInfo *enrFace, IN int numEnr, IN IKAlgConfigStruct *algConfig = NULL);

/*****************************************************************************
*                         停止注册
*  函 数 名：IKUSBSDK_StopEnroll
*  功    能：停止虹膜/人脸注册
*  说    明：
*  参    数：
*
*  返 回 值：0：停止注册成功
*    		<0：停止注册失败,根据返回值可以查询失败原因,详见函数调用错误码
*  创 建 人：lizhl
*  创建时间：2016-06-24
*  修 改 人：
*  修改时间：
*****************************************************************************/
__IK_USBSDK_DLL_ int __IK_STYLE_CALL IKUSBSDK_StopEnroll();

/*****************************************************************************
*                         开始虹膜识别
*  函 数 名：IKUSBSDK_StartIrisIden
*  功    能：虹膜识别
*  说    明：
*  参    数：irisMode：输入参数，指定需要指定哪只眼睛进行注册
*			 pCodeListL，pCodeListR：输入参数，左眼、右眼虹膜注册特征
*			 codeNumL，codeNumR：输入参数，左眼、右眼虹膜注册特征个数,单眼特征数超过授权数目无法使用,
*									如需支持更多特征,请与中科虹霸联系
*			 pMatchOutputL，pMatchOutputR：输出参数，左眼、右眼识别输出信息
*			 resultFlag：输出参数，左眼、右眼虹膜注册成功标志
*			 pAlgConfig：输入参数，算法控制参数
*
*
*  返 回 值：0：识别成功
*			 1：人为停止识别
*    		<0：识别失败,根据返回值可以查询失败原因,详见函数调用错误码
*  创 建 人：lizhl
*  创建时间：2016-06-24
*  修 改 人：
*  修改时间：
*****************************************************************************/
__IK_USBSDK_DLL_ int __IK_STYLE_CALL IKUSBSDK_StartIrisIden(IN IKIrisMode irisMode, IN unsigned char *pCodeListL, IN int codeNumL, OUT IKSDKMatchOutput *pMatchOutputL, 
	IN unsigned char *pCodeListR, IN int codeNumR, OUT IKSDKMatchOutput *pMatchOutputR, OUT IKResultFlag &resultFlag, IN IKAlgConfigStruct *pAlgConfig);

/*****************************************************************************
*                         开始识别
*  函 数 名：IKUSBSDK_StartIden
*  功    能：虹膜/人脸识别
*  说    明：
*  参    数：irisMode：输入参数，指定需要指定哪只眼睛进行注册
*			 pCodeListL，pCodeListR，pCodeListF：输入参数，左眼虹膜、右眼虹膜、人脸注册特征
*			 codeNumL，codeNumR，codeNumF：输入参数，左眼虹膜、右眼虹膜、人脸注册特征个数
*			 pMatchOutputL，pMatchOutputR，pMatchOutputF：输出参数，左眼、右眼、人脸识别输出信息
*			 resultFlag：输出参数，左眼、右眼虹膜注册成功标志
*			 pAlgConfig：输入参数，算法控制参数
*
*
*  返 回 值：0：识别成功
*			 1：人为停止识别
*    		<0：识别失败,根据返回值可以查询失败原因,详见函数调用错误码
*  创 建 人：lizhl
*  创建时间：2016-06-24
*  修 改 人：
*  修改时间：
*****************************************************************************/
__IK_USBSDK_DLL_ int __IK_STYLE_CALL IKUSBSDK_StartIden(IN IKIrisMode irisMode, IN unsigned char *pCodeListL, IN int codeNumL, OUT IKSDKMatchOutput *pMatchOutputL,
	IN unsigned char *pCodeListR, IN int codeNumR, OUT IKSDKMatchOutput *pMatchOutputR, IN unsigned char *pCodeListF, IN int codeNumF, OUT IKSDKMatchOutput *pMatchOutputF,
	OUT IKResultFlag &resultFlag, IN IKAlgConfigStruct *algConfig);

/*****************************************************************************
*                         停止识别
*  函 数 名：IKUSBSDK_StopIden
*  功    能：停止虹膜识别
*  说    明：
*  参    数：
*
*  返 回 值：0：停止识别成功
*    		<0：停止识别失败,根据返回值可以查询失败原因,详见函数调用错误码
*  创 建 人：lizhl
*  创建时间：2016-06-24
*  修 改 人：
*  修改时间：
*****************************************************************************/
__IK_USBSDK_DLL_ int __IK_STYLE_CALL IKUSBSDK_StopIden();

/*****************************************************************************
*                         获取虹膜特征信息
*  函 数 名：IKUSBSDK_StartGetIrisInfo
*  功    能：获取虹膜特征信息
*  说    明：
*  参    数：irisInfoL，irisInfoR：输出参数，获取到的左眼、右眼虹膜图像及相关信息
*			 numL，numR：输入参数，需要获取的左眼、右眼虹膜图像个数，由调用者指定,只能是3,6,9中的一个,推荐使用3
*			 irisMode：输入参数，指定需要获取哪只眼睛的虹膜信息
*			 resultFlag：输出参数，左眼、右眼虹膜获取成功标志
*			 algConfig：输入参数，算法控制参数
*
*
*  返 回 值：0：获取虹膜特征信息成功
*			 1：人为停止获取虹膜特征信息
*    		<0：获取虹膜特征信息失败,根据返回值可以查询失败原因,详见函数调用错误码
*  创 建 人：lizhl
*  创建时间：2016-08-23
*  修 改 人：
*  修改时间：
*****************************************************************************/
__IK_USBSDK_DLL_ int __IK_STYLE_CALL IKUSBSDK_StartGetIrisInfo(OUT IKSDKIrisInfo *irisInfoL, IN int numL, OUT IKSDKIrisInfo *irisInfoR, IN int numR, IN IKIrisMode irisMode, OUT IKResultFlag &resultFlag, IN IKAlgConfigStruct *algConfig);

/*****************************************************************************
*                         停止获取虹膜特征信息
*  函 数 名：IKUSBSDK_StopGetIrisInfo
*  功    能：停止获取虹膜特征信息
*  说    明：
*  参    数：
*
*  返 回 值：0：停止获取虹膜特征信息成功
*    		<0：停止取虹膜特征信息失败,根据返回值可以查询失败原因,详见函数调用错误码
*  创 建 人：lizhl
*  创建时间：2016-08-23
*  修 改 人：
*  修改时间：
*****************************************************************************/
__IK_USBSDK_DLL_ int __IK_STYLE_CALL IKUSBSDK_StopGetIrisInfo();


/*****************************************************************************
*                         IKSDKIrisInfo2IKCloudIrisInfo
*  函 数 名：IKSDKIrisInfo2IKCloudIrisInfo
*  功    能：将SDKIrisInfo转换成CloudIrisInfo
*  说    明：
*  参    数：
*
*  返 回 值：0：转换成功
*    		<0：转换失败,根据返回值可以查询失败原因,详见函数调用错误码
*  创 建 人：hezhf
*  创建时间：2016-11-21
*  修 改 人：caotao
*  修改时间：2016-12-1
*****************************************************************************/
__IK_USBSDK_DLL_ int __IK_STYLE_CALL IKUSBSDK_SDKIrisInfo2CloudIrisInfo(IKSDKIrisInfo* sdkIrisInfo, IKCloudIrisInfo* cloudIrisInfo);

/*****************************************************************************
*                         IKCloudIrisInfo2IKSDKIrisInfo
*  函 数 名：IKCloudIrisInfo2IKSDKIrisInfo
*  功    能：将CloudIrisInfo转换成SDKIrisInfo
*  说    明：
*  参    数：
*
*  返 回 值：0：转换成功
*    		<0：转换失败,根据返回值可以查询失败原因,详见函数调用错误码
*  创 建 人：hezhf
*  创建时间：2016-11-21
*  修 改 人：caotao
*  修改时间：2016-12-1
*****************************************************************************/
__IK_USBSDK_DLL_ int __IK_STYLE_CALL IKUSBSDK_CloudIrisInfo2SDKIrisInfo(IKCloudIrisInfo* cloudIrisInfo, IKSDKIrisInfo* sdkIrisInfo);

#endif

