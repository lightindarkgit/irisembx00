/******************************************************************************************
** �ļ���:   AlgApi.h
���� ��Ҫ��:   CAlgApi,CCameraApi
**  
** Copyright (c) �пƺ�����޹�˾
** ������:   lizhl
** ��  ��:   2013-11-05
** �޸���:   
** ��  ��:
** ��  ��:   IKEMBX00��Ŀ�㷨�߼�API,Ϊ����Ŀ�ṩ��Ĥͼ��ɼ�,��Ĥע��,��Ĥʶ��ȹ���
 *���� ��Ҫģ��˵��: AlgApi.h                               �㷨�߼���,����ͼ��ɼ���                     
**
** ��  ��:   1.0.0
** ��  ע:   
**
*****************************************************************************************/
#pragma once

//#include <tr1/memory>
#include "common.h"

//Ϊ��Ԫ���Զ���ĺ�
#define ALGAPITEST_NO

const int g_constIrisImgHeight  = 704;//1536;//���޸�
const int g_constIrisImgWidth   = 2048;//���޸�
const int g_constIrisImgSize    = ((g_constIrisImgHeight)*(g_constIrisImgWidth));

//"P5\n2048 704\n255\n";
const int g_constIrisPPMHeaderLength = 16;

const int g_constFaceImgHeigth  = 400;//���޸�
const int g_constFaceImgWidth   = 300;//���޸�
const int g_constFaceImgSize    = ((g_constFaceImgHeigth)*(g_constFaceImgWidth));

//�㷨����ı�׼��Ĥ�ߴ���Ϣ
const int g_constIKImgHeight        =   480;		//��Ĥͼ��ĸ߶�
const int g_constIKImgWidth         =   640;		//��Ĥͼ��Ŀ��
const int g_constIKImgSize          =   307200;		//��Ĥͼ���С
const int g_constIKEnrFeatureLen    =	512;        //��Ĥע�������Ĵ�С
const int g_constIKRecFeatureLen    =   1024;       //��Ĥʶ�������Ĵ�С

//ע�ᡢʶ��ɹ�ʧ�ܱ�־
enum LRSucFailFlag
{
	EnrRecBothFailed	=	-3,		//EnrRecBothFailed��ע�ᡢʶ��ʱ�����۾�ע���ʶ��ʧ��
	EnrRecRightFailed	=	-2,		//EnrRecRightFailed:����ע�ᡢʶ��ʧ�ܵ���ע���ʶ��ʧ��,�������δ֪
	EnrRecLeftFailed	=	-1,		//EnrRecLeftFailed:����ע�ᡢʶ��ʧ�ܵ���ע���ʶ��ʧ��,�������δ֪
    EnrRecUnknown       =   0,      //EnrRecUnknown:ע�ᡢʶ����δ֪
    EnrRecLeftSuccess	=	1,		//EnrRecLeftSuccess:ע�ᡢʶ��ʱ������ע���ʶ��ɹ�
	EnrRecRightSuccess	=	2,		//EnrRecRightSuccess:ע�ᡢʶ��ʱ������ע���ʶ��ɹ�
	EnrRecBothSuccess	=	3		//EnrRecBothSuccess:ע�ᡢʶ��ʱ�����۾�ע��ɹ�
};

//ͼ��������־
enum LRIrisClearFlag
{
	LAndRImgBlur	=	0,	//���ۡ�����ͼ�񶼲��ϸ�
	LImgClear		=	1,	//����ͼ��ϸ�
	RImgClear		=	2,	//����ͼ��ϸ�
	LAndRImgClear	=	3	//���ۡ�����ͼ�񶼺ϸ�
};

//λ����Ϣ��־
enum IrisPositionFlag
{
    Far				=	0,	//λ��ƫԶ
    Near			=	1,  //λ��ƫ��
    Left			=	2,  //λ��ƫ��
    Right			=	3,  //λ��ƫ��
    Up				=	4,  //λ��ƫ��
    Down			=	5,  //λ��ƫ��
    OK				=	6,  //λ�ú���
    Unknown         =   7   //λ��δ֪��������ʾ
};

//ע��ͼ�����
const int g_constMaxEnrollImNum =	3;		//ע��������㷨��������ע��ͼ����

//�豸 �㷨�߼����Ƶ��������
//����1. eyeMode������ͼ��ע�ᡢʶ��ʱ���۾�ģʽ
enum APIEyeMode
{
    BothEye         =   0,      //˫��
    LeftEye         =   1,      //����
    RightEye        =   2,      //����
    AnyEye          =   3       //�����ۣ�ע��ģʽ�£���������Ч
};

//����2. ע��ʱ�������Сʱ�䡢ʶ��ʱ��������ʱ�䣬��λ��s������ʹ��Ĭ��ֵ
const int g_constMinENRTime         =	10;		//Ŀǰʵ�ַ�ʽ�ĵ�λ���룬ע����̱��볬�����ʱ��Ż���ע��ɹ����û����õ�ע��ʱ����벻С�ڸ�ֵ��
                                                //�������û�������15����0-10�����ʱ�䣬������ע������10-15��֮�䣬���ע��ɹ��򷵻سɹ������ɹ������������15�뻹û�гɹ��򷵻�ʧ��
const int g_constMaxRECTime         =	5;		//��λ�룬���ʶ����̳������ʱ�䣬�򷵻�ʧ��

//����3. findMode�����ǡ��������������ǡ��ҵ���ͣ��,����ʹ��Ĭ��ֵ g_constFindModeExhaust
const int g_constFindModeExhaust    =	0;		//����������Ĭ��ֵ
const int g_constFindModeFirstMatch =   1;		//�ҵ���ͣ

//����4.�Ƿ�ִ�м��ۼ��
const int g_constIKNoSpoofDetect    =	0;		//��ִ�м��ۼ�⣬Ĭ��ֵ
const int g_constIKDoSpoofDetect    = 	1;		//ִ�м��ۼ��

//�ȶ��������
const int g_constMaxNumOfMatch      =	100;	//�ȶԹ����㷨��෵�صıȶԽ����
//left2right
const int g_constMtIsLeftEye        =   01;     //�ȶԻ���ʶ���������Ϊ���۵����
const int g_constMtIsRightEye       =   02;     //�ȶԻ���ʶ���������Ϊ���۵����
const int g_constMtUnknowEye        =   0;      //�Աȶ�ͼ������ۻ���������Ϣ����ȷ,��������û�����IK_AnyEyeʱ���ܳ���

//�������ô�����

const int E_ALGAPI_ERROR_BASE               =       0;      //����������

//��ϵͳ��صĶ��壬��1***��ͷ
const int E_ALGAPI_INIT_FAILED              =	E_ALGAPI_ERROR_BASE	- 1001;	//API��ʼ��ʧ�ܻ�����δ��ʼ��
const int E_ALGAPI_HANDLE_ERROR             =   E_ALGAPI_ERROR_BASE	- 1002;	//����Handle��صĺ�����ʧ��
const int E_ALGAPI_INVALID_DEV_STATUS       =   E_ALGAPI_ERROR_BASE	- 1003;	//�豸״̬����,�豸��ʼ��ʧ�ܻ�ֹͣ�������豸���γ�

//��ע���ʶ���㷨�߼���ص���21**��ͷ
const int E_ALGAPI_ENR_IDEN_FAILED          =	E_ALGAPI_ERROR_BASE	- 2101;	//ע���ʶ��ʧ��
const int E_ALGAPI_FAIL2STOP                =	E_ALGAPI_ERROR_BASE	- 2102;	//����ֹͣע���ֹͣʶ����ʱʧ��
const int E_ALGAPI_ALGORITHM_CONFLICT       =   E_ALGAPI_ERROR_BASE	- 2103;	//�㷨��ͻ��ע�ᡢʶ����ͬʱ����
const int E_ALGAPI_INVALID_ENR_INPUT        =   E_ALGAPI_ERROR_BASE - 2104; //ע��ʱ��Ч������,��ע��ͼ����Ŀ��ע��ʱ�䣬EyeMode��
const int E_ALGAPI_INVALID_REC_INPUT        =   E_ALGAPI_ERROR_BASE - 2105; //ʶ��ʱ��Ч������,��ʶ��ʱ�䣬EyeMode��
const int E_ALGAPI_INVALID_MATCH_INPUT      =   E_ALGAPI_ERROR_BASE - 2106; //�ȶ�ʱ��Ч������
const int E_ALGAPI_IDEN_OVERTIME            =   E_ALGAPI_ERROR_BASE - 2107; //ʶ��ʱovertime

//�ر�ģ�����Ϊ����ֹͣע���ʶ����ʱ��֮ǰ���õ�ע�ắ����ʶ����������1
const int E_ALGAPI_ENR_IDEN_INTERRUPT       =   1;                          //ע�ᡢʶ��ʱ��APP��ֹ��ע���ʶ����̣�Ҳ���ڲɼ��ϸ�ͼ��ʱAPP��ֹ�˲ɼ��ϸ�ͼ��

//��ȶ��㷨�߼���ص���22**��ͷ
const int E_ALGAPI_MATCH_FAILED             = 	E_ALGAPI_ERROR_BASE	- 2201;  //�ȶ�ʧ��

//���ͼ��ص���23**��ͷ
const int E_ALGAPI_CAP_IMG_FAILED           =	E_ALGAPI_ERROR_BASE	- 2301;	//ͼ��ɼ�ʧ��

//���㷨���Ʋ�����ص���30**��ͷ��
const int E_ALGAPI_INVALID_EYEMODE          =   E_ALGAPI_ERROR_BASE	- 3001;	//��Ч��Eyemode
const int E_ALGAPI_INVALID_TIME             =   E_ALGAPI_ERROR_BASE	- 3002;	//��Ч��ע��ʱ���ʶ��ʱ��
const int E_ALGAPI_INVALID_FINDMODE         =   E_ALGAPI_ERROR_BASE	- 3003;	//��Ч��FindMode
const int E_ALGAPI_INVALID_IFSPOOFDETECT    =   E_ALGAPI_ERROR_BASE	- 3004; //��Ч��IfSpoofDetect
const int E_ALGAPI_INVALID_PARAMETER        =   E_ALGAPI_ERROR_BASE	- 3005; //��Ч���㷨���Ʋ���

//�����ߡ�������ص���40**��ͷ
const int E_ALGAPI_SLEEP_FAILED             =   E_ALGAPI_ERROR_BASE - 4001; //����ʧ�ܣ�ϵͳ�Ѵ�������״̬
const int E_ALGAPI_WAKE_FAILED              =   E_ALGAPI_ERROR_BASE - 4002; //����ʧ�ܣ�ϵͳ�Ѵ��ڻ���״̬
const int E_ALGAPI_SYSTEM_IS_SLEEP          =   E_ALGAPI_ERROR_BASE - 4003; //ϵͳ��������״̬����غ�������ʧ��

//��������ͨ�÷���ֵ
const int E_ALGAPI_OK                       =	0;                          //�������óɹ�����������˺�������
const int E_ALGAPI_DEFAUL_ERROR             =	E_ALGAPI_ERROR_BASE	- 8000;	//��������ȱʡ����һ��Ϊ�������ý����ʼֵ��һ�㲻���ܷ��ظ�ֵ

//�㷨�߼����Ʋ���
struct AlgApiCtrlParam
{
    APIEyeMode  EyeMode;		//ע�����ʶ��ʱ���۾�ѡ��ģʽ���������ۣ����ۣ����ۻ���˫��ģʽ
    int         MaxENRTime;     //ע��ʱ��������ʱ�䣬��λ��s��������ʱ�䱨ע�ᳬʱ��Ĭ��ֵΪ20
    int         MaxRECTime;     //ʶ��ʱ��������ʱ�䣬��λ��s
	//����Ϊʶ������п����õ��Ĳ���
    int         FindMode;		//�ǡ��������������ǡ��ҵ���ͣ��
	//g_constIKNoSpoofDetect : Do not perform spoof eye detection Ĭ��ֵ
	//g_constIKDoSpoofDetect : Perform spoof eye detection
    int         IfSpoofDetect; //�Ƿ���м��ۼ�⣬�ù�����δ��ʽ������������ܲ�׼ȷ��
};

//ʶ�������Ϣ,���û��ɼ�
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
    int Left2Right[g_constMaxNumOfMatch];//�ݲ��ù�ע

	//Match score of each match, ranging from 0 to 1, 
	//and the higher the more confidence of the match.
	float MatchScore[g_constMaxNumOfMatch];
};

//API��Ĥͼ��ṹ��Ϣ
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

	//�ۺ���focus��occlusion��blur�����صó���������������
	int QualityScore;

	//Iris image feature template enrolled into database, with fixed length 512 bytes.
	unsigned char IrisEnrTemplate[g_constIKEnrFeatureLen];

	//Iris image feature template recogonized into database, with fixed length 1024 bytes.
	unsigned char IrisRecTemplate[g_constIKRecFeatureLen];

	//hancj���鲻�õ�ɾ��
	//�������õ���չ��Ϣ
	//���ݽṹ��Ԥ������ϢΪ��Ϊ������չʹ��.��Ӧ�ó�����㷨API�Ľӿ�ý��
//	unsigned char reserveSpace[IK_Reserve_Size];
};

//��Ĥ����ͷ�ɼ�����ͼ��ߴ���Ϣ
struct ImgSizeInfo
{
	int ImgHeight;	//ͼ��߶�
	int ImgWidth;	//ͼ����
	int ImgSize;	//ͼ��ߴ�
};

// �ɼ���Ĥͼ��ص�����(funResult,pIrisImg,lrIrisClearFlag,lrIrisPositionFlag)
typedef int (*CBAlgApiCapIrisImg)(int , unsigned char* , LRIrisClearFlag , IrisPositionFlag);
//��Ĥע��ص�����(funResult, enrIrisL, numEnrL, enrIrisR, numEnrR, lrFlag)
typedef int (*CBAlgApiEnroll)(int , APIIrisInfo* , int , APIIrisInfo* , int , LRSucFailFlag);
//��Ĥʶ��ص�����(funResult, recIrisL,recIrisR,lrFlag,pLeftMatchOutputParam,pRightMatchOutputParam)
typedef int (*CBAlgApiIden)(int , APIIrisInfo* , APIIrisInfo* , LRSucFailFlag , APIMatchOutput* , APIMatchOutput* );

//ǰ������
class CAPIBase;

//�㷨�߼�API��
class CAlgApi
{
public:
    CAlgApi(void);
    ~CAlgApi(void);

public:
	//��ȡ�豸��õĺ�Ĥͼ����Ϣ�ĳߴ���Ϣ
    int		GetImgInfo(OUT ImgSizeInfo *imgSizeInfo);
    //��ʼ���㷨�߼�APIʹ�õ������Դ
    int		Init();
    //�ͷ��㷨�߼�APIʹ�õ������Դ
//    int 	Release(char *strFunRts);
	//ʹ�㷨�߼�API��������״̬�����ɼ���Ĥͼ�񣬵����ͷ������Դ
    int		Sleep();
	//ʹ�㷨�߼�API������״̬���ѣ��ָ���������״̬
    int		Wake();

	//��ȡ�㷨�߼�API���Ʋ���
    int		GetParam(OUT AlgApiCtrlParam *paramStruct);
	//�����㷨�߼�API���Ʋ���
    int		SetParam(IN AlgApiCtrlParam *paramStruct);
	//ͬ����ʽ�ɼ���Ĥͼ��
    int		SyncCapIrisImg(OUT unsigned char *pIrisImg, OUT LRIrisClearFlag &lrIrisClearFlag, OUT IrisPositionFlag &irisPositionFlag);
	//�첽��ʽ�ɼ���Ĥͼ��
    int     AsyncCapIrisImg(IN CBAlgApiCapIrisImg appCapIrisImg);

	//ͬ����ʽ��Ĥע��
    int		SyncStartEnroll(OUT APIIrisInfo *enrIrisL, IN int numEnrL, OUT APIIrisInfo *enrIrisR, IN int numEnrR, OUT LRSucFailFlag &lrFlag);
	//�첽��ʽ��Ĥע��
    int		AsyncStartEnroll(IN CBAlgApiEnroll appEnroll, IN int numEnrL, IN int numEnrR);
	//ֹͣ��Ĥע��
    int		StopEnroll();

	//ͬ����ʽ��Ĥʶ��
    int		SyncStartIden(IN unsigned char *codeListL, IN int codeNumL, IN unsigned char *codeListR, IN int codeNumR,
                      OUT APIIrisInfo *recIrisL, OUT APIIrisInfo *recIrisR, OUT LRSucFailFlag &lrFlag,
                      OUT APIMatchOutput *pLeftMatchOutputParam, OUT APIMatchOutput *pRightMatchOutputParam);
	//�첽��ʽ��Ĥʶ��
   int		AsyncStartIden(IN CBAlgApiIden appIden, IN unsigned char *codeListL, IN int codeNumL, IN unsigned char *codeListR, IN int codeNumR);

	//ֹͣ��Ĥʶ��
    int		StopIden();
	//��Ĥʶ������ģ��ȶ�
    int		Match(IN unsigned char *recCode, IN int recNum, IN unsigned char *codeList, IN int codeNum, OUT APIMatchOutput *pMatchOutputParam);

private:
    CAPIBase* _pCAPIBase;
//    std::tr1::shared_ptr<CAPIBase> _pCAPIBase;//API�ڲ�ʹ����Դ
    bool _initFlag; //�Ƿ��ʼ����־
	CAlgApi(const CAlgApi&);//��ֹ������ֵ
    CAlgApi& operator=(const CAlgApi&);
};

//����ͷAPI�࣬���ڲɼ�����ͼ�񣬸ò����ƺ�ʵ��
class CCameraApi
{
public:
    CCameraApi(void);
    ~CCameraApi(void);
public:
	//��ȡ�豸��õ�����ͼ����Ϣ�ĳߴ���Ϣ
    int		GetImgInfo(OUT ImgSizeInfo *imgSizeInfo);
	//��ʼ���ɼ�����ͼ������ͷ
    int		Init();
	//�ɼ�����ͼ��
    int		CapImg(OUT unsigned char *pFaceImg);
	//�ͷŲɼ�����ͼ������ͷ�����Դ
    void	Release();
private:
//	Handle _pHandle;//����
};
