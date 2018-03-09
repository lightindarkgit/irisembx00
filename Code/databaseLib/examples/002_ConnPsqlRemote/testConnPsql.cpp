/*******************************************************************
** 文件名称：testConnPsql.cpp
** 主 要 类：
** 描    述：提供以SOCI库，连接postgresql数据库的简单示例
**           连接远端数据库
**           10.2.1.117
**
** 版权所有：北京中科虹霸科技有限公司 (IrisKing Inc.)
** 创 建 人：yqhe
** 创建日期：2014/1/12 17:30
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
* 说    明：演示连接10.2.1.117上postgresql数据库irisApp.wuhushan
*           postgresql用户名为postgres，密码为123456
*           如果以上条件具备，则可与数据成功连接
* 参    数：
* 返 回 值：
* 创 建 人：yqhe
* 创建日期：2014/1/25 17:35
*******************************************************************/
int main(int argc, char **argv)
{
    try
    {
        // 打开远端10.2.1.117上
        // postgresql的irisApp.wuhushan数据库，用户名为postgres，密码为123456
        std::string remoteHost = "10.2.1.117";
        std::string databaseName = "irisApp.wuhushan";
        std::string userName = "postgres";
        std::string userPasswd = "123456";

        soci::session sql(soci::postgresql, "host=" +remoteHost+ \
                          " dbname="+databaseName+ \
                          " user=" +userName+ \
                          " password=" + userPasswd);

        std::cout << "连接远端" << remoteHost << "数据库" << databaseName << "成功！" << std::endl;
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
