/*****************************************************************************
 ** 文 件 名： SysNetInterface.h
 ** 主 要 类： SysNetInterface, SysStatusSignal
 **
 ** Copyright (c) 中科虹霸有限公司
 **
 ** 创 建 人：Wang.L
 ** 创建时间：20014-05-01
 **
 ** 修 改 人：
 ** 修改时间：
 ** 描  述:  系统配置网络操作接口，用于设置设备的网络及系统时间等，信号类用于监视系统状态的变化
 ** 主要模块说明:
 **
 ** 版  本:   1.0.0
 ** 备  注:
 **
 *****************************************************************************/
#include "sysnetinterface.h"
#include <QHostInfo>
#include <QHostAddress>
#include <QNetworkInterface>
#include <QDateTime>
#include <QString>
#include <sstream>

#include <fcntl.h>
#include <unistd.h>


#include "../common.h"
#include "../XML/xml/IKXmlApi.h"
#include "ikxmlhelper.h"
#include "version.h"
#include "commononqt.h"
#include "../Common/Logger.h"
#include "../Common/Exectime.h"

const std::string NET_SETTINGS_FILE_PATH = "/etc/network/interfaces";

const std::string SysNetInterface::GetDevIP()
{
	Exectime etm(__FUNCTION__);
	//TO DO
	return  NetInfo::GetIP();
}


const std::string SysNetInterface::GetDevSN()
{
	Exectime etm(__FUNCTION__);
	//TO DO
	//    //The following implement is not reliable especially when the network is unavailable, so it's not recommended to use.
	//    QNetworkInterface interface = QNetworkInterface::interfaceFromName(GetCurrentInterface().c_str());

	//    return interface.hardwareAddress().toStdString();

	FILE* fd;
	size_t readLen;
	char* line = NULL;
	std::string devSN("");
	fd = popen("ifconfig -a | grep -EI '([0-9a-f]{2}:){5}[0-9a-f]{2}' | head -n 1 | awk '{print $NF}'", "r");
	if(-1 != getline(&line, &readLen, fd))
	{
		//TODO
		devSN = std::string(line);
		devSN.erase(devSN.find("\n"));
		std::cout << GetLogHead(__FUNCTION__) << "device SN: " << devSN << std::endl;
	}
	else
	{
		std::cout << GetLogHead(__FUNCTION__) << "faile to get device SN!" << std::endl;
	}

	//Free heap buffer as needed
	if(line)
	{
		free(line);
	}

	//Close file handle
	pclose(fd);

	return devSN;
}


const std::string SysNetInterface::GetSoftVer()
{
	Exectime etm(__FUNCTION__);
	//TO DO
	return VERSION_CODE;
}


const std::string SysNetInterface::GetSrvIP()
{
	Exectime etm(__FUNCTION__);
	//TO DO
	IKXmlHelper syscfg(GetXmlFilePath());

	return syscfg.GetServerIP();
}


const std::string SysNetInterface::GetSrvPort()
{
	Exectime etm(__FUNCTION__);
	//TO DO
	IKXmlHelper syscfg(GetXmlFilePath());

	return syscfg.GetServerPort();
}


const std::string SysNetInterface::GetSysTime()
{
	Exectime etm(__FUNCTION__);
	//TO DO
	return QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss").toStdString();
}


bool SysNetInterface::SetSysTime(const std::string& time)
{
	Exectime etm(__FUNCTION__);
	//TO DO
	time_t ttNewTime;
	ttNewTime = (time_t)QDateTime::fromString(time.c_str(), "yyyy-MM-dd hh:mm:ss").toTime_t();


	//更新用户设置的时间到系统
	if(0 == stime(&ttNewTime))
	{
		SysStatusSignal* ins = SysStatusSignal::GetInstance();
		if(ins)
		{
			ins->SysTimeUpdated();
			ins->Release();
		}

		return true;
	}
	else
	{
		return false;
	}
}

const std::string SysNetInterface::GetCurrentInterface()
{
	Exectime etm(__FUNCTION__);
	//TO DO
	foreach (QNetworkInterface ifa, QNetworkInterface::allInterfaces())
	{
		if(QNetworkInterface::IsLoopBack == (QNetworkInterface::IsLoopBack & ifa.flags()))
		{
			continue;
		}

		if(QNetworkInterface::IsUp != (QNetworkInterface::IsUp & ifa.flags()))
		{
			continue;
		}

		if(QNetworkInterface::IsRunning != (QNetworkInterface::IsRunning & ifa.flags()))
		{
			continue;
		}

		return ifa.name().toStdString();
	}


	return std::string("");
}


bool SysNetInterface::SetDevNetInfo(const std::string &ip, const std::string &mask, const std::string &gate)
{
	Exectime etm(__FUNCTION__);
	//TO DO
	if(ip.empty() || mask.empty() || gate.empty())
	{
		std::cout << GetLogHead(__FUNCTION__)
			<< "dev network setting has empty value!"
			<< std::endl;
		LOG_ERROR("dev network setting has empty value!");
		return false;
	}

	//Checking the net interface that current available
	if(GetCurrentInterface().length() == 0)
	{
		//TODO
		std::cout << GetLogHead(__FUNCTION__)
			<< "no valid net interface can be access!" << std::endl;
		LOG_ERROR("no valid net interface can be access!");
		return false;
	}


	//如果有必要需先修改文件的访问的所有者及权限
	int fd = open(NET_SETTINGS_FILE_PATH.c_str(), O_WRONLY | O_TRUNC);
	if(fd < 0)
	{
		LOG_ERROR("open(NET_SETTINGS_FILE_PATH.c_str(), O_WRONLY | O_TRUNC) failed!");
		return false;
	}

	//    std::string fileHeader("auto lo\n"
	//                           "iface lo inet loopback\n\n"
	//                           "# The primary network interface");

	std::string interface = GetCurrentInterface();
	QString fileHeader("auto lo\n"
			"iface lo inet loopback\n\n"
			"# The primary network interface\n"
			"auto %1\n"
			"iface %1 inet static\n"
			"address %2\n"
			"netmask %3\n"
			"gateway %4\n\n\n"
			/*                       "dns-nameservers  %5 \n"
									 "                 %6"*/);

		fileHeader = fileHeader.arg(interface.c_str(), ip.c_str(), mask.c_str(), gate.c_str());
	//组装网络设置参数



	//    fileHeader = fileHeader.append("\n").append("auto ").append(interface);
	//    fileHeader = fileHeader.append("\n").append("iface ").append(interface).append(" inet static");
	//    fileHeader = fileHeader.append("\n").append("address ").append(ip);
	//    fileHeader = fileHeader.append("\n").append("netmask ").append(mask);
	//    fileHeader = fileHeader.append("\n").append("gateway ").append(gate);
	////    fileHeader = fileHeader.append("\n").append("dns-nameservers").append("\t").append(_dns).append("\t").append(_vDns);
	//    fileHeader = fileHeader.append("\n");


	if(fileHeader.length() != write(fd, fileHeader.toStdString().c_str(), fileHeader.length()))
	{
		LOG_ERROR("fileHeader.length() != write(fd, fileHeader.toStdString().c_str(), fileHeader.length()) failed!");
		return false;
	}


	return true;
	return system("/etc/init.d/networking restart") >= 0;
}


bool SysNetInterface::SetSrvNetInfo(const std::string& ip, const int port)
{
	Exectime etm(__FUNCTION__);
	//TO DO
	IKXmlHelper syscfg(GetXmlFilePath());

	if(!syscfg.IsValid())
	{
		std::cout << "[" << __FUNCTION__  << "@SysNetInterface]"
			<< "faile to load server network settings from system configuration file!"
			<< std::endl;

		return false;
	}

	std::stringstream ss;
	ss << port;

	if(syscfg.SetServerIP(ip) &&
			syscfg.SetServerPort(ss.str()) &&
			syscfg.SaveToFile())
	{
		return true;
	}
	else
	{
		std::cout << "[" << __FUNCTION__  << "@SysNetInterface]"
			<< "faile to save server network settings to system configuration file!"
			<< std::endl;

		return false;
	}
}


bool SysNetInterface::isSingalworkMode()
{
	Exectime etm(__FUNCTION__);
	IKXmlHelper syscfg(GetXmlFilePath());

	if(!syscfg.IsValid())
	{
		std::cout << "[" << __FUNCTION__  << "@SysNetInterface]"
			<< "faile to load server network settings from system configuration file!"
			<< std::endl;

		return false;
	}

	if(IKXmlHelper::DevWorkMode::OFFLINE_MODE == syscfg.GetWorkMode())
	{
		return true;
	}
	else
	{
		return false;
	}
}


int SysNetInterface::GetPowerSavingSettings()
{
	Exectime etm(__FUNCTION__);
	//TODO
	IKXmlHelper syscfg(GetXmlFilePath());
	int defaultpsTime = 30;      //默认进入省电模式的时间为30分钟
	int powerSaveTime = syscfg.GetSysPowerSaveTime();

	if(powerSaveTime > 0)
	{
		return powerSaveTime;
	}
	else
	{
		std::cout << "[" << __FUNCTION__  << "@SysNetInterface]"
			<< "faile to load server network settings from system configuration file!"
			<< std::endl;

		return defaultpsTime;
	}
}

const std::string SysNetInterface::GetSysLanguage()
{
	Exectime etm(__FUNCTION__);
	//TO DO
	IKXmlHelper syscfg(GetXmlFilePath());
	IKXmlHelper::LanguageInfo lan = syscfg.GetLanguage();

	switch(lan)
	{
		case IKXmlHelper::LanguageInfo::LANG_CN:
			return "chinese";
		case IKXmlHelper::LanguageInfo::LANG_EN:
		default:
			return "english";
	}
}

//系统设置状态更新信号
SysStatusSignal* SysStatusSignal::_sigIns = nullptr;
int SysStatusSignal::_refCount = 0;


SysStatusSignal::SysStatusSignal()
{
	Exectime etm(__FUNCTION__);
	//TO DO
}

SysStatusSignal* SysStatusSignal::GetInstance()
{
	Exectime etm(__FUNCTION__);
	if(nullptr == _sigIns)
	{
		_sigIns = new SysStatusSignal();
	}
	//更新引用计数，增加引用计数
	_refCount++;


	return _sigIns;
}

void SysStatusSignal::Release()
{
	Exectime etm(__FUNCTION__);
	//引用计数减一
	if(--_refCount)
	{
		return;
	}

	//如果该单例无外部引用则释放相应资源
	if(_sigIns)
	{
		delete _sigIns;
		_sigIns = nullptr;
	}
}

void SysStatusSignal::SysTimeUpdated()
{
	Exectime etm(__FUNCTION__);
	emit sigSysTimeUpdated();
}

