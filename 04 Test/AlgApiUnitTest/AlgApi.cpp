/******************************************************************************************
** �ļ���:   AlgApi.cpp
���� ��Ҫ��:   CAlgApi,CCameraApi
**  
** Copyright (c) �пƺ�����޹�˾
** ������:   lizhl
** ��  ��:   2013-11-05
** �޸���:   
** ��  ��:
** ��  ��:   IKEMBX00��Ŀ�㷨�߼�API,Ϊ����Ŀ�ṩ��Ĥͼ��ɼ�,��Ĥע��,��Ĥʶ��ȹ���
 *���� ��Ҫģ��˵��: AlgApi.h                               �㷨�߼���                     
**
** ��  ��:   1.0.0
** ��  ע:   
**
*****************************************************************************************/
#include "AlgApi.h"
#include "ApiBase.h"
#include "BRCCommon.h"

/*****************************************************************************
*                         CAlgApi���캯��
*  �� �� ����CAlgApi
*  ��    �ܣ����캯������ȡ_pCAPIBaseʵ��
*  ˵    ����
*  ��    ����NULL
*  �� �� ֵ��NULL
*  �� �� �ˣ�lizhl
*  ����ʱ�䣺2013-11-05
*  �� �� �ˣ�
*  �޸�ʱ�䣺
*****************************************************************************/
CAlgApi::CAlgApi(void)
{
    _pCAPIBase = CAPIBase::GetInstance();
    _initFlag = false;
//    _pCAPIBase = static_cast <CAPIBase> (CAPIBase::GetInstance());
}

/*****************************************************************************
*                         CAlgApi��������
*  �� �� ����~CAlgApi
*  ��    �ܣ������������ͷ�_pCAPIBaseʵ��
*  ˵    ����
*  ��    ����NULL
*  �� �� ֵ��NULL
*  �� �� �ˣ�lizhl
*  ����ʱ�䣺2013-11-05
*  �� �� �ˣ�
*  �޸�ʱ�䣺
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
*                         �㷨�߼�API��ȡͼ��ߴ���Ϣ
*  �� �� ����GetImgInfo
*  ��    �ܣ���ȡ�豸��õĺ�Ĥͼ����Ϣ�ĳߴ���Ϣ
*  ˵    ����
*  ��    ����imgSizeInfo�����������ͼ��ߴ���Ϣ
*
*            
*  �� �� ֵ��0: ��ȡͼ��ߴ���Ϣ�ɹ�
*			 <0����ȡͼ��ߴ���Ϣʧ��
*  �� �� �ˣ�lizhl
*  ����ʱ�䣺2013-11-05
*  �� �� �ˣ�
*  �޸�ʱ�䣺
*****************************************************************************/
int CAlgApi::GetImgInfo(OUT ImgSizeInfo *imgSizeInfo)
{
	imgSizeInfo->ImgHeight = g_constIrisImgHeight;
	imgSizeInfo->ImgWidth = g_constIrisImgWidth;
	imgSizeInfo->ImgSize = g_constIrisImgSize;

    return E_ALGAPI_OK;
}

/*****************************************************************************
*                         �㷨�߼�API��ʼ��
*  �� �� ����Init
*  ��    �ܣ���ʼ���㷨�߼�APIʹ�õ������Դ
*  ˵    ����
*  ��    ����
*            
*  �� �� ֵ��0����ʼ���ɹ�
*			 <0����ʼ��ʧ��
*  �� �� �ˣ�lizhl
*  ����ʱ�䣺2013-11-05
*  �� �� �ˣ�
*  �޸�ʱ�䣺
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
*                         �㷨�߼�API�ͷ�
*  �� �� ����Release
*  ��    �ܣ��ͷ��㷨�߼�API��ʹ�õ������Դ
*  ˵    ����
*  ��    ����
*
*  �� �� ֵ��0���ͷųɹ�
*			 <0���ͷ�ʧ��
*  �� �� �ˣ�lizhl
*  ����ʱ�䣺2013-11-16
*  �� �� �ˣ�
*  �޸�ʱ�䣺
*****************************************************************************/
//int CAlgApi::Release()
//{
//    return E_ALGAPI_OK;
//}

/*****************************************************************************
*                         �㷨�߼�APIϵͳ����
*  �� �� ����Sleep
*  ��    �ܣ�ʹ�㷨�߼�API��������״̬�����ɼ���Ĥͼ�񣬵����ͷ������Դ
*  ˵    ����
*  ��    ����
*            
*  �� �� ֵ��0��ϵͳ���߳ɹ�
*    		<0��ϵͳ����ʧ�ܣ����ݷ���ֵ���Բ�ѯʧ��ԭ�򣬻��߲ο�strFunRts
*  �� �� �ˣ�lizhl
*  ����ʱ�䣺2013-11-05
*  �� �� �ˣ�
*  �޸�ʱ�䣺
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
*                         �㷨�߼�APIϵͳ����
*  �� �� ����Wake
*  ��    �ܣ�ʹ�㷨�߼�API������״̬���ѣ��ָ���������״̬
*  ˵    ����
*  ��    ����
*            
*  �� �� ֵ��0��ϵͳ���ѳɹ�
*    		<0��ϵͳ����ʧ�ܣ����ݷ���ֵ���Բ�ѯʧ��ԭ�򣬻��߲ο�strFunRts
*  �� �� �ˣ�lizhl
*  ����ʱ�䣺2013-11-05
*  �� �� �ˣ�
*  �޸�ʱ�䣺
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
*                         �㷨�߼�API���Ʋ�����ȡ
*  �� �� ����GetParam
*  ��    �ܣ���ȡ�㷨�߼�API���Ʋ���
*  ˵    ����
*  ��    ����paramStruct������������㷨�߼�API���Ʋ���
*
*            
*  �� �� ֵ��0��������ȡ�ɹ�
*    		<0��������ȡʧ�ܣ����ݷ���ֵ���Բ�ѯʧ��ԭ�򣬻��߲ο�strFunRts
*  �� �� �ˣ�lizhl
*  ����ʱ�䣺2013-11-05
*  �� �� �ˣ�
*  �޸�ʱ�䣺
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
*                         �㷨�߼�API���Ʋ�������
*  �� �� ����SetParam
*  ��    �ܣ������㷨�߼�API���Ʋ���
*  ˵    ����
*  ��    ����paramStruct������������㷨�߼�API���Ʋ���
*
*            
*  �� �� ֵ��0��������ȡ�ɹ�
*    		<0��������ȡʧ�ܣ����ݷ���ֵ���Բ�ѯʧ��ԭ�򣬻��߲ο�strFunRts
*  �� �� �ˣ�lizhl
*  ����ʱ�䣺2013-11-05
*  �� �� �ˣ�
*  �޸�ʱ�䣺
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
*                         �㷨�߼�API�ɼ���Ĥͼ��
*  �� �� ����SyncCapIrisImg
*  ��    �ܣ�ͬ����ʽ�ɼ���Ĥͼ�񣬲ɼ�����ͼ�������ʵʱ��ʾ
*  ˵    ����
*  ��    ����pIrisImg������������ɼ�����ͼ�����û������ڴ�ռ�
*			 lrIrisClearFlag������������������βɼ�����ͼ�����Ƿ��кϸ�ĺ�Ĥͼ�񣬸ñ�־������ָʾ������ʾ�졢����ʾ��
*				LAndRImgBlur	=	0,	//���ۡ�����ͼ�񶼲��ϸ�
*				LImgClear		=	1,	//����ͼ��ϸ�
*				RImgClear		=	2,	//����ͼ��ϸ�
*				LAndRImgClear	=	3	//���ۡ�����ͼ�񶼺ϸ�
*	   		 irisPositionFlag�������������ʾԶ�������ҡ�����
*				Far				=	0,	
*				Near			=	1,
*				Left			=	2,
*				Right			=	3,
*				Up				=	4,
*				Down			=	5,
*				OK				=	6
*
*            
*  �� �� ֵ��0���ɼ���Ĥͼ��ɹ�
*    		<0���ɼ���Ĥͼ��ʧ�ܣ����ݷ���ֵ���Բ�ѯʧ��ԭ�򣬻��߲ο�strFunRts
*  �� �� �ˣ�lizhl
*  ����ʱ�䣺2013-11-05
*  �� �� �ˣ�
*  �޸�ʱ�䣺
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
*                         �㷨�߼�API�ɼ���Ĥͼ��
*  �� �� ����AsyncCapIrisImg
*  ��    �ܣ��첽��ʽ�ɼ���Ĥͼ�񣬲ɼ�����ͼ�������ʵʱ��ʾ
*  ˵    ����
*  ��    ����appCapIrisImg������������û�ע��Ļص�����
*-------------------------------------------------------------------------------------------------------
*���²�������
*            pIrisImg������������ɼ�����ͼ�����û������ڴ�ռ�
*			 lrIrisClearFlag������������������βɼ�����ͼ�����Ƿ��кϸ�ĺ�Ĥͼ�񣬸ñ�־������ָʾ������ʾ�졢����ʾ��
*				LAndRImgBlur	=	0,	//���ۡ�����ͼ�񶼲��ϸ�
*				LImgClear		=	1,	//����ͼ��ϸ�
*				RImgClear		=	2,	//����ͼ��ϸ�
*				LAndRImgClear	=	3	//���ۡ�����ͼ�񶼺ϸ�
*	   		 irisPositionFlag�������������ʾԶ�������ҡ�����
*				Far				=	0,
*				Near			=	1,
*				Left			=	2,
*				Right			=	3,
*				Up				=	4,
*				Down			=	5,
*				OK				=	6
*---------------------------------------------------------------------------------------------------------
*  �� �� ֵ��0���������óɹ�
*    		<0����������ʧ�ܣ����ݷ���ֵ���Բ�ѯʧ��ԭ�򣬻��߲ο�strFunRts
*  �� �� �ˣ�lizhl
*  ����ʱ�䣺2013-11-05
*  �� �� �ˣ�
*  �޸�ʱ�䣺
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
*                         �㷨�߼�API��ʼע��
*  �� �� ����SyncStartEnroll
*  ��    �ܣ�ͬ����ʽ��Ĥע��
*  ˵    ����
*  ��    ����enrIrisL��enrIrisR�����������ע��õ������ۡ����ۺ�Ĥͼ�������Ϣ
*			 numEnrL��numEnrR�����������ע��õ������ۡ����ۺ�Ĥͼ��������ɵ�����ָ��
*			 lrFlag��������������ۡ����ۺ�Ĥע��ɹ���־
*				EnrRecBothFailed	=	-3,		//EnrRecBothFailed��ע�ᡢʶ��ʱ�����۾�ע���ʶ��ʧ��
*				EnrRecRightFailed	=	-2,		//EnrRecRightFailed:����ע�ᡢʶ��ʧ�ܵ���ע���ʶ��ʧ��,�������δ֪
*				EnrRecLeftFailed	=	-1,		//EnrRecLeftFailed:����ע�ᡢʶ��ʧ�ܵ���ע���ʶ��ʧ��,�������δ֪
*				EnrRecLeftSuccess	=	1,		//EnrRecLeftSuccess:ע�ᡢʶ��ʱ������ע���ʶ��ɹ�
*				EnrRecRightSuccess	=	2,		//EnrRecRightSuccess:ע�ᡢʶ��ʱ������ע���ʶ��ɹ�
*				EnrRecBothSuccess	=	3		//EnrRecBothSuccess:ע�ᡢʶ��ʱ�����۾�ע��ɹ�
*
*            
*  �� �� ֵ��0��ע��ɹ�
*			 1����Ϊֹͣע��
*    		<0��ע��ʧ�ܣ����ݷ���ֵ���Բ�ѯʧ��ԭ�򣬻��߲ο�strFunRts
*  �� �� �ˣ�lizhl
*  ����ʱ�䣺2013-11-05
*  �� �� �ˣ�
*  �޸�ʱ�䣺
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
*                         �㷨�߼�API��ʼע��
*  �� �� ����AsyncStartEnroll
*  ��    �ܣ��첽��ʽ��Ĥע��
*  ˵    ����
*  ��    ����appEnroll������������û�ע���ע�ắ��
*			 numEnrL��numEnrR�����������ע��õ������ۡ����ۺ�Ĥͼ��������ɵ�����ָ��
*-------------------------------------------------------------------------------------------------
*���²�������
*            enrIrisL��enrIrisR�����������ע��õ������ۡ����ۺ�Ĥͼ�������Ϣ
*			 lrFlag��������������ۡ����ۺ�Ĥע��ɹ���־
*				EnrRecBothFailed	=	-3,		//EnrRecBothFailed��ע�ᡢʶ��ʱ�����۾�ע���ʶ��ʧ��
*				EnrRecRightFailed	=	-2,		//EnrRecRightFailed:����ע�ᡢʶ��ʧ�ܵ���ע���ʶ��ʧ��,�������δ֪
*				EnrRecLeftFailed	=	-1,		//EnrRecLeftFailed:����ע�ᡢʶ��ʧ�ܵ���ע���ʶ��ʧ��,�������δ֪
*				EnrRecLeftSuccess	=	1,		//EnrRecLeftSuccess:ע�ᡢʶ��ʱ������ע���ʶ��ɹ�
*				EnrRecRightSuccess	=	2,		//EnrRecRightSuccess:ע�ᡢʶ��ʱ������ע���ʶ��ɹ�
*				EnrRecBothSuccess	=	3		//EnrRecBothSuccess:ע�ᡢʶ��ʱ�����۾�ע��ɹ�
*--------------------------------------------------------------------------------------------------
*  �� �� ֵ��0���������óɹ�
*    		<0����������ʧ�ܣ����ݷ���ֵ���Բ�ѯʧ��ԭ�򣬻��߲ο�strFunRts
*  �� �� �ˣ�lizhl
*  ����ʱ�䣺2013-11-05
*  �� �� �ˣ�
*  �޸�ʱ�䣺
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
*                         �㷨�߼�APIֹͣע��
*  �� �� ����AlgApiStopEnroll
*  ��    �ܣ�ֹͣ��Ĥע��
*  ˵    ����
*  ��    ����
*            
*  �� �� ֵ��0��ֹͣע��ɹ�
*    		<0��ֹͣע��ʧ�ܣ����ݷ���ֵ���Բ�ѯʧ��ԭ�򣬻��߲ο�strFunRts
*  �� �� �ˣ�lizhl
*  ����ʱ�䣺2013-11-05
*  �� �� �ˣ�
*  �޸�ʱ�䣺
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
*                         �㷨�߼�API��ʼʶ��
*  �� �� ����SyncStartIden
*  ��    �ܣ�ͬ����ʽ��Ĥʶ��
*  ˵    ����
*  ��    ����codeListL��codeListR��������������ۡ����ۺ�Ĥע������
*			 codeNumL��codeNumR��������������ۡ����ۺ�Ĥע����������
*			 recIrisL��recIrisR���������������ʶ��õ������ۡ����ۺ�Ĥͼ�������Ϣ
*			 lrFlag��������������ۡ����ۺ�Ĥʶ��ɹ���־
*				EnrRecBothFailed	=	-3,		//EnrRecBothFailed��ע�ᡢʶ��ʱ�����۾�ע���ʶ��ʧ��
*				EnrRecRightFailed	=	-2,		//EnrRecRightFailed:����ע�ᡢʶ��ʧ�ܵ���ע���ʶ��ʧ��,�������δ֪
*				EnrRecLeftFailed	=	-1,		//EnrRecLeftFailed:����ע�ᡢʶ��ʧ�ܵ���ע���ʶ��ʧ��,�������δ֪
*				EnrRecLeftSuccess	=	1,		//EnrRecLeftSuccess:ע�ᡢʶ��ʱ������ע���ʶ��ɹ�
*				EnrRecRightSuccess	=	2,		//EnrRecRightSuccess:ע�ᡢʶ��ʱ������ע���ʶ��ɹ�
*				EnrRecBothSuccess	=	3		//EnrRecBothSuccess:ע�ᡢʶ��ʱ�����۾�ע��ɹ�
*			 pLeftMatchOutputParam��pRightMatchOutputParam��������������ۡ�����ʶ�������Ϣ
*
*            
*  �� �� ֵ��0��ʶ��ɹ�
*			 1����Ϊֹͣʶ��
*    		<0��ʶ��ʧ�ܣ����ݷ���ֵ���Բ�ѯʧ��ԭ�򣬻��߲ο�strFunRts
*  �� �� �ˣ�lizhl
*  ����ʱ�䣺2013-11-05
*  �� �� �ˣ�
*  �޸�ʱ�䣺
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
*                         �㷨�߼�API��ʼʶ��
*  �� �� ����AsyncStartIden
*  ��    �ܣ��첽��ʽ��Ĥʶ��
*  ˵    ����
*  ��    ����appIden������������û�ע���ʶ��ص�����
*            codeListL��codeListR��������������ۡ����ۺ�Ĥע������
*			 codeNumL��codeNumR��������������ۡ����ۺ�Ĥע����������
*--------------------------------------------------------------------------------------------------------------
*���²�������
*			 recIrisL��recIrisR���������������ʶ��õ������ۡ����ۺ�Ĥͼ�������Ϣ
*			 lrFlag��������������ۡ����ۺ�Ĥʶ��ɹ���־
*				EnrRecBothFailed	=	-3,		//EnrRecBothFailed��ע�ᡢʶ��ʱ�����۾�ע���ʶ��ʧ��
*				EnrRecRightFailed	=	-2,		//EnrRecRightFailed:����ע�ᡢʶ��ʧ�ܵ���ע���ʶ��ʧ��,�������δ֪
*				EnrRecLeftFailed	=	-1,		//EnrRecLeftFailed:����ע�ᡢʶ��ʧ�ܵ���ע���ʶ��ʧ��,�������δ֪
*				EnrRecLeftSuccess	=	1,		//EnrRecLeftSuccess:ע�ᡢʶ��ʱ������ע���ʶ��ɹ�
*				EnrRecRightSuccess	=	2,		//EnrRecRightSuccess:ע�ᡢʶ��ʱ������ע���ʶ��ɹ�
*				EnrRecBothSuccess	=	3		//EnrRecBothSuccess:ע�ᡢʶ��ʱ�����۾�ע��ɹ�
*			 pLeftMatchOutputParam��pRightMatchOutputParam��������������ۡ�����ʶ�������Ϣ
*--------------------------------------------------------------------------------------------------------------
*  �� �� ֵ��0���������óɹ�
*    		<0����������ʧ�ܣ����ݷ���ֵ���Բ�ѯʧ��ԭ�򣬻��߲ο�strFunRts
*  �� �� �ˣ�lizhl
*  ����ʱ�䣺2013-11-05
*  �� �� �ˣ�
*  �޸�ʱ�䣺
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
*                         �㷨�߼�APIֹͣʶ��
*  �� �� ����StopIden
*  ��    �ܣ�ֹͣ��Ĥʶ��
*  ˵    ����
*  ��    ����
*            
*  �� �� ֵ��0��ֹͣʶ��ɹ�
*    		<0��ֹͣʶ��ʧ�ܣ����ݷ���ֵ���Բ�ѯʧ��ԭ�򣬻��߲ο�strFunRts
*  �� �� �ˣ�lizhl
*  ����ʱ�䣺2013-11-05
*  �� �� �ˣ�
*  �޸�ʱ�䣺
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
*                         �㷨�߼�API��Ĥ�ȶ�
*  �� �� ����Match
*  ��    �ܣ���Ĥʶ������ģ��ȶ�
*  ˵    ������StartEnroll�ɹ�����ñ���������ע��õ���ʶ������ģ����
*			 ��ע������ģ����бȶԣ���֤��ע�����Ƿ��ظ�ע���Ĥ
*  ��    ����recCode��recNum�����������ע��õ����û���ʶ������ģ�弰����
*			 codeList��codeNum�������������Ĥע������������
*			 pMatchOutputParam������������ȶ������Ϣ
*
*            
*  �� �� ֵ��0���ȶԳɹ�
*    		<0���ȶ�ʧ�ܣ����ݷ���ֵ���Բ�ѯʧ��ԭ�򣬻��߲ο�strFunRts
*  �� �� �ˣ�lizhl
*  ����ʱ�䣺2013-11-05
*  �� �� �ˣ�
*  �޸�ʱ�䣺
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
*                         CFaceCameraApi���캯��
*  �� �� ����CCameraApi
*  ��    �ܣ����캯��
*  ˵    ����
*  ��    ����NULL
*  �� �� ֵ��NULL
*  �� �� �ˣ�lizhl
*  ����ʱ�䣺2013-11-05
*  �� �� �ˣ�
*  �޸�ʱ�䣺
*****************************************************************************/
CCameraApi::CCameraApi(void)
{

}

/*****************************************************************************
*                         CFaceCameraApi��������
*  �� �� ����CCameraApi
*  ��    �ܣ���������
*  ˵    ����
*  ��    ����NULL
*  �� �� ֵ��NULL
*  �� �� �ˣ�lizhl
*  ����ʱ�䣺2013-11-05
*  �� �� �ˣ�
*  �޸�ʱ�䣺
*****************************************************************************/
CCameraApi::~CCameraApi(void)
{

}

/*****************************************************************************
*                         ��������ͷAPI��ȡͼ��ߴ���Ϣ
*  �� �� ����GetImgInfo
*  ��    �ܣ���ȡ�豸��õ�����ͼ����Ϣ�ĳߴ���Ϣ
*  ˵    ����
*  ��    ����imgSizeInfo�����������ͼ��ߴ���Ϣ
*
*            
*  �� �� ֵ��0����ȡͼ��ߴ���Ϣ�ɹ�
*			 <0����ȡͼ��ߴ���Ϣʧ��
*  �� �� �ˣ�lizhl
*  ����ʱ�䣺2013-11-05
*  �� �� �ˣ�
*  �޸�ʱ�䣺
*****************************************************************************/
int CCameraApi::GetImgInfo(OUT ImgSizeInfo *imgSizeInfo)
{
	imgSizeInfo->ImgHeight = g_constFaceImgHeigth;
	imgSizeInfo->ImgWidth = g_constFaceImgWidth;
	imgSizeInfo->ImgSize = g_constFaceImgSize;
    return E_ALGAPI_OK;
}



/*****************************************************************************
*                         ��������ͷAPI��ʼ���ɼ�����ͼ������ͷ
*  �� �� ����Init
*  ��    �ܣ���ʼ���ɼ�����ͼ������ͷ
*  ˵    ����
*  ��    ����
*            
*  �� �� ֵ��0����ʼ���ɹ�
*			 <0����ʼ��ʧ��
*  �� �� �ˣ�lizhl
*  ����ʱ�䣺2013-11-05
*  �� �� �ˣ�
*  �޸�ʱ�䣺
*****************************************************************************/
int CCameraApi::Init()
{
    char strFunRts[g_constLengthOfStrFunRts];
    return E_ALGAPI_OK;
}

/*****************************************************************************
*                         ��������ͷAPI�ɼ�����ͼ��
*  �� �� ����CapImg
*  ��    �ܣ��ɼ�����ͼ�񣬲ɼ�����ͼ�������ʵʱ��ʾ
*  ˵    ����
*  ��    ����pFaceImg������������ɼ�����ͼ�����û������ڴ�ռ�
*
*            
*  �� �� ֵ��0���ɼ�����ͼ��ɹ�
*    		<0���ɼ�����ͼ��ʧ�ܣ����ݷ���ֵ���Բ�ѯʧ��ԭ�򣬻��߲ο�strFunRts
*  �� �� �ˣ�lizhl
*  ����ʱ�䣺2013-11-05
*  �� �� �ˣ�
*  �޸�ʱ�䣺
*****************************************************************************/
int CCameraApi::CapImg(OUT unsigned char *pFaceImg)
{
    char strFunRts[g_constLengthOfStrFunRts];
    return E_ALGAPI_OK;
}

/*****************************************************************************
*                         �ͷŲɼ�����ͼ������ͷ�����Դ
*  �� �� ����Release
*  ��    �ܣ��ͷŲɼ�����ͼ������ͷ�����Դ
*  ˵    ����
*  ��    ����
*            
*  �� �� ֵ��NULL
*  �� �� �ˣ�lizhl
*  ����ʱ�䣺2013-11-05
*  �� �� �ˣ�
*  �޸�ʱ�䣺
*****************************************************************************/
void CCameraApi::Release()
{

}

