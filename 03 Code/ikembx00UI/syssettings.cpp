/*****************************************************************************
** 文 件 名：syssettings.cpp
** 主 要 类：SysSettings
**
** Copyright (c) 中科虹霸有限公司
**
** 创 建 人：王磊
** 创建时间：20013-10-18
**
** 修 改 人：
** 修改时间：
** 描  述:   系统设置
** 主要模块说明: IP配置管理， 系统时间配置
**
** 版  本:   1.0.0
** 备  注:
**
*****************************************************************************/
//QT头文件
#include <QStringList>
#include <QDebug>
#include <QProcess>
#include <QMessageBox>
#include <QRegExp>

//标准库头文件
#include <string.h>
#include <time.h>
#include <ifaddrs.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <errno.h>

//应用头文件
#include "syssettings.h"
#include "ui_syssettings.h"
#include "network.h"

//#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>


/*****************************************************************************
*                        检查IP的合法性
*  函 数 名：isValid
*  功    能：对输入参数，确定指定的IP地址是否合法
*  说    明：
*  参    数：
*  返 回 值：
*  创 建 人：王磊
*  创建时间：2014-01-08
*  修 改 人：
*  修改时间：
*****************************************************************************/
bool isIPValid(const QString& addr)
{
    //TO DO
//    const QRegExp ipPatten("^\\d{1,3}.\\d{1,3}.\\d{1,3}.\\d{1,3}$");
    const QRegExp ipPatten("^(((0?0?\\d)|(0?[1-9]\\d)|(1\\d{2})|(2[0-4]\\d)|(25[0-5]))(\\.((0?0?\\d)|(0?[1-9]\\d)|(1\\d{2})|(2[0-4]\\d)|(25[0-5]))){3})$");

    return ipPatten.exactMatch(addr.trimmed());
}



SysSettings::SysSettings(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::SysSettings),
    isSettingsChanged(false)
{
    ui->setupUi(this);
    ui->txtIPAddr->setFocus();


    init();
    initNetwork();

    connect(ui->btnBack, SIGNAL(clicked()), this, SIGNAL(sigGoBack()));
    connect(ui->btnSave, SIGNAL(clicked()), this, SLOT(slotSave()));

    connect(&timer, SIGNAL(timeout()), this, SLOT(slotTimerOut()));

    connect(ui->btnAdd, SIGNAL(clicked()), this, SLOT(slotAdd()));
    connect(ui->btnMinus, SIGNAL(clicked()), this, SLOT(slotMinus()));
    connect(ui->btnApply, SIGNAL(clicked()), this, SLOT(slotApplyTimeSettings()));
    connect(ui->txtIPAddr, SIGNAL(editingFinished()), this, SLOT(slotIPChanged()));
    connect(ui->txtMaskCode, SIGNAL(editingFinished()), this, SLOT(slotNetMaskChanged()));
    connect(ui->txtNetGate, SIGNAL(editingFinished()), this, SLOT(slotGateChanged()));
    connect(ui->txtDNS, SIGNAL(editingFinished()), this, SLOT(slotDNSChanged()));
    connect(ui->txtViceDNS, SIGNAL(editingFinished()), this, SLOT(slotViceDNSChanged()));

//    connect(ui->dateTimeEditor,SIGNAL(editingFinished()), this, SLOT(slotSaveTime()));
}

SysSettings::~SysSettings()
{
    delete ui;
}

/*****************************************************************************
*                        系统设置初始化
*  函 数 名：init
*  功    能：初始化系统设置，设置系统时间更新定时器及启动
*  说    明：
*  参    数：
*  返 回 值：
*  创 建 人：王磊
*  创建时间：2013-10-18
*  修 改 人：
*  修改时间：
*****************************************************************************/
void SysSettings::init()
{
    //初始化网络设置


    //初始化系统时间设置
    timer.setSingleShot(false);
    timer.start(1000);

    ui->dateTimeEditor->setDateTime(QDateTime::currentDateTime());
}

QString getGateWay()
{
    FILE *fp;
    char buf[512];
    char cmd[128];
    char gateway[30];
    char *tmp;

    strcpy(cmd, "ip route");
    fp = popen(cmd, "r");
    if(NULL == fp)
    {
        perror("popen error");
        return "";
    }

    //清空gateway缓存
    memset(buf, 0, sizeof(buf) / sizeof(buf[0]));
    memset(gateway, 0, sizeof(gateway) / sizeof(gateway[0]));
    bool findGateWay = false;
    while(fgets(buf, sizeof(buf), fp) != NULL)
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
//        printf("default gateway:%s/n", gateway);
    }


    return QString::fromUtf8(gateway);
}

QStringList getDNSFrom(char* file)
{
    //TO DO
    FILE *fp;
    char cmd[128] = "cat ";
    char dns[100];
    QStringList dnses;
    char* tmp;

    dnses.clear();           //清空dns列表
    strcat(cmd, file);
    fp = popen(cmd, "r");
    if(NULL == fp)
    {
        perror("popen error");
        return dnses;
    }

    bool findDNS = false;
    if(0 == strcmp(file, "/etc/network/interfaces"))
    {
        char buf[512];
        memset(buf, 0, sizeof(buf) / sizeof(buf[0]));
        while(fgets(buf, sizeof(buf), fp) != NULL)
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
            dnses << QString::fromUtf8(dns);
            sscanf(tmp, "%*s%*s%s", dns);
            dnses << QString::fromUtf8(dns);
        }
    }
    else
    {
        size_t readLen;
        char* readBuf = NULL;
        while(-1 != getline(&readBuf, &readLen, fp))
        {
            tmp = readBuf;
            while(*tmp && isspace(*tmp))
                ++tmp;

            if(strncmp(tmp, "nameserver", strlen("nameserver")) == 0)
            {
                sscanf(tmp, "%*s%s", dns);
                dnses << QString::fromUtf8(dns);
            }
        }

        if(readBuf)
        {
            free(readBuf);
        }
    }

    return dnses;
}

QStringList getDNS()
{
    //TO DO
    QStringList dnses = getDNSFrom("/etc/network/interfaces");
    if(0 == dnses.size())
    {
        dnses = getDNSFrom("/etc/resolv.conf");
    }

    return dnses;
}

bool SysSettings::initNetwork()
{
    //TO DO
    struct ifaddrs *addrs = NULL;
    void *tmpAddr = NULL;
    getifaddrs(&addrs);
    QRegExp ipPatten("eth\\d+");

    while(NULL != addrs)
    {
        //TO DO
        if(ipPatten.exactMatch(addrs->ifa_name))
        {
            interfaceNames.insert(addrs->ifa_name);
            //获取系统当前的IP地址
            tmpAddr = &((struct sockaddr_in*)addrs->ifa_addr)->sin_addr;
            if(AF_INET == addrs->ifa_addr->sa_family)
            {
                //TO DO
                char addressBuffer[INET_ADDRSTRLEN];
                inet_ntop(AF_INET, tmpAddr, addressBuffer, INET_ADDRSTRLEN);
                if(0 != strlen(addressBuffer))
                {
                    ui->txtIPAddr->setText(QString::fromUtf8(addressBuffer));
                }
            }
            else if(AF_INET6 == addrs->ifa_addr->sa_family)
            {
                //TO DO
                //暂不处理IPV6的情况
//                char addressBuffer[INET6_ADDRSTRLEN];
//                inet_ntop(AF_INET6, tmpAddr, addressBuffer, INET6_ADDRSTRLEN);
//                if(0 != strlen(addressBuffer))
//                {
//                    ui->txtIPAddr->setText(QString::fromUtf8(addressBuffer));
//                }
            }


            //获取系统当前的子网掩码
            tmpAddr = &((struct sockaddr_in*)addrs->ifa_netmask)->sin_addr;
            if(AF_INET == addrs->ifa_addr->sa_family)   //同样的IP地址类型，执行同样的判断时，ifa_netmask会导致程序收到终止信号
            {
                //TO DO
                char addressBuffer[INET_ADDRSTRLEN];
                inet_ntop(AF_INET, tmpAddr, addressBuffer, INET_ADDRSTRLEN);
                if(0 != strlen(addressBuffer))
                {
                    ui->txtMaskCode->setText(QString::fromUtf8(addressBuffer));
                }
            }
            else if(AF_INET6 == addrs->ifa_addr->sa_family)
            {
                //TO DO
//                char addressBuffer[INET6_ADDRSTRLEN];
//                inet_ntop(AF_INET6, tmpAddr, addressBuffer, INET6_ADDRSTRLEN);
//                if(0 != strlen(addressBuffer))
//                {
//                    ui->txtMaskCode->setText(QString::fromUtf8(addressBuffer));
//                }
            }
        }
        addrs = addrs->ifa_next;
    }

    ui->txtNetGate->setText(getGateWay());

    QStringList dns = getDNS();
    if(dns.size() > 0)
    {
        ui->txtDNS->setText(dns.at(0));
    }
    if(dns.size() > 1)
    {
        ui->txtViceDNS->setText(dns.at(1));
    }

    return true;
}

bool SysSettings::checkIPValid(QString& err, const QString &ip, const QString ipInfo)
{
    //TO DO
    if(ip.trimmed().isEmpty())
    {
        if(ipInfo.contains("DNS"))  //允许DNS或备用DNS为空
        {
            return true;
        }

        err.append(QString::fromUtf8("%1为空\n").arg(ipInfo));
        return false;
    }
    else
    {
        if(!isIPValid(ip))
        {
            err.append(QString::fromUtf8("%1格式非法\n").arg(ipInfo));
            return false;
        }
    }

    return true;
}

bool SysSettings::checkIPs()
{
    QString errMsg;
    errMsg.clear();

//    //判断IP地址
//    if(ui->txtIPAddr->text().trimmed().isEmpty())
//    {
//        errMsg.append("IP地址为空\n");
//    }
//    else
//    {
//        if(!isIPValid(ui->txtIPAddr->text()))
//        {
//            errMsg.append("IP地址格式非法\n");
//        }
//    }

//    //判断子网掩码
//    if(ui->txtMaskCode->text().trimmed().isEmpty())
//    {
//        errMsg.append("子网掩码为空\n");
//    }
//    else
//    {
//        if(!isIPValid(ui->txtMaskCode->text()))
//        {
//            errMsg.append("子网掩码格式非法\n");
//        }
//    }

//    //判断网关
//    if(ui->txtNetGate->text().trimmed().isEmpty())
//    {
//        errMsg.append("网关为空\n");
//    }
//    else
//    {
//        if(!isIPValid(ui->txtNetGate->text()))
//        {
//            errMsg.append("网关格式非法\n");
//        }
//    }

//    //判断DNS
//    if(ui->txtDNS->text().trimmed().isEmpty())
//    {
//        errMsg.append("DNS为空\n");
//    }
//    else
//    {
//        if(!isIPValid(ui->txtDNS->text()))
//        {
//            errMsg.append("DNS格式非法\n");
//        }
//    }

//    if(ui->txtViceDNS->text().trimmed().isEmpty())
//    {
//        errMsg.append("备用DNS为空\n");
//    }
//    else
//    {
//        if(!isIPValid(ui->txtDNS->text()))
//        {
//            errMsg.append("备用DNS格式非法\n");
//        }
//    }

    if(!checkIPValid(errMsg, ui->txtIPAddr->text(), QString::fromUtf8("IP地址")) ||
       !checkIPValid(errMsg, ui->txtMaskCode->text(), QString::fromUtf8("子网掩码")) ||
       !checkIPValid(errMsg, ui->txtNetGate->text(), QString::fromUtf8("网关")) ||
       !checkIPValid(errMsg, ui->txtDNS->text(), QString::fromUtf8("DNS")) ||
       !checkIPValid(errMsg, ui->txtViceDNS->text(), QString::fromUtf8("备用DNS")))
    {
        QMessageBox::warning(this, QString::fromUtf8("网络设置"), QString::fromUtf8(errMsg));
        return false;
    }
    else
    {
        return true;
    }
}

/*****************************************************************************
*                        存储系统设置
*  函 数 名：slotSave
*  功    能：槽函数，存储系统设置,主要用来存储网络设置
*  说    明：
*  参    数：
*  返 回 值：
*  创 建 人：王磊
*  创建时间：2013-10-18
*  修 改 人：
*  修改时间：
*****************************************************************************/
void SysSettings::slotSave()
{
    //TO DO
    if(!checkIPs())
    {
        return;
    }

    NetWork nw;

    nw.SetInterfaceName(*interfaceNames.begin());                        //设置网卡名称
    nw.SetIP(ui->txtIPAddr->text().toUtf8().data());           //设置IP地址
    nw.SetNetMask(ui->txtMaskCode->text().toUtf8().data());    //设置子网掩码
    nw.SetGateWay(ui->txtNetGate->text().toUtf8().data());     //设置网关
    nw.SetDNS(ui->txtDNS->text().toUtf8().data());             //设置DNS
    nw.SetViceDNS(ui->txtViceDNS->text().toUtf8().data());     //设置备用DNS


    //将网络配置写入系统配置文件
    if(!nw.UpdateToSystem(staticSet))
    {
        QMessageBox::warning(this, QString::fromUtf8("设置网络"), QString::fromUtf8("更新网络配置失败！"));
    }

    QMessageBox::information(this, QString::fromUtf8("设置网络"), QString::fromUtf8("更新网络配置成功！"));
}

/*****************************************************************************
*                        系统时间显示定时器超时处理
*  函 数 名：slotTimerOut
*  功    能：槽函数，处理定时器超时情况
*  说    明：
*  参    数：
*  返 回 值：
*  创 建 人：王磊
*  创建时间：2013-10-18
*  修 改 人：
*  修改时间：
*****************************************************************************/
void SysSettings::slotTimerOut()
{
    //TO DO
    ui->dateTimeEditor->setDateTime(QDateTime::currentDateTime());
}

/*****************************************************************************
*                        时间增量调整
*  函 数 名：slotAdd
*  功    能：槽函数，增加指定时间分量
*  说    明：
*  参    数：
*  返 回 值：
*  创 建 人：王磊
*  创建时间：2013-10-18
*  修 改 人：
*  修改时间：
*****************************************************************************/
void SysSettings::slotAdd()
{
    //TO DO
    if(timer.isActive())
        timer.stop();

    //修改时间
    QDateTime dateTime = ui->dateTimeEditor->dateTime();

    //ui->btnAdd->setText(dateTime.toString("yyyy-MM-dd hh:mm:ss"));
    switch(ui->dateTimeEditor->currentSectionIndex())
    {
    case 0:
        dateTime = dateTime.addYears(1);
        break;
    case 1:
        dateTime = dateTime.addMonths(1);
        break;
    case 2:
        dateTime = dateTime.addDays(1);
        break;
    case 3:
        dateTime = dateTime.addSecs(3600);
        break;
    case 4:
        dateTime = dateTime.addSecs(60);
        break;
    case 5:
    default:
        dateTime = dateTime.addSecs(1);
        break;
    }
    //ui->btnMinus->setText(dateTime.toString("yyyy-MM-dd hh:mm:ss"));
    ui->dateTimeEditor->setDateTime(dateTime);
}

/*****************************************************************************
*                        时间增量
*  函 数 名：slotShutdown
*  功    能：槽函数，减少指定时间分量
*  说    明：
*  参    数：
*  返 回 值：
*  创 建 人：王磊
*  创建时间：2013-10-18
*  修 改 人：
*  修改时间：
*****************************************************************************/
void SysSettings::slotMinus()
{
    //TO DO
    if(timer.isActive())
        timer.stop();

    //修改时间
    QDateTime dateTime = ui->dateTimeEditor->dateTime();
    qDebug() << dateTime.date().year() << "-" << dateTime.date().month();
    //ui->btnAdd->setText(dateTime.toString("yyyy-MM-dd hh:mm:ss"));

    switch(ui->dateTimeEditor->currentSectionIndex())
    {
    case 0:
        dateTime = dateTime.addYears(-1);
        break;
    case 1:
        dateTime = dateTime.addMonths(-1);
        break;
    case 2:
        dateTime = dateTime.addDays(-1);
        break;
    case 3:
        dateTime = dateTime.addSecs(-3600);
        break;
    case 4:
        dateTime = dateTime.addSecs(-60);
        break;
    case 5:
    default:
        dateTime = dateTime.addSecs(-1);
        break;
    }
    qDebug() << "after modified: "<< dateTime.date().year() << "-" << dateTime.date().month();
    //ui->btnMinus->setText(dateTime.toString("yyyy-MM-dd hh:mm:ss"));
    ui->dateTimeEditor->setDateTime(dateTime);
}

/*****************************************************************************
*                        应用时间设置
*  函 数 名：slotApplyTimeSettings
*  功    能：槽函数，使当前设置的时间生效
*  说    明：
*  参    数：
*  返 回 值：
*  创 建 人：王磊
*  创建时间：2013-10-18
*  修 改 人：
*  修改时间：
*****************************************************************************/
bool SysSettings::slotApplyTimeSettings()
{
    //TO DO
//    time_t oTime;
//    oTime = time(&oTime);

//    struct tm newTime;
//    struct tm* tp;

//    tp = localtime(&oTime);


//    newTime.tm_sec = ui->dateTimeEditor->time().second();
//    newTime.tm_min = ui->dateTimeEditor->time().minute();
//    newTime.tm_hour = ui->dateTimeEditor->time().hour();

//    newTime.tm_mday = ui->dateTimeEditor->date().day();
//    newTime.tm_mon = ui->dateTimeEditor->date().month();
//    newTime.tm_year = ui->dateTimeEditor->date().year() - 1900;
//    newTime.tm_wday = ui->dateTimeEditor->date().dayOfWeek();
//    newTime.tm_yday = ui->dateTimeEditor->date().dayOfYear();
//    newTime.tm_isdst = tp->tm_isdst;
//    newTime.tm_gmtoff = tp->tm_gmtoff;
//    time_t ttNewTime;
//    ttNewTime = mktime(&newTime);

    QDateTime dt = ui->dateTimeEditor->dateTime();

    time_t ttNewTime;
    ttNewTime = (time_t)dt.toTime_t();

    //更新用户设置的时间到系统
    int r = stime(&ttNewTime);
    if(r)
    {
        QMessageBox::critical(this, QString::fromUtf8("设置系统时间"), QString::fromUtf8(strerror(errno)));
    }

    //启动时间定时器刷新时间
    timer.start(1000);

    return true;
}

/*****************************************************************************
*                        编辑IP地址槽函数
*  函 数 名：slotIPChanged
*  功    能：槽函数，当IP地址发生变化
*  说    明：
*  参    数：
*  返 回 值：
*  创 建 人：王磊
*  创建时间：2013-10-18
*  修 改 人：
*  修改时间：
*****************************************************************************/
void SysSettings::slotIPChanged()
{
    //TO DO
    if(!isIPValid(ui->txtIPAddr->text()))
    {
        ui->txtIPAddr->setStyleSheet("color:rgb(255,0,0)");
    }
    else
    {
        ui->txtIPAddr->setStyleSheet("color:rgb(0,0,0)");
    }
}

void SysSettings::slotNetMaskChanged()
{
    //TO DO
    if(!isIPValid(ui->txtMaskCode->text()))
    {
        ui->txtMaskCode->setStyleSheet("color:rgb(255,0,0)");
    }
    else
    {
        ui->txtMaskCode->setStyleSheet("color:rgb(0,0,0)");
    }
}

void SysSettings::slotGateChanged()
{
    //TO DO
    if(!isIPValid(ui->txtNetGate->text()))
    {
        ui->txtNetGate->setStyleSheet("color:rgb(255,0,0)");
    }
    else
    {
        ui->txtNetGate->setStyleSheet("color:rgb(0,0,0)");
    }
}

void SysSettings::slotDNSChanged()
{
    //TO DO
    if(!isIPValid(ui->txtDNS->text()))
    {
        ui->txtDNS->setStyleSheet("color:rgb(255,0,0)");
    }
    else
    {
        ui->txtDNS->setStyleSheet("color:rgb(0,0,0)");
    }
}

void SysSettings::slotViceDNSChanged()
{
    //TO DO
    if(!isIPValid(ui->txtViceDNS->text()))
    {
        ui->txtViceDNS->setStyleSheet("color:rgb(255,0,0)");
    }
    else
    {
        ui->txtViceDNS->setStyleSheet("color:rgb(0,0,0)");
    }
}
