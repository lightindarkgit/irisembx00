/*******************************************************************
** 文件名称：DataAccessType.h
** 主 要 类：
** 描    述：访问数据库的数据结构
**
** 版权所有：北京中科虹霸科技有限公司 (IrisKing Inc.)
** 创 建 人：yqhe
** 创建日期：2014/2/20 8:29:30
**
** 版    本：2.6.85
** 备    注：命名及代码编写遵守C++编码规范
*******************************************************************/
#include <soci.h>
#include "IKDatabaseWrapper.h"

extern const char nameField[];
extern const char pidField[];
extern const char workSnField[];
extern const char timeField[];
extern const char departField[];

extern const char irisIdField [] ;
extern const char irisCodeField[];
extern const char irisImageField[];
extern const char devSnField[];

extern const char operatorIdField[];
extern const char passwdField[];
extern const char sexField[];
extern const char memoField[];

// 数据结构：Person
// 定义人员信息的数据结构
struct Person
{
    IkUuid      id;
    std::string workSn;
    std::string name;
    std::string depart;
    // std::tm createTime;

    Person ()
    {
    };
};

// 绑定Person结构与数据库数据
namespace soci
{
//    static std::tm defaultTime;

    template <> struct type_conversion<Person>
    {
        typedef values base_type;
        static void from_base (values const &v, indicator ind, Person &p)
        {
            // p.id        = v.get<int>(pidField, 0);
            p.id        = v.get<std::string>(pidField, "");
            p.name      = v.get<std::string>(nameField, "");
            p.depart	= v.get<std::string>(departField, "");
            // p.workSn    = v.get<std::string>(workSnField, "");
            //p.createTime= v.get<std::tm>(timeField, defaultTime);
        } // end of from base

        static void to_base(const Person &p, values &v, indicator &ind)
        {
            v.set(pidField, p.id);
            v.set(nameField, p.name);
            v.set(workSnField, p.workSn, p.workSn.empty() ? i_null : i_ok);
            v.set(departField, p.depart, p.depart.empty() ? i_null : i_ok);
            //v.set(timeField, p.createTime);

            ind = i_ok;
        } // end of to_base

    }; // end of template type_conversion

}


// 数据结构：虹膜特征
struct IrisDataDemo
{
    IkUuid          did;
    IkUuid          pid;
    IkByteaIrisCode irisCode;
    IkByteaLarge    irisImage;
    std::string     devSn;
};

// 绑定IrisDataDemo结构与数据库数据
namespace soci
{
//    static std::tm defaultTime;

    template <> struct type_conversion<IrisDataDemo>
    {
        typedef values base_type;
        static void from_base (values const &v, indicator ind, IrisDataDemo &d)
        {
            if (i_null == ind)
            {
                return;
            }

            // p.id        = v.get<int>(pidField, 0);
            d.did       = v.get<std::string>(irisIdField, "");
            d.pid       = v.get<std::string>(pidField, "");
            d.irisCode  = v.get<std::string>(irisCodeField, "");
            d.irisImage = v.get<std::string>(irisImageField, "");
            // d.devSn     = v.get<std::string>(devSnField, "");

            //p.workSn    = v.get<std::string>(workSnField, "");
            //p.createTime= v.get<std::tm>(timeField, defaultTime);
        } // end of from base

        static void to_base(const IrisDataDemo &d, values &v, indicator &ind)
        {
            v.set(irisIdField, d.did);
            v.set(pidField, d.pid);
            v.set(irisCodeField, d.irisCode, d.irisCode.Empty() ? i_null : i_ok);
            v.set(irisImageField, d.irisImage, d.irisImage.Empty() ? i_null : i_ok);
            v.set(devSnField, d.devSn);
            // v.set(workSnField, p.workSn, p.workSn.empty() ? i_null : i_ok);
            //v.set(timeField, p.createTime);

            ind = i_ok;
        } // end of to_base

    }; // end of template type_conversion

}

// 定义操作员数据结构
struct Operator
{
    int         oid;
    std::string name;
    std::string password;
    IkUuid      pid;
    std::string sex;
    std::string memo;
};

// 绑定Operator结构与数据库数据
namespace soci
{
//    static std::tm defaultTime;

    template <> struct type_conversion<Operator>
    {
        typedef values base_type;
        static void from_base (values const &v, indicator ind, Operator &o)
        {
            if (i_null == ind)
            {
                return;
            }

            // p.id        = v.get<int>(pidField, 0);
            o.oid       = v.get<int>(operatorIdField, 0);
            o.pid       = v.get<std::string>(pidField, "");
            o.name      = v.get<std::string>(nameField, "");
            o.password  = v.get<std::string>(passwdField, "");
            o.sex       = v.get<std::string>(sexField, "");
            o.memo      = v.get<std::string>(memoField, "");
            // d.devSn     = v.get<std::string>(devSnField, "");

            //p.workSn    = v.get<std::string>(workSnField, "");
            //p.createTime= v.get<std::tm>(timeField, defaultTime);
        } // end of from base

        static void to_base(const Operator &o, values &v, indicator &ind)
        {
            v.set(operatorIdField, o.oid);
            v.set(pidField, o.pid);
            v.set(nameField, o.name, o.name.empty() ? i_null : i_ok);
            v.set(passwdField, o.password, o.password.empty() ? i_null : i_ok);
            v.set(sexField, o.sex, o.sex.empty() ? i_null : i_ok);
            v.set(memoField, o.memo, o.sex.empty() ? i_null : i_ok);
            // v.set(workSnField, p.workSn, p.workSn.empty() ? i_null : i_ok);
            //v.set(timeField, p.createTime);

            ind = i_ok;
        } // end of to_base

    }; // end of template type_conversion

}

// 数据结构：虹膜特征
struct IrisDataTrans
{
    IkUuid          did;
    IkUuid          pid;
    std::string     irisCode;		// 用于postgresql数据库与sqlite之间的导入导出
    std::string     irisImage;
    std::string     devSn;
};

// 绑定IrisDataTrans结构与数据库数据
namespace soci
{
    template <> struct type_conversion<IrisDataTrans>
    {
        typedef values base_type;
        static void from_base (values const &v, indicator ind, IrisDataTrans &d)
        {
            if (i_null == ind)
            {
                return;
            }

            // p.id        = v.get<int>(pidField, 0);
            d.did       = v.get<std::string>(irisIdField, "");
            d.pid       = v.get<std::string>(pidField, "");
            d.irisCode  = v.get<std::string>(irisCodeField, "");
            d.irisImage = v.get<std::string>(irisImageField, "");
            // d.devSn     = v.get<std::string>(devSnField, "");

            //p.workSn    = v.get<std::string>(workSnField, "");
            //p.createTime= v.get<std::tm>(timeField, defaultTime);
        } // end of from base

        static void to_base(const IrisDataTrans &d, values &v, indicator &ind)
        {
            v.set(irisIdField, d.did);
            v.set(pidField, d.pid);
            v.set(irisCodeField, d.irisCode, d.irisCode.empty() ? i_null : i_ok);
            v.set(irisImageField, d.irisImage, d.irisImage.empty() ? i_null : i_ok);
            v.set(devSnField, d.devSn);
            // v.set(workSnField, p.workSn, p.workSn.empty() ? i_null : i_ok);
            //v.set(timeField, p.createTime);

            ind = i_ok;
        } // end of to_base

    }; // end of template type_conversion

}
