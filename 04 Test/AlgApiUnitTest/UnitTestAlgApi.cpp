/******************************************************************************************
** �ļ���:   UnitTestAlgApi.cpp
���� ��Ҫ��:
**
** Copyright (c) �пƺ�����޹�˾
** ������:   lizhl
** ��  ��:   2013-12-13
** �޸���:
** ��  ��:
** ��  ��:   IKEMBX00��Ŀ�㷨�߼�API����
 *���� ��Ҫģ��˵��: VirtualReadImg.h
**
** ��  ��:   1.0.0
** ��  ע:
**
*****************************************************************************************/
#include "UnitTestAlgApi.h"

/*****************************************************************************
*                         AlgApi��Ԫ����-��ʼ�������ͬ����ͼ����
*  �� �� ����
*  ��    �ܣ�AlgApi��ʼ�������ͬ����ͼ����
*  ˵    �������ڶ�ͼ��˳���ϵ��������Ӧ�÷��ڲ��Ե���ǰ�棬���򱾺����Ĳ����п��ܻ�ʧ��
*  ��    ����
*  �� �� ֵ��NULL
*  �� �� �ˣ�lizhl
*  ����ʱ�䣺2013-12-13
*  �� �� �ˣ�
*  �޸�ʱ�䣺
*****************************************************************************/
TEST_F(CUnitTestAlgApi, TestAlgApiSyncCapImg)
{
    //��ʼ��
    FunResult = PTestAlgApi->Init();
    EXPECT_EQ(E_ALGAPI_OK, FunResult);

    //���ڲ�ͼ�߳���ÿ����ͼ��֪ͨһ��flag�����ǰ���ζ���ϵͳ��ʼֵ�������βż���
    //ÿ6��ͼ��ǰ���β�ͼ�ľ���ֵΪ����
    for(int count = 0; count < 3; count++)
    {
        FunResult = PTestAlgApi->SyncCapIrisImg(ImgBuffer, ImgClearFlag, ImgPositionFlag);
        if(count < 2)
        {
            EXPECT_EQ(E_ALGAPI_OK, FunResult);
            EXPECT_EQ(LAndRImgBlur, ImgClearFlag);
            EXPECT_EQ(Unknown, ImgPositionFlag);
        }
    }

    EXPECT_EQ(E_ALGAPI_OK, FunResult);
    EXPECT_EQ(LAndRImgClear, ImgClearFlag);
    EXPECT_EQ(OK, ImgPositionFlag);

    //ÿ6��ͼ�񣬺����β�ͼ�ľ���ֵΪ��Զ
    for(int count = 0; count < 3; count++)
    {
        FunResult = PTestAlgApi->SyncCapIrisImg(ImgBuffer, ImgClearFlag, ImgPositionFlag);
        if(count < 2)
        {
            EXPECT_EQ(E_ALGAPI_OK, FunResult);
            EXPECT_EQ(LAndRImgClear, ImgClearFlag);
            EXPECT_EQ(OK, ImgPositionFlag);
        }
    }

    EXPECT_EQ(E_ALGAPI_OK, FunResult);
    EXPECT_EQ(LAndRImgClear, ImgClearFlag);
    EXPECT_EQ(Far, ImgPositionFlag);

    //ÿ6��ͼ��ǰ���β�ͼ�ľ���ֵΪ����
    for(int count = 0; count < 3; count++)
    {
        FunResult = PTestAlgApi->SyncCapIrisImg(ImgBuffer, ImgClearFlag, ImgPositionFlag);
        if(count < 2)
        {
            EXPECT_EQ(E_ALGAPI_OK, FunResult);
            EXPECT_EQ(LAndRImgClear, ImgClearFlag);
            EXPECT_EQ(Far, ImgPositionFlag);
        }
    }

    EXPECT_EQ(E_ALGAPI_OK, FunResult);
    EXPECT_EQ(LAndRImgClear, ImgClearFlag);
    EXPECT_EQ(OK, ImgPositionFlag);

    //�ظ���ͼ����
    for(int count = 0; count < g_constCapRepeatNum; count++)
    {
        FunResult = PTestAlgApi->SyncCapIrisImg(ImgBuffer, ImgClearFlag, ImgPositionFlag);
        EXPECT_EQ(E_ALGAPI_OK, FunResult);
    }
}

/*****************************************************************************
*                         AlgApi��Ԫ����-��ʼ��������첽��ͼ����
*  �� �� ����
*  ��    �ܣ�AlgApi��ʼ��������첽��ͼ����
*  ˵    ���������첽��ͼ������Ȼ���ڻص�������ȡ���
*  ��    ����
*  �� �� ֵ��NULL
*  �� �� �ˣ�lizhl
*  ����ʱ�䣺2013-12-19
*  �� �� �ˣ�
*  �޸�ʱ�䣺
*****************************************************************************/
TEST_F(CUnitTestAlgApi, TestAlgApiAsyncCapImg)
{
    //��ʼ��
    FunResult = PTestAlgApi->Init();
    EXPECT_EQ(E_ALGAPI_OK, FunResult);

    //�����첽��ͼ����
    std::cout << "AsyncCapImg's callback function should be called " << g_constAsyncCapRepeatNum << " times" << std::endl;
    for(int count = 0; count < g_constAsyncCapRepeatNum; count++)
    {
        FunResult = PTestAlgApi->AsyncCapIrisImg(CBAsyncCapIrisImg);
        EXPECT_EQ(E_ALGAPI_OK, FunResult);
        if(E_ALGAPI_OK != FunResult)
        {
            std::cout << "AsyncCapImg failed" << std::endl;
        }
        //�ȴ��첽��ͼ�ص����������ú��ٽ�����һ���첽��ͼ
        while(!s_flagAsyncCapImg)
        {
            sleep(1);
        }
        s_flagAsyncCapImg = false;
    }
}

/*****************************************************************************
*                         AlgApi��Ԫ����-�첽��ͼ�ص�����
*  �� �� ����
*  ��    �ܣ��첽��ͼ�ص�����
*  ˵    �����ڻص�������ȡ�첽��ͼ���
*  ��    ����
*  �� �� ֵ��NULL
*  �� �� �ˣ�lizhl
*  ����ʱ�䣺2013-12-19
*  �� �� �ˣ�
*  �޸�ʱ�䣺
*****************************************************************************/
int CBAsyncCapIrisImg(int funResult, unsigned char* pIrisImg, LRIrisClearFlag lrIrisClearFlag, IrisPositionFlag lrIrisPositionFlag)
{
    EXPECT_EQ(E_ALGAPI_OK, funResult);
    std::cout << "AsyncCapImg's callback function is called" << std::endl;
    s_flagAsyncCapImg = true;
}

/*****************************************************************************
*                         AlgApi��Ԫ����-ϵͳ����
*  �� �� ����
*  ��    �ܣ�AlgApiϵͳ����
*  ˵    ������Ҫ�ǲ��Գ�ʼ�������ߺͻ���
*  ��    ����
*  �� �� ֵ��NULL
*  �� �� �ˣ�lizhl
*  ����ʱ�䣺2013-12-13
*  �� �� �ˣ�
*  �޸�ʱ�䣺
*****************************************************************************/
TEST_F(CUnitTestAlgApi, TestSystemSet)
{
    //��ʼ��
    FunResult = PTestAlgApi->Init();
    EXPECT_EQ(E_ALGAPI_OK, FunResult);

    //��ʼ����ɼ�ͼ��
    FunResult = PTestAlgApi->SyncCapIrisImg(ImgBuffer, ImgClearFlag, ImgPositionFlag);
    EXPECT_EQ(E_ALGAPI_OK, FunResult);

    //δ���ߵ��û��Ѻ���
    FunResult = PTestAlgApi->Wake();
    EXPECT_EQ(E_ALGAPI_WAKE_FAILED, FunResult);

    //����
    FunResult = PTestAlgApi->Sleep();
    EXPECT_EQ(E_ALGAPI_OK, FunResult);

    //���ߺ���ø����ܺ�������ʱ�����ܺ���Ӧ�÷���ϵͳ��������״̬�Ĵ���
    FunResult = PTestAlgApi->SyncCapIrisImg(ImgBuffer, ImgClearFlag, ImgPositionFlag);
    EXPECT_EQ(E_ALGAPI_SYSTEM_IS_SLEEP, FunResult);

    FunResult = PTestAlgApi->GetParam(&ParamStruct);
    EXPECT_EQ(E_ALGAPI_SYSTEM_IS_SLEEP, FunResult);

    ParamStruct.EyeMode = BothEye;
    FunResult = PTestAlgApi->SetParam(&ParamStruct);
    EXPECT_EQ(E_ALGAPI_SYSTEM_IS_SLEEP, FunResult);

    FunResult = PTestAlgApi->SyncStartEnroll(EnrIrisL, NumEnrL, EnrIrisR, NumEnrR, EnrLrFlag);
    EXPECT_EQ(E_ALGAPI_SYSTEM_IS_SLEEP, FunResult);

    FunResult = PTestAlgApi->Match(EnrIrisL[0].IrisRecTemplate, 1, CodeListL, 1, &MatchOutputParam);
    EXPECT_EQ(E_ALGAPI_SYSTEM_IS_SLEEP, FunResult);

    FunResult = PTestAlgApi->StopEnroll();
    EXPECT_EQ(E_ALGAPI_SYSTEM_IS_SLEEP, FunResult);

    FunResult = PTestAlgApi->SyncStartIden(CodeListL, 1, CodeListR, 1, &RecIrisL, &RecIrisR,
                                           RecLrFlag, &LeftMatchOutputParam, &RightMatchOutputParam);
    EXPECT_EQ(E_ALGAPI_SYSTEM_IS_SLEEP, FunResult);

    FunResult = PTestAlgApi->StopIden();
    EXPECT_EQ(E_ALGAPI_SYSTEM_IS_SLEEP, FunResult);

    //���ߺ���ϵͳ��Ȼ����ø����ܺ���
    FunResult = PTestAlgApi->Wake();
    EXPECT_EQ(E_ALGAPI_OK, FunResult);

    FunResult = PTestAlgApi->SyncCapIrisImg(ImgBuffer, ImgClearFlag, ImgPositionFlag);
    EXPECT_EQ(E_ALGAPI_OK, FunResult);
}

/*****************************************************************************
*                         AlgApi��Ԫ����-�㷨�߼����Ʋ�������
*  �� �� ����
*  ��    �ܣ�AlgApi�㷨�߼����Ʋ�������
*  ˵    ���������㷨�߼����Ʋ�������ȷ����������
*  ��    ����
*  �� �� ֵ��NULL
*  �� �� �ˣ�lizhl
*  ����ʱ�䣺2013-12-18
*  �� �� �ˣ�
*  �޸�ʱ�䣺
*****************************************************************************/
TEST_F(CUnitTestAlgApi, TestAlgApiControlParamSet)
{
    //��ʼ��
    FunResult = PTestAlgApi->Init();
    EXPECT_EQ(E_ALGAPI_OK, FunResult);

    //�����������
    FunResult = PTestAlgApi->GetParam(&ParamStruct);
    EXPECT_EQ(E_ALGAPI_OK, FunResult);
    ParamStruct.EyeMode = 5;
    FunResult = PTestAlgApi->SetParam(&ParamStruct);
    EXPECT_EQ(E_ALGAPI_INVALID_EYEMODE, FunResult);

    FunResult = PTestAlgApi->GetParam(&ParamStruct);
    EXPECT_EQ(E_ALGAPI_OK, FunResult);
    ParamStruct.FindMode = 2;
    FunResult = PTestAlgApi->SetParam(&ParamStruct);
    EXPECT_EQ(E_ALGAPI_INVALID_FINDMODE, FunResult);

    FunResult = PTestAlgApi->GetParam(&ParamStruct);
    EXPECT_EQ(E_ALGAPI_OK, FunResult);
    ParamStruct.IfSpoofDetect = 2;
    FunResult = PTestAlgApi->SetParam(&ParamStruct);
    EXPECT_EQ(E_ALGAPI_INVALID_IFSPOOFDETECT, FunResult);

    FunResult = PTestAlgApi->GetParam(&ParamStruct);
    EXPECT_EQ(E_ALGAPI_OK, FunResult);
    ParamStruct.MaxENRTime = 5;
    FunResult = PTestAlgApi->SetParam(&ParamStruct);
    EXPECT_EQ(E_ALGAPI_INVALID_TIME, FunResult);

    FunResult = PTestAlgApi->GetParam(&ParamStruct);
    EXPECT_EQ(E_ALGAPI_OK, FunResult);
    ParamStruct.MaxRECTime = 15;
    FunResult = PTestAlgApi->SetParam(&ParamStruct);
    EXPECT_EQ(E_ALGAPI_INVALID_TIME, FunResult);

    //��ȷ��������
    FunResult = PTestAlgApi->GetParam(&ParamStruct);
    EXPECT_EQ(E_ALGAPI_OK, FunResult);
    ParamStruct.EyeMode = RightEye;
    FunResult = PTestAlgApi->SetParam(&ParamStruct);
    EXPECT_EQ(E_ALGAPI_OK, FunResult);

    FunResult = PTestAlgApi->GetParam(&ParamStruct);
    EXPECT_EQ(E_ALGAPI_OK, FunResult);
    ParamStruct.FindMode = g_constFindModeExhaust;
    FunResult = PTestAlgApi->SetParam(&ParamStruct);
    EXPECT_EQ(E_ALGAPI_OK, FunResult);

    FunResult = PTestAlgApi->GetParam(&ParamStruct);
    EXPECT_EQ(E_ALGAPI_OK, FunResult);
    ParamStruct.IfSpoofDetect = g_constIKDoSpoofDetect;
    FunResult = PTestAlgApi->SetParam(&ParamStruct);
    EXPECT_EQ(E_ALGAPI_OK, FunResult);

    FunResult = PTestAlgApi->GetParam(&ParamStruct);
    EXPECT_EQ(E_ALGAPI_OK, FunResult);
    ParamStruct.MaxENRTime = 15;
    FunResult = PTestAlgApi->SetParam(&ParamStruct);
    EXPECT_EQ(E_ALGAPI_OK, FunResult);

    FunResult = PTestAlgApi->GetParam(&ParamStruct);
    EXPECT_EQ(E_ALGAPI_OK, FunResult);
    ParamStruct.MaxRECTime = 2;
    FunResult = PTestAlgApi->SetParam(&ParamStruct);
    EXPECT_EQ(E_ALGAPI_OK, FunResult);

}

/*****************************************************************************
*                         AlgApi��Ԫ����-δ��ʼ�������ø��ӿں���
*  �� �� ����
*  ��    �ܣ�AlgApiδ������ʼ�������ø��ӿں���
*  ˵    ����
*  ��    ����
*  �� �� ֵ��NULL
*  �� �� �ˣ�lizhl
*  ����ʱ�䣺2013-12-13
*  �� �� �ˣ�
*  �޸�ʱ�䣺
*****************************************************************************/
TEST_F(CUnitTestAlgApi, TestAlgApiWithoutInit)
{
    FunResult = PTestAlgApi->Sleep();
    EXPECT_EQ(E_ALGAPI_INIT_FAILED, FunResult);

    FunResult = PTestAlgApi->Wake();
    EXPECT_EQ(E_ALGAPI_INIT_FAILED, FunResult);

    FunResult = PTestAlgApi->GetParam(&ParamStruct);
    EXPECT_EQ(E_ALGAPI_INIT_FAILED, FunResult);

    FunResult = PTestAlgApi->SetParam(&ParamStruct);
    EXPECT_EQ(E_ALGAPI_INIT_FAILED, FunResult);

    FunResult = PTestAlgApi->SyncCapIrisImg(ImgBuffer, ImgClearFlag, ImgPositionFlag);
    EXPECT_EQ(E_ALGAPI_INIT_FAILED, FunResult);

    FunResult = PTestAlgApi->SyncStartEnroll(EnrIrisL, NumEnrL, EnrIrisR, NumEnrR, EnrLrFlag);
    EXPECT_EQ(E_ALGAPI_INIT_FAILED, FunResult);

    CodeList = CodeListL;
    FunResult = PTestAlgApi->Match(EnrIrisL[0].IrisRecTemplate, RecNum, CodeList, CodeNum, &MatchOutputParam);
    EXPECT_EQ(E_ALGAPI_INIT_FAILED, FunResult);

    FunResult = PTestAlgApi->StopEnroll();
    EXPECT_EQ(E_ALGAPI_INIT_FAILED, FunResult);

    FunResult = PTestAlgApi->SyncStartIden(CodeListL, CodeNumL, CodeListR, CodeNumR, &RecIrisL, &RecIrisR, RecLrFlag,
                                           &LeftMatchOutputParam, &RightMatchOutputParam);
    EXPECT_EQ(E_ALGAPI_INIT_FAILED, FunResult);

    FunResult = PTestAlgApi->StopIden();
    EXPECT_EQ(E_ALGAPI_INIT_FAILED, FunResult);
}

/*****************************************************************************
*                         AlgApi��Ԫ����-ͬ����Ĥע��
*  �� �� ����
*  ��    �ܣ�ͬ����Ĥע��
*  ˵    �����ڲ�ͬ�۾�ģʽ�½���ע����ԣ�����˫�ۡ����ۡ����ۡ������ۣ�ע�����бȶ�
*  ��    ����
*  �� �� ֵ��NULL
*  �� �� �ˣ�lizhl
*  ����ʱ�䣺2013-12-18
*  �� �� �ˣ�
*  �޸�ʱ�䣺
*****************************************************************************/
TEST_F(CUnitTestAlgApi, TestSyncEnroll)
{
    //��ʼ��
    FunResult = PTestAlgApi->Init();
    EXPECT_EQ(E_ALGAPI_OK, FunResult);

    //ע��õ���ͼ�������·�ת�ģ�����ڱ���ʱ��Ҫ��תһ�£�����һ��buffer���洢��תͼ��
    unsigned char imgFlip[g_constIKImgSize];

    //ע��
    //˫��ע��
    //�����۾�ģʽ
    FunResult = PTestAlgApi->GetParam(&ParamStruct);
    EXPECT_EQ(E_ALGAPI_OK, FunResult);

    ParamStruct.EyeMode = BothEye;
    FunResult = PTestAlgApi->SetParam(&ParamStruct);
    EXPECT_EQ(E_ALGAPI_OK, FunResult);

    FunResult = PTestAlgApi->SyncCapIrisImg(ImgBuffer, ImgClearFlag, ImgPositionFlag);
    EXPECT_EQ(E_ALGAPI_OK, FunResult);

    //��ʼע��
    FunResult = PTestAlgApi->SyncStartEnroll(EnrIrisL, NumEnrL, EnrIrisR, NumEnrR, EnrLrFlag);
    EXPECT_EQ(E_ALGAPI_OK, FunResult);
    std::cout << "SyncStartEnroll function is success: BotyEye" << std::endl;

    if(E_ALGAPI_OK == FunResult)
    {
        for (int count=0; count < NumEnrL; count++)
        {
            char fileName[g_constLen];
            memset(fileName, 0, g_constLen);

            sprintf(fileName,"/home/fpc/MyIKEMB/algapiubuntu/enrL_%1d.enr", count);
            FunResult = FeatureSave(fileName, EnrIrisL[count].IrisEnrTemplate, IR_ENR_FEATURE_LENGTH);
            EXPECT_EQ(E_ALGAPI_OK, FunResult);



            sprintf(fileName,"/home/fpc/MyIKEMB/algapiubuntu/imgEnrL_%1d.ppm", count);
            ImageMirror(imgFlip, EnrIrisL[count].ImgData, g_constIKImgHeight, g_constIKImgWidth, false);
            FunResult = SaveGrayPPMFile(fileName, imgFlip, g_constIKImgWidth, g_constIKImgHeight);
            EXPECT_EQ(E_ALGAPI_OK, FunResult);
        }

        for (int count=0; count < NumEnrR; count++)
        {
            char fileName[g_constLen];
            memset(fileName, 0, g_constLen);

            sprintf(fileName,"/home/fpc/MyIKEMB/algapiubuntu/enrR_%1d.enr", count);
            FunResult = FeatureSave(fileName, EnrIrisR[count].IrisEnrTemplate, IR_ENR_FEATURE_LENGTH);
            EXPECT_EQ(E_ALGAPI_OK, FunResult);

            sprintf(fileName,"/home/fpc/MyIKEMB/algapiubuntu/imgEnrR_%1d.ppm", count);
            ImageMirror(imgFlip, EnrIrisR[count].ImgData, g_constIKImgHeight, g_constIKImgWidth, false);
            FunResult = SaveGrayPPMFile(fileName, imgFlip, g_constIKImgWidth, g_constIKImgHeight);
            EXPECT_EQ(E_ALGAPI_OK, FunResult);
        }

        //ע���ȶ�
        //������һ������ע���������������Բ�ͬ�۾�����˱ȶ�ʧ��
        FunResult = FeatrueLoad("/home/fpc/MyIKEMB/algapiubuntu/right_old.enr", CodeListL, IR_ENR_FEATURE_LENGTH);
        EXPECT_EQ(E_ALGAPI_OK, FunResult);
        FunResult = PTestAlgApi->Match(EnrIrisL[0].IrisRecTemplate, 1, CodeListL, 1, &MatchOutputParam);
        EXPECT_EQ(E_ALGAPI_MATCH_FAILED, FunResult);

        //��������һ������ע����������������ͬһ���˵����ۣ���˱ȶԳɹ�
        FunResult = FeatrueLoad("/home/fpc/MyIKEMB/algapiubuntu/left_old.enr", CodeListL, IR_ENR_FEATURE_LENGTH);
        EXPECT_EQ(E_ALGAPI_OK, FunResult);
        FunResult = PTestAlgApi->Match(EnrIrisL[0].IrisRecTemplate, 1, CodeListL, 1, &MatchOutputParam);
        EXPECT_EQ(E_ALGAPI_OK, FunResult);
    }

    for(int count = 0; count < g_constEnrRepeatNum; count++)
    {
        FunResult = PTestAlgApi->SyncStartEnroll(EnrIrisL, NumEnrL, EnrIrisR, NumEnrR, EnrLrFlag);
        EXPECT_EQ(E_ALGAPI_OK, FunResult);
        if(E_ALGAPI_OK != FunResult)
        {
            std::cout << "BothEye enroll failed, count = " << count << std::endl;
        }
        std::cout << "SyncStartEnroll function is success: BothEye, repeat count = " << count << std::endl;
    }

    //����ע��
    //�����۾�ģʽ
    FunResult = PTestAlgApi->GetParam(&ParamStruct);
    EXPECT_EQ(E_ALGAPI_OK, FunResult);

    ParamStruct.EyeMode = LeftEye;
    FunResult = PTestAlgApi->SetParam(&ParamStruct);
    EXPECT_EQ(E_ALGAPI_OK, FunResult);

    //��ʼע��
    for(int count = 0; count < g_constEnrRepeatNum; count++)
    {
        FunResult = PTestAlgApi->SyncStartEnroll(EnrIrisL, NumEnrL, EnrIrisR, NumEnrR, EnrLrFlag);
        EXPECT_EQ(E_ALGAPI_OK, FunResult);
        if(E_ALGAPI_OK != FunResult)
        {
            std::cout << "LeftEye enroll failed, count = " << count << std::endl;
        }
        std::cout << "SyncStartEnroll function is success: LeftEye, repeat count = " << count << std::endl;
    }


    //����ע��
    //�����۾�ģʽ
    FunResult = PTestAlgApi->GetParam(&ParamStruct);
    EXPECT_EQ(E_ALGAPI_OK, FunResult);

    ParamStruct.EyeMode = RightEye;
    FunResult = PTestAlgApi->SetParam(&ParamStruct);
    EXPECT_EQ(E_ALGAPI_OK, FunResult);

    //��ʼע��
    for(int count = 0; count < g_constEnrRepeatNum; count++)
    {
        FunResult = PTestAlgApi->SyncStartEnroll(EnrIrisL, NumEnrL, EnrIrisR, NumEnrR, EnrLrFlag);
        EXPECT_EQ(E_ALGAPI_OK, FunResult);
        if(E_ALGAPI_OK != FunResult)
        {
            std::cout << "RightEye enroll failed, count = " << count << std::endl;
        }
        std::cout << "SyncStartEnroll function is success: RightEye, repeat count = " << count << std::endl;
    }


    //������ע��
    //�����۾�ģʽ
    FunResult = PTestAlgApi->GetParam(&ParamStruct);
    EXPECT_EQ(E_ALGAPI_OK, FunResult);

    ParamStruct.EyeMode = AnyEye;
    FunResult = PTestAlgApi->SetParam(&ParamStruct);
    EXPECT_EQ(E_ALGAPI_OK, FunResult);
    //��ʼע��
    for(int count = 0; count < g_constEnrRepeatNum; count++)
    {
        FunResult = PTestAlgApi->SyncStartEnroll(EnrIrisL, NumEnrL, EnrIrisR, NumEnrR, EnrLrFlag);
        EXPECT_EQ(E_ALGAPI_OK, FunResult);
        if(E_ALGAPI_OK != FunResult)
        {
            //            printf("count = %d\n",count);
            std::cout << "AnyEye enroll failed, count = " << count << std::endl;
        }
        std::cout << "SyncStartEnroll function is success: AnyEye, repeat count = " << count << std::endl;
    }

}

/*****************************************************************************
*                         AlgApi��Ԫ����-�첽��Ĥע��
*  �� �� ����
*  ��    �ܣ��첽��Ĥע��
*  ˵    ��������ͬ��ģʽ���Ѿ�������֤�����ֻ��˫��ģʽ�½���ע�����
*  ��    ����
*  �� �� ֵ��NULL
*  �� �� �ˣ�lizhl
*  ����ʱ�䣺2013-12-19
*  �� �� �ˣ�
*  �޸�ʱ�䣺
*****************************************************************************/
TEST_F(CUnitTestAlgApi, TestAsyncEnroll)
{
    //��ʼ��
    FunResult = PTestAlgApi->Init();
    EXPECT_EQ(E_ALGAPI_OK, FunResult);

    //ע��
    //˫��ע��
    //�����۾�ģʽ
    FunResult = PTestAlgApi->GetParam(&ParamStruct);
    EXPECT_EQ(E_ALGAPI_OK, FunResult);

    ParamStruct.EyeMode = BothEye;
    FunResult = PTestAlgApi->SetParam(&ParamStruct);
    EXPECT_EQ(E_ALGAPI_OK, FunResult);

    bool onceFlag = false;

    std::cout << "AsyncEnroll's callback function should be called " << g_constAsyncEnrRepeatNum << " times" << std::endl;
    for(int count = 0; count < g_constAsyncEnrRepeatNum; count++)
    {
        FunResult = PTestAlgApi->AsyncStartEnroll(CBAsyncEnroll, NumEnrL, NumEnrR);
        EXPECT_EQ(E_ALGAPI_OK, FunResult);
        if(E_ALGAPI_OK != FunResult)
        {
            std::cout << "AsyncEnroll failed" << std::endl;
        }

        //�ȴ��첽��ͼ�ص����������ú��ٽ�����һ���첽��ͼ
        while(!s_flagAsyncEnr)
        {
//            if(!onceFlag)
//            {
//                onceFlag = true;
//                FunResult = PTestAlgApi->AsyncStartEnroll(CBAsyncEnroll, EnrIrisL, NumEnrL, EnrIrisR, NumEnrR, EnrLrFlag);
//                EXPECT_EQ(E_ALGAPI_ALGORITHM_CONFLICT, FunResult);
//            }
            sleep(1);
        }
        s_flagAsyncEnr = false;
    }
}

/*****************************************************************************
*                         AlgApi��Ԫ����-�첽ע��ص�����
*  �� �� ����
*  ��    �ܣ��첽ע��ص�����
*  ˵    �����ڻص�������ȡ�첽ע����
*  ��    ����
*  �� �� ֵ��NULL
*  �� �� �ˣ�lizhl
*  ����ʱ�䣺2013-12-19
*  �� �� �ˣ�
*  �޸�ʱ�䣺
*****************************************************************************/
int CBAsyncEnroll(int funResult, APIIrisInfo* enrIrisL, int numEnrL, APIIrisInfo* enrIrisR, int numEnrR, LRSucFailFlag lrFlag)
{
    EXPECT_EQ(E_ALGAPI_OK, funResult);
    EXPECT_EQ(EnrRecBothSuccess, lrFlag);
    std::cout << "AsyncEnroll's callback function is called" << std::endl;

    static bool tmpFlag = true;
    //ע��õ���ͼ�������·�ת�ģ�����ڱ���ʱ��Ҫ��תһ�£�����һ��buffer���洢��תͼ��
    unsigned char imgFlip[g_constIKImgSize];

    if((E_ALGAPI_OK == funResult) && tmpFlag)
    {
        for (int count=0; count < numEnrL; count++)
        {
            char fileName[g_constLen];
            memset(fileName, 0, g_constLen);

            sprintf(fileName,"/home/fpc/MyIKEMB/algapiubuntu/asyncEnrL_%1d.enr", count);
            funResult = FeatureSave(fileName, enrIrisL[count].IrisEnrTemplate, IR_ENR_FEATURE_LENGTH);
            EXPECT_EQ(E_ALGAPI_OK, funResult);

            sprintf(fileName,"/home/fpc/MyIKEMB/algapiubuntu/asyncImgEnrL_%1d.ppm", count);
            ImageMirror(imgFlip, enrIrisL[count].ImgData, g_constIKImgHeight, g_constIKImgWidth, false);
            funResult = SaveGrayPPMFile(fileName, imgFlip, g_constIKImgWidth, g_constIKImgHeight);
            EXPECT_EQ(E_ALGAPI_OK, funResult);
        }

        for (int count=0; count < numEnrR; count++)
        {
            char fileName[g_constLen];
            memset(fileName, 0, g_constLen);

            sprintf(fileName,"/home/fpc/MyIKEMB/algapiubuntu/asyncEnrR_%1d.enr", count);
            funResult = FeatureSave(fileName, enrIrisR[count].IrisEnrTemplate, IR_ENR_FEATURE_LENGTH);
            EXPECT_EQ(E_ALGAPI_OK, funResult);

            sprintf(fileName,"/home/fpc/MyIKEMB/algapiubuntu/asyncImgEnrR_%1d.ppm", count);
            ImageMirror(imgFlip, enrIrisR[count].ImgData, g_constIKImgHeight, g_constIKImgWidth, false);
            funResult = SaveGrayPPMFile(fileName, imgFlip, g_constIKImgWidth, g_constIKImgHeight);
            EXPECT_EQ(E_ALGAPI_OK, funResult);
        }

        tmpFlag = false;
    }

    s_flagAsyncEnr = true;
}

/*****************************************************************************
*                         AlgApi��Ԫ����-ͬ����Ĥʶ��
*  �� �� ����
*  ��    �ܣ�ͬ����Ĥʶ��
*  ˵    �����ڲ�ͬ�۾�ģʽ�½���ʶ����ԣ��������ۡ����ۡ�˫�ۣ�ע�����бȶ�
*  ��    ����
*  �� �� ֵ��NULL
*  �� �� �ˣ�lizhl
*  ����ʱ�䣺2013-12-18
*  �� �� �ˣ�
*  �޸�ʱ�䣺
*****************************************************************************/
TEST_F(CUnitTestAlgApi, TestSyncIden)
{
    //��ʼ��
    FunResult = PTestAlgApi->Init();
    EXPECT_EQ(E_ALGAPI_OK, FunResult);

    //ʶ��
    //ʶ������
    FunResult = PTestAlgApi->GetParam(&ParamStruct);
    EXPECT_EQ(E_ALGAPI_OK, FunResult);

    ParamStruct.EyeMode = LeftEye;
    FunResult = PTestAlgApi->SetParam(&ParamStruct);
    EXPECT_EQ(E_ALGAPI_OK, FunResult);

    //left_old.enr�ļ������ע�������Ǵ�windowsƽ̨ע��õ���ע������
    FunResult = FeatrueLoad("/home/fpc/MyIKEMB/algapiubuntu/left_old.enr", CodeListL, IR_ENR_FEATURE_LENGTH);
    EXPECT_EQ(E_ALGAPI_OK, FunResult);
    for(int count = 0; count < g_constRecRepeatNum; count++)
    {
        FunResult = PTestAlgApi->SyncStartIden(CodeListL, 1, CodeListR, 0, &RecIrisL, &RecIrisR,
                                               RecLrFlag, &LeftMatchOutputParam, &RightMatchOutputParam);
        EXPECT_EQ(E_ALGAPI_OK, FunResult);
        if(E_ALGAPI_OK != FunResult)
        {
            std::cout << "LeftEye iden failed, count = " << count << "\n" << std::endl;
        }
    }

    //ʶ������
    FunResult = PTestAlgApi->GetParam(&ParamStruct);
    EXPECT_EQ(E_ALGAPI_OK, FunResult);

    ParamStruct.EyeMode = RightEye;
    FunResult = PTestAlgApi->SetParam(&ParamStruct);
    EXPECT_EQ(E_ALGAPI_OK, FunResult);

    //right_old.enr�ļ������ע�������Ǵ�windowsƽ̨ע��õ���ע������
    FunResult = FeatrueLoad("/home/fpc/MyIKEMB/algapiubuntu/right_old.enr", CodeListR, IR_ENR_FEATURE_LENGTH);
    EXPECT_EQ(E_ALGAPI_OK, FunResult);
    for(int count = 0; count < g_constRecRepeatNum; count++)
    {
        FunResult = PTestAlgApi->SyncStartIden(CodeListL, 0, CodeListR, 1, &RecIrisL, &RecIrisR,
                                               RecLrFlag, &LeftMatchOutputParam, &RightMatchOutputParam);
        EXPECT_EQ(E_ALGAPI_OK, FunResult);
        if(E_ALGAPI_OK != FunResult)
        {
            std::cout << "RightEye iden failed, count = " << count << "\n" << std::endl;
        }
    }

    //ʶ��˫��
    FunResult = PTestAlgApi->GetParam(&ParamStruct);
    EXPECT_EQ(E_ALGAPI_OK, FunResult);

    ParamStruct.EyeMode = BothEye;
    FunResult = PTestAlgApi->SetParam(&ParamStruct);
    EXPECT_EQ(E_ALGAPI_OK, FunResult);

    //left_old.enr�ļ������ע�������Ǵ�windowsƽ̨ע��õ���ע������
    FunResult = FeatrueLoad("/home/fpc/MyIKEMB/algapiubuntu/left_old.enr", CodeListL, IR_ENR_FEATURE_LENGTH);
    EXPECT_EQ(E_ALGAPI_OK, FunResult);
    //right_old.enr�ļ������ע�������Ǵ�windowsƽ̨ע��õ���ע������
    FunResult = FeatrueLoad("/home/fpc/MyIKEMB/algapiubuntu/right_old.enr", CodeListR, IR_ENR_FEATURE_LENGTH);
    EXPECT_EQ(E_ALGAPI_OK, FunResult);

    for(int count = 0; count < g_constRecRepeatNum; count++)
    {
        FunResult = PTestAlgApi->SyncStartIden(CodeListL, 1, CodeListR, 1, &RecIrisL, &RecIrisR,
                                               RecLrFlag, &LeftMatchOutputParam, &RightMatchOutputParam);
        EXPECT_EQ(E_ALGAPI_OK, FunResult);
        if(E_ALGAPI_OK != FunResult)
        {
            std::cout << "RightEye iden failed, count = " << count << "\n" << std::endl;
        }
    }

    //ʶ��������
    FunResult = PTestAlgApi->GetParam(&ParamStruct);
    EXPECT_EQ(E_ALGAPI_OK, FunResult);

    ParamStruct.EyeMode = AnyEye;
    FunResult = PTestAlgApi->SetParam(&ParamStruct);
    EXPECT_EQ(E_ALGAPI_OK, FunResult);

    //left_old.enr�ļ������ע�������Ǵ�windowsƽ̨ע��õ���ע������
    FunResult = FeatrueLoad("/home/fpc/MyIKEMB/algapiubuntu/left_old.enr", CodeListL, IR_ENR_FEATURE_LENGTH);
    EXPECT_EQ(E_ALGAPI_OK, FunResult);
    //right_old.enr�ļ������ע�������Ǵ�windowsƽ̨ע��õ���ע������
    FunResult = FeatrueLoad("/home/fpc/MyIKEMB/algapiubuntu/right_old.enr", CodeListR, IR_ENR_FEATURE_LENGTH);
    EXPECT_EQ(E_ALGAPI_OK, FunResult);

    for(int count = 0; count < g_constRecRepeatNum; count++)
    {
        FunResult = PTestAlgApi->SyncStartIden(CodeListL, 1, CodeListR, 1, &RecIrisL, &RecIrisR,
                                               RecLrFlag, &LeftMatchOutputParam, &RightMatchOutputParam);
        EXPECT_EQ(E_ALGAPI_OK, FunResult);
        if(E_ALGAPI_OK != FunResult)
        {
            std::cout << "RightEye iden failed, count = " << count << "\n" << std::endl;
        }
    }
}


/*****************************************************************************
*                         AlgApi��Ԫ����-�첽��Ĥʶ��
*  �� �� ����
*  ��    �ܣ��첽��Ĥʶ��
*  ˵    ��������ͬ��ģʽ���Ѿ�������֤�����ֻ��˫��ģʽ�½���ʶ�����
*  ��    ����
*  �� �� ֵ��NULL
*  �� �� �ˣ�lizhl
*  ����ʱ�䣺2013-12-19
*  �� �� �ˣ�
*  �޸�ʱ�䣺
*****************************************************************************/
TEST_F(CUnitTestAlgApi, TestAsyncIden)
{
    //��ʼ��
    FunResult = PTestAlgApi->Init();
    EXPECT_EQ(E_ALGAPI_OK, FunResult);

    //ʶ��
    //˫��ʶ��
    //�����۾�ģʽ
    FunResult = PTestAlgApi->GetParam(&ParamStruct);
    EXPECT_EQ(E_ALGAPI_OK, FunResult);

    ParamStruct.EyeMode = BothEye;
    FunResult = PTestAlgApi->SetParam(&ParamStruct);
    EXPECT_EQ(E_ALGAPI_OK, FunResult);

    //left_old.enr�ļ������ע�������Ǵ�windowsƽ̨ע��õ���ע������
    FunResult = FeatrueLoad("/home/fpc/MyIKEMB/algapiubuntu/left_old.enr", CodeListL, IR_ENR_FEATURE_LENGTH);
    EXPECT_EQ(E_ALGAPI_OK, FunResult);
    //right_old.enr�ļ������ע�������Ǵ�windowsƽ̨ע��õ���ע������
    FunResult = FeatrueLoad("/home/fpc/MyIKEMB/algapiubuntu/right_old.enr", CodeListR, IR_ENR_FEATURE_LENGTH);
    EXPECT_EQ(E_ALGAPI_OK, FunResult);

    std::cout << "AsyncIden's callback function should be called " << g_constAsyncRecRepeatNum << " times" << std::endl;
    for(int count = 0; count < g_constAsyncRecRepeatNum; count++)
    {
        FunResult = PTestAlgApi->AsyncStartIden(CBAsyncIden, CodeListL, 1, CodeListR, 1);
        EXPECT_EQ(E_ALGAPI_OK, FunResult);
        if(E_ALGAPI_OK != FunResult)
        {
            std::cout << "AsyncIden failed" << std::endl;
        }

        //�ȴ��첽��ͼ�ص����������ú��ٽ�����һ���첽��ͼ
        while(!s_flagAsyncRec)
        {
//            if(!onceFlag)
//            {
//                onceFlag = true;
//                FunResult = PTestAlgApi->AsyncStartEnroll(CBAsyncEnroll, EnrIrisL, NumEnrL, EnrIrisR, NumEnrR, EnrLrFlag);
//                EXPECT_EQ(E_ALGAPI_ALGORITHM_CONFLICT, FunResult);
//            }
            sleep(1);
        }
        s_flagAsyncRec = false;
    }
}

/*****************************************************************************
*                         AlgApi��Ԫ����-�첽ʶ��ص�����
*  �� �� ����
*  ��    �ܣ��첽ʶ��ص�����
*  ˵    �����ڻص�������ȡ�첽ʶ����
*  ��    ����
*  �� �� ֵ��NULL
*  �� �� �ˣ�lizhl
*  ����ʱ�䣺2013-12-19
*  �� �� �ˣ�
*  �޸�ʱ�䣺
*****************************************************************************/
int CBAsyncIden(int funResult, APIIrisInfo* recIrisL, APIIrisInfo* recIrisR, LRSucFailFlag lrFlag, APIMatchOutput* pLeftMatchOutputParam, APIMatchOutput* pRightMatchOutputParam)
{
    EXPECT_EQ(E_ALGAPI_OK, funResult);
    EXPECT_EQ(EnrRecBothSuccess, lrFlag);
    std::cout << "AsyncIden's callback function is called" << std::endl;
    s_flagAsyncRec = true;
}
