/*****************************************************************************
** 文 件 名：serialframe.h
** 主 要 类：SerialFrame
**
** Copyright (c) 中科虹霸有限公司
**
** 创 建 人：王磊
** 创建时间：20013-10-18
**
** 修 改 人：
** 修改时间：
** 描  述:   帧提取类的定义，处理串口数据流中的帧及配置帧信息
** 主要模块说明: 帧提取类
**
** 版  本:   1.0.0
** 备  注:
**
*****************************************************************************/
#pragma once
#include <string>
#include <vector>
#include <mutex>

#include "basictypes.h"
#include "innercommon.h"

////帧解析器类定义
//class SerialFrame
//{
//public:
//    SerialFrame();
//    ~SerialFrame();

////    //初始帧提取类
////    bool init(IN FrameInfo &frameInfo);

////    //获取帧信息
////    int GetFrameInfo(OUT FrameInfo& frameInfo);
////    //设置帧信息
////    int SetFrameInfo(IN const FrameInfo& frameInfo);

//    //获取一帧数据
//    int GetFrame(OUT unsigned char* pFrameBuf, IN unsigned char* pDataBuf, IN const int dataSize);
//    //将数据组织成帧
//    int FormFrame(OUT unsigned char* pFrameBuf, IN const unsigned char* pRawDataBuf, IN const int dataSize);

//protected:
//    //protected members
//    int FindFrameHeader(OUT unsigned char* pBuf, IN const int dataSize);
//    bool CheckCRC16(IN const unsigned char *rawData, IN const int dataLen);
//    int GenerateCRC16(OUT unsigned short *crcVal, IN const unsigned char* rawData, IN const int dataLen);
//    unsigned short CaculateCRC16(IN unsigned char dataIn, IN unsigned short crcIn);

//private:
//    //FrameInfo _frameInfo;       //帧信息，包括帧头，帧尾和帧长

//    unsigned char _dataBuf[frameDataBufSize];   //串口数据缓冲区
//    int _readPos;                //当前读取数据的起始位置
//    int _dataSize;              //缓冲区内有效数据字节长度
//};

//串口帧类定义
class SerialFrame
{
public:
    //public members
    SerialFrame();
    ~SerialFrame();

    //获取帧
    int GetFrame(OUT unsigned char* pFrameBuf);
    //生成帧
    int FormFrame(OUT unsigned char* pFrameBuf, IN const unsigned char* pRawDataBuf, IN const int dataSize);

    //向环形数据缓冲区中填写数据
    int WriteData(IN const unsigned char* pBuf, IN const int dataSize);

//    //判断环形数据缓冲区是否为空
//    bool IsEmpty();
//    //判断环形数据缓冲区是否为满
//    bool IsFull();
protected:
    //protected members
    bool CheckCRC16(IN const unsigned char *rawData, IN const int dataLen);
    int GenerateCRC16(OUT unsigned short *crcVal, IN const unsigned char* rawData, IN const int dataLen);
    unsigned short CaculateCRC16(IN unsigned char dataIn, IN unsigned short crcIn);

private:
    //private members
    unsigned char _frameDataBuf[frameDataBufSize];   //环形数据缓冲区
    std::mutex _bufMutex;          //缓冲区锁
    int _rdPos;       //数据读出起点
    int _wrPos;       //数据写入起点
    bool _isEmpty;    //数据缓冲区为空状态标志
    bool _isFull;     //数据缓冲区为满状态标志
};
