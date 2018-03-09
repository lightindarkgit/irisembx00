/******************************************************************************************
** 文件名:   QueueThread.cpp
×× 主要类: QueueThread
**
** Copyright (c) 中科虹霸有限公司
** 创建人:   fjf
** 日  期:   2013-12-17
** 修改人:
** 日  期:
** 描  述:   线程安全队列类文件
** ×× 主要模块说明：
**
** 版  本:   1.0.0
** 备  注:
**
******************************************************************************************/
#include <queue>
#include <memory>
#include <mutex>
#include <condition_variable>


template<typename T>
class QueueThread
{
public:
	QueueThread(){}
	QueueThread& operator=(const QueueThread&) = delete;//禁止赋值操作是为了简化
	QueueThread(QueueThread const& other)
	{
		std::lock_guard<std::mutex> locker(other._mutex);
		dataQueue = other.dataQueue;
	}

	void push(T newValue)//入队操作
	{
		std::lock_guard<std::mutex> locker(_mutex);
		dataQueue.push(newValue);
		dataCond.notify_one();
	}

	void wait_and_pop(T& value)//直到有元素可以删除为止
	{
		std::unique_lock<std::mutex> locker(_mutex);
		dataCond.wait(locker, [this]{return !dataQueue.empty(); });
		value = dataQueue.front();
		dataQueue.pop();
	}

	std::shared_ptr<T> wait_and_pop()
	{
		std::unique_lock<std::mutex> locker(_mutex);
		dataCond.wait(locker, [this]{return !dataQueue.empty(); });
		std::shared_ptr<T> res(std::make_shared<T>(dataQueue.front()));
		dataQueue.pop();
		return res;
	}

	bool try_pop(T& value)//不管有没有队首元素直接返回
	{
		std::lock_guard<std::mutex> locker(_mutex);
		if (dataQueue.empty())
		{
			return false;
		}
			
		value = dataQueue.front();
		dataQueue.pop();
		return true;
	}

	std::shared_ptr<T> try_pop()
	{
		std::lock_guard<std::mutex> locker(_mutex);
		if (dataQueue.empty())
		{
			return std::shared_ptr<T>();
		}
			
		std::shared_ptr<T> res(std::make_shared<T>(dataQueue.front()));
		dataQueue.pop();
		return res;
	}

	bool empty() const
	{
		std::lock_guard<std::mutex> locker(_mutex);
		return dataQueue.empty();
	}
private:
	mutable std::mutex _mutex;
	std::queue<T> dataQueue;
	std::condition_variable dataCond;
};