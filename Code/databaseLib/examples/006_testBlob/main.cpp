/*******************************************************************
** 文件名称：main.cpp
** 主 要 类：
** 描    述：测试soci访问postgresql数据库中的虹膜数据
**           保存到SQLite数据库中的功能
**
** 版权所有：北京中科虹霸科技有限公司 (IrisKing Inc.)
** 创 建 人：yqhe
** 创建日期：2014/2/14 14:08
**
** 版    本：0.0.1
** 备    注：命名及代码编写遵守C++编码规范
*******************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <cstring>
#include "testSOCI.h"
#include <postgresql/soci-postgresql.h>
#include <sqlite3/soci-sqlite3.h>


using namespace std;
using namespace soci;

// 从postgresql中读出虹膜数据，为了方便，把两个数据库中字段名称设置为一样的
const char codeTableName[]   = "iris_data-2.1.0";
const char personTableName[] = "person_info";

const char sqliteCodeTableName[]   = "irisData210";
const char sqlitePersonTableName[] = "personInfo";

const char didField[]     = "iris_data_id";
const char irisCodeField[]= "iris_code";
const char devSnField[]   = "dev_sn";
const char regTimeField[] = "reg_time";

const char pidField[]     = "person_id";
const char nameField[]    = "person_name";
const char workSnField[]  = "person_work_sn";


/*******************************************************************
*           把std::tm格式的数据整理成string类型，可供输出
* 版权所有：北京中科虹霸科技有限公司 (IrisKing Inc.)
* 函数名称：outputTime
* 功    能：把std::tm格式的数据整理成string类型，可供输出
* 说    明：日期+时间，格式为1970-01-01 00:00:00
* 参    数：
*           输入参数
*           dispTime —— 需要显示的时间
* 返 回 值：无
* 创 建 人：yqhe
* 创建日期：2014/2/13 13:25:30
*******************************************************************/
std::string outputTime(const std::tm &dispTime)
{
    char mbstr[100];

    std::tm bakTime = dispTime;
    std::time_t tt = std::mktime(&bakTime);
    if (tt == -1)
    {
        mbstr[0] = '\0';
    }
    else
    {
        std::strftime(mbstr, 100, "%Y-%m-%d %H:%M:%S", &bakTime);
    }

    return (mbstr);
}

/*******************************************************************
*           连接postgresql数据库
* 版权所有：北京中科虹霸科技有限公司 (IrisKing Inc.)
* 函数名称：connPsqlDatabase
* 功    能：连接postgresql数据库
* 说    明：
* 参    数：
*           输出参数
*           sql —— 如果返回true，则为与指定的数据库的连接
* 返 回 值：true —— 连接成功；false —— 连接失败
* 创 建 人：yqhe
* 创建日期：2014/2/14 15:20:00
*******************************************************************/
bool connPsqlDatabase(session &sql)
{
    string connParam;  // parameters

    // 根据实际情况，设置自己使用的连接参数

    // 连接远程数据库的连接参数
    connParam = string("host=10.2.1.117 dbname=irisData.qujiang user=postgres password=123456");

    try
    {
        // Make the session and connect to database
        cout << "Connecting...";
        connection_parameters params("postgresql", connParam);
        sql.open(params);

        std::cout << " ok!" << std::endl;

        return true;

    }
    catch ( soci::postgresql_soci_error const &e)
    {
        std::cout // << "PostgreSQL error: "    << e.sqlstate()
                  << " " << e.what() << std::endl;
    }
    catch (exception const &e)
    {
        cerr << "Error: " << e.what() << endl;
    }

    return false;
}


/*******************************************************************
*           连接SQLite数据库
* 版权所有：北京中科虹霸科技有限公司 (IrisKing Inc.)
* 函数名称：connSqiteDatabase
* 功    能：连接SQlite数据库
* 说    明：
* 参    数：
*           输出参数
*           sql —— 如果返回true，则为与指定的数据库的连接
* 返 回 值：true —— 连接成功；false —— 连接失败
* 创 建 人：yqhe
* 创建日期：2014/2/14 15:30:00
*******************************************************************/
bool connSqliteDatabase(session &sql)
{
    string connParam;  // parameters

    // 根据实际情况，设置自己使用的连接参数

    // 如果数据库不存在，则会在当前路径下建立一个新的文件s
    connParam = string("sqliteIrisDb");

    try
    {
        // Make the session and connect to database
        cout << "Connecting...";
        connection_parameters params("sqlite3", connParam);
        sql.open(params);

        std::cout << " ok!" << std::endl;

        return true;

    }
    catch (exception const &e)
    {
        cerr << "Error: " << e.what() << endl;
    }

    return false;
}


/*******************************************************************
*           从postgresql数据库中读虹膜数据
* 版权所有：北京中科虹霸科技有限公司 (IrisKing Inc.)
* 函数名称：readIrisFromPsql
* 功    能：从postgresql数据库中读虹膜数据
* 说    明：
* 参    数：
*           输入参数
*           sql —— 数据库连接
* 返 回 值：true —— 读取成功；false —— 读取失败
* 创 建 人：yqhe
* 创建日期：2014/2/14 15:39
*******************************************************************/
bool readIrisFromPsql(session &sql, session &sqlSqlite)
{
    try
    {
        // 组织查询语句
        string queryString;

        // 设置中文编码，在windows环境下，8.4需要设置编码GBK
        // 在Linux环境下，9.1需要设置编码UNICODE，数据库缺省client_encoding是UNICODE
        // 如果设置不正确，则中文会显示乱码
        // queryString = string("set client_encoding to 'UNICODE';");
        // sql << queryString;

        std::ostringstream oss;

        oss << "SELECT "
            << " "
            << didField
            << ", "
            << pidField
            << ", "
            << devSnField
            << ", "
            << regTimeField
            << ", "
            << irisCodeField
            << " FROM \""
            << codeTableName
            << "\" ORDER BY "
            << didField
            << " limit 100"
            // << queryCount
            << ";"
            ;
//        oss << "SELECT "
//            << irisCodeField
//            << " FROM \""
//            << codeTableName
//            << "\" ORDER BY "
//            << didField
//            << " limit 1"
//            // << queryCount
//            << ";"
//            ;

        queryString = oss.str();
        std::cout << queryString << std::endl;

        std::vector<IrisData> vecIrisData;

        // 执行查询
        rowset<IrisData> rs = (sql.prepare << queryString);

        // 显示查询结果
        for (rowset<IrisData>::const_iterator it=rs.begin();
                    it!=rs.end(); ++it)
        {
            IrisData const &irisCode=*it;
            std::cout << "logid = " << setw(4) << irisCode.id
                << ", feaSize = " << setw(2) << irisCode.icode.size()
                << ", pid = " << setw(4) << irisCode.pid
                // << ", devSn = " << setw(8) << irisCode.devSn
                << ", createTime = " << outputTime(irisCode.regTime)
                << std::endl;

//            std::cout << "虹膜特征数据：" << std::endl;
//            std::cout << irisCode.icode << std::endl;
//
//            std::cout << "==========================================" << std::endl;

            vecIrisData.push_back(irisCode);
        }

        // 组织插入语句
        oss.str("");
        oss << "INSERT INTO  "
            << sqliteCodeTableName
            << "("
            << didField
            << ", "
            << irisCodeField
            << ", "
            << pidField
            << ", "
            << devSnField
            << ", "
            << regTimeField
            << ") values (:"
            << didField
            << ", :"
            << irisCodeField
            << ", :"
            << pidField
            << ", :"
            << devSnField
            << ", :"
            << regTimeField
            << ");"
            ;

        queryString = oss.str();
        std::cout << queryString << std::endl;

        IrisData valCode;
        statement st = (sqlSqlite.prepare << queryString, use(valCode));

        transaction tr(sqlSqlite);
        std::vector<IrisData>::const_iterator it;

        for (it=vecIrisData.begin(); it!=vecIrisData.end(); ++it)
        {
            valCode = (*it);
            st.execute(true);
        }

        tr.commit();

        return true;
    }
    catch ( soci::postgresql_soci_error const &e)
    {
        std::cout // << "PostgreSQL error: "    << e.sqlstate()
                  << " " << e.what() << std::endl;
    }
    catch (exception const &e)
    {
        cerr << "Some other error: " << e.what() << endl;
    }

    return false;

}


/*******************************************************************
*           在sqlite数据库中创建虹膜数据表
* 版权所有：北京中科虹霸科技有限公司 (IrisKing Inc.)
* 函数名称：createSqliteTable
* 功    能：在sqlite数据库中创建虹膜数据表
* 说    明：
* 参    数：
*           输入参数
*           sql —— 数据库连接
* 返 回 值：true —— 读取成功；false —— 读取失败
* 创 建 人：yqhe
* 创建日期：2014/2/14 17:30
*******************************************************************/
bool createSqliteTable(session &sql)
{
    try
    {
        // 组织查询语句
        string queryString;
        std::ostringstream oss;
/*
const char didField[]     = "iris_data_id";
const char irisCodeField[]= "iris_code";
const char devSnField[]   = "dev_sn";
const char regTimeField[] = "reg_time";

const char pidField[]     = "person_id";
*/
        oss << "CREATE TABLE "
            << sqliteCodeTableName
            << "\n ( \n    "
            << didField
            << " int4 PRIMARY KEY, \n    "
            << irisCodeField
            << " varchar, \n    "
            << pidField
            << " int4 NOT NULL, \n    "
            << devSnField
            << " varchar NOT NULL, \n    "
            << regTimeField
            << " timestamp \n"
            << ")"
            << ";"
            ;
        queryString = oss.str();
        std::cout << queryString << std::endl;

        // 执行查询，创建表
        sql << queryString;

        return true;
    }
    catch (exception const &e)
    {
        cerr << "Error: " << e.what() << endl;
    }

    return false;
}

/*******************************************************************
*           检测虹膜表是否存在
* 版权所有：北京中科虹霸科技有限公司 (IrisKing Inc.)
* 函数名称：checkIrisDataTable
* 功    能：检测虹膜表是否存在
* 说    明：
* 参    数：
*           输入参数
*           sql —— 数据库的连接
* 返 回 值：true —— 表已存在；false —— 表未存在
* 创 建 人：yqhe
* 创建日期：2014/2/13 17:25:00
*******************************************************************/
bool checkIrisDataTable (session &sql)
{
    int count;
    indicator ind;

    try
    {
        // 组织查询语句
        string queryString;
        std::ostringstream oss;

        oss << "SELECT count(*) FROM sqlite_master WHERE type='table' AND name='"
            << sqliteCodeTableName
            << "';"
            ;
        queryString = oss.str();
        std::cout << queryString << std::endl;

        // 执行查询
        sql << queryString, into(count, ind);

        // 显示查询结果
        std::cout << "totalCount=" << count << ", indicator=" << ind << std::endl;

        if (count>0)
        {
            return true;
        }
    }
    catch (exception const &e)
    {
        cerr << "Error: " << e.what() << endl;
    }

    return false;
}

/*******************************************************************
*           从sqlite数据库中读虹膜数据
* 版权所有：北京中科虹霸科技有限公司 (IrisKing Inc.)
* 函数名称：readIrisFromSqlite
* 功    能：从sqlite数据库中读虹膜数据
* 说    明：
* 参    数：
*           输入参数
*           sql —— 数据库连接
* 返 回 值：true —— 读取成功；false —— 读取失败
* 创 建 人：yqhe
* 创建日期：2014/2/14 15:39
*******************************************************************/
bool readIrisFromSqlite(session &sql)
{
    try
    {
        // 组织查询语句
        string queryString;

        // 设置中文编码，在windows环境下，8.4需要设置编码GBK
        // 在Linux环境下，9.1需要设置编码UNICODE，数据库缺省client_encoding是UNICODE
        // 如果设置不正确，则中文会显示乱码
        // queryString = string("set client_encoding to 'UNICODE';");
        // sql << queryString;

        std::ostringstream oss;

        oss << "SELECT "
            << " "
            << didField
            << ", "
            << pidField
            << ", "
            << devSnField
            << ", "
            << regTimeField
            << ", "
            << irisCodeField
            << " FROM \""
            << sqliteCodeTableName
            << "\" ORDER BY "
            << didField
            << " limit 100"
            // << queryCount
            << ";"
            ;

        queryString = oss.str();
        std::cout << queryString << std::endl;

        std::vector<IrisData> vecIrisData;

        // 执行查询
        rowset<IrisData> rs = (sql.prepare << queryString);

        // 显示查询结果
        for (rowset<IrisData>::const_iterator it=rs.begin();
                    it!=rs.end(); ++it)
        {
            IrisData const &irisCode=*it;
            std::cout << "logid = " << setw(4) << irisCode.id
                << ", feaSize = " << setw(2) << irisCode.icode.size()
                << ", pid = " << setw(4) << irisCode.pid
                // << ", devSn = " << setw(8) << irisCode.devSn
                << ", createTime = " << outputTime(irisCode.regTime)
                << std::endl;

            std::cout << "虹膜特征数据：" << std::endl;
            std::cout << irisCode.icode << std::endl;

            std::cout << "==========================================" << std::endl;

            vecIrisData.push_back(irisCode);
        }

        return true;
    }
    catch (exception const &e)
    {
        cerr << "Some other error: " << e.what() << endl;
    }

    return false;

}

void lastComment ()
{
    std::cout << std::endl;
    std::cout << "按回车键退出本次测试..." << std::endl;
    std::cin.peek();
}

/*******************************************************************
*           主程序
* 版权所有：北京中科虹霸科技有限公司 (IrisKing Inc.)
* 函数名称：main
* 功    能：主程序
* 说    明：
* 参    数：
* 返 回 值：无
* 创 建 人：yqhe
* 创建日期：2014/2/13 16:36:11
*******************************************************************/
int main( void )
{
    // 删除数据库文件
    std::cout << "删除数据库文件'sqliteIrisDb'，按回车键继续（Ctrl+Z退出）..." << std::endl;
    std::cin.peek();
    remove("sqliteIrisDb");

    bool bret;

    // 连接postgresql数据库
    session sqlPsql;
    bret = connPsqlDatabase(sqlPsql);
    std::cout << "Connect Postgresql Database return : " << bret << std::endl;

    if (!bret)
    {
        lastComment();
        return 0;
    }

    // 连接sqlite数据库
    session sqlSqite;
    bret = connSqliteDatabase(sqlSqite);
    std::cout << "Connect SQLite Database return : " << bret << std::endl;

    if (!bret)
    {
        lastComment();
        return 0;
    }

    // 在sqlite数据库中创建特征表
    bret = createSqliteTable(sqlSqite);
    if (!bret)
    {
        lastComment();
        return 0;
    }

    bret = checkIrisDataTable(sqlSqite);
    if (!bret)
    {
        lastComment();
        return 0;
    }

    // 从postgresql数据库中读虹膜数据
    bret = readIrisFromPsql(sqlPsql, sqlSqite);

    bret = readIrisFromSqlite(sqlSqite);

    std::cout << std::endl;
    std::cout << "按回车键退出本次测试..." << std::endl;
    std::cin.peek();

    return 0;
}
