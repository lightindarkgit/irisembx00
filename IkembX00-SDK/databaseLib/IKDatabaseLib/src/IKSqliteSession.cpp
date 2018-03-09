/*******************************************************************
** 文件名称：IKSqliteSession.cpp
** 主 要 类：SqliteSession
** 描    述：sqlite连接类
**
** 版权所有：北京中科虹霸科技有限公司 (IrisKing Inc.)
** 创 建 人：yqhe
** 创建日期：2014/3/4 9:56:38
**
** 版    本：0.0.1
** 备    注：命名及代码编写遵守C++编码规范
*******************************************************************/
#include <iostream>
#include "sqlite3/soci-sqlite3.h"
#include "IKBackendSession.h"
#include "../../../Common/Exectime.h"

soci::session SqliteSession::g_sqliteSession;   // sqlite连接单一实例
bool          SqliteSession::_bValid = false;	// 当前连接状态

/*******************************************************************
*           sqlite构造函数
* 版权所有：北京中科虹霸科技有限公司 (IrisKing Inc.)
* 函数名称：SqliteSession
* 功    能：sqlite构造函数
* 说    明：按照连接参数打开数据库
* 参    数：
* 返 回 值：无
* 创 建 人：yqhe
* 创建日期：2014/2/20 10:27:20
*******************************************************************/
SqliteSession::SqliteSession(EnumDBConfigType configType, const std::string &connString)
{
	Exectime etm(__FUNCTION__);
    if (connString.empty())
    {
        OpenDefaultDB();
    }
    else if (instanceConfig==configType)
    {
        OpenInstantDB(connString);
    }
}

SqliteSession::SqliteSession()
{
	Exectime etm(__FUNCTION__);

}

/*******************************************************************
*           打开缺省数据库
* 版权所有：北京中科虹霸科技有限公司 (IrisKing Inc.)
* 函数名称：OpenDefaultDB
* 功    能：打开缺省数据库
* 说    明：缺省的库为当前目录下的数据库
* 参    数：
* 返 回 值：无
* 创 建 人：yqhe
* 创建日期：2014/2/20 10:16:56
*******************************************************************/
bool SqliteSession::OpenInstantDB(const std::string &connString)
{
	Exectime etm(__FUNCTION__);
    std::string connParam;  // parameters

    // 根据实际情况，设置自己使用的连接参数
    connParam = connString;

    try
    {
        if (Valid())
        {
            return true;
        }

        // Make the session and connect to database
        std::cout << "Connecting(sqlite)...";
        soci::connection_parameters params("sqlite3", connParam);

        g_sqliteSession.open(params);

        CheckValid();

        if (Valid())
        {
        	std::cout << " ok!" << std::endl;
        }
        else
        {
        	std::cout << " error!" << std::endl;
        }



        return true;

    }
    catch (std::exception const &e)
    {
        _errString = e.what();

        std::cerr << "Sqlite Error: " << _errString << std::endl;
    }

    return false;
}

/*******************************************************************
*           与缺省数据库sqliteIrisDB连接
* 版权所有：北京中科虹霸科技有限公司 (IrisKing Inc.)
* 函数名称：OpenDefaultDB
* 功    能：与缺省数据库sqliteIrisDB连接
* 说    明：
* 参    数：
* 返 回 值：无
* 创 建 人：yqhe
* 创建日期：2014/3/3 17:23:25
*******************************************************************/
bool SqliteSession::OpenDefaultDB()
{
	Exectime etm(__FUNCTION__);
    // parameters
    std::string connParam = "sqliteIrisDB";

    return OpenInstantDB(connParam);
}

bool SqliteSession::Valid()
{
	Exectime etm(__FUNCTION__);
	return _bValid;
}
/*******************************************************************
*
* 版权所有：北京中科虹霸科技有限公司 (IrisKing Inc.)
* 函数名称：
* 功    能：
* 说    明：
* 参    数：
* 返 回 值：无
* 创 建 人：yqhe
* 创建日期：2014/2/20 10:24:14
*******************************************************************/
bool SqliteSession::CheckValid()
{
	Exectime etm(__FUNCTION__);
    try
    {
        std::string backendName = g_sqliteSession.get_backend_name();

        if (!backendName.empty())
        {
        	_bValid = true;
            return true;
        }
        else
        {
        	_bValid = false;
            return false;
        }
    }
    catch (std::exception const &e)
    {
        _errString = e.what();
        std::cerr << "Sqlite Error: " << _errString << std::endl;
    }

    return false;
}

/*******************************************************************
*           获得session指针
* 版权所有：北京中科虹霸科技有限公司 (IrisKing Inc.)
* 函数名称：GetSession
* 功    能：获得session指针
* 说    明：
* 参    数：
* 返 回 值：nullptr —— 无效的session；其他 —— 可用的
* 创 建 人：yqhe
* 创建日期：2014/2/20 10:24:32
*******************************************************************/
soci::session* SqliteSession::GetSession()
{
	Exectime etm(__FUNCTION__);
    if (Valid())
    {
        return &g_sqliteSession;
    }

    return nullptr;
}

void SqliteSession::ReleaseSession(soci::session* pSqliteSession)
{
	Exectime etm(__FUNCTION__);
	Close();
	return;
}

void SqliteSession::Close()
{
	Exectime etm(__FUNCTION__);
	std::cout << "sqlite close" << std::endl;
	g_sqliteSession.close();
	_bValid = false;

}
