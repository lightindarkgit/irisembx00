/*******************************************************************
** Logger.cpp
**   Logger
**     Logger
**
** §µ§á (IrisKing Inc.)
**   yqhe
** 2016/06/01 15:35
**
**     0.0.1
**     §ÕC++œZ
*******************************************************************/

#pragma once

class Logger_Impl;

#include <stdio.h>
#include <sstream>
#include <iostream>

#ifndef CONFIGFILE	
#define	CONFIGFILE	"./log4cpp.conf"
#endif

class Logger
{
public:
    Logger();
    ~Logger();

public:
    static void LOGD(const char* msg);     // debug, 700
    static void LOGI(const char* msg);     // info,  600
    static void LOGW(const char* msg);     // warn,  400
    static void LOGE(const char* msg);     // error, 300
};

#define		LOG_MAX_SIZE	4096

#define		LOG_DEBUG(fmt,...)	\
{	\
	char	sztmp[LOG_MAX_SIZE] = {0};	\
    sprintf(sztmp,"[%s:%s:%d]	"#fmt,__FILE__,__FUNCTION__,__LINE__,##__VA_ARGS__);	\
    Logger::LOGD(sztmp);	\
}


#define		LOG_INFO(fmt,...)	\
{	\
	char	sztmp[LOG_MAX_SIZE] = {0};	\
    sprintf(sztmp,"[%s:%s:%d]	"#fmt,__FILE__,__FUNCTION__,__LINE__,##__VA_ARGS__);	\
    Logger::LOGI(sztmp);	\
}


#define		LOG_WARN(fmt,...)	\
{	\
	char	sztmp[LOG_MAX_SIZE] = {0};	\
    sprintf(sztmp,"[%s:%s:%d]	"#fmt,__FILE__,__FUNCTION__,__LINE__,##__VA_ARGS__);	\
    Logger::LOGW(sztmp);	\
}


#define		LOG_ERROR(fmt,...)	\
{	\
	char	sztmp[LOG_MAX_SIZE] = {0};	\
    sprintf(sztmp,"[%s:%s:%d]	"#fmt,__FILE__,__FUNCTION__,__LINE__,##__VA_ARGS__);	\
    Logger::LOGE(sztmp);	\
}


void cpuFreqSet(const char *gov);
