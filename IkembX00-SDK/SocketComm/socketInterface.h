/******************************************************************************
*  文 件 名：socketInterface.cpp
*  主 要 类：INetSocket
*  说    明: 网络通信的标准数据处理类
*  创 建 人：fjf
*  创建时间：20013-11-04
*  修 改 人：
*  修改时间：
*****************************************************************************/
#pragma once
class INetSocket
{
	public:
		INetSocket(){};
		virtual	~INetSocket(){};
	public:
		virtual int DoWithData(unsigned char * buf) = 0;
	private:
		int iLen;
	
};
