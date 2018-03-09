/*******************************************************************
** 文件名称：IKDatabaseLib.cpp
** 主 要 类：对数据库访问的接口类
** 描    述：
**
** 版权所有：北京中科虹霸科技有限公司 (IrisKing Inc.)
** 创 建 人：yqhe
** 创建日期：2014/2/18 14:45:18
**
** 版    本：0.0.1
** 备    注：命名及代码编写遵守C++编码规范
*******************************************************************/
#include <iostream>
#include <sstream>

#include "IKDatabaseLib.h"
#include "IKBackendSession.h"

#include <postgresql/soci-postgresql.h>
#include "../../../Common/Exectime.h"

/*******************************************************************
*           构造函数，指定连接参数
* 版权所有：北京中科虹霸科技有限公司 (IrisKing Inc.)
* 函数名称：DatabaseAccess
* 功    能：构造函数，指定连接参数
* 说    明：
* 参    数：
* 返 回 值：无
* 创 建 人：yqhe
* 创建日期：2014/2/19 8:27:31
*******************************************************************/
DatabaseAccess::DatabaseAccess(const std::string &connString,
                EnumDBConnType connType)
: _sql(nullptr)
, _backendName("")
{
	Exectime etm(__FUNCTION__);
    ConnectDatabase(connString, connType);
}

/*******************************************************************
*           构造函数，连接缺省数据库
* 版权所有：北京中科虹霸科技有限公司 (IrisKing Inc.)
* 函数名称：DatabaseAccess
* 功    能：构造函数，连接缺省数据库
* 说    明：
* 参    数：
* 返 回 值：无
* 创 建 人：yqhe
* 创建日期：2014/2/20 11:24:29
*******************************************************************/
DatabaseAccess::DatabaseAccess()
: _sql(nullptr)
, _backendName("")

{
	Exectime etm(__FUNCTION__);
    try
    {
        ConnectDatabase();
    }
    catch (std::exception const &e)
    {
        _errString = e.what();
        std::cerr << "Error: " << _errString << std::endl;
    }
}

// 析构函数
DatabaseAccess::~DatabaseAccess()
{
	Exectime etm(__FUNCTION__);
	ReleaseSession();
}

/*******************************************************************
*           连接数据库
* 版权所有：北京中科虹霸科技有限公司 (IrisKing Inc.)
* 函数名称：ConnectDatabase
* 功    能：连接数据库
* 说    明：
* 参    数：
* 返 回 值：无
* 创 建 人：yqhe
* 创建日期：2014/2/20 11:26:04
*******************************************************************/
bool DatabaseAccess::ConnectDatabase(const std::string &connString,
                EnumDBConnType connType)
{
	Exectime etm(__FUNCTION__);
    BackendFactory factory;

    soci::session* sqlSession = factory.CreateBackend(connString, connType);

    if (sqlSession)
    {
        _sql = sqlSession;
        _backendName = _sql->get_backend_name();

        return true;
    }

    return false;
}

/*******************************************************************
*           测试连接是否有效
* 版权所有：北京中科虹霸科技有限公司 (IrisKing Inc.)
* 函数名称：Valid
* 功    能：测试连接是否有效
* 说    明：
* 参    数：
* 返 回 值：无
* 创 建 人：yqhe
* 创建日期：2014/2/20 11:29:48
*******************************************************************/
bool DatabaseAccess::Valid()
{
	Exectime etm(__FUNCTION__);
    try
    {
        if (!_sql)
        {
            return false;
        }

        std::string backendName = _sql->get_backend_name();


        if (!backendName.empty())
        {
            return true;
        }
        else
        {
            return false;
        }
    }
    catch (std::exception const &e)
    {
        _errString = e.what();
        std::cerr << "Error: " << _errString << std::endl;
        return false;
    }
    catch (...)
    {
        std::cerr << "Error: " << __FUNCTION__ << " Unknown" << std::endl; 
        return false;
    }

    return false;

}

/*******************************************************************
*           获得实际数据库的连接
* 版权所有：北京中科虹霸科技有限公司 (IrisKing Inc.)
* 函数名称：Valid
* 功    能：获得实际数据库的连接
* 说    明：因为各个库类型连接不同，所以返回details::session_backend *
*           使用者通过返回值再找到连接
*           使用示例——
*           DataAccess access;
*           PGconn* pConn = access.GetPsqlBackend();
* 参    数：
* 返 回 值：nullptr —— 无效的连接；其他 —— 有效的连接
* 创 建 人：yqhe
* 创建日期：2014/3/7 16:16
*******************************************************************/
PGconn* DatabaseAccess::GetPsqlBackend()
{
	Exectime etm(__FUNCTION__);
    if (!Valid())
    {
        return nullptr;
    }

    soci::details::session_backend *backend = _sql->get_backend();
    soci::postgresql_session_backend *psqlBackend = static_cast<soci::postgresql_session_backend *>(backend);
    if (nullptr!=psqlBackend)
    {
        return psqlBackend->conn_;
    }

    return nullptr;
}

/*******************************************************************
*         关闭与数据库的连接
* 版权所有：北京中科虹霸科技有限公司 (IrisKing Inc.)
* 函数名称：Close
* 功    能：关闭与数据库的连接
* 说    明：
* 参    数：
* 返 回 值：无
* 创 建 人：yqhe
* 创建日期：2014/3/17 15:56
*******************************************************************/
void DatabaseAccess::Close()
{
	Exectime etm(__FUNCTION__);
	ReleaseSession();
}


/*******************************************************************
*         释放与数据库的连接
* 版权所有：北京中科虹霸科技有限公司 (IrisKing Inc.)
* 函数名称：ReleaseSession
* 功    能：释放与数据库的连接
* 说    明：
* 参    数：无
* 返 回 值：无
* 创 建 人：yqhe
* 创建日期：2014/7/11 16:33
*******************************************************************/
void DatabaseAccess::ReleaseSession()
{
	Exectime etm(__FUNCTION__);
	try
	{
		BackendFactory factory;

		factory.ReleaseSession(_backendName, _sql);

		_backendName = "";
		_sql = nullptr;
	}
    catch (std::exception const &e)
    {
        _errString = e.what();
        std::cerr << "Error: " << _errString << std::endl;
    }

}

/*******************************************************************
*         获得与数据库的连接
* 版权所有：北京中科虹霸科技有限公司 (IrisKing Inc.)
* 函数名称：GetSession
* 功    能：获得与数据库的连接
* 说    明：
* 参    数：无
* 返 回 值：nullptr —— 无效的连接；其他 —— 有效的连接
* 创 建 人：yqhe
* 创建日期：2014/5/5 14:53
*******************************************************************/
soci::session*  DatabaseAccess::GetSession()
{
	Exectime etm(__FUNCTION__);
	return _sql;
}

