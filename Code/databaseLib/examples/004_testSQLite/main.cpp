/*******************************************************************
** 文件名称：main.cpp
** 主 要 类：
** 描    述：测试soci访问SQLite数据库的简单功能
**           连接SQLite数据库     -- connDatabase
**           检测人员表是否存在    -- checkPersonTable
**           创建人员表           -- createPersonTable
**           模拟插入人员数据      -- insertMockPerson
**           取数据条数(count)    -- readPersonCount
**           取多条数据并显示     -- readPerson
**
** 版权所有：北京中科虹霸科技有限公司 (IrisKing Inc.)
** 创 建 人：yqhe
** 创建日期：2014/2/13 16:15:27
**
** 版    本：0.0.1
** 备    注：命名及代码编写遵守C++编码规范
*******************************************************************/
#include <stdio.h>
#include "testSOCI.h"
#include <sqlite3/soci-sqlite3.h>


using namespace std;
using namespace soci;

const char tableName[] = "personBase";
const char nameField[] = "Name";
const char idField[]   = "RecordID";
const char workSnField[]="workSn";
const char timeField[] = "CreateTime";

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
*           连接数据库
* 版权所有：北京中科虹霸科技有限公司 (IrisKing Inc.)
* 函数名称：connDatabase
* 功    能：连接数据库
* 说    明：
* 参    数：
*           输出参数
*           sql —— 如果返回true，则为与指定的数据库的连接
* 返 回 值：true —— 连接成功；false —— 连接失败
* 创 建 人：yqhe
* 创建日期：2014/2/12 15:41:00
*******************************************************************/
bool connDatabase(session &sql)
{
    string connParam;  // parameters

    // 根据实际情况，设置自己使用的连接参数

    // 如果数据库不存在，则会在当前路径下建立一个新的文件s
    connParam = string("sqliteDb");

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
*           检测人员表是否存在
* 版权所有：北京中科虹霸科技有限公司 (IrisKing Inc.)
* 函数名称：checkPersonTable
* 功    能：检测人员表是否存在
* 说    明：
* 参    数：
*           输入参数
*           sql —— 数据库的连接
* 返 回 值：true —— 表已存在；false —— 表未存在
* 创 建 人：yqhe
* 创建日期：2014/2/13 17:25:00
*******************************************************************/
bool checkPersonTable (session &sql)
{
    int count;
    indicator ind;

    try
    {
        // 组织查询语句
        string queryString;
        std::ostringstream oss;

        oss << "SELECT count(*) FROM sqlite_master WHERE type='table' AND name='"
            << tableName
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
*           人员信息数量
* 版权所有：北京中科虹霸科技有限公司 (IrisKing Inc.)
* 函数名称：readPersonCount
* 功    能：人员信息数量
* 说    明：
* 参    数：
* 返 回 值：无
*           输入参数
*           sql —— 与数据库的连接
*           输出参数
*           personCount —— 查到的人员个数
* 创 建 人：yqhe
* 创建日期：2014/2/14 10:00:42
*******************************************************************/
bool readPersonCount(session &sql, int &personCount)
{
    try
    {
        personCount = 0;
        indicator ind;

        int count;

        // 组织查询语句
        string queryString;
        std::ostringstream oss;

        oss << "SELECT count(*) FROM "
            << tableName
            << ";"
            ;
        queryString = oss.str();
        std::cout << queryString << std::endl;

        // 执行查询
        sql << queryString, into(count, ind);

        // 显示查询结果
        std::cout << "totalCount=" << count << ", indicator=" << ind << std::endl;

        personCount = count;

        return true;
    }
    catch (exception const &e)
    {
        cerr << "Error: " << e.what() << endl;
    }

    return false;
}

/*******************************************************************
*           创建personBase表
* 版权所有：北京中科虹霸科技有限公司 (IrisKing Inc.)
* 函数名称：createPersonTable
* 功    能：创建personBase表
* 说    明：
* 参    数：
*           输入参数
*           sql —— 数据库的连接
* 返 回 值：true —— 处理成功；false —— 处理失败
* 创 建 人：yqhe
* 创建日期：2014/2/13 17:29:45
*******************************************************************/
bool createPersonTable (session &sql)
{
    try
    {
        // 组织查询语句
        string queryString;
        std::ostringstream oss;

        oss << "CREATE TABLE "
            << tableName
            << "\n ( \n    logID int4 PRIMARY KEY, \n    "
            << idField
            << " int4 NOT NULL, \n    "
            << nameField
            << " varchar NOT NULL, \n    "
            << workSnField
            << " varchar, \n    "
            << timeField
            << " timestamp DEFAULT (datetime('now', 'localtime'))\n"
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
*           模拟插入person数据
* 版权所有：北京中科虹霸科技有限公司 (IrisKing Inc.)
* 函数名称：insertMockPerson
* 功    能：模拟插入person数据
* 说    明：
* 参    数：
*           输入参数
*           sql —— 数据库的连接
* 返 回 值：true —— 处理成功；false —— 处理失败
* 创 建 人：yqhe
* 创建日期：2014/2/14 10:15:45
*******************************************************************/
bool insertMockPerson(session &sql)
{
    try
    {
        // 组织要写入的数据
        std::vector<Person> vecPerson;

        {
        Person person;
        person.id     = 1;
        person.name   = "张三";
        person.workSn = "test001";

        vecPerson.push_back(person);

        person.id     = 2;
        person.name   = "李斯";
        person.workSn = "test002";

        vecPerson.push_back(person);

        person.id     = 3;
        person.name   = "wangwu";
        person.workSn = "test003";

        vecPerson.push_back(person);

        person.id     = 5;
        person.name   = "唐僧";
        person.workSn = "和尚001";

        vecPerson.push_back(person);
        }

        // 组织查询语句
        string queryString;
        std::ostringstream oss;

        oss << "INSERT INTO  "
            << tableName
            << "("
            << idField
            << ", "
            << nameField
            << ", "
            << workSnField
            << ") values (:"
            << idField
            << ", :"
            << nameField
            << ", :"
            << workSnField
            << ");"
            ;

        queryString = oss.str();
        std::cout << queryString << std::endl;

        // sql << queryString, use(person);
        Person valPerson;
        statement st = (sql.prepare << queryString, use(valPerson));

        transaction tr(sql);
        std::vector<Person>::const_iterator it;

        for (it=vecPerson.begin(); it!=vecPerson.end(); ++it)
        {
            valPerson = (*it);
            st.execute(true);
        }

        tr.commit();

        return true;
    }
    catch (exception const &e)
    {
        cerr << "Error: " << e.what() << endl;
    }

    return false;
}

/*******************************************************************
*           查询指定个数的人员信息
* 版权所有：北京中科虹霸科技有限公司 (IrisKing Inc.)
* 函数名称：readPerson
* 功    能：查询指定个数的人员信息
* 说    明：
* 参    数：
*           输入参数
*           sql —— 与数据库的连接
*           queryCount —— 查询个数，控制个数，避免太多
* 返 回 值：无
* 创 建 人：yqhe
* 创建日期：2014/2/12 16:32:21
*******************************************************************/
bool readPerson(session &sql, int queryCount)
{
    try
    {
        // 组织查询语句
        string queryString;
        std::ostringstream oss;

        oss << "SELECT "
            << idField
            << ", "
            << nameField
            << ", "
            << workSnField
            << ", "
            << timeField
            << " FROM "
            << tableName
            << " ORDER BY "
            << idField
            << " LIMIT "
            << queryCount
            << ";"
            ;
        queryString = oss.str();
        std::cout << queryString << std::endl;

        // 执行查询
        rowset<Person> rs = (sql.prepare << queryString);

        // 显示查询结果
        for (rowset<Person>::const_iterator it=rs.begin();
                    it!=rs.end(); ++it)
        {
            Person const &person=*it;
            std::cout << "id = " << setw(4) << person.id
                << ", name = " << setw(8) << person.name
                << ", workSn = " << setw(8) << person.workSn
                << ", createTime = " << outputTime(person.createTime)
                << std::endl;
        }

        return true;
    }
    catch (exception const &e)
    {
        cerr << "Error: " << e.what() << endl;
    }

    return false;
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
    std::cout << "删除数据库文件'sqliteDb'，按回车键继续（Ctrl+Z退出）..." << std::endl;
    std::cin.peek();
    remove("sqliteDb");

    session sql;
    bool bret = connDatabase(sql);

    std::cout << "Connect Database return : " << bret << std::endl;

    if (true==bret)
    {
        bret = checkPersonTable(sql);

        if (!bret)
        {
            // 要查询的表不存在，则新建一个表，并写入数据
            std::cout << tableName << "表不存在，尝试创建..." << std::endl;
            bret = createPersonTable(sql);
        }
        else
        {
            std::cout << tableName << "表已存在。" << std::endl;
        }

    }

    if (true==bret)
    {
        // person表已存在，查询是否有数据
        int personCount = 0;
        bret = readPersonCount(sql, personCount);

        if ((true==bret)&&(0==personCount))
        {
            std::cout << tableName << "表中没有数据，尝试写入测试数据..." << std::endl;
            bret = insertMockPerson(sql);
        }

    }

    if (true==bret)
    {
        // 查询人员信息
        int queryCount = 10;
        bret = readPerson(sql, queryCount);
    }

    std::cout << std::endl;
    std::cout << "本次测试完毕。" << std::endl;

    return 0;
}
