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

#include <sstream>
#include <iostream>

#define  __IK_LOGGER_API_DLL_  __declspec(dllexport)

#ifdef  __IK_LOGGER_API_DLL_

//__IKUSBCAMERA_DLL_ should be defined in all of the DLL's source
//code modules before this header file is included

//All functions/variables are being exported.

#else

//This header file is included by an EXE source code module.
//Indicated that all functions/variables are being imported.
#define  __IK_LOGGER_API_DLL_  __declspec(dllimport)
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
    sprintf(sztmp,"[%s:%d]	"fmt,__FUNCTION__,__LINE__,##__VA_ARGS__);	\
    Logger::LOGD(sztmp);	\
}


#define		LOG_INFO(fmt,...)	\
{	\
	char	sztmp[LOG_MAX_SIZE] = {0};	\
    sprintf(sztmp,"[%s:%d]	"fmt,__FUNCTION__,__LINE__,##__VA_ARGS__);	\
    Logger::LOGI(sztmp);	\
}


#define		LOG_WARN(fmt,...)	\
{	\
	char	sztmp[LOG_MAX_SIZE] = {0};	\
    sprintf(sztmp,"[%s:%d]	"fmt,__FUNCTION__,__LINE__,##__VA_ARGS__);	\
    Logger::LOGW(sztmp);	\
}


#define		LOG_ERROR(fmt,...)	\
{	\
	char	sztmp[LOG_MAX_SIZE] = {0};	\
    sprintf(sztmp,"[%s:%d]	"fmt,__FUNCTION__,__LINE__,##__VA_ARGS__);	\
    Logger::LOGE(sztmp);	\
}


