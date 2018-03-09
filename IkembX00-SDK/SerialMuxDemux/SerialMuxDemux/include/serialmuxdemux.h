#pragma once
#include <map>
#include <mutex>
#include<pthread.h>
#include <condition_variable>  // deanji
#include <chrono>              // deanji

#include "../../../common.h"
#include "../../../serial/serial/include/asyncserial.h"
#include "../../../serial/serial/include/basictypes.h"
#include "../../../serial/serial/include/serialfifo.h"
#include "../../../USBAPI/simplelock.h"
#define IN
#define OUT

enum class PriorityLevels
{
	HighPriority,
	LowPriority
};

const int SEND_ONCE_SIZE = 4;
const int SEND_BUFFER_SIZE = 8192;        //发送缓冲区大小
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

    int Send(IN const unsigned char* data, IN const int size, PriorityLevels level = PriorityLevels::LowPriority);
    int SynSend(IN const unsigned char* data, IN const int size, PriorityLevels level = PriorityLevels::LowPriority);  // deanji
    bool AddCallback(IN unsigned char cmd, IN Callback callback);
    bool AddCallback(IN unsigned char cmd, IN Callback callback, OUT int &callbackID);
    bool RemoveCallback(IN unsigned char cmd);
    bool RemoveCallback(IN unsigned char cmd, const int &callbackID);

protected:
    //Protected members
    SerialMuxDemux();
    static void CallbackDemux(IN const unsigned char* data, IN const int size);
    static void* SendToSerial(void* arg);


    int WriteToBuffer(IN const unsigned char* pBuf, IN const int dataSize);

    int ReadFromBuffer(OUT unsigned char* data, IN const int size);

    //liu
    bool _isSendOver;
    void ClearBuffer();
    int WriteToBufferForHighCmd(IN const unsigned char* pBuf, IN const int dataSize);
    int ReadFromBufferHighCmd(OUT unsigned char* data, IN const int size);

    //获取缓冲区状态
    bool IsBufferEmpty();
    bool IsBufferFull();

    //判断callback是否存在
    bool ExistCallback(IN const std::map<int, Callback>& callbacks, IN Callback callback, OUT int& callbackID);

    //获取可用的键值
    int GetUsableKey(const std::map<int, Callback>& callbacks);


private:
    //Private members
    static SerialMuxDemux* s_ins;
    static std::mutex s_mutex;
    static std::map<unsigned char, std::map<int, Callback>> s_callbacks;

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

    //发送优先级高的缓冲处理 --liu
    unsigned char _dataBufHigh[SEND_BUFFER_SIZE];   //环形数据缓冲区
    std::mutex _bufMutexHigh;           //缓冲区读写锁
    int _rdPosHigh;           //数据读出起点
    int _wrPosHigh;           //数据写入起点
    bool _isEmptyHigh;        //数据缓冲区为空状态标志
    bool _isFullHigh;         //数据缓冲区为满状态标志

    bool _enableSend;     //允许向串口发送数据
    pthread_t _thrID;     //发送线程ID

    //liu
    static bool _isSucces;
	//deanji
	bool _isShutdown;	
//public:                             // deanji
	
	
};
