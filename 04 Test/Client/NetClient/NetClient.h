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
    bool IsConnect();       // �Ƿ�������

private:
    SOCKET			_adminSocket;   // �����ļ���������SOCKERT�����
    SOCKADDR_IN		_adminSin;
	std::string	    _adminIp;       // �����IP
	unsigned int	_adminPort;     // �����Port
    bool            _connected;     // �����ӱ�־

private:
    CInitSocket     _initSocket;    //SOCKET�������Ϳ���

};

#endif

