/******************************************************************************************
** �ļ���:  autoMachine.cpp
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

#include "autoMachine.h"

AutoMachine::AutoMachine():_lenAdd(0)
{
}
AutoMachine::~AutoMachine()
{
}
int AutoMachine::BuildFrame(char ch)
{
	return 0;
}

bool AutoMachine::IsFullFrame()
{
	bool bRet = false;
	return bRet;
}

bool AutoMachine::CheckSum()
{
	bool bRet = false;
	return bRet;
}
bool AutoMachine::IsEnd()
{
	bool bRet = false;
	return bRet;
}

int AutoMachine::ParseData(char *buf, int len)
{
	int iRet = 0;
	for (int num = 0; num < len;num++)
	{
		this->_nextState = this->RouteCircle(buf[num],this->_curState);
		IsFullFrame();
		_curState = _nextState;
	}
	return iRet;
}
int AutoMachine::RouteCircle(char ch, int state)
{
	switch (state)
	{
	case 0:
		if (0x53 == ch)
		{
			this->BuildFrame(ch);
			state = 1;
		}
		
		return state;
	case 1:
		if (0x53 == ch)
		{
			this->BuildFrame(ch);
			state = 2;
		}
		return state;
	case 2:
		this->BuildFrame(ch);
		
		if (this->IsEnd())
		{
			state = 0;
		}
		return state;
	default:
		break;
		return 0;
	}

	
}