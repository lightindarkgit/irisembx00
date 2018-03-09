/******************************************************************************************
** �ļ���:   ApiGetFlag.h
���� ��Ҫ��:   CAPIGetFlag
**
** Copyright (c) �пƺ�����޹�˾
** ������:   lizhl
** ��  ��:   2013-11-29
** �޸���:
** ��  ��:
** ��  ��:   IKEMBX00��Ŀ�㷨�߼�API���ڻ�ȡ��Ĥͼ���Ƿ�������������Ϣ��־�����ⲻ�ɼ�
 *���� ��Ҫģ��˵��: ApiGetFlag.h
**
** ��  ��:   1.0.0
** ��  ע:
**
*****************************************************************************************/
#pragma once
#include "AlgApi.h"
#include "ApiBase.h"

//������ģ�鷵��ֵ����
//const int g_constDistanceNearValue =        24;     //�������ֵ����λcm
//const int g_constDistanceOKValue =          29;     //�������ֵ����λcm
//const int g_constDistanceFarValue =         33;     //�����Զֵ����λcm
//const int g_constDistanceNoPersonValue =    35;     //���ˣ���λcm

//��ȡ��Ĥͼ���Ƿ�������������Ϣ��־��
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
