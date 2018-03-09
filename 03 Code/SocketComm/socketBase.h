/******************************************************************************************
** 文件名:   SocketBase.h
×× 主要类:   SocketBase
**  
** Copyright (c) 中科虹霸有限公司
** 创建人:   fjf
** 日  期:   2013-11-06
** 修改人:   
** 日  期:
** 描  述:   网络通信抽象基类
** ×× 主要模块说明：                    
**
** 版  本:   1.0.0
** 备  注:   
**
*****************************************************************************************/
#pragma once
#include "socketcom.h"

class SocketBase
{
public:
	SocketBase(){};
	virtual ~SocketBase(){};
public:
	virtual bool Init(const ClientData &cd, DelegateRecv cbr) = 0;
};
