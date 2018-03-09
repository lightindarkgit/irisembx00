#include "../include/cserial.h"
#include "../include/basictypes.h"
#include <string.h>
#include <iostream>
#include <pthread.h>
#include <unistd.h>
#include "../../../Common/Exectime.h"

CSerial_UT::CSerial_UT()
{
	Exectime etm(__FUNCTION__);
	//TO DO
}

CSerial_UT::~CSerial_UT()
{
	Exectime etm(__FUNCTION__);
	//TO DO
	std::cout << "~CSerial() is called!" << std::endl;
}

int CSerial_UT::Open()
{
	Exectime etm(__FUNCTION__);
	return (int)(this);
}

int CSerial_UT::GetConfig()
{
	Exectime etm(__FUNCTION__);
	//TO DO
	return E_SERIAL_OK;
}

int CSerial_UT::SetConfig(const SerialConfig& cfg)
{
	Exectime etm(__FUNCTION__);
	//TO DO
	std::cout << "serial config: " << std::endl
		<< "    baud rate: " << cfg.SerialBaudRate << std::endl
		<< "    parity mode: " << cfg.SerialParityMode << std::endl
		<< "    data bits: " << cfg.SerialDataBits << std::endl
		<< "    stop bits: " << cfg.SerialStopBits << std::endl;

	return E_SERIAL_OK;
}



int CSerial_UT::Read(unsigned char *pBuf)
{
	Exectime etm(__FUNCTION__);
	//TO DO
	static int sentCounter = 0;

	sentCounter++;
	if(sentCounter %= 5)
	{
		sleep(sentCounter);
	}

	unsigned char cmd[] = {0xDD, 0x09, 0x0D, 0x05, 0x00, 0x00, 0xDC, 0x3A, 0xED,
		0xdd, 0x09, 0x0b, 0x00, 0x00, 0x00, 0xcc, 0xb3, 0xed,
		0xDD, 0x09, 0x0F, 0x03, 0x02, 0x01, 0xFD, 0x23, 0xED,
		0xDD, 0x09, 0x0E, 0x02, 0x03, 0x01, 0xAC, 0x8F, 0xED,
		0xDD, 0x09, 0x0D, 0x01, 0x02, 0x03, 0xDC, 0x9A, 0xED,
		0xDD, 0x09, 0x0C, 0x01, 0x02, 0x03, 0xDD, 0x66, 0xED,
		0xDD, 0x09, 0x0B, 0x01, 0x02, 0x03, 0xDC, 0x12, 0xED,
		0xDD, 0x09, 0x0A, 0x01, 0x01, 0x02, 0x1C, 0xDE, 0xED,
		0xDD, 0x09, 0x0F, 0x03, 0x02, 0x01, 0xFD, 0x23, 0xED,
		0xDD, 0x09, 0x0E, 0x02, 0x03, 0x01, 0xAC, 0x8F, 0xED,
		0xDD, 0x09, 0x0D, 0x01, 0x02, 0x03, 0xDC, 0x9A, 0xED,
		0xDD, 0x09, 0x0C, 0x01, 0x02, 0x03, 0xDD, 0x66, 0xED,
		0xDD, 0x09, 0x0B, 0x01, 0x02, 0x03, 0xDC, 0x12, 0xED,
		0xDD, 0x09, 0x0A, 0x01, 0x01, 0x02, 0x1C, 0xDE, 0xED,
		0xDD, 0x09, 0x0E, 0x02, 0x03, 0x01, 0xAC, 0x8F, 0xED};
	unsigned char* pCmd = cmd;

	int cmdSize = sizeof(cmd) / sizeof(cmd[0]);    
	memcpy(pBuf, pCmd + sentCounter * 3 * 9, cmdSize / 5);

	return cmdSize / 5;
}

int CSerial_UT::Write(const unsigned char *pBuf, const int sizeToWrite)
{
	Exectime etm(__FUNCTION__);
	//TO DO
	int bufSize = sizeof(_serialData) / sizeof(_serialData[0]);
	int minSize, leftToWrite;


	for(leftToWrite = sizeToWrite; leftToWrite > 0;)
	{
		minSize = leftToWrite >= bufSize ? bufSize : sizeToWrite;
		memcpy(_serialData, pBuf, minSize);
		_serialData[minSize] = '\0';

		//显示通过串口发送的数据
		std::cout << "serial data written: 0x" ;
		for(int size = 0; size < minSize;)
		{
			printf("%02X", _serialData[size++]);
		}
		printf("\n\n");

		leftToWrite -= minSize;
		pBuf += minSize;
	}
	return sizeToWrite - leftToWrite;
}

