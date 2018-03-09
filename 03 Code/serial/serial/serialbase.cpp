/*****************************************************************************
** 文 件 名：serialbase.cpp
** 主 要 类：SerialBase
**
** Copyright (c) 中科虹霸有限公司
**
** 创 建 人：王磊
** 创建时间：20013-10-18
**
** 修 改 人：
** 修改时间：
** 描  述:   串口基类实现
** 主要模块说明: 串口基类实现
**
** 版  本:   1.0.0
** 备  注:
**
*****************************************************************************/
#include <stdio.h>        //标准输入输出定义
#include <stdlib.h>       //标准函数库定义
#include <unistd.h>       //Unix标准函数定义
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>        //文件控制定义
#include <termios.h>      //POSIX中断控制定义
#include <errno.h>        //错误号定义
#include <string.h>
#include <pthread.h>

#include "serialbase.h"

//单元测试头文件
#ifdef UT_SERIAL
#include "cserial.h"
CSerial_UT cs;
#endif

using namespace std;

/*****************************************************************************
*                        字符串转成小写
*  函 数 名：StrToLower
*  功    能：将给定的字符串全部转化为小写
*  说    明：
*  参    数：
*          str，未转换之前的原始字符串
*  返 回 值：
*          返回转换之后的字符串
*  创 建 人：王磊
*  创建时间：2013-11-08
*  修 改 人：
*  修改时间：
*****************************************************************************/
string ToLowerStr(IN const string &str)
{
    string lowerStr = str;
    for(size_t offset = 0; offset < str.length(); offset++)   //str.length()返回无符号整数，因此offset类型被定义为size_t
    {
        lowerStr[offset] = tolower(lowerStr[offset]);
    }

    return lowerStr;
}


SerialBase::SerialBase():
    _fd(-1)
{
    //TO DO
    pthread_mutex_init(&_recvBufMutex, NULL);
    pthread_cond_init(&_recvBufNoneEmpty, NULL);
}

SerialBase::~SerialBase()
{
    //TO DO
    CloseSerial();
}

/*****************************************************************************
*                        串口类初始化
*  函 数 名：Init
*  功    能：初始化串口基类的数据成员
*  说    明：
*  参    数：
*  返 回 值：
*           成功返回E_SERIAL_OK
*  创 建 人：王磊
*  创建时间：2013-11-08
*  修 改 人：
*  修改时间：
*****************************************************************************/
//int SerialBase::Init()
//{
//    //TO DO
//    pthread_mutex_init(&_recvBufMutex, NULL);
//    pthread_cond_init(&_recvBufNoneEmpty, NULL);

//    return E_SERIAL_OK;
//}


/*****************************************************************************
*                        关闭串口
*  函 数 名：CloseSerial
*  功    能：关闭串口
*  说    明：
*  参    数：无
*  返 回 值：void
*  创 建 人：王磊
*  创建时间：2013-10-18
*  修 改 人：
*  修改时间：
*****************************************************************************/
void SerialBase::CloseSerial()
{
    //TO DO
    if(IsOpen())
    {
        close(_fd);
        _fd = -1;
        _serialName.clear();

//单元测试....
#ifdef UT_SERIAL
        _fd = -1;
        printf("~CloseSerial() is called!\n");
#endif
//....单元测试
    }

    return;
}

/*****************************************************************************
*                        获取串口状态
*  函 数 名：IsOpen
*  功    能：获取串口当前状态
*  说    明：
*  参    数：无
*  返 回 值：
*           串口打开返回true， 否则返回false
*  创 建 人：王磊
*  创建时间：2013-10-18
*  修 改 人：
*  修改时间：
*****************************************************************************/
bool SerialBase::IsOpen()
{
    return (_fd >= 0);
}

/*****************************************************************************
*                        获取串口描述符
*  函 数 名：GetSerialFd
*  功    能：获取串口的文件描述符
*  说    明：
*  参    数：无
*  返 回 值：
*           串口描述符
*  创 建 人：王磊
*  创建时间：2013-10-18
*  修 改 人：
*  修改时间：
*****************************************************************************/
int SerialBase::GetSerialFd()
{
    return _fd;
}

/*****************************************************************************
*                        获取串口配置
*  函 数 名：GetConfig
*  功    能：获取串口配置信息
*  说    明：
*  参    数：
*           serialConfig， 通过该参数返回当前串口配置
*  返 回 值：
*           成功获取配置返回E_SERIAL_OK, 其他返回对应错误码
*  创 建 人：王磊
*  创建时间：2013-10-18
*  修 改 人：
*  修改时间：
*****************************************************************************/
int SerialBase::GetConfig(OUT SerialConfig &serialConfig)
{
    //TO DO
    serialConfig = _serialConfig;

    return E_SERIAL_OK;
}

/*****************************************************************************
*                        设置串口
*  函 数 名：SetConfig
*  功    能：设置串口配置参数
*  说    明：
*  参    数：
*           serialConfig， 串口配置参数
*  返 回 值：
*           成功设置配置返回E_SERIAL_OK, 其他返回对应错误码
*  创 建 人：王磊
*  创建时间：2013-10-18
*  修 改 人：
*  修改时间：
*****************************************************************************/
int SerialBase::SetConfig(IN const SerialConfig &serialConfig)
{
    //TO DO
//单元测试....
#ifdef UT_SERIAL
    _serialConfig = serialConfig;   //更新串口配置
    return cs.SetConfig(serialConfig);
#endif
//....单元测试

    termios oldCfg, newCfg;
    int speed;

    if(0 != tcgetattr(_fd, &oldCfg))
    {
        return E_SERIAL_GET_SERIAL;
    }


    newCfg = oldCfg;
    cfmakeraw(&newCfg);       //raw mode

    newCfg.c_cflag |= CLOCAL | CREAD;

    //设置波特率
    switch(serialConfig.SerialBaudRate)
    {
    case 150:
        speed = B150;
        break;

    case 300:
        speed = B300;
        break;

    case 600:
        speed = B600;
        break;

    case 1200:
        speed = B1200;
        break;

    case 2400:
        speed = B2400;
        break;

    case 4800:
        speed = B4800;
        break;

    case 9600:
        speed = B9600;
        break;

    case 19200:
        speed = B19200;
        break;

    case 38400:
        speed = B38400;
        break;

    case 115200:
        speed = B115200;
        break;
    default:
        return E_SERIAL_SET_BAUD;
    }

    cfsetispeed(&newCfg, speed);
    cfsetospeed(&newCfg, speed);


    //设置校验方式
    switch(serialConfig.SerialParityMode)
    {
    case oddParity:    //odd parity
        newCfg.c_cflag |= (PARODD | PARENB);
        newCfg.c_iflag |= INPCK;
        break;

    case evenParity:   //even parity
        newCfg.c_cflag |= PARENB;
        newCfg.c_cflag &= ~PARODD;
        newCfg.c_iflag |= INPCK;
        break;

    case spaceParity:  //as no parity
        newCfg.c_cflag &= ~PARENB;
        newCfg.c_cflag &= ~CSTOPB;
        break;

    case noParity:     //not parity
        newCfg.c_cflag &= ~PARENB;
        newCfg.c_iflag &= ~INPCK;
        break;

    default:
        return E_SERIAL_SET_PARITY;
    }


    newCfg.c_cflag &= ~CSIZE;

    //设置数据位
    switch(serialConfig.SerialDataBits)
    {
    case fiveBits:
        newCfg.c_cflag |= CS5;
        break;

    case sixBits:
        newCfg.c_cflag |= CS6;
        break;

    case sevenBits:
        newCfg.c_cflag |= CS7;
        break;
    case eightBits:
        newCfg.c_cflag |= CS8;
        break;

    default:
        return E_SERIAL_SET_DATA_BITS;
    }

    //设置停止位
    switch(serialConfig.SerialStopBits)
    {
    case oneBit:
        newCfg.c_cflag &= ~CSTOPB;
        break;

    case twoBits:
        newCfg.c_cflag |= CSTOPB;
        break;

    default:
        return E_SERIAL_SET_STOP_BITS;
    }


//    //设置输入校验方式
//    if(noParity == serialConfig.SerialParityMode)
//    {
//        newCfg.c_iflag |= INPCK;
//    }

    newCfg.c_cflag &= ~CRTSCTS;

    //newCfg.c_lflag |= ICANON;
    newCfg.c_cc[VTIME] = 20;   //Minum wait time: 2s.
    newCfg.c_cc[VMIN] = 0;
    tcflush(_fd, TCIOFLUSH);     //Handle the rest of data that hasn't received
    if(0 != (tcsetattr(_fd, TCSANOW, &newCfg)))   //Active the new configuration
    {
        return E_SERIAL_SET_SERIAL;
    }

    sleep(1);
    _serialConfig = serialConfig;   //更新串口配置



    return E_SERIAL_OK;
}

/*****************************************************************************
*                        接收指定长度的串口数据
*  函 数 名：Recv
*  功    能：接收指定长度的串口数据
*  说    明：
*  参    数：
*           pBuf， 接收缓冲区指针
*           sizeToRecv，期望接收的数据字节长度
*           timeoutSecs，超时时间，单位为秒
*  返 回 值：
*           成功，返回读取的数据字节长度；失败返回对应错误码
*  创 建 人：王磊
*  创建时间：2013-10-18
*  修 改 人：
*  修改时间：
*****************************************************************************/
int SerialBase::Recv(OUT unsigned char *pBuf, IN const int sizeToRecv, IN const int timeoutMSecs)
{
    //TO DO
    if(!IsOpen())
    {
        return E_SERIAL_INVALID_PORT;
    }

    if(NULL == pBuf)
    {
        return E_SERIAL_INVALID_PTR;
    }

//单元测试....
#ifdef UT_SERIAL
    return cs.Read(pBuf);
#endif
//....单元测试

    int nFds = 0;
    int readyFds = 0;
    fd_set readFds;
    timeval tv;

    FD_ZERO(&readFds);
    FD_SET(_fd, &readFds);

    nFds = _fd + 1;

    tv.tv_sec = timeoutMSecs / 1000;
    tv.tv_usec = (timeoutMSecs % 1000) * 1000;

    if(BLOCK_MODE == timeoutMSecs)
    {
        //同步方式读取数据
        readyFds = select(nFds, &readFds, NULL, NULL, NULL);
    }
    else
    {
        //阻塞超时模式读取数据
        readyFds = select(nFds, &readFds, NULL, NULL, &tv);
    }

    if(readyFds > 0)
    {
        if(FD_ISSET(_fd, &readFds))
        {
            return read(_fd, pBuf, sizeToRecv);
        }
    }


    return E_SERIAL_READ;
}

/*****************************************************************************
*                        发送数据
*  函 数 名：Send
*  功    能：发送数据到串口
*  说    明：
*  参    数：
*           pBuf，待发送数据缓冲区，缓冲待发送的数据
*           sizeToSend，待发送数据的字节长度
*  返 回 值：
*           成功，返回发送到串口的字节数，失败返回对应错误码
*  创 建 人：王磊
*  创建时间：2013-10-18
*  修 改 人：
*  修改时间：
*****************************************************************************/
int SerialBase::Send(IN const unsigned char *pBuf, IN const int sizeToSend)
{
    //TO DO
    if(!IsOpen())
    {
        return E_SERIAL_INVALID_PORT;
    }

    if(NULL == pBuf)
    {
        return E_SERIAL_INVALID_PTR;
    }


//单元测试....
#ifdef UT_SERIAL
    return cs.Write(pBuf, sizeToSend);
#endif
//....单元测试

    tcflush(_fd, TCOFLUSH);
    int rtn = write(_fd, pBuf, sizeToSend);
    return rtn;
}

/*****************************************************************************
*                        打开串口
*  函 数 名：Open
*  功    能：打开指定串口
*  说    明：
*  参    数：
*           port，串口端口名称
*  返 回 值：
*           成功打开串口返回E_SERIAL_OK, 其他返回对应错误码
*  创 建 人：王磊
*  创建时间：2013-10-18
*  修 改 人：
*  修改时间：
*****************************************************************************/
int SerialBase::Open(IN const char *port)
{
    //TO DO
    if(NULL == port)
    {
        return E_SERIAL_INVALID_PTR;
    }

    //串口已打开
    if(IsOpen())
    {        
        if(_serialName.compare(port))
        {
            //非指定的串口已经打开，则关闭当前已打开串口
            CloseSerial();
        }
        else
        {
            //指定的串口已打开
            return E_SERIAL_OK;
        }
    }

//单元测试....
#ifdef UT_SERIAL
    _fd = cs.Open();

    return E_SERIAL_OK;
#endif
//....单元测试

    //以非阻塞方式打开串口
    _fd = open(port, O_RDWR | O_NOCTTY | O_NONBLOCK | O_SYNC);
    if(_fd < 0)
    {
        return E_SERIAL_OPEN;
    }

//    //设置文件描述符状态
//    if(fcntl(_fd, F_SETFL, 0) < 0)
//    {
//        return E_SERIAL_SET_FILE_STATUS;
//    }

    //判断当前打开的设备是否终端设备
    if(0 == isatty(_fd))
    {
        return E_SERIAL_NOT_DEV;
    }

    //设置串口端口名称
    SetSerialName(port);

    return E_SERIAL_OK;
}

/*****************************************************************************
*                        设置串口名称
*  函 数 名：SetSerialName
*  功    能：设置串口名称
*  说    明：
*  参    数：
*           pName，串口名称
*  返 回 值：
*           成功，返回E_SERIAL_OK
*  创 建 人：王磊
*  创建时间：2013-10-18
*  修 改 人：
*  修改时间：
*****************************************************************************/
int SerialBase::SetSerialName(IN const char *serialName)
{
    _serialName = serialName;

    return E_SERIAL_OK;
}
