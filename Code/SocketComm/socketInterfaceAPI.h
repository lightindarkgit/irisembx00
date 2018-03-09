
/******************************************************************************************
** 文件名:  socketInterfaceAPI.h 
×× 主要类:  ISocketAPI
**  
** Copyright (c) 中科虹霸有限公司
** 创建人:   fjf
** 日  期:   2013-11-20
** 修改人:   
** 日  期:
** 描  述:   网络通信接口类
** ×× 主要模块说明：                    
**
** 版  本:   1.0.0
** 备  注:   
**
*****************************************************************************************/
#pragma once
#include <functional>
#include "socketcom.h"

class ISocketAPI
{
public:
	typedef std::function<void(char*buf,int len)> CallBackRecv;    			 //标准库的function变量声明
public:
	ISocketAPI();
	virtual ~ISocketAPI();
public:
	static ISocketAPI* CreateInstance(CallBackRecv cbr, const ClientData &cd) throw();
	virtual int Start() = 0;
	virtual int Stop()  = 0;
	virtual int Close() = 0;
	virtual int SendData(char * buf, int len) = 0;

};
