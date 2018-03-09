/*****************************************************************************
 ** 文 件 名：basictypes.cpp
 ** 主 要 类：
 **
 ** Copyright (c) 中科虹霸有限公司
 **
 ** 创 建 人：王磊
 ** 创建时间：20013-10-27
 **
 ** 修 改 人：
 ** 修改时间：
 ** 描  述:   串口类所涉及的基本数据类型定义
 ** 主要模块说明: 端口， 奇偶校验方式，数据位，停止位和串口配置, 帧信息结构
 **
 ** 版  本:   1.0.0
 ** 备  注:
 **
 *****************************************************************************/
#include "../include/basictypes.h"
#include "../../../Common/Exectime.h"


SerialConfig::SerialConfig(IN ParityMode parityMode,
		IN BaudRate baudRate,
		IN DataBits dataBits,
		IN StopBits stopBits)
{
	Exectime etm(__FUNCTION__);
	SerialParityMode = parityMode;
	SerialBaudRate   = baudRate;
	SerialDataBits   = dataBits;
	SerialStopBits   = stopBits;
}


//FrameInfo::FrameInfo(IN const unsigned char frameHeader, IN const unsigned char frameTail, IN const int frameLen)
//{
//    FrameHeader = frameHeader;
//    FrameTail   = frameTail;
//    FrameLen    = frameLen;
//}
