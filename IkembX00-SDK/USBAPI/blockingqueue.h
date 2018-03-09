#include <condition_variable>
//#include <thread>
#include <list>
#include <assert.h>
#include <chrono>

template<typename T>
class BlockingQueue
{
public:
    BlockingQueue (int size) :_maxSize(size), _mutex (), _condvar (), _queue ()
    {
    }

    void Put (const T& task)
    {
        {
//            std::cout << "put1 " <<std::endl;
            if(_maxSize<=Size())
            {
                Take ();
            }
            std::lock_guard<std::mutex> lock (_mutex);
            _queue.push_back (task);
//            std::cout << "put2 " <<std::endl;
        }
        _condvar.notify_all ();
    }

    T Take ()
    {
        std::unique_lock<std::mutex> lock (_mutex);
        _condvar.wait (lock, [this]{return !_queue.empty ();});
        assert (!_queue.empty ());
        T front (_queue.front ());
        _queue.pop_front ();
        return front;
    }

   //timeOut :为超时时间 单位秒
   bool Take(T& task,int timeOut)
   {
//        std::cout << "Take1 " <<std::endl;
        std::unique_lock<std::mutex> lock (_mutex);
        if(!_condvar.wait_for(lock,std::chrono::seconds(timeOut), [this]{return !_queue.empty ();}))
        {
             return false;
        }
        assert (!_queue.empty ());
        task  = _queue.front ();
        _queue.pop_front ();

//        std::cout << "Take2 " <<std::endl;
        return true;
    }

    size_t Size() const
    {
        std::lock_guard<std::mutex> lock (_mutex);
        return _queue.size();
    }

private:
    BlockingQueue (const BlockingQueue& rhs);
    BlockingQueue& operator = (const BlockingQueue& rhs);

private:
    size_t _maxSize;
    mutable std::mutex _mutex;
    std::condition_variable _condvar;
    std::list<T> _queue;
};
