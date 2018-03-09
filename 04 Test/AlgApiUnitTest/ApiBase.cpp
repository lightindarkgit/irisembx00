/******************************************************************************************
** �ļ���:   AlgBase.cpp
���� ��Ҫ��:   CAPIBase
**
** Copyright (c) �пƺ�����޹�˾
** ������:   lizhl
** ��  ��:   2013-11-14
** �޸���:
** ��  ��:
** ��  ��:   IKEMBX00��Ŀ�㷨�߼�API�ڲ�ʵ��
 *���� ��Ҫģ��˵��: API�����ڲ�����ʵ��
**
** ��  ��:   1.0.0
** ��  ע:
**
*****************************************************************************************/
#include <iostream>
#include "AlgApi.h"
#include "ApiBase.h"

#ifdef ALGAPITEST
#include "VirtualReadImg.h"
#endif

using namespace std;

mutex CAPIBase::s_instanceMutex;

//shared_ptr<CAPIBase> CAPIBase::s_pSingletonCAPIBase;
CAPIBase* CAPIBase::s_pSingletonCAPIBase = NULL;

/*****************************************************************************
*                         CAPIBase���캯��
*  �� �� ����CAPIBase
*  ��    �ܣ����캯��
*  ˵    ����
*  ��    ����NULL
*  �� �� ֵ��NULL
*  �� �� �ˣ�lizhl
*  ����ʱ�䣺2013-11-16
*  �� �� �ˣ�
*  �޸�ʱ�䣺
*****************************************************************************/
CAPIBase::CAPIBase()
{

}

/*****************************************************************************
*                         CAPIBase��������
*  �� �� ����~CAPIBase
*  ��    �ܣ���������
*  ˵    ����
*  ��    ����NULL
*  �� �� ֵ��NULL
*  �� �� �ˣ�lizhl
*  ����ʱ�䣺2013-11-16
*  �� �� �ˣ�
*  �޸�ʱ�䣺
*****************************************************************************/
CAPIBase::~CAPIBase()
{
//    delete _pWakeMainThreadCond;
//    delete _pXferBestImgCond;
//    delete _pXferCurrentImgCond;
}

/*****************************************************************************
*                         ��ȡCAPIBaseʵ��
*  �� �� ����GetInstance
*  ��    �ܣ���ȡCAPIBaseʵ��
*  ˵    ����CAPIBase���õ���ģʽ���Ҷ��̰߳�ȫ
*  ��    ����NULL
*
*  �� �� ֵ��!NULL: CAPIBaseʵ��ָ��
*			NULL����ȡʧ��
*  �� �� �ˣ�lizhl
*  ����ʱ�䣺2013-11-16
*  �� �� �ˣ�
*  �޸�ʱ�䣺
*****************************************************************************/
//shared_ptr<CAPIBase> CAPIBase::GetInstance()
CAPIBase* CAPIBase::GetInstance()
{
    // ˫�ؼ�飬�������
    if (NULL == s_pSingletonCAPIBase)
    {
        lock_guard<mutex> lck (s_instanceMutex);

        if (NULL == s_pSingletonCAPIBase)
        {
            s_pSingletonCAPIBase = new CAPIBase();
            s_pSingletonCAPIBase->_instanceNum = 0;
            s_pSingletonCAPIBase->_mainThreadFlag = false;//��ʼֵ
            s_pSingletonCAPIBase->_isWake = false;//��ʼֵ            
        }

    }

    return s_pSingletonCAPIBase;
}

/*****************************************************************************
*                         �㷨�߼�API��ʼ��
*  �� �� ����Init
*  ��    �ܣ���ʼ���㷨�߼�APIʹ�õ������Դ
*  ˵    ����
*  ��    ����strFunRts��������������ַ�����ʽ��ʾ���κ������ý�������û�����
*			 �ڴ�ռ�
*
*  �� �� ֵ��0����ʼ���ɹ�
*			 <0����ʼ��ʧ��
*  �� �� �ˣ�lizhl
*  ����ʱ�䣺2013-11-16
*  �� �� �ˣ�
*  �޸�ʱ�䣺
*****************************************************************************/
int CAPIBase::Init(OUT char *strFunRts)
{
    int funResult = E_ALGAPI_DEFAUL_ERROR;
    //����ǵ�һ�γ�ʼ�������ʼ�������Դ����ʼ��USB����ͷ��������ͼ�߳�
    if(0 == _instanceNum)
    {
        _algHandle = IKIR_HandleInit(strFunRts);
        if(NULL == _algHandle)
        {
            SetFunRt(strFunRts,E_ALGAPI_INIT_FAILED, "Get algorithm's handle failed");
            return E_ALGAPI_INIT_FAILED;
        }

        //��USBģ��
#ifndef ALGAPITEST
        _syncUsbApi = new SyncUSBAPI();
        funResult = _syncUsbApi->Open();
        if(E_ALGAPI_OK != funResult)
        {
            SetFunRt(strFunRts,E_ALGAPI_INIT_FAILED, "Init usb failed");
            return E_ALGAPI_INIT_FAILED;
        }
#endif
        //��ʼ����Ĥͼ����Ϣ�ṹ��
        CASIAGenInitIrisInfo(&_leftCurImg);
        CASIAGenInitIrisInfo(&_rightCurImg);
        CASIAGenInitIrisInfo(&_leftBestImg);
        CASIAGenInitIrisInfo(&_rightBestImg);

        _keepIdenFlag = false;//��ʼֵ
        _outIdenFlag = true;//��ʼֵ
        _keepEnrFlag = false;//��ʼֵ
        _outEnrFlag = true;//��ʼֵ

        _getNewImgFlag = false;//��ʼֵ
        _getLeftNewGoodImgFlag = false;//��ʼֵ
        _getRightNewGoodImgFlag = false;//��ʼֵ

//        _asyncCapImgFlag = nullptr;
        _asyncCapImgFlag = false;
        _asyncEnrFlag = false;
        _asyncIdenFlag = false;

        //��ʼ���ص����ָ��
        InitCapCBSrc();
        InitEnrCBSrc();
        InitIdenCBSrc();

        _mainThreadFlag = true;
        _isWake = true;

        _tThreadMain = thread(ThreadMain, s_pSingletonCAPIBase);
        _tAsyncCapImg = thread(ThreadAsyncCaptureImg, s_pSingletonCAPIBase);
        _tAsyncEnroll = thread(ThreadAsyncEnroll, s_pSingletonCAPIBase);
        _tAsyncIden = thread(ThreadAsyncIden, s_pSingletonCAPIBase);

    }

    if(!_isWake)
    {
        funResult = Wake(strFunRts);
    }

    if(_keepIdenFlag)
    {
        funResult = StopIden(strFunRts);
        if(E_ALGAPI_OK != funResult)
        {
            SetFunRt(strFunRts,E_ALGAPI_INIT_FAILED, "Stop iden failed");
            return E_ALGAPI_INIT_FAILED;
        }
    }

    if(_keepEnrFlag)
    {
        funResult = StopEnroll(strFunRts);
        if(E_ALGAPI_OK != funResult)
        {
            SetFunRt(strFunRts,E_ALGAPI_INIT_FAILED, "Stop enroll failed");
            return E_ALGAPI_INIT_FAILED;
        }
    }

    //�Ƿ���Ҫ�Բ�ͼ���ܽ��г�ʼ������
    if(_cbCapIrisImg)
    {
        InitCapCBSrc();
    }


    _qualityMode = IR_WORK_MODE_REC;//Ĭ��Ϊʶ��ģʽ
    _algApiCtrlParam.EyeMode = AnyEye;//Ĭ��Ϊ������
    _algApiCtrlParam.FindMode = g_constFindModeExhaust;//����������Ĭ��ֵ
    _algApiCtrlParam.IfSpoofDetect = g_constIKNoSpoofDetect;		//��ִ�м��ۼ�⣬Ĭ��ֵ
    _algApiCtrlParam.MaxENRTime = g_constMaxENRTime;		//Ŀǰʵ�ַ�ʽ�ĵ�λ���룬���ע����̳������ʱ�䣬�򷵻�ʧ��
    _algApiCtrlParam.MaxRECTime = g_constMaxRECTime;		//��λ�룬���ʶ����̳������ʱ�䣬�򷵻�ʧ��

    _instanceNum++;//�����ǳ�ʼ���ɹ�����+1
    SetFunRt(strFunRts,E_ALGAPI_OK, "Init success");
    return E_ALGAPI_OK;
}

/*****************************************************************************
*                         �㷨�߼�API�ͷ�
*  �� �� ����Release
*  ��    �ܣ��ͷ��㷨�߼�API��ʹ�õ������Դ
*  ˵    ����
*  ��    ����strFunRts��������������ַ�����ʽ��ʾ���κ������ý�������û�����
*			 �ڴ�ռ�
*
*  �� �� ֵ��0���ͷųɹ�
*			 <0���ͷ�ʧ��
*  �� �� �ˣ�lizhl
*  ����ʱ�䣺2013-11-16
*  �� �� �ˣ�
*  �޸�ʱ�䣺
*****************************************************************************/
//int CAPIBase::Release(OUT char *strFunRts)
//{
////    s_instanceNum--;//�����Ǳ�Ҫ������ɺ���-1��
////    if(0 == s_instanceNum)
////    {
////        //�ͷ�USB�����������Դ
//////        CAPIBase::~CAPIBase();
////        s_pSingletonCAPIBase.reset();
//////        ~CAPIBase();
////    }
//    return E_ALGAPI_OK;
//}

/*****************************************************************************
*                         �㷨�߼�APIϵͳ����
*  �� �� ����Sleep
*  ��    �ܣ�ʹ�㷨�߼�API��������״̬�����ɼ���Ĥͼ�񣬵����ͷ������Դ
*  ˵    ����
*  ��    ����strFunRts��������������ַ�����ʽ��ʾ���κ������ý�������û�����
*			 �ڴ�ռ�
*
*  �� �� ֵ��0��ϵͳ���߳ɹ�
*    		<0��ϵͳ����ʧ�ܣ����ݷ���ֵ���Բ�ѯʧ��ԭ�򣬻��߲ο�strFunRts
*  �� �� �ˣ�lizhl
*  ����ʱ�䣺2013-11-05
*  �� �� �ˣ�
*  �޸�ʱ�䣺
*****************************************************************************/
int CAPIBase::Sleep(OUT char *strFunRts)
{
    if(!_isWake)
    {
        SetFunRt(strFunRts,E_ALGAPI_SLEEP_FAILED,"System is already sleep");
        return E_ALGAPI_SLEEP_FAILED;
    }

    int funResult;
    if(_keepIdenFlag)
    {
        funResult = StopIden(strFunRts);
    }

    if(_keepEnrFlag)
    {
        funResult = StopEnroll(strFunRts);
    }

    lock_guard<mutex> lck(_wakeMainThreadMutex);
    _isWake = false;
    SetFunRt(strFunRts,E_ALGAPI_OK, "Sleep success");
    return E_ALGAPI_OK;
}

/*****************************************************************************
*                         �㷨�߼�APIϵͳ����
*  �� �� ����Wake
*  ��    �ܣ�ʹ�㷨�߼�API������״̬���ѣ��ָ���������״̬
*  ˵    ����
*  ��    ����strFunRts��������������ַ�����ʽ��ʾ���κ������ý�������û�����
*			 �ڴ�ռ�
*
*  �� �� ֵ��0��ϵͳ���ѳɹ�
*    		<0��ϵͳ����ʧ�ܣ����ݷ���ֵ���Բ�ѯʧ��ԭ�򣬻��߲ο�strFunRts
*  �� �� �ˣ�lizhl
*  ����ʱ�䣺2013-11-05
*  �� �� �ˣ�
*  �޸�ʱ�䣺
*****************************************************************************/
int CAPIBase::Wake(OUT char *strFunRts)
{
//    _pWakeMainThreadCond->Lock();
//    _pWakeMainThreadCond->Signal();
//    _pWakeMainThreadCond->Unlock();
//    while(!_isWake)
//    {
//        usleep(0);
//    }

    if(_isWake)
    {
        SetFunRt(strFunRts,E_ALGAPI_WAKE_FAILED,"System is already wake");
        return E_ALGAPI_WAKE_FAILED;
    }

    unique_lock <mutex> lck(_wakeMainThreadMutex);
    _isWake = true;
    _wakeMainThreadCV.notify_one();
    SetFunRt(strFunRts,E_ALGAPI_OK, "Wake success");
    return E_ALGAPI_OK;
}

/*****************************************************************************
*                         �㷨�߼�API���Ʋ�����ȡ
*  �� �� ����GetParam
*  ��    �ܣ���ȡ�㷨�߼�API���Ʋ���
*  ˵    ����
*  ��    ����paramStruct������������㷨�߼�API���Ʋ���
*            strFunRts��������������ַ�����ʽ��ʾ���κ������ý�������û�����
*			 �ڴ�ռ�
*
*  �� �� ֵ��0��������ȡ�ɹ�
*    		<0��������ȡʧ�ܣ����ݷ���ֵ���Բ�ѯʧ��ԭ�򣬻��߲ο�strFunRts
*  �� �� �ˣ�lizhl
*  ����ʱ�䣺2013-11-05
*  �� �� �ˣ�
*  �޸�ʱ�䣺
*****************************************************************************/
int CAPIBase::GetParam(OUT AlgApiCtrlParam *paramStruct, OUT char *strFunRts)
{
    if(!_isWake)
    {
        SetFunRt(strFunRts,E_ALGAPI_SYSTEM_IS_SLEEP,"System is sleep");
        return E_ALGAPI_SYSTEM_IS_SLEEP;
    }

    if( sizeof(*paramStruct) != sizeof(_algApiCtrlParam) )
    {
        SetFunRt(strFunRts,E_ALGAPI_INVALID_PARAMETER, "Get algorithm's control parameter failed");
        return E_ALGAPI_INVALID_PARAMETER;
    }

    memcpy(paramStruct, &_algApiCtrlParam, sizeof(_algApiCtrlParam));
    SetFunRt(strFunRts,E_ALGAPI_OK, "Get algorithm's control parameter success");
    return E_ALGAPI_OK;
}

/*****************************************************************************
*                         �㷨�߼�API���Ʋ�������
*  �� �� ����SetParam
*  ��    �ܣ������㷨�߼�API���Ʋ���
*  ˵    ����
*  ��    ����paramStruct������������㷨�߼�API���Ʋ���
*            strFunRts��������������ַ�����ʽ��ʾ���κ������ý�������û�����
*			 �ڴ�ռ�
*
*  �� �� ֵ��0��������ȡ�ɹ�
*    		<0��������ȡʧ�ܣ����ݷ���ֵ���Բ�ѯʧ��ԭ�򣬻��߲ο�strFunRts
*  �� �� �ˣ�lizhl
*  ����ʱ�䣺2013-11-05
*  �� �� �ˣ�
*  �޸�ʱ�䣺
*****************************************************************************/
int CAPIBase::SetParam(IN AlgApiCtrlParam* paramStruct, OUT char* strFunRts)
{
    if(!_isWake)
    {
        SetFunRt(strFunRts,E_ALGAPI_SYSTEM_IS_SLEEP,"System is sleep");
        return E_ALGAPI_SYSTEM_IS_SLEEP;
    }

    if( sizeof(*paramStruct) != sizeof(_algApiCtrlParam) )
    {
        SetFunRt(strFunRts,E_ALGAPI_INVALID_PARAMETER, "Set algorithm's control parameter failed");
        return E_ALGAPI_INVALID_PARAMETER;
    }

    if( (paramStruct->EyeMode < BothEye) || (paramStruct->EyeMode > AnyEye) )
    {
        SetFunRt(strFunRts,E_ALGAPI_INVALID_EYEMODE, "Invalid eyemode! Set algorithm's control parameter failed");
        return E_ALGAPI_INVALID_EYEMODE;
    }

    if( (g_constFindModeExhaust != paramStruct->FindMode) && (g_constFindModeFirstMatch != paramStruct->FindMode) )
    {
        SetFunRt(strFunRts,E_ALGAPI_INVALID_FINDMODE, "Invalid findmode! Set algorithm's control parameter failed");
        return E_ALGAPI_INVALID_FINDMODE;
    }

    if( (g_constIKNoSpoofDetect != paramStruct->IfSpoofDetect) && (g_constIKDoSpoofDetect != paramStruct->IfSpoofDetect) )
    {
        SetFunRt(strFunRts,E_ALGAPI_INVALID_IFSPOOFDETECT, "Invalid ifspoofdetect! Set algorithm's control parameter failed");
        return E_ALGAPI_INVALID_IFSPOOFDETECT;
    }

    if( g_constMinENRTime > paramStruct->MaxENRTime)
    {
        SetFunRt(strFunRts,E_ALGAPI_INVALID_TIME,"Invalid maxenrtime! Set algorithm's control parameter failed");
        return E_ALGAPI_INVALID_TIME;
    }

    if(g_constMaxRECTime < paramStruct->MaxRECTime)
    {
        SetFunRt(strFunRts,E_ALGAPI_INVALID_TIME,"Invalid maxrectime! Set algorithm's control parameter failed");
        return E_ALGAPI_INVALID_TIME;
    }

    memcpy(&_algApiCtrlParam,paramStruct,sizeof(_algApiCtrlParam));
    SetFunRt(strFunRts,E_ALGAPI_OK, "Set algorithm's control parameter success");
    return E_ALGAPI_OK;
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
*            strFunRts��������������ַ�����ʽ��ʾ���κ������ý�������û�����
*			 �ڴ�ռ�
*
*  �� �� ֵ��0���ɼ���Ĥͼ��ɹ�
*    		<0���ɼ���Ĥͼ��ʧ�ܣ����ݷ���ֵ���Բ�ѯʧ��ԭ�򣬻��߲ο�strFunRts
*  �� �� �ˣ�lizhl
*  ����ʱ�䣺2013-11-05
*  �� �� �ˣ�
*  �޸�ʱ�䣺
*****************************************************************************/
int CAPIBase::SyncCapIrisImg(OUT unsigned char *pIrisImg, OUT LRIrisClearFlag &lrIrisClearFlag, OUT IrisPositionFlag &irisPositionFlag, OUT char *strFunRts)
{
    if(!_isWake)
    {
        SetFunRt(strFunRts,E_ALGAPI_SYSTEM_IS_SLEEP,"System is sleep");
        return E_ALGAPI_SYSTEM_IS_SLEEP;
    }

    if(pIrisImg)
    {
        unique_lock<mutex> lck(_xferCurrentImgMutex);
        _pSwapImg = pIrisImg;
        _swapNewImgFlag = true;//֪ͨ���߳̿��Խ���ͼ��
        while(_swapNewImgFlag)
        {
//            _xferCurrentImgCV.wait(lck);
            if(_xferCurrentImgCV.wait_for(lck,chrono::seconds(1)) == cv_status::timeout)
            {
                _pSwapImg = NULL;
                _swapNewImgFlag = false;
                SetFunRt(strFunRts,E_ALGAPI_CAP_IMG_FAILED, "Capture image failed");
                return E_ALGAPI_CAP_IMG_FAILED;
            }
        }

        _pSwapImg = NULL;
        lrIrisClearFlag = _lrIrisClearFlag;
        irisPositionFlag = _irisPositionFlag;

        SetFunRt(strFunRts,E_ALGAPI_OK, "Capture image success");
        return E_ALGAPI_OK;
    }
    else
    {
        SetFunRt(strFunRts,E_ALGAPI_CAP_IMG_FAILED, "Capture image failed");
        return E_ALGAPI_CAP_IMG_FAILED;
    }


}

/*****************************************************************************
*                         �㷨�߼�API�ɼ���Ĥͼ��
*  �� �� ����AsyncCapIrisImg
*  ��    �ܣ��첽��ʽ�ɼ���Ĥͼ�񣬲ɼ�����ͼ�������ʵʱ��ʾ
*  ˵    ����
*  ��    ����appCapIrisImg������������û�ע��Ļص�����
*--------------------------------------------------------------------
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
*--------------------------------------------------------------------
*            strFunRts��������������ַ�����ʽ��ʾ���κ������ý�������û�����
*			 �ڴ�ռ�
*
*  �� �� ֵ��0���������óɹ�
*    		<0����������ʧ�ܣ����ݷ���ֵ���Բ�ѯʧ��ԭ�򣬻��߲ο�strFunRts
*  �� �� �ˣ�lizhl
*  ����ʱ�䣺2013-11-05
*  �� �� �ˣ�
*  �޸�ʱ�䣺
*****************************************************************************/
int CAPIBase::AsyncCapIrisImg(IN CBAlgApiCapIrisImg appCapIrisImg, OUT char *strFunRts)
{
    if(!_isWake)
    {
        SetFunRt(strFunRts,E_ALGAPI_SYSTEM_IS_SLEEP,"System is sleep");
        return E_ALGAPI_SYSTEM_IS_SLEEP;
    }

    unique_lock <mutex> lck(_asyncCapImgMutex);

//    _cbIrisImg = pIrisImg;
    _cbCapIrisImg = appCapIrisImg;
//    _cbIrisPositionFlag = irisPositionFlag;
//    _cbLRIrisClearFlag = lrIrisClearFlag;

    _asyncCapImgFlag = true;
    _asyncCapImgCV.notify_one();//�����źŸ��첽��ͼ�߳�

    SetFunRt(strFunRts,E_ALGAPI_OK, "Async capture image success");
    return E_ALGAPI_OK;
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
*			 strFunRts��������������ַ�����ʽ��ʾ���κ������ý�������û�����
*			 �ڴ�ռ��ڴ�ռ�
*
*  �� �� ֵ��0��ע��ɹ�
*			 1����Ϊֹͣע��
*    		<0��ע��ʧ�ܣ����ݷ���ֵ���Բ�ѯʧ��ԭ�򣬻��߲ο�strFunRts
*  �� �� �ˣ�lizhl
*  ����ʱ�䣺2013-11-05
*  �� �� �ˣ�
*  �޸�ʱ�䣺
*****************************************************************************/
int CAPIBase::SyncStartEnroll(OUT APIIrisInfo *enrIrisL, IN int numEnrL, OUT APIIrisInfo *enrIrisR, IN int numEnrR, OUT LRSucFailFlag &lrFlag, OUT char *strFunRts)
{
    int maxEnrollTime = _algApiCtrlParam.MaxENRTime;//������ʱ�������û��ע��ɹ�������Ϊע��ʧ��
    bool leftEyeNeed = false;
    bool rightEyeNeed = false;
    bool leftEyeActive = false;
    bool rightEyeActive = false;


    {
        //��麯�����úϷ��Բ����ñ�־λ
        if(!_isWake)
        {
            SetFunRt(strFunRts,E_ALGAPI_SYSTEM_IS_SLEEP,"System is sleep");
            return E_ALGAPI_SYSTEM_IS_SLEEP;
        }

        lock_guard<mutex> lck(_setEnrRecMutex);

        if(_keepIdenFlag || _keepEnrFlag)
        {
            SetFunRt(strFunRts,E_ALGAPI_ALGORITHM_CONFLICT, "Start enroll failed, iden is running");
            return E_ALGAPI_ALGORITHM_CONFLICT;
        }

        if( ((numEnrL < 2) || (numEnrL > g_constMaxEnrollImNum)) || ((numEnrR < 2) || (numEnrR > g_constMaxEnrollImNum)) )
        {
            SetFunRt(strFunRts,E_ALGAPI_INVALID_ENR_INPUT,"Invalid enroll number");
            return E_ALGAPI_INVALID_ENR_INPUT;
        }

        if(maxEnrollTime < g_constMinENRTime)
        {
            SetFunRt(strFunRts,E_ALGAPI_INVALID_TIME,"Invalid maxenrtime! Set algorithm's control parameter failed");
            return E_ALGAPI_INVALID_TIME;
        }

        switch(_algApiCtrlParam.EyeMode)
        {
        case BothEye:
            leftEyeNeed = true;
            rightEyeNeed = true;
            break;

        case LeftEye:
            leftEyeNeed = true;
            rightEyeNeed = false;
            break;

        case RightEye:
            leftEyeNeed = false;
            rightEyeNeed = true;
            break;

        case AnyEye:
            leftEyeNeed = true;
            rightEyeNeed = true;
            break;

        default:
            SetFunRt(strFunRts,E_ALGAPI_INVALID_EYEMODE,"Invalid eyemode! Set algorithm's control parameter failed");
            return E_ALGAPI_INVALID_EYEMODE;
        }

        if( ((true == leftEyeNeed) && (NULL == enrIrisL)) || ((true == rightEyeNeed) && (NULL == enrIrisR))  )
        {
            SetFunRt(strFunRts,E_ALGAPI_INVALID_ENR_INPUT,"Invalid enroll iris list");
            return E_ALGAPI_INVALID_ENR_INPUT;
        }

        _keepEnrFlag = true;
        _outEnrFlag = false;
    }

    //��ʼע��
    _qualityMode = IR_WORK_MODE_ENR;

    int leftExistedNum = 0;
    int rightExistedNum = 0;

    int funResult = E_ALGAPI_DEFAUL_ERROR;
    lrFlag = EnrRecUnknown;
    int enrResult = 0;

    //�ͳ�ʱ�ж���ر���
    int durationTime = 0;
    struct timeval tvEnrStart, tvEnrEnd;
    bool flagOverTime = false;//��ʱ�ı�־
    bool flagOverMinTime = false;//������Сע��ʱ��ı�־��������Сע��ʱ�������ע����

    //��ʼ��������
    for (int count = 0; count < numEnrL;count++)
    {
        IKInitAPIIrisInfo(enrIrisL+count);
    }

    for (int count = 0; count < numEnrR; count++)
    {
        IKInitAPIIrisInfo(enrIrisR+count);
    }

    for(int count = 0; count <= g_constMaxEnrollImNum; count++)
    {
        CASIAGenInitIrisInfo(&_leftEnrollImg[count]);
        CASIAGenInitIrisInfo(&_rightEnrollImg[count]);
    }

    gettimeofday(&tvEnrStart,NULL);
    while(_keepEnrFlag)
    {
        gettimeofday(&tvEnrEnd,NULL);

        durationTime = tvEnrEnd.tv_sec - tvEnrStart.tv_sec;
        if(durationTime > g_constMinENRTime)
        {
            flagOverMinTime = true;

            if(durationTime > maxEnrollTime)
            {
                flagOverTime = true;
            }
        }

        {
            //�����ɼ����ĺ�ͼ��ע������
            unique_lock<mutex> lck(_xferBestImgMutex);
            _swapNewGoodImgFlag = true;//֪ͨ���߳̿��Խ���ͼ��
            while(_swapNewGoodImgFlag)
            {
//                _xferBestImgCV.wait(lck);
                if(_xferBestImgCV.wait_for(lck,chrono::seconds(1)) == cv_status::timeout)
                {
                    break;
                }

            }

            if( _getLeftNewGoodImgFlag && leftEyeNeed )
            {
                leftEyeActive = true;
                _getLeftNewGoodImgFlag = false;
            }

            if( _getRightNewGoodImgFlag && rightEyeNeed )
            {
                rightEyeActive = true;
                _getRightNewGoodImgFlag = false;
            }

            //�ֱ�������۽���ע��
            if(leftEyeActive)
            {
                funResult = IKEnrollLRFeature(_algHandle, _algApiCtrlParam.IfSpoofDetect, _leftEnrollImg, &leftExistedNum,
                                              &_leftEnrollImg[g_constIREnrollTimeNum-1], _leftEnrMatchImgInfo, &_curBestEnrMatchImgInfo, strFunRts);
                leftEyeActive = false;//Ϊ��һ�ָ���ֵ
            }

            if(rightEyeActive)
            {
                funResult = IKEnrollLRFeature(_algHandle, _algApiCtrlParam.IfSpoofDetect, _rightEnrollImg, &rightExistedNum,
                                              &_rightEnrollImg[g_constIREnrollTimeNum-1], _rightEnrMatchImgInfo, &_curBestEnrMatchImgInfo, strFunRts);
                rightEyeActive = false;//Ϊ��һ�ָ���ֵ
            }

        }

        //�ж��Ƿ�ע��ɹ�
        if( (leftExistedNum >= g_constMaxEnrollImNum) && flagOverMinTime && leftEyeNeed)
        {
            for(int count = 0; count < numEnrL; count++)
            {
                IKCopyIRIrisInfo2APIIrisInfo(enrIrisL+count, &_leftEnrollImg[count]);
            }

            enrResult += EnrRecLeftSuccess;
            leftEyeNeed = false;
        }

        if( (rightExistedNum >= g_constMaxEnrollImNum) && flagOverMinTime && rightEyeNeed)
        {

            for(int count = 0; count < numEnrR; count++)
            {
                IKCopyIRIrisInfo2APIIrisInfo(enrIrisR+count, &_rightEnrollImg[count]);
            }

            enrResult += EnrRecRightSuccess;
            rightEyeNeed = false;
        }

        //�ж�������
        if(AnyEye == _algApiCtrlParam.EyeMode)
        {
            if(!leftEyeNeed || !rightEyeNeed)
            {
                rightEyeNeed = false;
                leftEyeNeed = false;
            }
        }

        if( (false == leftEyeNeed) && (false == rightEyeNeed) )
        {
            //success
            lock_guard<mutex> lck(_setEnrRecMutex);
            lrFlag = (LRSucFailFlag)enrResult;

            _keepEnrFlag = false;
            _outEnrFlag = true;
            SetFunRt(strFunRts,E_ALGAPI_OK, "Enroll success");
            return E_ALGAPI_OK;
        }
        else if(flagOverTime)
        {
            lock_guard<mutex> lck(_setEnrRecMutex);
            switch(_algApiCtrlParam.EyeMode)
            {
            case BothEye:
                enrResult -= EnrRecBothSuccess;
                break;

            case LeftEye:
                enrResult -= EnrRecLeftSuccess;
                break;

            case RightEye:
                enrResult -= EnrRecRightSuccess;
                break;

            case AnyEye:
                enrResult -= EnrRecBothSuccess;
                break;

            default:
                lrFlag = (LRSucFailFlag)enrResult;
                _keepEnrFlag = false;
                _outEnrFlag = true;
                SetFunRt(strFunRts,E_ALGAPI_INVALID_EYEMODE, "Invalid eyemode");
                return E_ALGAPI_INVALID_EYEMODE;
            }

            lrFlag = (LRSucFailFlag)enrResult;
            _keepEnrFlag = false;
            _outEnrFlag = true;
            SetFunRt(strFunRts,E_ALGAPI_ENR_IDEN_FAILED, "Enroll failed");
            return E_ALGAPI_ENR_IDEN_FAILED;
        }

    }//end while

    lock_guard<mutex> lck(_setEnrRecMutex);
    lrFlag = (LRSucFailFlag)enrResult;
    _keepEnrFlag = false;
    _outEnrFlag = true;
    SetFunRt(strFunRts,E_ALGAPI_ENR_IDEN_INTERRUPT, "Enroll process is stopped");
    return E_ALGAPI_ENR_IDEN_INTERRUPT;
}

/*****************************************************************************
*                         �㷨�߼�API��ʼע��
*  �� �� ����AsyncStartEnroll
*  ��    �ܣ��첽��ʽ��Ĥע��
*  ˵    ����
*  ��    ����appEnroll������������û�ע���ע�ắ��
*			 numEnrL��numEnrR�����������ע��õ������ۡ����ۺ�Ĥͼ��������ɵ�����ָ��
*-----------------------------------------------------------------------------------------
*���²�������
*            enrIrisL��enrIrisR�����������ע��õ������ۡ����ۺ�Ĥͼ�������Ϣ
*			 lrFlag��������������ۡ����ۺ�Ĥע��ɹ���־
*				EnrRecBothFailed	=	-3,		//EnrRecBothFailed��ע�ᡢʶ��ʱ�����۾�ע���ʶ��ʧ��
*				EnrRecRightFailed	=	-2,		//EnrRecRightFailed:����ע�ᡢʶ��ʧ�ܵ���ע���ʶ��ʧ��,�������δ֪
*				EnrRecLeftFailed	=	-1,		//EnrRecLeftFailed:����ע�ᡢʶ��ʧ�ܵ���ע���ʶ��ʧ��,�������δ֪
*				EnrRecLeftSuccess	=	1,		//EnrRecLeftSuccess:ע�ᡢʶ��ʱ������ע���ʶ��ɹ�
*				EnrRecRightSuccess	=	2,		//EnrRecRightSuccess:ע�ᡢʶ��ʱ������ע���ʶ��ɹ�
*				EnrRecBothSuccess	=	3		//EnrRecBothSuccess:ע�ᡢʶ��ʱ�����۾�ע��ɹ�
*-------------------------------------------------------------------------------------------
*			 strFunRts��������������ַ�����ʽ��ʾ���κ������ý�������û�����
*			 �ڴ�ռ��ڴ�ռ�
*
*  �� �� ֵ��0���������óɹ�
*    		<0����������ʧ�ܣ����ݷ���ֵ���Բ�ѯʧ��ԭ�򣬻��߲ο�strFunRts
*  �� �� �ˣ�lizhl
*  ����ʱ�䣺2013-11-05
*  �� �� �ˣ�
*  �޸�ʱ�䣺
*****************************************************************************/
int CAPIBase::AsyncStartEnroll(IN CBAlgApiEnroll appEnroll, IN int numEnrL, IN int numEnrR, OUT char *strFunRts)
{
    if(!_isWake)
    {
        SetFunRt(strFunRts,E_ALGAPI_SYSTEM_IS_SLEEP,"System is sleep");
        return E_ALGAPI_SYSTEM_IS_SLEEP;
    }

    _cbEnroll = appEnroll;
//    _cbEnrIrisL = enrIrisL;
    _cbNumEnrL = numEnrL;
//    _cbEnrIrisR = enrIrisR;
    _cbNumEnrR = numEnrR;
//    _cbEnrLRFlag = lrFlag;

    unique_lock <mutex> lck(_asyncEnrMutex);
    _asyncEnrFlag = true;
    _asyncEnrCV.notify_one();//�����źŸ��첽enroll�߳�
    SetFunRt(strFunRts,E_ALGAPI_OK, "Async start enroll success");
    return E_ALGAPI_OK;
}

/*****************************************************************************
*                         �㷨�߼�APIֹͣע��
*  �� �� ����AlgApiStopEnroll
*  ��    �ܣ�ֹͣ��Ĥע��
*  ˵    ����
*  ��    ����strFunRts��������������ַ�����ʽ��ʾ���κ������ý�������û�����
*			 �ڴ�ռ�
*
*  �� �� ֵ��0��ֹͣע��ɹ�
*    		<0��ֹͣע��ʧ�ܣ����ݷ���ֵ���Բ�ѯʧ��ԭ�򣬻��߲ο�strFunRts
*  �� �� �ˣ�lizhl
*  ����ʱ�䣺2013-11-05
*  �� �� �ˣ�
*  �޸�ʱ�䣺
*****************************************************************************/
int CAPIBase::StopEnroll(OUT char *strFunRts)
{
    if(!_isWake)
    {
        SetFunRt(strFunRts,E_ALGAPI_SYSTEM_IS_SLEEP,"System is sleep");
        return E_ALGAPI_SYSTEM_IS_SLEEP;
    }

    lock_guard<mutex> lck(_setEnrRecMutex);
    if(!_keepEnrFlag)
    {
        SetFunRt(strFunRts,E_ALGAPI_FAIL2STOP, "Stop enroll failed, enroll is not running");
        return E_ALGAPI_FAIL2STOP;
    }
    _keepEnrFlag = false;

    //�ȴ�enroll��������
    while(!_outEnrFlag)
    {
        usleep(0);
    }

    if(_cbEnroll)
    {
        InitEnrCBSrc();
    }

    SetFunRt(strFunRts,E_ALGAPI_OK, "Stop enroll success");
    return E_ALGAPI_OK;
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
*            strFunRts��������������ַ�����ʽ��ʾ���κ������ý�������û�����
*			 �ڴ�ռ�
*
*  �� �� ֵ��0��ʶ��ɹ�
*			 1����Ϊֹͣʶ��
*    		<0��ʶ��ʧ�ܣ����ݷ���ֵ���Բ�ѯʧ��ԭ�򣬻��߲ο�strFunRts
*  �� �� �ˣ�lizhl
*  ����ʱ�䣺2013-11-05
*  �� �� �ˣ�
*  �޸�ʱ�䣺
*****************************************************************************/
int CAPIBase::SyncStartIden(IN unsigned char *codeListL, IN int codeNumL, IN unsigned char *codeListR, IN int codeNumR,
                        OUT APIIrisInfo *recIrisL, OUT APIIrisInfo *recIrisR, OUT LRSucFailFlag &lrFlag,
                        OUT APIMatchOutput *pLeftMatchOutputParam, OUT  APIMatchOutput *pRightMatchOutputParam, OUT char *strFunRts)
{
    int maxRecTime = _algApiCtrlParam.MaxRECTime;//������ʱ�������û��ע��ɹ�������Ϊע��ʧ��
    bool leftEyeNeed = false;
    bool rightEyeNeed = false;
    bool leftEyeActive = false;
    bool rightEyeActive = false;

    {
        //��麯�����úϷ��Բ����ñ�־λ
        if(!_isWake)
        {
            SetFunRt(strFunRts,E_ALGAPI_SYSTEM_IS_SLEEP,"System is sleep");
            return E_ALGAPI_SYSTEM_IS_SLEEP;
        }

        lock_guard<mutex> lck(_setEnrRecMutex);

        if(_keepIdenFlag || _keepEnrFlag)
        {
            SetFunRt(strFunRts,E_ALGAPI_ALGORITHM_CONFLICT, "Start iden failed, enroll is running");
            return E_ALGAPI_ALGORITHM_CONFLICT;
        }

        if(maxRecTime > g_constMaxRECTime)
        {
            SetFunRt(strFunRts,E_ALGAPI_INVALID_TIME,"Invalid maxenrtime! Set algorithm's control parameter failed");
            return E_ALGAPI_INVALID_TIME;
        }

        switch(_algApiCtrlParam.EyeMode)
        {
        case BothEye:
            leftEyeNeed = true;
            rightEyeNeed = true;
            break;

        case LeftEye:
            leftEyeNeed = true;
            rightEyeNeed = false;
            break;

        case RightEye:
            leftEyeNeed = false;
            rightEyeNeed = true;
            break;

        case AnyEye:
            leftEyeNeed = true;
            rightEyeNeed = true;
            break;

        default:
            SetFunRt(strFunRts,E_ALGAPI_INVALID_EYEMODE,"Invalid eyemode! Set algorithm's control parameter failed");
            return E_ALGAPI_INVALID_EYEMODE;
        }

        if( ((true == leftEyeNeed) && (NULL == codeListL)) || ((true == rightEyeNeed) && (NULL == codeListR))  )
        {
            SetFunRt(strFunRts,E_ALGAPI_INVALID_REC_INPUT,"Invalid iden code list");
            return E_ALGAPI_INVALID_REC_INPUT;
        }

        _keepIdenFlag = true;
        _outIdenFlag = false;
    }

    //��ʼʶ��
    lrFlag = EnrRecUnknown;
    int recResult = 0;
    _qualityMode = IR_WORK_MODE_REC;
    int idenFailedCount = 0;//ʶ��ʧ�ܼ���
    int funRetL = E_ALGAPI_DEFAUL_ERROR;
    int funRetR = E_ALGAPI_DEFAUL_ERROR;

    MatchOutput outputMtPara;
    MatchInput inputPara;

    inputPara.angRotate[0]=IR_MAX_NEG_ROTATION_ANGLE;	//Ĭ�ϣ�left rotate
    inputPara.angRotate[1]=IR_MAX_POS_ROTATION_ANGLE;	//Ĭ�ϣ�right rotate
    inputPara.matchStrategy =IR_CASCADE_CLASSIFIER;		//Ĭ�ϣ�Not defined
    //20130704�ȶ���ֵ�����û�����
    inputPara.matchTh = IR_USE_DEAULT_MATCHTH;

    //ͨ��APP���õĲ������û����õĲ��֣�
    //inputPara.matchTh = pIKIRInfo->irPara.matchTh;		//APP ����
    inputPara.findMode = _algApiCtrlParam.FindMode;	//APP ����

    //����ʶ����㷨�ṹ��
    pIrisImageInfo pImgInfoL = NULL;
    pIrisImageInfo pImgInfoR = NULL;

    //�ͳ�ʱ�ж���ر���
    int durationTime = 0;
    struct timeval tvRecStart, tvRecEnd;
    bool flagOverTime = false;//��ʱ�ı�־

    //��ʼ��������,����Ĭ��ֵ
    IKInitAPIMatchOutput(pLeftMatchOutputParam);
    IKInitAPIMatchOutput(pRightMatchOutputParam);

    gettimeofday(&tvRecStart,NULL);
    while(_keepIdenFlag)
    {
        gettimeofday(&tvRecEnd,NULL);

        //Ϊ����ʶ�𸳳�ֵ
        leftEyeActive = false;
        rightEyeActive = false;

        durationTime = tvRecEnd.tv_sec - tvRecStart.tv_sec;
        if(durationTime > maxRecTime)
        {
            flagOverTime = true;
            lock_guard<mutex> lck(_setEnrRecMutex);
            lrFlag = (LRSucFailFlag)recResult;

            _keepIdenFlag = false;
            _outIdenFlag = true;
            SetFunRt(strFunRts,E_ALGAPI_IDEN_OVERTIME, "Iden overtime");
            return E_ALGAPI_IDEN_OVERTIME;
        }

        //�����ɼ����ĺ�ͼ��ʶ��
        {
            unique_lock<mutex> lck(_xferBestImgMutex);
            _swapNewGoodImgFlag = true;//֪ͨ���߳̿��Խ���ͼ��
            while(_swapNewGoodImgFlag)
            {
//                _xferBestImgCV.wait(lck);
                if(_xferBestImgCV.wait_for(lck,chrono::seconds(1)) == cv_status::timeout)
                {
                    break;
                }
            }

            if( _getLeftNewGoodImgFlag && leftEyeNeed )
            {
                leftEyeActive = true;
                _getLeftNewGoodImgFlag = false;
            }

            if( _getRightNewGoodImgFlag && rightEyeNeed )
            {
                rightEyeActive = true;
                _getRightNewGoodImgFlag = false;
            }

            //��ʼ������ȡ��ȶ�
            funRetL = E_ALGAPI_DEFAUL_ERROR;
            funRetR = E_ALGAPI_DEFAUL_ERROR;

            if(leftEyeActive)
            {
                pImgInfoL = &(_leftEnrollImg[g_constIREnrollTimeNum-1]);
                //ʶ������
                funRetL = IKIdenFeature(_algHandle, codeListL, codeNumL, pImgInfoL, inputPara, &outputMtPara, strFunRts);
                IKCopyIRIrisInfo2APIIrisInfo(recIrisL, pImgInfoL);

                if( (IR_FUN_SUCCESS == funRetL) && leftEyeNeed)
                {
                    IKInsertIRMtOutput2APIMtOutput(pLeftMatchOutputParam, &outputMtPara, g_constMtIsLeftEye);
                    leftEyeNeed = false;
                    recResult += EnrRecLeftSuccess;
                }

            }

            if(rightEyeActive)
            {
                pImgInfoR = &(_rightEnrollImg[g_constIREnrollTimeNum-1]);
                //ʶ������
                funRetR = IKIdenFeature(_algHandle, codeListR, codeNumR, pImgInfoR, inputPara, &outputMtPara, strFunRts);
                IKCopyIRIrisInfo2APIIrisInfo(recIrisR, pImgInfoR);
                if( (IR_FUN_SUCCESS == funRetR) && rightEyeNeed)
                {
                    IKInsertIRMtOutput2APIMtOutput(pRightMatchOutputParam, &outputMtPara, g_constMtIsRightEye);
                    rightEyeNeed = false;
                    recResult += EnrRecRightSuccess;
                }
            }
        }

        //�ж�������
        if(AnyEye == _algApiCtrlParam.EyeMode)
        {
            if(!leftEyeNeed || !rightEyeNeed)
            {
                rightEyeNeed = false;
                leftEyeNeed = false;
            }
        }

        if( (false == leftEyeNeed) && (false == rightEyeNeed) )
        {
            //success
            lock_guard<mutex> lck(_setEnrRecMutex);
            lrFlag = (LRSucFailFlag)recResult;

            _keepIdenFlag = false;
            _outIdenFlag = true;
            SetFunRt(strFunRts,E_ALGAPI_OK, "Iden success");
            return E_ALGAPI_OK;
        }
        else
        {
            //ʶ��ʧ�ܼ�����1
            idenFailedCount++;
            //�����˫��ʶ��ģʽ��ֻҪ��һֻ�۾�ʶ��ͨ������ʧ�ܼ���������
            if(BothEye == _algApiCtrlParam.EyeMode)
            {
                if( (false == leftEyeNeed) || (false == rightEyeNeed) )
                {
                    idenFailedCount--;
                }
            }

            if(idenFailedCount >= g_constIdenFailedTolerantNum)
            {
                lock_guard<mutex> lck(_setEnrRecMutex);
                switch(_algApiCtrlParam.EyeMode)
                {
                case BothEye:
                    recResult -= EnrRecBothSuccess;
                    break;

                case LeftEye:
                    recResult -= EnrRecLeftSuccess;
                    break;

                case RightEye:
                    recResult -= EnrRecRightSuccess;
                    break;

                case AnyEye:
                    recResult -= EnrRecBothSuccess;
                    break;

                default:
                    lrFlag = (LRSucFailFlag)recResult;
                    _keepIdenFlag = false;
                    _outIdenFlag = true;
                    SetFunRt(strFunRts,E_ALGAPI_INVALID_EYEMODE, "Invalid eyemode");
                    return E_ALGAPI_INVALID_EYEMODE;
                }

                lrFlag = (LRSucFailFlag)recResult;
                _keepIdenFlag = false;
                _outIdenFlag = true;
                SetFunRt(strFunRts,E_ALGAPI_ENR_IDEN_FAILED, "Iden failed");
                return E_ALGAPI_ENR_IDEN_FAILED;
            }
        }

    }

    lock_guard<mutex> lck(_setEnrRecMutex);
    lrFlag = (LRSucFailFlag)recResult;
    _keepIdenFlag = false;
    _outIdenFlag = true;
    SetFunRt(strFunRts,E_ALGAPI_ENR_IDEN_INTERRUPT, "Iden process is stopped");
    return E_ALGAPI_ENR_IDEN_INTERRUPT;
}

/*****************************************************************************
*                         �㷨�߼�API��ʼʶ��
*  �� �� ����AsyncStartIden
*  ��    �ܣ��첽��ʽ��Ĥʶ��
*  ˵    ����
*  ��    ����appIden������������û�ע���ʶ��ص�����
*            codeListL��codeListR��������������ۡ����ۺ�Ĥע������
*			 codeNumL��codeNumR��������������ۡ����ۺ�Ĥע����������
*----------------------------------------------------------------------------------------------
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
*-----------------------------------------------------------------------------------------------
*            strFunRts��������������ַ�����ʽ��ʾ���κ������ý�������û�����
*			 �ڴ�ռ�
*
*  �� �� ֵ��0���������óɹ�
*    		<0����������ʧ�ܣ����ݷ���ֵ���Բ�ѯʧ��ԭ�򣬻��߲ο�strFunRts
*  �� �� �ˣ�lizhl
*  ����ʱ�䣺2013-11-05
*  �� �� �ˣ�
*  �޸�ʱ�䣺
*****************************************************************************/
int CAPIBase::AsyncStartIden(IN CBAlgApiIden appIden, IN unsigned char *codeListL, IN int codeNumL, IN unsigned char *codeListR, IN int codeNumR, OUT char *strFunRts)
{
    if(!_isWake)
    {
        SetFunRt(strFunRts,E_ALGAPI_SYSTEM_IS_SLEEP,"System is sleep");
        return E_ALGAPI_SYSTEM_IS_SLEEP;
    }

    _cbIden = appIden;
    _cbCodeListL = codeListL;
    _cbCodeNumL = codeNumL;
    _cbCodeListR = codeListR;
    _cbCodeNumR = codeNumR;

//    _cbRecIrisL = recIrisL;
//    _cbRecIrisR = recIrisR;
//    _cbIdenLRFlag = lrFlag;
//    _cbLeftMatchOutputParam = pLeftMatchOutputParam;
//    _cbRightMatchOutputParam = pRightMatchOutputParam;

    unique_lock <mutex> lck(_asyncIdenMutex);
    _asyncIdenFlag = true;
    _asyncIdenCV.notify_one();//�����źŸ��첽iden�߳�
    SetFunRt(strFunRts,E_ALGAPI_OK, "Async start iden success");
    return E_ALGAPI_OK;
}

/*****************************************************************************
*                         �㷨�߼�APIֹͣʶ��
*  �� �� ����StopIden
*  ��    �ܣ�ֹͣ��Ĥʶ��
*  ˵    ����
*  ��    ����strFunRts��������������ַ�����ʽ��ʾ���κ������ý�������û�����
*			 �ڴ�ռ�
*
*  �� �� ֵ��0��ֹͣʶ��ɹ�
*    		<0��ֹͣʶ��ʧ�ܣ����ݷ���ֵ���Բ�ѯʧ��ԭ�򣬻��߲ο�strFunRts
*  �� �� �ˣ�lizhl
*  ����ʱ�䣺2013-11-05
*  �� �� �ˣ�
*  �޸�ʱ�䣺
*****************************************************************************/
int CAPIBase::StopIden(OUT char *strFunRts)
{
    if(!_isWake)
    {
        SetFunRt(strFunRts,E_ALGAPI_SYSTEM_IS_SLEEP,"System is sleep");
        return E_ALGAPI_SYSTEM_IS_SLEEP;
    }

    lock_guard<mutex> lck(_setEnrRecMutex);
    if(!_keepIdenFlag)
    {
        SetFunRt(strFunRts,E_ALGAPI_FAIL2STOP, "Stop iden failed, iden is not running");
        return E_ALGAPI_FAIL2STOP;
    }
    _keepIdenFlag = false;

    //�ȴ�iden��������
    while(!_outIdenFlag)
    {
        usleep(0);
    }

    if(_cbIden)
    {
        InitIdenCBSrc();
    }

    SetFunRt(strFunRts,E_ALGAPI_OK, "Stop iden success");
    return E_ALGAPI_OK;
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
*            strFunRts��������������ַ�����ʽ��ʾ���κ������ý�������û�����
*			 �ڴ�ռ�
*
*  �� �� ֵ��0���ȶԳɹ�
*    		<0���ȶ�ʧ�ܣ����ݷ���ֵ���Բ�ѯʧ��ԭ�򣬻��߲ο�strFunRts
*  �� �� �ˣ�lizhl
*  ����ʱ�䣺2013-11-05
*  �� �� �ˣ�
*  �޸�ʱ�䣺
*****************************************************************************/
int CAPIBase::Match(IN unsigned char *recCode, IN int recNum, IN unsigned char *codeList, IN int codeNum, OUT  APIMatchOutput *pMatchOutputParam, OUT char *strFunRts)
{
    if(!_isWake)
    {
        SetFunRt(strFunRts,E_ALGAPI_SYSTEM_IS_SLEEP,"System is sleep");
        return E_ALGAPI_SYSTEM_IS_SLEEP;
    }

    if( (recNum <= 0) || (codeNum <= 0) || (NULL == recCode) || (NULL == codeList) )
    {
        SetFunRt(strFunRts,E_ALGAPI_INVALID_MATCH_INPUT, "Invalid match input");
        return E_ALGAPI_INVALID_MATCH_INPUT;
    }

    for(int count = 0; count < recNum; count++)
    {
        //���ñȶԲ���
        MatchInput inputPara;
        inputPara.angRotate[0]=IR_MAX_NEG_ROTATION_ANGLE;	//Ĭ�ϣ�left rotate
        inputPara.angRotate[1]=IR_MAX_POS_ROTATION_ANGLE;	//Ĭ�ϣ�right rotate
        inputPara.matchStrategy =IR_CASCADE_CLASSIFIER;		//Ĭ�ϣ�Not defined

        //20130704�ȶ���ֵ�����û�����
        inputPara.matchTh = IR_USE_DEAULT_MATCHTH;
        inputPara.findMode = _algApiCtrlParam.FindMode;	//APP ����

        MatchOutput outputMtPara;
        int funResult = IR_DEFAULT_ERR;
        funResult = IKIR_Match(_algHandle, recCode + count*IR_REC_FEATURE_LENGTH, codeList, codeNum, inputPara, &outputMtPara, strFunRts);

        if(IR_FUN_SUCCESS == funResult)
        {
            IKInsertIRMtOutput2APIMtOutput(pMatchOutputParam, &outputMtPara, g_constMtUnknowEye);
            SetFunRt(strFunRts,E_ALGAPI_OK, "Match success");
            return E_ALGAPI_OK;
        }
    }

    SetFunRt(strFunRts,E_ALGAPI_MATCH_FAILED, "Match failed");
    return E_ALGAPI_MATCH_FAILED;
}

/*****************************************************************************
*                         ��ʼ���첽��ͼ���������Դ
*  �� �� ����InitCapCBSrc
*  ��    �ܣ���ʼ���첽��ͼ���������Դ
*  ˵    ����
*  ��    ����NULL
*
*  �� �� ֵ��NULL
*  �� �� �ˣ�lizhl
*  ����ʱ�䣺2013-12-04
*  �� �� �ˣ�
*  �޸�ʱ�䣺
*****************************************************************************/
void  CAPIBase::InitCapCBSrc(void)
{
    _cbCapIrisImg = NULL;
//    _cbIrisImg = NULL;
//    _myIrisImg = NULL;
//    _cbLRIrisClearFlag = NULL;
//    _cbIrisPositionFlag = NULL;
}

/*****************************************************************************
*                         ��ʼ���첽ע�ắ�������Դ
*  �� �� ����InitEnrCBSrc
*  ��    �ܣ���ʼ���첽ע�ắ�������Դ
*  ˵    ����
*  ��    ����NULL
*
*  �� �� ֵ��NULL
*  �� �� �ˣ�lizhl
*  ����ʱ�䣺2013-12-04
*  �� �� �ˣ�
*  �޸�ʱ�䣺
*****************************************************************************/
void  CAPIBase::InitEnrCBSrc(void)
{
    _cbEnroll = NULL;
//    _cbEnrIrisL = NULL;
    _cbNumEnrL = 0;
//    _cbEnrIrisR = NULL;
    _cbNumEnrR = 0;
//    _cbEnrLRFlag = NULL;
}

/*****************************************************************************
*                         ��ʼ���첽ʶ���������Դ
*  �� �� ����InitIdenCBSrc
*  ��    �ܣ���ʼ���첽ʶ���������Դ
*  ˵    ����
*  ��    ����NULL
*
*  �� �� ֵ��NULL
*  �� �� �ˣ�lizhl
*  ����ʱ�䣺2013-12-04
*  �� �� �ˣ�
*  �޸�ʱ�䣺
*****************************************************************************/
void  CAPIBase::InitIdenCBSrc(void)
{
    _cbIden = NULL;
    _cbCodeListL = NULL;
    _cbCodeNumL = 0;
    _cbCodeListR = NULL;
    _cbCodeNumR = 0;
//    _cbIdenLRFlag = NULL;

//    _cbRecIrisL = NULL;
//    _cbRecIrisR = NULL;
//    _cbLeftMatchOutputParam = NULL;
//    _cbRightMatchOutputParam = NULL;
}

/*****************************************************************************
*                         ���߳�
*  �� �� ����ThreadMain
*  ��    �ܣ���USB�豸��ͼ
*  ˵    ����
*  ��    ����pParam�����������CAPIBaseָ��
*
*  �� �� ֵ��NULL
*  �� �� �ˣ�lizhl
*  ����ʱ�䣺2013-11-19
*  �� �� �ˣ�
*  �޸�ʱ�䣺
*****************************************************************************/
void* CAPIBase::ThreadMain(IN CAPIBase* pParam)
{
    char strFunRts[g_constLengthOfStrFunRts];
    int funResult = E_ALGAPI_DEFAUL_ERROR;
    pParam->_lrIrisClearFlag = LAndRImgBlur;
    pParam->_irisPositionFlag = Unknown;

    int leftQualifiedImgNum = 0;
    int rightQualifiedImgNum = 0;
    int leftQualifiedImgScore = 0;
    int rightQualifiedImgScore = 0;
    int savedScoreL = 0;
    int savedScoreR = 0;

    int imgInformCount  = 0;//֪ͨ����������֪ͨAPP�Ĳ�ͼ����
    int imgSelectCount  = 0;//ͼ��ѡ�����������ע���ʶ��ʱ��ѡͼ�����

    unsigned int getImgLen = 0;

    CAPIGetFlag getFlag;

    while(pParam->_mainThreadFlag)
    {
        if(pParam->_isWake)
        {            
            //��ͼ���ŵ�_pCapImg��            

 #ifdef ALGAPITEST
            //++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
            //��Ԫ����ʱû����صĲ�ͼ�����ͺ�Ĥ����㷨����˶�Դ����������޸ģ���Ϊ���ļ���ͼ
//            CASIAGenInitIrisInfo(&pParam->_leftCurImg);
//            CASIAGenInitIrisInfo(&pParam->_rightCurImg);
            funResult = TestAlgApiReadImg(pParam->_pCapImg, &pParam->_leftCurImg, &pParam->_rightCurImg);
            if(E_ALGAPI_OK != funResult)
            {
                continue;
            }
            //�����к�Ĥ��⣬ֱ�Ӹ�ֵ
            funResult = E_ALGAPI_OK;
            pParam->_eyeDetectRet.lImFlag = IR_IM_EYE_LEFF;
            pParam->_eyeDetectRet.rImFlag = IR_IM_EYE_RIGHT;

            //++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
#else
            //TO DO
            funResult = pParam->_syncUsbApi->GetImage(pParam->_pCapImg, getImgLen);
            if(E_ALGAPI_OK != funResult)
            {
                continue;
            }

            /////////////////////////

            //��Ԫ����
            //��_pSwapImg�м���Ĥ
            funResult = IKIR_IrisDetection(pParam->_algHandle, pParam->_pCapImg, g_constIrisImgHeight, g_constIrisImgWidth,
                                           &pParam->_leftCurImg, &pParam->_rightCurImg, &pParam->_eyeDetectRet, strFunRts);

#endif


            if(E_ALGAPI_OK == funResult)
            {
                //�ж�����ͼ������
                if( (IR_IM_EYE_LEFF == pParam->_eyeDetectRet.lImFlag) && (RightEye != pParam->_algApiCtrlParam.EyeMode) )
                {
                    pParam->_leftCurImg.processStatus = IR_STATUS_BEGIN;

                    funResult = IKIR_JudgeImQuality(pParam->_algHandle, &pParam->_leftCurImg, pParam->_qualityMode, strFunRts);
                    if ( (IR_FUN_SUCCESS == funResult) && (pParam->_leftCurImg.QualityLevel >= IR_IMAGE_QUALITY_FAIR) )
                    {
                        savedScoreL = pParam->_leftCurImg.QualityScore;
                        leftQualifiedImgNum++;
                    }
                    else
                    {
                        savedScoreL = 0;
                    }
                }

                //�ж�����ͼ������
                if( (IR_IM_EYE_RIGHT == pParam->_eyeDetectRet.rImFlag) && (LeftEye != pParam->_algApiCtrlParam.EyeMode) )
                {
                    pParam->_rightCurImg.processStatus = IR_STATUS_BEGIN;

                    funResult = IKIR_JudgeImQuality(pParam->_algHandle, &pParam->_rightCurImg, pParam->_qualityMode, strFunRts);
                    if ( (IR_FUN_SUCCESS == funResult) && (pParam->_rightCurImg.QualityLevel >= IR_IMAGE_QUALITY_FAIR) )
                    {
                        savedScoreR = pParam->_rightCurImg.QualityScore;
                        rightQualifiedImgNum++;
                    }
                    else
                    {
                        savedScoreR = 0;
                    }
                }

            }

            //������ǰ�ɼ�����ͼ��
            if(pParam->_swapNewImgFlag)
            {
                unique_lock <mutex> lck(pParam->_xferCurrentImgMutex);

                imgInformCount++;
                memcpy(pParam->_pSwapImg, pParam->_pCapImg, g_constIrisImgSize);
                getFlag.GetDistanceFlag(imgInformCount, g_constImgNumToInform, &pParam->_irisPositionFlag);
                getFlag.GetClearFlag(imgInformCount, g_constImgNumToInform, leftQualifiedImgNum, rightQualifiedImgNum, &pParam->_lrIrisClearFlag);

                pParam->_swapNewImgFlag = false;
                pParam->_xferCurrentImgCV.notify_one();
            }


            //���������ע���ʶ�������������ݲ����¿�ʼ
            if( (!pParam->_keepEnrFlag) && (!pParam->_keepIdenFlag) )
            {
                CASIAGenInitIrisInfo(&pParam->_leftBestImg);
                CASIAGenInitIrisInfo(&pParam->_rightBestImg);

                imgSelectCount = 0;
                leftQualifiedImgNum = 0;
                rightQualifiedImgNum = 0;
                leftQualifiedImgScore = 0;
                rightQualifiedImgScore = 0;
                continue;
            }

            imgSelectCount++;
            if(savedScoreL > leftQualifiedImgScore)
            {
                CASIAGenCopyIrisInfo(&pParam->_leftBestImg,&pParam->_leftCurImg);
                leftQualifiedImgScore = savedScoreL;
            }

            if(savedScoreR > rightQualifiedImgScore)
            {
                CASIAGenCopyIrisInfo(&pParam->_rightBestImg,&pParam->_rightCurImg);
                rightQualifiedImgScore = savedScoreR;
            }

            //�����Ҫ�������ͼ�����Ѿ�ѡ�����㹻���ͼ���򽻻��ɼ��������ͼ���ע���ʶ����
            if( (pParam->_swapNewGoodImgFlag) && (imgSelectCount >= g_constImgNumToSelect) )
            {
                unique_lock<mutex> lck(pParam->_xferBestImgMutex);

                if(leftQualifiedImgNum >= g_constImgNumOfClear)
                {
                    CASIAGenCopyIrisInfo(&pParam->_leftEnrollImg[g_constIREnrollTimeNum-1], &pParam->_leftBestImg);
                    pParam->_getLeftNewGoodImgFlag = true;
                    CASIAGenInitIrisInfo(&pParam->_leftBestImg);
                }

                if(rightQualifiedImgNum >= g_constImgNumOfClear)
                {
                    CASIAGenCopyIrisInfo(&pParam->_rightEnrollImg[g_constIREnrollTimeNum-1], &pParam->_rightBestImg);
                    pParam->_getRightNewGoodImgFlag = true;
                    CASIAGenInitIrisInfo(&pParam->_rightBestImg);
                }

                if( (pParam->_getLeftNewGoodImgFlag) || (pParam->_getRightNewGoodImgFlag))
                {
                    imgSelectCount = 0;
                    leftQualifiedImgNum = 0;
                    rightQualifiedImgNum = 0;
                    leftQualifiedImgScore = 0;
                    rightQualifiedImgScore = 0;
                    pParam->_swapNewGoodImgFlag = false;
                    pParam->_xferBestImgCV.notify_one();
                }

            }
            //////////////////

        }
        else
        {
            //���ߣ��ȴ�����
            unique_lock <mutex> lck(pParam->_wakeMainThreadMutex);
            while(!pParam->_isWake)
            {
                pParam->_wakeMainThreadCV.wait(lck);
            }

        }

        usleep(0);
    }

    return ((void*)0);
}

/*****************************************************************************
*                         �첽��ͼ�߳�
*  �� �� ����ThreadAsyncCaptureImg
*  ��    �ܣ��첽��ʽ��ͼ��ͨ�������û��������Ļص�����֪ͨ�û��ɼ�ͼ��ɹ�
*  ˵    ����
*  ��    ����pParam�����������CAPIBaseָ��
*
*  �� �� ֵ��NULL
*  �� �� �ˣ�lizhl
*  ����ʱ�䣺2013-11-19
*  �� �� �ˣ�
*  �޸�ʱ�䣺
*****************************************************************************/
void* CAPIBase::ThreadAsyncCaptureImg(IN CAPIBase* pParam)
{
    char strFunRts[g_constLengthOfStrFunRts];
    int funResult = E_ALGAPI_DEFAUL_ERROR;

    while(pParam->_mainThreadFlag)
    {
        if(pParam->_isWake)
        {
            unique_lock <mutex> lck(pParam->_asyncCapImgMutex);
            while(!pParam->_asyncCapImgFlag)
            {
                pParam->_asyncCapImgCV.wait(lck);
            }

            //�ȵ������źź󣬵���ͬ����ͼ����
            funResult = pParam->SyncCapIrisImg(pParam->_cbIrisImg,
                                               pParam->_cbLRIrisClearFlag,
                                               pParam->_cbIrisPositionFlag, strFunRts);
            if(pParam->_cbCapIrisImg)
            {
                pParam->_cbCapIrisImg(funResult, pParam->_cbIrisImg, pParam->_cbLRIrisClearFlag, pParam->_cbIrisPositionFlag);
                pParam->InitCapCBSrc();
            }

            pParam->_asyncCapImgFlag = false;
        }

        usleep(0);//�˴���sleep��Ҫ�Ƿ�ֹ��������״̬��while������ѭ�����cpuռ�ã����Ǽ��������ܳ��ָ����
    }

    //�����Ҫ���ٱ��̣߳���Ҫ���������ж�


    return ((void*)0);
}

/*****************************************************************************
*                         �첽ע���߳�
*  �� �� ����ThreadAsyncEnroll
*  ��    �ܣ��첽��ʽע�ᣬͨ�������û��������Ļص�����֪ͨ�û�ע����
*  ˵    ����
*  ��    ����pParam�����������CAPIBaseָ��
*
*  �� �� ֵ��NULL
*  �� �� �ˣ�lizhl
*  ����ʱ�䣺2013-11-19
*  �� �� �ˣ�
*  �޸�ʱ�䣺
*****************************************************************************/
void* CAPIBase::ThreadAsyncEnroll(IN CAPIBase* pParam)
{
    char strFunRts[g_constLengthOfStrFunRts];
    int funResult = E_ALGAPI_DEFAUL_ERROR;

    while(pParam->_mainThreadFlag)
    {
        if(pParam->_isWake)
        {
            unique_lock <mutex> lck(pParam->_asyncEnrMutex);
            while(!pParam->_asyncEnrFlag)
            {
                pParam->_asyncEnrCV.wait(lck);
            }

            //�ȵ������źź󣬵���ͬ��ע�ắ��
            funResult = pParam->SyncStartEnroll(pParam->_cbEnrIrisL, pParam->_cbNumEnrL, pParam->_cbEnrIrisR, pParam->_cbNumEnrR, pParam->_cbEnrLRFlag, strFunRts);
            if(pParam->_cbEnroll)
            {
                pParam->_cbEnroll(funResult, pParam->_cbEnrIrisL, pParam->_cbNumEnrL, pParam->_cbEnrIrisR, pParam->_cbNumEnrR, pParam->_cbEnrLRFlag);
                pParam->InitEnrCBSrc();
            }

            pParam->_asyncEnrFlag = false;
        }

        usleep(0);//�˴���sleep��Ҫ�Ƿ�ֹ��������״̬��while������ѭ�����cpuռ�ã����Ǽ��������ܳ��ָ����
    }

    //�����Ҫ���ٱ��̣߳���Ҫ���������ж�


    return ((void*)0);

}

/*****************************************************************************
*                         �첽ʶ���߳�
*  �� �� ����ThreadAsyncIden
*  ��    �ܣ��첽��ʽʶ��ͨ�������û��������Ļص�����֪ͨ�û�ʶ����
*  ˵    ����
*  ��    ����pParam�����������CAPIBaseָ��
*
*  �� �� ֵ��NULL
*  �� �� �ˣ�lizhl
*  ����ʱ�䣺2013-11-19
*  �� �� �ˣ�
*  �޸�ʱ�䣺
*****************************************************************************/
void* CAPIBase::ThreadAsyncIden(IN CAPIBase* pParam)
{
    char strFunRts[g_constLengthOfStrFunRts];
    int funResult = E_ALGAPI_DEFAUL_ERROR;

    while(pParam->_mainThreadFlag)
    {
        if(pParam->_isWake && pParam->_asyncIdenFlag)
        {
            unique_lock <mutex> lck(pParam->_asyncIdenMutex);
            while(!pParam->_asyncIdenFlag)
            {
                pParam->_asyncIdenCV.wait(lck);
            }

            //�ȵ������źź󣬵���ͬ��ʶ����
            funResult = pParam->SyncStartIden(pParam->_cbCodeListL, pParam->_cbCodeNumL, pParam->_cbCodeListR, pParam->_cbCodeNumR,
                                              &pParam->_cbRecIrisL, &pParam->_cbRecIrisR, pParam->_cbIdenLRFlag,
                                              &pParam->_cbLeftMatchOutputParam, &pParam->_cbRightMatchOutputParam,strFunRts);
            if(pParam->_cbIden)
            {
                pParam->_cbIden(funResult, &pParam->_cbRecIrisL, &pParam->_cbRecIrisR, pParam->_cbIdenLRFlag,
                                &pParam->_cbLeftMatchOutputParam, &pParam->_cbRightMatchOutputParam);
                pParam->InitIdenCBSrc();
            }

            pParam->_asyncIdenFlag = false;
        }

        usleep(0);//�˴���sleep��Ҫ�Ƿ�ֹ��������״̬��while������ѭ�����cpuռ�ã����Ǽ��������ܳ��ָ����
    }

    //�����Ҫ���ٱ��̣߳���Ҫ���������ж�


    return ((void*)0);
}


