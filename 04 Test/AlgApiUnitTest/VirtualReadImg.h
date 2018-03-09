/******************************************************************************************
** ļ:   VirtualReadImg.h
 Ҫ:
**
** Copyright (c) пƺ޹˾
** :   lizhl
**   :   2013-12-11
** ޸:
**   :
**   :   Ϊ˲AlgApiһļжȡļĺ
 * Ҫģ˵:
**
**   :   1.0.0
**   ע:
**
*****************************************************************************************/
#pragma once
#include "IK_IR_DLL.h"
#include "AlgApi.h"
//#include <string.h>
#include "BRCCommon.h"
#include <thread>
#include <string>

using namespace std;

const int g_constTotalReadNum = 3;
const int g_constLen = 260;

//ppmIrisImgHeader = "P5\n640 480\n255\n";
const int g_constIrisImgHeaderLen = 15;
//ppmBigImgHeader = "P5\n2048 704\n255\n";
const int g_constBigImgHeaderLen = 16;

//ԶĴļͼ
int TestAlgApiReadImg(unsigned char* pBigImg, pIrisImageInfo pLeftIris, pIrisImageInfo pRightIris);
