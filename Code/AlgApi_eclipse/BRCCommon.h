/******************************************************************************************
** 文件名:   BRCCommon.h
×× 主要类:   NULL
**
** Copyright (c) 中科虹霸有限公司
** 创建人:   lizhl hezhf
** 日  期:   2013-11-18
** 修改人:
** 日  期:
** 描  述:   算法相关处理的通用函数
 *×× 主要模块说明：
**
** 版  本:   1.0.0
** 备  注:
**
*****************************************************************************************/
#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "IK_IR_DLL.h"
#include "AlgApi.h"

//函数返回结果字符串需要分配的长度
const int g_constLengthOfStrFunRts  =   260;        //函数返回结果字符串的长度

const int g_constIREnrollImgNum     =   3;          //注册过程中算法使用的注册图像数
const int g_constIREnrollTimeNum = g_constIREnrollImgNum+2;//为注册开辟的空间

const float g_constMatchScoreWeight = 0.95;         //注册过程中，选择图像时比对分数所占权重

//注册时结果标记
//#define IK_Enr_UpdateWithNewIm		2120	//注册过程中，当前图像质量较好，替换或者直接加入到注册图像中，对已有的注册结果进行了更新
//#define IK_Enr_DiscardNewIm			2121	//注册过程中，当前图像质量一般，没有改变原来已有的注册结果
const int g_constEnrUpdateWithNewImg    =   2120;   //注册过程中，当前图像质量较好，替换或者直接加入到注册图像中，对已有的注册结果进行了更新
const int g_constEnrDiscardNewImg       =   2121;   //注册过程中，当前图像质量一般，没有改变原来已有的注册结果

//20130427增加一个数据结构用来存储注册图像相互比对分数等信息

//#define IK_MaxMatchScoreTh      0.9
const float g_constIRMaxMatchScoreTh    =   0.9;
//#define IK_MaxMatchFailedNum	5
const int g_constIRMaxMatchFailedNum    =   5;
//#define IK_CompositeScoreInitValue  2
const int g_constIRCompositeScoreInitValue = 2;//综合比对分数初始值
//#define IK_CompositeScoreMinValue	-5
const int g_constIRCompositeScoreMinValue = -5;

//注册过程中图像比对信息结构体
struct IKIREnrMatchImInfo
{
    //当前注册图像的综合分数，由scoretable[focus][occlusion]得到
    float QualityScore;	//int
    //matchScore[0],matchScore[1]分别表示当前图像与最好的第一幅注册图像、第二幅注册图像之间的比对分数，由IK_Match得到
    float MatchScore[g_constIREnrollImgNum-1];
    //第一副注册图像的compositeScore[0]为初始值,compositeScore[1]无意义
    //第二幅注册图像的compositeScore[1]为初始值
    float CompositeScore[g_constIREnrollImgNum-1];
    //图像与其它注册图像比对时的累计比对失败次数，初始化为0
    //比对成功一次，该值-1
    //比对失败一次，该值+1
    float FailedMatchCount;//int
};


//^^^^^^^^^^^^^^^^^^^^^^^^
//bmp图像格式相关定义
#pragma pack(push, 1)

typedef unsigned char  U8;
typedef unsigned short U16;
typedef unsigned int   U32;

typedef struct tagBITMAPFILEHEADER
{

 U16 bfType;
 U32 bfSize;
 U16 bfReserved1;
 U16 bfReserved2;
 U32 bfOffBits;
} BITMAPFILEHEADER,*LPBITMAPFILEHEADER;

typedef struct tagBITMAPINFOHEADER
{
 U32 biSize;
 U32 biWidth;
 U32 biHeight;
 U16 biPlanes;
 U16 biBitCount;
 U32 biCompression;
 U32 biSizeImage;
 U32 biXPelsPerMeter;
 U32 biYPelsPerMeter;
 U32 biClrUsed;
 U32 biClrImportant;
} BITMAPINFOHEADER,*LPBITMAPINFOHEADER;

typedef struct tagRGBQUAD
{
 U8 rgbBlue;
 U8 rgbGreen;
 U8 rgbRed;
 U8 rgbReserved;
} RGBQUAD;

typedef struct tagBITMAPINFO
{
 BITMAPINFOHEADER bmiHeader;
 RGBQUAD bmiColors[1];
} BITMAPINFO;


typedef struct tagBITMAP
{
 BITMAPFILEHEADER bfHeader;
 BITMAPINFO biInfo;
}BITMAPFILE;

#pragma pack(pop)
//vvvvvvvvvvvvvvvvvvvvvv

void    CASIAGenInitIrisInfo(OUT pIrisImageInfo pDestImginfo);                                      //初始化
void    CASIAGenCopyIrisInfo(OUT pIrisImageInfo pDestImginfo,IN pIrisImageInfo pSrcImgInfo);           //pIrisImageInfo结构体间拷贝
void    IKInitAPIIrisInfo(OUT APIIrisInfo *pDestAPIIrisInfo);                                           //初始化APIIrisInfo
void    IKCopyIRIrisInfo2APIIrisInfo(OUT APIIrisInfo* pDestAPIIrisInfo, IN pIrisImageInfo pSrcAPIIrisInfo);  //pIrisImageInfo拷贝到APIIrisInfo
void    IKCopyAPIIrisInfo2IRIrisInfo(OUT pIrisImageInfo pDestImginfo, IN APIIrisInfo* pSrcAPIIrisInfo);  //APIIrisInfo拷贝到pIrisImageInfo

void    IKInsertIRMtOutput2APIMtOutput(OUT APIMatchOutput* pAPIOutputDest, IN MatchOutput* pIROutputSrc, IN int leftRightFlag);//比对信息插入
void    IKInitAPIMatchOutput(OUT APIMatchOutput *pAPIMatchOutput); //初始化APIMatchOutput信息
void    SetFunRt(OUT char *strFunRts, IN const int errCode, IN const char *strFunResults);                //设置输出字符串信息

/*****************************************************************************
*                         获取最小值
*  函 数 名：GetMinValue
*  功    能：获取最小值
*  说    明：
*  参    数：value1：输入参数
*           value2：输入参数
*  返 回 值：最小值
*  创 建 人：lizhl
*  创建时间：2013-11-22
*  修 改 人：
*  修改时间：
*****************************************************************************/
template <typename Tmin>
Tmin    GetMinValue(IN Tmin value1, IN Tmin value2)
{
    return value1 < value2 ? value1 : value2;
}

//注册特征
//返回值说明
//2120 注册过程中，当前图像质量较好，替换或者直接加入到注册图像中，对已有的注册结果进行了更新
//2121 注册过程中，当前图像质量一般，没有改变原来已有的注册结果
int  IKEnrollLRFeature(IN HANDLE pIRAlgHandle, IN int ifSpoofDetect,  OUT pIrisImageInfo pEnrImages, OUT int* existedNum,
                       OUT pIrisImageInfo curBestIm, OUT IKIREnrMatchImInfo* pEnrMatchImInfo,
                       OUT IKIREnrMatchImInfo* pCurBestEnrMatchImInfo, OUT char *strFunRts);//注册特征

//从N幅图像中选择一幅好图像给注册或识别
int IKSelectImg(HANDLE pIRAlgHandle, int minSelectCount, int qualityMode, pIrisImageInfo pCurIrisImg, pIrisImageInfo pBestIrisImg, char *strFunRts);

//识别特征
int IKIdenFeature(HANDLE pIRAlgHandle, unsigned char* codeList, int codeNum, pIrisImageInfo pBestIrisImg, MatchInput inputPara, pMatchOutput pMatchOutputPara, char* strFunRts);

//图像镜像
bool ImageMirror(unsigned char *objIm, unsigned char *srcIm, int nImH, int nImW, bool bDirection);

//特征装载
int FeatrueLoad(const char* fileName, unsigned char *irisFeat, int featLen);

//特征保存
int FeatureSave(const char* fileName, unsigned char *irisFeat, int featLen);

//保存ppm图像，pImgBuffer为裸数据（不包含ppm格式文件头），其大小为width*height
int SaveGrayPPMFile(const char* fileName, unsigned char *pImgBuffer, int width, int height);

//保存bmp图像，pImgBuffer为裸数据（不包含bmp格式文件头），其大小为width*height
int SaveGrayBMPFile(const char* filename, unsigned char* pImgBuffer, int height, int width);

//给裸数据加bmp头，将其转换为完整的bmp图像格式，rawBuffer为裸数据（不包含bmp格式文件头），其大小为width*height；
//bmpBuffer为完整的bmp图像，其大小为width*height+g_constBMPHeaderLength
int ConvertRaw2Bmp(unsigned char* bmpBuffer, unsigned char* rawBuffer, int height, int width);
