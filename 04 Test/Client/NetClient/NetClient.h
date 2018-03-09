#ifndef _NET_CLIENT_H_
#define _NET_CLIENT_H_

#include <iostream>

#include "LSHeaders.h"
#include "LSGlobal.h"
#include "InitSocket.h"

class NetClient
{
public:
	NetClient();
	virtual ~NetClient();

private:

public:
	int	Connect(const std::string& ip, const unsigned int uport);
	int	ReConnect();
	int DisConn();
	int Send(const char* pbuf,const int len);
    int Send(const char* pbuf);
	int Recv(char* pbuf,const int len);
    bool IsConnect();       // 是否已连接

private:
    SOCKET			_adminSocket;   // 网络文件描述符（SOCKERT句柄）
    SOCKADDR_IN		_adminSin;
	std::string	    _adminIp;       // 服务的IP
	unsigned int	_adminPort;     // 服务的Port
    bool            _connected;     // 已连接标志

private:
    CInitSocket     _initSocket;    //SOCKET库的引入和控制

};

#endif

