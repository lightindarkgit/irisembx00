#include <log4cpp/Category.hh>
#include <log4cpp/Appender.hh>
#include <log4cpp/Priority.hh>
#include <log4cpp/NDC.hh>
#include <log4cpp/PatternLayout.hh>
#include <log4cpp/BasicConfigurator.hh>
#include <log4cpp/PropertyConfigurator.hh>
#include <log4cpp/DailyRollingFileAppender.hh>

//#include <mutex>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>


#include <string>
#include "../Common/Logger.h"
#include <iostream>

using namespace std;
using namespace log4cpp;

///////////////////////////////////////////////////////////
// Logger_Impl
class Logger_Impl
{
public:
    static Logger_Impl* getInstance()
    {
       // std::lock_guard<std::mutex> lck(instanceLoggerMutex);

        if (NULL == pLoggerInstance)
        {
            // ����Log����ȡ����
            pLoggerInstance = new Logger_Impl;
        }

        return pLoggerInstance;
    }

private:

    log4cpp::Category* root ;
    log4cpp::Category* sub1;
    Logger_Impl()
    {
         root = NULL;
         sub1 = NULL;
        log4cpp::NDC::clear();
        // 1 ��ȡ���������ļ�
        // ��ȡ����, ��ȫ���Ժ��ԣ����Զ���һ��ȱʡ���Ի���ʹ��ϵͳȱʡ����
        // BasicLayout����������ȼ���־��ConsoleAppender
        try
		{
			std::stringstream oss;

			string log4cpp = CONFIGFILE;
			const char* pconf = log4cpp.c_str();

			// 检测文件是否存在 
			if(access(pconf,F_OK) == -1)
			{
				printf("log4cpp config [%s] is not exist.\n",pconf);
				return ;
			}

			char exeFullPath[2048] = {0};
#ifdef WIN32
			GetModuleFileName(NULL, exeFullPath, sizeof(exeFullPath)); //�õ�����ģ��.exeȫ·��
			strrchr(exeFullPath, '\\')[1] = 0;

			oss << exeFullPath;
			if(*(pconf + 1) == ':')
			{
				oss.str("");
			}
#else
			getcwd(exeFullPath, 2048);
			oss << exeFullPath;
			oss << "/";
			if(*pconf == '/')
			{
				oss.str("");
			}
#endif

			oss << pconf;

			std::string confFilePath = oss.str();

			std::cout << "log4cpp config file : " << confFilePath << std::endl;

			// log4cpp::PropertyConfigurator::configure("./log4cpp.conf");
			log4cpp::PropertyConfigurator::configure(confFilePath);
			root = &log4cpp::Category::getRoot();
			sub1 = &log4cpp::Category::getInstance(std::string("sub1"));;

		}
        catch (log4cpp::ConfigureFailure& f)
        {
            std::cout << "Configure Problem " << f.what() << std::endl;
        }
        
    }

public:
    void debug(const char* msg)
    {
		if(sub1 == NULL)
		{
			printf("log4cpp file handler is NULL.\n");
			return;
		}
        sub1->debug(msg);
    }

    void info(const char* msg)
    {
		if(sub1 == NULL)
		{
			printf("log4cpp file handler is NULL.\n");
			return;
		}
        sub1->info(msg);
    }

    void warn(const char* msg)
    {
		if(sub1 == NULL)
		{
			printf("log4cpp file handler is NULL.\n");
			return;
		}
        sub1->warn(msg);
    }

    void error(const char* msg)
    {
		if(sub1 == NULL)
		{
			printf("log4cpp file handler is NULL.\n");
			return;
		}
        sub1->error(msg);
    }

private:
    ~Logger_Impl()
    {
        //root =NULL;

        log4cpp::NDC::clear();
        log4cpp::Category::shutdown();
    }



private:
    static Logger_Impl* pLoggerInstance;
 //   static std::mutex instanceLoggerMutex;
};


// ���Logger_Impl�ĵ���ģʽ
Logger_Impl* Logger_Impl::pLoggerInstance = NULL;
//std::mutex Logger_Impl::instanceLoggerMutex;


///////////////////////////////////////////////////////////
// Logger
Logger::Logger()
{
}

Logger::~Logger()
{

}

void Logger::LOGD(const char* msg)
{
    Logger_Impl::getInstance()->debug(msg);
}

void Logger::LOGI(const char* msg)
{
    Logger_Impl::getInstance()->info(msg);
}

void Logger::LOGW(const char* msg)
{
    Logger_Impl::getInstance()->warn(msg);
}

void Logger::LOGE(const char* msg)
{
    Logger_Impl::getInstance()->error(msg);
}
//caotao 20170510, 添加cpu频率设置接口，最好放到全局头文件里,几个地方要引用
void cpuFreqSet(const char *gov)
{
	char govPath[100];

	FILE *f = NULL;

	if(NULL == gov)
	{
		fprintf(stdout, "gov is null, return\n");
		return;
	}

	for(int i = 0; i < 4; i++)
	{
		sprintf(govPath, "/sys/devices/system/cpu/cpu%d/cpufreq/scaling_governor", i);

		if(NULL != (f = fopen(govPath, "w")))
		{
			fwrite(gov, 1, strlen(gov), f);
			fclose(f);
			printf("gov len: %d\n", strlen(gov));
		}
		else
		{
			fprintf(stdout, "set %s on %s failed, due to %s\n", gov, govPath, strerror(errno));
		}
	}
}
