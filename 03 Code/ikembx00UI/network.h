/*****************************************************************************
** 文 件 名：network.h
** 主 要 类：NetWork
**
** Copyright (c) 中科虹霸有限公司
**
** 创 建 人：Wang.L
** 创建时间：2014-01-08
**
** 修 改 人：
** 修改时间：
** 描  述:   网络配置管理类设置IP地址、子网掩码、网关和DNS等，并通过重启网络服务使设置生效
** 主要模块说明:
**
** 版  本:   1.0.0
** 备  注:
**
*****************************************************************************/
#pragma once
#include <string>
#include <map>
#include <list>

//IP获取方式
enum IPGetMode{autoGet = 0, staticSet = 1};
//IP版本
enum IPVersion{ipV4 = 0, ipV6 = 1};

//bool isValid(const std::string addr);

class NetWork
{
public:
    NetWork();
    ~NetWork();

    bool UpdateToSystem(IPGetMode ipMode);


    bool SetInterfaceName(const std::string& ifaName);
    bool SetIP(const std::string& ipAddr);
    bool SetNetMask(const std::string& netMask);
    bool SetGateWay(const std::string& gateWay);
    bool SetDNS(const std::string& dns);
    bool SetViceDNS(const std::string& vDns);

    std::string GetInterfaceName();
    std::string GetIP();
    std::string GetNetMask();
    std::string GetGateWay();
    std::string GetDNS();
    std::string GetViceDNS();


protected:
    bool SetStaticMode();
    bool SetDNS();
    bool SetDHCPMode();


private:
    static const std::string _netSettingFilePath;
    static const std::string _dnsSettingFilePath;
//    std::map<std::string, std::string> _addrs;
    std::string _ip;
    std::string _netMask;
    std::string _gateWay;
    std::string _dns;
    std::string _vDns;
    std::string _interfaceName;
};
