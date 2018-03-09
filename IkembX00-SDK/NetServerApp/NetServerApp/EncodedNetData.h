#pragma once
#include "stdafx.h"
#include <string.h>
#include <vector>

//请求编码
const unsigned short  REQ_SRVCTRL_RESET            = 0x0662;           //重启
const unsigned short  REQ_KEEP_ALIVE               = 0x0001;           //心跳
const unsigned short  REQ_OFFLINE_ADD_PERSON_IRIS  = 0x0301;           //增加脱机虹膜特征从服务器到客户端

//回应解码
const unsigned short  ACK_SRVCTRL_RESET            = 0x4662;
const unsigned short  ACK_KEEP_ALIVE               = 0x4001;
const unsigned short  ACK_OFFLINE_ADD_PERSON_IRIS  = 0x4301;

#pragma pack(push)
#pragma pack(1)
//头数据结构
typedef struct __HeadData
{
	char Head0;
	char Head1;
	unsigned short RequestCode;
	unsigned short RequestSubCode;
}HeadData, *PHeadData;

//头数据结构1：增加长度和错误码
typedef struct __HeadData1
{
	char Head0;
	char Head1;
	unsigned short RequestCode;               //请求码
	unsigned short RequestSubCode;            //请求子码
	int Len;                                  //数据区长度=错误码+实际长度
}HeadData1,*PHeadData1;

//标准命令的结构体---适用于无数据区传送的命令：请求和回应都使用本结构体
//为了方便也处理了一部分命令如下面的重启和心跳
typedef struct __ControlCmd
{
	char Head0;
	char Head1;
	unsigned short RequestCode;
	unsigned short RequestSubCode;
	int Len;
	unsigned short Check;
}ControlCmd,PControlCmd;

//远程控制重启请求：
typedef struct __ResetCmd
{
	char Head0;
	char Head1;
	unsigned short RequestCode;
	unsigned short RequestSubCode;
	int Len;
	unsigned short Check;                      //校验码 
}ResetCmd, *PRestCmd;
//远程控制重启请求回应：
typedef struct __ReResetCmd
{
	char Head0;
	char Head1;
	unsigned short RequestCode;
	unsigned short RequestSubCode;
	int Len;
	int ErrorCode;
	unsigned short Check;
}ReResetCmd, *PReResetCmd;

//心跳请求发送
typedef struct __KeepAliveCmd
{
	char Head0;
	char Head1;
	unsigned short RequestCode;
	unsigned short RequestSubCode;
	int Len;
	unsigned short Check;
}KeepAliveCmd,*PKeepAliveCmd;
//心跳回应
typedef struct __ReKeepAliveCmd
{
	char Head0;
	char Head1;
	unsigned short RequestCode;
	unsigned short RequestSubCode;
	int Len;
	int ErrorCode;
	unsigned short Check;
}ReKeepAliveCmd, *PReKeepAliveCmd;

//数据传输部分----从服务器到客户端用ADD，从客户端到服务器用GET开头;两个方向一致的，不带有这两个头。
//虹膜数据结构体
typedef struct __OfflineData
{
	char DevSn[64];
	uuid_t Fid;
	uuid_t Pid;
	int EyeFlag;
	BYTE FeatureData[512];
}OfflineData,POfflineData;

//最后两位的校验最后加上
typedef struct  __AddFeatureData
{
	HeadData1 Hd1;
	int DataSize;
	OfflineData FeatureData[0];
}AddFeatureData,*PAddFeatureData;
#pragma push(pop)

class EncodedNetData
{
public:
	EncodedNetData(){};
	~EncodedNetData(){};
public:
	static void EncodedRemoteReset(char *buf)
	{
		ResetCmd rc;
		char *bufCheck = nullptr; 

		rc.Head0 = 0x53;
		rc.Head1 = 0x53;
		rc.RequestCode = REQ_SRVCTRL_RESET;
		rc.RequestSubCode = 0xaaaa;
		rc.Len = 0;

		bufCheck = (char*)&rc;
		rc.Check = CheckData(buf,10);         //校验：去除校验两个字节后的整个帧
		memmove(buf,(char*)&rc,sizeof(rc));
	}
	static void EncodedKeepLive(char *buf)
	{
		KeepAliveCmd kac;
		char *bufCheck;

		kac.Head0 = 0x53;
		kac.Head1 = 0x53;
		kac.RequestCode = REQ_KEEP_ALIVE;
		kac.RequestSubCode = 0xaaaa;
		kac.Len = 0;

		bufCheck = (char*)&kac;
		kac.Check = CheckData(buf, 10);         //校验：去除校验两个字节后的整个帧
		memmove(buf, (char*)&kac, sizeof(kac));
	}
	//len :buf的长度即总长度
	static void EncodedAddFeature(char *buf,std::vector<OfflineData> &vod,int len)
	{
		//sizeof(HeadData1)+vod.size()+vod.size()*612+2
		PAddFeatureData pTemp = nullptr;     //传送特征缓冲区
		unsigned short checkValue = 0;
		int num = 0;
		pTemp = (PAddFeatureData)buf;
		 
		pTemp->Hd1.Head0 = 0x53;
		pTemp->Hd1.Head1 = 0x53;
		pTemp->Hd1.RequestCode = REQ_OFFLINE_ADD_PERSON_IRIS;
		pTemp->Hd1.RequestSubCode = 0xaaaa;

		pTemp->DataSize = vod.size();

		for (OfflineData &od : vod)
		{
			memmove(&pTemp->FeatureData[num++], &od, sizeof(od));
		}

		//去除头和校验
		pTemp->Hd1.Len = len - sizeof(pTemp->Hd1) -2;
		checkValue = CheckData(buf, len - 2);
		memmove(buf +len -2,&checkValue,2);
		
	}
	static unsigned short CheckData(char *buf, int len)
	{
		unsigned short checkValue = 0;
		for (int num = 0; num < len; num++)
		{
			checkValue += buf[num];
		}

		return checkValue;
	}
private:
};
class DeCodingNetData
{
public:
	static void DecodingRetomteReset(char *buf, PReResetCmd rrc)
	{
		memmove(rrc,buf,sizeof(rrc));
	}
	static void DecodingKeepLive(char *buf, PReKeepAliveCmd rkac)
	{
		memmove(rkac,buf,sizeof(rkac));
	}
	static void DecodingAddFeature(char *buf, std::vector<OfflineData> &vod)
	{
		PAddFeatureData pTemp = nullptr;     //传送特征缓冲区
		pTemp = (PAddFeatureData)buf;
		int len = pTemp->DataSize;
		for (int num = 0; num < len; num++)
		{
			OfflineData od;
			memmove(&od,&pTemp->FeatureData[num],sizeof(od));
			vod.push_back(od);
		}
	}
	static unsigned short CheckData(char *buf, int len)
	{
		unsigned short checkValue = 0;
		for (int num = 0; num < len; num++)
		{
			checkValue += buf[num];
		}

		return checkValue;
	}
};

