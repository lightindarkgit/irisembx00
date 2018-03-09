/******************************************************************************************
** �ļ���:   ApiGetFlag.cpp
���� ��Ҫ��:   CAPIGetFlag
**
** Copyright (c) �пƺ�����޹�˾
** ������:   lizhl
** ��  ��:   2013-11-29
** �޸���:
** ��  ��:
** ��  ��:   IKEMBX00��Ŀ�㷨�߼�API���ڻ�ȡ��Ĥͼ���Ƿ�������������Ϣ��־�����ⲻ�ɼ�
 *���� ��Ҫģ��˵��: ApiGetFlag.h                  ��ȡ��Ĥͼ���Ƿ�������������Ϣ��־��
**
** ��  ��:   1.0.0
** ��  ע:
**
*****************************************************************************************/
#include "ApiGetFlag.h"

/*****************************************************************************
*                         CAPIGetFlag���캯��
*  �� �� ����CAPIGetFlag
*  ��    �ܣ����캯��
*  ˵    ����
*  ��    ����NULL
*  �� �� ֵ��NULL
*  �� �� �ˣ�lizhl
*  ����ʱ�䣺2013-11-29
*  �� �� �ˣ�
*  �޸�ʱ�䣺
*****************************************************************************/
CAPIGetFlag::CAPIGetFlag()
{
    _distanceValue = 0;
    _totalDistanceValue = 0;
}

/*****************************************************************************
*                         CAPIGetFlag��������
*  �� �� ����~CAPIGetFlag
*  ��    �ܣ���������
*  ˵    ����
*  ��    ����NULL
*  �� �� ֵ��NULL
*  �� �� �ˣ�lizhl
*  ����ʱ�䣺2013-11-29
*  �� �� �ˣ�
*  �޸�ʱ�䣺
*****************************************************************************/
CAPIGetFlag::~CAPIGetFlag()
{

}

/*****************************************************************************
*                         CAPIGetFlag��պ���
*  �� �� ����Clear
*  ��    �ܣ���պ���
*  ˵    ����
*  ��    ����NULL
*  �� �� ֵ��NULL
*  �� �� �ˣ�lizhl
*  ����ʱ�䣺2013-11-29
*  �� �� �ˣ�
*  �޸�ʱ�䣺
*****************************************************************************/
void CAPIGetFlag::Clear()
{
    _distanceValue = 0;
    _totalDistanceValue = 0;
}

/*****************************************************************************
*                         CAPIGetFlag��ȡ������Ϣ��־����
*  �� �� ����GetDistanceFlag
*  ��    �ܣ���ȡ������Ϣ��־
*  ˵    ����
*  ��    ����imgInformCount�������������ǰͼ�����
*           imgNumToInform�����������ÿimgNumToInform��ͼ����Ҫ�����־
*           pIrisPositionFlag�����������������Ϣ��־
*  �� �� ֵ��NULL
*  �� �� �ˣ�lizhl
*  ����ʱ�䣺2013-11-29
*  �� �� �ˣ�
*  �޸�ʱ�䣺
*****************************************************************************/
void CAPIGetFlag::GetDistanceFlag(IN int imgInformCount, IN int imgNumToInform, OUT IrisPositionFlag *pIrisPositionFlag)
{
    //��ȡ������ģ��ľ���ֵ
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

    _totalDistanceValue += _distanceValue;

    if( 0 == (imgInformCount % imgNumToInform) )
    {
        _distanceValue = _totalDistanceValue / imgNumToInform;

        if(_distanceValue >= g_constDistanceFarValue)
        {
            *pIrisPositionFlag = Far;
        }
        else if( (g_constDistanceFarValue > _distanceValue) && ( _distanceValue > g_constDistanceNearValue))
        {
            *pIrisPositionFlag = OK;
        }
        else if(_distanceValue <= g_constDistanceNearValue)
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
*                         CAPIGetFlag��ȡͼ��������־λ
*  �� �� ����GetClearFlag
*  ��    �ܣ���ȡͼ��������־λ
*  ˵    ����
*  ��    ����imgInformCount�������������ǰͼ�����
*           imgNumToInform�����������ÿimgNumToInform��ͼ����Ҫ�����־
*           leftQualifiedImgCount��������������ۺϸ�ͼ�����
*           rightQualifiedImgCount��������������ۺϸ�ͼ�����
*           pLRIrisClearFlag�����������ͼ��������־λ
*
*  �� �� ֵ��NULL
*  �� �� �ˣ�lizhl
*  ����ʱ�䣺2013-11-29
*  �� �� �ˣ�
*  �޸�ʱ�䣺
*****************************************************************************/
void CAPIGetFlag::GetClearFlag(int imgInformCount, int imgNumToInform, int leftQualifiedImgCount, int rightQualifiedImgCount, LRIrisClearFlag *pLRIrisClearFlag)
{
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
