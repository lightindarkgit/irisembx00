
/******************************************************************************************
** �ļ���:  autoMachine.h
���� ��Ҫ��:  AutoMachine
**
** Copyright (c) �пƺ�����޹�˾
** ������:   fjf
** ��  ��:   2013-12-09
** �޸���:
** ��  ��:
** ��  ��:   ����ͨ�Ž�����
** ���� ��Ҫģ��˵����
**
** ��  ��:   1.0.0
** ��  ע:
**
*****************************************************************************************/
#pragma once

#include "socketcom.h"

class AutoMachine
{
public:
	AutoMachine();
	~AutoMachine();
public:
	int ParseData(char * buf,int len);
	int BuildFrame(char ch);
	bool IsFullFrame();
	bool CheckSum();
	bool IsEnd();
	int RouteCircle(char ch,int state);
private:
	int _curState;
	int _nextState;
	int _countData;
	int _lenAdd;              //ѭ��ÿ���ۼӵĳ���
	int _lenData;             //����ʵ�ʳ���

	char _bufFrame[g_recvBuf * 2];
};