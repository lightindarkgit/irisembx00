#pragma once

#include <list>
#include <condition_variable>
#include <assert.h>



class serialfifo
{
public:
	 serialfifo()
	 {
	 }
	 ~serialfifo()
	 {
	 }
public:
	void writebytetofifo(unsigned char data)
	{
		std::lock_guard<std::mutex> lock (_mutex);
		queue.push_back(data);
	}
	unsigned char readbytefromfifo()
	{
		unsigned char data;
		std::lock_guard<std::mutex> lock (_mutex);
		data = queue.front();
		queue.pop_front();
		return data;
	}
	unsigned int fifonumber()
	{
		std::lock_guard<std::mutex> lock (_mutex);
		return queue.size();
	}

	void fifoclear()
	{
		queue.clear();
	}

 private:
 	std::list<unsigned char> queue;
	mutable std::mutex _mutex;
    std::condition_variable _condvar;
 	
};






