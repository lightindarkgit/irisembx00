/*******************************************************************
** 文件名称：main.cpp
** 主 要 类：
** 描    述：测试soci访问PostgreSQL数据库的简单功能
**           连接PostgreSQL数据库 -- connDatabase
**           取数据条数(count)    -- readPersonCount
**           取多条数据并显示     -- readPerson
**                                   注意，需要设置文字编码，否则显示的中文是乱码
**           
** 版权所有：北京中科虹霸科技有限公司 (IrisKing Inc.)
** 创 建 人：yqhe
** 创建日期：2014/2/12 16:57:06
**           
** 版    本：0.0.1
** 备    注：命名及代码编写遵守C++编码规范
*******************************************************************/
#include "testSOCI.h"
#include <postgresql/soci-postgresql.h>
 
using namespace std;
using namespace soci;

const char tableName[] = "person_base";
const char nameField[] = "name";
const char idField[]   = "person_id";
const char workSnField[]="work_sn";
const char timeField[] = "create_time";

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
    // 测试时间格式是否正确，如不正确，就不进行转化
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

    // 连接远程数据库的连接参数
    connParam = string("host=10.2.1.117 dbname=irisApp.wuhushan user=postgres password=123456");

    // 连接本地数据源的连接参数
    // connParam = string("DSN=tfdq;UID=sa;PWD=123456");

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
        cerr << "Some other error: " << e.what() << endl;
    }

    return false;
}

/*******************************************************************
*           从打开的数据库连接中读取有多少个人员信息
* 版权所有：北京中科虹霸科技有限公司 (IrisKing Inc.)
* 函数名称：readPersonCount
* 功    能：从打开的数据库连接中读取有多少个人员信息
* 说    明：
* 参    数：
* 返 回 值：无
*           输入参数
*           sql —— 与数据库的连接 
*           输出参数
*           personCount —— 查到的人员个数
* 创 建 人：yqhe
* 创建日期：2014/2/12 15:43:42
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

        // 设置中文编码，在windows环境下，8.4需要设置编码GBK
        // 在Linux环境下，9.1需要设置编码UNICODE，数据库缺省client_encoding是UNICODE
        // 如果设置不正确，则中文会显示乱码
        // queryString = string("set client_encoding to 'UNICODE';");
        // sql << queryString;

        std::ostringstream oss;

        oss << "SELECT "
            << " "
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
            << nameField
            << " limit "
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
*           主程序
* 版权所有：北京中科虹霸科技有限公司 (IrisKing Inc.)
* 函数名称：main
* 功    能：主程序
* 说    明：
* 参    数：
* 返 回 值：无
* 创 建 人：yqhe
* 创建日期：2014/2/12 15:43:11
*******************************************************************/
int main( void )
{
    session sql;
    bool bret = connDatabase(sql);

    std::cout << "Connect Database return : " << bret << std::endl;

    if (true == bret)
    {
        // 查询人员数量
        int personCount;
        bret = readPersonCount(sql, personCount);
    }

    if (true==bret)
    {
        // 查询人员信息
        int queryCount = 10;
        bret = readPerson(sql, queryCount);
    }

    std::cout << std::endl;
    std::cout << "按回车键退出本次测试..." << std::endl;
    std::cin.peek();

    return 0;
}
