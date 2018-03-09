/******************************************************************************************
** 文件名:   UnitTestAlgApi.cpp
×× 主要类:
**
** Copyright (c) 中科虹霸有限公司
** 创建人:   lizhl
** 日  期:   2013-12-13
** 修改人:
** 日  期:
** 描  述:   IKEMBX00项目算法逻辑API测试
 *×× 主要模块说明: VirtualReadImg.h
**
** 版  本:   1.0.0
** 备  注:
**
*****************************************************************************************/
#include "UnitTestAlgApi.h"

/*****************************************************************************
*                         AlgApi单元测试-初始化后调用同步采图函数
*  函 数 名：
*  功    能：AlgApi初始化后调用同步采图函数
*  说    明：由于读图的顺序关系，本函数应该放在测试的最前面，否则本函数的测试有可能会失败
*  参    数：
*  返 回 值：NULL
*  创 建 人：lizhl
*  创建时间：2013-12-13
*  修 改 人：
*  修改时间：
*****************************************************************************/
TEST_F(CUnitTestAlgApi, TestAlgApiSyncCapImg)
{
    //初始化
    FunResult = PTestAlgApi->Init();
    EXPECT_EQ(E_ALGAPI_OK, FunResult);

    //由于采图线程是每三幅图像通知一次flag，因此前两次都是系统初始值，第三次才计算
    //每6副图像，前三次采图的距离值为合适
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

    //每6副图像，后三次采图的距离值为过远
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

    //每6副图像，前三次采图的距离值为合适
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

    //重复采图测试
    for(int count = 0; count < g_constCapRepeatNum; count++)
    {
        FunResult = PTestAlgApi->SyncCapIrisImg(ImgBuffer, ImgClearFlag, ImgPositionFlag);
        EXPECT_EQ(E_ALGAPI_OK, FunResult);
    }
}

/*****************************************************************************
*                         AlgApi单元测试-初始化后调用异步采图函数
*  函 数 名：
*  功    能：AlgApi初始化后调用异步采图函数
*  说    明：调用异步采图函数，然后在回调函数中取结果
*  参    数：
*  返 回 值：NULL
*  创 建 人：lizhl
*  创建时间：2013-12-19
*  修 改 人：
*  修改时间：
*****************************************************************************/
TEST_F(CUnitTestAlgApi, TestAlgApiAsyncCapImg)
{
    //初始化
    FunResult = PTestAlgApi->Init();
    EXPECT_EQ(E_ALGAPI_OK, FunResult);

    //调用异步采图函数
    std::cout << "AsyncCapImg's callback function should be called " << g_constAsyncCapRepeatNum << " times" << std::endl;
    for(int count = 0; count < g_constAsyncCapRepeatNum; count++)
    {
        FunResult = PTestAlgApi->AsyncCapIrisImg(CBAsyncCapIrisImg);
        EXPECT_EQ(E_ALGAPI_OK, FunResult);
        if(E_ALGAPI_OK != FunResult)
        {
            std::cout << "AsyncCapImg failed" << std::endl;
        }
        //等待异步采图回调函数被调用后再进行下一次异步采图
        while(!s_flagAsyncCapImg)
        {
            sleep(1);
        }
        s_flagAsyncCapImg = false;
    }
}

/*****************************************************************************
*                         AlgApi单元测试-异步采图回调函数
*  函 数 名：
*  功    能：异步采图回调函数
*  说    明：在回调函数中取异步采图结果
*  参    数：
*  返 回 值：NULL
*  创 建 人：lizhl
*  创建时间：2013-12-19
*  修 改 人：
*  修改时间：
*****************************************************************************/
int CBAsyncCapIrisImg(int funResult, unsigned char* pIrisImg, LRIrisClearFlag lrIrisClearFlag, IrisPositionFlag lrIrisPositionFlag)
{
    EXPECT_EQ(E_ALGAPI_OK, funResult);
    std::cout << "AsyncCapImg's callback function is called" << std::endl;
    s_flagAsyncCapImg = true;
}

/*****************************************************************************
*                         AlgApi单元测试-系统设置
*  函 数 名：
*  功    能：AlgApi系统设置
*  说    明：主要是测试初始化、休眠和唤醒
*  参    数：
*  返 回 值：NULL
*  创 建 人：lizhl
*  创建时间：2013-12-13
*  修 改 人：
*  修改时间：
*****************************************************************************/
TEST_F(CUnitTestAlgApi, TestSystemSet)
{
    //初始化
    FunResult = PTestAlgApi->Init();
    EXPECT_EQ(E_ALGAPI_OK, FunResult);

    //初始化后采集图像
    FunResult = PTestAlgApi->SyncCapIrisImg(ImgBuffer, ImgClearFlag, ImgPositionFlag);
    EXPECT_EQ(E_ALGAPI_OK, FunResult);

    //未休眠调用唤醒函数
    FunResult = PTestAlgApi->Wake();
    EXPECT_EQ(E_ALGAPI_WAKE_FAILED, FunResult);

    //休眠
    FunResult = PTestAlgApi->Sleep();
    EXPECT_EQ(E_ALGAPI_OK, FunResult);

    //休眠后调用各功能函数，此时各功能函数应该返回系统处于休眠状态的错误
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

    //休眠后唤醒系统，然后调用各功能函数
    FunResult = PTestAlgApi->Wake();
    EXPECT_EQ(E_ALGAPI_OK, FunResult);

    FunResult = PTestAlgApi->SyncCapIrisImg(ImgBuffer, ImgClearFlag, ImgPositionFlag);
    EXPECT_EQ(E_ALGAPI_OK, FunResult);
}

/*****************************************************************************
*                         AlgApi单元测试-算法逻辑控制参数设置
*  函 数 名：
*  功    能：AlgApi算法逻辑控制参数设置
*  说    明：测试算法逻辑控制参数的正确、错误设置
*  参    数：
*  返 回 值：NULL
*  创 建 人：lizhl
*  创建时间：2013-12-18
*  修 改 人：
*  修改时间：
*****************************************************************************/
TEST_F(CUnitTestAlgApi, TestAlgApiControlParamSet)
{
    //初始化
    FunResult = PTestAlgApi->Init();
    EXPECT_EQ(E_ALGAPI_OK, FunResult);

    //错误参数设置
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

    //正确参数设置
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
*                         AlgApi单元测试-未初始化即调用各接口函数
*  函 数 名：
*  功    能：AlgApi未经过初始化即调用各接口函数
*  说    明：
*  参    数：
*  返 回 值：NULL
*  创 建 人：lizhl
*  创建时间：2013-12-13
*  修 改 人：
*  修改时间：
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
*                         AlgApi单元测试-同步虹膜注册
*  函 数 名：
*  功    能：同步虹膜注册
*  说    明：在不同眼睛模式下进行注册测试，包括双眼、左眼、右眼、任意眼，注册后进行比对
*  参    数：
*  返 回 值：NULL
*  创 建 人：lizhl
*  创建时间：2013-12-18
*  修 改 人：
*  修改时间：
*****************************************************************************/
TEST_F(CUnitTestAlgApi, TestSyncEnroll)
{
    //初始化
    FunResult = PTestAlgApi->Init();
    EXPECT_EQ(E_ALGAPI_OK, FunResult);

    //注册得到的图像是上下翻转的，因此在保存时需要翻转一下，开辟一个buffer来存储翻转图像
    unsigned char imgFlip[g_constIKImgSize];

    //注册
    //双眼注册
    //设置眼睛模式
    FunResult = PTestAlgApi->GetParam(&ParamStruct);
    EXPECT_EQ(E_ALGAPI_OK, FunResult);

    ParamStruct.EyeMode = BothEye;
    FunResult = PTestAlgApi->SetParam(&ParamStruct);
    EXPECT_EQ(E_ALGAPI_OK, FunResult);

    FunResult = PTestAlgApi->SyncCapIrisImg(ImgBuffer, ImgClearFlag, ImgPositionFlag);
    EXPECT_EQ(E_ALGAPI_OK, FunResult);

    //开始注册
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

        //注册后比对
        //先载入一个右眼注册特征，由于来自不同眼睛，因此比对失败
        FunResult = FeatrueLoad("/home/fpc/MyIKEMB/algapiubuntu/right_old.enr", CodeListL, IR_ENR_FEATURE_LENGTH);
        EXPECT_EQ(E_ALGAPI_OK, FunResult);
        FunResult = PTestAlgApi->Match(EnrIrisL[0].IrisRecTemplate, 1, CodeListL, 1, &MatchOutputParam);
        EXPECT_EQ(E_ALGAPI_MATCH_FAILED, FunResult);

        //重新载入一个左眼注册特征，由于来自同一个人的左眼，因此比对成功
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

    //左眼注册
    //设置眼睛模式
    FunResult = PTestAlgApi->GetParam(&ParamStruct);
    EXPECT_EQ(E_ALGAPI_OK, FunResult);

    ParamStruct.EyeMode = LeftEye;
    FunResult = PTestAlgApi->SetParam(&ParamStruct);
    EXPECT_EQ(E_ALGAPI_OK, FunResult);

    //开始注册
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


    //右眼注册
    //设置眼睛模式
    FunResult = PTestAlgApi->GetParam(&ParamStruct);
    EXPECT_EQ(E_ALGAPI_OK, FunResult);

    ParamStruct.EyeMode = RightEye;
    FunResult = PTestAlgApi->SetParam(&ParamStruct);
    EXPECT_EQ(E_ALGAPI_OK, FunResult);

    //开始注册
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


    //任意眼注册
    //设置眼睛模式
    FunResult = PTestAlgApi->GetParam(&ParamStruct);
    EXPECT_EQ(E_ALGAPI_OK, FunResult);

    ParamStruct.EyeMode = AnyEye;
    FunResult = PTestAlgApi->SetParam(&ParamStruct);
    EXPECT_EQ(E_ALGAPI_OK, FunResult);
    //开始注册
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
*                         AlgApi单元测试-异步虹膜注册
*  函 数 名：
*  功    能：异步虹膜注册
*  说    明：由于同步模式下已经做过验证，因此只对双眼模式下进行注册测试
*  参    数：
*  返 回 值：NULL
*  创 建 人：lizhl
*  创建时间：2013-12-19
*  修 改 人：
*  修改时间：
*****************************************************************************/
TEST_F(CUnitTestAlgApi, TestAsyncEnroll)
{
    //初始化
    FunResult = PTestAlgApi->Init();
    EXPECT_EQ(E_ALGAPI_OK, FunResult);

    //注册
    //双眼注册
    //设置眼睛模式
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

        //等待异步采图回调函数被调用后再进行下一次异步采图
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
*                         AlgApi单元测试-异步注册回调函数
*  函 数 名：
*  功    能：异步注册回调函数
*  说    明：在回调函数中取异步注册结果
*  参    数：
*  返 回 值：NULL
*  创 建 人：lizhl
*  创建时间：2013-12-19
*  修 改 人：
*  修改时间：
*****************************************************************************/
int CBAsyncEnroll(int funResult, APIIrisInfo* enrIrisL, int numEnrL, APIIrisInfo* enrIrisR, int numEnrR, LRSucFailFlag lrFlag)
{
    EXPECT_EQ(E_ALGAPI_OK, funResult);
    EXPECT_EQ(EnrRecBothSuccess, lrFlag);
    std::cout << "AsyncEnroll's callback function is called" << std::endl;

    static bool tmpFlag = true;
    //注册得到的图像是上下翻转的，因此在保存时需要翻转一下，开辟一个buffer来存储翻转图像
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
*                         AlgApi单元测试-同步虹膜识别
*  函 数 名：
*  功    能：同步虹膜识别
*  说    明：在不同眼睛模式下进行识别测试，包括左眼、右眼、双眼，注册后进行比对
*  参    数：
*  返 回 值：NULL
*  创 建 人：lizhl
*  创建时间：2013-12-18
*  修 改 人：
*  修改时间：
*****************************************************************************/
TEST_F(CUnitTestAlgApi, TestSyncIden)
{
    //初始化
    FunResult = PTestAlgApi->Init();
    EXPECT_EQ(E_ALGAPI_OK, FunResult);

    //识别
    //识别左眼
    FunResult = PTestAlgApi->GetParam(&ParamStruct);
    EXPECT_EQ(E_ALGAPI_OK, FunResult);

    ParamStruct.EyeMode = LeftEye;
    FunResult = PTestAlgApi->SetParam(&ParamStruct);
    EXPECT_EQ(E_ALGAPI_OK, FunResult);

    //left_old.enr文件保存的注册特征是从windows平台注册得到的注册特征
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

    //识别右眼
    FunResult = PTestAlgApi->GetParam(&ParamStruct);
    EXPECT_EQ(E_ALGAPI_OK, FunResult);

    ParamStruct.EyeMode = RightEye;
    FunResult = PTestAlgApi->SetParam(&ParamStruct);
    EXPECT_EQ(E_ALGAPI_OK, FunResult);

    //right_old.enr文件保存的注册特征是从windows平台注册得到的注册特征
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

    //识别双眼
    FunResult = PTestAlgApi->GetParam(&ParamStruct);
    EXPECT_EQ(E_ALGAPI_OK, FunResult);

    ParamStruct.EyeMode = BothEye;
    FunResult = PTestAlgApi->SetParam(&ParamStruct);
    EXPECT_EQ(E_ALGAPI_OK, FunResult);

    //left_old.enr文件保存的注册特征是从windows平台注册得到的注册特征
    FunResult = FeatrueLoad("/home/fpc/MyIKEMB/algapiubuntu/left_old.enr", CodeListL, IR_ENR_FEATURE_LENGTH);
    EXPECT_EQ(E_ALGAPI_OK, FunResult);
    //right_old.enr文件保存的注册特征是从windows平台注册得到的注册特征
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

    //识别任意眼
    FunResult = PTestAlgApi->GetParam(&ParamStruct);
    EXPECT_EQ(E_ALGAPI_OK, FunResult);

    ParamStruct.EyeMode = AnyEye;
    FunResult = PTestAlgApi->SetParam(&ParamStruct);
    EXPECT_EQ(E_ALGAPI_OK, FunResult);

    //left_old.enr文件保存的注册特征是从windows平台注册得到的注册特征
    FunResult = FeatrueLoad("/home/fpc/MyIKEMB/algapiubuntu/left_old.enr", CodeListL, IR_ENR_FEATURE_LENGTH);
    EXPECT_EQ(E_ALGAPI_OK, FunResult);
    //right_old.enr文件保存的注册特征是从windows平台注册得到的注册特征
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
*                         AlgApi单元测试-异步虹膜识别
*  函 数 名：
*  功    能：异步虹膜识别
*  说    明：由于同步模式下已经做过验证，因此只对双眼模式下进行识别测试
*  参    数：
*  返 回 值：NULL
*  创 建 人：lizhl
*  创建时间：2013-12-19
*  修 改 人：
*  修改时间：
*****************************************************************************/
TEST_F(CUnitTestAlgApi, TestAsyncIden)
{
    //初始化
    FunResult = PTestAlgApi->Init();
    EXPECT_EQ(E_ALGAPI_OK, FunResult);

    //识别
    //双眼识别
    //设置眼睛模式
    FunResult = PTestAlgApi->GetParam(&ParamStruct);
    EXPECT_EQ(E_ALGAPI_OK, FunResult);

    ParamStruct.EyeMode = BothEye;
    FunResult = PTestAlgApi->SetParam(&ParamStruct);
    EXPECT_EQ(E_ALGAPI_OK, FunResult);

    //left_old.enr文件保存的注册特征是从windows平台注册得到的注册特征
    FunResult = FeatrueLoad("/home/fpc/MyIKEMB/algapiubuntu/left_old.enr", CodeListL, IR_ENR_FEATURE_LENGTH);
    EXPECT_EQ(E_ALGAPI_OK, FunResult);
    //right_old.enr文件保存的注册特征是从windows平台注册得到的注册特征
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

        //等待异步采图回调函数被调用后再进行下一次异步采图
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
*                         AlgApi单元测试-异步识别回调函数
*  函 数 名：
*  功    能：异步识别回调函数
*  说    明：在回调函数中取异步识别结果
*  参    数：
*  返 回 值：NULL
*  创 建 人：lizhl
*  创建时间：2013-12-19
*  修 改 人：
*  修改时间：
*****************************************************************************/
int CBAsyncIden(int funResult, APIIrisInfo* recIrisL, APIIrisInfo* recIrisR, LRSucFailFlag lrFlag, APIMatchOutput* pLeftMatchOutputParam, APIMatchOutput* pRightMatchOutputParam)
{
    EXPECT_EQ(E_ALGAPI_OK, funResult);
    EXPECT_EQ(EnrRecBothSuccess, lrFlag);
    std::cout << "AsyncIden's callback function is called" << std::endl;
    s_flagAsyncRec = true;
}
