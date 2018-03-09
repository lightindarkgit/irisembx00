/******************************************************************************************
** �ļ���:   BRCCommon.h
���� ��Ҫ��:   NULL
**
** Copyright (c) �пƺ�����޹�˾
** ������:   lizhl hezhf
** ��  ��:   2013-11-18
** �޸���:
** ��  ��:
** ��  ��:   �㷨��ش����ͨ�ú���
 *���� ��Ҫģ��˵����
**
** ��  ��:   1.0.0
** ��  ע:
**
*****************************************************************************************/
#pragma once

#include <stdio.h>
#include <stdlib.h>
//#include <string>
#include <string.h>
#include "IK_IR_DLL.h"
#include "AlgApi.h"

//�������ؽ���ַ�����Ҫ����ĳ���
const int g_constLengthOfStrFunRts  =   260;        //�������ؽ���ַ����ĳ���

const int g_constIREnrollImgNum     =   3;          //ע��������㷨ʹ�õ�ע��ͼ����
const int g_constIREnrollTimeNum = g_constIREnrollImgNum+2;//Ϊע�Ὺ�ٵĿռ�

const float g_constMatchScoreWeight = 0.95;         //ע������У�ѡ��ͼ��ʱ�ȶԷ�����ռȨ��

//ע��ʱ������
//#define IK_Enr_UpdateWithNewIm		2120	//ע������У���ǰͼ�������Ϻã��滻����ֱ�Ӽ��뵽ע��ͼ���У������е�ע���������˸���
//#define IK_Enr_DiscardNewIm			2121	//ע������У���ǰͼ������һ�㣬û�иı�ԭ�����е�ע����
const int g_constEnrUpdateWithNewImg    =   2120;   //ע������У���ǰͼ�������Ϻã��滻����ֱ�Ӽ��뵽ע��ͼ���У������е�ע���������˸���
const int g_constEnrDiscardNewImg       =   2121;   //ע������У���ǰͼ������һ�㣬û�иı�ԭ�����е�ע����

//20130427����һ�����ݽṹ�����洢ע��ͼ���໥�ȶԷ�������Ϣ

//#define IK_MaxMatchScoreTh      0.9
const float g_constIRMaxMatchScoreTh    =   0.9;
//#define IK_MaxMatchFailedNum	5
const int g_constIRMaxMatchFailedNum    =   5;
//#define IK_CompositeScoreInitValue  2
const int g_constIRCompositeScoreInitValue = 2;//�ۺϱȶԷ�����ʼֵ
//#define IK_CompositeScoreMinValue	-5
const int g_constIRCompositeScoreMinValue = -5;

//ע�������ͼ��ȶ���Ϣ�ṹ��
struct IKIREnrMatchImInfo
{
    //��ǰע��ͼ����ۺϷ�������scoretable[focus][occlusion]�õ�
    float QualityScore;	//int
    //matchScore[0],matchScore[1]�ֱ��ʾ��ǰͼ������õĵ�һ��ע��ͼ�񡢵ڶ���ע��ͼ��֮��ıȶԷ�������IK_Match�õ�
    float MatchScore[g_constIREnrollImgNum-1];
    //��һ��ע��ͼ���compositeScore[0]Ϊ��ʼֵ,compositeScore[1]������
    //�ڶ���ע��ͼ���compositeScore[1]Ϊ��ʼֵ
    float CompositeScore[g_constIREnrollImgNum-1];
    //ͼ��������ע��ͼ��ȶ�ʱ���ۼƱȶ�ʧ�ܴ�������ʼ��Ϊ0
    //�ȶԳɹ�һ�Σ���ֵ-1
    //�ȶ�ʧ��һ�Σ���ֵ+1
    float FailedMatchCount;//int
};

void    CASIAGenInitIrisInfo(OUT pIrisImageInfo pDestImginfo);                                      //��ʼ��
void    CASIAGenCopyIrisInfo(OUT pIrisImageInfo pDestImginfo,IN pIrisImageInfo pSrcImgInfo);           //pIrisImageInfo�ṹ��俽��
void    IKInitAPIIrisInfo(OUT APIIrisInfo *pDestAPIIrisInfo);                                           //��ʼ��APIIrisInfo
void    IKCopyIRIrisInfo2APIIrisInfo(OUT APIIrisInfo* pDestAPIIrisInfo, IN pIrisImageInfo pSrcAPIIrisInfo);  //pIrisImageInfo������APIIrisInfo
void    IKCopyAPIIrisInfo2IRIrisInfo(OUT pIrisImageInfo pDestImginfo, IN APIIrisInfo* pSrcAPIIrisInfo);  //APIIrisInfo������pIrisImageInfo

void    IKInsertIRMtOutput2APIMtOutput(OUT APIMatchOutput* pAPIOutputDest, IN MatchOutput* pIROutputSrc, IN int leftRightFlag);//�ȶ���Ϣ����
void    IKInitAPIMatchOutput(OUT APIMatchOutput *pAPIMatchOutput); //��ʼ��APIMatchOutput��Ϣ
void    SetFunRt(OUT char *strFunRts, IN const int errCode, IN const char *strFunResults);                //��������ַ�����Ϣ

/*****************************************************************************
*                         ��ȡ��Сֵ
*  �� �� ����GetMinValue
*  ��    �ܣ���ȡ��Сֵ
*  ˵    ����
*  ��    ����value1���������
*           value2���������
*  �� �� ֵ����Сֵ
*  �� �� �ˣ�lizhl
*  ����ʱ�䣺2013-11-22
*  �� �� �ˣ�
*  �޸�ʱ�䣺
*****************************************************************************/
template <typename Tmin>
Tmin    GetMinValue(IN Tmin value1, IN Tmin value2)
{
    return value1 < value2 ? value1 : value2;
}

//ע������
//����ֵ˵��
//2120 ע������У���ǰͼ�������Ϻã��滻����ֱ�Ӽ��뵽ע��ͼ���У������е�ע���������˸���
//2121 ע������У���ǰͼ������һ�㣬û�иı�ԭ�����е�ע����
int  IKEnrollLRFeature(IN HANDLE pIRAlgHandle, IN int ifSpoofDetect,  OUT pIrisImageInfo pEnrImages, OUT int* existedNum,
                       OUT pIrisImageInfo curBestIm, OUT IKIREnrMatchImInfo* pEnrMatchImInfo,
                       OUT IKIREnrMatchImInfo* pCurBestEnrMatchImInfo, OUT char *strFunRts);//ע������

//��N��ͼ����ѡ��һ����ͼ���ע���ʶ��
int IKSelectImg(HANDLE pIRAlgHandle, int minSelectCount, int qualityMode, pIrisImageInfo pCurIrisImg, pIrisImageInfo pBestIrisImg, char *strFunRts);

//ʶ������
int IKIdenFeature(HANDLE pIRAlgHandle, unsigned char* codeList, int codeNum, pIrisImageInfo pBestIrisImg, MatchInput inputPara, pMatchOutput pMatchOutputPara, char* strFunRts);

//ͼ����
bool ImageMirror(unsigned char *objIm, unsigned char *srcIm, int nImH, int nImW, bool bDirection);

//����װ��
int FeatrueLoad(const char* fileName, unsigned char *irisFeat, int featLen);

//��������
int FeatureSave(const char* fileName, unsigned char *irisFeat, int featLen);

//����ppmͼ��
int SaveGrayPPMFile(const char* fileName, unsigned char *pImgBuffer, int width, int height);
