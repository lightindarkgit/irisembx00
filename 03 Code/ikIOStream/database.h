#ifndef DATABASE_H
#define DATABASE_H
/******************************************************************************************
** 文件名:   DATABASE_H
×× 主要类:   DataBase
**
** Copyright (c) 中科虹霸有限公司
** 创建人:   刘中昌
** 日  期:   2014-01-15
** 修改人:
** 日  期:
** 描  述:  数据库处理模块，用于封装用sqlite3进行的数据库增删查改及输入指定的查询语句的操作
 *×× 主要模块说明:
**
** 版  本:   1.0.0
** 备  注:
**
*****************************************************************************************/
#include <sqlite3.h>
#include <unistd.h>
#include <cstdio>
#include <cstdlib>
#include <iostream>

using namespace std;
//保存数据库查询结果的结构
typedef struct queryresult
{
	int row;        //查询结果行数
	int col;        //查询结果列数
	char ** result; //查询结果的二维数组
	void clear()
	{
		if (result != NULL)
		{
			sqlite3_free_table(result);
			row = 0;
			col = 0;
		}
	}
    queryresult()         //结构的构造函数
	{
		row = 0;
		col = 0;
		result = NULL;
	}
    ~queryresult()        //析构函数，如果存入了结果，释放内存
	{
		clear();
	}
} QueryResult;

//封装数据库操作的类
class DataBase
{
public:
	DataBase();
	~DataBase();
    bool InitDataBase(string databaseName);    //初始化数据库

    bool OpenDataBase();                            //打开数据库
    bool CloseDataBase();                           //关闭数据库

    bool ModifyDataBase(const string & querySQL);   //修改数据库，插入删除

    bool SelectSQL(const string & querySQL,
            QueryResult & res);                     //运行查询语句querySQL，结果存放到res
    void ClearResult(QueryResult & res);            //清除查询结果结构，以供新的查询结果存入

    bool InsertContainOneBlob(const string insertSQL,const void *pdata, const int length);
    bool InsertContainTwoBlob(const string insertSQL,const void *pdata1, const int length1,
                                  const void *pdata2, const int length2);

    bool BatchBegin();
    bool BatchInsert(const string & querySQL);
    bool BatchCommit();

    bool CreateTable(const string & creatTableSQL); //创建数据库及相关的表
    bool DropTable(const string & tableName);       //创建数据库及相关的表

    char * GetErrorMessage(void);                   //获得错误信息
    void Finalize();                                //析构二进制传输结构

private:
    void RmDataBase();                              //内部函数，用于在初始化的时侯删除sqlite3数据库文件


private:
    sqlite3 * _db;              //数据库指针
    sqlite3_stmt * _stat;       //二进制传输结构
    char * _errMsg;             //数据库操作的错误信息
    bool _bOpen;                //标志数据库是否打开的变量
    QueryResult _result;        //查询数据库的结果
    string  _databaseName; //数据库名称
};
#endif
