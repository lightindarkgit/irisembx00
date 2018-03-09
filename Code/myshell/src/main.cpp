#include <stdio.h>
#include <time.h>
#include <iostream>
#include <sstream>
#include <cassert>
#include <string>
#include <string.h>

#include <memory>
#include <thread>
#include <mutex>

#include <signal.h>
#include <sys/time.h>

class KLogFile;
std::mutex _mutex;
KLogFile *g_log = nullptr;

void sigFunc(int);

template<typename TMutex>
class guard
{
	TMutex &_mutex;
public:
	guard(TMutex &m)
		:_mutex(m)
	{
		_mutex.lock();
	}
	~guard()
	{
		_mutex.unlock();
	}
};
// 日志:
// 当一定时间不使用FILE *时，关掉句柄
// 每天一个日志文件，同一程序的日志文件最多缓存30天
// LOG信息往文件中写时，采用缓冲机制，避免对频繁写文件对文件系统造成破坏。
class KLogFile
{
	friend void sigFunc(int);

	FILE *_fp;
	std::string _fileName;
	time_t _lastAccessTime;	// 最后一次访问_fp

	struct tm _day;

	std::string _cmdLine;
public:
	KLogFile(const char *cmdLine)
		:_fp(nullptr), _cmdLine(cmdLine)
	{
		initFileName(time(nullptr));
		// 先向日志中写入进程启动时间
		append("\n----------------------------------------------------------------\n");
		fprintf(_fp, "[%s]: cmd line: %s\n", formatTime(true).c_str(), cmdLine);

		time_t t = time(nullptr);
		_day = *localtime(&t);

		g_log = this;
	}
	~KLogFile()
	{
        release();
		// 安全地将g_log置为nullptr
        guard<std::mutex> _(_mutex);
        g_log = nullptr;

	}

	// 取得命令行中程序的名称
	std::string cmdName() const
	{
		const char *space = _cmdLine.c_str();
		for (; *space && !isspace(*space); space++)
			;

		const char *_cmd = space;
		for (; _cmd > _cmdLine.c_str() && *_cmd != '/'; _cmd--)
			;

		return std::string((*_cmd == '/'? _cmd+1: _cmd), space);
	}
	// 写日志
	void append(const char *buf, int bufCount = -1)
	{
		if (bufCount < 0)
			bufCount = strlen(buf);
        //std::cout << "H: " << __FUNCTION__ << "(" << bufCount << " bytes)" << std::endl;

		guard<std::mutex> _(_mutex);
		// 此过程与timer是冲突的，因此需要加锁，以阻止timer执行
		FILE *fp = getFile();
        //assert(fp != nullptr);
		if (fp)
		{
			fwrite(buf, 1, bufCount, fp);
			_lastAccessTime = time(nullptr);
		}
	}
    // release
    void release()
    {
        guard<std::mutex> _(_mutex);
        if (nullptr != _fp)
            fclose(_fp);
        _fp = nullptr;
    }

	// 当进程退出时，调用此函数，以便于打印进程退出信息
	void appendExitCode(int exitCode)
	{
		std::stringstream ss;
		ss << "[" << formatTime(true) << "][ExitCode: " << exitCode << "] " << _cmdLine << std::endl;

        std::string str = ss.str();
        append(str.c_str(), str.size());
	}

	static std::string formatTime(bool withTime)
	{
		time_t t = time(nullptr);
		return formatTime(*localtime(&t), withTime);
	}

private:
	// 定时器，由sigFunc调用
	void timer()
	{
		time_t t = time(NULL);
		struct tm curTime = *localtime(&t);

		// 由于是在sigFunc调用的，因此该函数是并发的，需要加锁(已在sigFunc中加锁)
		//guard<std::mutex> _(_mutex);
		// 年月日不相同，则跨天
		bool isDiffDay = (curTime.tm_year != _day.tm_year || curTime.tm_yday != _day.tm_yday);//mon != _day.tm_mon || curTime.tm_mday != _day.tm_mday);

		bool closeFile = false;
		// 跨天了，则重新生成日志文件
		if (isDiffDay)
		{
			_day = curTime;

			// 跨天了，则需要关闭日志文件，以打开新的日志文件
			closeFile = (_fp != nullptr);

			// 重新生成日志文件名
			initFileName(curTime);
		}

		// 判断_fp是否超时，超时则关闭
		if (!closeFile && _fp != nullptr)
		{
			// 判断_fp是否已超时(距最后一次访问超过20秒，则关闭文件)
			double secs = difftime(t, _lastAccessTime);
			closeFile = (secs >= 20);
		}

		if (closeFile)
		{
			fclose(_fp);
			_fp = nullptr;
		}
	}
	// 写日志前调用，以便于生成有效的FILE *
	FILE *getFile()
	{
		if (_fp == nullptr)
		{
			_fp = fopen(_fileName.c_str(), "a+");
			_lastAccessTime = time(nullptr);
		}
		return _fp;
	}

	static std::string formatTime(const tm &curTime, bool withTime)
	{
		char szDate[64];
		int len = sprintf(szDate, "%04d-%02d-%02d",
				curTime.tm_year + 1900, curTime.tm_mon + 1, curTime.tm_mday);
		if (len >= 0 && withTime)
		{
			sprintf(szDate + len, " %02d:%02d:%02d",
				curTime.tm_hour, curTime.tm_min, curTime.tm_sec);
		}
		return szDate;
	}

	// 日志文件名生成
	void initFileName(const tm &curTime)
	{
		std::stringstream ss;
		ss << cmdName() << ".log." << formatTime(curTime, false);
		_fileName = ss.str();
		//std::cout << __FUNCTION__ << ": " << _fileName << std::endl;
	}
	void initFileName(time_t t)
	{
		initFileName(*localtime(&t));
	}
};

void sigFunc(int)
{
	// 加锁是为了保护g_log不会被提前置nullptr
	// 同时也是为了保护g_log中的成员
	guard<std::mutex> _(_mutex);
	if (g_log == nullptr)
		return;
	g_log->timer();
}
bool startTimer()
{
	struct itimerval tv, otv;
	signal(SIGALRM, sigFunc);
	tv.it_value.tv_sec = 1;
	tv.it_value.tv_usec = 0;
	tv.it_interval.tv_sec = 1;
	tv.it_interval.tv_usec = 0;

	return setitimer(ITIMER_REAL, &tv, nullptr) == 0;
}

void sigInt(int)
{
    g_log->release();
    exit(4);
}

int main(int argc, char *argv[])
{
    signal(SIGINT, sigInt);

	// 判断参数合法性
	if (argc <= 1)
	{
		printf("Usage: %s bin_and_cmd_line\n", argv[0]);
		printf("Note: bin_and_cmd_line must be in one-string\n");
		printf("Example: %s \"ls -l\"\n", argv[0]);
		return 0;
	}

	// 日志
	KLogFile log(argv[1]);

	// 定时器
	if (!startTimer())
	{
		printf("起动定时器失败\n");
		return 1;
	}

	// 打开进程
	FILE *pipe = popen(argv[1], "r");
	if (pipe == NULL)
	{
		printf("无法运行：%s, 请确认命令行是否正确\n", argv[1]);
		return 2;
	}

    char buffer[4096], *bufferEnd;
    bufferEnd = buffer;

    char c;
	int rdCnt;
    while((rdCnt = fread(&c, 1, 1, pipe)) > 0)
	{
        *bufferEnd = c;
        bufferEnd++;
        if (c == '\n' || bufferEnd - buffer >= sizeof(buffer))
        {
            log.append(buffer, bufferEnd - buffer);
            bufferEnd = buffer;
        }
        //log.append(buffer, rdCnt);
        //std::cout << "H: " << std::string(buffer, buffer + rdCnt) << std::endl;
	}

	// 进程退出
	int eofPipe = feof(pipe);
	int processReturn = pclose(pipe);
	log.appendExitCode(processReturn);	// 记录进程退出时间
	if (!eofPipe)
		return 3;
	
	return processReturn;
}
