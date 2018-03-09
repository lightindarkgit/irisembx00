#if !defined(_IK_USB_ID_H)
#define _IK_USB_ID_H


//////////////////////////////////////////////////////////////////////////
//Image size setting
//////////////////////////////////////////////////////////////////////////
#define IR_IMAGE_HEIGHT					480
#define IR_IMAGE_WIDTH					640
#define IR_IMAGE_SISE					307200
#define IR_UnkonwIrisInfoLen			30		//算法内部参数交换区域大小，用户可见，但是用不不允许改变该区域的值

//////////////////////////////////////////////////////////////////////////
//Iris detection setting
//////////////////////////////////////////////////////////////////////////
#define	IR_ID_MAXOLDFRAMENUM			5		//利用视频序列进行虹膜检测时所使用的最多的帧数

//////////////////////////////////////////////////////////////////////////
//Type of imagae
//////////////////////////////////////////////////////////////////////////
#define IR_IM_EYE_UNDEF					0//Unknown left or right
#define IR_IM_EYE_LEFF					1//Left image	
#define IR_IM_EYE_RIGHT					2//Right image	2
#define IR_IM_EYE_NONE					-1//NO iris image is presented
#define IR_VIDEO_HIST_INFO_LEN			30//Inf vector len of video stream information

//////////////////////////////////////////////////////////////////////////
//Different template lengths in Byte 
//////////////////////////////////////////////////////////////////////////
#define IR_ENR_FEATURE_LENGTH			512  
#define IR_REC_FEATURE_LENGTH			1024  

//////////////////////////////////////////////////////////////////////////
//In the struct MatchOutputStruct, denote the maximum 
//of possible matched result under a certain threshold. 
//////////////////////////////////////////////////////////////////////////
#define IR_MAX_NUM_OF_MATCH				100  

//////////////////////////////////////////////////////////////////////////
//Total additional memory space that are used in IrisImageInfoStruct
//////////////////////////////////////////////////////////////////////////
//The unit is byte
#define IR_IRISINFO_MEMORYSIZE         ((2*IR_IMAGE_SISE+IR_ENR_FEATURE_LENGTH+IR_REC_FEATURE_LENGTH)*sizeof(unsigned char))
#define IR_INFO_IMDATA_POS              0
#define IR_INFO_LOCIM_POS              IR_IMAGE_SISE
#define IR_INFO_ENRTP_POS              (IR_INFO_LOCIM_POS+IR_IMAGE_SISE)
#define IR_INFO_RECTP_POS              (IR_INFO_ENRTP_POS+IR_ENR_FEATURE_LENGTH)

//////////////////////////////////////////////////////////////////////////
//Possible status of an iris image
//////////////////////////////////////////////////////////////////////////
//No processing is performed
#define IR_STATUS_BEGIN                 0
//Iris is detected from a big image just now
#define IR_STATUS_DETECTED				4
//Iris image quality evaluations have been finished
#define IR_STATUS_QUALITY	            1
//CASIA_Gne_Loc has succeed
#define IR_STATUS_LOC                   2
//The iris image has been coded
#define IR_STATUS_CODED                 3

//////////////////////////////////////////////////////////////////////////
//Localization methods
//////////////////////////////////////////////////////////////////////////
#define IR_LOC_DEFAULT					0
#define IR_LOC_CIRCLE					1
#define IR_LOC_SNAKE					2

//////////////////////////////////////////////////////////////////////////
//Whether to perform spoof iris image detection
//////////////////////////////////////////////////////////////////////////
#define IR_NO_SPOOF_DETECT				0
#define IR_DO_SPOOF_DETECT				1

//////////////////////////////////////////////////////////////////////////
//The position that the iris comes across the image boundaries
//They are defined as shown in the following figure.
//////////////////////////////////////////////////////////////////////////
//         ----------------------------------------------------------
//		   |  TopLeft  (1-1=4) |   TopCenter (1-0=3) |  TopCenter (1-2=5) | 
//         |---------------------------------------------------------
//		   | CenterLeft(0-1=1) | CenterCenter(0-0=0) | CenterRight(0-2=2) |
//         |---------------------------------------------------------
//		   | BottomLeft(2-1=7) | BottomCenter(2-0=6) | BottomRight(2-2=8) |
//         |---------------------------------------------------------
#define IR_POS_CenterCenter             0
#define IR_POS_CenterLeft               1
#define IR_POS_CenterRight              2
#define IR_POS_TopCenter                3
#define IR_POS_TopLeft                  4
#define IR_POS_TopRight                 5
#define IR_POS_BottomCenter             6
#define IR_POS_BottomLeft               7
#define IR_POS_BottomRight              8
#define IR_POS_OuterOuter               -1

//////////////////////////////////////////////////////////////////////////
//Overall image quality
//////////////////////////////////////////////////////////////////////////
#define IR_IMAGE_QUALITY_BAD            -1 
#define IR_IMAGE_QUALITY_POOR           0
#define IR_IMAGE_QUALITY_FAIR           1
#define IR_IMAGE_QUALITY_GOOD           2
#define IR_IMAGE_QUALITY_EXCELLENT      3


//////////////////////////////////////////////////////////////////////////
//find mode (or FisrtMatch)
//////////////////////////////////////////////////////////////////////////
//Do an exhaustive search and return the Best match you find in the entire list.
#define IR_FIND_MODE_EXHAUST			0  
//Return with the very first match.
#define IR_FIND_MODE_FIRSTMATCH			1   

//////////////////////////////////////////////////////////////////////////
//Use default match threshold
//////////////////////////////////////////////////////////////////////////
#define IR_USE_DEAULT_MATCHTH			(0.66f)		//Suggest threashold is between [0.65, 0.75], the higher the lower FAR(but higher FRR)

//////////////////////////////////////////////////////////////////////////
//Max negative and positive rotation angle 
//////////////////////////////////////////////////////////////////////////
#define IR_MAX_NEG_ROTATION_ANGLE		(-24.0f)
#define IR_MAX_POS_ROTATION_ANGLE		(24.0f)

//////////////////////////////////////////////////////////////////////////
//Match strategy
//////////////////////////////////////////////////////////////////////////
//Do not use cascade classifier
#define IR_NONCASCADE_CLASSIFIER         1
//Use cascade classifier
#define IR_CASCADE_CLASSIFIER            2

//////////////////////////////////////////////////////////////////////////
//Match Threshold Calculation
//////////////////////////////////////////////////////////////////////////
//The default (also the largest possible) value of systemFAR when calling 
//the function CASIA_Gen_GetSuggestTH()
//#define IR_DEFAULT_SYSTEMFARLEVEL        0.001
#define IR_MIN_NUM_ENR_TP                5000L
#define IR_DEFAULT_CONSERVATION_LEVEL    50

//////////////////////////////////////////////////////////////////////////
//Quality levels of each match
//////////////////////////////////////////////////////////////////////////
#define IR_MATCH_LEVEL_BAD              -1
#define IR_MATCH_LEVEL_POOR              0 //>1050
#define IR_MATCH_LEVEL_FAIR              1 //>1140
#define IR_MATCH_LEVEL_GOOD              2 //>1184
#define IR_MATCH_LEVEL_EXCELLENT         3 //1270

//Work mode
//Enroll mode, with high quality requirement.
//Do not show the localization result.
#define IR_WORK_MODE_ENR				 0   
//Identification mode, with Fair  quality requirement, 
//Do not show the localization result.
#define IR_WORK_MODE_REC				 1   
//Demo mode, with low  quality requirement.
//Show the localization result.
#define IR_WORK_MODE_DEMO				 2   

//////////////////////////////////////////////////////////////////////////
//Error codes
//////////////////////////////////////////////////////////////////////////

//Global Error Code
#define IR_FUN_SUCCESS					0 //Function Success
#define IR_DEFAULT_ERR				-8000 //Default Function Error For Some Infrequent Reason
#define IR_InvalidLicense			-8900 //Invalid license, or the license expired 
//The inputted iris IrisImageInfo is in wrong status, e.g., an IrisImageInfo is 
//processed by CASIA_Gen_Code before calling CASIA_Gen_Loc
#define IR_IMAGE_IN_BAD_STATUS      -8001   //Bad Process Stage
#define IR_LOC_BAD_IMSIZE           -8002   //Bad image size setting
#define IR_BAD_IM_NUM               -8003   //A bad Image number was inputted into CG_Enroll or CG_Loc or CG_Code
#define IR_Bad_IKIR_Handle			-8004	//Invalid IK_IR_Handle. Pls init IK_IR_Handle first before calling IKIR functions	
#define IR_BAD_IRISINFO_HANDLE		-8005	//Invalid IrisImageInfo pointer
#define IR_BAD_INPUT				-8006	//Error if the input image parameters are invalid
//Error Codes For Bad Image Qualities of Some Reason
#define IR_QE_BAD_IMQUALITY         -8100   //Bad Image Quality for Some Infrequent Reason
#define IR_QE_BAD_REFLECTION        -8101   //Too Many Reflection Points in the Iris Image
#define IR_QE_DEFOCUS               -8103   //Image Severe Defocus
#define IR_QE_MOTION_BLUR           -8104   //Image Severe Motion Blur
#define IR_QE_HEAVY_OCCLUSION       -8106   //Heavy Occlusion, not suitable for recognition and enrollment
#define IR_QE_TOO_SMALL_PUPIL       -8115   //The Pupil Size is Too Small
#define IR_QE_TOO_SMALL_IRIS        -8116   //The Iris Size is Too Small
#define IR_QE_OFF_AXIS_VIEWANGLE    -8117   //Off-axis View Angle
#define IR_QE_BAD_POSITION			-8118	//Invalid Iris Position
#define IR_QE_IS_FAKE_IRIS          -8139   //The image is fake iris
#define IR_QE_WEAR_CONTACTLENS      -8141   //The image is wearing contact lens

//Error During Iris Detection (Coarse Iris Localization)
#define IR_PF_ERROR                 -8200   //Error during pupil finding (Coarse Iris Localization)
#define IR_ID_ERROR					-8201	//Error during iris detection from big image (5M)

//Error When Localizing the Boundaries of the Iris Image
#define IR_SG_FAIL_INNER            -8302   //Fail to Localize the Inner Boundary
#define IR_SG_FAIL_OUTER            -8303   //Fail to Localize the Outer Boundary
#define IR_SG_DEFAULT_ERR           -8300   //Fail to Localize the Iris Boundaries for Some Infrequent Reason
#define IR_LOC_MARK_ERR				-8340   //Error when marking the iris image during demo.


//Error During Image Coding
#define IR_FE_CODING_ERR            -8500   //Fail to Code the Iris Image (Both Enrollment and Recognition)
#define IR_ENROLL_ERR				-8501   //Fail to enroll the iris images


//Error During Template Matching
#define IR_FM_MATCH_ERR             -8600   //Default Error During Template Matching
#define IR_MATCH_NO_MATCH			-8601   //Fail to find a match.
#define IR_MATCH_BAD_FINDMODE       -8602   //Invalid find mode setting in MatchInput
#define IR_MATCH_BAD_MATCHSTRATEGY  -8603   //Invalid match strategy, currently, only IR_CASCADE_CLASSIFIER is supported
#define IR_MATCH_BAD_MTTHRESHOLD    -8604   //Invalid match threshold is inputted
#define IR_MATHC_BAD_CGVERSION      -8610   //The codes of the two matching templates are generated with different versions of CG.
#define IR_MATCH_BAD_MATCHPARA      -8611   //Bad input values for CASIA_Gen_GetSuggestTH.


/* Some useful structure */
typedef struct EyeDetectionInfoStruct2
{
	int lCentR;	//row index of left eye
	int lCentC;	//col index of left eye
	int lBgR;	//row index of up-left point of left eye
	int lBgC;	//col index of up-left point of left eye
	int lImFlag;//left or right eye flag of 'left' image (see IR_IM_EYE_UNDEF)
	//
	int lPupilRow;		//The row index of pupil center in iris image, ranging from 1 to IR_IMAGE_HEIGHT.
	int lPupilCol;		//The column index of pupil center in iris image,ranging from 1 toIR_IMAGE_WIDTH
	int lPupilRadius;	//The radius of pupil in iris image
	int lIrisRow;		//The row index of iris center in iris image, ranging from 1 to IR_IMAGE_HEIGHT
	int lIrisCol;		//The column index of iris center in iris image, ranging from 1 to IR_IMAGE_WIDTH
	int lIrisRadius;	//The radius of iris in iris image
	int lFocusScore; 	//Indicator of the focus level of the iris image, ranging from 0 to 100. High value of focusScore means clear iris image and low value of focusScore means defocused iris image.
	int lPercentVisible;//Percentage of the visible part of the iris., ranging from 0 to 100. 100 means no occlusion while 65 means 65 percent of the iris are occluded.
	int lSaturation; 	// Saturation level of the iris image.
	int lInterlaceValue;// Indicator of the how much the image is less interlaced. Higher value indicates the iris image is less interlaced.
	int	lQualityLevel;	
	int lQualityScore;	//最终的质量分数，与QualityLevel是相对应的



	int rCentR;	//row index of right eye
	int rCentC;	//col index of right eye
	int rBgR;	//row index of up-left point of right eye
	int rBgC;	//col index of up-left point of right eye
	int rImFlag;//left or right eye flag of 'right' image (see IR_IM_EYE_UNDEF)
	//
	int rPupilRow;		//The row index of pupil center in iris image, ranging from 1 to IR_IMAGE_HEIGHT.
	int rPupilCol;		//The column index of pupil center in iris image,ranging from 1 toIR_IMAGE_WIDTH
	int rPupilRadius;	//The radius of pupil in iris image
	int rIrisRow;		//The row index of iris center in iris image, ranging from 1 to IR_IMAGE_HEIGHT
	int rIrisCol;		//The column index of iris center in iris image, ranging from 1 to IR_IMAGE_WIDTH
	int rIrisRadius;	//The radius of iris in iris image
	int rFocusScore; 	//Indicator of the focus level of the iris image, ranging from 0 to 100. High value of focusScore means clear iris image and low value of focusScore means defocused iris image.
	int rPercentVisible;//Percentage of the visible part of the iris., ranging from 0 to 100. 100 means no occlusion while 65 means 65 percent of the iris are occluded.
	int rSaturation; 	// Saturation level of the iris image.
	int rInterlaceValue;// Indicator of the how much the image is less interlaced. Higher value indicates the iris image is less interlaced.
	int	rQualityLevel;	
	int rQualityScore;	//最终的质量分数，与QualityLevel是相对应的

	int streamInf[IR_VIDEO_HIST_INFO_LEN];//used for video stream information (not used currently)


}EyeDetectionInfo2, *pEyeDetectionInfo2;

// typedef struct IrisImageInfoStruct 
// {  
	// //////////////////////////////////////////////////////////////////////////
	// // Input Parameters
	// //////////////////////////////////////////////////////////////////////////

	// //Height of the iris image in pixels. It must be fixed to 480 currently.
	// int imageH;  

	// //Width of the iris image in pixels. It must be fixed to 640 currently.
	// int imageW;  

	// //Type of Image
	// //IR_IM_VGA_LEFF: VGA,Left
	// //IR_IM_VGA_RIGHT: VGA,right
	// //IR_IM_VGA_UNDEF: Unkown left or right
	// //IR_IM_NONE: there is no valid image
	// int imType;

	// //Code of iris localization method, defined as follows:
	// //IR_LOC_DEFAULT: Default technique determined by CASIA_GEN algorithm.
	// //IR_LOC_CIRCLE : Use circle fitting strategy to localize the iris edges.
	// //IR_LOC_SNAKE  : Use edge fitting strategy (e.g. active contour) to localize the iris edges.
	// int locMethod;

	// //Specify whether to enable spoof eye detection
	// //IR_NO_SPOOF_DETECT : Do not perform spoof eye detection
	// //IR_DO_SPOOF_DETECT : Perform spoof eye detection
	// int ifSpoofDetect;

	// //In which status the iris image is processed
	// //IR_STATUS_BEGIN 
	// //IR_STATUS_IRISDETECT 
	// //IR_STATUS_SEGMENTATION          
	// //IR_STATUS_QUALITYEVAL
	// //IR_STATUS_CODED
	// int processStatus;


	// //The row index of pupil center in iris image, ranging from 1 to IR_IMAGE_HEIGHT.
	// float pupilRow;

	// //The column index of pupil center in iris image,ranging from 1 toIR_IMAGE_WIDTH
	// float pupilCol;

	// //The radius of pupil in iris image
	// float pupilRadius;


	// //The row index of iris center in iris image, ranging from 1 to IR_IMAGE_HEIGHT
	// float irisRow;

	// //The column index of iris center in iris image, ranging from 1 to IR_IMAGE_WIDTH
	// float irisCol;

	// //The radius of iris in iris image
	// float irisRadius;


	// //The overall percentage of visible part of the annular iris region
	// //int overallVisiblePercent;
	// //以上几个参数无实际意思
	// //#######################################################################


	// //Indicator of the focus level of the iris image, ranging from 0 to 100. High value of focusScore means clear iris image and low value of focusScore means defocused iris image.
	// int focusScore; 

	// //Percentage of the visible part of the iris., ranging from 0 to 100. 100 means no occlusion while 65 means 65 percent of the iris are occluded.
	// //ISO.29794.6 specifies
	// int percentVisible;

	// // Saturation level of the iris image.
	// int saturation; 

	// // Indicator of the how much the image is less interlaced. Higher value indicates the iris image is less interlaced.
	// int interlaceValue; 

	// // Confidence of the input iris image is obtained from spoof eye. 
	// // 0 means the image is obtained from genuine eye, while 100 means the image is a spoof one.
	// int spoofValue; 


	// //Indicator of reflection occlusion within the ROI region of the iris image.
	// //This parameter is obtained based on the results of iris localization.
	// //Higher value indicates more reflection ratio. ranging from 0-100.
	// //where 100 means all the region within the limbic circle is occluded by reflections.
	// //int  reflectionRatioInROI; 

	// //Indicator of reflection occlusion within the entire iris image.
	// //This parameter is obtained before iris localization.
	// //Higher value indicates more reflection ratio. ranging from 0-100.
	// //where 100 means all the region within the limbic circle is occluded by reflections.
	// //int  reflectionRatioInImage; 

	// // The overall quality of the iris image, can be one of the following levels.
	// //IR_IMAGE_QUALITY_BAD      
	// //IR_IMAGE_QUALITY_POOR     
	// //IR_IMAGE_QUALITY_FAIR     
	// //IR_IMAGE_QUALITY_GOOD     
	// //IR_IMAGE_QUALITY_EXCELLENT
	// int	QualityLevel;
	// //最终的质量分数，与QualityLevel是相对应的
	// int QualityScore;


	// //1D array of pixel intensity values of the original iris image
	// unsigned char imageData[IR_IMAGE_SISE]; 
	// //1D array of pixel intensity values of the demo iris image. Pupil/iris and iris/sclera boundary points are marked with white pixels.
	// unsigned char locImage[IR_IMAGE_SISE];

	// //Iris image feature template enrolled into database, with fixed length 512 bytes.
	// unsigned char irisEnrTemplate[IR_ENR_FEATURE_LENGTH];
	// //Iris image feature template of recognition image, with fixed length 1024 bytes.
	// unsigned char irisRecTemplate[IR_REC_FEATURE_LENGTH];
	// int irisInfoUnkonwn[IR_UnkonwIrisInfoLen];
// } IrisImageInfo, *pIrisImageInfo;



#ifdef WIN32
#	ifdef IK_IR_EXPORTS
#		define IK_IR_API __declspec(dllexport)
#	else
#		define IK_IR_API __declspec(dllimport)
#	endif
#else
#	define IK_IR_API 
#endif 


typedef void *HANDLE;


extern "C" IK_IR_API
	HANDLE IKIR_HandleInit(char *strFunRts);

extern "C" IK_IR_API
	int IKIR_HandleRelease(HANDLE hIKIRHandle);

extern "C" IK_IR_API
	int IKIR_SetDefaultImageInfo(pIrisImageInfo pIrisImage,char *strFunRts);

extern "C" IK_IR_API
	int IKIR_ClsInit( );

extern "C" IK_IR_API
	int IKIR_ClsRelease( );

extern "C" IK_IR_API
	// 从脸部图像中检测眼睛，并输出结果
	int IKIR_IrisDetection_Eye(HANDLE hIKIRHandle, unsigned char* surImage, int surImH, int surImW, 
									pEyeDetectionInfo2 detectResults, char *strFunRts);

#endif	//_IK_USB_H
