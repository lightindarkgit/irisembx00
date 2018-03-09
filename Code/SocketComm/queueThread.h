/******************************************************************************************
** �ļ���:   QueueThread.cpp
���� ��Ҫ��: QueueThread
**
** Copyright (c) �пƺ�����޹�˾
** ������:   fjf
** ��  ��:   2013-12-17
** �޸���:
** ��  ��:
** ��  ��:   �̰߳�ȫ�������ļ�
** ���� ��Ҫģ��˵����
**
** ��  ��:   1.0.0
** ��  ע:
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
	QueueThread& operator=(const QueueThread&) = delete;//��ֹ��ֵ������Ϊ�˼�
	QueueThread(QueueThread const& other)
	{
		std::lock_guard<std::mutex> locker(other._mutex);
		dataQueue = other.dataQueue;
	}

	void push(T newValue)//��Ӳ���
	{
		std::lock_guard<std::mutex> locker(_mutex);
		dataQueue.push(newValue);
		dataCond.notify_one();
	}

	void wait_and_pop(T& value)//ֱ����Ԫ�ؿ���ɾ��Ϊֹ
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

	bool try_pop(T& value)//������û�ж���Ԫ��ֱ�ӷ���
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