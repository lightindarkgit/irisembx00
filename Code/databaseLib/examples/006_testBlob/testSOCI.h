/*******************************************************************
** 文件名称：testSOCI.h
** 主 要 类：
** 描    述：
**
** 版权所有：北京中科虹霸科技有限公司 (IrisKing Inc.)
** 创 建 人：yqhe
** 创建日期：2014/2/14 14:29
**
** 版    本：0.0.1
** 备    注：命名及代码编写遵守C++编码规范
*******************************************************************/
#include <iostream>
#include <sstream>
#include <iomanip>      // 控制格式输出

#include <ctime>

#include <soci.h>

// postgresql数据库中
// 虹膜数据表名称
extern const char codeTableName[];
// 人员表名称
extern const char personTableName[];

// sqlite数据库中
// 虹膜数据表名称
extern const char sqliteCodeTableName[];
// 人员表名称
extern const char sqlitePersonTableName[];

// 字段定义
// 虹膜ID
extern const char didField[];
// 虹膜特征
extern const char irisCodeField[];
// 注册设备序列号
extern const char devSnField[];
// 注册时间
extern const char regTimeField[];

// 人员ID
extern const char pidField[];
// 人员名称
extern const char nameField[];
// 人员工号
extern const char workSnField[];


// 数据结构：Person
// 定义人员信息的数据结构
struct Person
{
    int id;
    std::string workSn;
    std::string name;

    Person ()
    {
        id = 0;
    };
};

#define irisCodeLen     1024
// 数据结构：IrisData
struct IrisData
{
    int id;
    std::string icode;
    int pid;
    std::string devSn;
    std::tm     regTime;

    IrisData()
    {
        id = 0;
        pid= 0;
    };
};

// 绑定Person结构与数据库数据
namespace soci
{
    static std::tm defaultTime;

    // Person结构与数据库字段绑定
    template <> struct type_conversion<Person>
    {
        typedef values base_type;
        static void from_base (values const &v, indicator ind, Person &p)
        {
            p.id        = v.get<int>(pidField, 0);
            p.name      = v.get<std::string>(nameField, "");
            p.workSn    = v.get<std::string>(workSnField, "");
            // p.createTime= v.get<std::tm>(timeField, defaultTime);
        } // end of from base

        static void to_base(const Person &p, values &v, indicator &ind)
        {
            v.set(pidField, p.id);
            v.set(nameField, p.name);
            v.set(workSnField, p.workSn, p.workSn.empty() ? i_null : i_ok);
            // v.set(timeField, p.createTime);

            ind = i_ok;
        } // end of to_base

    }; // end of template type_conversion

    // IrisData结构与数据库字段绑定
    template <> struct type_conversion<IrisData>
    {
        typedef values base_type;
        static void from_base (values const &v, indicator ind, IrisData &d)
        {
            d.id        = v.get<int>(didField, 0);
            d.icode     = v.get<std::string>(irisCodeField, "");
            d.pid       = v.get<int>(pidField, 0);
            d.devSn     = v.get<std::string>(devSnField, "");
            d.regTime   = v.get<std::tm>(regTimeField, defaultTime);
       } // end of from base

        static void to_base(const IrisData &d, values &v, indicator &ind)
        {
            v.set(didField, d.id);
            v.set(irisCodeField, d.icode);
            v.set(pidField, d.pid);
            v.set(devSnField, d.devSn);
            v.set(regTimeField, d.regTime);

            ind = i_ok;
        } // end of to_base

    }; // end of template type_conversion

}
