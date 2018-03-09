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
#include "../Common/Exectime.h"

AutoMachine::AutoMachine():_lenAdd(0)
{
	Exectime etm(__FUNCTION__);
}
AutoMachine::~AutoMachine()
{
	Exectime etm(__FUNCTION__);
}
int AutoMachine::BuildFrame(char ch)
{
	Exectime etm(__FUNCTION__);
	return 0;
}

bool AutoMachine::IsFullFrame()
{
	Exectime etm(__FUNCTION__);
	bool bRet = false;
	return bRet;
}

bool AutoMachine::CheckSum()
{
	Exectime etm(__FUNCTION__);
	bool bRet = false;
	return bRet;
}
bool AutoMachine::IsEnd()
{
	Exectime etm(__FUNCTION__);
	bool bRet = false;
	return bRet;
}

int AutoMachine::ParseData(char *buf, int len)
{
	Exectime etm(__FUNCTION__);
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
	Exectime etm(__FUNCTION__);
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

	return 0;	
}

