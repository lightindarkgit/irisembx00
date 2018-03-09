#include "ikconfig.h"
#include "commononqt.h"
#include "../Common/Logger.h"
#include "../Common/Exectime.h"
#include "../XML/xml/IKXmlApi.h"


IkConfig::IkConfig()
{

}

IkConfig::~IkConfig()
{
}

int IkConfig::GetDevLogoCfg()
{
    IKXml::IKXmlDocument cfgFile;
    cfgFile.LoadFromFile(GetXmlFilePath());
    std::string cfg = cfgFile.GetElementValue("/IKEMBX00/configures/logo");
    if(cfg.empty())
    {
        LOG_ERROR("not found logo node in ikembx00.xml file");
        return -1;
    }
    else
    {
        int logo = atoi(cfg.c_str());
        return logo;
    }

}

string IkConfig::GetDevComCfg()
{
    IKXml::IKXmlDocument cfgFile;
    cfgFile.LoadFromFile(GetXmlFilePath());
    std::string cfg = cfgFile.GetElementValue("/IKEMBX00/configures/devcom");
    if(cfg.empty())
    {
        LOG_ERROR("not found logo node in ikembx00.xml file");
        return "";
    }
    else
    {
        return cfg;

    }
}


int IkConfig::GetMouseCfg()
{
    IKXml::IKXmlDocument cfgFile;
    cfgFile.LoadFromFile(GetXmlFilePath());

    int bshowcursor = 0;
    std::string duration = cfgFile.GetElementValue("/IKEMBX00/configures/cursor");
    std::cout<<"get value from xml:"<<duration<<std::endl;
    if(duration.empty())
    {
        LOG_ERROR("unable to get [arlarm]/IKEMBX00/configures/cursor] duration from configure file");
    }
    else
    {
        bshowcursor = atoi(duration.c_str());
        LOG_INFO("cursor status: %s,ret: %d",duration.c_str(),bshowcursor);
    }


    return bshowcursor;

}

int IkConfig::GetRebootTime()
{
    IKXml::IKXmlDocument cfgFile;
    cfgFile.LoadFromFile(GetXmlFilePath());

    int reboottime = 365*24;  //s 默认一年
    std::string duration = cfgFile.GetElementValue("/IKEMBX00/configures/duration");
    std::cout<<"get duration value from xml:"<<duration<<std::endl;
    if(duration.empty())
    {
        LOG_ERROR("unable to get [reboottime]/IKEMBX00/configures/reboottime] duration from configure file");
    }
    else
    {
        reboottime = atoi(duration.c_str());
        LOG_INFO("reboottime is %d",reboottime);
    }

    if(reboottime<1)
    {
        reboottime = 1;
    }

    return reboottime;
}


