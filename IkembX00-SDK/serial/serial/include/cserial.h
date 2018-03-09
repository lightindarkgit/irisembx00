#pragma once

//单元测试类
#include "basictypes.h"

class CSerial_UT
{
public:
    CSerial_UT();
    ~CSerial_UT();

    int Open();
    int SetConfig(const SerialConfig& cfg);
    int GetConfig();

    int Read(unsigned char* pBuf);
    int Write(const unsigned char* pBuf, const int sizeToWrite);

private:
    unsigned char _serialData[18];
};
