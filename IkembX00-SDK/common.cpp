/*****************************************************************************
** 文 件 名：common.cpp
** 主 要 类：DetailPerson
**
** Copyright (c) 中科虹霸有限公司
**
** 创 建 人：王磊
** 创建时间：20013-12-16
**
** 修 改 人：
** 修改时间：
** 描  述:   公共类实现
** 主要模块说明:
**
** 版  本:   1.0.0
** 备  注:
**
*****************************************************************************/
#include <string.h>
#include <list>
#include <stdio.h>
#include <stdlib.h>
#include <ifaddrs.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <regex.h>


#include "common.h"
#include "ikconfig.h"
#include "Logger.h"


std::list<std::string> GetDNSFrom(const char* file);
//正则表达式匹配
bool RegexMatch(const char* matchStr, const char* matchedStr);
//校验IP是否格式正确
bool IsValidIP(const char* ip);


const char* GetSerialPortName()
{
    //const char* serialName = "ttys4";    //要打开的串口端口名称 Vantron
    std::string devcom = IkConfig::GetDevComCfg();
    return devcom.c_str();

}


/******************************************************************f***********
*                        获取IP地址
*  函 数 名：GetIP
*  功    能：
*  说    明：
*  参    数： *  返 回 值：IP地址字符串
*  创 建 人：L.Wang
*  创建时间：2013-10-18
*  修 改 人：
*  修改时间：
*****************************************************************************/
std::string NetInfo::GetIP()
{
    //TO DO
    struct ifaddrs *addrs = nullptr;
    void *tmpAddr = nullptr;
    std::string ipAddr;

   // getifaddrs(&addrs);              //获取网络信息

    if(getifaddrs(&addrs) == -1)
    {
        LOG_ERROR("get ip fail");
        return ipAddr;
    }


    //遍历网络配置信息，找到有效网卡后返回其IP地址
    while(nullptr != addrs)
    {
        //TO DO
        if(RegexMatch("eth[0-9]{1,}", addrs->ifa_name))
        {
//            interfaceNames.insert(addrs->ifa_name);
            //获取系统当前的IP地址
            tmpAddr = &((struct sockaddr_in*)addrs->ifa_addr)->sin_addr;
            if(AF_INET == addrs->ifa_addr->sa_family)
            {
                //TO DO
                char addressBuffer[INET_ADDRSTRLEN];
                inet_ntop(AF_INET, tmpAddr, addressBuffer, INET_ADDRSTRLEN);
                if(0 != strlen(addressBuffer))
                {
                    ipAddr = std::string(addressBuffer);
                    break;
                }
            }
//            else if(AF_INET6 == addrs->ifa_addr->sa_family)
//            {
                //TO DO
                //暂不处理IPV6的情况
//                char addressBuffer[INET6_ADDRSTRLEN];
//                inet_ntop(AF_INET6, tmpAddr, addressBuffer, INET6_ADDRSTRLEN);
//                if(0 != strlen(addressBuffer))
//                {
//                    ui->txtIPAddr->setText(QString::fromUtf8(addressBuffer));
//                    ipAddr = std::string(addressBuffer);
//                    break;
//                }
//            }
        }
        addrs = addrs->ifa_next;
    }

    return ipAddr;
}

/*****************************************************************************
*                        获取子网掩码字符串
*  函 数 名：GetNetMask
*  功    能：
*  说    明：
*  参    数：
*  返 回 值：子网掩码地址字符串
*  创 建 人：L.Wang
*  创建时间：2013-10-18
*  修 改 人：
*  修改时间：
*****************************************************************************/
std::string NetInfo::GetNetMask()
{
    //TO DO
    struct ifaddrs *addrs = nullptr;
    void *tmpAddr = nullptr;
    std::string netMask;


    getifaddrs(&addrs);
    while(nullptr != addrs)
    {
        //TO DO
        if(RegexMatch("eth[0-9]{1,}", addrs->ifa_name))
        {
            //获取系统当前的子网掩码
            tmpAddr = &((struct sockaddr_in*)addrs->ifa_netmask)->sin_addr;
            if(AF_INET == addrs->ifa_addr->sa_family)   //同样的IP地址类型，执行同样的判断时，ifa_netmask会导致程序收到终止信号
            {
                //TO DO
                char addressBuffer[INET_ADDRSTRLEN];
                inet_ntop(AF_INET, tmpAddr, addressBuffer, INET_ADDRSTRLEN);
                if(0 != strlen(addressBuffer))
                {
                    netMask = std::string(addressBuffer);
                    break;
                }
            }

        }
        addrs = addrs->ifa_next;
    }

    return netMask;
}

/*****************************************************************************
*                        获取网关
*  函 数 名：getGateWay
*  功    能：
*  说    明：
*  参    数：
*  返 回 值：网关地址字符串
*  创 建 人：L.Wang
*  创建时间：2013-10-18
*  修 改 人：
*  修改时间：
*****************************************************************************/
std::string NetInfo::GetGateWay()
{
    //TO DO
    FILE *fp;
    char buf[512];
    char cmd[128];
    char gateway[30];
    char *tmp;

    strcpy(cmd, "ip route");
    fp = popen(cmd, "r");
    if(nullptr == fp)
    {
        perror("popen error");
        return "";
    }

    //清空gateway缓存
    memset(buf, 0, sizeof(buf) / sizeof(buf[0]));
    memset(gateway, 0, sizeof(gateway) / sizeof(gateway[0]));
    bool findGateWay = false;
    while(fgets(buf, sizeof(buf), fp) != nullptr)
    {
        tmp =buf;
        while(*tmp && isspace(*tmp))
            ++ tmp;
        if(strncmp(tmp, "default", strlen("default")) == 0)
        {
            findGateWay = true;
            break;
        }
    }
    pclose(fp);

    if(findGateWay)
    {
        sscanf(buf, "%*s%*s%s", gateway);
        if(IsValidIP(gateway))
        {
            return std::string(gateway);
        }
    }


    return std::string("");
}

/*****************************************************************************
*                        获取DNS
*  函 数 名：getDNS
*  功    能：
*  说    明：
*  参    数：
*  返 回 值：网关列表
*  创 建 人：L.Wang
*  创建时间：2013-10-18
*  修 改 人：
*  修改时间：
*****************************************************************************/
std::list<std::string> NetInfo::GetDNS()
{
    std::list<std::string> dnses = GetDNSFrom("/etc/network/interfaces");
    if(0 == dnses.size())
    {
        dnses = GetDNSFrom("/etc/resolv.conf");
    }

    return dnses;
}

/*****************************************************************************
*                        从指定文件中获取DNS
*  函 数 名：getDNSFrom
*  功    能：
*  说    明：
*  参    数：file， 欲获取的DNS所在的文件名称
*  返 回 值：网关列表
*  创 建 人：L.Wang
*  创建时间：2013-10-18
*  修 改 人：
*  修改时间：
*****************************************************************************/
std::list<std::string> GetDNSFrom(const char* file)
{
    //TO DO
    FILE *fp;
    char cmd[128] = "cat ";
    char dns[100];
    std::list<std::string> dnses;
    char* tmp;

    dnses.clear();           //清空dns列表
    strcat(cmd, file);
    fp = popen(cmd, "r");
    if(nullptr == fp)
    {
        perror("popen error");
        return dnses;
    }

    bool findDNS = false;
    if(0 == strcmp(file, "/etc/network/interfaces"))
    {
        char buf[512];
        memset(buf, 0, sizeof(buf) / sizeof(buf[0]));
        while(fgets(buf, sizeof(buf), fp) != nullptr)
        {
            tmp = buf;
            while(*tmp && isspace(*tmp))
                ++tmp;
            if(strncmp(tmp, "dns-nameservers", strlen("dns-nameservers")) == 0)
            {
                findDNS = true;
                break;
            }
        }

        if(findDNS)
        {
            sscanf(tmp, "%*s%s", dns);
            if(IsValidIP(dns))
            {
                dnses.push_back(dns);
            }
            sscanf(tmp, "%*s%*s%s", dns);
            if(IsValidIP(dns))
            {
                dnses.push_back(dns);
            }
        }
    }
    else
    {
        size_t readLen;
        char* readBuf = nullptr;
        while(-1 != getline(&readBuf, &readLen, fp))
        {
            tmp = readBuf;
            while(*tmp && isspace(*tmp))
                ++tmp;

            if(strncmp(tmp, "nameserver", strlen("nameserver")) == 0)
            {
                sscanf(tmp, "%*s%s", dns);
                if(IsValidIP(dns))
                {
                    dnses.push_back(dns);
                }
            }
        }

        if(readBuf)
        {
            free(readBuf);
        }
    }

    return dnses;
}

std::set<std::string> NetInfo::GetEths()
{
    //TO DO
    struct ifaddrs *addrs = nullptr;
    getifaddrs(&addrs);

    std::set<std::string> eths;
    while(nullptr != addrs)
    {
        //TO DO
        if(RegexMatch("eth[0-9]{1,}", addrs->ifa_name))
        {
            eths.insert(addrs->ifa_name);
        }
        addrs = addrs->ifa_next;
    }

    return eths;
}


bool RegexMatch(const char *regExpr, const char *str)
{
    //TO DO
    const int ERR_BUF_LENT = 128;
    const int N_MATCHES = 10;
    regex_t reg;
    char errMsgBuf[ERR_BUF_LENT];
    regmatch_t pMatch[N_MATCHES];
    int errNo;

    //编译正则表达式
    errNo = regcomp(&reg, regExpr, REG_EXTENDED | REG_ICASE);
    if(!errNo)
    {
        errNo = regexec(&reg, str, sizeof(pMatch) / sizeof(regmatch_t), pMatch,0);
    }

    //释放编译过的正则表达式所使用的资源
    regfree(&reg);
    if(!errNo)
    {
        return true;
    }
    else if(REG_NOMATCH != errNo)
    {
        regerror(errNo, &reg, errMsgBuf, sizeof(errMsgBuf));
        std::cout << __FUNCTION__ << ":" << errMsgBuf << std::endl;
        return false;
    }
}


bool IsValidIP(const char* ip)
{
    return RegexMatch("^([0-9]{1,3}.){3,3}[0-9]{1,3}$", ip);
}

