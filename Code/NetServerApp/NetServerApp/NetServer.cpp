#include "stdafx.h"
#include "NetServer.h"
#include "../../Common/Exectime.h"

NetServer::NetServer()
{
	Exectime etm(__FUNCTION__);

}
NetServer::NetServer(std::string ip, USHORT port)
{
	Exectime etm(__FUNCTION__);

}

NetServer::~NetServer()
{
	Exectime etm(__FUNCTION__);
}

int NetServer::Init(std::string ip, USHORT port)
{
	Exectime etm(__FUNCTION__);
	//���õ�ַ����
	SOCKADDR_IN server;
	//server.S_un.S_addr = inet_addr(ip);

	//��ʼ�������˿�
	this->_listenSock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	server.sin_family = AF_INET;
	server.sin_port = htons(port);
	server.sin_addr.S_un.S_addr = htonl(INADDR_ANY);

	//�󶨵�ǰSOCKET
	if (SOCKET_ERROR == bind(_listenSock, (sockaddr*)&server, sizeof(server)))
	{
		return  WSAGetLastError();
	}

	//������������
	int iRet = listen(_listenSock,5);
	if (SOCKET_ERROR == iRet)
	{
		iRet = WSAGetLastError();
	}

	return iRet;
}

int NetServer::Send(char *buf,int len)
{
	Exectime etm(__FUNCTION__);
	int iRet = -1;
	return iRet;
}

bool NetServer::CreateRecvThread()
{
	Exectime etm(__FUNCTION__);
	bool bRet = false;
	return bRet;
}

bool NetServer::CreateSendThread()
{
	Exectime etm(__FUNCTION__);
	bool bRet = false;
	return bRet;
}

bool NetServer::CreateAcceptThread(void)
{
	Exectime etm(__FUNCTION__);
	return true;
}

void NetServer::StartThread(void)
{
	Exectime etm(__FUNCTION__);
}

