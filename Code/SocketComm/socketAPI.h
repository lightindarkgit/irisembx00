/******************************************************************************************
** 文件名:  socketAPI.h 
×× 主要类:  socketAPI
**  
** Copyright (c) 中科虹霸有限公司
** 创建人:   fjf
** 日  期:   2013-11-06
** 修改人:   
** 日  期:
** 描  述:   网络通信接口实现类
** ×× 主要模块说明：                    
**
** 版  本:   1.0.0
** 备  注:   
**
*****************************************************************************************/
#pragma once
#include "socketInterfaceAPI.h"
#include "socketcom.h"
#include "autoMachine.h"
#include "Client/netClient.h"
#include "Client/netClientSelect.h"


class SocketAPI:public ISocketAPI
{
public:
	SocketAPI();
	SocketAPI(const ClientData &cd,CallBackRecv cbr);
	~SocketAPI();
public:
	int  GetData(char *buf); 
	bool InitSocket(const ClientData &cd,CallBackRecv cbr);
	int  SendData(char * buf,int len);	
	void FrameData(char*buf,int len);
	void NetRecvData(char*buf,int len);

	bool InitSocket();                                             //初始化的重载函数
	int Start();                                                  //开始通信
	int Close();                                                  //关闭退出
	int Stop();                                                   //停止
public:
	//线程发送数据的封送和清除
	bool AddData(SendDataBuf sdb);
	bool ClearData();
private:
	bool SetBind(CallBackRecv cbr);			               //绑定函数
private:
	CallBackRecv _cbRecv;                                  //函数变量 2013-12-31
	NetClientSelect _netClient;
	AutoMachine _autoMac;
	ClientData _cd;
};

