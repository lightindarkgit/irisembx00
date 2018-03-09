/******************************************************************************************
** 文件名:   UnitTestAlgApi.h
×× 主要类:   CUnitTestAlgApi
**
** Copyright (c) 中科虹霸有限公司
** 创建人:   lizhl
** 日  期:   2013-12-13
** 修改人:
** 日  期:
** 描  述:   AlgApi单元测试头文件
 *×× 主要模块说明:申明一个算法逻辑单元测试类，并完成相关测试
**
** 版  本:   1.0.0
** 备  注:
**
*****************************************************************************************/
#pragma once
#include "IK_IR_DLL.h"
#include "AlgApi.h"
#include "gtest/gtest.h"
#include "VirtualReadImg.h"

//最大存储注册特征数量
const int g_constFeatureNum = 10;
//采图重复次数
const int g_constCapRepeatNum = 100;//0;//
//注册重复次数
const int g_constEnrRepeatNum = 5;//0;//
//识别重复次数
const int g_constRecRepeatNum = 100;//0;//100;

//异步采图重复次数
const int g_constAsyncCapRepeatNum = 10;//0;//
//为了方便不断重复异步采图，设置一个标志位
static bool s_flagAsyncCapImg;

//异步注册重复次数
const int g_constAsyncEnrRepeatNum = 2;//0;//
//为了方便不断重复异步注册，设置一个标志位
static bool s_flagAsyncEnr;

//异步识别重复次数
const int g_constAsyncRecRepeatNum = 5;//0;//
//为了方便不断重复异步识别，设置一个标志位
static bool s_flagAsyncRec;


//回调函数
//异步采图回调函数
int CBAsyncCapIrisImg(int funResult, unsigned char* pIrisImg, LRIrisClearFlag lrIrisClearFlag, IrisPositionFlag lrIrisPositionFlag);

//异步注册回调函数
int CBAsyncEnroll(int funResult, APIIrisInfo* enrIrisL, int numEnrL, APIIrisInfo* enrIrisR, int numEnrR, LRSucFailFlag lrFlag);

//异步识别回调函数
int CBAsyncIden(int funResult, APIIrisInfo* recIrisL, APIIrisInfo* recIrisR, LRSucFailFlag lrFlag, APIMatchOutput* pLeftMatchOutputParam, APIMatchOutput* pRightMatchOutputParam);


//AlgApi单元测试类
class CUnitTestAlgApi : public testing::Test
{
public:
    CAlgApi *PTestAlgApi;
    int FunResult;

    //采图相关参数
    unsigned char ImgBuffer[g_constIrisImgSize];
    LRIrisClearFlag ImgClearFlag;
    IrisPositionFlag ImgPositionFlag;

    //获取、设置参数相关参数
    AlgApiCtrlParam ParamStruct;

    //注册相关参数
    APIIrisInfo EnrIrisL[g_constMaxEnrollImNum];
    APIIrisInfo EnrIrisR[g_constMaxEnrollImNum];
    int NumEnrL;
    int NumEnrR;
    LRSucFailFlag EnrLrFlag;

    //识别相关参数
    unsigned char CodeListL[g_constFeatureNum*IR_ENR_FEATURE_LENGTH];
    int CodeNumL;
    unsigned char CodeListR[g_constFeatureNum*IR_ENR_FEATURE_LENGTH];
    int CodeNumR;
    APIIrisInfo RecIrisL;
    APIIrisInfo RecIrisR;
    LRSucFailFlag RecLrFlag;
    APIMatchOutput LeftMatchOutputParam;
    APIMatchOutput RightMatchOutputParam;

    //比对相关参数
    int RecNum;
    unsigned char *CodeList;
    int CodeNum;
    APIMatchOutput MatchOutputParam;

public:
    virtual void SetUp()
    {
        //该函数在每个case执行前都会被调用，用于初始化相关资源
        PTestAlgApi = new CAlgApi();
        NumEnrL = 3;
        NumEnrR = 3;

        s_flagAsyncCapImg = false;
        s_flagAsyncEnr = false;
        s_flagAsyncRec = false;

        CodeNumL = g_constFeatureNum;
        CodeNumR = g_constFeatureNum;

        //清空特征
        memset(CodeListL, 0, g_constFeatureNum*IR_ENR_FEATURE_LENGTH);
        memset(CodeListR, 0, g_constFeatureNum*IR_ENR_FEATURE_LENGTH);
    }

    virtual void TearDown()
    {
        //该函数在每个case执行后都会被调用，用于销毁相关资源
        delete PTestAlgApi;
    }
};
