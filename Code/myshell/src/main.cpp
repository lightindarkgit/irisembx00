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
// ��־:
// ��һ��ʱ�䲻ʹ��FILE *ʱ���ص����
// ÿ��һ����־�ļ���ͬһ�������־�ļ���໺��30��
// LOG��Ϣ���ļ���дʱ�����û�����ƣ������Ƶ��д�ļ����ļ�ϵͳ����ƻ���
class KLogFile
{
	friend void sigFunc(int);

	FILE *_fp;
	std::string _fileName;
	time_t _lastAccessTime;	// ���һ�η���_fp

	struct tm _day;

	std::string _cmdLine;
public:
	KLogFile(const char *cmdLine)
		:_fp(nullptr), _cmdLine(cmdLine)
	{
		initFileName(time(nullptr));
		// ������־��д���������ʱ��
		append("\n----------------------------------------------------------------\n");
		fprintf(_fp, "[%s]: cmd line: %s\n", formatTime(true).c_str(), cmdLine);

		time_t t = time(nullptr);
		_day = *localtime(&t);

		g_log = this;
	}
	~KLogFile()
	{
        release();
		// ��ȫ�ؽ�g_log��Ϊnullptr
        guard<std::mutex> _(_mutex);
        g_log = nullptr;

	}

	// ȡ���������г��������
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
	// д��־
	void append(const char *buf, int bufCount = -1)
	{
		if (bufCount < 0)
			bufCount = strlen(buf);
        //std::cout << "H: " << __FUNCTION__ << "(" << bufCount << " bytes)" << std::endl;

		guard<std::mutex> _(_mutex);
		// �˹�����timer�ǳ�ͻ�ģ������Ҫ����������ֹtimerִ��
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

	// �������˳�ʱ�����ô˺������Ա��ڴ�ӡ�����˳���Ϣ
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
	// ��ʱ������sigFunc����
	void timer()
	{
		time_t t = time(NULL);
		struct tm curTime = *localtime(&t);

		// ��������sigFunc���õģ���˸ú����ǲ����ģ���Ҫ����(����sigFunc�м���)
		//guard<std::mutex> _(_mutex);
		// �����ղ���ͬ�������
		bool isDiffDay = (curTime.tm_year != _day.tm_year || curTime.tm_yday != _day.tm_yday);//mon != _day.tm_mon || curTime.tm_mday != _day.tm_mday);

		bool closeFile = false;
		// �����ˣ�������������־�ļ�
		if (isDiffDay)
		{
			_day = curTime;

			// �����ˣ�����Ҫ�ر���־�ļ����Դ��µ���־�ļ�
			closeFile = (_fp != nullptr);

			// ����������־�ļ���
			initFileName(curTime);
		}

		// �ж�_fp�Ƿ�ʱ����ʱ��ر�
		if (!closeFile && _fp != nullptr)
		{
			// �ж�_fp�Ƿ��ѳ�ʱ(�����һ�η��ʳ���20�룬��ر��ļ�)
			double secs = difftime(t, _lastAccessTime);
			closeFile = (secs >= 20);
		}

		if (closeFile)
		{
			fclose(_fp);
			_fp = nullptr;
		}
	}
	// д��־ǰ���ã��Ա���������Ч��FILE *
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

	// ��־�ļ�������
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
	// ������Ϊ�˱���g_log���ᱻ��ǰ��nullptr
	// ͬʱҲ��Ϊ�˱���g_log�еĳ�Ա
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

	// �жϲ����Ϸ���
	if (argc <= 1)
	{
		printf("Usage: %s bin_and_cmd_line\n", argv[0]);
		printf("Note: bin_and_cmd_line must be in one-string\n");
		printf("Example: %s \"ls -l\"\n", argv[0]);
		return 0;
	}

	// ��־
	KLogFile log(argv[1]);

	// ��ʱ��
	if (!startTimer())
	{
		printf("�𶯶�ʱ��ʧ��\n");
		return 1;
	}

	// �򿪽���
	FILE *pipe = popen(argv[1], "r");
	if (pipe == NULL)
	{
		printf("�޷����У�%s, ��ȷ���������Ƿ���ȷ\n", argv[1]);
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

	// �����˳�
	int eofPipe = feof(pipe);
	int processReturn = pclose(pipe);
	log.appendExitCode(processReturn);	// ��¼�����˳�ʱ��
	if (!eofPipe)
		return 3;
	
	return processReturn;
}
