/*****************************************************************************
** 文 件 名：syncserialbase.h
** 主 要 类：SyncSerialBase
**
** Copyright (c) 中科虹霸有限公司
**
** 创 建 人：王磊
** 创建时间：20013-10-18
**
** 修 改 人：
** 修改时间：
** 描  述:   同步串口基类定义
** 主要模块说明: 同步串口基类
**
** 版  本:   1.0.0
** 备  注:
**
*****************************************************************************/
#pragma once
#include <string>
#include <pthread.h>

#include "basictypes.h"
#include "serialbase.h"


//帧提取器类声明
class SerialFrame;


//同步串口基类
class SyncSerialBase : public SerialBase
{
    friend class FrameExtractor;
public:
    SyncSerialBase();
    ~SyncSerialBase();

//    //初始化串口
//    int Init(IN const FrameInfo& frameInfo);

    //打开串口
    int OpenSerial(IN const char *port);

    //获取帧
    int GetFrame(OUT unsigned char* pBuf, IN const int bufSize);
    //封装帧
    int FormFrame(OUT unsigned char* pBuf, IN const unsigned char* pRawData, IN const int dataSize);

private:
    unsigned char _frameBuf[recvBufSize];      //帧缓冲区
    SerialFrame* _serialFrame;      //数据帧析取器
};
