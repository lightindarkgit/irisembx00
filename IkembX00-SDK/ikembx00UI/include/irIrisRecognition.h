#ifndef _CASIA_GEN_H
#define _CASIA_GEN_H
//#include "ikIrisKing.h"
//StarLee：在编译库之前，定义平台的类型
#define IKALG_PLATFORM_LINUX_NO
#define IKALG_PLATFORM_ANDROID_NO
#define IKALG_PLATFORM_WINDOWS_NO
#define IKALG_VERSION_X64_NO




#define Setting_FeatureCode2Mask_NO//如果定义该宏，表示特征码中包含template和mask
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//Different template lengths in Byte 
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
#ifdef Setting_FeatureCode2Mask
	#define IR_ENR_FEATURE_LENGTH		1000	//length of enroll code+mask  
	#define IR_REC_FEATURE_LENGTH		1536    //length of recog code+mask 
#else
	#define IR_ENR_FEATURE_LENGTH		512	//length of enroll code+mask  
	#define IR_REC_FEATURE_LENGTH		1024    //length of recog code+mask 
#endif




//////////////////////////////////////////////////////////////////////////
//以下为IKIrisKing的宏定义,与IKALGO_API_SEPRATE_MODULE相关
//////////////////////////////////////////////////////////////////////////
#ifndef IKALGO_API_SEPRATE_MODULE

//对外接口函数格式
#ifdef IKALG_PLATFORM_WINDOWS
#	ifdef IK_ALGO_EXPORTS
#		define IK_ALGO_API extern "C" __declspec(dllexport)
#	else
#		define IK_ALGO_API extern "C" __declspec(dllimport)
#	endif
#else
#		define IK_ALGO_API extern "C"
#endif
//////////////////////////////////////////////////////////////////////////
//Type of image
//////////////////////////////////////////////////////////////////////////
#define IR_IM_EYE_UNDEF					0//Unknown left or right
#define IR_IM_EYE_LEFT					1//Left image
#define IR_IM_EYE_RIGHT					2//Right image
#define IR_IM_EYE_BOTH					3//get both left and right eye
#define IR_IM_EYE_NONE					-1//NO iris image is presented
#define IR_IM_Face						4//人脸图形

//////////////////////////////////////////////////////////////////////////
//Iris image size setting
//////////////////////////////////////////////////////////////////////////
#define IR_IMAGE_HEIGHT					480
#define IR_IMAGE_WIDTH					640
#define IR_IMAGE_SISE					307200

//图像中个别信息的位置记录
#define IR_ImIndexPos					2


//////////////////////////////////////////////////////////////////////////
//Type of device
//////////////////////////////////////////////////////////////////////////
#define IKInvalidDevice			0	//Default
#define IK220A					1
#define IKX00					2
#define IKUSB100s				3	//IKUSB：USB 100s的设备类型号
#define IKUSBUVC				4	//IKUSBUVC：USB-UVC项目的设备类型号
#define	IKUSBAIO				5	//USB一体（All-in-One）,建行OCX
#define	IKUSBOV580				6	//OV580
#define IKUSBAIO4060			7	//USB一体（All-in-One）,金融解决方案，FOV缩小
#define	IKUSBOV580L				8	//61所演示用检测及距离查找表
#define IKMCASIAV4Lamp			9	//针对CASIA_V4 Lamp database
#define IKMUSB132_810			60	//使用IMX132+810LED
#define IKMLenovoMixx			66  //联想pad平板
#define IKMJiaotu_tc			67	//展讯手机
#define IKMHuaying6580			68	//20160317领创手机设备类型号
#define IKMJiaotu_tcIndia		69	//20160317展讯手机印度应用
#define IKMQCQIKU				70	//20160428奇酷手机
#define IKMSIMDATA				71	//20160707达闼手机

//用于性能测试的宏定义
#define IK_MatchPerfBinsNum          1001		//统计confidence and ROC 时bin num




#endif //IKALGO_API_SEPRATE_MODULE
//////////////////////////////////////////////////////////////////////////
//以上为IKIrisKing的宏定义，与IKALGO_API_SEPRATE_MODULE相关
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
//以下为idIrisDetection的宏定义
//////////////////////////////////////////////////////////////////////////
#ifndef IKALGO_API_SEPRATE_MODULE
////用于基于眼睛检测的距离提示
//#define IR_Dist_Too_Far					-2	//距离太远
//#define IR_Dist_ABit_Far				-1	//距离略远
//#define IR_Dist_Good					0	//距离合适
//#define IR_Dist_ABit_Near				1	//距离略近
//#define IR_Dist_Near					2	//距离太近
//人眼检测方法选择宏定义
#define DETECT_EYE_UPTODOWN_SEARCH//_NO
#define DETECT_ALGO_ROI_DET_NO//
#define DETECT_EYE_OUTDOOR//_NO
#define	DETECT_EYE_SMALL_BLOCK//_NO
//人脸图像饱和度宏定义
#define DETIM_SATUATION_HIGH_Th			200
#define DETIM_SATUATION_LOW_Th			20
//眼睛ROI选取的比率
#define DETEYE_ROI_RATIO				0.8f
//无效的眼睛检测距离，如果无效，表示检测失败
#define IR_Invalid_ID_Dist				-1
//眼睛检测结果信息结构体
#define IR_VIDEO_HIST_INFO_LEN			50	//Inf vector len of video stream information
//#define IR_VIDEO_USER_DEVICE_DIS				48
#define IR_VIDEO_WROK_MODE				0	//眼睛检测的工作模式
#define IR_VIDEO_GLASS_EYEMED			48
#define IR_VIDEO_ORG_L_ROW				47	
#define IR_VIDEO_ORG_L_COL				46
#define IR_VIDEO_ORG_R_ROW				45
#define IR_VIDEO_ORG_R_COL				44
#define IR_VIDEO_KP_FLAG				1
#define IR_VIDEO_KP_L01R				2
#define IR_VIDEO_KP_L01C				(IR_VIDEO_KP_L01R+KD_POINTNUM)//=9
#define IR_VIDEO_KP_R01R				(IR_VIDEO_KP_L01C+KD_POINTNUM)//=16
#define IR_VIDEO_KP_R01C				(IR_VIDEO_KP_R01R+KD_POINTNUM)//=23
#define IR_VIDEO_KP_LOPEN				30	//左眼睁开程度
#define IR_VIDEO_KP_ROPEN				31	//右眼睁开程度
#define IR_VIDEO_KP_BLINK				32	//是否眨眼
#define IR_VIDEO_KP_LRJUDGE				33	//标记了关键点估计的左右眼标识和眼睛检测得到的左右眼标识是否一致。0为一致，1为不一致
#define IR_VIDEO_KP_KDLRD				34	//当检测到一只眼睛时，关键点估计的虹膜半径
#define IR_VIDEO_KP_KDRRD				35  //当检测到一只眼睛时，关键点估计的虹膜半径
#define IR_VIDEO_KP_DO_BLINK_DET		36	//是否需要做眨眼检测
//#define IR_VIDEO_KP_H_ANGLE				36	//关键点检测得到的两眼水平角度的平均值
//#define IR_VIDEO_KP_SLOPE_ANGLE			37	//关键点检测得到的两眼中心连线的角度
#define IR_VIDEO_DEVICE_TYPE			38	//当前设备类型
#define IR_VIDEO_IRIS_DIST				39	//两眼间距离

typedef struct EyeDetectionInfoStruct
{
	int lBgR;	//row index of up-left point of left eye
	int lBgC;	//col index of up-left point of left eye
	int lROIH;	//height index of left eye
	int lROIW;	//width index of left eye
	int lImFlag;//left or right eye flag of 'left' image (see IR_IM_EYE_UNDEF)
				//
	int lIrisRow;		//The row index of iris center in iris image, ranging from 1 to IR_IMAGE_HEIGHT
	int lIrisCol;		//The column index of iris center in iris image, ranging from 1 to IR_IMAGE_WIDTH
	int lIrisRadius;	//The radius of iris in iris image



	int rBgR;	//row index of up-left point of right eye
	int rBgC;	//col index of up-left point of right eye
	int rROIH;	//height index of left eye
	int rROIW;	//width index of left eye
	int rImFlag;//left or right eye flag of 'right' image (see IR_IM_EYE_UNDEF)
				//
	int rIrisRow;		//The row index of iris center in iris image, ranging from 1 to IR_IMAGE_HEIGHT
	int rIrisCol;		//The column index of iris center in iris image, ranging from 1 to IR_IMAGE_WIDTH
	int rIrisRadius;	//The radius of iris in iris image

	int captureDistance;  //The distance estimation whether the user is too far or too near
	int streamInf[IR_VIDEO_HIST_INFO_LEN];//used for video stream information (not used currently)
}EyeDetectionInfo, *pEyeDetectionInfo;

/*****************************************************************************
*  函 数 名：IKIR_GetIKAlgoDeviceAReversion
*  功    能：获取当前算法的版本信息
*  说    明：主要获取的算法的发布日期和对应的SVN版本号
*  参    数：输入infoA和infoB
*  返 回 值：=0：成功，否则为失败
*****************************************************************************/
IK_ALGO_API int IKIR_GetVersion(char* infoA, char* infoB);

//眼睛检测函数
///*****************************************************************************
//从脸部图像中检测眼睛，并输出结果。
//注1：minIrisDiameter, maxIrisDiameter为需要检测的最大和最小虹膜直径
//注2：surImage要求左上角为零点。
//*****************************************************************************/
IK_ALGO_API int IKIR_IrisDetection(unsigned char* surImage, int surImH, int surImW,	int minIrisDiameter, int maxIrisDiameter, 
							pEyeDetectionInfo detectResults,int deviceType);
//在图像上将关键点检测结果标记出来
IK_ALGO_API int IKIR_DrawKeyPointResult(unsigned char* surImage, int imH, int imW, int *detectResults);
//在图像上将检测结果标记出来
IK_ALGO_API int IKIR_DrawIrisDetecionResult(unsigned char* surImage, int imH, int imW, EyeDetectionInfo detectResults);
//对眼睛定位结果进行初始化
IK_ALGO_API int IKIR_InitEyeDetectResInit(pEyeDetectionInfo eyedetectResults);
//IK_ALGO_API int	IKIR_CopyEyeDetectResult(pEyeDetectionInfo p_dest_imginfo, pEyeDetectionInfo p_src_imginfo);



//关键点检测函数
//////////////////////////////////////////////////////////////////////////
//关键点检测相关的定义
#define KD_POINT		3000
#define KD_POINTNUM		7
#define KD_OPENVAR_TH	7// 30//20	//越大越严格，检测率越低
struct IKKeyPoint
{
	int l_landmarks_x[KD_POINTNUM];
	int l_landmarks_y[KD_POINTNUM];
	int r_landmarks_x[KD_POINTNUM];
	int r_landmarks_y[KD_POINTNUM];
	int l_var;
	int r_var;
	int l_openLevel;
	int r_openLevel;
	//int horizont_angle;
	//int slope_angle;
	int l_valid;
	int r_valid;
	int blinkFlag;
};

IK_ALGO_API int IKIR_InitKeyPoints(IKKeyPoint* kp);
IK_ALGO_API int IKIR_KeyPointsDetect(unsigned char *data, const int surImH, int surImW,
	int lEyeBgPx, int lEyeBgPy, int lEyeRectH, int lEyeRectW,
	int rEyeBgPx, int rEyeBgPy, int rEyeRectH, int rEyeRectW,
	IKKeyPoint *kp, int deviceType, int algoFlag,int needBlinkDet);
int IKIR_KDGetBlinkResultV1(IKKeyPoint *kp, int captureDistance, int deviceType);
int IKIR_KDGetBlinkResultV2(IKKeyPoint *kp, int captureDistance, int deviceType);

IK_ALGO_API int IKIR_GetKDCurvePoints(const int* eyeKPRows, const int* eyeKPCols, int* eyeKPCurveRows, int* eyeKPCurveCols, int* eyeKPCurvePtNum);
IK_ALGO_API int IKIR_GetKDDnCurvePoints(const int* eyeKPRows, const int* eyeKPCols, int* eyeKPCurveRows,int eyeKPCurvePtNum);

#endif //IKALGO_API_SEPRATE_MODULE
//////////////////////////////////////////////////////////////////////////
//以上为idIrisDetection的宏定义，与IKALGO_API_SEPRATE_MODULE相关
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
//以下为dpImageDisplay的宏定义,与IKALGO_API_SEPRATE_MODULE相关
//////////////////////////////////////////////////////////////////////////
#ifndef IKALGO_API_SEPRATE_MODULE
//灰度转换函数:RGB8888图像转换为灰度图像
IK_ALGO_API int IKIR_RGB8888ToGrayInvert(int* img8888, int imageH, int imageW, unsigned char* imgGray);//将RGB图像转换为灰度图像
//从YUV分量获取将采样图像
IK_ALGO_API int IKIR_YChannelToDnIm(unsigned char* img8, int imageH, int imageW, unsigned char* dnImGray, int dnImH, int dnImW, int dnRate, int rotateAngle, int l2rFlag);
//Ychannel得到RGB888图像
IK_ALGO_API int IKIR_YChannelROIToBitmap8888(unsigned char* img8, int imageH, int imageW, int bgX, int bgY, int endX, int endY,
	int* img8888, int rotateAngle, int l2rFlag, int dnRate);
IK_ALGO_API int IKIR_YChannelROIToBitmap8888A780(unsigned char* img8, int imageH, int imageW, int bgX, int bgY, int endX, int endY,
	int* img8888, int rotateAngle, int l2rFlag, int dnRate);
//从YChanel获取Iris Images
IK_ALGO_API int IKIR_CropIRImFromYChannel(unsigned char* img8, int imageH, int imageW, unsigned char* leftIRIm, int leftValid, int leftBgR, int leftBgC,
	unsigned char* rightIRIm, int rightValid, int rightBgR, int rightBgC, int irImH, int irImW, int rotateAngle, int l2rFlag);
//IK_ALGO_API int IKIR_CropIRImFromYChannelA588t(unsigned char* img8, int imageH, int imageW, unsigned char* leftIRIm, int leftValid, int leftBgR, int leftBgC,
//	unsigned char* rightIRIm, int rightValid, int rightBgR, int rightBgC, int irImH, int irImW, int rotateAngle, int l2rFlag);
//IK_ALGO_API  int IKIR_Bayer2GrayIm(unsigned char* bayerFrame, unsigned char* grayIm, int imH, int imW);//将Bayer图像数据转换为灰度图像
//int IKIR_RGB8888ToGray(int* img8888, int imageH, int imageW, unsigned char* imgGray, unsigned char* dnImGray, int dnImH, int dnImW, int dnRate, int rotateAngle, int l2rFlag);
//int IKIR_YChannelToGray(unsigned char* img8, int imageH, int imageW, unsigned char* imgGray, unsigned char* dnImGray, int dnImH, int dnImW, int dnRate, int rotateAngle, int l2rFlag);
//IK_ALGO_API int IKIR_YChannelToDnImA588t(unsigned char* img8, int imageH, int imageW, unsigned char* dnImGray, int dnImH, int dnImW, int dnRate, int rotateAngle, int l2rFlag);
//IK_ALGO_API int IKIR_YChannelToBitmap8888(unsigned char* img8, int imageH, int imageW, int* img8888, int rotateAngle, int l2rFlag, int dnRate);
//IK_ALGO_API int IKIR_YChannelROIToBitmap8888Gamma(unsigned char* img8, int imageH, int imageW, int bgX, int bgY, int endX, int endY,
//	int* img8888, int rotateAngle, int l2rFlag, int dnRate);
//IK_ALGO_API int IKIR_YChannelROIToBitmap8888ZTES2004(unsigned char* img8, int imageH, int imageW, int bgX, int bgY, int endX, int endY,
//	int* img8888, int rotateAngle, int l2rFlag, int dnRate);

//IK_ALGO_API int IKIR_YChannelROIToBitmap8888A588t(unsigned char* img8, int imageH, int imageW, int bgX, int bgY, int endX, int endY,
//	int* img8888, int rotateAngle, int l2rFlag, int dnRate);
#endif //IKALGO_API_SEPRATE_MODULE
//////////////////////////////////////////////////////////////////////////
//以上为dpImageDisplay的宏定义，与IKALGO_API_SEPRATE_MODULE相关
//////////////////////////////////////////////////////////////////////////


//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//supported work mode
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
#define IR_WORK_MODE_ENR			0		//Enroll mode, with high quality requirement.
#define IR_WORK_MODE_REC			1		//Identification mode, with Fair  quality requirement, 
#define IR_WORK_MODE_NULL			-1		//算法空闲
//default output match index which means it does not match
#define IR_DEFAULT_MACTH_INDEX			 -1
//#define IR_DEFAULT_MATCH_SCORE			 -1

//////////////////////////////////////////////////////////////////////////
//Possible status of an iris image
//////////////////////////////////////////////////////////////////////////
#define IR_STATUS_BEGIN			0//No processing is performed
#define IR_STATUS_IRISDETECT    1//Iris detection has been performed
#define IR_STATUS_QUALITYEVAL	3//Iris image quality evaluations have been finished
#define IR_STATUS_SEGMENTATION	4//CASIA_Gne_Loc has succeed
#define IR_STATUS_CODED			5//The iris image has been coded

//////////////////////////////////////////////////////////////////////////
// 比对分数
#define FM_MinCascadeMatchScoreInt   0
//遮挡分数
#define IR_QE_ED_MinValidOccScore	 1

/* Some useful structure */
#define IR_UnknownInfoLen						100		//算法内部参数交换区域大小，用户可见，但是用不不允许改变该区域的值
#define InfoSpace_DevTypePos					0		//第0位表示设备类型
#define InfoSpace_QEFairThPos					1		//第1位表示满足识别的质量分数阈值
#define InfoSpace_QEGoodThPos					2		//第2位表示满足注册的质量分数阈值
#define InfoSpace_QEExcellentThPos				3		//第3位表示满足高质量的质量分数阈值
#define InfoSpace_PFRefInIm						4		//图像中光板点的比例	
#define InfoSpace_PFRefInIris					5		//虹膜中光板点的比例	
#define InfoSpace_IDIrisRd						6		//根据眼睛检测结果得到的当前图像的虹膜半径估计值
#define InfoSpace_SpotPtInPupil					7		//瞳孔中是否有光斑点
#define InfoSpace_CaptureDist					8		//当前图像的图像采集距离(cm)
#define InfoSpace_PseudoUID						9		//通过随机数或者用户输入的当前注册用户的UID信息
#define InfoSpace_MaxMtScore					10		//Iden中最佳匹配分数
#define InfoSpace_MaxMtIndex					11		//Iden中最佳匹配分数对应的特征的index，与Pseudp ID不一致
#define InfoSpace_MatchedNum					12		//Iden中匹配的数目
#define InfoSpace_CGQE_Rt						12		//与InfoSpace_MatchedNum复用,在Code之前，标识当前图像JudgeQuality的结果.在满足Quality之后，变为InfoSpace_MatchedNum
#define InfoSpace_ImIndex						13		//调试时使用，保存了当前图像的序号
//#define InfoSpace_EnrNum						14		//表示已经注册成功的数目
#define InfoSpace_QEOccThPos					15		//遮挡度的阈值
#define InfoSpace_KD1R							16		//第一个关键点检测点
#define InfoSpace_KD2R							17		//第一个关键点检测点
#define InfoSpace_KD3R							18		//第一个关键点检测点
#define InfoSpace_KD4R							19		//第一个关键点检测点
#define InfoSpace_KD5R							20		//第一个关键点检测点
#define InfoSpace_KD6R							21		//第一个关键点检测点
#define InfoSpace_KD7R							22		//第一个关键点检测点
#define InfoSpace_KD1C							23		//第一个关键点检测点
#define InfoSpace_KD2C							24		//第一个关键点检测点
#define InfoSpace_KD3C							25		//第一个关键点检测点
#define InfoSpace_KD4C							26		//第一个关键点检测点
#define InfoSpace_KD5C							27		//第一个关键点检测点
#define InfoSpace_KD6C							28		//第一个关键点检测点
#define InfoSpace_KD7C							29		//第一个关键点检测点
#define InfoSpace_OpenLevel						30		//该虹膜图像眼皮睁开的程度
#define InfoSpace_Rd2LRDistRatio				31		//该虹膜粗定位得到的虹膜半径与关键点左右两个眼角距离的比值
#define InfoSpace_KDIrisRd						32		//当检测到一只眼睛时，关键点估计的虹膜半径
#define InfoSpace_GlassFlag						33		//是否是戴眼镜
#define InfoSpace_IDIRIsSameIm					34		//标记IR中虹膜图像是否来自于与ID时的人脸图像
//20160517，Star：说明：99位被印度项目使用，宏定义为InfoSpace_IndiaSingleK7Flag
typedef struct IrisImageInfoStruct
{  
	//////////////////////////////////////////////////////////////////////////
	// Input Parameters
	//////////////////////////////////////////////////////////////////////////

	//Height of the iris image in pixels. It must be fixed to 480 currently.
	int imageH;  

	//Width of the iris image in pixels. It must be fixed to 640 currently.
	int imageW;  

	//Type of Image
	//IR_IM_VGA_LEFF: VGA,Left
	//IR_IM_VGA_RIGHT: VGA,right
	//IR_IM_VGA_UNDEF: Unkown left or right
	//IR_IM_NONE: there is no valid image
	int imType;

	//Code of iris localization method, defined as follows:
	//IR_LOC_DEFAULT: Default technique determined by CASIA_GEN algorithm.
	//IR_LOC_CIRCLE : Use circle fitting strategy to localize the iris edges.
	//IR_LOC_SNAKE  : Use edge fitting strategy (e.g. active contour) to localize the iris edges.
	int locMethod;	//20151213 下一版本修改中, 将此变量删除,替换为 DeviceType

	//Specify whether to enable spoof eye detection
	//IR_NO_SPOOF_DETECT : Do not perform spoof eye detection
	//IR_DO_SPOOF_DETECT : Perform spoof eye detection
	int ifSpoofDetect;

	//In which status the iris image is processed
	int processStatus;


	//The row index of pupil center in iris image, ranging from 1 to IR_IMAGE_HEIGHT.
	float pupilRow;

	//The column index of pupil center in iris image,ranging from 1 toIR_IMAGE_WIDTH
	float pupilCol;

	//The radius of pupil in iris image
	float pupilRadius;

	////The confidence of pupillary boundary localization, ranging from 0-100, the larger
	////the confidence, the better the pupillary boundary localization is.
	//int pupilBdConf;

	//The row index of iris center in iris image, ranging from 1 to IR_IMAGE_HEIGHT
	float irisRow;

	//The column index of iris center in iris image, ranging from 1 to IR_IMAGE_WIDTH
	float irisCol;

	//The radius of iris in iris image
	float irisRadius;


	//The position that the pupillary boundary cross the image boundaries.
	//ranging from 0-9, see the micro-definitions whose name begin with IR_POS_*******
	int pupilPos;

	//THe percentage of visible part of the pupillary boundary. 
	int pupilVisiblePercent;

	//The position that the limbic boundary cross the image boundaries.
	//ranging from 0-9, see the micro-definitions whose name begin with IR_POS_*******
	int irisPos;

	//THe percentage of visible part of the iris boundary. 
	int irisVisiblePercent;

	//The overall percentage of visible part of the annular iris region
	int overallVisiblePercent;

	//Indicator of the focus level of the iris image, ranging from 0 to 100. High value of focusScore means clear iris image and low value of focusScore means defocused iris image.
	int focusScore; 

	//Percentage of the visible part of the iris., ranging from 0 to 100. 100 means no occlusion while 65 means 65 percent of the iris are occluded.
	int percentVisible;

	// Number of valid iris feature bits of the iris.
	//20151117:在CASIA_Gen_JudgeImQuality 函数中借用 numValidBits保存第二次计算得到的黑脸判断函数)
	//20151125:在Code函数中，赋予该变量其实际含义，即有效的bit数
	int numValidBits;

	// Confidence of the input iris image is obtained from spoof eye. 
	// 0 means the image is obtained from genuine eye, while 100 means the image is a spoof one.
	int spoofValue; 

	// Signal to noise ratio of the iris image.
	float SNR; 

	// Saturation level of the iris image.
	float Saturation; 

	// Indicator of the how much the image is not interlaced. Higher value indicates the iris image is less interlaced.
	float interlaceValue; 


	// The overall quality of the iris image, can be one of the following levels.
	int	QualityLevel;
	int QualityScore;

	//1D array of pixel intensity values of the original iris image
	unsigned char imageData[IR_IMAGE_SISE]; 
	//1D array of pixel intensity values of the demo iris image. Pupil/iris and iris/sclera boundary points are marked with white pixels.
	unsigned char locImage[IR_IMAGE_SISE]; 

	//Iris image feature template enrolled into database, with fixed length 512 bytes.
	unsigned char irisEnrTemplate[IR_ENR_FEATURE_LENGTH];

	//Iris image feature template of recognition image, with fixed length 1024 bytes.
	unsigned char irisRecTemplate[IR_REC_FEATURE_LENGTH];
	int irisInfoUnkonwn[IR_UnknownInfoLen];
} IrisImageInfo, *pIrisImageInfo;



//加入了算法逻辑函数
#define g_constIREnrollImgNum		3						//注册过程中算法使用的注册图像数
#define g_constIREnrollTimeNum  (g_constIREnrollImgNum+2)	//为注册开辟的空间
#define IKIR_EnrCodeUID_MaxLen	20

//注册过程中图像比对信息结构体数组结构体
typedef struct IrisImageInfoArrayStruct
{
	int count; //表示当前已经符合注册要求的图像数目
	IrisImageInfo IrisImageInfoArray[g_constIREnrollImgNum+1];
} IrisImageInfoArray, *pIrisImageInfoArray;


//比对过程中的输出结构
typedef struct IrisMatchIndexStruct
{
	int matchIndex; //表示当前已经符合注册要求的图像数目
} IrisMatchIndex, *pIrisMatchIndex;



/*****************************************************************************
*  函 数 名：IKIR_GetDevInfo
*  功    能：获取当前设备的信息
*  说    明：主要获取Android手机的BuildID和DeviceModel
*  参    数：m_BuildID：输出参数，设备的BuildID
*            m_DevModel：输出参数，设备的Device Model Name
*  返 回 值：=0：成功，否则为失败
*****************************************************************************/
IK_ALGO_API int IKIR_CheckLicence(char* info1, char* info2);

IK_ALGO_API int IKIR_GetDevInfo(char* m_BuildID, char* m_DevModel);//获得当前设备的版本等信息
IK_ALGO_API int IKIR_GetDevInfoMore(char* m_CpuAbi, char* m_AndroidVer, char* m_SDKVer);//获得当前设备的更多信息
/*****************************************************************************
*  函 数 名：IKIC_GetCpuIDMacAdd
*  功    能：获取当前设备的信息
*  说    明：主要获取X86平台下的cpuid和mac address
*  参    数：cpuID：输出参数，设备的cpuid
*            macAdd：输出参数，设备的mac address
*  返 回 值：=0：成功，否则为失败
*****************************************************************************/
IK_ALGO_API int IKIC_GetCpuIDMacAdd(char* cpuID, char* macAdd);



/*****************************************************************************
*  函 数 名：IKIR_Init
*  功    能：初始化虹膜识别算法结构体
*  说    明：初始化IrisImageInfo结构体
*  参    数：deviceType：输入参数，虹霸设备类型;
*  参    数：deviceID：	输入参数，虹霸设备ID;
*  返 回 值：=0：成功，否则为失败
*****************************************************************************/
IK_ALGO_API int IKIR_Init(int deviceType, char* deviceID);

/*****************************************************************************
*  函 数 名：IKIR_InitIrisInfo
*  功    能：初始化IrisImageInfo结构体
*  说    明：初始化IrisImageInfo结构体
*  参    数：pDestImginfo：输出参数，虹膜信息结构体
*  返 回 值：=0：成功，否则为失败
*****************************************************************************/
IK_ALGO_API int IKIR_InitIrisInfo(pIrisImageInfo pDestImginfo);

/*****************************************************************************
*  函 数 名：IKIR_CopyIrisInfo
*  功    能：复制IrisImageInfo结构体
*  说    明：复制IrisImageInfo结构体
*  参    数：p_dest_imginfo：输出参数，目标虹膜信息结构体
*            p_src_imginfo：输入参数，源虹膜信息结构体
*  返 回 值：=0：成功，否则为失败
*****************************************************************************/
IK_ALGO_API int	IKIR_CopyIrisInfo( pIrisImageInfo p_dest_imginfo, pIrisImageInfo p_src_imginfo);

//在虹膜结构体中写入和读取图像序号，仅用于debug使用
IK_ALGO_API int IKIR_SetImIndex(pIrisImageInfo pIrisImage, unsigned char curIndex);
IK_ALGO_API int IKIR_CheckImIndex(pIrisImageInfo pIrisImage);

/*****************************************************************************
*  函 数 名：IKIR_GetROIImage
*  功    能：从原始图像中将左右眼ROI区域截取640*480的图像
*  说    明：从原始图像中将左右眼ROI区域截取640*480的图像
*  参    数：unsigned char* rawImage, int imageH, int imageW:从摄像探头拍摄的图片的数据和大小，图像为灰度图像，大小根据摄像头大小设置即可
			int ROIImType: 扣取区域标志位。如果是扣取左眼，则设置为IR_IM_EYE_LEFT，如果扣取右眼，则设置为IR_IM_EYE_RIGHT，如果是扣取双目，则设置为3
			pIrisImageInfo p_ROI1_imginfo, int* ROI1BgRow, int* ROI1BgCol, int* ROI1ImH, int* ROI1ImW+
			pIrisImageInfo p_ROI2_imginfo, int* ROI2BgRow, int* ROI2BgCol, int* ROI2ImH, int* ROI2ImW:左眼或者右眼获得的图像数据，
					即从原始图像中截取的ROI区域复制到这两个结构体重的imageData中.
*  返 回 值：=0：成功，否则为失败
*  注1：注意在调用前，首先要分配好结构体的空间
*  注2：rawImage要求左上角为零点。即
*****************************************************************************/
IK_ALGO_API int	IKIR_GetROIImage(unsigned char* rawImage, int imageH, int imageW,int ROIImType,
					 pIrisImageInfo p_ROIL_imginfo, int* ROILBgRow, int* ROILBgCol, int* ROILImH, int* ROILImW, 
					 int lIirisRow, int lIrisCol, int lIrisRadius,
					 pIrisImageInfo p_ROIR_imginfo, int* ROIRBgRow, int* ROIRBgCol, int* ROIRImH, int* ROIRImW,
					 int rIirisRow, int rIrisCol, int rIrisRadius);
IK_ALGO_API int	IKIR_GetROIImageKD(unsigned char* rawImage, int imageH, int imageW, pEyeDetectionInfo eyeDtRt, pIrisImageInfo p_ROIL_imginfo, pIrisImageInfo p_ROIR_imginfo);


/*****************************************************************************
*  函 数 名：IKIR_ImageSelection
*  功    能：从至少minSelectCount幅图像中选择一幅最好的送入注册或识别算法
*  说    明：当N幅图像中没有合适图像时，该函数会继续选择图像直至有合适图像时才返回成功
*  参    数：minSelectCount：输入参数，从至少minSelectCount幅图像中选择最佳图像。即该函数至少调用minSelectCount次才有可能成功
*            qualityMode：输入参数，质量判断模式，注册时为IR_WORK_MODE_ENR，识别时为IR_WORK_MODE_REC
*            pCurLIrisImg，pCurRIrisImg：输入参数，当前图像的pIrisImageInfo结构体
*            pBestLIrisImg，pBestRIrisImg：输出参数，在选择过程中得到的最好图像的pIrisImageInfo结构体
*  返 回 值：>=minSelectCount：成功，否则为失败
*  注01：注意返回值的含义
*  注02：至少调用minSelectCount次才有可能成功。否则即使当前图像满足要求，仍返回失败。
*****************************************************************************/
IK_ALGO_API int IKIR_ImageSelection(int minSelectCount, int qualityMode, pIrisImageInfo pCurLIrisImg, pIrisImageInfo pBestLIrisImg,
							pIrisImageInfo pCurRIrisImg, pIrisImageInfo pBestRIrisImg);

/*****************************************************************************
*  函 数 名：IKIR_Enrollment
*  功    能：图像注册函数，将输入的通过质量判断的ROI数据进行处理，直到注册成功
*  说    明：特别需要注意，调用注册特征时，用户应分配足够的空间
*  参    数：pEnrImagesArray：输入输出参数，预先分配的结构体，存储了符合注册要求的虹膜图像的虹膜结构体
*            curBestIm：输入参数，当前获得的最好虹膜图像对应的虹膜结构体
*  返 回 值：表示已经成功注册的图像数目，如果>=3,则注册成功
*****************************************************************************/
IK_ALGO_API int  IKIR_Enrollment(pIrisImageInfoArray pEnrImagesArray,pIrisImageInfo curBestIm);
IK_ALGO_API int  IKIR_EnrollmentNew(pIrisImageInfoArray pEnrImagesArray, pIrisImageInfo curBestIm);

/*****************************************************************************
*  函 数 名：IKIR_SetUID2EnrCodes
*  功    能：对注册得到的注册特征加入UID信息
*  说    明：特别需要注意，调用注册特征时，用户应分配足够的空间
*  参    数：codeList：输入参数，已有注册特征列表
*            codeNum：输入参数，已有注册特征数量
*			unsigned char* UID, int uidLen:用户名或者随机生成的用户名
*  返 回 值：0
*****************************************************************************/
IK_ALGO_API int  IKIR_GenerateRandomUID4EnrCodes(unsigned char* randomUID, int uidLen);
IK_ALGO_API int  IKIR_SetRandomUID2EnrCodes(unsigned char* codeList, int codeNum, unsigned char* UID, int uidLen);
IK_ALGO_API int  IKIR_GetRandomUID8EnrCodes(unsigned char* codeList, int codeNum,int matchIndex, unsigned char* UID, int uidLen);
IK_ALGO_API int  IKIR_IsEqualUID(unsigned char* UID1, unsigned char* UID2, int uidLen);

#ifndef Setting_FillKind7InEnr4India //印度项目不可进行识别
/*****************************************************************************
*  函 数 名：IKIR_Identification
*  功    能：输入一幅通过质量判断的图像进行特征识别
*  参    数：codeList：输入参数，已有注册特征列表
*            codeNum：输入参数，已有注册特征数量
*            pBestIrisImg：输入参数，通过质量判断的图像
*            pIrisMatchIndex pMatchResult：输出参数，得到比对成功的模板的序号
*  返 回 值：0：成功，!0：失败
*****************************************************************************/
IK_ALGO_API int IKIR_Identification(unsigned char* codeList, int codeNum, pIrisImageInfo pBestIrisImg, pIrisMatchIndex pMatchResult);
IK_ALGO_API int IKIR_IdenCode(pIrisImageInfo pBestIrisImg);
IK_ALGO_API int IKIR_IdenMatch(unsigned char* codeList, int codeNum, unsigned char* irisRecTemplate, pIrisMatchIndex pMatchResult);


/*****************************************************************************
*  函 数 名：IKIR_IrisFaceFusion
*  功    能：根据当前图像的左眼、右眼、人脸识别比对分数进行多模态的融合，得到多模态融合的结果
*  参    数：maxS_L, maxID_L：输入参数，左眼识别分数及对应的最佳ID
*            maxS_R, maxID_R：输入参数，右眼识别分数及对应的最佳ID
*            maxS_F, maxID_F：输入参数，人脸识别分数及对应的最佳ID
*            pIrisMatchIndex pFusionResult：输出参数，多模态融合后最佳的识别ID（或者-1，即无识别结果）
*返 回 值：-1 有问题,0 成功（识别成功和拒绝）; IndexFusion：识别成功为对应的ID值, - 1 拒绝,0无效输入,
*****************************************************************************/
IK_ALGO_API int IKIR_IrisFaceFusion(int maxS_L, int maxID_L, int maxS_R, int maxID_R, int maxS_F, int maxID_F, pIrisMatchIndex pFusionResult);

#define FM_MFF_MaxMemLength		10
#define FM_MFF_MemInfoLength	7
typedef struct MultiFrameFusionInfoStruct
{
	int RestestTimer;
	int FrameFusionNum;
	int FailedNum;
	int MemList[FM_MFF_MaxMemLength * FM_MFF_MemInfoLength];
	int Dist[FM_MFF_MaxMemLength];
}MultiFrameFusionInfo, *pMultiFrameFusionInfo;
IK_ALGO_API int IKIR_MultiFrameFusionInfoReset(pMultiFrameFusionInfo pDestFusionInfo);
IK_ALGO_API int IKIR_MultiFrameFusionInfoInit(pMultiFrameFusionInfo pDestFusionInfo);
IK_ALGO_API int IKIR_MultiFrameFusion(int leftS, int ID_L, int rightS, int ID_R, int faceS, int ID_F, pMultiFrameFusionInfo pDestFusionInfo, pIrisMatchIndex pFusionResult);
#endif

IK_ALGO_API int   CASIA_Gen_Loc(pIrisImageInfo pIrisInfo, int workMode);
IK_ALGO_API int	 IKIR_MarkPreProcResults(pIrisImageInfo pIrisInfo);

#endif //_CASIA_GEN_H
