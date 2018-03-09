/*******************************************************************
** 文件名称：IKDatabaseType.h
** 主 要 类：
** 描    述：IKDatabaseLib中用到的数据定义
**
** 版权所有：北京中科虹霸科技有限公司 (IrisKing Inc.)
** 创 建 人：yqhe
** 创建日期：2014/2/20 11:03:08
**
** 版    本：0.0.1
** 备    注：命名及代码编写遵守C++编码规范
*******************************************************************/
#pragma once


#ifdef _MSC_VER
    // windows系统下编译的预定义
    #ifdef IKDATABASELIB_EXPORTS
	#define IKDBLIB_API __declspec(dllexport)
    #else
	#define IKDBLIB_API __declspec(dllimport)
    #	pragma comment(lib, "IKDatabaseLib.lib")
    #endif

#else
    // linux系统下编译的预定义
    #define IKDBLIB_API
    // #define nullptr NULL
#endif

// 返回值
enum EnumReturnValue
{
    dbSuccess     = 0,        // 成功
    dbErrorOpen   = -1,       // 数据库打开失败
    dbErrorQuery  = -2,       // 数据库查询失败
    dbErrorInsert = -3,       // 数据库插入失败
    dbErrorUpdate = -4       // 数据库更新失败
};

// 数据库连接方式
enum EnumDBConnType
{
    connSingle,               // 单连接方式
    connPool                  // 连接池方式
};

// 数据库配置方式
enum EnumDBConfigType
{
    xmlConfig,              // xml文件配置方式
    instanceConfig          // 即时连接参数方式
};
