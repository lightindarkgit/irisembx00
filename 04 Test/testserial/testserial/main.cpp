#include <iostream>
#include "gtest/gtest.h"

#include "../../../03 Code/serial/serial/asyncserial.h"
#include "../../../03 Code/serial/serial/basictypes.h"
#include "../../../03 Code/serial/serial//syncserial.h"

using namespace std;
#define TEST_OPEN_SERIAL
#define TEST_CLOSE_SERIAL
#define TEST_SET_CONFIG
#define TEST_GET_CONFIG
#define TEST_GET_FD
#define TEST_RECV
#define TEST_SEND

//异步串口类测试实例
SyncSerial ss;

TEST(TEST_SYNC_SERIAL, OpenSerial)
{
    //TO DO


    //========== Open serial ===============
    TEST_OPEN_SERIAL
    {
        //must open failed
        EXPECT_EQ(E_SERIAL_INVALID_PORT, ss.OpenSerial(-1));
        EXPECT_EQ(E_SERIAL_INVALID_PORT, ss.OpenSerial(7));

        //should success
        for(int port = 0; port < 6; port++)
        {
            EXPECT_EQ(E_SERIAL_OK, ss.OpenSerial(port));
        }

        char* devArr[] = {"ttyS0", "ttyS1", "TTYs2", "ttYusb0", "ttyUSB1"};
        for(int arrOffset = 0; arrOffset < sizeof(devArr) / sizeof(devArr[0]); arrOffset++)
        {
            EXPECT_EQ(E_SERIAL_OK, ss.OpenSerial(devArr[arrOffset]));
        }

        EXPECT_EQ(E_SERIAL_INVALID_PORT, ss.OpenSerial("deVtty4")); //must open failed
    }
}

//=========== Set serial config ===============
TEST(TEST_SYNC_SERIAL, SetConfig)
{
    //TO DO
    SerialConfig serialConfig;

    serialConfig.SerialParityMode = evenParity;
    serialConfig.SerialBaudRate = 19200;
    serialConfig.SerialDataBits = sevenBits;
    serialConfig.SerialStopBits = oneBit;

    EXPECT_EQ(E_SERIAL_OK, ss.SetConfig(serialConfig));
}




//=========== Get serial config ===============
TEST(TEST_SYNC_SERIAL, GetConfig)
{
    SerialConfig sc;
    EXPECT_EQ(E_SERIAL_OK, ss.GetConfig(sc));

    std::cout << "serial config: " << std::endl
              << "    baud rate: " << sc.SerialBaudRate << std::endl
              << "    parity mode: " << sc.SerialParityMode << std::endl
              << "    data bits: " << sc.SerialDataBits << std::endl
              << "    stop bits: " << sc.SerialStopBits << std::endl
              << std::endl;
}

//============ Get serial FD =============
TEST(TEST_SYNC_SERIAL, GetSerialFd)
{
    EXPECT_NE(E_SERIAL_INVALID_PORT, ss.GetSerialFd());
    std::cout << "Serial fd: "<< ss.GetSerialFd() << std::endl << std::endl;
}

//============ Write data to serial =============
TEST(TEST_SYNC_SERIAL, Send)
{
    unsigned char dataToWr[] = {0x0a, 0x01, 0x01, 0x02,
                                0x0b, 0x01, 0x02, 0x03,
                                0x0c, 0x01, 0x02, 0x03,
                                0x0d, 0x01, 0x02, 0x03,
                                0x0e, 0x02, 0x03, 0x01,
                                0x0f, 0x03, 0x02, 0x01};

    int arrSize = sizeof(dataToWr) / sizeof(dataToWr[0]);
    EXPECT_EQ(arrSize, ss.Send(dataToWr, arrSize));

    //        unsigned char* pBuf = dataToWr;

    //        //start sending
    //        for(int offset = 0; offset < arrSize;)
    //        {
    //            EXPECT_EQ(4, syncSerial.Send(pBuf + offset, 4));
    //            offset += 4;
    //        }
}

//====== Read data from serial ================
TEST(TEST_SYNC_SERIAL, Recv)
{
    unsigned char dataRdBuf[9];
    memset(dataRdBuf, 0, 9);

    for(int i = 0; i < 8; i++)
    {
        EXPECT_EQ(4, ss.Recv(dataRdBuf, sizeof(dataRdBuf) / sizeof(dataRdBuf[0])));

        //start receiving
        printf("serial data receive: 0x");
        for(int offset = 0; offset < 4; offset++)
        {
            printf("%02X", dataRdBuf[offset]);
        }
        printf("\n\n");
        fflush(stdout);
    }
}

//=========== Close serial =================
TEST(TEST_SYNC_SERIAL, CloseSerial)
{
    EXPECT_NO_FATAL_FAILURE(ss.CloseSerial());
}

static int counter = 0;

/*****************************************************************************
*                        接收串口数据回调函数
*  函 数 名：RecvSerialCallBack
*  功    能：接收串口数据回调函数
*  说    明：
*  参    数：
*           pBuf,串口数据缓冲区
*           bufSize， 接收的串口字节长度
*  返 回 值：void
*  创 建 人：王磊
*  创建时间：2013-11-08
*  修 改 人：
*  修改时间：
*****************************************************************************/
void RecvSerialCallBack(unsigned char* pBuf, int dataSize)
{
    printf("[Num-%2d] async serial data received: 0x", counter++);
    for(int offset = 0; offset < dataSize;)
    {
         printf("%02X", pBuf[offset++]);
    }
    printf("\n\n");
}


//异步串口类测试实例
AsyncSerial as;

//========== Open serial ===============
TEST(TEST_ASYNC_SERIAL, OpenSerial)
{
    EXPECT_EQ(E_SERIAL_OK, as.OpenSerial("ttyS0", RecvSerialCallBack));
}

//=========== Set serial config ===============
TEST(TEST_ASYNC_SERIAL, SetConfig)
{
    SerialConfig serialConfig;

    serialConfig.SerialParityMode = evenParity;
    serialConfig.SerialBaudRate = 19200;
    serialConfig.SerialDataBits = sevenBits;
    serialConfig.SerialStopBits = oneBit;

    EXPECT_EQ(E_SERIAL_OK, as.SetConfig(serialConfig));
}

//=========== Get serial config ===============
TEST(TEST_ASYNC_SERIAL, GetConfig)
{
    SerialConfig sc;
    EXPECT_EQ(E_SERIAL_OK, as.GetConfig(sc));

    std::cout << "serial config: " << std::endl
              << "    baud rate: " << sc.SerialBaudRate << std::endl
              << "    parity mode: " << sc.SerialParityMode << std::endl
              << "    data bits: " << sc.SerialDataBits << std::endl
              << "    stop bits: " << sc.SerialStopBits << std::endl
              << std::endl;
}

//============ Get serial FD =============
TEST(TEST_ASYNC_SERIAL, GetSerialFd)
{
    EXPECT_NE(E_SERIAL_INVALID_PORT, as.GetSerialFd());
    std::cout << "Serial fd: "<< as.GetSerialFd() << std::endl << std::endl;
}

//====== Read data from serial ================
TEST(TEST_ASYNC_SERIAL, StartRecvAndStopRecv)
{
    unsigned char dataRdBuf[9];
    memset(dataRdBuf, 0, 9);
    EXPECT_EQ(E_SERIAL_OK, as.StartRecv());
    sleep(1);
    EXPECT_EQ(E_SERIAL_OK, as.StopRecv());
}

//============ Write data to serial =============
TEST(TEST_ASYNC_SERIAL, Send)
{
    unsigned char dataToWr[] = {0x0a, 0x01, 0x01, 0x02,
                                0x0b, 0x01, 0x02, 0x03,
                                0x0c, 0x01, 0x02, 0x03,
                                0x0d, 0x01, 0x02, 0x03,
                                0x0e, 0x02, 0x03, 0x01,
                                0x0f, 0x03, 0x02, 0x01,
                                0x0d, 0x05};
    int arrSize = sizeof(dataToWr) / sizeof(dataToWr[0]);
    EXPECT_EQ(arrSize, as.Send(dataToWr, arrSize));
//        unsigned char* pBuf = dataToWr;

//        //start sending
//        for(int offset = 0; offset < arrSize;)
//        {
//            EXPECT_EQ(4, as.Send(pBuf + offset, 4));
//            offset += 4;
//        }
}

//=========== Close serial =================
TEST(TEST_ASYNC_SERIAL, Close)
{
    EXPECT_NO_FATAL_FAILURE(as.CloseSerial());
}


int main(int argc, char* argv[])
{
    testing::InitGoogleTest(&argc, argv);
    cout << "Hello World!" << endl;


    return RUN_ALL_TESTS();
}
