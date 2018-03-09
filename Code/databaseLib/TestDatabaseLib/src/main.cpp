/*******************************************************************
** 文件名称：main.cpp
** 主 要 类：
** 描    述：测试IKDatabaseLib提供的功能
**           
** 版权所有：北京中科虹霸科技有限公司 (IrisKing Inc.)
** 创 建 人：yqhe
** 创建日期：2014/2/19 8:50:51
**           
** 版    本：0.0.1
** 备    注：命名及代码编写遵守C++编码规范
*******************************************************************/
#include <iostream>
#include <sstream>

#include <iomanip>      // 控制格式输出

#include "IKDatabaseLib.h"
#include "DataAccessType.h"

using namespace std;
using namespace soci;


const char pidField[]     = "person_id";
const char nameField[]    = "name";
const char workSnField[]  = "work_sn";
const char timeField[]    = "create_time";

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
*           主程序
* 版权所有：北京中科虹霸科技有限公司 (IrisKing Inc.)
* 函数名称：main
* 功    能：主程序
* 说    明：
* 参    数：
* 返 回 值：无
* 创 建 人：yqhe
* 创建日期：2014/2/19 8:54:26
*******************************************************************/
int main( void )
{
    DatabaseAccess dbAccess;
    std::string queryString;

    // 设置编码，Windows下设置为GBK
    // Linux下用缺省的UNICODE
    queryString = "set client_encoding to 'GBK';";
    dbAccess.Query(queryString);

    // 查询记录数量，一个返回值
    queryString = "SELECT count(*) from person_base;";
    int count = 0;

    dbAccess.Query(queryString, count);

    // 显示查询结果
    std::cout << "query person count=" << count << std::endl;

    // 查询多条记录
    queryString= "SELECT person_id, name, work_sn, create_time FROM person_base ORDER BY name LIMIT 10 ;";

    std::vector<Person> vecPerson;

    dbAccess.Query(queryString, vecPerson);

    std::cout << "查询到" << vecPerson.size() << "条记录。" << std::endl;

    // 显示查询结果
    std::vector<Person>::const_iterator it;
    for (it=vecPerson.begin(); it!=vecPerson.end(); ++it)
    {
        const Person &person = *it;
        std::cout << "id = " << setw(4) << person.id
            << ", name = " << setw(8) << person.name
            << ", workSn = " << setw(8) << person.workSn
            << ", createTime = " << outputTime(person.createTime)
            << std::endl;
    }

    // 测试插入
    // 先删除以前插入的数据
    std::cout << std::endl;
    std::cout << "删除之前插入的数据……" << std::endl;
    queryString = "DELETE FROM person_base WHERE work_sn='Test001的工号';";
    dbAccess.Query(queryString);

    std::cout << std::endl;
    std::cout << "测试插入一条数据……" << std::endl;
    // 因为person_id和create_time字段是数据库自动生成的，所以这里不需要写入
    queryString = "INSERT INTO person_base (name, work_sn) VALUES (:name, :work_sn);";

    //  模拟一条人员数据
    vecPerson.clear();
    Person person;
    person.name = "Test001";
    person.workSn = "Test001的工号";
    vecPerson.push_back(person);
    dbAccess.Insert(queryString, vecPerson);

    // 查询刚写入的记录
    std::cout << std::endl;
    std::cout << "查询刚写入的数据……" << std::endl;

    vecPerson.clear();

    queryString= "SELECT person_id, name, work_sn, create_time FROM person_base WHERE work_sn='Test001的工号' ;";
    dbAccess.Query(queryString, vecPerson);

    std::cout << "查询到" << vecPerson.size() << "条记录。" << std::endl;

    // 显示查询结果
    for (it=vecPerson.begin(); it!=vecPerson.end(); ++it)
    {
        const Person &person = *it;
        std::cout << "id = " << setw(4) << person.id
            << ", name = " << setw(8) << person.name
            << ", workSn = " << setw(8) << person.workSn
            << ", createTime = " << outputTime(person.createTime)
            << std::endl;
    }

    std::cout << std::endl;
    std::cout << "按回车键退出本次测试..." << std::endl;
    std::cin.peek();

    return 0;
}