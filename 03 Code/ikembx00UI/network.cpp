/*****************************************************************************
** 文 件 名：network.cpp
** 主 要 类：NetWork
**
** Copyright (c) 中科虹霸有限公司
**
** 创 建 人：王磊
** 创建时间：2014-01-08
**
** 修 改 人：
** 修改时间：
** 描  述:   设置网络地址和DNS,以及重新启动网络服务以使网络配置生效
** 主要模块说明:
**
** 版  本:   1.0.0
** 备  注:
**
*****************************************************************************/
//引用标准库头文件
#include <string>
#include <stdio.h>
#include <fcntl.h>
#include <stdlib.h>
#include <unistd.h>

//引用类头文件
#include "network.h"

#include <QRegExp>

//引用标准命名空间
using namespace std;


const std::string NetWork::_netSettingFilePath = "/etc/network/interfaces";
const std::string NetWork::_dnsSettingFilePath = "/etc/resolv.conf";

NetWork::NetWork()
{
}

NetWork::~NetWork()
{
}

/*****************************************************************************
*                        设置网卡名称
*  函 数 名：SetInterfaceName
*  功    能：
*  说    明：
*  参    数：
*  返 回 值：
*  创 建 人：王磊
*  创建时间：2014-02-08
*  修 改 人：
*  修改时间：
*****************************************************************************/
bool NetWork::SetInterfaceName(const string &ifaName)
{
    //TO DO
    _interfaceName = ifaName;
}

/*****************************************************************************
*                        设置IP地址
*  函 数 名：SetIP
*  功    能：
*  说    明：
*  参    数：
*  返 回 值：
*  创 建 人：王磊
*  创建时间：2014-01-08
*  修 改 人：
*  修改时间：
*****************************************************************************/
bool NetWork::SetIP(const std::string &ipAddr)
{
    //TO DO
//    isValid(ipAddr);
    _ip = ipAddr;
//    _addrs["address"] = ipAddr;
}

/*****************************************************************************
*                        设置子网掩码
*  函 数 名：SetNetMaskt
*  功    能：
*  说    明：
*  参    数：
*  返 回 值：
*  创 建 人：王磊
*  创建时间：2014-01-08
*  修 改 人：
*  修改时间：
*****************************************************************************/
bool NetWork::SetNetMask(const std::string &netMask)
{
    //TO DO
//    isValid(netMask);
    _netMask = netMask;
//    _addrs["netmask"] = netMask;
}

/*****************************************************************************
*                        设置网关
*  函 数 名：SetGateWay
*  功    能：
*  说    明：
*  参    数：
*  返 回 值：
*  创 建 人：王磊
*  创建时间：2014-01-08
*  修 改 人：
*  修改时间：
*****************************************************************************/
bool NetWork::SetGateWay(const std::string &gateWay)
{
    //TO DO
//    isValid(gateWay);
    _gateWay = gateWay;
//    _addrs["gateway"] = gateWay;
}

/*****************************************************************************
*                        设置DNS
*  函 数 名：SetDNS
*  功    能：
*  说    明：
*  参    数：
*  返 回 值：
*  创 建 人：王磊
*  创建时间：2014-01-08
*  修 改 人：
*  修改时间：
*****************************************************************************/
bool NetWork::SetDNS(const std::string& dns)
{
    //TO DO
    //isValid(dns);
    _dns = dns;
//    _addrs["nameserver"] = dns;
}

/*****************************************************************************
*                        设置备用DNS
*  函 数 名：SetViceDNS
*  功    能：
*  说    明：
*  参    数：
*  返 回 值：
*  创 建 人：王磊
*  创建时间：2014-01-08
*  修 改 人：
*  修改时间：
*****************************************************************************/
bool NetWork::SetViceDNS(const string &vDns)
{
    //TO DO
    _vDns = vDns;
}

/*****************************************************************************
*                        获取网卡名称
*  函 数 名：GetInterfaceName
*  功    能：
*  说    明：
*  参    数：
*  返 回 值：
*  创 建 人：王磊
*  创建时间：2014-02-08
*  修 改 人：
*  修改时间：
*****************************************************************************/
std::string NetWork::GetInterfaceName()
{
    //TO DO
    return _interfaceName;
}

/*****************************************************************************
*                        获取IP
*  函 数 名：GetIP
*  功    能：
*  说    明：
*  参    数：
*  返 回 值：
*  创 建 人：王磊
*  创建时间：2014-01-08
*  修 改 人：
*  修改时间：
*****************************************************************************/
std::string NetWork::GetIP()
{
    return _ip;
}

/*****************************************************************************
*                        获取子网掩码
*  函 数 名：GetNetMask
*  功    能：
*  说    明：
*  参    数：
*  返 回 值：
*  创 建 人：王磊
*  创建时间：2014-01-08
*  修 改 人：
*  修改时间：
*****************************************************************************/
std::string NetWork::GetNetMask()
{
    return _netMask;
}

/*****************************************************************************
*                        获取网关
*  函 数 名：GetGateWay
*  功    能：
*  说    明：
*  参    数：
*  返 回 值：
*  创 建 人：王磊
*  创建时间：2014-01-08
*  修 改 人：
*  修改时间：
*****************************************************************************/
std::string NetWork::GetGateWay()
{
    return _gateWay;
}

/*****************************************************************************
*                        获取DNS
*  函 数 名：GetDNS
*  功    能：获取DNS
*  说    明：
*  参    数：
*  返 回 值：
*  创 建 人：王磊
*  创建时间：2014-01-08
*  修 改 人：
*  修改时间：
*****************************************************************************/
std::string NetWork::GetDNS()
{
    return _dns;
}


std::string NetWork::GetViceDNS()
{
    return _vDns;
}

/*****************************************************************************
*                        设置动态IP分配模式
*  函 数 名：SetDHCPMode
*  功    能：
*  说    明：
*  参    数：
*  返 回 值：
*  创 建 人：王磊
*  创建时间：2014-01-08
*  修 改 人：
*  修改时间：
*****************************************************************************/
bool NetWork::SetDHCPMode()
{
    int fd = -1;
    //如果有必要需先修改文件的访问的所有者及权限
    fd = open(_netSettingFilePath.c_str(), O_WRONLY |O_TRUNC);
    if(fd < 0)
    {
        return false;
    }

    string fileHeader("auto lo\n"
                    "iface lo inet loopback\n"
                    "#The primary network interface - use DHCP to find our address\n\n");
    fileHeader = fileHeader.append("\n").append("auto ").append(_interfaceName);
    fileHeader = fileHeader.append("\n").append("iface ").append(_interfaceName).append(" inet dhcp");
    fileHeader = fileHeader.append("\n");

    if(fileHeader.length() != write(fd, fileHeader.c_str(), fileHeader.length()))
    {
        return false;
    }

    system("/etc/init.d/networking restart");


    return true;
}

/*****************************************************************************
*                        设置静态网络IP
*  函 数 名：SetStaticMode
*  功    能：设置静态网络IP
*  说    明：
*  参    数：
*  返 回 值：
*  创 建 人：王磊
*  创建时间：2014-01-08
*  修 改 人：
*  修改时间：
*****************************************************************************/
bool NetWork::SetStaticMode()
{
    int fd = -1;
    //如果有必要需先修改文件的访问的所有者及权限
    fd = open(_netSettingFilePath.c_str(), O_WRONLY | O_TRUNC);
    if(fd < 0)
    {
        return false;
    }

    string fileHeader("auto lo\n"
                      "iface lo inet loopback\n\n"
                      "# The primary network interface");
    //组装网络设置参数
    fileHeader = fileHeader.append("\n").append("auto ").append(_interfaceName);
    fileHeader = fileHeader.append("\n").append("iface ").append(_interfaceName).append(" inet static");
    fileHeader = fileHeader.append("\n").append("address ").append(_ip);
    fileHeader = fileHeader.append("\n").append("netmask ").append(_netMask);
    fileHeader = fileHeader.append("\n").append("gateway ").append(_gateWay);
    fileHeader = fileHeader.append("\n").append("dns-nameservers").append("\t").append(_dns).append("\t").append(_vDns);
    fileHeader = fileHeader.append("\n");


    if(fileHeader.length() != write(fd, fileHeader.c_str(), fileHeader.length()))
    {
        return false;
    }


    return system("/etc/init.d/networking restart") >= 0;
}

/*****************************************************************************
*                        更新系统DNS配置
*  函 数 名：UpdateDNSToSystem
*  功    能：更新系统DNS配置
*  说    明：
*  参    数：
*  返 回 值：
*  创 建 人：王磊
*  创建时间：2014-01-08
*  修 改 人：
*  修改时间：
*****************************************************************************/
bool NetWork::SetDNS()
{
    //TO DO
    int fd = -1;
    //如果有必要需先修改文件的访问的所有者及权限
    fd = open(_dnsSettingFilePath.c_str(), O_WRONLY | O_TRUNC);
    if(fd < 0)
    {
        return false;
    }

//    string dnsSettings("search chotim.com\n\n");
    string dnsSettings;
    dnsSettings = dnsSettings.append("\n\n").append("nameserver\t").append(_dns);
    dnsSettings = dnsSettings.append("\n\n").append("nameserver\t").append(_vDns);
    dnsSettings = dnsSettings.append("\n");

    if(dnsSettings.length() != write(fd, dnsSettings.c_str(), dnsSettings.length()))
    {
        return false;
    }

    return true;
}

/*****************************************************************************
*                        更新系统网络配置
*  函 数 名：UpdateToSystem
*  功    能：更新系统网络配置，由用户指定IP分配方式
*  说    明：
*  参    数：
*  返 回 值：
*  创 建 人：王磊
*  创建时间：2014-01-08
*  修 改 人：
*  修改时间：
*****************************************************************************/
bool NetWork::UpdateToSystem(IPGetMode ipMode)
{
    if(staticSet == ipMode)
    {
        return SetStaticMode();
    }
    else
    {
        return SetDHCPMode();
    }
}
