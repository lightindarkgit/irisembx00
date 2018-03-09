#ifndef _CASIA_GEN_H
#define _CASIA_GEN_H
//#include "ikIrisKing.h"
//StarLee���ڱ����֮ǰ������ƽ̨������
#define IKALG_PLATFORM_LINUX_NO
#define IKALG_PLATFORM_ANDROID_NO
#define IKALG_PLATFORM_WINDOWS_NO
#define IKALG_VERSION_X64_NO




#define Setting_FeatureCode2Mask_NO//�������ú꣬��ʾ�������а���template��mask
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
//����ΪIKIrisKing�ĺ궨��,��IKALGO_API_SEPRATE_MODULE���
//////////////////////////////////////////////////////////////////////////
#ifndef IKALGO_API_SEPRATE_MODULE

//����ӿں�����ʽ
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
#define IR_IM_Face						4//����ͼ��

//////////////////////////////////////////////////////////////////////////
//Iris image size setting
//////////////////////////////////////////////////////////////////////////
#define IR_IMAGE_HEIGHT					480
#define IR_IMAGE_WIDTH					640
#define IR_IMAGE_SISE					307200

//ͼ���и�����Ϣ��λ�ü�¼
#define IR_ImIndexPos					2


//////////////////////////////////////////////////////////////////////////
//Type of device
//////////////////////////////////////////////////////////////////////////
#define IKInvalidDevice			0	//Default
#define IK220A					1
#define IKX00					2
#define IKUSB100s				3	//IKUSB��USB 100s���豸���ͺ�
#define IKUSBUVC				4	//IKUSBUVC��USB-UVC��Ŀ���豸���ͺ�
#define	IKUSBAIO				5	//USBһ�壨All-in-One��,����OCX
#define	IKUSBOV580				6	//OV580
#define IKUSBAIO4060			7	//USBһ�壨All-in-One��,���ڽ��������FOV��С
#define	IKUSBOV580L				8	//61����ʾ�ü�⼰������ұ�
#define IKMCASIAV4Lamp			9	//���CASIA_V4 Lamp database
#define IKMUSB132_810			60	//ʹ��IMX132+810LED
#define IKMLenovoMixx			66  //����padƽ��
#define IKMJiaotu_tc			67	//չѶ�ֻ�
#define IKMHuaying6580			68	//20160317�촴�ֻ��豸���ͺ�
#define IKMJiaotu_tcIndia		69	//20160317չѶ�ֻ�ӡ��Ӧ��
#define IKMQCQIKU				70	//20160428����ֻ�
#define IKMSIMDATA				71	//20160707�����ֻ�

//�������ܲ��Եĺ궨��
#define IK_MatchPerfBinsNum          1001		//ͳ��confidence and ROC ʱbin num




#endif //IKALGO_API_SEPRATE_MODULE
//////////////////////////////////////////////////////////////////////////
//����ΪIKIrisKing�ĺ궨�壬��IKALGO_API_SEPRATE_MODULE���
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
//����ΪidIrisDetection�ĺ궨��
//////////////////////////////////////////////////////////////////////////
#ifndef IKALGO_API_SEPRATE_MODULE
////���ڻ����۾����ľ�����ʾ
//#define IR_Dist_Too_Far					-2	//����̫Զ
//#define IR_Dist_ABit_Far				-1	//������Զ
//#define IR_Dist_Good					0	//�������
//#define IR_Dist_ABit_Near				1	//�����Խ�
//#define IR_Dist_Near					2	//����̫��
//���ۼ�ⷽ��ѡ��궨��
#define DETECT_EYE_UPTODOWN_SEARCH//_NO
#define DETECT_ALGO_ROI_DET_NO//
#define DETECT_EYE_OUTDOOR//_NO
#define	DETECT_EYE_SMALL_BLOCK//_NO
//����ͼ�񱥺ͶȺ궨��
#define DETIM_SATUATION_HIGH_Th			200
#define DETIM_SATUATION_LOW_Th			20
//�۾�ROIѡȡ�ı���
#define DETEYE_ROI_RATIO				0.8f
//��Ч���۾������룬�����Ч����ʾ���ʧ��
#define IR_Invalid_ID_Dist				-1
//�۾��������Ϣ�ṹ��
#define IR_VIDEO_HIST_INFO_LEN			50	//Inf vector len of video stream information
//#define IR_VIDEO_USER_DEVICE_DIS				48
#define IR_VIDEO_WROK_MODE				0	//�۾����Ĺ���ģʽ
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
#define IR_VIDEO_KP_LOPEN				30	//���������̶�
#define IR_VIDEO_KP_ROPEN				31	//���������̶�
#define IR_VIDEO_KP_BLINK				32	//�Ƿ�գ��
#define IR_VIDEO_KP_LRJUDGE				33	//����˹ؼ�����Ƶ������۱�ʶ���۾����õ��������۱�ʶ�Ƿ�һ�¡�0Ϊһ�£�1Ϊ��һ��
#define IR_VIDEO_KP_KDLRD				34	//����⵽һֻ�۾�ʱ���ؼ�����Ƶĺ�Ĥ�뾶
#define IR_VIDEO_KP_KDRRD				35  //����⵽һֻ�۾�ʱ���ؼ�����Ƶĺ�Ĥ�뾶
#define IR_VIDEO_KP_DO_BLINK_DET		36	//�Ƿ���Ҫ��գ�ۼ��
//#define IR_VIDEO_KP_H_ANGLE				36	//�ؼ�����õ�������ˮƽ�Ƕȵ�ƽ��ֵ
//#define IR_VIDEO_KP_SLOPE_ANGLE			37	//�ؼ�����õ��������������ߵĽǶ�
#define IR_VIDEO_DEVICE_TYPE			38	//��ǰ�豸����
#define IR_VIDEO_IRIS_DIST				39	//���ۼ����

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
*  �� �� ����IKIR_GetIKAlgoDeviceAReversion
*  ��    �ܣ���ȡ��ǰ�㷨�İ汾��Ϣ
*  ˵    ������Ҫ��ȡ���㷨�ķ������ںͶ�Ӧ��SVN�汾��
*  ��    ��������infoA��infoB
*  �� �� ֵ��=0���ɹ�������Ϊʧ��
*****************************************************************************/
IK_ALGO_API int IKIR_GetVersion(char* infoA, char* infoB);

//�۾���⺯��
///*****************************************************************************
//������ͼ���м���۾�������������
//ע1��minIrisDiameter, maxIrisDiameterΪ��Ҫ����������С��Ĥֱ��
//ע2��surImageҪ�����Ͻ�Ϊ��㡣
//*****************************************************************************/
IK_ALGO_API int IKIR_IrisDetection(unsigned char* surImage, int surImH, int surImW,	int minIrisDiameter, int maxIrisDiameter, 
							pEyeDetectionInfo detectResults,int deviceType);
//��ͼ���Ͻ��ؼ���������ǳ���
IK_ALGO_API int IKIR_DrawKeyPointResult(unsigned char* surImage, int imH, int imW, int *detectResults);
//��ͼ���Ͻ��������ǳ���
IK_ALGO_API int IKIR_DrawIrisDetecionResult(unsigned char* surImage, int imH, int imW, EyeDetectionInfo detectResults);
//���۾���λ������г�ʼ��
IK_ALGO_API int IKIR_InitEyeDetectResInit(pEyeDetectionInfo eyedetectResults);
//IK_ALGO_API int	IKIR_CopyEyeDetectResult(pEyeDetectionInfo p_dest_imginfo, pEyeDetectionInfo p_src_imginfo);



//�ؼ����⺯��
//////////////////////////////////////////////////////////////////////////
//�ؼ�������صĶ���
#define KD_POINT		3000
#define KD_POINTNUM		7
#define KD_OPENVAR_TH	7// 30//20	//Խ��Խ�ϸ񣬼����Խ��
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
//����ΪidIrisDetection�ĺ궨�壬��IKALGO_API_SEPRATE_MODULE���
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
//����ΪdpImageDisplay�ĺ궨��,��IKALGO_API_SEPRATE_MODULE���
//////////////////////////////////////////////////////////////////////////
#ifndef IKALGO_API_SEPRATE_MODULE
//�Ҷ�ת������:RGB8888ͼ��ת��Ϊ�Ҷ�ͼ��
IK_ALGO_API int IKIR_RGB8888ToGrayInvert(int* img8888, int imageH, int imageW, unsigned char* imgGray);//��RGBͼ��ת��Ϊ�Ҷ�ͼ��
//��YUV������ȡ������ͼ��
IK_ALGO_API int IKIR_YChannelToDnIm(unsigned char* img8, int imageH, int imageW, unsigned char* dnImGray, int dnImH, int dnImW, int dnRate, int rotateAngle, int l2rFlag);
//Ychannel�õ�RGB888ͼ��
IK_ALGO_API int IKIR_YChannelROIToBitmap8888(unsigned char* img8, int imageH, int imageW, int bgX, int bgY, int endX, int endY,
	int* img8888, int rotateAngle, int l2rFlag, int dnRate);
IK_ALGO_API int IKIR_YChannelROIToBitmap8888A780(unsigned char* img8, int imageH, int imageW, int bgX, int bgY, int endX, int endY,
	int* img8888, int rotateAngle, int l2rFlag, int dnRate);
//��YChanel��ȡIris Images
IK_ALGO_API int IKIR_CropIRImFromYChannel(unsigned char* img8, int imageH, int imageW, unsigned char* leftIRIm, int leftValid, int leftBgR, int leftBgC,
	unsigned char* rightIRIm, int rightValid, int rightBgR, int rightBgC, int irImH, int irImW, int rotateAngle, int l2rFlag);
//IK_ALGO_API int IKIR_CropIRImFromYChannelA588t(unsigned char* img8, int imageH, int imageW, unsigned char* leftIRIm, int leftValid, int leftBgR, int leftBgC,
//	unsigned char* rightIRIm, int rightValid, int rightBgR, int rightBgC, int irImH, int irImW, int rotateAngle, int l2rFlag);
//IK_ALGO_API  int IKIR_Bayer2GrayIm(unsigned char* bayerFrame, unsigned char* grayIm, int imH, int imW);//��Bayerͼ������ת��Ϊ�Ҷ�ͼ��
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
//����ΪdpImageDisplay�ĺ궨�壬��IKALGO_API_SEPRATE_MODULE���
//////////////////////////////////////////////////////////////////////////


//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//supported work mode
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
#define IR_WORK_MODE_ENR			0		//Enroll mode, with high quality requirement.
#define IR_WORK_MODE_REC			1		//Identification mode, with Fair  quality requirement, 
#define IR_WORK_MODE_NULL			-1		//�㷨����
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
// �ȶԷ���
#define FM_MinCascadeMatchScoreInt   0
//�ڵ�����
#define IR_QE_ED_MinValidOccScore	 1

/* Some useful structure */
#define IR_UnknownInfoLen						100		//�㷨�ڲ��������������С���û��ɼ��������ò�������ı�������ֵ
#define InfoSpace_DevTypePos					0		//��0λ��ʾ�豸����
#define InfoSpace_QEFairThPos					1		//��1λ��ʾ����ʶ�������������ֵ
#define InfoSpace_QEGoodThPos					2		//��2λ��ʾ����ע�������������ֵ
#define InfoSpace_QEExcellentThPos				3		//��3λ��ʾ���������������������ֵ
#define InfoSpace_PFRefInIm						4		//ͼ���й���ı���	
#define InfoSpace_PFRefInIris					5		//��Ĥ�й���ı���	
#define InfoSpace_IDIrisRd						6		//�����۾�������õ��ĵ�ǰͼ��ĺ�Ĥ�뾶����ֵ
#define InfoSpace_SpotPtInPupil					7		//ͫ�����Ƿ��й�ߵ�
#define InfoSpace_CaptureDist					8		//��ǰͼ���ͼ��ɼ�����(cm)
#define InfoSpace_PseudoUID						9		//ͨ������������û�����ĵ�ǰע���û���UID��Ϣ
#define InfoSpace_MaxMtScore					10		//Iden�����ƥ�����
#define InfoSpace_MaxMtIndex					11		//Iden�����ƥ�������Ӧ��������index����Pseudp ID��һ��
#define InfoSpace_MatchedNum					12		//Iden��ƥ�����Ŀ
#define InfoSpace_CGQE_Rt						12		//��InfoSpace_MatchedNum����,��Code֮ǰ����ʶ��ǰͼ��JudgeQuality�Ľ��.������Quality֮�󣬱�ΪInfoSpace_MatchedNum
#define InfoSpace_ImIndex						13		//����ʱʹ�ã������˵�ǰͼ������
//#define InfoSpace_EnrNum						14		//��ʾ�Ѿ�ע��ɹ�����Ŀ
#define InfoSpace_QEOccThPos					15		//�ڵ��ȵ���ֵ
#define InfoSpace_KD1R							16		//��һ���ؼ������
#define InfoSpace_KD2R							17		//��һ���ؼ������
#define InfoSpace_KD3R							18		//��һ���ؼ������
#define InfoSpace_KD4R							19		//��һ���ؼ������
#define InfoSpace_KD5R							20		//��һ���ؼ������
#define InfoSpace_KD6R							21		//��һ���ؼ������
#define InfoSpace_KD7R							22		//��һ���ؼ������
#define InfoSpace_KD1C							23		//��һ���ؼ������
#define InfoSpace_KD2C							24		//��һ���ؼ������
#define InfoSpace_KD3C							25		//��һ���ؼ������
#define InfoSpace_KD4C							26		//��һ���ؼ������
#define InfoSpace_KD5C							27		//��һ���ؼ������
#define InfoSpace_KD6C							28		//��һ���ؼ������
#define InfoSpace_KD7C							29		//��һ���ؼ������
#define InfoSpace_OpenLevel						30		//�ú�Ĥͼ����Ƥ�����ĳ̶�
#define InfoSpace_Rd2LRDistRatio				31		//�ú�Ĥ�ֶ�λ�õ��ĺ�Ĥ�뾶��ؼ������������۽Ǿ���ı�ֵ
#define InfoSpace_KDIrisRd						32		//����⵽һֻ�۾�ʱ���ؼ�����Ƶĺ�Ĥ�뾶
#define InfoSpace_GlassFlag						33		//�Ƿ��Ǵ��۾�
#define InfoSpace_IDIRIsSameIm					34		//���IR�к�Ĥͼ���Ƿ���������IDʱ������ͼ��
//20160517��Star��˵����99λ��ӡ����Ŀʹ�ã��궨��ΪInfoSpace_IndiaSingleK7Flag
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
	int locMethod;	//20151213 ��һ�汾�޸���, ���˱���ɾ��,�滻Ϊ DeviceType

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
	//20151117:��CASIA_Gen_JudgeImQuality �����н��� numValidBits����ڶ��μ���õ��ĺ����жϺ���)
	//20151125:��Code�����У�����ñ�����ʵ�ʺ��壬����Ч��bit��
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



//�������㷨�߼�����
#define g_constIREnrollImgNum		3						//ע��������㷨ʹ�õ�ע��ͼ����
#define g_constIREnrollTimeNum  (g_constIREnrollImgNum+2)	//Ϊע�Ὺ�ٵĿռ�
#define IKIR_EnrCodeUID_MaxLen	20

//ע�������ͼ��ȶ���Ϣ�ṹ������ṹ��
typedef struct IrisImageInfoArrayStruct
{
	int count; //��ʾ��ǰ�Ѿ�����ע��Ҫ���ͼ����Ŀ
	IrisImageInfo IrisImageInfoArray[g_constIREnrollImgNum+1];
} IrisImageInfoArray, *pIrisImageInfoArray;


//�ȶԹ����е�����ṹ
typedef struct IrisMatchIndexStruct
{
	int matchIndex; //��ʾ��ǰ�Ѿ�����ע��Ҫ���ͼ����Ŀ
} IrisMatchIndex, *pIrisMatchIndex;



/*****************************************************************************
*  �� �� ����IKIR_GetDevInfo
*  ��    �ܣ���ȡ��ǰ�豸����Ϣ
*  ˵    ������Ҫ��ȡAndroid�ֻ���BuildID��DeviceModel
*  ��    ����m_BuildID������������豸��BuildID
*            m_DevModel������������豸��Device Model Name
*  �� �� ֵ��=0���ɹ�������Ϊʧ��
*****************************************************************************/
IK_ALGO_API int IKIR_CheckLicence(char* info1, char* info2);

IK_ALGO_API int IKIR_GetDevInfo(char* m_BuildID, char* m_DevModel);//��õ�ǰ�豸�İ汾����Ϣ
IK_ALGO_API int IKIR_GetDevInfoMore(char* m_CpuAbi, char* m_AndroidVer, char* m_SDKVer);//��õ�ǰ�豸�ĸ�����Ϣ
/*****************************************************************************
*  �� �� ����IKIC_GetCpuIDMacAdd
*  ��    �ܣ���ȡ��ǰ�豸����Ϣ
*  ˵    ������Ҫ��ȡX86ƽ̨�µ�cpuid��mac address
*  ��    ����cpuID������������豸��cpuid
*            macAdd������������豸��mac address
*  �� �� ֵ��=0���ɹ�������Ϊʧ��
*****************************************************************************/
IK_ALGO_API int IKIC_GetCpuIDMacAdd(char* cpuID, char* macAdd);



/*****************************************************************************
*  �� �� ����IKIR_Init
*  ��    �ܣ���ʼ����Ĥʶ���㷨�ṹ��
*  ˵    ������ʼ��IrisImageInfo�ṹ��
*  ��    ����deviceType���������������豸����;
*  ��    ����deviceID��	�������������豸ID;
*  �� �� ֵ��=0���ɹ�������Ϊʧ��
*****************************************************************************/
IK_ALGO_API int IKIR_Init(int deviceType, char* deviceID);

/*****************************************************************************
*  �� �� ����IKIR_InitIrisInfo
*  ��    �ܣ���ʼ��IrisImageInfo�ṹ��
*  ˵    ������ʼ��IrisImageInfo�ṹ��
*  ��    ����pDestImginfo�������������Ĥ��Ϣ�ṹ��
*  �� �� ֵ��=0���ɹ�������Ϊʧ��
*****************************************************************************/
IK_ALGO_API int IKIR_InitIrisInfo(pIrisImageInfo pDestImginfo);

/*****************************************************************************
*  �� �� ����IKIR_CopyIrisInfo
*  ��    �ܣ�����IrisImageInfo�ṹ��
*  ˵    ��������IrisImageInfo�ṹ��
*  ��    ����p_dest_imginfo�����������Ŀ���Ĥ��Ϣ�ṹ��
*            p_src_imginfo�����������Դ��Ĥ��Ϣ�ṹ��
*  �� �� ֵ��=0���ɹ�������Ϊʧ��
*****************************************************************************/
IK_ALGO_API int	IKIR_CopyIrisInfo( pIrisImageInfo p_dest_imginfo, pIrisImageInfo p_src_imginfo);

//�ں�Ĥ�ṹ����д��Ͷ�ȡͼ����ţ�������debugʹ��
IK_ALGO_API int IKIR_SetImIndex(pIrisImageInfo pIrisImage, unsigned char curIndex);
IK_ALGO_API int IKIR_CheckImIndex(pIrisImageInfo pIrisImage);

/*****************************************************************************
*  �� �� ����IKIR_GetROIImage
*  ��    �ܣ���ԭʼͼ���н�������ROI�����ȡ640*480��ͼ��
*  ˵    ������ԭʼͼ���н�������ROI�����ȡ640*480��ͼ��
*  ��    ����unsigned char* rawImage, int imageH, int imageW:������̽ͷ�����ͼƬ�����ݺʹ�С��ͼ��Ϊ�Ҷ�ͼ�񣬴�С��������ͷ��С���ü���
			int ROIImType: ��ȡ�����־λ������ǿ�ȡ���ۣ�������ΪIR_IM_EYE_LEFT�������ȡ���ۣ�������ΪIR_IM_EYE_RIGHT������ǿ�ȡ˫Ŀ��������Ϊ3
			pIrisImageInfo p_ROI1_imginfo, int* ROI1BgRow, int* ROI1BgCol, int* ROI1ImH, int* ROI1ImW+
			pIrisImageInfo p_ROI2_imginfo, int* ROI2BgRow, int* ROI2BgCol, int* ROI2ImH, int* ROI2ImW:���ۻ������ۻ�õ�ͼ�����ݣ�
					����ԭʼͼ���н�ȡ��ROI�����Ƶ��������ṹ���ص�imageData��.
*  �� �� ֵ��=0���ɹ�������Ϊʧ��
*  ע1��ע���ڵ���ǰ������Ҫ����ýṹ��Ŀռ�
*  ע2��rawImageҪ�����Ͻ�Ϊ��㡣��
*****************************************************************************/
IK_ALGO_API int	IKIR_GetROIImage(unsigned char* rawImage, int imageH, int imageW,int ROIImType,
					 pIrisImageInfo p_ROIL_imginfo, int* ROILBgRow, int* ROILBgCol, int* ROILImH, int* ROILImW, 
					 int lIirisRow, int lIrisCol, int lIrisRadius,
					 pIrisImageInfo p_ROIR_imginfo, int* ROIRBgRow, int* ROIRBgCol, int* ROIRImH, int* ROIRImW,
					 int rIirisRow, int rIrisCol, int rIrisRadius);
IK_ALGO_API int	IKIR_GetROIImageKD(unsigned char* rawImage, int imageH, int imageW, pEyeDetectionInfo eyeDtRt, pIrisImageInfo p_ROIL_imginfo, pIrisImageInfo p_ROIR_imginfo);


/*****************************************************************************
*  �� �� ����IKIR_ImageSelection
*  ��    �ܣ�������minSelectCount��ͼ����ѡ��һ����õ�����ע���ʶ���㷨
*  ˵    ������N��ͼ����û�к���ͼ��ʱ���ú��������ѡ��ͼ��ֱ���к���ͼ��ʱ�ŷ��سɹ�
*  ��    ����minSelectCount�����������������minSelectCount��ͼ����ѡ�����ͼ�񡣼��ú������ٵ���minSelectCount�β��п��ܳɹ�
*            qualityMode����������������ж�ģʽ��ע��ʱΪIR_WORK_MODE_ENR��ʶ��ʱΪIR_WORK_MODE_REC
*            pCurLIrisImg��pCurRIrisImg�������������ǰͼ���pIrisImageInfo�ṹ��
*            pBestLIrisImg��pBestRIrisImg�������������ѡ������еõ������ͼ���pIrisImageInfo�ṹ��
*  �� �� ֵ��>=minSelectCount���ɹ�������Ϊʧ��
*  ע01��ע�ⷵ��ֵ�ĺ���
*  ע02�����ٵ���minSelectCount�β��п��ܳɹ�������ʹ��ǰͼ������Ҫ���Է���ʧ�ܡ�
*****************************************************************************/
IK_ALGO_API int IKIR_ImageSelection(int minSelectCount, int qualityMode, pIrisImageInfo pCurLIrisImg, pIrisImageInfo pBestLIrisImg,
							pIrisImageInfo pCurRIrisImg, pIrisImageInfo pBestRIrisImg);

/*****************************************************************************
*  �� �� ����IKIR_Enrollment
*  ��    �ܣ�ͼ��ע�ắ�����������ͨ�������жϵ�ROI���ݽ��д���ֱ��ע��ɹ�
*  ˵    �����ر���Ҫע�⣬����ע������ʱ���û�Ӧ�����㹻�Ŀռ�
*  ��    ����pEnrImagesArray���������������Ԥ�ȷ���Ľṹ�壬�洢�˷���ע��Ҫ��ĺ�Ĥͼ��ĺ�Ĥ�ṹ��
*            curBestIm�������������ǰ��õ���ú�Ĥͼ���Ӧ�ĺ�Ĥ�ṹ��
*  �� �� ֵ����ʾ�Ѿ��ɹ�ע���ͼ����Ŀ�����>=3,��ע��ɹ�
*****************************************************************************/
IK_ALGO_API int  IKIR_Enrollment(pIrisImageInfoArray pEnrImagesArray,pIrisImageInfo curBestIm);
IK_ALGO_API int  IKIR_EnrollmentNew(pIrisImageInfoArray pEnrImagesArray, pIrisImageInfo curBestIm);

/*****************************************************************************
*  �� �� ����IKIR_SetUID2EnrCodes
*  ��    �ܣ���ע��õ���ע����������UID��Ϣ
*  ˵    �����ر���Ҫע�⣬����ע������ʱ���û�Ӧ�����㹻�Ŀռ�
*  ��    ����codeList���������������ע�������б�
*            codeNum���������������ע����������
*			unsigned char* UID, int uidLen:�û�������������ɵ��û���
*  �� �� ֵ��0
*****************************************************************************/
IK_ALGO_API int  IKIR_GenerateRandomUID4EnrCodes(unsigned char* randomUID, int uidLen);
IK_ALGO_API int  IKIR_SetRandomUID2EnrCodes(unsigned char* codeList, int codeNum, unsigned char* UID, int uidLen);
IK_ALGO_API int  IKIR_GetRandomUID8EnrCodes(unsigned char* codeList, int codeNum,int matchIndex, unsigned char* UID, int uidLen);
IK_ALGO_API int  IKIR_IsEqualUID(unsigned char* UID1, unsigned char* UID2, int uidLen);

#ifndef Setting_FillKind7InEnr4India //ӡ����Ŀ���ɽ���ʶ��
/*****************************************************************************
*  �� �� ����IKIR_Identification
*  ��    �ܣ�����һ��ͨ�������жϵ�ͼ���������ʶ��
*  ��    ����codeList���������������ע�������б�
*            codeNum���������������ע����������
*            pBestIrisImg�����������ͨ�������жϵ�ͼ��
*            pIrisMatchIndex pMatchResult������������õ��ȶԳɹ���ģ������
*  �� �� ֵ��0���ɹ���!0��ʧ��
*****************************************************************************/
IK_ALGO_API int IKIR_Identification(unsigned char* codeList, int codeNum, pIrisImageInfo pBestIrisImg, pIrisMatchIndex pMatchResult);
IK_ALGO_API int IKIR_IdenCode(pIrisImageInfo pBestIrisImg);
IK_ALGO_API int IKIR_IdenMatch(unsigned char* codeList, int codeNum, unsigned char* irisRecTemplate, pIrisMatchIndex pMatchResult);


/*****************************************************************************
*  �� �� ����IKIR_IrisFaceFusion
*  ��    �ܣ����ݵ�ǰͼ������ۡ����ۡ�����ʶ��ȶԷ������ж�ģ̬���ںϣ��õ���ģ̬�ںϵĽ��
*  ��    ����maxS_L, maxID_L���������������ʶ���������Ӧ�����ID
*            maxS_R, maxID_R���������������ʶ���������Ӧ�����ID
*            maxS_F, maxID_F���������������ʶ���������Ӧ�����ID
*            pIrisMatchIndex pFusionResult�������������ģ̬�ںϺ���ѵ�ʶ��ID������-1������ʶ������
*�� �� ֵ��-1 ������,0 �ɹ���ʶ��ɹ��;ܾ���; IndexFusion��ʶ��ɹ�Ϊ��Ӧ��IDֵ, - 1 �ܾ�,0��Ч����,
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
