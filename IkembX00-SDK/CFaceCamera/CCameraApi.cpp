/******************************************************************************************
** 文件名:   AlgApi.cpp
×× 主要类:   CAlgApi,CCameraApi
**  
** Copyright (c) 中科虹霸有限公司
** 创建人:   lizhl
** 日  期:   2013-11-05
** 修改人:   
** 日  期:
** 描  述:   IKEMBX00项目算法逻辑API,为该项目提供虹膜图像采集,虹膜注册,虹膜识别等功能
 *×× 主要模块说明: AlgApi.h                               算法逻辑类                     
**
** 版  本:   1.0.0
** 备  注:   
**
*****************************************************************************************/

#include "CameraBase.h"

/*****************************************************************************
*                         CFaceCameraApi构造函数
*  函 数 名：CCameraApi
*  功    能：构造函数
*  说    明：
*  参    数：NULL
*  返 回 值：NULL
*  创 建 人：lizhl
*  创建时间：2013-11-05
*  修 改 人：
*  修改时间：
*****************************************************************************/
CCameraApi::CCameraApi(void)
{
    _pCCameraBase = CCameraBase::GetInstance();
    _initFlag = false;
}

/*****************************************************************************
*                         CFaceCameraApi析构函数
*  函 数 名：CCameraApi
*  功    能：析构函数
*  说    明：
*  参    数：NULL
*  返 回 值：NULL
*  创 建 人：lizhl
*  创建时间：2013-11-05
*  修 改 人：guodj
*  修改时间：20170404
*****************************************************************************/
CCameraApi::~CCameraApi(void)
{
    if(_pCCameraBase)
    {
       // _pCCameraBase = NULL;
       // delete _pCCameraBase;
        _pCCameraBase = NULL;
    }
}

/*****************************************************************************
*                         人脸摄像头API获取图像尺寸信息
*  函 数 名：GetImgInfo
*  功    能：获取设备获得的人脸图像信息的尺寸信息
*  说    明：
*  参    数：imgSizeInfo：输出参数，图像尺寸信息
*
*            
*  返 回 值：0：获取图像尺寸信息成功
*			 <0：获取图像尺寸信息失败
*  创 建 人：lizhl
*  创建时间：2013-11-05
*  修 改 人：
*  修改时间：
*****************************************************************************/
int CCameraApi::GetImgInfo(OUT ImgSizeInfo *imgSizeInfo)
{
	imgSizeInfo->ImgHeight = g_constFaceImgHeigth;
	imgSizeInfo->ImgWidth = g_constFaceImgWidth;
	imgSizeInfo->ImgSize = g_constFaceImgSize;
    return E_ALGAPI_OK;
}



/*****************************************************************************
*                         人脸摄像头API初始化采集人脸图像摄像头
*  函 数 名：Init
*  功    能：初始化采集人脸图像摄像头
*  说    明：
*  参    数：
*            
*  返 回 值：0：初始化成功
*			 <0：初始化失败
*  创 建 人：lizhl
*  创建时间：2013-11-05
*  修 改 人：
*  修改时间：
*****************************************************************************/
int CCameraApi::Init()
{
    if(!_pCCameraBase)
    {
        return E_ALGAPI_CAMERA_INIT_FAILED;
    }
    char strFunRts[260];

    int funResult = _pCCameraBase->Init(strFunRts);
    if(E_ALGAPI_OK == funResult)
    {
        _initFlag = true;
    }
    return funResult;

}

/*****************************************************************************
*                         人脸摄像头API采集人脸图像
*  函 数 名：CapImg
*  功    能：采集人脸图像，采集到的图像可用于实时显示
*  说    明：
*  参    数：pFaceImg：输出参数，采集到的图像，由用户分配内存空间
*
*            
*  返 回 值：0：采集人脸图像成功
*    		<0：采集人脸图像失败，根据返回值可以查询失败原因，或者参考strFunRts
*  创 建 人：lizhl
*  创建时间：2013-11-05
*  修 改 人：
*  修改时间：
*****************************************************************************/
int CCameraApi::CapImg(OUT unsigned char *pFaceImg)
{
    if( (!_pCCameraBase) || (!_initFlag) )
    {
        return E_ALGAPI_CAMERA_INIT_FAILED;
    }

    char strFunRts[260];
    return _pCCameraBase->CapImg(pFaceImg, strFunRts);
}

/*****************************************************************************
*                         释放采集人脸图像摄像头相关资源
*  函 数 名：Release
*  功    能：释放采集人脸图像摄像头相关资源
*  说    明：
*  参    数：
*            
*  返 回 值：NULL
*  创 建 人：lizhl
*  创建时间：2013-11-05
*  修 改 人：guodj
*  修改时间：20170404
*****************************************************************************/
void CCameraApi::Release(bool bCloseCamera )
{
    if(_pCCameraBase)
    {
	    return;
//        _initFlag = false;
//        _pCCameraBase->Release();
        if(bCloseCamera)
        {
//            _initFlag = false;
//            _pCCameraBase->Release();
        }
        

         //_pCCameraBase = NULL;
 //       delete _pCCameraBase;
        _pCCameraBase = NULL;

    }
}
