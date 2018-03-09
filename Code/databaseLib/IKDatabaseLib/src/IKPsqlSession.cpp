/*******************************************************************
** 文件名称：IKPsqlSession.cpp
** 主 要 类：PsqlSession
** 描    述：postgresql连接类
**
** 版权所有：北京中科虹霸科技有限公司 (IrisKing Inc.)
** 创 建 人：yqhe
** 创建日期：2014/3/4 9:45:49
**
** 版    本：0.0.1
** 备    注：命名及代码编写遵守C++编码规范
*******************************************************************/
#include <iostream>
#include "postgresql/soci-postgresql.h"
#include "IKBackendSession.h"
#include "../../../Common/Exectime.h"

// soci::session   PsqlSession::g_pSqlSession;     // postgresql连接单一实例
// modified at 20140605 by yqhe
// 采用连接池的方式，获得连接
const size_t 			g_pSqlConnSize = 10;
soci::connection_pool*	g_pSqlPool = nullptr;

/*******************************************************************
*           缺省构造函数，打开缺省数据库
* 版权所有：北京中科虹霸科技有限公司 (IrisKing Inc.)
* 函数名称：PsqlSession
* 功    能：缺省构造函数，打开缺省数据库
* 说    明：
* 参    数：
* 返 回 值：无
* 创 建 人：yqhe
* 创建日期：2014/3/3 17:13:10
*******************************************************************/
PsqlSession::PsqlSession()
{
	Exectime etm(__FUNCTION__);
    OpenDefaultDB();
}

/*******************************************************************
*           postgresql构造函数
* 版权所有：北京中科虹霸科技有限公司 (IrisKing Inc.)
* 函数名称：PsqlSession
* 功    能：postgresql构造函数
* 说    明：按照连接参数打开数据库
* 参    数：
* 返 回 值：无
* 创 建 人：yqhe
* 创建日期：2014/2/20 10:27:20
*******************************************************************/
PsqlSession::PsqlSession(EnumDBConfigType configType, const std::string &connParam)
{
	Exectime etm(__FUNCTION__);
    if (connParam.empty())
    {
        OpenDefaultDB();
    }
    else if (instanceConfig==configType)
    {
        OpenInstantDB(connParam);
    }
}

/*******************************************************************
*         按照即时连接参数生成数据库连接池
* 版权所有：北京中科虹霸科技有限公司 (IrisKing Inc.)
* 函数名称：CreateConnPool
* 功   能：按照即时连接参数生成数据库连接池
* 说   明：
* 参   数：
* 返 回 值：true —— 正确初始化连接池
*         false —— 初始化连接池出错
* 创 建 人：yqhe
* 创建日期：2014/3/3 17:06:32
*******************************************************************/
bool PsqlSession::CreateConnPool(const std::string &connParam)
{
	Exectime etm(__FUNCTION__);
    try
    {
        // 如果已经打开，则直接返回
        if (Valid())
        {
            return true;
        }

        if (nullptr == g_pSqlPool)
        {
        	g_pSqlPool = new soci::connection_pool(g_pSqlConnSize);
        }

        // Make the session and connect to database
        std::cout << "Create connection pool (postgresql)...";
        soci::connection_parameters params("postgresql", connParam);

        for (size_t count=0; count<g_pSqlConnSize; count++)
        {
        	soci::session& sqlSession = g_pSqlPool->at(count);
        	sqlSession.open(params);
        }

        std::cout << "ok! (postgresql pool)" << std::endl;

        return true;

    }
    catch ( soci::postgresql_soci_error const &e)
    {
        _errString = e.what();

        std::cout // << "PostgreSQL error: "    << e.sqlstate()
            << " " << _errString << std::endl;

    }
    catch (std::exception const &e)
    {
        _errString = e.what();

        std::cerr << "Error: " << _errString << std::endl;
    }
    catch (...)
    {
        std::cout << "What?!" << std::endl;
    }

    return false;
}

/*******************************************************************
*           按照即时连接参数连接数据库
* 版权所有：北京中科虹霸科技有限公司 (IrisKing Inc.)
* 函数名称：OpenInstantDB
* 功    能：按照即时连接参数连接数据库
* 说    明：
* 参    数：
* 返 回 值：true —— 正确打开数据库
*           false —— 打开数据库出错
* 创 建 人：yqhe
* 创建日期：2014/3/3 17:06:32
*******************************************************************/
bool PsqlSession::OpenInstantDB(const std::string &connParam)
{
	Exectime etm(__FUNCTION__);
	return (CreateConnPool(connParam));
}

/*******************************************************************
*           打开缺省数据库
* 版权所有：北京中科虹霸科技有限公司 (IrisKing Inc.)
* 函数名称：OpenDefaultDB
* 功    能：打开缺省数据库
* 说    明：缺省的库为本地的irisApp数据库
* 参    数：
* 返 回 值：true  —— 正确打开数据库
*           false —— 打开数据库出错
* 创 建 人：yqhe
* 创建日期：2014/2/20 10:16:56
*******************************************************************/
bool PsqlSession::OpenDefaultDB()
{
	Exectime etm(__FUNCTION__);
    std::string connParam;  // parameters

    // 根据实际情况，设置自己使用的连接参数

    // 连接远程数据库的连接参数
    connParam =std::string("host=127.0.0.1 dbname=irisApp user=postgres password=123456");

    return OpenInstantDB(connParam);

}

/*******************************************************************
*           判断当前连接是否有效
* 版权所有：北京中科虹霸科技有限公司 (IrisKing Inc.)
* 函数名称：Valid
* 功    能：判断当前连接是否有效
* 说    明：
* 参    数：
* 返 回 值：true —— 连接有效；false —— 连接无效
* 创 建 人：yqhe
* 创建日期：2014/2/20 10:24:14
*******************************************************************/
bool PsqlSession::Valid()
{
	Exectime etm(__FUNCTION__);
    try
    {
        // std::string backendName = g_pSqlSession.get_backend_name();
    	if (nullptr == g_pSqlPool)
    	{
    		return false;
    	}

    	// 尝试从池中获取一个能用的连接
        std::string backendName = "";
    	soci::session* pSqlSession = GetSession();
        if (nullptr != pSqlSession)
        {
            backendName = pSqlSession->get_backend_name();
            ReleaseSession(pSqlSession);
        }

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
    }

    return false;
}

// added at 20140605 12:45 by yqhe
// 输出数据库连接是否有可用的
// 如果没有可用的连接，则打印出来，否则不打印，避免输出信息过多
void printPool()
{
#ifdef _DEBUG

	Exectime etm(__FUNCTION__);
	int timeout = 2000;
	size_t count = 0;

    bool bsuccess = g_pSqlPool->try_lease(count, timeout);

    if (bsuccess)
    {
        // 注意：一定要返回到连接池中！！
        g_pSqlPool->give_back(count);
        // std::cout << "连接" << count << "可用" << std::endl;
    }
    else
    {
        std::cerr << "数据库没有连接可用！！" << std::endl;
    }
#endif

}

// 测试是否有可用连接
bool PsqlSession::TryLease()
{
    int timeout = 500;  // 获取连接超时等待时间
    size_t count = 0;
    bool bsuccess = g_pSqlPool->try_lease(count, timeout);

    if (bsuccess)
    {
        // 注意：一定要返回到连接池中！！
        g_pSqlPool->give_back(count);
    }

    return bsuccess;
}

soci::session* PsqlSession::GetConnFromPool()
{
	Exectime etm(__FUNCTION__);
	soci::session* pSqlSession = nullptr;
    try
    {
        // std::string backendName = g_pSqlSession.get_backend_name();
    	if (nullptr == g_pSqlPool)
    	{
    		return pSqlSession;
    	}

    	// std::cout << "Before get : ";
    	printPool();

    	// 尝试从池中获取一个能用的连接
    	// pSqlSession = new soci::session(*g_pSqlPool);
        if (TryLease())
        {
            pSqlSession = new soci::session(*g_pSqlPool);
        }
        else
        {
            // 没有可用连接
            std::cout << "postgresql 数据库没有连接可用！！" << std::endl;
        }


    	printPool();

    }
    catch (std::exception const &e)
    {
        _errString = e.what();
        std::cerr << "Error: " << _errString << std::endl;
    }

    return pSqlSession;

}

void PsqlSession::ReturnConnToPool(soci::session* pSqlSession)
{
	Exectime etm(__FUNCTION__);
    try
    {
    	printPool();
    	if (nullptr != pSqlSession)
    	{
    		delete pSqlSession;
			pSqlSession = nullptr;
    	}
//    	std::cout << "After return : ";
    	printPool();

    }
    catch (std::exception const &e)
    {
        _errString = e.what();
        std::cerr << "Error: " << _errString << std::endl;
    }

    return;
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
soci::session* PsqlSession::GetSession()
{
	Exectime etm(__FUNCTION__);
	return GetConnFromPool();
}

void PsqlSession::ReleaseSession(soci::session* pSqlSession)
{
	Exectime etm(__FUNCTION__);
	ReturnConnToPool(pSqlSession);
    return;
}

void PsqlSession::Close()
{
	Exectime etm(__FUNCTION__);
    try
    {
        // 如果已经关闭，则直接返回
        if (!Valid())
        {
            return;
        }

        for (size_t count=0; count<g_pSqlConnSize; count++)
        {
        	soci::session& sqlSession = g_pSqlPool->at(count);
        	sqlSession.close();
        }

        std::cout << "close all connection! (postgresql pool)" << std::endl;

    }
    catch (std::exception const &e)
    {
        _errString = e.what();
        std::cerr << "Error: " << _errString << std::endl;
    }
}
