/*****************************************************************************
** 文 件 名：asyncserialbase.h
** 主 要 类：AsyncSerialBase
**
** Copyright (c) 中科虹霸有限公司
**
** 创 建 人：王磊
** 创建时间：20013-11-08
**
** 修 改 人：
** 修改时间：
** 描  述:   异步串口基类定义
** 主要模块说明: 异步串口基类
**
** 版  本:   1.0.0
** 备  注:
**
*****************************************************************************/
#pragma once
#include <string>
#include <pthread.h>
#include <semaphore.h>

#include "basictypes.h"
#include "innercommon.h"
#include "serialbase.h"

//帧提取器类前置声明
class SerialFrame;


//异步串口基类
class AsyncSerialBase: public SerialBase
{
    friend class FrameExtractor;
public:
    AsyncSerialBase();
    ~AsyncSerialBase();

    //初始化串口
    //int Init(IN const FrameInfo& frameInfo, IN RecvCallback callback);
    int Init(IN RecvCallback callback);

    //打开串口
    int OpenSerial(IN const char *port);

    //启动接收串口数据
    int StartRecv();
    //停止接收串口数据
    int StopRecv();

    //封装帧
    int FormFrame(OUT unsigned char* pBuf, IN const unsigned char* pRawData, IN const int dataSize);

private:
    //接收数据线程
    static void *RecvThread(IN void* arg);
    //接收串口数据
    int RecvData(OUT unsigned char* pBuf, IN const int fd, IN const int sizeToRecv, IN const int timeoutMSecs = oneMSecond);


private:
    volatile bool _isRunning;                 //串口工作状态
    unsigned char _frameBuf[frameBufSize];    //帧缓冲区

    sem_t _threadSem;                   //线程工作信号量

    SerialFrame* _serialFrame;          //数据帧析取器
    RecvCallback _callback;             //帧接收回调函数
};
