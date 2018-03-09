#include <sstream>
#include "ikxmlhelper.h"
#include "commononqt.h"
#include "../XML/xml/IKXmlApi.h"
#include "../Common/Logger.h"
#include "../Common/Exectime.h"

#define MAX_ALARMERS    8  //The max number of alarmers that is allowed
using namespace std;

IKXmlHelper::IKXmlHelper(const std::string & cfgFilePath) :
	_xfile(nullptr),
	_fileName(cfgFilePath)
{
	
	//TO DO
	_xfile = new IKXml::IKXmlDocument();
	if(! _xfile->LoadFromFile(GetXmlFilePath()))
	{
		delete _xfile;
		_xfile = nullptr;

		std::cout << "[" << __FUNCTION__  << "]"
			<< "Cannot open config file:" << GetXmlFilePath()
			<< std::endl;
		LOG_ERROR("Cannot open config file: %s",GetXmlFilePath().c_str());
	}
}


IKXmlHelper::~IKXmlHelper()
{
	
	//TO DO
	//    if(_needSave && ! _xfile->SaveToFile(GetXmlFilePath()))
	//    {
	//        std::cout << "[" << __FUNCTION__  << "]"
	//                  << "Cannot save update infomation to :" << GetXmlFilePath()
	//                  << std::endl;
	//    }

	delete _xfile;
}


bool IKXmlHelper::CreatXmlFile(const string &filePath)
{
	
	_xfile = new IKXml::IKXmlDocument();
    if(nullptr != _xfile )
    {
        return _xfile->SaveToFile(filePath);
    }
    LOG_ERROR("创建Xml文件失败");

    return false;
}

IKXmlHelper::DevWorkMode  IKXmlHelper::GetWorkMode()
{
	
	//TODO
	if(!_xfile)
	{
		std::cout << "[" << __FUNCTION__  << "]"
			<< "Cannot find workmode node in configure file " << GetXmlFilePath()
			<< std::endl;

		LOG_ERROR("Cannot find workmode node in configure file: %s",GetXmlFilePath().c_str());
		return DevWorkMode::UNKNOWN_MODE;
	}

	std::string workMode =  _xfile->GetElementValue("/IKEMBX00/configures/workmode");
	if("online" == workMode)
	{
		LOG_INFO("workMode online");
		return DevWorkMode::ONLINE_MODE;
	}
	else if("offline" == workMode)
	{
		LOG_INFO("workMode offline");
		return DevWorkMode::OFFLINE_MODE;
	}

	return DevWorkMode::UNKNOWN_MODE;
}


//It is not as meanful as it conveying
bool IKXmlHelper::SetWorkMode(const DevWorkMode newMode/* = UNKNOWN_MODE*/)
{
	
	//TO DO
	if(!_xfile)
	{
		std::cout << "[" << __FUNCTION__  << "]"
			<< "Cannot find workmode node in configure file " << GetXmlFilePath()
			<< std::endl;

		LOG_ERROR("Cannot find workmode node in configure file: %s",GetXmlFilePath().c_str());
		return false;
	}

	//Verify target node's existence
	if(!_CheckNodeExist("/IKEMBX00/configures/workmode"))
	{
		_AddNewNode("/IKEMBX00/configures/workmode");
	}

	bool result = _xfile->SetElementValue("/IKEMBX00/configures/workmode",
			newMode == DevWorkMode::OFFLINE_MODE ? "offline" :
			newMode == DevWorkMode::ONLINE_MODE ? "online" : "online"
			);
	result &= _xfile->SaveToFile(GetXmlFilePath());

	return result;

}

const std::string IKXmlHelper::GetServerIP()
{
	
	//TODO
	if(!_xfile)
	{
		std::cout << "[" << __FUNCTION__  << "]"
			<< "Cannot find workmode node in configure file " << GetXmlFilePath()
			<< std::endl;

		LOG_ERROR("Cannot find workmode node in configure file: %s",GetXmlFilePath().c_str());
		return std::string("");
	}


	return _xfile->GetElementValue("/IKEMBX00/configures/server/ip");
}


bool IKXmlHelper::SetServerIP(const std::string &srvIP)
{
	
	if(!_xfile)
	{
		std::cout << "[" << __FUNCTION__  << "]"
			<< "Cannot find workmode node in configure file " << GetXmlFilePath()
			<< std::endl;

		LOG_ERROR("Cannot find workmode node in configure file: %s",GetXmlFilePath().c_str());
		return false;
	}

	//Verify target node's existence
	if(!_CheckNodeExist("/IKEMBX00/configures/server/ip"))
	{
		_AddNewNode("/IKEMBX00/configures/server/ip");
	}

	return  _xfile->SetElementValue("/IKEMBX00/configures/server/ip", srvIP);
}

const std::string IKXmlHelper::GetServerPort()
{
	
	if(!_xfile)
	{
		std::cout << "[" << __FUNCTION__  << "]"
			<< "Cannot find workmode node in configure file " << GetXmlFilePath()
			<< std::endl;

		LOG_ERROR("Cannot find workmode node in configure file: %s",GetXmlFilePath().c_str());
		return std::string("");
	}


	return _xfile->GetElementValue("/IKEMBX00/configures/server/port");
}


bool IKXmlHelper::SetServerPort(const std::string &srvPort)
{
	
	if(!_xfile)
	{
		std::cout << "[" << __FUNCTION__  << "]"
			<< "Cannot find workmode node in configure file " << GetXmlFilePath()
			<< std::endl;

		LOG_ERROR("Cannot find workmode node in configure file: %s",GetXmlFilePath().c_str());
		return false;
	}

	//Verify target node's existence
	if(!_CheckNodeExist("/IKEMBX00/configures/server/port"))
	{
		_AddNewNode("/IKEMBX00/configures/server/port");
	}

	return  _xfile->SetElementValue("/IKEMBX00/configures/server/port", srvPort);
}


const std::map<std::string, std::string> IKXmlHelper::GetAlarms()
{
	
	//TO DO
	std::map<std::string, std::string> alms;
	std::string alarmTags[MAX_ALARMERS] = {"alarm1st","alarm2nd","alarm3rd","alarm4th","alarm5th","alarm6th","alarm7th","alarm8th"};
	//TODO
	if(!_xfile)
	{
		std::cout << "[" << __FUNCTION__  << "]"
			<< "Cannot find alarms node in configure file " << GetXmlFilePath()
			<< std::endl;

		LOG_ERROR("Cannot find alarms node in configure file: %s",GetXmlFilePath().c_str());
		return alms;
	}

	//Fetch each alarms from xml file
	for(auto iter = 0; iter < MAX_ALARMERS; ++iter)
	{
		alms[alarmTags[iter]] = _xfile->GetElementValue(std::string("/IKEMBX00/configures/alarm/") + alarmTags[iter]);
	}

	return alms;
}


bool IKXmlHelper::SetAlarms(const std::map<std::string, std::string> &alms)
{
	
	if(!_xfile)
	{
		std::cout << "[" << __FUNCTION__  << "]"
			<< "Cannot find alarm node in configure file " << GetXmlFilePath()
			<< std::endl;

		LOG_ERROR("Cannot find alarms node in configure file: %s",GetXmlFilePath().c_str());
		return false;
	}

	//    std::string alarmTags[MAX_ALARMERS] = {"alarm1st","alarm2nd","alarm3rd","alarm4th","alarm5th","alarm6th","alarm7th","alarm8th"};
	for(auto iter = alms.begin(); iter != alms.end(); ++iter)
	{
		//        std::string alarmTag(std::string("/IKEMBX00/configures/alarm/") + iter->first);
		//        //Verify target node's existence
		//        if(!_CheckNodeExist(alarmTag))
		//        {
		//            _AddNewNode(alarmTag);
		//        }

		//        _xfile->SetElementValue(alarmTag, iter->second);


		if(_xfile->Exist(std::string("/IKEMBX00/configures/alarm/") + iter->first))
		{
			_xfile->SetElementValue(std::string("/IKEMBX00/configures/alarm/") + iter->first, iter->second);
		}
		else
		{
			return false;
		}
	}

	return true;
}


IKXmlHelper::LanguageInfo IKXmlHelper::GetLanguage()
{
	
	//TO DO
	if(!_xfile)
	{
		std::cout << "[" << __FUNCTION__  << "]"
			<< "Cannot find language node in configure file " << GetXmlFilePath()
			<< std::endl;

		LOG_ERROR("Cannot find language node in configure file: %s",GetXmlFilePath().c_str());
		return IKXmlHelper::LanguageInfo::UNKNOWN_LANG;
	}

	std::string  sysLang = _xfile->GetElementValue("/IKEMBX00/configures/language");
	if("english" == sysLang)
	{
		return  IKXmlHelper::LanguageInfo::LANG_EN;
	}
	else if("chinese" == sysLang)
	{
		return IKXmlHelper::LanguageInfo::LANG_CN;
	}
	else
	{
		return IKXmlHelper::LanguageInfo::UNKNOWN_LANG;
	}

}

bool IKXmlHelper::SetLanguage(const IKXmlHelper::LanguageInfo newLang)
{
	
	//TO DO
	if(!_xfile)
	{
		std::cout << "[" << __FUNCTION__  << "]"
			<< "Cannot find language node in configure file " << GetXmlFilePath()
			<< std::endl;

		LOG_ERROR("Cannot find language node in configure file: %s",GetXmlFilePath().c_str());
		return false;
	}

	//Verify target node's existence
	if(!_CheckNodeExist("/IKEMBX00/configures/language"))
	{
		_AddNewNode("/IKEMBX00/configures/language");
	}


	std::map< IKXmlHelper::LanguageInfo, std::string> langMap = {{IKXmlHelper::LanguageInfo::LANG_CN, "chinese"}, {IKXmlHelper::LanguageInfo::LANG_EN, "english"}};

	return _xfile->SetElementValue("/IKEMBX00/configures/language", langMap[newLang]);
}

int IKXmlHelper::GetAllowedLoginDelay()
{
	
	//TO DO
	if(!_xfile)
	{
		std::cout << "[" << __FUNCTION__  << "]"
			<< "Cannot find login delay time node in configure file " << GetXmlFilePath()
			<< std::endl;

		LOG_ERROR("Cannot find login delay time mode in configure file: %s",GetXmlFilePath().c_str());
		return -1;
	}

	std::string loginDelay = _xfile->GetElementValue("/IKEMBX00/configures/login/downtime");
	if(loginDelay.empty())
	{
		return -1;
	}

	return  atoi(loginDelay.c_str());
}

bool IKXmlHelper::SetAllowedLoginDelay(const int secs)
{
	
	if(!_xfile)
	{
		std::cout << "[" << __FUNCTION__  << "]"
			<< "configure file does not exist!!" << GetXmlFilePath()
			<< std::endl;

		LOG_ERROR("configure file does not exist!!: %s",GetXmlFilePath().c_str());
		return -1;
	}

	std::string path("/IKEMBX00/configures/login/downtime");
	//Verify target node's existence
	if(!_CheckNodeExist(path))
	{
		_AddNewNode(path);
	}

	std::stringstream ss;
	ss << secs;

	return _xfile->SetElementValue(path, ss.str());

}

int IKXmlHelper::GetSysPowerSaveTime()
{
	
	//TO DO
	if(!_xfile)
	{
		std::cout << "[" << __FUNCTION__  << "]"
			<< "Cannot find power save time node in configure file " << GetXmlFilePath()
			<< std::endl;

		LOG_ERROR("Cannot find power save time node in configure file: %s",GetXmlFilePath().c_str());
		return -1;
	}

	std::string powerSaveTime = _xfile->GetElementValue("/IKEMBX00/configures/systemsleep");
	if(powerSaveTime.empty())
	{
		return -1;
	}

	return  atoi(powerSaveTime.c_str());
}

bool IKXmlHelper::SetSysPowerSaveTime(const int mins)
{
	
	if(!_xfile)
	{
		std::cout << "[" << __FUNCTION__  << "]"
			<< "configure file does not exist!!" << GetXmlFilePath()
			<< std::endl;

		LOG_ERROR("configure file does not exist!!: %s",GetXmlFilePath().c_str());
		return -1;
	}

	std::string path("/IKEMBX00/configures/systemsleep");
	//Verify target node's existence
	if(!_CheckNodeExist(path))
	{
		_AddNewNode(path);
	}

	std::stringstream ss;
	ss << mins;

	return _xfile->SetElementValue(path, ss.str());
}

int IKXmlHelper::GetAlarmDuration()
{
	
	//TO DO
	if(!_xfile)
	{
		std::cout << "[" << __FUNCTION__  << "]"
			<< "Cannot find alarm duration node in configure file " << GetXmlFilePath()
			<< std::endl;

		LOG_ERROR("Cannot find alarm duration node in configure file: %s",GetXmlFilePath().c_str());
		return -1;
	}

	std::string alarmDuration = _xfile->GetElementValue("/IKEMBX00/configures/alarmduration");
	if(alarmDuration.empty())
	{
		return -1;
	}

	return  atoi(alarmDuration.c_str());
}

bool IKXmlHelper::SetAlarmDuration(const int secs)
{
	
	if(!_xfile)
	{
		std::cout << "[" << __FUNCTION__  << "]"
			<< "configure file does not exist!!" << GetXmlFilePath()
			<< std::endl;

		LOG_ERROR("configure file does not exist!!: %s",GetXmlFilePath().c_str());
		return -1;
	}

	std::string path("/IKEMBX00/configures/alarmduration");
	//Verify target node's existence
	if(!_CheckNodeExist(path))
	{
		_AddNewNode(path);
	}

	std::stringstream ss;
	ss << secs;

	return _xfile->SetElementValue(path, ss.str());
}

int IKXmlHelper::GetDoorBellDuration()
{
	
	//TO DO
	if(!_xfile)
	{
		std::cout << "[" << __FUNCTION__  << "]"
			<< "Cannot find alarm duration node in configure file " << GetXmlFilePath()
			<< std::endl;

		LOG_ERROR("Cannot find alarm duration node in configure file: %s",GetXmlFilePath().c_str());
		return -1;
	}

	std::string doorBellDuration = _xfile->GetElementValue("/IKEMBX00/configures/identify/belltime");
	if(doorBellDuration.empty())
	{
		return 0;
	}

	return  atoi(doorBellDuration.c_str());
}

bool IKXmlHelper::SetDoorBellDuration(const int secs)
{
	
	if(!_xfile)
	{
		std::cout << "[" << __FUNCTION__  << "]"
			<< "configure file does not exist!!" << GetXmlFilePath()
			<< std::endl;

		LOG_ERROR("configure file does not exist!!: %s",GetXmlFilePath().c_str());
		return -1;
	}

	std::string path("/IKEMBX00/configures/identify/belltime");
	//Verify target node's existence
	if(!_CheckNodeExist(path))
	{
		_AddNewNode(path);
	}

	std::stringstream ss;
	ss << secs;

	return _xfile->SetElementValue(path, ss.str());
}

IKXmlHelper::IDenMode IKXmlHelper::GetIdenMode()
{
	
	//TO DO
	if(!_xfile)
	{
		std::cout << "[" << __FUNCTION__  << "]"
			<< "Cannot find iden mode node in configure file " << GetXmlFilePath()
			<< std::endl;

		LOG_ERROR("Cannot find iden mode node in configure file: %s",GetXmlFilePath().c_str());
		return IKXmlHelper::IDenMode::UNKNOWN_MODE;
	}

	std::string cardMode    =  _xfile->GetElementValue("/IKEMBX00/configures/identify/identmode/card");
	std::string irisMode    =  _xfile->GetElementValue("/IKEMBX00/configures/identify/identmode/iris");
	//fjf 2015-08-26 读取新的识别模式
	std::string cOririsMode =  _xfile->GetElementValue("/IKEMBX00/configures/identify/identmode/cardoriris");

	//fjf 2015-08-26处理增加新模式：虹膜或者刷卡
	if ("true" == cOririsMode)
	{
		return IKXmlHelper::IDenMode::IRIS_OR_CARD;
	}
	else
	{
		//2015-08-26 keep intact
		if("true" == irisMode)
		{
			if("true" == cardMode)
			{
				return IKXmlHelper::IDenMode::IRIS_AND_CARD;
			}

			return IKXmlHelper::IDenMode::IRIS_ONLY;
		}
		else
		{
			if("true" == cardMode)
			{
				return IKXmlHelper::IDenMode::CARD_ONLY;
			}

			return IKXmlHelper::IDenMode::UNKNOWN_MODE;
		}
	}

}

bool IKXmlHelper::SetIdenMode(IKXmlHelper::IDenMode mod)
{
	
	if(!_xfile)
	{
		std::cout << "[" << __FUNCTION__  << "]"
			<< "configure file does not exist!!" << GetXmlFilePath()
			<< std::endl;

		LOG_ERROR("configure file does not exist!!: %s",GetXmlFilePath().c_str());
		return -1;
	}

	std::string cardPath("/IKEMBX00/configures/identify/identmode/card");
	std::string irisPath("/IKEMBX00/configures/identify/identmode/iris");
	///////////////////////////////
	// added at 20150826 by yqhe
	// 增加刷卡或虹膜识别方式
	std::string cardOrIrisPath("/IKEMBX00/configures/identify/identmode/cardoriris");
	///////////////////////////////


	//Verify target node's existence
	if(!_CheckNodeExist(cardPath))
	{
		_AddNewNode(cardPath);
	}

	if(!_CheckNodeExist(irisPath))
	{
		_AddNewNode(irisPath);
	}

	///////////////////////////////
	// added at 20150826 by yqhe
	// 如果node不存在，则需要主动创建
	if(!_CheckNodeExist(cardOrIrisPath))
	{
		_AddNewNode(cardOrIrisPath);
	}

	// 设置初始值
	_xfile->SetElementValue(irisPath, "false");
	_xfile->SetElementValue(cardPath, "false");
	_xfile->SetElementValue(cardOrIrisPath, "false");
	///////////////////////////////

	switch(mod)
	{
		case IDenMode::IRIS_ONLY:
			return  _xfile->SetElementValue(irisPath, "true") && _xfile->SetElementValue(cardPath, "false");
		case IDenMode::CARD_ONLY:
			return  _xfile->SetElementValue(irisPath, "false") && _xfile->SetElementValue(cardPath, "true");
		case IDenMode::IRIS_AND_CARD:
			return  _xfile->SetElementValue(irisPath, "true") && _xfile->SetElementValue(cardPath, "true");

			///////////////////////////////
			// added at 20150826 by yqhe
			// 增加刷卡或虹膜识别方式
		case IDenMode::IRIS_OR_CARD:
			return  _xfile->SetElementValue(cardOrIrisPath, "true");
			///////////////////////////////

		case IDenMode::UNKNOWN_MODE:
		default:
			return  _xfile->SetElementValue(irisPath, "true") && _xfile->SetElementValue(cardPath, "false");
	}
}

IKXmlHelper::OutputInfo IKXmlHelper::GetOutputTo()
{
	
	//TO DO
	if(!_xfile)
	{
		std::cout << "[" << __FUNCTION__  << "]"
			<< "Cannot find output end info node in configure file " << GetXmlFilePath()
			<< std::endl;

		LOG_ERROR("Cannot find output end info node in configure file: %s",GetXmlFilePath().c_str());
		return IKXmlHelper::OutputInfo:: UNKNOWN_OUTPUT;
	}

	std::string toWiegand =  _xfile->GetElementValue("/IKEMBX00/configures/identify/outwiegand");
	std::string toRelay =  _xfile->GetElementValue("/IKEMBX00/configures/identify/outio");

    LOG_DEBUG("wiegand = %s",toWiegand.c_str());

    if("true" == toWiegand)
	{
		if("true" == toRelay)
		{
            return IKXmlHelper::OutputInfo::TO_WIEGAND_AND_RELAY;
		}

        return IKXmlHelper::OutputInfo::TO_WIEGAND;

	}
    else if("off" == toWiegand)
	{
		if("true" == toRelay)
		{
			return IKXmlHelper::OutputInfo::TO_RELAY;
		}

		return IKXmlHelper::OutputInfo::TO_NONE;
	}
}


bool IKXmlHelper::SetOutputTo(IKXmlHelper::OutputInfo output)
{
	
	if(!_xfile)
	{
		std::cout << "[" << __FUNCTION__  << "]"
			<< "configure file does not exist!!" << GetXmlFilePath()
			<< std::endl;

		LOG_ERROR("configure file does not exist!!: %s",GetXmlFilePath().c_str());
		return -1;
	}

	std::string wgnPath("/IKEMBX00/configures/identify/outwiegand");
	std::string rlyPath("/IKEMBX00/configures/identify/outio");
	//Verify target node's existence
	if(!_CheckNodeExist(wgnPath))
	{
		_AddNewNode(wgnPath);
	}

	if(!_CheckNodeExist(rlyPath))
	{
		_AddNewNode(rlyPath);
	}

	switch(output)
	{
        case OutputInfo::TO_WIEGAND:
            return  _xfile->SetElementValue(wgnPath, "true") && _xfile->SetElementValue(rlyPath, "false");
		case OutputInfo::TO_RELAY:
			return  _xfile->SetElementValue(wgnPath, "false") && _xfile->SetElementValue(rlyPath, "true");
        case OutputInfo::TO_WIEGAND_AND_RELAY:
            return  _xfile->SetElementValue(wgnPath, "true") && _xfile->SetElementValue(rlyPath, "true");
		case OutputInfo::TO_NONE:
		case OutputInfo::UNKNOWN_OUTPUT:
		default:
			return  _xfile->SetElementValue(wgnPath, "false") && _xfile->SetElementValue(rlyPath, "false");
	}
}


/*****************************************************************************
 *                      获取当前城市名称
 *  函 数 名：GetCityName
 *  功    能：获取系统显示的天气所对应的城市名称
 *  说    明：
 *  参    数：
 *  返 回 值：成功返回当前城市信息，失败返回空字符串
 *  创 建 人：L.Wang
 *  创建时间：2015-05-13
 *  修 改 人：
 *  修改时间：
 *****************************************************************************/
const std::string IKXmlHelper::GetCityName()
{
	

	//TODO
	if(!_xfile)
	{
		std::cout << "[" << __FUNCTION__  << "]"
			<< "Cannot find output end info node in configure file " << GetXmlFilePath()
			<< std::endl;

		LOG_ERROR("Cannot find output end info node in configure file : %s",GetXmlFilePath().c_str());
		return "";
	}

	return _xfile->GetElementValue("/IKEMBX00/configures/city");
}
/*****************************************************************************
 *                      设置当前城市名称
 *  函 数 名：SetCityName
 *  功    能：设置系统显示的天气所对应的城市名称
 *  说    明：
 *  参    数：city, 当前城市名称，不能为空
 *  返 回 值：成功返回true，其他返回false
 *  创 建 人：L.Wang
 *  创建时间：2015-05-13
 *  修 改 人：
 *  修改时间：
 *****************************************************************************/
bool IKXmlHelper::SetCityName(const std::string& city)
{
	
	//TODO
	if(!_xfile)
	{
		std::cout << "[" << __FUNCTION__  << "]"
			<< "configure file does not exist!!" << GetXmlFilePath()
			<< std::endl;

		LOG_ERROR("configure file does not exist!!: %s",GetXmlFilePath().c_str());
		return -1;
	}

	std::string cityPath("/IKEMBX00/configures/city");
	//Verify target node's existence
	if(!_CheckNodeExist(cityPath))
	{
		_AddNewNode(cityPath);
	}


	return  _xfile->SetElementValue(cityPath, city);
}


bool IKXmlHelper::SaveToFile(const std::string &fileName)
{
	
	return _xfile->SaveToFile(fileName);
}

bool IKXmlHelper::SaveToFile()
{
	
	return SaveToFile(_fileName);
}


bool IKXmlHelper::IsValid()
{
	
	return nullptr != _xfile;
}

bool IKXmlHelper::_CheckNodeExist(const string &node)
{
	
	return _xfile->Exist(node);
}

bool IKXmlHelper::_AddNewNode(const std::string &node)
{
	
	//Get the parent path and check its existence, or create it as need
	std::string pPath(node.substr(0, node.find_last_of("/")));


	if(!_CheckNodeExist(pPath))
	{
		_AddNewNode(pPath);
	}
	else
	{
		_xfile->AddElement(pPath, node.substr(pPath.length() + strlen("/"), node.length()));
	}

	return true;
}

