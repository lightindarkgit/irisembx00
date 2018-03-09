/*******************************************************************
 ** 文件名称：IKDatabaseLib.h
 ** 主 要 类：对数据库访问的接口类头文件
 ** 描    述：
 **
 ** 版权所有：北京中科虹霸科技有限公司 (IrisKing Inc.)
 ** 创 建 人：yqhe
 ** 创建日期：2014/2/18 14:46:35
 **
 ** 版    本：0.0.1
 ** 备    注：命名及代码编写遵守C++编码规范
 *******************************************************************/
#pragma once
#include <soci.h>

#include <libpq-fe.h>
#include "IKDatabaseType.h"
#include "IKTransaction.h"
#include "iostream"

// 数据库访问类
class IKDBLIB_API DatabaseAccess
{
public:
	// 构造函数
	// 缺省连接本地的postgresql中irisApp库
	DatabaseAccess();

    // 构造函数
    // 根据连接参数创建（目前未实现，仍连接缺省数据库）
    DatabaseAccess(const std::string &connString,
                EnumDBConnType connType);

	// 析构函数
	~DatabaseAccess();

	// 获得连接
	PGconn* GetPsqlBackend();

	soci::session* GetSession();		// 获得数据库连接

	// 关闭连接
	void Close();

	/*******************************************************************
	 *           无输出的查询
	 * 版权所有：北京中科虹霸科技有限公司 (IrisKing Inc.)
	 * 函数名称：Query
	 * 功    能：无输出的查询
	 * 说    明：例如创建、删除等
	 * 参    数：
	 *           输入参数
	 *           queryString     —— 查询SQL语句
	 * 返 回 值：EnumReturnValue —— dbSuccess，表示成功执行
	 * 创 建 人：yqhe
	 * 创建日期：2014/2/19 9:24:59
	 *******************************************************************/
    EnumReturnValue Query(const std::string& queryString)
    {
        try
        {
            // 检测数据库连接是否有效
            if (!Valid())
            {
                return dbErrorOpen;
            }

            // 执行查询语句
            (*_sql) << queryString;
            return dbSuccess;
        }
        catch (std::exception const &e)
        {
            _errString = e.what();
            std::cerr << "Error: " << _errString << std::endl;
            return dbErrorQuery;
        }
        catch (...)
        {
            std::cerr << "Error: "<< __FUNCTION__ << " Unknown" << std::endl;
            return dbErrorQuery;
        }

    }
	/*******************************************************************
	 *           输出一个查询结果的查询
	 * 版权所有：北京中科虹霸科技有限公司 (IrisKing Inc.)
	 * 函数名称：Query
	 * 功    能：输出一个查询结果的查询
	 * 说    明：例如，查询有几条数据，只返回一个count
	 * 参    数：
	 *           输入参数
	 *           queryString     —— 查询SQL语句
	 *           输出参数
	 *           result          —— 返回结果，目前可以是以下类型
	 *                                char, (short, int, unsigned long, longlong, double)
	 *                                std::string, std::tm（日期时间）
	 * 返 回 值：EnumReturnValue —— dbSuccess，表示成功执行
	 * 创 建 人：yqhe
	 * 创建日期：2014/2/19 8:29:19
	 *******************************************************************/
    template <typename T>
    EnumReturnValue Query(const std::string &queryString, T& result)
    {
        try
        {
            // 检测数据库连接是否有效
            if (!Valid())
            {
                return dbErrorOpen;
            }

            soci::indicator ind;
            // 执行查询语句
            (*_sql) << queryString, soci::into(result, ind);
            return dbSuccess;
        }
        catch (std::exception const &e)
        {
            _errString = e.what();
            std::cerr << "Error: " << _errString << std::endl;
        }

        return dbErrorQuery;
    }

	/*******************************************************************
	 *           有多条输出的查询
	 * 版权所有：北京中科虹霸科技有限公司 (IrisKing Inc.)
	 * 函数名称：Query
	 * 功    能：有多条输出的查询
	 * 说    明：调用前需要根据查询结果按照SOCI要求的格式
	 *          写好绑定的数据结构，可参见示例头文件中Person数据结构
	 *          函数内部会清除vecQueryResult查询前的数据，请注意
	 * 参    数：
	 *           输入参数
	 *           queryString     —— 查询SQL语句
	 *           输出参数
	 *           vecQueryResult  —— 返回结果，模板T即为SOCI绑定的数据结构
	 *                                除按照格式自定义外，静态已支持的数据类型如下：
	 *                                char, (short, int, unsigned long, longlong, double)
	 *                                std::string, std::tm（日期时间）
	 * 返 回 值：EnumReturnValue —— dbSuccess，表示成功执行
	 * 创 建 人：yqhe
	 * 创建日期：2014/2/19 13:05:10
	 *******************************************************************/
	template<typename T>
    EnumReturnValue Query(const std::string queryString, 
                    std::vector<T>& vecQueryResult)
    {
        try
        {
            // 检测数据库连接是否有效
            if (!Valid())
            {
                return dbErrorOpen;
            }

        	// 使用之前清除已有内容
        	vecQueryResult.clear();

            // 执行查询语句
            // 查询结果放在rowset记录中
            soci::rowset<T> rs = ((*_sql).prepare << queryString);

            // 收集查询结果
            typename soci::rowset<T>::const_iterator it;
            for (it=rs.begin(); it!=rs.end(); ++it)
            {
                T const &templateResult=*it;
                vecQueryResult.push_back(templateResult);
            }
            return dbSuccess;
        }
        catch (std::exception const &e)
        {
            _errString = e.what();
            std::cerr << "Error: " << _errString << std::endl;
        }

		return dbErrorQuery;
	}

	/*******************************************************************
	 *           插入数据
	 * 版权所有：北京中科虹霸科技有限公司 (IrisKing Inc.)
	 * 函数名称：Insert
	 * 功    能：插入数据
	 * 说    明：注意，此函数插入数据缺省是带事务的操作，
	 * 		   如果需要插入时自己控制事务，请调用另一个函数InsertWithoutTrans
	 * 参    数：
	 *           输入参数
	 *           queryString     —— 查询SQL语句
	 *           vecInsertData   —— 需要插入的数据，模板T即为SOCI绑定的数据结构
	 *                                除按照格式自定义外，静态已支持的数据类型如下：
	 *                                char, (short, int, unsigned long, longlong, double)
	 *                                std::string, std::tm（日期时间）
	 * 返 回 值：EnumReturnValue —— dbSuccess，表示成功执行
	 * 创 建 人：yqhe
	 * 创建日期：2014/2/20 12:33:56
	 *******************************************************************/
    template <typename T>
    EnumReturnValue Insert(const std::string& queryString,
    		const std::vector<T>& vecInsertData)
    {
        try
        {
            // 检测数据库连接是否有效
            if (!Valid())
            {
                return dbErrorOpen;
            }

            T singleData;
            soci::statement st = ((*_sql).prepare << queryString, soci::use(singleData));

            soci::transaction tr(*_sql);

            typename std::vector<T>::const_iterator it;

            for (it=vecInsertData.begin(); it!=vecInsertData.end(); ++it)
            {
                singleData = (*it);
                st.execute(true);
                // 测试插入单条数据
                // (*_sql) << queryString, soci::use(singleData);

            }

            tr.commit();

            return dbSuccess;

        }
        catch (std::exception const &e)
        {
            _errString = e.what();
            std::cerr << "Error: " << _errString << std::endl;
        }

        return dbErrorInsert;
    }

	/*******************************************************************
	 *           插入数据，内部无事务控制
	 * 版权所有：北京中科虹霸科技有限公司 (IrisKing Inc.)
	 * 函数名称：InsertWithoutTrans
	 * 功    能：插入数据，内部无事务控制
	 * 说    明：注意，此函数插入数据是不带事务的操作，
	 * 参    数：
	 *           输入参数
	 *           queryString     —— 查询SQL语句
	 *           vecInsertData   —— 需要插入的数据，模板T即为SOCI绑定的数据结构
	 *                                除按照格式自定义外，静态已支持的数据类型如下：
	 *                                char, (short, int, unsigned long, longlong, double)
	 *                                std::string, std::tm（日期时间）
	 * 返 回 值：EnumReturnValue —— dbSuccess，表示成功执行
	 * 创 建 人：yqhe
	 * 创建日期：2014/5/5 15:36
	 *******************************************************************/
	template<typename T>
	EnumReturnValue InsertWithoutTrans(const std::string& queryString,
                    const std::vector<T>& vecInsertData)
    {
        try
        {

            // 检测数据库连接是否有效
            if (!Valid())
            {
                return dbErrorOpen;
            }

            T singleData;
            soci::statement st = ((*_sql).prepare << queryString, soci::use(singleData));

            typename std::vector<T>::const_iterator it;

            for (it=vecInsertData.begin(); it!=vecInsertData.end(); ++it)
            {
                singleData = (*it);
                st.execute(true);
                // 测试插入单条数据
                // (*_sql) << queryString, soci::use(singleData);

            }

            return dbSuccess;

        }
        catch (std::exception const &e)
        {
            _errString = e.what();
            std::cerr << "Error: " << _errString << std::endl;
        }

        return dbErrorInsert;
    }

	/*******************************************************************
	 *           插入单条数据
	 * 版权所有：北京中科虹霸科技有限公司 (IrisKing Inc.)
	 * 函数名称：Insert
	 * 功    能：插入数据
	 * 说    明：
	 * 参    数：
	 *           输入参数
	 *           queryString     —— 查询SQL语句
	 *           insertData      —— 需要插入的一条数据，模板T即为SOCI绑定的数据结构
	 *                                除按照格式自定义外，静态已支持的数据类型如下：
	 *                                char, (short, int, unsigned long, longlong, double)
	 *                                std::string, std::tm（日期时间）
	 * 返 回 值：EnumReturnValue  —— dbSuccess，表示成功执行
	 *                               dbErrorOpen，数据库尚未成功连接
	 * 创 建 人：yqhe
	 * 创建日期：2014/3/7 15:31:56
	 *******************************************************************/
    template <typename T>
    EnumReturnValue Insert(const std::string& queryString, 
                const T& insertData)
    {
        std::vector<T> vecInsertData;
        vecInsertData.push_back(insertData);


        return Insert(queryString, vecInsertData);
    }

	/*******************************************************************
	 *           插入单条数据，内部无事物控制
	 * 版权所有：北京中科虹霸科技有限公司 (IrisKing Inc.)
	 * 函数名称：InsertWithoutTrans
	 * 功    能：插入数据，内部无事物控制
	 * 说    明：
	 * 参    数：
	 *           输入参数
	 *           queryString     —— 查询SQL语句
	 *           insertData      —— 需要插入的一条数据，模板T即为SOCI绑定的数据结构
	 *                                除按照格式自定义外，静态已支持的数据类型如下：
	 *                                char, (short, int, unsigned long, longlong, double)
	 *                                std::string, std::tm（日期时间）
	 * 返 回 值：EnumReturnValue  —— dbSuccess，表示成功执行
	 *                               dbErrorOpen，数据库尚未成功连接
	 * 创 建 人：yqhe
	 * 创建日期：2014/6/5 11:23
	 *******************************************************************/
    template <typename T>
    EnumReturnValue InsertWithoutTrans(const std::string& queryString, const T& insertData)
    {
        std::vector<T> vecInsertData;
        vecInsertData.push_back(insertData);


        return InsertWithoutTrans(queryString, vecInsertData);
    }

    /*******************************************************************
	 *          修改数据
	 * 版权所有：北京中科虹霸科技有限公司 (IrisKing Inc.)
	 * 函数名称：Update
	 * 功    能：修改数据
	 * 说    明：
	 * 参    数：
	 *           输入参数
	 *           queryString     —— 查询SQL语句
	 *           insertData      —— 需要插入的一条数据，模板T即为SOCI绑定的数据结构
	 *                                除按照格式自定义外，静态已支持的数据类型如下：
	 *                                char, (short, int, unsigned long, longlong, double)
	 *                                std::string, std::tm（日期时间）
	 * 返 回 值：EnumReturnValue  —— dbSuccess，表示成功执行
	 *                               dbErrorOpen，数据库尚未成功连接
	 * 创 建 人：yqhe
	 * 创建日期：2014/3/7 15:31:56
	 *******************************************************************/
	template<typename TUpdate>
    EnumReturnValue Update(const std::string& queryString,
            const TUpdate& updateData)
    {
		return InsertWithoutTrans(queryString, updateData);
	}

private:
	soci::session* _sql;
	std::string _errString;
	std::string _backendName;

private:
	bool ConnectDatabase(const std::string &connString = "",
			EnumDBConnType connType = connSingle);

	bool Valid();

	void ReleaseSession();
};

