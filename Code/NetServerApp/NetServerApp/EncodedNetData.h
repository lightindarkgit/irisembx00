#pragma once
#include "stdafx.h"
#include <string.h>
#include <vector>

//�������
const unsigned short  REQ_SRVCTRL_RESET            = 0x0662;           //����
const unsigned short  REQ_KEEP_ALIVE               = 0x0001;           //����
const unsigned short  REQ_OFFLINE_ADD_PERSON_IRIS  = 0x0301;           //�����ѻ���Ĥ�����ӷ��������ͻ���

//��Ӧ����
const unsigned short  ACK_SRVCTRL_RESET            = 0x4662;
const unsigned short  ACK_KEEP_ALIVE               = 0x4001;
const unsigned short  ACK_OFFLINE_ADD_PERSON_IRIS  = 0x4301;

#pragma pack(push)
#pragma pack(1)
//ͷ���ݽṹ
typedef struct __HeadData
{
	char Head0;
	char Head1;
	unsigned short RequestCode;
	unsigned short RequestSubCode;
}HeadData, *PHeadData;

//ͷ���ݽṹ1�����ӳ��Ⱥʹ�����
typedef struct __HeadData1
{
	char Head0;
	char Head1;
	unsigned short RequestCode;               //������
	unsigned short RequestSubCode;            //��������
	int Len;                                  //����������=������+ʵ�ʳ���
}HeadData1,*PHeadData1;

//��׼����Ľṹ��---�����������������͵��������ͻ�Ӧ��ʹ�ñ��ṹ��
//Ϊ�˷���Ҳ������һ�������������������������
typedef struct __ControlCmd
{
	char Head0;
	char Head1;
	unsigned short RequestCode;
	unsigned short RequestSubCode;
	int Len;
	unsigned short Check;
}ControlCmd,PControlCmd;

//Զ�̿�����������
typedef struct __ResetCmd
{
	char Head0;
	char Head1;
	unsigned short RequestCode;
	unsigned short RequestSubCode;
	int Len;
	unsigned short Check;                      //У���� 
}ResetCmd, *PRestCmd;
//Զ�̿������������Ӧ��
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

//����������
typedef struct __KeepAliveCmd
{
	char Head0;
	char Head1;
	unsigned short RequestCode;
	unsigned short RequestSubCode;
	int Len;
	unsigned short Check;
}KeepAliveCmd,*PKeepAliveCmd;
//������Ӧ
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

//���ݴ��䲿��----�ӷ��������ͻ�����ADD���ӿͻ��˵���������GET��ͷ;��������һ�µģ�������������ͷ��
//��Ĥ���ݽṹ��
typedef struct __OfflineData
{
	char DevSn[64];
	uuid_t Fid;
	uuid_t Pid;
	int EyeFlag;
	BYTE FeatureData[512];
}OfflineData,POfflineData;

//�����λ��У��������
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
		rc.Check = CheckData(buf,10);         //У�飺ȥ��У�������ֽں������֡
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
		kac.Check = CheckData(buf, 10);         //У�飺ȥ��У�������ֽں������֡
		memmove(buf, (char*)&kac, sizeof(kac));
	}
	//len :buf�ĳ��ȼ��ܳ���
	static void EncodedAddFeature(char *buf,std::vector<OfflineData> &vod,int len)
	{
		//sizeof(HeadData1)+vod.size()+vod.size()*612+2
		PAddFeatureData pTemp = nullptr;     //��������������
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

		//ȥ��ͷ��У��
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
		PAddFeatureData pTemp = nullptr;     //��������������
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

