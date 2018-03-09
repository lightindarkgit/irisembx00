/******************************************************************************************
** �ļ���:   VirtualReadImg.cpp
���� ��Ҫ��:
**
** Copyright (c) �пƺ�����޹�˾
** ������:   lizhl
** ��  ��:   2013-12-12
** �޸���:
** ��  ��:
** ��  ��:   IKEMBX00��Ŀ�㷨�߼�API����
 *���� ��Ҫģ��˵��: VirtualReadImg.h
**
** ��  ��:   1.0.0
** ��  ע:
**
*****************************************************************************************/
#include "VirtualReadImg.h"


/*****************************************************************************
*                         ���ļ��ж�ȡͼ����
*  �� �� ����TestAlgApiReadImg
*  ��    �ܣ����ļ��ж�ȡͼ��
*  ˵    ����
*  ��    ����pBigImg����ͼͼ��ָ��
*           pLeftIris�����ۺ�Ĥͼ��
*           pRightIris�����ۺ�Ĥͼ��
*  �� �� ֵ��0���ɹ�
*           -1��ʧ��
*  �� �� �ˣ�lizhl
*  ����ʱ�䣺2013-12-12
*  �� �� �ˣ�
*  �޸�ʱ�䣺
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

    //��ȡ����ͼ���ļ�
    sprintf(fileName,"/home/fpc/MyIKEMB/algapiubuntu/left_%1d.ppm", index);
//    sprintf(fileName,"/home/fpc/MyIKEMB/algapiubuntu/caileft/tmp/%06d.ppm", index);
    leftImgStream = fopen(fileName,"rb");
    if(NULL == leftImgStream)
    {
        return -1;
    }

    fseek(leftImgStream, g_constIrisImgHeaderLen,SEEK_SET);
    readCount = fread(imgRead, sizeof(unsigned char), g_constIKImgSize, leftImgStream);

    //�鿴����������
    unsigned char viewMem[20];
    memcpy(viewMem, imgRead, 20);

//    memcpy(pLeftIris->imageData, imgRead, g_constIKImgSize);
    ImageMirror(pLeftIris->imageData, imgRead, g_constIKImgHeight,g_constIKImgWidth,false);
    fclose(leftImgStream);
    if(readCount != g_constIKImgSize)
    {
        return -1;
    }



    //��ȡ����ͼ���ļ�
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

    //��ȡ��ͼͼ���ļ�
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


