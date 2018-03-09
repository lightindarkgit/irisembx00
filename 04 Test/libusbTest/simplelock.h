
/******************************************************************************************
** 文件名:   SIMPLELOCK_H
**
** Copyright (c) 中科虹霸有限公司
** 创建人:   刘中昌
** 日  期:   2013-11-27
** 修改人:
** 日  期:
** 描  述:   简单锁类  条件锁  条件等待
 *×× 主要模块说明: 数据锁、条件锁
**
** 版  本:   1.0.0
** 备  注:
**
*****************************************************************************************/


#ifndef __SIMPLELOCK_H__
#define __SIMPLELOCK_H__
#include <pthread.h>
//#include "tr1/memory"

//禁止拷贝类
class NoCopy
{
protected:
    NoCopy() {}
    ~NoCopy() {}
private:  // emphasize the following members are private
    NoCopy( const NoCopy& );
    const NoCopy& operator=( const NoCopy& );
};


//内部自动创建Mutex 类自身管理Mutex
class SimpleLock: NoCopy
{
public:
    SimpleLock()
    {
        pthread_mutex_init(&_mutex,nullptr);
        lock();
    }

    ~SimpleLock()
    {
        unlock();
        pthread_mutex_destroy(&_mutex);
    }

private:
    inline void lock()
    {
        pthread_mutex_lock(&_mutex);
    }

    inline void unlock()
    {
        pthread_mutex_unlock(&_mutex);
    }

private:
    //std::tr1::shared_ptr<pthread_mutex_t> linux_mutex;
   pthread_mutex_t _mutex;
};

//外部传入Mutex
class SimpleLockForMutex: NoCopy
{
public:
    SimpleLockForMutex(pthread_mutex_t& mutex)
    {
        _linux_mutex = mutex;
        lock();
    }

     ~SimpleLockForMutex()
    {
        unlock();
    }

private:
    void lock()
    {
        pthread_mutex_lock(&_linux_mutex);
    }

    void unlock()
    {
        pthread_mutex_unlock(&_linux_mutex);
    }

private:
    //std::tr1::shared_ptr<pthread_mutex_t> linux_mutex;
   pthread_mutex_t _linux_mutex;
};

//条件等等基函数
class WaiterBase : NoCopy
{
protected:
    WaiterBase()
    {
        pthread_mutex_init(&_mutex, NULL);
        pthread_cond_init(&_cond, NULL);
    }

    ~WaiterBase()
    {
        pthread_mutex_destroy(&_mutex);
        pthread_cond_destroy(&_cond);
    }

    pthread_mutex_t _mutex;
    pthread_cond_t _cond;
};

//线程等待函数
class ThreadWaiter : private WaiterBase
{
public:
    //等待
   inline void wait()
    {
        _signaled = false;
        pthread_mutex_lock(&_mutex);

        while (!_signaled)
        {
            pthread_cond_wait(&_cond, &_mutex);
        }
        pthread_mutex_unlock(&_mutex);
    }

    //条件信号
   inline void broadcast()
    {
        pthread_mutex_lock(&_mutex);
        _signaled = true;
        pthread_cond_broadcast(&_cond);
        pthread_mutex_unlock(&_mutex);
    }

private:
    bool _signaled;
};
#endif // __SIMPLELOCK_H__
