/*****************************************************************************
** 文 件 名：asyncserialbase.cpp
** 主 要 类：AsyncSerialBase
**
** Copyright (c) 中科虹霸有限公司
**
** 创 建 人：王磊
** 创建时间：20013-10-18
**
** 修 改 人：
** 修改时间：
** 描  述:   异步串口基类实现
** 主要模块说明: 异步串口基类
**
** 版  本:   1.0.0
** 备  注:
**
*****************************************************************************/
#include <stdio.h>        //标准输入输出定义
#include <stdlib.h>       //标准函数库定义
#include <unistd.h>       //Unix标准函数定义
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>        //文件控制定义
#include <termios.h>      //POSIX中断控制定义
#include <errno.h>        //错误号定义
#include <string.h>
#include <pthread.h>

#include "innercommon.h"
#include "asyncserialbase.h"        //异步串口类基类定义头文件
#include "serialframe.h"         //帧处理头文件

//引入标准命名空间
using namespace std;


AsyncSerialBase::AsyncSerialBase():
    _serialFrame(NULL)
{
    _serialFrame = new SerialFrame;   //创建帧解析器

    sem_init(&_threadSem, 0, 0);      //初始化线程信号量, 仅供本进程内的线程共享该信号量
}

AsyncSerialBase::~AsyncSerialBase()
{
    StopRecv();
    CloseSerial();

    delete _serialFrame;          //销毁帧解析器
    sem_destroy(&_threadSem);     //销毁线程信号量
}

/*****************************************************************************
*                        串口类初始化
*  函 数 名：Init
*  功    能：初始化串口类的数据成员，以及帧缓冲区内存分配
*  说    明：
*  参    数：
*  返 回 值：
*           成功打开串口返回E_SERIAL_OK，否则返回对应错误码
*  创 建 人：王磊
*  创建时间：2013-11-08
*  修 改 人：
*  修改时间：
*****************************************************************************/
//int AsyncSerialBase::Init(IN const FrameInfo& frameInfo, IN RecvCallback callback)
int AsyncSerialBase::Init(IN RecvCallback callback)
{
//    //设置帧信息
//    if(E_SERIAL_OK != _frameExtractor->SetFrameInfo(frameInfo))
//    {
//        return E_SERIAL_ERROR;
//    }


//    //重新分配内存
//    _frameBuf = new unsigned char[frameInfo.FrameLen];   //申请帧缓冲区内存

//    //判断缓冲区分配结果
//    if(NULL == _frameBuf)
//    {
//        return E_SERIAL_ALLOC_MEMORY;
//    }

    //设置接收串口数据的回调函数
    if(NULL == callback)
    {
        //TO DO
        return E_SERIAL_INVALID_CALLBACK;
    }
    else
    {
        //TO DO
        _callback = callback;
    }

    return E_SERIAL_OK;
}


/*****************************************************************************
*                        打开串口
*  函 数 名：OpenPort
*  功    能：根据指定的参数打开串口
*  说    明：在成功打开串口之后，还会启动串口数据接收线程；
*  参    数：
*           port，串口端口号
*  返 回 值：
*           成功打开串口返回E_SERIAL_OK，否则返回对应错误码
*  创 建 人：王磊
*  创建时间：2013-10-18
*  修 改 人：
*  修改时间：
*****************************************************************************/
int AsyncSerialBase::OpenSerial(IN const char *port)
{
    int rtnVal;

    if(NULL == port)
    {
        return E_SERIAL_INVALID_PTR;
    }

    //打开串口
    rtnVal = Open(port);
    if(E_SERIAL_OK != rtnVal)
    {
        return rtnVal;
    }

    return E_SERIAL_OK;
}


/*****************************************************************************
*                        接收串口数据
*  函 数 名：RecvData
*  功    能：读串口线程，接收串口数据
*  说    明：
*  参    数：
*           fd, 串口文件描述符
*           pBuf， 接收缓冲区指针
*           sizeToRecv，期望接收的数据字节长度
*           timeOutSecs，超时时间长度
*  返 回 值：
*           从串口读取到的字节数
*  创 建 人：王磊
*  创建时间：2013-10-18
*  修 改 人：
*  修改时间：
*****************************************************************************/
int AsyncSerialBase::RecvData(OUT unsigned char *pBuf, IN int fd, IN const int sizeToRecv, IN const int timeoutMSecs)
{
//    return Recv(pBuf, sizeToRecv, timeoutMSecs);
    int nFds;
    fd_set readFds;
    struct timeval tv;

    //设置超时时间
    tv.tv_sec = timeoutMSecs;
    tv.tv_usec = 0;

    FD_ZERO(&readFds);
    FD_SET(fd, &readFds);

    nFds = select(fd + 1, &readFds, NULL, NULL, &tv);
    if(0 >= nFds)
    {
        if(0 == nFds)
        {
            errno = ETIME;
        }

        return E_SERIAL_ERROR;
    }

    return (read(fd, pBuf, sizeToRecv));
}

/*****************************************************************************
*                        接收串口数据线程
*  函 数 名：RecvThread
*  功    能：读串口线程，接收串口数据
*  说    明：
*  参    数：
*           arg， 串口实例对象指针
*  返 回 值：
*           void指针
*  创 建 人：王磊
*  创建时间：2013-10-18
*  修 改 人：
*  修改时间：
*****************************************************************************/
void* AsyncSerialBase::RecvThread(IN void *arg)
{
//    int rtnVal;
    AsyncSerialBase* asyncSerial = static_cast<AsyncSerialBase *>(arg);

    //如果回调函数为空
    if(NULL == asyncSerial->_callback)
    {
        return (void *)E_SERIAL_INVALID_CALLBACK;
    }

    //启动串口数据接收和帧析取
    int nRead;          //从串口读到的字节数
    unsigned char* pRecvBuf = asyncSerial->_recvBuf;
    while(asyncSerial->IsOpen() && asyncSerial->_isRunning)
    {
        pRecvBuf = asyncSerial->_recvBuf;  //复位串口数据缓冲区指针
        nRead = asyncSerial->Recv(asyncSerial->_recvBuf, recvBufSize, 500 * oneMSecond);

        //处理数据
        if(nRead > 0)
        {
            asyncSerial->_serialFrame->WriteData(asyncSerial->_recvBuf, nRead);
        }

        while(E_SERIAL_OK == asyncSerial->_serialFrame->GetFrame(asyncSerial->_frameBuf))
        {
            if(NULL != asyncSerial->_callback)
            {
                asyncSerial->_callback(asyncSerial->_frameBuf, FRAME_DATA_SIZE);    //回调用户的帧数据处理程序
            }
        }
    }

    sem_post(&asyncSerial->_threadSem);      //向结束操作(StopRecv)发线程结束信号

    return (void*)0;
}

/*****************************************************************************
*                        启动接收串口数据线程
*  函 数 名：StartRecv
*  功    能：启动读串口线程，接收串口数据
*  说    明：
*  参    数：
*  返 回 值：
*           成功，返回E_SERIAL_OK
*  创 建 人：王磊
*  创建时间：2013-10-18
*  修 改 人：
*  修改时间：
*****************************************************************************/
int AsyncSerialBase::StartRecv()
{
    //TO DO
    //启动异步串口数据接收线程
    pthread_t readThreadId =0;
    _isRunning = true;
    int rtn = pthread_create(&readThreadId, NULL, RecvThread, static_cast<void *>(this));
    if(rtn < 0)
    {
        _isRunning = false;
        CloseSerial();

        return E_SERIAL_ERROR;
    }


    //pthread_join(readThreadId, NULL);

    return E_SERIAL_OK;
}

/*****************************************************************************
*                        停止接收串口数据线程
*  函 数 名：StopRecv
*  功    能：停止读串口线程，停止接收串口数据
*  说    明：
*  参    数：
*  返 回 值：
*           成功，返回E_SERIAL_OK
*  创 建 人：王磊
*  创建时间：2013-10-18
*  修 改 人：
*  修改时间：
*****************************************************************************/
int AsyncSerialBase::StopRecv()
{
    //TO DO
    if(!_isRunning)
    {
       return E_SERIAL_OK;
    }

    //设置运行状态
    _isRunning = false;

    struct timespec timeOut;
    timeOut.tv_sec = 5 * oneMSecond;
    //超时等待线程结束信号量
    sem_timedwait(&_threadSem, &timeOut);

    return E_SERIAL_OK;
}


/*****************************************************************************
*                        获取一帧串口数据
*  函 数 名：GetFrame
*  功    能：获取一帧串口数据
*  说    明：如果在超时之前获取到一帧数据，则返回E_SERIAL_OK, 否则返回对应的错误码
*  参    数：
*           pBuf，帧缓冲区
*           timeoutSecs, 超时时间，单位：秒
*  返 回 值：
*           成功返回E_SERIAL_OK，否则返回对应错误码
*  创 建 人：王磊
*  创建时间：2013-10-18
*  修 改 人：
*  修改时间：
*****************************************************************************/
int AsyncSerialBase::FormFrame(unsigned char *pFrameBuf, const unsigned char *prawData, IN const int dataSize)
{
    //TO DO
    if(NULL == _serialFrame)
    {
        return E_SERIAL_INVALID_FRAME_EXTRACTOR;
    }

   return _serialFrame->FormFrame(pFrameBuf,prawData, dataSize);
}
