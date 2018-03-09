/*****************************************************************************
** 文 件 名：innercommon.cpp
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
#include "innercommon.h"

const unsigned char FRAME_HEADER = 0xdd;
const unsigned char FRAME_TAIL = 0xed;

const int FRAME_CRCED_DATA_LEN = 6;
const int FRAME_CRC_DATA_LEN = 2;
const int FRAME_LEN = frameBufSize;
const int FRAME_DATA_OFFSET = 2;
const int FRAME_DATA_SIZE = 4;

const int BLOCK_MODE = -1;
