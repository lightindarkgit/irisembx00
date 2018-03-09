/******************************************************************************************
** �ļ���:   CAtuoState.h
���� ��Ҫ��:   CAtuoState
**  
** Copyright (c) �пƺ�����޹�˾
** ������:   fjf
** ��  ��:   2013-04-17
** �޸���:   
** ��  ��:
** ��  ��:   ��̨����ͨ�Ų�����
 *���� ��Ҫģ��˵��: CAtuoState.h                               �Զ����ദ����                     
**
** ��  ��:   1.0.0
** ��  ע:   
**
*****************************************************************************************/

#pragma once

#include "initsock.h"

#define MAX_SUM   1000                               //���Ͳ��ܳ���1000
const int g_constMinSum = 200                        //��С�Ͳ���С��200----------˵�������Ǳ��룬ʹ��ȫ�ֳ��������               

class CNetComm;//ǰ������

class CAtuoState
{
public:
	CAtuoState(void);
	virtual ~CAtuoState(void);
public:
	int ParseData(char* buf, int len);	              //���ݷ�������
	void MakePicData(char* frameBuf,int len);           //��װͼ��
	BYTE GetCheckSum(char* buf, int len);
	int ParseData(void*owner,char *buf,int len,bool);  //���ط�������
protected:
	int _sumData;                                         //������s
private:
	int _sumPack;                                         //ͼ���������
	int _packageLen;                                      //ÿ������
	int _picNum;                                          //ͼƬ��С
	char _bufFrame[256];                                  //GPRS����ͨ��֡��С��Ŀǰ���Ϊ128������һ������չ
	static bool s_isGprs;

public:
	int CheckValue;                                       //У��͵�ֵ ---����������д�շ�                               
};
