#include <memory>
#include <string>
#include <string.h>
#include <unistd.h>
#include "../include/serialmuxdemux.h"
#include "../../../serial/serial/include/basictypes.h"
#include "../../../serial/serial/include/asyncserial.h"
#include <pthread.h>
#include <errno.h>
#include <sys/time.h>
#include <time.h>
#include <unistd.h>

//#define DEBUG
//
//-----test head files
#include <stdio.h>

using namespace std;

//static bool SerialMuxDemux::hasInited = false;
SerialMuxDemux* SerialMuxDemux::s_ins = NULL;
std::mutex SerialMuxDemux::s_mutex;
std::map<unsigned char, std::map<int, Callback>> SerialMuxDemux::s_callbacks;
std::mutex SerialMuxDemux::s_refMutex;
int SerialMuxDemux::s_refCount = 0;
//static AsyncSerial* SerialMuxDemux::_asyncSerial = NULL;
bool SerialMuxDemux::_isSucces = false;
static ThreadWaiter  s_threadWaiter;
//bool SerialMuxDemux::_threadWaiter = new ThreadWaiter();
//add by deanji
unsigned char currcmdsending = 0 ;  
std::mutex asynsend_mutex;
std::condition_variable asynsend_condvar;	

pthread_mutex_t _asmutex;
pthread_cond_t _ascond;
bool _assignaled = false;
static ThreadWaiter asend_threadWaiter;


SerialMuxDemux::SerialMuxDemux():
    _hasInited(false),
    _asyncSerial(NULL),
    _rdPos(0),
    _wrPos(0),
    _isEmpty(true),
    _isFull(false),
    _rdPosHigh(0),
    _wrPosHigh(0),
    _isEmptyHigh(true),
    _isFullHigh(false),
    _isSendOver(true),
    _enableSend(false),
	_isShutdown(false)
	
{
    //TO DO
    s_refCount = 0;
    s_callbacks.clear();

    memset(_dataBuf, 0, SEND_BUFFER_SIZE);
    memset(_dataBufHigh, 0, SEND_BUFFER_SIZE);
    _asyncSerial = new AsyncSerial();
}

SerialMuxDemux::~SerialMuxDemux()
{
    //TO DO
    if(NULL != _asyncSerial)
    {
        std::cout << "[SerialMuxDemux]: closing serial..." << std::endl;
        if(true == _enableSend)
        {
            _enableSend = false;       //终止发送
            pthread_join(_thrID, NULL);
        }

        int serialClosed = s_ins->_asyncSerial->StopRecv();
        std::cout << "[SerialMuxDemux]:" << (E_SERIAL_OK == serialClosed ? "serial is closed successfully!" : "failed to close serial!!!")
                  << std::endl;

        delete _asyncSerial;
    }
}

SerialMuxDemux* SerialMuxDemux::GetInstance()
{
    if(NULL == s_ins)
    {
        std::lock_guard<mutex> lockGuard(s_mutex);
        if(NULL == s_ins)
        {
            s_ins = new SerialMuxDemux();
        }
    }

    //更新实例引用计数
    std::lock_guard<mutex> lockGurad(s_refMutex);
    s_refCount++;

    return s_ins;
}

void SerialMuxDemux::ReleaseInstance()
{
    //TO DO
    std::lock_guard<mutex> lockGurad(s_refMutex);
    if(0 == --s_refCount)
    {
        delete s_ins;
        s_ins = NULL;
    }
}

int SerialMuxDemux::Init(const char* port, SerialConfig* cfg)
{
    //TO DO
    //仅初始化一次
    if(_hasInited)
    {
        return E_SERIAL_OK;
    }


    //判断串口是否有效
    if(nullptr == _asyncSerial)
    {
    	return E_SERIAL_INVALID_PORT;
    }


    std::string sPort(port);
    //打开和配置串口
    if(NULL == port)
    {
        //return E_SERIAL_INVALID_PORT;
    	sPort = "ttys1";
    }



    //打开串口，并注册串口数据分发回调函数
    int rtnVal =  _asyncSerial->OpenSerial(sPort.c_str(), CallbackDemux);
    if(rtnVal != E_SERIAL_OK)
    {
        return rtnVal;
    }

    //配置串口
    SerialConfig serialCfg;
    if(nullptr != cfg)
    {
        serialCfg = *cfg;
    }

    rtnVal = _asyncSerial->SetConfig(serialCfg);
//    printf("Serial Config: %d %d %d %d\n",serialCfg.SerialBaudRate, serialCfg.SerialParityMode, serialCfg.SerialDataBits, serialCfg.SerialStopBits);

    if(rtnVal != E_SERIAL_OK)
    {
        return rtnVal;
    }

    rtnVal = _asyncSerial->StartRecv();
    if(rtnVal != E_SERIAL_OK)
    {
        return rtnVal;
    }


    //启动发送线程
    _enableSend = true;
    rtnVal = pthread_create(&_thrID, NULL, SendToSerial, this);
    if(E_SERIAL_OK != rtnVal)
    {
        return rtnVal;
    }

    _hasInited = true;

	pthread_mutex_init(&_asmutex, NULL);   // deanji
    pthread_cond_init(&_ascond, NULL); // deanji
    _assignaled = false;
	
    return  rtnVal;
}


int SerialMuxDemux::Send(const unsigned char *data, const int size , PriorityLevels level)
{
// deanji
	unsigned char ledoff[4]={0x07,0x00,0x00,0x00};
	unsigned char cameraoff[4]={0x05,0x00,0x00,0x00};    


//TO DO
    if(_hasInited)
    {
#ifdef DEBUG
        printf("sending data: 0x");
        for(int i = 0; i < size; i++)
        {
            printf("%02X", data[i]);
        }
        printf("\n");
#endif

        std::lock_guard<mutex> lockGuard(s_mutex);


		if(data[0]==0xFF)   // 关机
		{
			_isShutdown=true;
			_isSendOver = false;
                	ClearBuffer();
			std::cout<<"写命令到缓冲区"<<"0x07"<<std::endl;
			WriteToBufferForHighCmd(ledoff, 4);
			std::cout<<"写命令到缓冲区"<<"0x05"<<std::endl;
			WriteToBufferForHighCmd(cameraoff, 4);
        		return size;	
		}
		else if(data[0]==0xFE)  //开机
		{
			_isShutdown=false;
			return size;
		}

		if(_isShutdown)   // 
		{
			if((data[0]==0x0B) || (data[0]==0x0A))
			{
				if(level == PriorityLevels::LowPriority)
	        		{
            				if(!_isSendOver)
        	  			{
        	  	 		std::cout<<"忽略下发串口请求！"<<std::endl;
        	   			return E_SERIAL_CANCEL;
           				}
					std::cout<<"写命令到缓冲区"<<data[0]<<std::endl;
           				return WriteToBuffer(data, size);
        			}
        			else
        			{
        				_isSendOver = false;
        				ClearBuffer();
					std::cout<<"写命令到缓冲区"<<data[0]<<std::endl;
        				return WriteToBufferForHighCmd(data, size);
        			}
			}
			else
			{
				std::cout<<"丢弃命令"<<data[0]<<std::endl;
				return size;
			}		
		}
		else
		{
			//liu
        	if(level == PriorityLevels::LowPriority)
        	{
            	if(!_isSendOver)
          	 	{
        	  	 	std::cout<<"忽略下发串口请求！"<<std::endl;
        	   		return E_SERIAL_CANCEL;
           		}
			std::cout<<"写命令到缓冲区"<<data[0]<<std::endl;
           		return WriteToBuffer(data, size);
        	}
        	else
        	{
        		_isSendOver = false;
        		ClearBuffer();
			std::cout<<"写命令到缓冲区"<<data[0]<<std::endl;
        		return WriteToBufferForHighCmd(data, size);
        	}
		}        
    }

    return E_SERIAL_INVALID_PTR;
}

// add by deanji 20170209
int SerialMuxDemux::SynSend(const unsigned char *data, const int size , PriorityLevels level)
{
// deanji
	unsigned char datatmp[20];
	int rlt;
	struct timeval now;
	struct timespec timeOut;
	unsigned char ledoff[4]={0x07,0x00,0x00,0x00};
	unsigned char cameraoff[4]={0x05,0x00,0x00,0x00}; 

	for(int m = 0; m<size ;m++)
	{
		datatmp[m] = data[m];
		if(m>10) break;
	}
	datatmp[3] = 0xbe;   //  Synsend flag  (diff from Send)
	if(datatmp[0]==0x0A) return E_SERIAL_CANCEL;

//TO DO
    if(_hasInited)
    {
#ifdef DEBUG
        printf("sending data: 0x");
        for(int i = 0; i < size; i++)
        {
            printf("%02X", data[i]);
        }
        printf("\n");
#endif

//        std::lock_guard<mutex> lockGuard(s_mutex);
//        �����ȥ�����_asmutex �����������!

		if(data[0]==0xFF)   // 关机
		{
			_isShutdown=true;
			_isSendOver = false;
                	ClearBuffer();
			std::cout<<"写命令到缓冲区"<<"0x07"<<std::endl;
			WriteToBufferForHighCmd(ledoff, 4);
			std::cout<<"写命令到缓冲区"<<"0x05"<<std::endl;
			WriteToBufferForHighCmd(cameraoff, 4);
			usleep(50000);
        	return E_SERIAL_OK;	
		}
		else if(data[0]==0xFE)  //开机
		{
			_isShutdown=false;
			return E_SERIAL_OK;	
		}

		if(_isShutdown)   // 
		{
			if((data[0]==0x0B) || (data[0]==0x0A))
			{				
				if(level == PriorityLevels::LowPriority)
	        	{
            		if(!_isSendOver)
        	  		{
        	  	 		std::cout<<"忽略下发串口请求！"<<std::endl;
        	   			return E_SERIAL_CANCEL;
           			}
					std::cout<<"写命令到缓冲区"<<data[0]<<std::endl;
					currcmdsending = data[0];
					rlt = WriteToBuffer(data, size);
					if(rlt != size)  return E_SERIAL_CANCEL;
					gettimeofday(&now, NULL);
					timeOut.tv_sec = now.tv_sec + 2;
					timeOut.tv_nsec = now.tv_usec * 1000;

					int ret = 0;
					ret = asend_threadWaiter.wait(timeOut);
					currcmdsending = 0;
					if(ret)
					{						
						printf(" \n synsend OK! \n");
						return E_SERIAL_OK;
					}
					else
					{
						printf(" \n  pthread_cond_timedwait timeout \n");
						return E_SERIAL_CANCEL;
					}

/*					
					pthread_mutex_lock(&_asmutex);
					int ret = 0;
					ret = pthread_cond_timedwait(&_ascond, &_asmutex,&timeOut);
					currcmdsending = 0;
					if(ret == ETIMEDOUT)
            		{
                 		pthread_mutex_unlock(&_asmutex);
						printf(" \n  pthread_cond_timedwait timeout \n");
                		return E_SERIAL_CANCEL;
            		}
					else
					{
						pthread_mutex_unlock(&_asmutex);
						printf(" \n asynsend OK! \n");
						return E_SERIAL_OK;
					}
*/
        		}
        		else
        		{
        			_isSendOver = false;
        			ClearBuffer();
					std::cout<<"写命令到缓冲区"<<data[0]<<std::endl;
					currcmdsending = data[0];	
        			rlt = WriteToBufferForHighCmd(data, size);
					if(rlt != size)  return E_SERIAL_CANCEL;
					gettimeofday(&now, NULL);
					timeOut.tv_sec = now.tv_sec + 2;
					timeOut.tv_nsec = now.tv_usec * 1000;
					pthread_mutex_lock(&_asmutex);
					int ret = 0;
					ret = pthread_cond_timedwait(&_ascond, &_asmutex,&timeOut);

					if(ret == ETIMEDOUT)
            		{
                 		pthread_mutex_unlock(&_asmutex);
						printf(" \n  pthread_cond_timedwait timeout \n");
                		return E_SERIAL_CANCEL;
            		}
					else
					{
						pthread_mutex_unlock(&_asmutex);
						printf(" \n asynsend OK! \n");
						return E_SERIAL_OK;
					}
        		}
			}
			else
			{
//				std::cout<<"丢弃命令"<<data[0]<<std::endl;
				return E_SERIAL_CANCEL;
			}		
		}
		else
		{
			//liu
        	if(level == PriorityLevels::LowPriority)
        	{
            	if(!_isSendOver)
          	 	{
        	  	 	std::cout<<"忽略下发串口请求！"<<std::endl;
        	   		return E_SERIAL_CANCEL;
           		}
				std::cout<<"写命令到缓冲区"<<data[0]<<std::endl;
     
					currcmdsending = data[0];
					rlt = WriteToBuffer(datatmp, size);
					if(rlt != size)  return E_SERIAL_CANCEL;				

					gettimeofday(&now, NULL);
					timeOut.tv_sec = now.tv_sec + 2;
					timeOut.tv_nsec = now.tv_usec * 1000;
					
//					timeOut.tv_sec = time(nullptr);
//               	timeOut.tv_sec += 3;
					int ret = 0;
					ret = asend_threadWaiter.wait(timeOut);
					currcmdsending = 0;
					if(ret)
					{						
						printf(" \n synsend OK! \n");
						return E_SERIAL_OK;
					}
					else
					{
						printf(" \n  pthread_cond_timedwait timeout \n");
						return E_SERIAL_CANCEL;
					}


/*					pthread_mutex_lock(&_asmutex);
					
					printf("begin pthread_cond_timedwait \n");
					ret = pthread_cond_timedwait(&_ascond, &_asmutex,&timeOut);
					printf("end pthread_cond_timedwait \n");
//					currcmdsending = 0;
					if(ret == ETIMEDOUT)
            		{
                 		pthread_mutex_unlock(&_asmutex);
						printf(" \n  pthread_cond_timedwait timeout \n");
                		return E_SERIAL_CANCEL;
            		}
					else
					{
						pthread_mutex_unlock(&_asmutex);
						printf(" \n synsend OK! \n");
						return E_SERIAL_OK;
					}
					*/
        	}
        	else
        	{
        		_isSendOver = false;
        		ClearBuffer();
				std::cout<<"写命令到缓冲区"<<data[0]<<std::endl;
					currcmdsending = data[0];
					rlt = WriteToBufferForHighCmd(datatmp, size);
					
					if(rlt != size)  return E_SERIAL_CANCEL;
					
					gettimeofday(&now, NULL);
					timeOut.tv_sec = now.tv_sec + 2;
					timeOut.tv_nsec = now.tv_usec * 1000;
					pthread_mutex_lock(&_asmutex);
					int ret = 0;
					ret = pthread_cond_timedwait(&_ascond, &_asmutex,&timeOut);
//					currcmdsending = 0;
					if(ret == ETIMEDOUT)
            		{
                 		pthread_mutex_unlock(&_asmutex);
						printf(" \n  pthread_cond_timedwait timeout \n");
                		return E_SERIAL_CANCEL;
            		}
					else
					{
						pthread_mutex_unlock(&_asmutex);
						printf(" \n asynsend OK! \n");
						return E_SERIAL_OK;
					}
        	}
		}        
    }
	
    return E_SERIAL_INVALID_PTR;
}


void SerialMuxDemux::ClearBuffer()
{
	std::lock_guard<std::mutex> lock(_bufMutex);
	_rdPos = _wrPos;
	_isEmpty = true;


}
bool SerialMuxDemux::AddCallback(unsigned char cmd, Callback callback)
{
    //TO DO
#ifdef DEBUG
    printf("add callback cmd: 0x%x\n", cmd);
#endif

    int callbackID;
    return AddCallback(cmd, callback, callbackID);
//    if(ExistCallback(s_callbacks[cmd], callback, callbackID))
//    {
//    	printf("[%s@SerialMuxDemux]: this callback has alread enrolled\n", __FUNCTION__);
//    	return true;
//    }
//
//    callbackID = GetUsableKey(s_callbacks[cmd]);
//    std::lock_guard<mutex> lockGuard(s_mutex);
//    s_callbacks[cmd][callbackID] = callback;
//
//    printf("[%s@SerialMuxDemux]: add a new callback with: cmd = 0x%02X  callbackid = %d\n", __FUNCTION__, cmd, callbackID);

//    return s_callbacks.count(cmd) > 0 && s_callbacks[cmd].count(callbackID) > 0;
}

bool SerialMuxDemux::AddCallback(IN unsigned char cmd, IN Callback callback, OUT int &callbackID)
{
#ifdef DEBUG
    printf("add callback cmd: 0x%x\n", cmd);
#endif

    if(ExistCallback(s_callbacks[cmd], callback, callbackID))
    {
    	printf("[%s@SerialMuxDemux]: this callback has alread enrolled\n", __FUNCTION__);
    	return true;
    }

    callbackID = GetUsableKey(s_callbacks[cmd]);
    std::lock_guard<mutex> lockGuard(s_mutex);
    s_callbacks[cmd][callbackID] = callback;
    printf("[%s@SerialMuxDemux]: add a new callback with: cmd = 0x%02X  callbackid = %d\n", __FUNCTION__, cmd, callbackID);

    return s_callbacks.count(cmd) > 0 && s_callbacks[cmd].count(callbackID) > 0;
}

bool SerialMuxDemux::RemoveCallback(unsigned char cmd)
{
    if(s_callbacks.count(cmd) > 0)
    {
        std::lock_guard<mutex> lockGuard(s_mutex);
        s_callbacks.erase(cmd);
    }

    return true;
}

bool SerialMuxDemux::RemoveCallback(IN unsigned char cmd, const int &callbackID)
{
    if(s_callbacks.count(cmd) > 0)
    {
    	if(s_callbacks[cmd].count(callbackID) > 0)
    	{
    		std::lock_guard<mutex> lockGuard(s_mutex);
    		s_callbacks[cmd].erase(callbackID);

    		if(s_callbacks[cmd].empty())
    		{
    			s_callbacks.erase(cmd);
    		}
    	}
    }

    return true;
}

void SerialMuxDemux::CallbackDemux(const unsigned char *data, const int size)
{
    //TO DO
    if(data != NULL && size > 0)
    {
#ifdef DEBUG
        printf("[serial received]: 0x");
        for(int i = 0; i < size; i++)
        {
            printf("%02X", data[i]);
        }
        printf("\n");

        struct timeval tv;
        gettimeofday(&tv,NULL);
        std::cout<<"tv_sec :"<<tv.tv_sec<<" tv_usec :"<<tv.tv_usec<<endl;
#endif

        //liu
        if((data[size -1] & 0x01) == 0 && (data[size -1] & 0x02) == 0)
        {
 //       	std::cout<<"串口回应成功,发送信号，命令："<<hex<<data[0]<<std::endl;
 			printf("���ڷ��سɹ�:%d\n",data[0]);
            _isSucces = true;
            s_threadWaiter.signal();

        }
        else if((data[size -1] & 0x01)  == 1 && (data[size -1] & 0x02) == 0)
        {
        	std::cout<<"串口回应失败，发送信号， 返回，！串口命令："<<hex<<data[0]<<std::endl;
        	_isSucces = false;
        	s_threadWaiter.signal();
        	return;
        }

        if(data[0] == 0x0b || data[0] == 0x0a)
        {
            printf("[serial received]: 0x");
            for(int i = 0; i < size; i++)
            {
                printf("%02X", data[i]);
            }
            printf("\n");

        	//unsigned char temp = data[0];
        	//std::cout<<"串口回应失败！串口命令："<<temp;
        	//temp = data[size -1];
            //std::cout<<"回应位信息："<<temp<<std::endl;
        }


        //wait

		// add by deanji  				


				if( (data[0] == currcmdsending) && (data[3] == 0xbe) )				
				{
//					printf("signal emit begin \n");
//					pthread_mutex_lock(&_asmutex);
//        			_assignaled = true;
//       			pthread_cond_signal(&_ascond);
//       			pthread_mutex_unlock(&_asmutex);
 					asend_threadWaiter.signal();
//					printf("pthread_cond_signal emited \n");
				}


        if(s_callbacks.count(data[0]) > 0)
        {
        	std::lock_guard<mutex> lockGuard(s_mutex);
        	std::map<int, Callback>::iterator iter = s_callbacks[data[0]].begin();
        	std::map<int, Callback>::iterator endIter = s_callbacks[data[0]].end();

        	for(;iter != endIter; ++iter)
        	{
//        		iter->second(data + 1, size - 3);
//deanji
				if((data[0]==0x0c) || (data[0]==0x0f) || (data[0]==0x08))
				{
					iter->second(data + 1, size - 3);					
				}
				else
				{
					iter->second(data + 1, size-1);
				}				
        	}
        }
    }
}


int SerialMuxDemux::WriteToBuffer(const unsigned char *pBuf, const int dataSize)
{
    //TO DO
    //TO DO
    if(dataSize <= 0)
    {
        return E_SERIAL_NO_DATA;
    }

    //在写入缓冲区之前对缓冲区锁定
    std::lock_guard<std::mutex> lock(_bufMutex);
    //暂存读写标志
    int tWrPos = _wrPos;
    int tRdPos = _rdPos;

    //写入新数据
    int leftCount = SEND_BUFFER_SIZE - _wrPos;    //从写入起点到缓冲区的结尾的字节长度
    if(leftCount > dataSize)
    {
        memcpy(_dataBuf + _wrPos, pBuf, dataSize);
        _wrPos = (_wrPos + dataSize) % SEND_BUFFER_SIZE;   //更新写入起点
    }
    else
    {
        memcpy(_dataBuf + _wrPos, pBuf, leftCount);
        _wrPos = dataSize - leftCount;
        memcpy(_dataBuf, pBuf + leftCount, _wrPos);
    }


    if(_isFull)     //缓冲区满
    {
        //TO DO
        _rdPos = _wrPos;   //更新读出起点
        printf("[%s@SerialMuxDemux]: data buffer for sent to serial is full, some unsent data will be lost\n", __FUNCTION__);
    }

    else if(tWrPos == tRdPos)    //缓冲区空
    {
        //TO DO
        _isFull = (_wrPos == _rdPos);    //更新缓冲区满标志
        _isEmpty = false;
    }
    else if(tWrPos < tRdPos)
    {
        //TO DO
        if(leftCount > dataSize)
        {
            _isFull = _wrPos >= _rdPos;
            _rdPos =  _isFull ? _wrPos : _rdPos;
        }
        else
        {
            _isFull = true;
            _rdPos = _wrPos;

        }
    }
    else if(tWrPos > tRdPos)
    {
        //TO DO
        if(leftCount > dataSize)
        {
            _isFull = false;
        }
        else
        {
            _isFull = _wrPos >= _rdPos;
            _rdPos = _isFull ? _wrPos : _rdPos;
        }
    }


    return dataSize;
}

//liu
int SerialMuxDemux::WriteToBufferForHighCmd(const unsigned char *pBuf, const int dataSize)
{
    //TO DO
    //TO DO
    if(dataSize <= 0)
    {
        return E_SERIAL_NO_DATA;
    }

    //在写入缓冲区之前对缓冲区锁定
    std::lock_guard<std::mutex> lock(_bufMutexHigh);
    //暂存读写标志
    int tWrPos = _wrPosHigh;
    int tRdPos = _rdPosHigh;

    //写入新数据
    int leftCount = SEND_BUFFER_SIZE - _wrPosHigh;    //从写入起点到缓冲区的结尾的字节长度
    if(leftCount > dataSize)
    {
        memcpy(_dataBufHigh + _wrPosHigh, pBuf, dataSize);
        _wrPosHigh = (_wrPosHigh + dataSize) % SEND_BUFFER_SIZE;   //更新写入起点
    }
    else
    {
        memcpy(_dataBufHigh + _wrPosHigh, pBuf, leftCount);
        _wrPosHigh = dataSize - leftCount;
        memcpy(_dataBufHigh, pBuf + leftCount, _wrPosHigh);
    }


    if(_isFullHigh)     //缓冲区满
    {
        //TO DO
        _rdPosHigh = _wrPosHigh;   //更新读出起点
        printf("[%s@SerialMuxDemux]: data buffer for sent to serial is full, some unsent data will be lost\n", __FUNCTION__);
    }

    else if(tWrPos == tRdPos)    //缓冲区空
    {
        //TO DO
        _isFullHigh = (_wrPosHigh == _rdPosHigh);    //更新缓冲区满标志
        _isEmptyHigh = false;
    }
    else if(tWrPos < tRdPos)
    {
        //TO DO
        if(leftCount > dataSize)
        {
            _isFullHigh = _wrPosHigh >= _rdPosHigh;
            _rdPosHigh =  _isFullHigh ? _wrPosHigh : _rdPosHigh;
        }
        else
        {
            _isFullHigh = true;
            _rdPosHigh = _wrPosHigh;

        }
    }
    else if(tWrPos > tRdPos)
    {
        //TO DO
        if(leftCount > dataSize)
        {
            _isFullHigh = false;
        }
        else
        {
            _isFullHigh = _wrPosHigh >= _rdPosHigh;
            _rdPosHigh = _isFullHigh ? _wrPosHigh : _rdPosHigh;
        }
    }
    return dataSize;
}

int SerialMuxDemux::ReadFromBuffer(unsigned char *data, const int size)
{
    //TO DO
    if(NULL == data)
    {
        return E_SERIAL_INVALID_PTR;
    }

    std::lock_guard<std::mutex> lock(_bufMutex);
    if(IsBufferEmpty())   //_wrPos等于_rdPos
    {
        return E_SERIAL_NO_DATA;
    }

    if(_wrPos > _rdPos)    //_wrPos大于_rdPos
    {
        if(_wrPos - _rdPos >= size)
        {
            memcpy(data, _dataBuf + _rdPos, size);
            _rdPos = (_rdPos + size) % SEND_BUFFER_SIZE;
            _isEmpty = (_rdPos == _wrPos);
#ifdef DEBUG
            printf("[%c]write_pos: %d\t read_pos: %d\n", _wrPos >= _rdPos ? 'T' : 'F', _wrPos, _rdPos);
#endif
        }
        else
        {
            return E_SERIAL_NO_DATA;
        }
    }
    else      //_wrPos小于等于_rdPos
    {
        if(_wrPos + SEND_BUFFER_SIZE - _rdPos < size)
        {
            return E_SERIAL_NO_DATA;
        }

        int sizeToBufEnd = SEND_BUFFER_SIZE - _rdPos;
        if(sizeToBufEnd >= size)
        {
            memcpy(data, _dataBuf + _rdPos, size);
            _rdPos = (_rdPos + size) % SEND_BUFFER_SIZE;
            _isEmpty = (_rdPos == _wrPos);

#ifdef DEBUG
            printf("[%c]write_pos: %d\t read_pos: %d\n", _wrPos >= _rdPos ? 'T' : 'F', _wrPos, _rdPos);
#endif
        }
        else
        {
            memcpy(data, _dataBuf + _rdPos, sizeToBufEnd);
            memcpy(data + sizeToBufEnd, _dataBuf, size - sizeToBufEnd);
            _rdPos = size - sizeToBufEnd;
            _isEmpty = (_rdPos == _wrPos);

#ifdef DEBUG
            printf("[%c]write_pos: %d\t read_pos: %d\n", _wrPos >= _rdPos ? 'T' : 'F', _wrPos, _rdPos);
#endif
        }
    }

    if(true == _isFull)
    {
        _isFull = false;
    }
#ifdef DEBUG
    fflush(stdout);
#endif

    return size;
}

int SerialMuxDemux::ReadFromBufferHighCmd(unsigned char *data, const int size)
{
    //TO DO
    if(NULL == data)
    {
        return E_SERIAL_INVALID_PTR;
    }

    std::lock_guard<std::mutex> lock(_bufMutexHigh);
    if(_isEmptyHigh)   //_wrPos等于_rdPos
    {
        return E_SERIAL_NO_DATA;
    }

    if(_wrPosHigh > _rdPosHigh)    //_wrPos大于_rdPos
    {
        if(_wrPosHigh - _rdPosHigh >= size)
        {
            memcpy(data, _dataBufHigh + _rdPosHigh, size);
            _rdPosHigh = (_rdPosHigh + size) % SEND_BUFFER_SIZE;
            _isEmptyHigh = (_rdPosHigh == _wrPosHigh);
#ifdef DEBUG
            printf("[%c]write_pos: %d\t read_pos: %d\n", _wrPos >= _rdPos ? 'T' : 'F', _wrPos, _rdPos);
#endif
        }
        else
        {
        	_isEmptyHigh = true;
            return E_SERIAL_NO_DATA;
        }
    }
    else      //_wrPos小于等于_rdPos
    {
        if(_wrPosHigh + SEND_BUFFER_SIZE - _rdPosHigh < size)
        {
            return E_SERIAL_NO_DATA;
        }

        int sizeToBufEnd = SEND_BUFFER_SIZE - _rdPosHigh;
        if(sizeToBufEnd >= size)
        {
            memcpy(data, _dataBufHigh + _rdPosHigh, size);
            _rdPosHigh = (_rdPosHigh + size) % SEND_BUFFER_SIZE;
            _isEmptyHigh = (_rdPosHigh == _wrPosHigh);

#ifdef DEBUG
            printf("[%c]write_pos: %d\t read_pos: %d\n", _wrPos >= _rdPos ? 'T' : 'F', _wrPos, _rdPos);
#endif
        }
        else
        {
            memcpy(data, _dataBufHigh + _rdPosHigh, sizeToBufEnd);
            memcpy(data + sizeToBufEnd, _dataBufHigh, size - sizeToBufEnd);
            _rdPosHigh = size - sizeToBufEnd;
            _isEmptyHigh = (_rdPosHigh == _wrPosHigh);

#ifdef DEBUG
            printf("[%c]write_pos: %d\t read_pos: %d\n", _wrPos >= _rdPos ? 'T' : 'F', _wrPos, _rdPos);
#endif
        }
    }

    if(true == _isFullHigh)
    {
        _isFullHigh = false;
    }
#ifdef DEBUG
    fflush(stdout);
#endif

    return size;
}

void* SerialMuxDemux::SendToSerial(void * arg)
{
    //TO DO
    SerialMuxDemux* sm = static_cast<SerialMuxDemux *>(arg);
    unsigned char sendBuf[SEND_ONCE_SIZE];
#ifdef DEBUG
    printf("SerialMuxDemux发送线程启动\n");
#endif

    //超时时间
    timespec tim;
//    tim.tv_sec = time(nullptr);
//    tim.tv_nsec = 500;
//    tim.tv_sec += 3;

    int tryCount = 0;
    while(sm->_enableSend)
    {
    	//liu 优先级高的串口命令
        if(SEND_ONCE_SIZE == sm->ReadFromBufferHighCmd(sendBuf, SEND_ONCE_SIZE))
        {
        	int count =0;
        	std::cout<<"优先级高的请求下发！rdPosHigh:"<<sm->_rdPosHigh<<" wrPosHigh:"<<sm->_wrPosHigh<<std::endl;
        	//基于串口数据传输速度限制，在连续发送数据的时候需要短暂休眠防止数据溢出
        	usleep(50000);
			//deanji
			if((sendBuf[0] < 5) || (sendBuf[0] == 9) || (sendBuf[0]>0x0F))
			{
				sm->_rdPosHigh = 0;
				sm->_wrPosHigh = 0;
				sm->_isEmptyHigh = true;
				sm->_isFullHigh = false;
			}

			
        	while(true)
        	{
        		//下发是否成功
        		sm->_isSucces = false;
        		count++;
               //如果当前发送缓冲区中还有未发送的数据
               while(SEND_ONCE_SIZE != sm->_asyncSerial->Send(sendBuf, SEND_ONCE_SIZE) && ++tryCount <= TOTAL_TRY_COUNT)
               {
                 //处理发送失败的情况
                  std::string failedCmd("");
                  for(int iter = 0; iter < SEND_ONCE_SIZE; iter++)
                  {
                    failedCmd += sendBuf[iter];
                  }

                  printf("[failed to send]: %s\n", failedCmd.c_str());
                  usleep(35000);
           	  	}

               //设置超时时间--为5秒
               tim.tv_sec = time(nullptr);
               tim.tv_sec += 3;
                if(!s_threadWaiter.wait(tim))
                {
                	std::cout<<"串口下发超时！ tim "<<tim.tv_sec<<std::endl;
                }

                //
                if(sm->_isSucces || count > 4)
             	{
                	std::cout<<"下发是否成功："<<sm->_isSucces <<" 下发次数："<<count<<std::endl;
            		break;
                }

              }

            continue;
        }


        sm->_isSendOver = true;
        if(SEND_ONCE_SIZE == sm->ReadFromBuffer(sendBuf, SEND_ONCE_SIZE))
        {
        	std::cout<<"优先级低的请求下发！rdPos:"<<sm->_rdPos<<" wrPos:"<<sm->_wrPos<<std::endl;
            //如果当前发送缓冲区中还有未发送的数据
			if((sendBuf[0] < 5) || (sendBuf[0] == 9) || (sendBuf[0]>0x0F))
			{
				sm->_rdPos = 0;
				sm->_wrPos = 0;
				sm->_isEmpty = true;
				sm->_isFull = false;
			}
            while(SEND_ONCE_SIZE != sm->_asyncSerial->Send(sendBuf, SEND_ONCE_SIZE) && ++tryCount <= TOTAL_TRY_COUNT)
            {
                //处理发送失败的情况
                std::string failedCmd("");
                for(int iter = 0; iter < SEND_ONCE_SIZE; iter++)
                {
                    failedCmd += sendBuf[iter];
                }

                printf("[failed to send]: %s\n", failedCmd.c_str());
                usleep(35000);
            }
#ifdef DEBUG
            printf("[%lu]发送线程\n", pthread_self());
#endif
            //基于串口数据传输速度限制，在连续发送数据的时候需要短暂休眠防止数据溢出
            usleep(35000);
        }
        else
        {
            //线程轮换调度，避免死循环问题
            usleep(1000);
        }
    }

    return (void *)0;
}


bool SerialMuxDemux::IsBufferEmpty()
{
    //TO DO
    return _isEmpty;
}

bool SerialMuxDemux::IsBufferFull()
{
    //TO DO
    return _isFull;
}


int SerialMuxDemux::GetUsableKey(const std::map<int, Callback>& callbacks)
{
	int i ;

	for(i = 0; ; i++)
	{
		if(0 == callbacks.count(i))
		{
			break;
		}
	}

	return i;
}

bool SerialMuxDemux::ExistCallback(IN const std::map<int, Callback>& callbacks, IN Callback callback, OUT int& callbackID)
{
	callbackID = -1;

	//初始化查询边界
	std::map<int, Callback>::const_iterator iter = callbacks.begin();
	std::map<int, Callback>::const_iterator endIter = callbacks.end();

	//查询指定回调是否存在
	for(;iter != endIter; ++iter)
	{
		if(callback == iter->second)
		{
			callbackID = iter->first;
			return true;
		}
	}

	return false;
}
