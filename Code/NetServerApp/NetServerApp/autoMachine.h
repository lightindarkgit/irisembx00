
/******************************************************************************************
** 文件名:  autoMachine.h
×× 主要类:  AutoMachine
**
** Copyright (c) 中科虹霸有限公司
** 创建人:   fjf
** 日  期:   2013-12-09
** 修改人:
** 日  期:
** 描  述:   网络通信解析类
** ×× 主要模块说明：
**
** 版  本:   1.0.0
** 备  注:
**
*****************************************************************************************/
#pragma once

#include "socketcom.h"

class AutoMachine
{
public:
	AutoMachine();
	~AutoMachine();
public:
	int ParseData(char * buf,int len);
	int BuildFrame(char ch);
	bool IsFullFrame();
	bool CheckSum();
	bool IsEnd();
	int RouteCircle(char ch,int state);
private:
	int _curState;
	int _nextState;
	int _countData;
	int _lenAdd;              //循环每次累加的长度
	int _lenData;             //数据实际长度

	char _bufFrame[g_recvBuf * 2];
};