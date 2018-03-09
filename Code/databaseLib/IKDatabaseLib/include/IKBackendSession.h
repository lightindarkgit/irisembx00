/*******************************************************************
** 文件名称：IKBackendSession.h
** 主 要 类：IBackendSession及其实现类
** 描    述：
**           
** 版权所有：北京中科虹霸科技有限公司 (IrisKing Inc.)
** 创 建 人：yqhe
** 创建日期：2014/2/20 11:05:16
**           
** 版    本：0.0.1
** 备    注：命名及代码编写遵守C++编码规范
*******************************************************************/
#pragma once

#include <soci.h>
#include "IKDatabaseType.h"

// 数据库连接接口类
class IBackendSession
{
public:
    virtual ~IBackendSession() 
    {
    }

    virtual bool Valid() = 0;

    virtual soci::session* GetSession() = 0;
    virtual void ReleaseSession(soci::session* pSession) = 0;

    virtual void Close() = 0;
};

// 数据库连接接口实现，单连接postgresql数据库
class PsqlSession : public IBackendSession
{
public:
    
    // 无参数构造函数，打开缺省数据库
    PsqlSession();

    // 按照连接参数，打开缺省数据库
    PsqlSession(EnumDBConfigType configType, const std::string &connString);
    
    ~PsqlSession()
    {
    }

    bool Valid();

    soci::session* GetSession();
    void ReleaseSession(soci::session* pSession);

    void Close();

    void GiveBack();

private:
    // static soci::session g_pSqlSession; // postgresql连接单一实例
    std::string          _errString;

private:
    bool OpenDefaultDB();
    bool OpenInstantDB(const std::string &connParam);
    bool OpenXMLDB();
    bool CreateConnPool(const std::string &connParam);
    soci::session* GetConnFromPool();
    void ReturnConnToPool(soci::session* pSqlSession);

    // added at 20161223 by yqhe
    // 测试是否有可用连接，如果有，则返回true，如果没有，返回false
    // 发现当连接池用尽时，会进入无限等待状态，所以在外部增加一个测试函数
    // 如果没有连接，返回nullptr
    bool TryLease();
};

// 数据库连接创建工厂类
class BackendFactory
{
public:
    // 创建数据库连接，可用缺省连接或指定连接，可选择单连接或连接池方式
    // 缺省为以单连接方式连接本地postgresql irisApp数据库
    soci::session* CreateBackend(const std::string &connString = "", 
                EnumDBConnType connType = connSingle);

    // 关闭连接
    void Close(const std::string &backend);

    void ReleaseSession(const std::string &backendName, soci::session* pSession);

private:
    soci::session* CheckBackend(IBackendSession *backend);
};

////////////////////////////////
// 数据库连接接口实现，单连接sqlite数据库
class SqliteSession : public IBackendSession
{
public:
    // 带参数构造函数，打开指定数据库
    SqliteSession(EnumDBConfigType configType, const std::string &connString);
    SqliteSession();
    
    ~SqliteSession()
    {
    }

    bool Valid();

    soci::session* GetSession();

    void ReleaseSession(soci::session* pSession);

    void Close();

private:
    static soci::session g_sqliteSession; // sqlite连接单一实例
    static bool			 _bValid;
    std::string          _errString;

private:
    bool OpenDefaultDB();
    bool OpenInstantDB(const std::string &connString);
    bool OpenXMLDB();

    bool CheckValid();
};

