/******************************************************************************************
** �ļ���:   UnitTestAlgApi.h
���� ��Ҫ��:   CUnitTestAlgApi
**
** Copyright (c) �пƺ�����޹�˾
** ������:   lizhl
** ��  ��:   2013-12-13
** �޸���:
** ��  ��:
** ��  ��:   AlgApi��Ԫ����ͷ�ļ�
 *���� ��Ҫģ��˵��:����һ���㷨�߼���Ԫ�����࣬�������ز���
**
** ��  ��:   1.0.0
** ��  ע:
**
*****************************************************************************************/
#pragma once
#include "IK_IR_DLL.h"
#include "AlgApi.h"
#include "gtest/gtest.h"
#include "VirtualReadImg.h"

//���洢ע����������
const int g_constFeatureNum = 10;
//��ͼ�ظ�����
const int g_constCapRepeatNum = 100;//0;//
//ע���ظ�����
const int g_constEnrRepeatNum = 5;//0;//
//ʶ���ظ�����
const int g_constRecRepeatNum = 100;//0;//100;

//�첽��ͼ�ظ�����
const int g_constAsyncCapRepeatNum = 10;//0;//
//Ϊ�˷��㲻���ظ��첽��ͼ������һ����־λ
static bool s_flagAsyncCapImg;

//�첽ע���ظ�����
const int g_constAsyncEnrRepeatNum = 2;//0;//
//Ϊ�˷��㲻���ظ��첽ע�ᣬ����һ����־λ
static bool s_flagAsyncEnr;

//�첽ʶ���ظ�����
const int g_constAsyncRecRepeatNum = 5;//0;//
//Ϊ�˷��㲻���ظ��첽ʶ������һ����־λ
static bool s_flagAsyncRec;


//�ص�����
//�첽��ͼ�ص�����
int CBAsyncCapIrisImg(int funResult, unsigned char* pIrisImg, LRIrisClearFlag lrIrisClearFlag, IrisPositionFlag lrIrisPositionFlag);

//�첽ע��ص�����
int CBAsyncEnroll(int funResult, APIIrisInfo* enrIrisL, int numEnrL, APIIrisInfo* enrIrisR, int numEnrR, LRSucFailFlag lrFlag);

//�첽ʶ��ص�����
int CBAsyncIden(int funResult, APIIrisInfo* recIrisL, APIIrisInfo* recIrisR, LRSucFailFlag lrFlag, APIMatchOutput* pLeftMatchOutputParam, APIMatchOutput* pRightMatchOutputParam);


//AlgApi��Ԫ������
class CUnitTestAlgApi : public testing::Test
{
public:
    CAlgApi *PTestAlgApi;
    int FunResult;

    //��ͼ��ز���
    unsigned char ImgBuffer[g_constIrisImgSize];
    LRIrisClearFlag ImgClearFlag;
    IrisPositionFlag ImgPositionFlag;

    //��ȡ�����ò�����ز���
    AlgApiCtrlParam ParamStruct;

    //ע����ز���
    APIIrisInfo EnrIrisL[g_constMaxEnrollImNum];
    APIIrisInfo EnrIrisR[g_constMaxEnrollImNum];
    int NumEnrL;
    int NumEnrR;
    LRSucFailFlag EnrLrFlag;

    //ʶ����ز���
    unsigned char CodeListL[g_constFeatureNum*IR_ENR_FEATURE_LENGTH];
    int CodeNumL;
    unsigned char CodeListR[g_constFeatureNum*IR_ENR_FEATURE_LENGTH];
    int CodeNumR;
    APIIrisInfo RecIrisL;
    APIIrisInfo RecIrisR;
    LRSucFailFlag RecLrFlag;
    APIMatchOutput LeftMatchOutputParam;
    APIMatchOutput RightMatchOutputParam;

    //�ȶ���ز���
    int RecNum;
    unsigned char *CodeList;
    int CodeNum;
    APIMatchOutput MatchOutputParam;

public:
    virtual void SetUp()
    {
        //�ú�����ÿ��caseִ��ǰ���ᱻ���ã����ڳ�ʼ�������Դ
        PTestAlgApi = new CAlgApi();
        NumEnrL = 3;
        NumEnrR = 3;

        s_flagAsyncCapImg = false;
        s_flagAsyncEnr = false;
        s_flagAsyncRec = false;

        CodeNumL = g_constFeatureNum;
        CodeNumR = g_constFeatureNum;

        //�������
        memset(CodeListL, 0, g_constFeatureNum*IR_ENR_FEATURE_LENGTH);
        memset(CodeListR, 0, g_constFeatureNum*IR_ENR_FEATURE_LENGTH);
    }

    virtual void TearDown()
    {
        //�ú�����ÿ��caseִ�к󶼻ᱻ���ã��������������Դ
        delete PTestAlgApi;
    }
};
