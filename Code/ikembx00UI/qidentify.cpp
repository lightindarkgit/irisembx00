/*****************************************************************************
 ** 文 件 名：QIdentify.cpp
 ** 主 要 类：QIdentify
 **
 ** Copyright (c) 中科虹霸有限公司
 **
 ** 创 建 人：刘中昌
 ** 创建时间：20013-10-15
 **
 ** 修 改 人：
 ** 修改时间：
 ** 描  述:   识别模块
 ** 主要模块说明: 识别类 ：识别过程中需要的状态、回调等信息
 **
 ** 版  本:   1.0.0
 ** 备  注: 该类已经取消
 **
 *****************************************************************************/
#include "qidentify.h"
#include "../Common/Exectime.h"

QIdentify::QIdentify(PersonType personType)
{
	Exectime etm(__FUNCTION__);
	_personType= personType;
}
QIdentify::~QIdentify()
{
	Exectime etm(__FUNCTION__);
}
