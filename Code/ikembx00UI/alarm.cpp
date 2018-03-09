/******************************************************************************************
 ** 文件名:   alarm.cpp
 ×× 主要类:   (无)
 **
 ** Copyright (c) 中科虹霸有限公司
 ** 创建人:   Wang.L
 ** 日  期:   2015-01-28
 ** 修改人:
 ** 日  期:
 ** 描  述:   闹铃模块实现文件
 ** ×× 主要模块说明：
 **
 ** 版  本:   1.0.0
 ** 备  注:
 **
 ******************************************************************************************/
#include "alarm.h"
#include "../XML/xml/IKXmlApi.h"
#include "../serial/serial/include/basictypes.h"
#include "commononqt.h"

//QT库头文件
#include <QCoreApplication>
#include <QTime>
#include <QDir>
//标准库头文件
#include <unistd.h>
#include <pthread.h>
#include <string.h>
#include "../Common/Logger.h"
#include "../Common/Exectime.h"

static Alarm* Alarm::alarmer = NULL;
static int Alarm::refCounter = 0;
//liu 根据新协议修改
unsigned char openAlarm[4] = {0x0d, 0x01, 0x00, 0x00};
unsigned char closeAlarm[4] = {0x0d, 0x00, 0x00, 0x00};
QStringList alarmTimers;
QTimer timer;

SerialMuxDemux* sm = nullptr;

bool optOpenOK = false;
bool optCloseOK = false;

//std::string GetXmlFilePath()
//{
//    QDir appDir(QCoreApplication::applicationDirPath());
//    QString xmlFile("%1%2%3");
//    xmlFile = xmlFile.arg(appDir.absolutePath(), QDir::separator(), "ikembx00.xml");

//    if(QFile::exists(xmlFile))
//    {
//        return xmlFile.toStdString();
//    }

//    return  std::string("");
//}


Alarm::Alarm():
	alarmDuration(20)
{
	Exectime etm(__FUNCTION__);
}

Alarm::~Alarm()
{
	Exectime etm(__FUNCTION__);
	sm->ReleaseInstance();
	if(nullptr != alarmer)
	{
		delete alarmer;
	}
}

bool Alarm::init()
{
	Exectime etm(__FUNCTION__);
	//TO DO
	updateAlarms();
	getAlarmDuration(alarmDuration);

	sm = SerialMuxDemux::GetInstance();
	sm->AddCallback(0x0d, alarmCallback);
	sm->Init(GetSerialPortName());

	connect(&(alarmer->timer), SIGNAL(timeout()), this, SLOT(slotTimeout()));

	//启动定时器
	timer.setSingleShot(false);
	timer.start(1000);
}

Alarm* Alarm::getInstance()
{
	Exectime etm(__FUNCTION__);
	//TO DO;
	if(nullptr == alarmer)
	{
		alarmer = new Alarm();
		alarmer->init();
	}
	refCounter++;

	return alarmer;
}


void Alarm::release()
{
	Exectime etm(__FUNCTION__);
	//TO DO
	if(0 == refCounter--)
	{
		delete alarmer;
	}
}


bool Alarm::getAlarmDuration(int &dur)
{
	Exectime etm(__FUNCTION__);
	IKXml::IKXmlDocument cfgFile;
	cfgFile.LoadFromFile(GetXmlFilePath());

	std::string duration = cfgFile.GetElementValue("/IKEMBX00/configures/alarmduration");
	if(duration.empty())
	{
		std::cout << "[" << __FUNCTION__ << "@Alarm]: unable to get arlarm duration from configure file!" << std::endl;
		LOG_ERROR("unable to get arlarm duration from configure file");
		return false;
	}

	dur = atoi(duration.c_str());
	return true;
}

void Alarm::updateAlarms()
{
	Exectime etm(__FUNCTION__);
	//TO DO
	IKXml::IKXmlDocument cfgFile;
	cfgFile.LoadFromFile(GetXmlFilePath());

	QStringList alarmTags;
	std::string alarmTimer;

	alarmTags << "alarm1st" << "alarm2nd" << "alarm3rd" << "alarm4th" << "alarm5th" << "alarm6th" << "alarm7th" << "alarm8th";

	alarmTimers.clear();
	for(int iter = 0; iter < alarmTags.size(); iter++)
	{
		alarmTimer = cfgFile.GetElementValue(QString("/IKEMBX00/configures/alarm/%1").arg(alarmTags[iter]).toStdString());
		if(!alarmTimer.empty())
		{
			alarmTimers.push_back(alarmTimer.c_str());
			//std::cout << alarmTags[iter].toStdString() << "alarm time:" << alarmTimer << std::endl;
		}
	}
}


void Alarm::slotTimeout()
{
	Exectime etm(__FUNCTION__);
	//TO DO
	std::string curTime = QTime::currentTime().toString("HH:mm:ss").toStdString();
	//std::cout << "current time: " << curTime << std::endl;
	if(alarmTimers.contains(curTime.c_str()))
	{
		pthread_t thrID;
		pthread_create(&thrID, NULL, ringAlarm, this);
	}
}


void Alarm::alarmCallback(const unsigned char *data, const int size)
{
	Exectime etm(__FUNCTION__);
	//TO DO
	//printf("[@%s]get serial data(size: %d): 0x%02X%02X%02X\n", __FUNCTION__, size, data[0], data[1], data[2]);
	if(0 == strncmp(data, openAlarm + 1, size))
	{
		optOpenOK = true;
		optCloseOK = false;
		std::cout << "[" << __FUNCTION__ << "@Alarm]: alarm has been opened!" << std::endl;
	}
	else if(0 == strncmp(data, closeAlarm + 1, size))
	{
		optCloseOK = true;
		optOpenOK = false;
		std::cout << "[" << __FUNCTION__ << "@Alarm]: alarm has been closed!" << std::endl;
	}
}


void* Alarm::ringAlarm(void *arg)
{
	Exectime etm(__FUNCTION__);
	//TO DO
	Alarm* alarmIns = (Alarm *)arg;

	if(alarmIns->startRinging())
	{
		sleep(alarmIns->alarmDuration);
		alarmIns->stopRinging();
	}
}

bool Alarm::startRinging()
{
	Exectime etm(__FUNCTION__);
	//TO DO
	std::cout << "[" << QDateTime::currentDateTime().toString("yyyy-MM-dd HH:mm:ss").toStdString() << "]"
		<< "it is time to open alarm..."
		<< std::endl;

	int tryCount = maxTry;
	while(--tryCount && !optOpenOK)
	{
		sm->Send(openAlarm, sizeof(openAlarm) / sizeof(openAlarm[0]));
		usleep(500000);            //Retry to ring alarm after 100ms
	}

	//If alarm wasnot opened correctly
	if(0 == tryCount && !optOpenOK)
	{
		std::cout  << "[" << QDateTime::currentDateTime().toString("yyyy-MM-dd HH:mm:ss").toStdString() << "]"
			<< "[" << __FUNCTION__ << "@Alarm]: after trying amount of times, it has also failed to open the alarm!"
			<< std::endl;
		LOG_ERROR("after trying amount of times, it has also failed to open the alarm!");
		return false;
	}

	//Reset the alarm opened flag
	optOpenOK = false;

	return true;
}


bool Alarm::stopRinging()
{
	Exectime etm(__FUNCTION__);
	//TO DO
	std::cout << "[" << QDateTime::currentDateTime().toString("yyyy-MM-dd HH:mm:ss").toStdString() << "]"
		<< "it is time to close alarm..."
		<< std::endl;

	int tryCount = maxTry;
	while(--tryCount && !optCloseOK)
	{
		sm->Send(closeAlarm, sizeof(closeAlarm) / sizeof(closeAlarm[0]));
		usleep(500000);
	}

	//If alarm wasnot closed correctly
	if(0 == tryCount && !optCloseOK)
	{
		std::cout << "[" << QDateTime::currentDateTime().toString("yyyy-MM-dd HH:mm:ss").toStdString() << "]"
			<< "[" << __FUNCTION__ << "@Alarm]: after trying amount of times, it has also failed to close the alarm!"
			<< std::endl;
		LOG_ERROR("after trying amount of times, it has also failed to close the alarm!");
		return;
	}

	//Reset the alarm opened flag
	optCloseOK = false;

	return true;
}

