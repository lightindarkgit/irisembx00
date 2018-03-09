/******************************************************************************************
** 文件名:   VirtualReadImg.cpp
×× 主要类:
**
** Copyright (c) 中科虹霸有限公司
** 创建人:   lizhl
** 日  期:   2013-12-12
** 修改人:
** 日  期:
** 描  述:   IKEMBX00项目算法逻辑API测试
 *×× 主要模块说明: VirtualReadImg.h
**
** 版  本:   1.0.0
** 备  注:
**
*****************************************************************************************/
#include "VirtualReadImg.h"


/*****************************************************************************
*                         从文件中读取图像函数
*  函 数 名：TestAlgApiReadImg
*  功    能：从文件中读取图像
*  说    明：
*  参    数：pBigImg：大图图像指针
*           pLeftIris：左眼虹膜图像
*           pRightIris：右眼虹膜图像
*  返 回 值：0：成功
*           -1：失败
*  创 建 人：lizhl
*  创建时间：2013-12-12
*  修 改 人：
*  修改时间：
*****************************************************************************/
int TestAlgApiReadImg(unsigned char* pBigImg, pIrisImageInfo pLeftIris, pIrisImageInfo pRightIris)
{
    FILE *leftImgStream;
    FILE *rightImgStream;
    FILE *bigImgStream;

    char fileName[g_constLen];
    memset(fileName,0,g_constLen);
    unsigned char imgRead[g_constIKImgSize];

    static int imgNum = 1;
//    int index = imgNum%(g_constTotalReadNum+1);

    if(imgNum > 9)
    {
        imgNum = 1;
    }
    int index = imgNum;
    int readCount;

    //读取左眼图像文件
    sprintf(fileName,"/home/fpc/MyIKEMB/algapiubuntu/left_%1d.ppm", index);
//    sprintf(fileName,"/home/fpc/MyIKEMB/algapiubuntu/caileft/tmp/%06d.ppm", index);
    leftImgStream = fopen(fileName,"rb");
    if(NULL == leftImgStream)
    {
        return -1;
    }

    fseek(leftImgStream, g_constIrisImgHeaderLen,SEEK_SET);
    readCount = fread(imgRead, sizeof(unsigned char), g_constIKImgSize, leftImgStream);

    //查看读到的数据
    unsigned char viewMem[20];
    memcpy(viewMem, imgRead, 20);

//    memcpy(pLeftIris->imageData, imgRead, g_constIKImgSize);
    ImageMirror(pLeftIris->imageData, imgRead, g_constIKImgHeight,g_constIKImgWidth,false);
    fclose(leftImgStream);
    if(readCount != g_constIKImgSize)
    {
        return -1;
    }



    //读取右眼图像文件
    sprintf(fileName,"/home/fpc/MyIKEMB/algapiubuntu/right_%1d.ppm", index);
    rightImgStream = fopen(fileName,"rb");
    if(NULL == rightImgStream)
    {
        return -1;
    }

    fseek(rightImgStream, g_constIrisImgHeaderLen,SEEK_SET);
    readCount = fread(imgRead, sizeof(unsigned char), g_constIKImgSize, rightImgStream);
//    memcpy(pRightIris->imageData, imgRead, g_constIKImgSize);
    ImageMirror(pRightIris->imageData, imgRead, g_constIKImgHeight,g_constIKImgWidth,false);
    fclose(rightImgStream);
    if(readCount != g_constIKImgSize)
    {
        return -1;
    }

    //读取大图图像文件
    sprintf(fileName,"/home/fpc/MyIKEMB/algapiubuntu/big_%1d.ppm", index);
    bigImgStream = fopen(fileName,"rb");
    if(NULL == bigImgStream)
    {
        return -1;
    }

    fseek(bigImgStream, g_constBigImgHeaderLen,SEEK_SET);
    readCount = fread(pBigImg, sizeof(unsigned char), g_constIrisImgSize, bigImgStream);
    fclose(bigImgStream);
    if(readCount != g_constIrisImgSize)
    {
        return -1;
    }

    imgNum++;
//    usleep(90000);
    return 0;
}


