/*****************************************************************************
** 文 件 名：serialbase.h
** 主 要 类：SerialBase
**
** Copyright (c) 中科虹霸有限公司
**
** 创 建 人：王磊
** 创建时间：20013-10-18
**
** 修 改 人：
** 修改时间：
** 描  述:   串口基类定义
** 主要模块说明: 串口基类定义
**
** 版  本:   1.0.0
** 备  注:
**
*****************************************************************************/
#pragma once
#include <string>
#include <pthread.h>

#include "basictypes.h"
#include "innercommon.h"

//将字符串转为小写
std::string ToLowerStr(IN const std::string &str);

//串口基类
class SerialBase
{
public:
    SerialBase();
    virtual ~SerialBase();

    ////初始化串口
    //int Init();

    //打开串口
    virtual int OpenSerial(IN const char* port) = 0;

    //关闭串口
    void CloseSerial();

    //获取串口状态：打开或关闭
    bool IsOpen();

    //获取串口配置
    int GetConfig(OUT SerialConfig& serialConfig);
    //设置串口配置
    int SetConfig(IN const SerialConfig& serialConfig);

    //获取串口描述符
    int GetSerialFd();

    //发送数据到串口
    int Send(IN const unsigned char* pBuf, IN const int sizeToSend);
    //接收串口数据，定义为虚函数以便于子类实现为同步或异步接收
    int Recv(OUT unsigned char* pBuf, IN const int sizeToRecv, IN const int timeoutMSecs = zeroMSecond);


protected:
    //打开串口
    int Open(IN const char* port);
    //设置串口名称
    int SetSerialName(IN const char* serialName);


protected:
    int _fd;                    //串口设备描述符
    std::string _serialName;    //串口名
    SerialConfig _serialConfig;     //串口配置参数

    unsigned char _recvBuf[recvBufSize];    //接收缓冲区
    pthread_mutex_t _recvBufMutex;            //串口接收缓冲区线程锁
    pthread_cond_t _recvBufNoneEmpty;         //接收缓冲区非空条件互斥量
};
