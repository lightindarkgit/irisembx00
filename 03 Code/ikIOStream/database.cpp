/******************************************************************************************
** 文件名:   DATABASE_CPP
×× 主要类:   DataBase
**
** Copyright (c) 中科虹霸有限公司
** 创建人:   刘中昌
** 日  期:   2014-01-15
** 修改人:
** 日  期:
** 描  述:  数据库处理模块，用于封装用sqlite3进行的数据库增删查改及输入指定的查询语句的操作 实现部分
 *×× 主要模块说明:
**
** 版  本:   1.0.0
** 备  注:
**
*****************************************************************************************/
#include "database.h"

DataBase::DataBase()
{
    _databaseName = "money.db"; //指定数据库文件名
    _db   = NULL;
    _stat = NULL;
    _errMsg = "";
    _bOpen = false; //开始时数据库关闭
}

DataBase::~DataBase()
{
	//如果打开了数据库，关闭
    if (_db != NULL)
	{
        CloseDataBase();
	}
	//如果分配了结果空间，收回
    if (_result.result != NULL)
	{
        sqlite3_free_table(_result.result);
        _result.row = 0;
        _result.col = 0;
	}
}
/**********************************************************************************************************************************
*                       含有一个字段为二进制流的插入语句函数
*  函 数 名： InsertContainOneBlob
*  功    能：含有一个字段为二进制流的插入语句
*  说    明：
*  返 回 值：
*  创 建 人：刘中昌
*  创建时间：2014-01-20
*  修 改 人：
*  修改时间：
*********************************************************************************************************************************/
bool DataBase::InsertContainOneBlob(const string insertSQL, const void * pdata, const int length)
{
    if(SQLITE_OK == sqlite3_prepare(_db,insertSQL.c_str(),-1,&_stat,0)&&_stat !=NULL)
    {
        sqlite3_bind_blob(_stat,1,pdata,length,NULL);

        //提交到数据库
        int result = sqlite3_step(_stat);
        //重置结构_stat
        if(_stat != NULL)
        {
            sqlite3_reset(_stat);
        }

        if(result !=SQLITE_OK)
        {
            cout<<"sqlite3_step函数错误！"<<endl;
            return false;
        }
    }
    else
    {
        cout<<"sqlite3_prepare函数错误！"<<endl;
        return false;
    }
    return true;
}

/**********************************************************************************************************************************
*                       含有两个字段为二进制流的插入语句函数
*  函 数 名： InsertContainTwoBlob
*  功    能：含有两个字段为二进制流的插入语句
*  说    明：
*  返 回 值：
*  创 建 人：刘中昌
*  创建时间：2014-01-20
*  修 改 人：
*  修改时间：
*********************************************************************************************************************************/
bool DataBase::InsertContainTwoBlob(const string insertSQL,const void *pdata1, const int length1,
                                    const void *pdata2, const int length2)
{
    sqlite3_stmt * stat = NULL;
    if(SQLITE_OK == sqlite3_prepare(_db,insertSQL.c_str(),-1,&stat,0)&&stat !=NULL)
    {
        sqlite3_bind_blob(stat,1,pdata1,length1,NULL);
        sqlite3_bind_blob(stat,2,pdata2,length2,NULL);

        //提交到数据库
        int result = sqlite3_step(stat);
        //释放结构stat
        if(stat != NULL)
        {
            sqlite3_finalize(stat);
        }

        if(result !=SQLITE_OK)
        {
            cout<<"sqlite3_step函数错误！"<<endl;
            return false;
        }
    }
    else
    {
        cout<<"sqlite3_prepare函数错误！"<<endl;
        return false;
    }
    return true;
}

/**********************************************************************************************************************************
*                       析构二进制传输结构函数
*  函 数 名： Finalize
*  功    能：析构二进制传输结构
*  说    明：
*  返 回 值：
*  创 建 人：刘中昌
*  创建时间：2014-01-20
*  修 改 人：
*  修改时间：
*********************************************************************************************************************************/
void DataBase::Finalize()
{
    if(_stat)
    {
        sqlite3_finalize(_stat);
    }
}

/**********************************************************************************************************************************
*                       删除数据库文件函数
*  函 数 名： RmDataBase
*  功    能：删除所有数据，重新初始化
*  说    明：
*  返 回 值：
*  创 建 人：刘中昌
*  创建时间：2014-01-15
*  修 改 人：
*  修改时间：
*********************************************************************************************************************************/
void DataBase::RmDataBase(void)
{
	//如果该文件存在，删除之。linux里有效，其它系统应该要修改
    if (access(_databaseName.c_str(), F_OK) != -1)
	{
        system(("rm " +_databaseName).c_str());
	}
}


/**********************************************************************************************************************************
*                       打开数据库函数
*  函 数 名： OpenDataBase
*  功    能：打开数据库
*  说    明：
*  返 回 值：
*  创 建 人：刘中昌
*  创建时间：2014-01-15
*  修 改 人：
*  修改时间：
*********************************************************************************************************************************/
bool DataBase::OpenDataBase(void)
{
    int rc = sqlite3_open(_databaseName.c_str(), &_db);
	if (rc)
	{
        _errMsg = const_cast<char*>(sqlite3_errmsg(_db));
		return false;
	}
    _bOpen = true;
	return true;
}


/**********************************************************************************************************************************
*                       关闭数据库函数
*  函 数 名： CloseDataBase
*  功    能：关闭数据库
*  说    明：
*  参    数：
*  返 回 值：
*  创 建 人：刘中昌
*  创建时间：2014-01-15
*  修 改 人：
*  修改时间：
*********************************************************************************************************************************/
bool DataBase::CloseDataBase(void)
{
    if (_bOpen)
	{
        sqlite3_close(_db);
        _bOpen = false;
		return true;
	}
	//若数据库是打开的但程序还是到这里，说明关闭有误。
    if (_bOpen)
	{
		return false;
	}
	//到这说明数据库本来就是没打开的。
	return true;
}

/**********************************************************************************************************************************
*                       创建数据库函数
*  函 数 名： CreateDataBase
*  功    能：创建数据库
*  说    明：
*  参    数：
*           querySQL:SQL语句
*  返 回 值：
*  创 建 人：刘中昌
*  创建时间：2014-01-15
*  修 改 人：
*  修改时间：
*********************************************************************************************************************************/
bool DataBase::CreateTable(const string &creatTableSQL)
{
	//如果数据库未打开，先打开数据库（如果没有该数据库则自动创建）
    if (!_bOpen)
	{
        if (!OpenDataBase())
		{
			return false;
		}
	}

    sqlite3_stmt * statement;
    if (sqlite3_prepare_v2(_db, creatTableSQL.c_str(), -1, &statement, NULL) != SQLITE_OK)
	{
        _errMsg = "创建数据表失败!";
		return false;
	}
	int success = sqlite3_step(statement);
	sqlite3_finalize(statement);
	if (success != SQLITE_DONE)
	{
        _errMsg = "创建数据表失败!";
		return false;
	}

   // CloseDataBase();
	return true;
}

/**********************************************************************************************************************************
*                       创建数据库函数
*  函 数 名： CreateDataBase
*  功    能：创建数据库
*  说    明：
*  参    数：
*           querySQL:SQL语句
*  返 回 值：
*  创 建 人：刘中昌
*  创建时间：2014-01-15
*  修 改 人：
*  修改时间：
*********************************************************************************************************************************/
bool DataBase::DropTable(const string & tableName)
{
    //如果数据库未打开，先打开数据库（如果没有该数据库则自动创建）
    if (!_bOpen)
    {
        if (!OpenDataBase())
        {
            return false;
        }
    }

    sqlite3_stmt * statement;
    if (sqlite3_prepare_v2(_db, ("DROP TABLE " + tableName).c_str(), -1, &statement, NULL) != SQLITE_OK)
    {
        _errMsg = "删除数据表失败!";
        return false;
    }
    int success = sqlite3_step(statement);
    sqlite3_finalize(statement);
    if (success != SQLITE_DONE)
    {
        _errMsg = "删除数据表失败!";
        return false;
    }
}

/**********************************************************************************************************************************
*                       获取错误信息函数
*  函 数 名： GetErrorMessage
*  功    能：获取错误信息
*  说    明：
*  参    数：
*  返 回 值：最后一次错误信息
*  创 建 人：刘中昌
*  创建时间：2014-01-15
*  修 改 人：
*  修改时间：
*********************************************************************************************************************************/
char * DataBase::GetErrorMessage(void)
{
    return _errMsg;
}

/**********************************************************************************************************************************
*                       初始化数据库函数
*  函 数 名： InitDataBase
*  功    能：初始化数据库
*  说    明：
*  参    数：
*           databaseName:数据库名
*  返 回 值：
*  创 建 人：刘中昌
*  创建时间：2014-01-15
*  修 改 人：
*  修改时间：
*********************************************************************************************************************************/
bool DataBase::InitDataBase(string databaseName)
{
    _databaseName = databaseName;
    return true;
//    RmDataBase();
//    CreateTable();
}

/**********************************************************************************************************************************
*                       修改数据库函数
*  函 数 名： ModifyDataBase
*  功    能：插入、删除
*  说    明：修改数据库（插入、删除）
*  参    数：
*           querySQL:SQL语句
*  返 回 值：
*  创 建 人：刘中昌
*  创建时间：2014-01-15
*  修 改 人：
*  修改时间：
*********************************************************************************************************************************/
bool DataBase::ModifyDataBase(const string & querySQL)
{
    if (!_bOpen)
	{
        OpenDataBase();
	}
	sqlite3_stmt * statement;
    if (sqlite3_prepare_v2(_db, querySQL.c_str(), -1, &statement, NULL) != SQLITE_OK)
	{
        _errMsg = "修改数据库失败!";
		return false;
	}

	int success = sqlite3_step(statement);
	sqlite3_finalize(statement);
	if (success != SQLITE_DONE)
	{
        _errMsg = "修改数据库失败!";
		return false;
	}

    CloseDataBase();
	return true;
}

/**********************************************************************************************************************************
*                       批量添加开始函数
*  函 数 名： BatchBegin
*  功    能：批量添加开始函数
*  说    明：
*  参    数：
*  返 回 值：
*  创 建 人：刘中昌
*  创建时间：2014-01-16
*  修 改 人：
*  修改时间：
*********************************************************************************************************************************/
bool DataBase::BatchBegin()
{
    if (!_bOpen)
    {
        OpenDataBase();
    }

//    int res = sqlite3_exec(pDB,"begin transaction;",0,0, &errMsg);

//     for (int i= 1; i < 10; ++i)
//     {
//      std::stringstream strsql;
//      strsql << "insert into test_tab  values(";
//      strsql  << i << ","<< (i+10) << ");";
//      std::string str = strsql.str();
//      res = sqlite3_exec(pDB,str.c_str(),0,0, &errMsg);
//      if (res != SQLITE_OK)
//      {
//       std::cout << "执行SQL 出错." << errMsg << std::endl;
//       return -1;
//      }
//     }

//     res = sqlite3_exec(pDB,"commit transaction;",0,0, &errMsg);

    int success = sqlite3_exec(_db,"begin transaction;", 0, 0, &_errMsg);

    if (success != SQLITE_OK)
    {
        std::cout<<success<<std::endl;
        std::cout<<sqlite3_errmsg(_db)<<std::endl;
        _errMsg = "批量插入开始失败!";
        sqlite3_rollback_hook(_db,0,0);

        return false;
    }
    return true;
}

/**********************************************************************************************************************************
*                      插入数据函数
*  函 数 名： BatchInsert
*  功    能：插入数据
*  说    明：
*  参    数：
*           querySQL:插入SQL语句
*  返 回 值：
*  创 建 人：刘中昌
*  创建时间：2014-01-16
*  修 改 人：
*  修改时间：
*********************************************************************************************************************************/
bool DataBase::BatchInsert(const string & querySQL)
{
    if (!_bOpen)
    {
        //OpenDataBase();
        return false;
    }

    int success = sqlite3_exec(_db, querySQL.c_str(), 0, 0, &_errMsg);
    if (success != SQLITE_OK)
    {
        std::cout<<success<<std::endl;
        std::cout<<sqlite3_errmsg(_db)<<std::endl;

        _errMsg = "批量插入开始失败!";
        sqlite3_rollback_hook(_db,0,0);
        return false;
    }
    return true;
}

/**********************************************************************************************************************************
*                       事务提交函数
*  函 数 名： BatchCommit
*  功    能： 事务提交
*  说    明：
*  参    数：
*  返 回 值：
*  创 建 人：刘中昌
*  创建时间：2014-01-16
*  修 改 人：
*  修改时间：
*********************************************************************************************************************************/
bool DataBase::BatchCommit()
{
    if (!_bOpen)
    {
//        OpenDataBase();
        return false;
    }

    int success = sqlite3_exec(_db, "commit transaction;", 0, 0, &_errMsg);

    if (success != SQLITE_OK)
    {
        std::cout<<success<<std::endl;
        std::cout<<sqlite3_errmsg(_db)<<std::endl;

        _errMsg = "批量插入提交失败！";
        sqlite3_rollback_hook(_db,0,0);
        return false;
    }
    return true;
}

/**********************************************************************************************************************************
*                       运行SQL查询函数
*  函 数 名： RunSQL
*  功    能：执行SQL查询语句
*  说    明：
*  参    数：
*           querySQL:SQL语句
*           res     :结果
*  返 回 值：
*  创 建 人：刘中昌
*  创建时间：2014-01-15
*  修 改 人：
*  修改时间：
*********************************************************************************************************************************/
bool DataBase::SelectSQL(const string & querySQL,
	    QueryResult & res)
{
	//如果还没打开就查询，会报错。
    if (!_bOpen)
	{
        if (!OpenDataBase())
		{
			return false;
		}
	}
    int rc = sqlite3_get_table(_db, querySQL.c_str(), &res.result, &res.row,
             &res.col, &_errMsg);
	if (rc != 0)
	{
		return false;
	}

    if (!CloseDataBase())
	{
		return false;
	}
	return true;
}


/**********************************************************************************************************************************
*                       清除数据函数
*  函 数 名： ClearResult
*  功    能：将数据导出
*  说    明：清除查询结果结构，释放内存，供新的结果存入。因为使用
*           类的过程中会多次运行查询，多次获得结果，不能等程序
*           结束后自动调用结构的析构函数来释放内存。
*  参    数：
*           res:需要清除结果
*  返 回 值：
*  创 建 人：刘中昌
*  创建时间：2014-01-15
*  修 改 人：
*  修改时间：
*********************************************************************************************************************************/
void DataBase::ClearResult(QueryResult & res)
{
    if (res.row != 0 || res.col != 0 || res.result != NULL)
	{
        sqlite3_free_table(res.result);
        res.result = NULL;
		res.row = 0;
		res.col = 0;
	}
}
