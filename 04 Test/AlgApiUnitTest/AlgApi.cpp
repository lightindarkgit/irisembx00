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
#include "AlgApi.h"
#include "ApiBase.h"
#include "BRCCommon.h"

/*****************************************************************************
*                         CAlgApi构造函数
*  函 数 名：CAlgApi
*  功    能：构造函数，获取_pCAPIBase实例
*  说    明：
*  参    数：NULL
*  返 回 值：NULL
*  创 建 人：lizhl
*  创建时间：2013-11-05
*  修 改 人：
*  修改时间：
*****************************************************************************/
CAlgApi::CAlgApi(void)
{
    _pCAPIBase = CAPIBase::GetInstance();
    _initFlag = false;
//    _pCAPIBase = static_cast <CAPIBase> (CAPIBase::GetInstance());
}

/*****************************************************************************
*                         CAlgApi析构函数
*  函 数 名：~CAlgApi
*  功    能：析构函数，释放_pCAPIBase实例
*  说    明：
*  参    数：NULL
*  返 回 值：NULL
*  创 建 人：lizhl
*  创建时间：2013-11-05
*  修 改 人：
*  修改时间：
*****************************************************************************/
CAlgApi::~CAlgApi(void)
{
    if(_pCAPIBase)
    {
        _pCAPIBase = NULL;
        delete _pCAPIBase;
    }
}

/*****************************************************************************
*                         算法逻辑API获取图像尺寸信息
*  函 数 名：GetImgInfo
*  功    能：获取设备获得的虹膜图像信息的尺寸信息
*  说    明：
*  参    数：imgSizeInfo：输出参数，图像尺寸信息
*
*            
*  返 回 值：0: 获取图像尺寸信息成功
*			 <0：获取图像尺寸信息失败
*  创 建 人：lizhl
*  创建时间：2013-11-05
*  修 改 人：
*  修改时间：
*****************************************************************************/
int CAlgApi::GetImgInfo(OUT ImgSizeInfo *imgSizeInfo)
{
	imgSizeInfo->ImgHeight = g_constIrisImgHeight;
	imgSizeInfo->ImgWidth = g_constIrisImgWidth;
	imgSizeInfo->ImgSize = g_constIrisImgSize;

    return E_ALGAPI_OK;
}

/*****************************************************************************
*                         算法逻辑API初始化
*  函 数 名：Init
*  功    能：初始化算法逻辑API使用的相关资源
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
int CAlgApi::Init()
{
    if(!_pCAPIBase)
    {
        return E_ALGAPI_INIT_FAILED;
    }
    char strFunRts[g_constLengthOfStrFunRts];

    int funResult = _pCAPIBase->Init(strFunRts);
    if(E_ALGAPI_OK == funResult)
    {
        _initFlag = true;
    }
    return funResult;
}

/*****************************************************************************
*                         算法逻辑API释放
*  函 数 名：Release
*  功    能：释放算法逻辑API中使用的相关资源
*  说    明：
*  参    数：
*
*  返 回 值：0：释放成功
*			 <0：释放失败
*  创 建 人：lizhl
*  创建时间：2013-11-16
*  修 改 人：
*  修改时间：
*****************************************************************************/
//int CAlgApi::Release()
//{
//    return E_ALGAPI_OK;
//}

/*****************************************************************************
*                         算法逻辑API系统休眠
*  函 数 名：Sleep
*  功    能：使算法逻辑API进入休眠状态，不采集虹膜图像，但不释放相关资源
*  说    明：
*  参    数：
*            
*  返 回 值：0：系统休眠成功
*    		<0：系统休眠失败，根据返回值可以查询失败原因，或者参考strFunRts
*  创 建 人：lizhl
*  创建时间：2013-11-05
*  修 改 人：
*  修改时间：
*****************************************************************************/
int CAlgApi::Sleep()
{
    if((!_pCAPIBase) || (!_initFlag) )
    {
        return E_ALGAPI_INIT_FAILED;
    }
    char strFunRts[g_constLengthOfStrFunRts];
    return _pCAPIBase->Sleep(strFunRts);
}

/*****************************************************************************
*                         算法逻辑API系统唤醒
*  函 数 名：Wake
*  功    能：使算法逻辑API从休眠状态唤醒，恢复正常工作状态
*  说    明：
*  参    数：
*            
*  返 回 值：0：系统唤醒成功
*    		<0：系统唤醒失败，根据返回值可以查询失败原因，或者参考strFunRts
*  创 建 人：lizhl
*  创建时间：2013-11-05
*  修 改 人：
*  修改时间：
*****************************************************************************/
int CAlgApi::Wake()
{
    if((!_pCAPIBase) || (!_initFlag) )
    {
        return E_ALGAPI_INIT_FAILED;
    }
    char strFunRts[g_constLengthOfStrFunRts];
    return _pCAPIBase->Wake(strFunRts);
}

/*****************************************************************************
*                         算法逻辑API控制参数获取
*  函 数 名：GetParam
*  功    能：获取算法逻辑API控制参数
*  说    明：
*  参    数：paramStruct：输出参数，算法逻辑API控制参数
*
*            
*  返 回 值：0：参数获取成功
*    		<0：参数获取失败，根据返回值可以查询失败原因，或者参考strFunRts
*  创 建 人：lizhl
*  创建时间：2013-11-05
*  修 改 人：
*  修改时间：
*****************************************************************************/
int CAlgApi::GetParam(OUT AlgApiCtrlParam *paramStruct)
{
    if((!_pCAPIBase) || (!_initFlag) )
    {
        return E_ALGAPI_INIT_FAILED;
    }
    char strFunRts[g_constLengthOfStrFunRts];
    return _pCAPIBase->GetParam(paramStruct,strFunRts);
}

/*****************************************************************************
*                         算法逻辑API控制参数设置
*  函 数 名：SetParam
*  功    能：设置算法逻辑API控制参数
*  说    明：
*  参    数：paramStruct：输出参数，算法逻辑API控制参数
*
*            
*  返 回 值：0：参数获取成功
*    		<0：参数获取失败，根据返回值可以查询失败原因，或者参考strFunRts
*  创 建 人：lizhl
*  创建时间：2013-11-05
*  修 改 人：
*  修改时间：
*****************************************************************************/
int CAlgApi::SetParam(IN AlgApiCtrlParam* paramStruct)
{
    if((!_pCAPIBase) || (!_initFlag) )
    {
        return E_ALGAPI_INIT_FAILED;
    }
    char strFunRts[g_constLengthOfStrFunRts];
    return _pCAPIBase->SetParam(paramStruct,strFunRts);
}

/*****************************************************************************
*                         算法逻辑API采集虹膜图像
*  函 数 名：SyncCapIrisImg
*  功    能：同步方式采集虹膜图像，采集到的图像可用于实时显示
*  说    明：
*  参    数：pIrisImg：输出参数，采集到的图像，由用户分配内存空间
*			 lrIrisClearFlag：输出参数，表明本次采集到的图像中是否有合格的虹膜图像，该标志可用于指示界面显示红、绿提示框
*				LAndRImgBlur	=	0,	//左眼、右眼图像都不合格；
*				LImgClear		=	1,	//左眼图像合格
*				RImgClear		=	2,	//右眼图像合格
*				LAndRImgClear	=	3	//左眼、右眼图像都合格
*	   		 irisPositionFlag：输出参数，提示远近、左右、上下
*				Far				=	0,	
*				Near			=	1,
*				Left			=	2,
*				Right			=	3,
*				Up				=	4,
*				Down			=	5,
*				OK				=	6
*
*            
*  返 回 值：0：采集虹膜图像成功
*    		<0：采集虹膜图像失败，根据返回值可以查询失败原因，或者参考strFunRts
*  创 建 人：lizhl
*  创建时间：2013-11-05
*  修 改 人：
*  修改时间：
*****************************************************************************/
int CAlgApi::SyncCapIrisImg(OUT unsigned char *pIrisImg, OUT LRIrisClearFlag &lrIrisClearFlag, OUT IrisPositionFlag &irisPositionFlag)
{
    if((!_pCAPIBase) || (!_initFlag) )
    {
        return E_ALGAPI_INIT_FAILED;
    }
    char strFunRts[g_constLengthOfStrFunRts];
    return _pCAPIBase->SyncCapIrisImg(pIrisImg, lrIrisClearFlag, irisPositionFlag, strFunRts);
}

/*****************************************************************************
*                         算法逻辑API采集虹膜图像
*  函 数 名：AsyncCapIrisImg
*  功    能：异步方式采集虹膜图像，采集到的图像可用于实时显示
*  说    明：
*  参    数：appCapIrisImg：输入参数，用户注册的回调函数
*-------------------------------------------------------------------------------------------------------
*以下参数无用
*            pIrisImg：输出参数，采集到的图像，由用户分配内存空间
*			 lrIrisClearFlag：输出参数，表明本次采集到的图像中是否有合格的虹膜图像，该标志可用于指示界面显示红、绿提示框
*				LAndRImgBlur	=	0,	//左眼、右眼图像都不合格；
*				LImgClear		=	1,	//左眼图像合格
*				RImgClear		=	2,	//右眼图像合格
*				LAndRImgClear	=	3	//左眼、右眼图像都合格
*	   		 irisPositionFlag：输出参数，提示远近、左右、上下
*				Far				=	0,
*				Near			=	1,
*				Left			=	2,
*				Right			=	3,
*				Up				=	4,
*				Down			=	5,
*				OK				=	6
*---------------------------------------------------------------------------------------------------------
*  返 回 值：0：函数调用成功
*    		<0：函数调用失败，根据返回值可以查询失败原因，或者参考strFunRts
*  创 建 人：lizhl
*  创建时间：2013-11-05
*  修 改 人：
*  修改时间：
*****************************************************************************/
int	CAlgApi::AsyncCapIrisImg(IN CBAlgApiCapIrisImg appCapIrisImg)
{
    if((!_pCAPIBase) || (!_initFlag) )
    {
        return E_ALGAPI_INIT_FAILED;
    }
    char strFunRts[g_constLengthOfStrFunRts];
    return _pCAPIBase->AsyncCapIrisImg(appCapIrisImg, strFunRts);
}

/*****************************************************************************
*                         算法逻辑API开始注册
*  函 数 名：SyncStartEnroll
*  功    能：同步方式虹膜注册
*  说    明：
*  参    数：enrIrisL，enrIrisR：输出参数，注册得到的左眼、右眼虹膜图像及相关信息
*			 numEnrL，numEnrR：输入参数，注册得到的左眼、右眼虹膜图像个数，由调用者指定
*			 lrFlag：输出参数，左眼、右眼虹膜注册成功标志
*				EnrRecBothFailed	=	-3,		//EnrRecBothFailed：注册、识别时左右眼均注册或识别失败
*				EnrRecRightFailed	=	-2,		//EnrRecRightFailed:右眼注册、识别失败导致注册或识别失败,左眼情况未知
*				EnrRecLeftFailed	=	-1,		//EnrRecLeftFailed:左眼注册、识别失败导致注册或识别失败,右眼情况未知
*				EnrRecLeftSuccess	=	1,		//EnrRecLeftSuccess:注册、识别时仅左眼注册或识别成功
*				EnrRecRightSuccess	=	2,		//EnrRecRightSuccess:注册、识别时仅右眼注册或识别成功
*				EnrRecBothSuccess	=	3		//EnrRecBothSuccess:注册、识别时左右眼均注册成功
*
*            
*  返 回 值：0：注册成功
*			 1：人为停止注册
*    		<0：注册失败，根据返回值可以查询失败原因，或者参考strFunRts
*  创 建 人：lizhl
*  创建时间：2013-11-05
*  修 改 人：
*  修改时间：
*****************************************************************************/
int CAlgApi::SyncStartEnroll(OUT APIIrisInfo *enrIrisL, IN int numEnrL, OUT APIIrisInfo *enrIrisR, IN int numEnrR, OUT LRSucFailFlag &lrFlag)
{
    if((!_pCAPIBase) || (!_initFlag) )
    {
        return E_ALGAPI_INIT_FAILED;
    }
    char strFunRts[g_constLengthOfStrFunRts];
    return _pCAPIBase->SyncStartEnroll(enrIrisL, numEnrL, enrIrisR, numEnrR, lrFlag, strFunRts);
}

/*****************************************************************************
*                         算法逻辑API开始注册
*  函 数 名：AsyncStartEnroll
*  功    能：异步方式虹膜注册
*  说    明：
*  参    数：appEnroll：输入参数，用户注册的注册函数
*			 numEnrL，numEnrR：输入参数，注册得到的左眼、右眼虹膜图像个数，由调用者指定
*-------------------------------------------------------------------------------------------------
*以下参数无用
*            enrIrisL，enrIrisR：输出参数，注册得到的左眼、右眼虹膜图像及相关信息
*			 lrFlag：输出参数，左眼、右眼虹膜注册成功标志
*				EnrRecBothFailed	=	-3,		//EnrRecBothFailed：注册、识别时左右眼均注册或识别失败
*				EnrRecRightFailed	=	-2,		//EnrRecRightFailed:右眼注册、识别失败导致注册或识别失败,左眼情况未知
*				EnrRecLeftFailed	=	-1,		//EnrRecLeftFailed:左眼注册、识别失败导致注册或识别失败,右眼情况未知
*				EnrRecLeftSuccess	=	1,		//EnrRecLeftSuccess:注册、识别时仅左眼注册或识别成功
*				EnrRecRightSuccess	=	2,		//EnrRecRightSuccess:注册、识别时仅右眼注册或识别成功
*				EnrRecBothSuccess	=	3		//EnrRecBothSuccess:注册、识别时左右眼均注册成功
*--------------------------------------------------------------------------------------------------
*  返 回 值：0：函数调用成功
*    		<0：函数调用失败，根据返回值可以查询失败原因，或者参考strFunRts
*  创 建 人：lizhl
*  创建时间：2013-11-05
*  修 改 人：
*  修改时间：
*****************************************************************************/
int CAlgApi::AsyncStartEnroll(IN CBAlgApiEnroll appEnroll, IN int numEnrL, IN int numEnrR)
{
    if((!_pCAPIBase) || (!_initFlag) )
    {
        return E_ALGAPI_INIT_FAILED;
    }
    char strFunRts[g_constLengthOfStrFunRts];
    return _pCAPIBase->AsyncStartEnroll(appEnroll, numEnrL, numEnrR, strFunRts);
}

/*****************************************************************************
*                         算法逻辑API停止注册
*  函 数 名：AlgApiStopEnroll
*  功    能：停止虹膜注册
*  说    明：
*  参    数：
*            
*  返 回 值：0：停止注册成功
*    		<0：停止注册失败，根据返回值可以查询失败原因，或者参考strFunRts
*  创 建 人：lizhl
*  创建时间：2013-11-05
*  修 改 人：
*  修改时间：
*****************************************************************************/
int CAlgApi::StopEnroll()
{
    if((!_pCAPIBase) || (!_initFlag) )
    {
        return E_ALGAPI_INIT_FAILED;
    }
    char strFunRts[g_constLengthOfStrFunRts];
    return _pCAPIBase->StopEnroll(strFunRts);
}

/*****************************************************************************
*                         算法逻辑API开始识别
*  函 数 名：SyncStartIden
*  功    能：同步方式虹膜识别
*  说    明：
*  参    数：codeListL，codeListR：输入参数，左眼、右眼虹膜注册特征
*			 codeNumL，codeNumR：输入参数，左眼、右眼虹膜注册特征个数
*			 recIrisL，recIrisR：输出参数，本次识别得到的左眼、右眼虹膜图像及相关信息
*			 lrFlag：输出参数，左眼、右眼虹膜识别成功标志
*				EnrRecBothFailed	=	-3,		//EnrRecBothFailed：注册、识别时左右眼均注册或识别失败
*				EnrRecRightFailed	=	-2,		//EnrRecRightFailed:右眼注册、识别失败导致注册或识别失败,左眼情况未知
*				EnrRecLeftFailed	=	-1,		//EnrRecLeftFailed:左眼注册、识别失败导致注册或识别失败,右眼情况未知
*				EnrRecLeftSuccess	=	1,		//EnrRecLeftSuccess:注册、识别时仅左眼注册或识别成功
*				EnrRecRightSuccess	=	2,		//EnrRecRightSuccess:注册、识别时仅右眼注册或识别成功
*				EnrRecBothSuccess	=	3		//EnrRecBothSuccess:注册、识别时左右眼均注册成功
*			 pLeftMatchOutputParam，pRightMatchOutputParam：输出参数，左眼、右眼识别输出信息
*
*            
*  返 回 值：0：识别成功
*			 1：人为停止识别
*    		<0：识别失败，根据返回值可以查询失败原因，或者参考strFunRts
*  创 建 人：lizhl
*  创建时间：2013-11-05
*  修 改 人：
*  修改时间：
*****************************************************************************/
int CAlgApi::SyncStartIden(IN unsigned char *codeListL, IN int codeNumL, IN unsigned char *codeListR, IN int codeNumR,
                       OUT APIIrisInfo *recIrisL, OUT APIIrisInfo *recIrisR, OUT LRSucFailFlag &lrFlag,
                       OUT APIMatchOutput *pLeftMatchOutputParam, OUT APIMatchOutput *pRightMatchOutputParam)
{    
    if((!_pCAPIBase) || (!_initFlag) )
    {
        return E_ALGAPI_INIT_FAILED;
    }
    char strFunRts[g_constLengthOfStrFunRts];

    return _pCAPIBase->SyncStartIden(codeListL, codeNumL, codeListR, codeNumR, recIrisL, recIrisR,
                                     lrFlag, pLeftMatchOutputParam, pRightMatchOutputParam, strFunRts);
}

/*****************************************************************************
*                         算法逻辑API开始识别
*  函 数 名：AsyncStartIden
*  功    能：异步方式虹膜识别
*  说    明：
*  参    数：appIden：输入参数，用户注册的识别回调函数
*            codeListL，codeListR：输入参数，左眼、右眼虹膜注册特征
*			 codeNumL，codeNumR：输入参数，左眼、右眼虹膜注册特征个数
*--------------------------------------------------------------------------------------------------------------
*以下参数无用
*			 recIrisL，recIrisR：输出参数，本次识别得到的左眼、右眼虹膜图像及相关信息
*			 lrFlag：输出参数，左眼、右眼虹膜识别成功标志
*				EnrRecBothFailed	=	-3,		//EnrRecBothFailed：注册、识别时左右眼均注册或识别失败
*				EnrRecRightFailed	=	-2,		//EnrRecRightFailed:右眼注册、识别失败导致注册或识别失败,左眼情况未知
*				EnrRecLeftFailed	=	-1,		//EnrRecLeftFailed:左眼注册、识别失败导致注册或识别失败,右眼情况未知
*				EnrRecLeftSuccess	=	1,		//EnrRecLeftSuccess:注册、识别时仅左眼注册或识别成功
*				EnrRecRightSuccess	=	2,		//EnrRecRightSuccess:注册、识别时仅右眼注册或识别成功
*				EnrRecBothSuccess	=	3		//EnrRecBothSuccess:注册、识别时左右眼均注册成功
*			 pLeftMatchOutputParam，pRightMatchOutputParam：输出参数，左眼、右眼识别输出信息
*--------------------------------------------------------------------------------------------------------------
*  返 回 值：0：函数调用成功
*    		<0：函数调用失败，根据返回值可以查询失败原因，或者参考strFunRts
*  创 建 人：lizhl
*  创建时间：2013-11-05
*  修 改 人：
*  修改时间：
*****************************************************************************/
int CAlgApi::AsyncStartIden(IN CBAlgApiIden appIden, IN unsigned char *codeListL, IN int codeNumL, IN unsigned char *codeListR, IN int codeNumR)
{
    if((!_pCAPIBase) || (!_initFlag) )
    {
        return E_ALGAPI_INIT_FAILED;
    }
    char strFunRts[g_constLengthOfStrFunRts];

//    return _pCAPIBase->AsyncStartIden(appIden, codeListL, codeNumL, codeListR, codeNumR, recIrisL, recIrisR, lrFlag,
//                                      pLeftMatchOutputParam, pRightMatchOutputParam,strFunRts);
    return _pCAPIBase->AsyncStartIden(appIden, codeListL, codeNumL, codeListR, codeNumR, strFunRts);
}

/*****************************************************************************
*                         算法逻辑API停止识别
*  函 数 名：StopIden
*  功    能：停止虹膜识别
*  说    明：
*  参    数：
*            
*  返 回 值：0：停止识别成功
*    		<0：停止识别失败，根据返回值可以查询失败原因，或者参考strFunRts
*  创 建 人：lizhl
*  创建时间：2013-11-05
*  修 改 人：
*  修改时间：
*****************************************************************************/
int CAlgApi::StopIden()
{
    if((!_pCAPIBase) || (!_initFlag) )
    {
        return E_ALGAPI_INIT_FAILED;
    }
    char strFunRts[g_constLengthOfStrFunRts];
    return _pCAPIBase->StopIden(strFunRts);
}

/*****************************************************************************
*                         算法逻辑API虹膜比对
*  函 数 名：Match
*  功    能：虹膜识别特征模板比对
*  说    明：在StartEnroll成功后调用本函数，将注册得到的识别特征模板与
*			 已注册特征模板进行比对，验证被注册者是否重复注册虹膜
*  参    数：recCode，recNum：输入参数，注册得到的用户的识别特征模板及个数
*			 codeList，codeNum：输入参数，虹膜注册特征及个数
*			 pMatchOutputParam：输出参数，比对输出信息
*
*            
*  返 回 值：0：比对成功
*    		<0：比对失败，根据返回值可以查询失败原因，或者参考strFunRts
*  创 建 人：lizhl
*  创建时间：2013-11-05
*  修 改 人：
*  修改时间：
*****************************************************************************/
int CAlgApi::Match(IN unsigned char *recCode, IN int recNum, IN unsigned char *codeList, IN int codeNum, OUT APIMatchOutput *pMatchOutputParam)
{
    if((!_pCAPIBase) || (!_initFlag) )
    {
        return E_ALGAPI_INIT_FAILED;
    }
    char strFunRts[g_constLengthOfStrFunRts];
    return _pCAPIBase->Match(recCode, recNum, codeList, codeNum, pMatchOutputParam, strFunRts);
}

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
*  修 改 人：
*  修改时间：
*****************************************************************************/
CCameraApi::~CCameraApi(void)
{

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
    char strFunRts[g_constLengthOfStrFunRts];
    return E_ALGAPI_OK;
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
    char strFunRts[g_constLengthOfStrFunRts];
    return E_ALGAPI_OK;
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
*  修 改 人：
*  修改时间：
*****************************************************************************/
void CCameraApi::Release()
{

}

