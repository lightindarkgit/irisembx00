/*******************************************************************
** 文件名称：testSOCI.h
** 主 要 类：
** 描    述：
**
** 版权所有：北京中科虹霸科技有限公司 (IrisKing Inc.)
** 创 建 人：yqhe
** 创建日期：2014/2/12 15:45:41
**
** 版    本：0.0.1
** 备    注：命名及代码编写遵守C++编码规范
*******************************************************************/
#include <iostream>
#include <sstream>
#include <iomanip>      // 控制格式输出

#include <ctime>

#include <soci.h>

extern const char tableName[];
extern const char nameField[];
extern const char idField[];
extern const char workSnField[];
extern const char timeField[];


// 数据结构：Person
// 定义人员信息的数据结构
struct Person
{
    int id;
    std::string workSn;
    std::string name;
    std::tm createTime;

    Person ()
    {

    };
};



// 绑定Person结构与数据库数据
namespace soci
{
    static std::tm defaultTime;

    template <> struct type_conversion<Person>
    {
        typedef values base_type;
        static void from_base (values const &v, indicator ind, Person &p)
        {
            p.id        = v.get<int>(idField, 0);
            p.name      = v.get<std::string>(nameField, "");
            p.workSn    = v.get<std::string>(workSnField, "");
            p.createTime= v.get<std::tm>(timeField, defaultTime);
        } // end of from base

        static void to_base(const Person &p, values &v, indicator &ind)
        {
            v.set(idField, p.id);
            v.set(nameField, p.name);
            v.set(workSnField, p.workSn, p.workSn.empty() ? i_null : i_ok);
            v.set(timeField, p.createTime);

            ind = i_ok;
        } // end of to_base

    }; // end of template type_conversion

}
