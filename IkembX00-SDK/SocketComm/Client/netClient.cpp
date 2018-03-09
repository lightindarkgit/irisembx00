/******************************************************************************************
 ** 文件名:   NetClient.cpp
 ×× 主要类:   NetClient
 **  
 ** Copyright (c) 中科虹霸有限公司
 ** 创建人:   fjf
 ** 日  期:   2013-11-05
 ** 修改人:   
 ** 日  期:
 ** 描  述:   网络通信客户端基类
 **×× 主要模块说明: NetClient.h                               网络通信客户端基类                    
 **
 ** 版  本:   1.0.0
 ** 备  注:   
 **
 *****************************************************************************************/
#include "netClient.h"
#include <time.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/time.h>
#include <errno.h>
#include <sys/ioctl.h>
#include "../../Common/Exectime.h"
#include "../../Common/Logger.h"


NetClient::NetClient():_socket(-1)
{
	Exectime etm(__FUNCTION__);
}
NetClient::~NetClient()
{
	Exectime etm(__FUNCTION__);
}
bool NetClient::Init(const ClientData &cd, DelegateRecv dr)
{
	Exectime etm(__FUNCTION__);
	//处理基本参变量
	unsigned short port = g_port;  //针对新增加的端口进行控制 2015-04-24 fjf
	if (cd.Port > 0)
	{
		port = cd.Port;
	}
	this->_dr = dr;
	_socket = socket(PF_INET,SOCK_STREAM,0);

	//增加连接超时的处理从默认75秒减少到20秒并可以再次设置
	struct timeval tmDelay = {g_timeOut,0};
	socklen_t len = sizeof(tmDelay);

	if (setsockopt(_socket,SOL_SOCKET,SO_SNDTIMEO,&tmDelay,len) < 0)
	{
		//写日志
	}

	struct  sockaddr_in inServ;
	bzero(&inServ,sizeof(inServ));
	inServ.sin_family = AF_INET;
	inServ.sin_addr.s_addr = inet_addr(cd.Addr);
	inServ.sin_port = htons(port);                       //fjf 2015-04-24

	//连接服务端
	if (-1 == connect(_socket,(struct sockaddr*)&inServ,sizeof(inServ)))
	{
		LOG_ERROR("connect server failed. code: %d, %s",errno,strerror(errno));
		return false;
	}
	else
	{
		timeval tmv;
		tmv.tv_sec = 20;
		tmv.tv_usec = 0;

		if(0 != setsockopt(_socket,SOL_SOCKET,SO_SNDTIMEO,(char*)&tmv,sizeof(tmv)))
		{
			LOG_WARN("setsockopt SO_SNDTIMEO failed. fd: %d, %d %s",_socket,errno,strerror(errno));
		}

		LOG_ERROR("connect server success. fd: %d",_socket);
	}

	return true;
}
/*****************************************************************************
 *                        网络连接函数
 *  函 数 名：Connect
 *  功    能：连接服务器函数
 *  说    明：
 *  参    数：
 *           
 *  返 回 值：
 *  创 建 人：fjf
 *  创建时间：2013-11-22
 *  修 改 人：
 *  修改时间：
 *****************************************************************************/

bool NetClient::Connect(int socketFd, struct sockaddr*serverAddr, int addrLen)
{
	Exectime etm(__FUNCTION__);
	//在INIT中实现
	bool bRet = false;
	return bRet;
}
/*****************************************************************************
 *                         发送函数
 *  函 数 名：Send
 *  功    能：发送网络数据
 *  说    明：
 *  参    数：buf:发送网络缓冲区数据
 *            len:发送数据长度
 *  返 回 值：
 *  创 建 人：fjf
 *  创建时间：2013-11-05
 *  修 改 人：
 *  修改时间：
 *****************************************************************************/
int NetClient::Send( char*buf,int len)
{
	Exectime etm(__FUNCTION__);
	int dataLen = -1;
    dataLen = send(_socket,buf,len,MSG_NOSIGNAL);

    if (0 > dataLen )
	{
		//Add log
		LOG_ERROR("send failed. code: %d, %s",errno,strerror(errno));
	}

	return dataLen;
}

/*****************************************************************************
 *                         接收函数
 *  函 数 名：RecvTransfer
 *  功    能：接收网络数据
 *  说    明：
 *  参    数：buf:接收网络缓冲区数据
 * 
 *  返 回 值：
 *  创 建 人：fjf
 *  创建时间：2013-11-05
 *  修 改 人：
 *  修改时间：
 *****************************************************************************/

int NetClient::RecvTransfer( char * buf)
{
	Exectime etm(__FUNCTION__);
	int recvLen = -1;
	return recvLen;
}
bool NetClient::Close()
{
	Exectime etm(__FUNCTION__);
	_isQuit = false;
	return close(_socket) == 0 ? true:false;
}

/*****************************************************************************
 *                         非阻塞判断连接函数
 *  函 数 名：NonBlockConnect
 *  功    能：利用非阻塞的机制创建网络通信连接
 *  说    明：这个有移植的局限性，一定要引起注意
 *  参    数： 
 *             
 *  返 回 值：
 *  创 建 人：fjf
 *  创建时间：2013-11-21
 *  修 改 人：
 *  修改时间：
 *****************************************************************************/

bool NetClient::NonBlockConnect()
{
	Exectime etm(__FUNCTION__);
	timeval tm;
	fd_set set;

	if (!this->ControlBlock(false))
	{
		return false;
	}

	if (connect(_socket,(struct sockaddr*)&_cd.Addr,sizeof(_cd.Addr)) == -1)
	{
		tm.tv_sec = g_timeOut;
		tm.tv_usec = 0;
		FD_ZERO(&set);
		FD_SET(_socket,&set);

		if (select(_socket + 1,NULL,&set,NULL,&tm) > 0)
		{
			int error = 0;
			socklen_t len = sizeof(error);
			getsockopt(_socket,SOL_SOCKET,SO_ERROR,&error,&len);
			if (0 == error)
			{
				return false;
			}
		}
	}

	if (!this->ControlBlock(true))
	{
		return false;
	}

	return true;
}

/*****************************************************************************
 *                         设置当前网络阻塞或非阻塞函数
 *  函 数 名：ControlBlock
 *  功    能：设置网络通信属性
 *  说    明： 
 *  参    数：isBlock:是否为阻塞通信
 *             
 *  返 回 值：
 *  创 建 人：fjf
 *  创建时间：2013-11-21
 *  修 改 人：
 *  修改时间：
 *****************************************************************************/

bool NetClient::ControlBlock(bool isBlock)
{
	Exectime etm(__FUNCTION__);
	int flags = fcntl(_socket,F_GETFL,0);
	if (-1 == flags)
	{
		return false;			   
	}

	if (isBlock)
	{
		flags &= O_NONBLOCK;

	}
	else
	{
		flags |= O_NONBLOCK;
	}

	flags = fcntl(_socket,F_SETFL,flags);

	return 0 ==flags?true:false;

}

/*****************************************************************************
 *                         唤醒阻塞线程函数
 *  函 数 名：NoticeOne
 *  功    能：唤醒当前阻塞的线程
 *  说    明：
 *  参    数： 
 *
 *  返 回 值：
 *  创 建 人：fjf
 *  创建时间：2013-12-31
 *  修 改 人：
 *  修改时间：
 *****************************************************************************/
void NetClient::NoticeOne()
{
	Exectime etm(__FUNCTION__);
	ConWait.NoticeOne();
}

