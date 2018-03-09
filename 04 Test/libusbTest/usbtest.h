/******************************************************************************************
** 文件名:   USBTEST.H
×× 主要类:
**
** Copyright (c) 中科虹霸有限公司
** 创建人:   liuzhch
** 日  期:   2013-12-23
** 修改人:
** 日  期:
** 描  述:   IKEMBX00项目USBAPI测试
 *×× 主要模块说明:
**
** 版  本:   1.0.0
** 备  注:
**
*****************************************************************************************/
#ifndef __USBTEST_H__
#define __USBTEST_H__
#include "syncubsapi.h"
#include "asyncubsapi.h"
#include "gtest/gtest.h"

#include "iostream"
#include "../IKEmbX00/03 Code/USBAPI/dealwithusbtransferbuf.h"
#include "../IKEmbX00/03 Code/USBAPI/ikusbapibase.h"
#include "../IKEmbX00/03 Code/USBAPI/imagemanger.h"
#include "../IKEmbX00/03 Code/USBAPI/iusbimage.h"

/*****************************************************************************
*                         USBAPI单元测试
*  函 数 名：
*  功    能：
*  说    明：
*  参    数：
*  返 回 值：NULL
*  创 建 人：liuzhch
*  创建时间：2013-12-23
*  修 改 人：
*  修改时间：
*****************************************************************************/
class CUSBAPITest
{
public:
    CUSBAPITest();
    SyncUSBAPI  t_syncUSBAPI;
    AsyncUSBAPI t_asyncUSBAPI;
    DealWithUSBTransferBuf t_dealWith;
    IkUSBAPI t_usbAPI;
    ImageManger t_imageManger;
    IkUSBDevice t_usbDevice;
};



#endif // USBTEST_H
