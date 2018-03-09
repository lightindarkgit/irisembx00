/******************************************************************************************
 ** 文件名:   commononqt.cpp
 ×× 主要类:   (无)
 **
 ** Copyright (c) 中科虹霸有限公司
 ** 创建人:   Wang.L
 ** 日  期:   2015-01-28
 ** 修改人:
 ** 日  期:
 ** 描  述:   引用了QT库的公共头文件定义
 ** ×× 主要模块说明：
 **
 ** 版  本:   1.0.0
 ** 备  注:
 **
 ******************************************************************************************/
#include "commononqt.h"
#include "../XML/xml/IKXmlApi.h"      //XML接口头文件
#include "ikxmlhelper.h"
#include "../Common/Logger.h"
#include "interaction.h"

#include <QCoreApplication>
#include <QDir>
#include <QDateTime>
#include <QTimer>
#include <QNetworkInterface>
#include <string>
#include "../Common/Exectime.h"

using namespace std;

//是否是单机工作模式
bool WorkMode::isSingleWorkMode = false;


std::map<std::string, std::string> weekDay = {{"Monday", "星期一"},
	{"Tuesday", "星期二"},
	{"Wednesday", "星期三"},
	{"Thursday", "星期四"},
	{"Friday", "星期五"},
	{"Saturday", "星期六"},
	{"Sunday", "星期日"}};


//获取XML的文件路径，请注意：该函数不做路径的存在性检查，需要调用者负责实施
const std::string GetXmlFilePath()
{
	
	QString xmlFile("%1%2%3");
	xmlFile = xmlFile.arg(QCoreApplication::applicationDirPath(), QDir::separator(), "ikembx00.xml");


	return xmlFile.toStdString();
}


//获取系统的当前时间
const std::string GetCurrentDateTime(const std::string& format)
{
	
	return QDateTime::currentDateTime().toString(format.c_str()).toStdString();
}


/*****************************************************************************
 *                       判断配置文件是否存在
 *  函 数 名：isCfgFileExist
 *  功    能：判断可执行程序所在目录中是否有配置文件存在
 *  说    明：
 *  参    数：
 *  返 回 值：存在返回true， 其他返回false
 *  创 建 人：L.Wang
 *  创建时间：2014-3-25
 *  修 改 人：
 *  修改时间：
 *****************************************************************************/
bool WorkMode::isCfgFileExist()
{
	
	//TO DO
	QDir appDir(QCoreApplication::applicationDirPath());

	return appDir.exists("ikembx00.xml");
}

bool WorkMode::IsSingleWorkMode()
{
	
	//TO DO
	//重新实现获取设备工作模式的方法
	IKXmlHelper syscfg(GetXmlFilePath());

	if(IKXmlHelper::DevWorkMode::OFFLINE_MODE == syscfg.GetWorkMode())
	{
		isSingleWorkMode = true;
	}

	return isSingleWorkMode;
}

void WorkMode::SetWorkMode(bool singleMode)
{
     Exectime etm(__FUNCTION__);
	isSingleWorkMode = singleMode;
}



//By: Wang.L @2015-01-29
//电源管理类实现（START）-----
PowerManager::PowerManager()
{
	
	//Init timer for timeout event
	po = PowerOpt::NONE;
	InitTimer();

	connect(_eventIns.GetIKEvent(), SIGNAL(sigInfraredClosed()), this, SLOT(SlotHandlePowerOpt()));
    connect(&_tmCounter, SIGNAL(timeout()),this, SLOT(SlotHandlePowerOpt()));
}


PowerManager::~PowerManager()
{
	
}


void PowerManager::Reboot()
{
	
	po = PowerOpt::REBOOT;    //Save power management command type
	disconnect(_eventIns.GetIKEvent(), SIGNAL(sigInfraredClosed()), this, SLOT(SlotHandlePowerOpt()));
	CloseInfrared();
}


void PowerManager::Shutdown()
{
	
	po = PowerOpt::SHUTDOWN;
	disconnect(_eventIns.GetIKEvent(), SIGNAL(sigInfraredClosed()), this, SLOT(SlotHandlePowerOpt()));
	CloseInfrared();
}


void PowerManager::SlotHandlePowerOpt()
{
	
	switch(po)
	{
		case PowerOpt::SHUTDOWN:
			ShutdownImp();
			break;
		case PowerOpt::REBOOT:
			RebootImp();
			break;
		default:
			break;
	}

	//Reinit member variable po
	po = PowerOpt::NONE;
}


void PowerManager::ShutdownImp()
{
	
	//TODO
	Interaction::GetInteractionInstance()->CloseUsb();
	LOG_INFO("shutdown -hf now");
	system("shutdown -hf now");
}


void PowerManager::RebootImp()
{
	
	//TO DO
	Interaction::GetInteractionInstance()->CloseUsb();
	LOG_INFO("shutdown -rf now");
	system("shutdown -rf now");
}


void PowerManager::InitTimer()
{
	
	//TODO
	//Set timeout interval as 5s
    _tmCounter.setInterval(5000);
	//Set timer single shot
    _tmCounter.setSingleShot(true);
}


void PowerManager::CloseInfrared()
{
	
	//Start timer incase of getting none response
    _tmCounter.start();

	//Sent command of close infrared
	_eventIns.GetIKEvent()->CloseInfrared();
}
//电源管理类实现（END）-----



/*****************************************************************************
 *                       获取调试日志格式化标记头
 *  函 数 名：IsConnectedToNetwork
 *  功    能：获取调试函数的函数头格式：“[yyyy-MM-dd hh:mm:ss.zzz@__FUNCTION__]”
 *  说    明：
 *  参    数：
 *  返 回 值：日志格式化标记头字符串
 *  创 建 人：L.Wang
 *  创建时间：2015-12-23
 *  修 改 人：
 *  修改时间：
 *****************************************************************************/
const std::string GetLogHead(const char *funcName)
{
	
	QString head("[%1@%2]");

	return head.arg(QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss.zzz")).arg(funcName).toStdString();
}


/*****************************************************************************
 *                        是否网络已连接
 *  函 数 名：IsConnectedToNetwork
 *  功    能：判断当前网络是否已经连接
 *  说    明：
 *  参    数：
 *  返 回 值：true:已连接    false: 其他
 *  创 建 人：L.Wang
 *  创建时间：2015-12-23
 *  修 改 人：
 *  修改时间：
 *****************************************************************************/
bool IsConnectedToNetwork()
{
	
	QList<QNetworkInterface> ifaces = QNetworkInterface::allInterfaces();
	bool isConnected = false;

	for (int i = 0; i < ifaces.count(); i++)
	{
		QNetworkInterface iface = ifaces.at(i);
		if ( iface.flags().testFlag(QNetworkInterface::IsUp)
				&& iface.flags().testFlag(QNetworkInterface::IsRunning)
				&& !iface.flags().testFlag(QNetworkInterface::IsLoopBack)
		   )
		{

#ifdef DEBUG
			// details of connection
			qDebug() << "name:" << iface.name() << endl
				<< "ip addresses:" << endl
				<< "mac:" << iface.hardwareAddress() << endl;
#endif

			// this loop is important
			for (int j=0; j<iface.addressEntries().count(); j++)
			{
#ifdef DEBUG
				qDebug() << iface.addressEntries().at(j).ip().toString()
					<< " / " << iface.addressEntries().at(j).netmask().toString() << endl;
#endif

				// we have an interface that is up, and has an ip address
				// therefore the link is present

				// we will only enable this check on first positive,
				// all later results are incorrect
				if (isConnected == false)
					isConnected = true;
			}
		}

	}

	return isConnected;
}



/*****************************************************************************
 *                        十六进制转十进制
 *  函 数 名：ConvertCardNum
 *  功    能：将输入的十六进制卡号转成十进制
 *  说    明：卡号转换规则是，高两位和低四位分别转，请参考韦根卡号转换规则
 *  参    数：
 *  返 回 值：转换之后的十进制卡号
 *  创 建 人：L.Wang
 *  创建时间：2016-3-21
 *  修 改 人：
 *  修改时间：
 *****************************************************************************/
QString ConvertCardNumToDec(QString hexValStr)
{
	
	//TODO
	QString cardIDCvtRes("");
	//如果没有卡号，直接返回空字符串
	if(hexValStr.trimmed().length() == 0)
	{
		LOG_ERROR("not find cardID");
		return cardIDCvtRes;
	}

	QString srcHexVal(hexValStr.trimmed());
	QString cardIDValL("");
	QString cardIDValH("");


	if(srcHexVal.length() > 4)
	{
		cardIDValL = srcHexVal.mid(srcHexVal.length() - 4);
		cardIDValH = srcHexVal.mid(0, srcHexVal.length() - 4);
	}
	else
	{
		cardIDValL = srcHexVal;
	}

	cardIDCvtRes = cardIDCvtRes.sprintf("%03d%05d", cardIDValH.toULong(NULL, 16), cardIDValL.toULong(NULL, 16));
	LOG_INFO("************cardIDCvtRes%s",cardIDCvtRes.toStdString().c_str());
    LOG_INFO("************hexvalstr%s",hexValStr.toStdString().c_str());

	//以下这种转换方式位数不固定，且会丢失前导0
	//cardIDCvtRes = QString::number(cardIDValH.toULong(NULL, 16)) + QString::number(cardIDValL.toULong(NULL, 16));
	printf("%scardID convertion: before(%s)  after(%s)\n", GetLogHead(__FUNCTION__).c_str(),
			hexValStr.toStdString().c_str(), cardIDCvtRes.toStdString().c_str());

	return cardIDCvtRes;
}


/*****************************************************************************
 *                        十进制转十六进制
 *  函 数 名：ConvertCardNumToHex
 *  功    能：将输入的十进制卡号转成十六进制
 *  说    明：卡号转换规则是，高两位和低四位分别转，请参考韦根卡号转换规则
 *  参    数：
 *  返 回 值：转换之后的十进制卡号
 *  创 建 人：L.Wang
 *  创建时间：2016-3-21
 *  修 改 人：
 *  修改时间：
 *****************************************************************************/
QString ConvertCardNumToHex(QString decValStr)
{
	
	//TODO因为暂时还没有找到合理的分解方法，所以暂时通过缓存来实现
	QString cardIDCvtRes("");


	if(decValStr.trimmed().isEmpty())
	{
		LOG_ERROR("not find cardID");
		return cardIDCvtRes;
	}

	QString decCIDL;
	QString decCIDH;

	if(decValStr.length() > 5)
	{
		decCIDL = decValStr.mid(decValStr.length() - 5);
		decCIDH = decValStr.mid(0, decValStr.length() - 5);
	}
	else
	{
		decCIDL = decValStr;
	}

	printf("%scardID convertion: decCIDL(%s)  decCIDH(%s)\n", GetLogHead(__FUNCTION__).c_str(),
			decCIDL.toStdString().c_str(), decCIDH.toStdString().c_str());

	cardIDCvtRes = cardIDCvtRes.sprintf("%02X%04X", decCIDH.toULong(NULL, 10), decCIDL.toULong(NULL, 10));
	printf("%scardID convertion: before(%s)  after(%s)\n", GetLogHead(__FUNCTION__).c_str(),
			decValStr.toStdString().c_str(), cardIDCvtRes.toStdString().c_str());


	return cardIDCvtRes;
}

int CheckCardIDAndGen(IkUuid personID)
{
	
	// 如果是在线模式，不进行处理
	if(WorkMode::IsSingleWorkMode() == false)
	{
		return 0;
	}

	DatabaseAccess dbAccess;
	string queryString;
	std::ostringstream oss;

	vector<int>     idvec;

	string uid = personID.Unparse();

	oss<<"SELECT id FROM person  where  (cardid is null or trim(cardid) = '')";
	if(uid.compare("00000000-0000-0000-0000-000000000000") != 0)
	{
		// 更新person表的CardID
		oss << " and personid = '";
		oss << personID.Unparse();
		oss << "'";
	}

	queryString = oss.str();
	int nret = dbAccess.Query(queryString,idvec);
	if(nret != dbSuccess)
	{
		LOG_ERROR("query sql failed. %s",queryString.c_str());
		return -1;
	}

	// 生成的卡号从100开始,所以从自增字段上+100即可,如果修改基础值,改变100即可
	//int  addnum = 0;

	//guodj
	//修改为id创建时种子值从100开始，此处不再对id进行处理

	string cardidstr = "";
	for(int i = 0 ; i < idvec.size(); i++)
	{
		ConvertCardIdInt2String(idvec[i] , cardidstr);
		oss.str("");

		oss << " update person set cardid = '";
		oss << cardidstr;
		oss << "' where id = ";
		oss << idvec[i];

		queryString = oss.str();
		nret = dbAccess.Query(queryString);
		if(nret != dbSuccess)
		{
			LOG_ERROR("query sql failed. %s",queryString.c_str());
			return -1;
		}
	}

	return 0;
}

int ConvertCardIdInt2String(long cardIdInt, std::string& cardIdString)
{
	
	int high, low;
	high = (cardIdInt >> 16) & 0xff;
	low = (cardIdInt)& 0xffff;
	std::stringstream oss;
	oss << std::dec << std::setw(3) << std::setfill('0');
	oss << high;
	oss << std::dec << std::setw(5) << std::setfill('0');
	oss << low;

	cardIdString = oss.str();
}

