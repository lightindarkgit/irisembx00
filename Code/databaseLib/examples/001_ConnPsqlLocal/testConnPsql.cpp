/*******************************************************************
** 文件名称：testConnPsql.cpp
** 主 要 类：
** 描    述：提供以SOCI库，连接postgresql数据库的简单示例
**           连接本地数据库
**
** 版权所有：北京中科虹霸科技有限公司 (IrisKing Inc.)
** 创 建 人：yqhe
** 创建日期：2014/1/12 16:15
**
** 版    本：1.0.0
** 备    注：命名及代码编写遵守C++编码规范
*******************************************************************/
// 用于输出信息
#include <iostream>

// SOCI相关头文件
#include <soci.h>
#include <postgresql/soci-postgresql.h>

using namespace soci;

/*******************************************************************
*           主函数
* 版权所有：北京中科虹霸科技有限公司 (IrisKing Inc.)
* 函数名称：main
* 功    能：主函数
* 说    明：演示连接本地postgresql数据库mydb
*           postgresql用户名为postgres，密码为123456
*           如果以上条件具备，则可与数据成功连接
* 参    数：
* 返 回 值：
* 创 建 人：yqhe
* 创建日期：2014/1/25 16:40
*******************************************************************/
int main(int argc, char **argv)
{
    try
    {
        // 打开本地postgresql的mydb数据库，用户名为postgres，密码为123456
        std::string databaseName = "mydb";
        std::string userName = "postgres";
        std::string userPasswd = "123456";

        soci::session sql(soci::postgresql, "dbname="+databaseName+" user=" +userName+ " password=" + userPasswd);

        std::cout << "连接本地数据库" << databaseName << "成功！" << std::endl;
    }
    catch (soci::postgresql_soci_error const &e)
    {
        // postgresql特有的错误信息
        std::cerr << "PostgreSQL error:"
                  << e.sqlstate()          // sqlstat()为postgresql独有，长度为5的字符串
                  << " "
                  << e.what()
                  << std::endl;
    }
    catch (std::exception const &e)
    {
        // 其他错误信息
        std::cerr << "Some other error:" << e.what() << std::endl;
    }

    return 0;
}
