/*****************************************************************************
** 文 件 名：innercommon.h
** 主 要 类：
**
** Copyright (c) 中科虹霸有限公司
**
** 创 建 人：王磊
** 创建时间：20013-11-08
**
** 修 改 人：
** 修改时间：
** 描  述:   内部实现用到的常量定义
** 主要模块说明:
**
** 版  本:   1.0.0
** 备  注:
**
*****************************************************************************/
#pragma once

//单元测试标记
//#define UT_SERIAL

//定义各缓冲区大小
enum BufferSize{frameBufSize = 9, recvBufSize = 1024, frameDataBufSize = recvBufSize};

//定义时间常量，单位为毫秒
enum{zeroMSecond = 0, oneMSecond = 1};

extern const unsigned char FRAME_HEADER;   //帧头码
extern const unsigned char FRAME_TAIL;     //帧尾码

extern const int FRAME_CRCED_DATA_LEN;     //被校验数据长度，不包含CRC校验位，本应用的串口协议定义该值为6
extern const int FRAME_CRC_DATA_LEN;       //CRC校验位长度，本应用的串口协议定义该值为2
extern const int FRAME_LEN;                //帧长，本应用的串口协议定义该值为9
extern const int FRAME_DATA_OFFSET;        //帧内用户数据字节偏移量，本应用的串口协议定义该值为2
extern const int FRAME_DATA_SIZE;          //帧内封装的用户数据字节大小，本应用的串口协议定义该值为4
                                           //一个命令字节，三个数据字节

extern const int BLOCK_MODE;              //阻塞模式标志
