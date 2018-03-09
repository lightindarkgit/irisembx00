/******************************************************************************************
** 文件名:   ApiGetFlag.cpp
×× 主要类:   CAPIGetFlag
**
** Copyright (c) 中科虹霸有限公司
** 创建人:   lizhl
** 日  期:   2013-11-29
** 修改人:
** 日  期:
** 描  述:   IKEMBX00项目算法逻辑API用于获取虹膜图像是否清晰、距离信息标志，对外不可见
 *×× 主要模块说明: ApiGetFlag.h                  获取虹膜图像是否清晰、距离信息标志类
**
** 版  本:   1.0.0
** 备  注:
**
*****************************************************************************************/
#include "ApiGetFlag.h"
#include "../Common/Exectime.h"

/*****************************************************************************
*                         CAPIGetFlag构造函数
*  函 数 名：CAPIGetFlag
*  功    能：构造函数
*  说    明：
*  参    数：NULL
*  返 回 值：NULL
*  创 建 人：lizhl
*  创建时间：2013-11-29
*  修 改 人：
*  修改时间：
*****************************************************************************/
CAPIGetFlag::CAPIGetFlag()
{
	Exectime etm(__FUNCTION__);
    _distanceValue = 0;
    _totalDistanceValue = 0;
}

/*****************************************************************************
*                         CAPIGetFlag析构函数
*  函 数 名：~CAPIGetFlag
*  功    能：析构函数
*  说    明：
*  参    数：NULL
*  返 回 值：NULL
*  创 建 人：lizhl
*  创建时间：2013-11-29
*  修 改 人：
*  修改时间：
*****************************************************************************/
CAPIGetFlag::~CAPIGetFlag()
{
	Exectime etm(__FUNCTION__);

}

/*****************************************************************************
*                         CAPIGetFlag清空函数
*  函 数 名：Clear
*  功    能：清空函数
*  说    明：
*  参    数：NULL
*  返 回 值：NULL
*  创 建 人：lizhl
*  创建时间：2013-11-29
*  修 改 人：
*  修改时间：
*****************************************************************************/
void CAPIGetFlag::Clear()
{
	Exectime etm(__FUNCTION__);
    _distanceValue = 0;
    _totalDistanceValue = 0;
}

/*****************************************************************************
*                         CAPIGetFlag获取距离信息标志函数
*  函 数 名：GetDistanceFlag
*  功    能：获取距离信息标志
*  说    明：
*  参    数：imgInformCount：输入参数，当前图像计数
*           imgNumToInform：输入参数，每imgNumToInform幅图像需要计算标志
*           pIrisPositionFlag：输出参数，距离信息标志
*  返 回 值：NULL
*  创 建 人：lizhl
*  创建时间：2013-11-29
*  修 改 人：
*  修改时间：
*****************************************************************************/
void CAPIGetFlag::GetDistanceFlag(IN int imgInformCount, IN int imgNumToInform, OUT IrisPositionFlag *pIrisPositionFlag)
{
	Exectime etm(__FUNCTION__);
    //获取红外测距模块的距离值
    //_distanceValue = GetADValue();

#ifdef ALGAPITEST
    int tempInformCount;
    if((imgInformCount%6) == 0)
    {
        tempInformCount = imgInformCount - 1;
    }
    else
    {
        tempInformCount = imgInformCount;
    }

    if((tempInformCount % 6) <=3)
    {
        _distanceValue = 25;
    }
    else
    {
        _distanceValue = 35;
    }
#endif

//    _totalDistanceValue += _distanceValue;
    _totalDistanceValue += g_distanceValue;
//    g_distanceValue = 0;



    if( 0 == (imgInformCount % imgNumToInform) )
    {
        _distanceValue = _totalDistanceValue / imgNumToInform;

		if(NoSoundDistanceValue == _distanceValue)
		{
            *pIrisPositionFlag = Unknown;
		}
        else if(_distanceValue >= FarDistanceValue)
        {
            *pIrisPositionFlag = Far;
        }
        else if( (FarDistanceValue > _distanceValue) && ( _distanceValue > NearDistanceValue))
        {
            *pIrisPositionFlag = OK;
        }
        else if(_distanceValue <= NearDistanceValue)
        {
            *pIrisPositionFlag = Near;
        }
        else
        {
            *pIrisPositionFlag = Unknown;
        }

        _totalDistanceValue = 0;
        _distanceValue = 0;
    }
//    else
//    {
//        *pIrisPositionFlag = Unknown;
//    }
}

/*****************************************************************************
*                         CAPIGetFlag获取图像清晰标志位
*  函 数 名：GetClearFlag
*  功    能：获取图像清晰标志位
*  说    明：
*  参    数：imgInformCount：输入参数，当前图像计数
*           imgNumToInform：输入参数，每imgNumToInform幅图像需要计算标志
*           leftQualifiedImgCount：输入参数，左眼合格图像个数
*           rightQualifiedImgCount：输入参数，右眼合格图像个数
*           pLRIrisClearFlag：输出参数，图像清晰标志位
*
*  返 回 值：NULL
*  创 建 人：lizhl
*  创建时间：2013-11-29
*  修 改 人：
*  修改时间：
*****************************************************************************/
void CAPIGetFlag::GetClearFlag(int imgInformCount, int imgNumToInform, int leftQualifiedImgCount, int rightQualifiedImgCount, LRIrisClearFlag *pLRIrisClearFlag)
{
	Exectime etm(__FUNCTION__);
    if( 0 == (imgInformCount % imgNumToInform) )
    {
        if( (leftQualifiedImgCount > 0) && (rightQualifiedImgCount > 0) )
        {
            *pLRIrisClearFlag = LAndRImgClear;
        }
        else if(leftQualifiedImgCount > 0)
        {
            *pLRIrisClearFlag = LImgClear;
        }
        else if(rightQualifiedImgCount > 0)
        {
            *pLRIrisClearFlag = RImgClear;
        }
        else
        {
            *pLRIrisClearFlag = LAndRImgBlur;
        }
    }
//    else
//    {
//        *pLRIrisClearFlag = LAndRImgBlur;
//    }
}
