/******************************************************************************************
** 文件名:   ApiGetFlag.h
×× 主要类:   CAPIGetFlag
**
** Copyright (c) 中科虹霸有限公司
** 创建人:   lizhl
** 日  期:   2013-11-29
** 修改人:
** 日  期:
** 描  述:   IKEMBX00项目算法逻辑API用于获取虹膜图像是否清晰、距离信息标志，对外不可见
 *×× 主要模块说明: ApiGetFlag.h
**
** 版  本:   1.0.0
** 备  注:
**
*****************************************************************************************/
#pragma once
#include "AlgApi.h"
#include "ApiBase.h"

//红外测距模块返回值含义
//const int g_constDistanceNearValue =        24;     //距离过近值，单位cm
//const int g_constDistanceOKValue =          29;     //距离合适值，单位cm
//const int g_constDistanceFarValue =         33;     //距离过远值，单位cm
//const int g_constDistanceNoPersonValue =    35;     //无人，单位cm

//获取虹膜图像是否清晰、距离信息标志类
class CAPIGetFlag
{
public:
    CAPIGetFlag();
    ~CAPIGetFlag();

    void Clear();
    void GetDistanceFlag(IN int imgInformCount, IN int imgNumToInform, OUT IrisPositionFlag *pIrisPositionFlag);
    void GetClearFlag(IN int imgInformCount, IN int imgNumToInform, IN int leftQualifiedImgCount, IN int rightQualifiedImgCount, OUT LRIrisClearFlag *pLRIrisClearFlag);

private:
    int _distanceValue;
    int _totalDistanceValue;

};
