#pragma once
#include <map>
#include <mutex>
#include<pthread.h>

//#include "../../common.h"
#define IN
#define OUT

const int SEND_ONCE_SIZE = 4;
const int SEND_BUFFER_SIZE = 1024;        //发送缓冲区大小
const int TOTAL_TRY_COUNT = 5;            //发送失败后允许最大重发次数
typedef void (*Callback)(IN const unsigned char* data, IN const int size);

struct SerialConfig;            //串口配置类型声明
class AsyncSerial;              //异步串口类声明

class /*SERIALMUXDEMUXSHARED_EXPORT*/ SerialMuxDemux {
public:

    ~SerialMuxDemux();
    static SerialMuxDemux* GetInstance();
    static void ReleaseInstance();

    int Init(IN const char* port = NULL, IN SerialConfig* cfg = NULL);

    int Send(IN const unsigned char* data, IN const int size);
    bool AddCallback(IN unsigned char cmd, IN Callback callback);
    bool RemoveCallback(IN unsigned char cmd);

protected:
    //Protected members
    SerialMuxDemux();
    static void CallbackDemux(IN const unsigned char* data, IN const int size);
    static void* SendToSerial(void* arg);


    int WriteToBuffer(IN const unsigned char* pBuf, IN const int dataSize);
    int ReadFromBuffer(OUT unsigned char* data, IN const int size);

    //获取缓冲区状态
    bool IsBufferEmpty();
    bool IsBufferFull();


private:
    //Private members
    static SerialMuxDemux* s_ins;
    static std::mutex s_mutex;
    static std::map<unsigned char, Callback> s_callbacks;

    static std::mutex s_refMutex;
    static int s_refCount;

    bool _hasInited;
    AsyncSerial* _asyncSerial;


    //发送缓冲处理
    unsigned char _dataBuf[SEND_BUFFER_SIZE];   //环形数据缓冲区
    std::mutex _bufMutex;           //缓冲区读写锁
    int _rdPos;           //数据读出起点
    int _wrPos;           //数据写入起点
    bool _isEmpty;        //数据缓冲区为空状态标志
    bool _isFull;         //数据缓冲区为满状态标志

    bool _enableSend;     //允许向串口发送数据
    pthread_t _thrID;     //发送线程ID
};
