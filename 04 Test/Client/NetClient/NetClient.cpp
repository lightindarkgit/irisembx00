#include "NetClient.h"
#include "LSGlobal.h"


NetClient::NetClient() :
    _connected(false)
{
    _adminSocket = 0;
    memset(&_adminSin, 0, sizeof(_adminSin));
    
    _adminPort = 0;
}

NetClient::~NetClient()
{
    DisConn();
}

int	NetClient::Connect(const std::string& ip, const unsigned int uport)
{
    if (ip.empty())
    {
        return netClientErrParam;
    }

    _adminIp   = ip;
    _adminPort = uport;

    _adminSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (_adminSocket < 0)
    {
        return netClientErrSocket;
    }
		

    _adminSin.sin_family = AF_INET;
    _adminSin.sin_addr.s_addr = inet_addr(_adminIp.c_str());
    _adminSin.sin_port = htons(_adminPort);

    int nret = connect(_adminSocket, (sockaddr*)&_adminSin, sizeof(_adminSin));
	if(nret < 0)
	{
        closesocket(_adminSocket);
		return -3;
	}

	int nodelay = 1;
    nret = setsockopt(_adminSocket, IPPROTO_TCP, TCP_NODELAY, (char*)&nodelay, sizeof(nodelay));
	if(nret < 0)
	{
		printf("set socket nodelay failed! %d\n", WSAGetLastError());
	}

    _connected = true;

	return 0;
}

int	NetClient::ReConnect()
{
	int nret = DisConn();
	
    nret = Connect(_adminIp, _adminPort);

	return nret;
}

int NetClient::DisConn()
{
    if (IsConnect())
    {
        closesocket(_adminSocket);
    }
    
    _adminSocket = 0;
    _connected = false;
	return 0;
}

bool NetClient::IsConnect()
{
    return _connected;
}

int NetClient::Send(const char* pbuf,const int len)
{
    int nret = writedata(_adminSocket, pbuf, len);
	return nret;
}

int NetClient::Send(const char* pbuf)
{
    PPACKHEAD pHead = (PPACKHEAD)pbuf;
    int sendlen = pHead->length + packHeadSize;

    return (Send(pbuf, sendlen));
}

int NetClient::Recv(char* pbuf,const int len)
{
    RecvParam recvParam;
    recvParam.fd = _adminSocket;
    recvParam.pbuf = pbuf;
    recvParam.buflen = len;

    // int nret = readdata(_adminSocket, pbuf, len);
    int nret = readdata(&recvParam);
    if (nret == netClientSuccess)
    {
        return recvParam.recvlen;
    }
    else
    {
        return recvParam.error;
    }

}



