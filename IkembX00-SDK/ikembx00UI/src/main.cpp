/*****************************************************************************
 ** 文 件 名： main.h
 ** 主 要 类： 主程序
 **
 ** Copyright (c) 中科虹霸有限公司
 **
 ** 创 建 人：刘中昌
 ** 创建时间：20013-10-24
 **
 ** 修 改 人：
 ** 修改时间：
 ** 描  述:    项目入口
 ** 主要模块说明: 项目入口
 **
 ** 版  本:   1.0.0
 ** 备  注:
 **
 *****************************************************************************/
//#include <QtGui/QApplication>
#include <QTranslator>      // 用于支持多语言环境
#include <QTranslator>
#include "mainwindow.h"
#include <QTextCodec>
#include "application.h"
#include "keyboardinput.h"
#include "../XML/xml/IKXmlApi.h"    // 多语言环境配置在XML文件中
#include "alarm.h"
#include "sysnetinterface.h"
#include "wakescreen.h"
#include "commononqt.h"
#include "../Common/Logger.h"
#include "../Common/Exectime.h"
#include "../XML/xml/IKXmlApi.h"
#include <QString>

#include <fcntl.h>
#include "ikconfig.h"


// 获得需要装载语言文件名（*.qm）
bool getLanguageFile(QString &translateFile);
//设置应用的编解码方式
void SetAppCodec(const char* codecType);
//设置多语言支持
void SetMultiLangSupport(Application& app);
// 检查和更新数据表结构
int	CheckAndUpdateTable();
//检查和更新表中的人员id长度和姓名长度
void CheckAndUpdatePersonNameAndCardLen();

int AddDbTrigger(DatabaseAccess &dbAccess);

bool        g_binited;

extern std::string devName;
std::string devName;

int main(int argc, char *argv[])
{
    Exectime etm(__FUNCTION__);
    //caotao, 20170510 设置cpu调频策略
    cpuFreqSet("ondemand");
    Application a(argc, argv);
    a.setStyle("macintosh");
    g_binited = false;



    int showMouse = IkConfig::GetMouseCfg();

    if(showMouse == 0)
    {
        //Hide the cursor
        QApplication::setOverrideCursor(Qt::BlankCursor);
        LOG_INFO("Hide Cursor....");
    }
    else
    {
        QApplication::setOverrideCursor(Qt::ArrowCursor);
        LOG_INFO("Show Cursor....");
    }


    std::cout << "sys:font: " << a.font().family().toStdString() << std::endl;

    //获取设备的工作模式，是否为单机模式
    WorkMode::SetWorkMode(SysNetInterface::isSingalworkMode());

    //设置应用支持多语言
    SetMultiLangSupport(a);

    ////////////////////////////
    SetAppCodec("UTF-8");

    if(access("./log",0) == -1)
    {
        system("mkdir ./log");
    }

    // 检查更新表结构 2015-07-13 by lius
    int nret = CheckAndUpdateTable();
    if(nret != 0)
    {
        LOG_ERROR("check db.table failed");
        return -1;
    }

    KeyboardInput keyboard;
   // keyboard.allocaInputContext()->s
    a.setInputContext(keyboard.allocaInputContext());


    //dj
    QFile StyleSheet(":/ikembx00.qss");
    if(!StyleSheet.open(QIODevice::ReadOnly))
    {
        LOG_WARN("Can't open the style sheet file");
    }
    a.setStyleSheet(StyleSheet.readAll());

    const char* pdns = "echo \"nameserver 202.106.196.105\nnameserver 8.8.8.8\" > /etc/resolv.conf";
    system(pdns);

    MainWindow w;
    w.show();

    a.SetWindowInstance(&w);

    //加载闹铃模块
    Alarm::getInstance();

    return a.exec();
}


/*******************************************************************
 *         获得配置文件XML中的语言配置
 * 版权所有：北京中科虹霸科技有限公司 (IrisKing Inc.)
 * 函数名称：getLanguageConfig
 * 功    能：获得配置文件XML中的语言配置
 * 说    明：注意默认配置文件在程序所在路径下
 *          默认配置文件名为ikembx00.xml
 * 参    数：无
 * 返 回 值：如果有配置，则返回配置字符串
 *          如果没有配置，则返回'chinese'，表示缺省为中文
 * 创 建 人：yqhe
 * 创建日期：2014/4/1 17:14
 *******************************************************************/
const char x00ConfigFile[]   = "ikembx00.xml";
const char x00LanguagePath[] = "/IKEMBX00/configures/language";

std::string getLanguageConfig()
{

    IKXml::IKXmlDocument cfgDoc;

    QDir dir(QCoreApplication::applicationDirPath());
    QString cfgFile = QString::fromUtf8("%1%2%3").arg(dir.absolutePath()).arg(QDir::separator()).arg(x00ConfigFile);

    if(cfgDoc.LoadFromFile(cfgFile.toStdString()))
    {
        return cfgDoc.GetElementValue(x00LanguagePath);
    }

    return "chinese";
}

/*******************************************************************
 *         获得语言支持文件名（*.qm）
 * 版权所有：北京中科虹霸科技有限公司 (IrisKing Inc.)
 * 函数名称：getLanguageFile
 * 功    能：获得语言支持文件名（*.qm）
 * 说    明：
 * 参    数：
 *          输出参数
 *          translateFile —— 语言支持文件名
 * 返 回 值：如果是中文环境，则返回true
 * 创 建 人：yqhe
 * 创建日期：2014/4/1 17:31
 *******************************************************************/
bool getLanguageFile(QString &translateFile)
{

    std::string config = getLanguageConfig();

    QDir dir(QCoreApplication::applicationDirPath());

    if ("english"==config)
    {
        translateFile  = QString::fromUtf8("%1%2%3").arg(dir.absolutePath()).arg(QDir::separator()).arg("ikembx00_en.qm");
        LOG_ERROR("language is  englist！");
        return false;
    }
    else
    {
        translateFile = QString::fromUtf8("%1%2%3").arg(dir.absolutePath()).arg(QDir::separator()).arg("ikembx00_zh_CN.qm");
        return true;
    }
}

//设置多语言支持
/*******************************************************************
 *                    设置应用支持多语言
 * 版权所有：北京中科虹霸科技有限公司 (IrisKing Inc.)
 * 函数名称：SetMultiLangSupport
 * 功    能：将指定的编解码方式设置应用的编解码方式
 * 说    明：
 * 参    数：
 *          输入参数
 *          app —— Application对象引用
 * 返 回 值：
 * 创 建 人：Wang.L
 * 创建日期：2015/10/10
 *******************************************************************/
void SetMultiLangSupport(Application& app)
{
    Exectime etm(__FUNCTION__);

    ////////////////////////////
    // removed at 20140401 by yqhe
    // 汉化消息框按钮已写到自己的多语言支持文件中
    //    QTranslator oTranslator;
    //    oTranslator.load(":/qt_zh_CN");
    //    a.installTranslator(&oTranslator);   //汉化消息框按钮
    ////////////////////////////

    ////////////////////////////
    // added at 20140401 by yqhe
    // 多语言化IKEMBX00
    QTranslator oTranslator;

    QString loadFile;
    getLanguageFile(loadFile);      // 获得语言支持文件名(*.qm)

    std::cout << loadFile.toStdString() << std::endl;

    oTranslator.load(loadFile);
    app.installTranslator(&oTranslator);
}

// 检查和更新数据表结构
/* ********************************************************/
/**
 * @Name: CheckAndUpdateTable
 *
 * @Returns: 0 - succ, other - error
 */
/* ********************************************************/
int	CheckAndUpdateTable()
{
    Exectime etm(__FUNCTION__);

    DatabaseAccess dbAccess;
    string queryString;
    std::ostringstream oss;
    int  columncnt;

    // 查找是否存在列
    oss<<"SELECT count(1) as count  FROM INFORMATION_SCHEMA.COLUMNS WHERE table_name = 'person' AND COLUMN_NAME = 'id'";
    queryString = oss.str();
    int nret = dbAccess.Query(queryString,columncnt);
    if(nret != dbSuccess)
    {
        LOG_ERROR("query sql failed. %s",queryString.c_str());
        return -1;
    }

    if(columncnt == 0)
    {
        oss.str("");
        //        oss<<"create sequence id cycle;";
        //        oss<<"grant all on id to public;";
        //        oss<<"alter table   person  add  id  integer  not null NOT NULL DEFAULT nextval('id') ";

        //guodj 添加自增id
        oss<<"ALTER TABLE person ADD COLUMN id serial;";
        queryString = oss.str();
        nret = dbAccess.Query(queryString);
        if(nret != dbSuccess)
        {
            LOG_ERROR("query sql failed. %s",queryString.c_str());
            return -1;
        }
        nret = dbAccess.Query("select setval('person_id_seq',100,false)");
        if(nret != dbSuccess)
        {
            LOG_ERROR("Query sql failed. [select setval('person_id_seq',100,false)]");
            return -1;
        }
    }

    nret = CheckCardIDAndGen();
    if(nret != 0)
    {
        LOG_ERROR("CheckCardIDAndGen failed.");
    }


    AddDbTrigger(dbAccess);


    return 0;
}
void CheckAndUpdatePersonNameAndCardLen()
{

    Exectime etm(__FUNCTION__);

    std::ostringstream oss;
    DatabaseAccess dbAccess;

    oss<< " DROP VIEW v_personrec;"
       << " DROP VIEW v_person_detail;"
       << " DROP VIEW v_person;"
       << " DROP VIEW v_irisdata;"
       << " DROP VIEW v_iirsdisable;"
       << " ALTER TABLE person ALTER idcard TYPE character varying(20);"
       << " ALTER TABLE person ALTER name TYPE character varying(256);"

          //-- DROP VIEW v_iirsdisable;
       << " CREATE OR REPLACE VIEW v_iirsdisable AS "
       << " SELECT person.personid AS person_personid, person.name, person.sex, person.cardid, person.idcard, person.memo, person.departname, person.disabletime, persondisableinfo.disableid, persondisableinfo.starttime, persondisableinfo.endtime"
       << " FROM person, persondisableinfo"
       << " WHERE person.personid::text = persondisableinfo.personid::text;"

       << " ALTER TABLE v_iirsdisable"
       << " OWNER TO postgres;"
       << " COMMENT ON VIEW v_iirsdisable"
       << " IS '关于人员的虹膜启禁的数据连接';"

          /*-- DROP VIEW v_irisdata;*/

       << " CREATE OR REPLACE VIEW v_irisdata AS "
       << " SELECT person.personid AS person_personid, person.name, person.superid, person.sex, person.cardid, person.worksn, person.idcard, person.departid, person.departname, person.disabletime, irisdatabase.irisdataid, irisdatabase.iriscode, irisdatabase.devsn, irisdatabase.eyeflag, irisdatabase.regtime, irisdatabase.regpalce"
       << " FROM person, irisdatabase"
       << " WHERE person.personid::text = irisdatabase.personid::text;"

       << " ALTER TABLE v_irisdata"
       << " OWNER TO postgres;"
       << " COMMENT ON VIEW v_irisdata"
       << "  IS '人员虹膜数据';"
          //-- DROP VIEW v_person;

       << " CREATE OR REPLACE VIEW v_person AS "
       << " SELECT personimage.imageid, person.personid, person.name, person.sex, person.cardid, person.idcard, person.memo AS person_memo, person.departname, person.disabletime, personimage.personimage, personimage.memo AS personimage_memo"
       << " FROM personimage, person"
       << " WHERE personimage.personid::text = person.personid::text;"

       << " ALTER TABLE v_person"
       << " OWNER TO postgres;"
       << " COMMENT ON VIEW v_person"
       << " IS '人员的视图';"


          //-- DROP VIEW v_person_detail;
          //比較老，需要修改該

       << " CREATE OR REPLACE VIEW v_person_detail AS "
       << " SELECT tmp.personid, tmp.name, tmp.sex, tmp.cardid, tmp.idcard, tmp.memo, tmp.departid, tmp.departname, tmp.disabletime, tmp.worksn, tmp.superid,"
       << " CASE"
       << " WHEN tmp.irisdatacount > 0 THEN '是'::text"
       << " ELSE '否'::text"
       << " END AS hasirisdata"
       << " FROM ( SELECT p.personid, p.name, p.worksn, p.superid, p.sex, p.cardid, p.idcard, p.memo, p.departid, p.departname, p.disabletime, p.hasirisdata, t.irisdatacount"
       << " FROM person p"
       << " LEFT JOIN ( SELECT irisdatabase.personid, count(*)::integer AS irisdatacount"
       << " FROM irisdatabase"
       << " GROUP BY irisdatabase.personid) t ON p.personid::text = t.personid::text) tmp;"

       << " ALTER TABLE v_person_detail"
       << " OWNER TO postgres;"
       << " COMMENT ON VIEW v_person_detail"
       << " IS '人员的详细视图';"

       << " CREATE OR REPLACE VIEW v_personrec AS "
       << " SELECT person.personid AS person_personid, person.name, person.sex, person.cardid, person.idcard, person.memo AS person_memo, person.departname, person.disabletime, personreclog.reclogid, personreclog.irisdataid, personreclog.recogtime, personreclog.recogtype, personreclog.atpostion, personreclog.devsn, personreclog.devtype, personreclog.deltime, personreclog.memo AS personreclog_memo"
       << " FROM person, personreclog"
       << " WHERE person.personid::text = personreclog.personid::text;"

       << " ALTER TABLE v_personrec"
       << " OWNER TO postgres;"
       << " COMMENT ON VIEW v_personrec"
       << " IS '人员识别记录的详细信息';";



    int nret = dbAccess.Query(oss.str());
    if(nret != dbSuccess)
    {
        LOG_ERROR("Query sql failed. CheckAndUpdatePersonNameAndCardLen ");
        return ;
    }


}

          /*******************************************************************
       *                    设置应用的编解码方式
       * 版权所有：北京中科虹霸科技有限公司 (IrisKing Inc.)
       * 函数名称：AddDbTrigger
       * 功    能： 为x00数据库表添加trigger，以解决人员过大时数据库读写缓慢的问题
       * 说    明：
       * 参    数：
       *          输入参数
       *          dbAccess —— 虹膜数据库
       * 返 回 值：
       * 创 建 人：guodj
       * 创建日期：2017/1/3
       *******************************************************************/
          int AddDbTrigger(DatabaseAccess &dbAccess)
    {
          //增加了trigger 当数据库人员过多时，刷新和显示造成软件反应迟缓，
          //为了解决该问题，在数据库中增加了信号等

          QString sqlTrigger = "SELECT count(*) FROM pg_proc WHERE proname='change_iris_trigger';";
    int count = 0;
    int nret = dbAccess.Query(sqlTrigger.toStdString(),count);
    if(nret != dbSuccess)
    {
        LOG_ERROR("query sql failed. %s",sqlTrigger.toStdString().c_str());
        return -1;
    }
    else
    {
        if(count <= 0)
        {
            //1 创建触发函数
            std::ostringstream sql1;
            sql1 << " CREATE FUNCTION change_iris_trigger() "
                 << " RETURNS trigger AS $$ "
                 << " BEGIN "
                 << " IF TG_OP = 'INSERT' THEN "
                 << " UPDATE person SET hasirisdata=1 WHERE personid=NEW.personid;"
                 << " RETURN NEW;"
                 << " ELSIF TG_OP = 'DELETE'  THEN "
                 << " UPDATE person SET hasirisdata=0 WHERE personid=OLD.personid;"
                 << " RETURN OLD;"// --返回值要与ELSIF平齐,因为先插入后最好才执行返回"
                 << " END IF;"
                 << " END;"
                 << " $$ LANGUAGE 'plpgsql' VOLATILE;";
            nret = dbAccess.Query(sql1.str());
            if(nret != dbSuccess)
            {
                LOG_ERROR("Query sql failed. CREATE FUNCTION change_iris_trigger() ");
                return -1;
            }

            // 2、创建触发器
            std::ostringstream sql2;
            sql2 << " CREATE TRIGGER \"logging_iris_change\""
                 << " AFTER INSERT OR DELETE"
                 << " ON irisdatabase"
                 << " FOR EACH ROW"
                 << " EXECUTE PROCEDURE change_iris_trigger();";

            nret = dbAccess.Query(sql2.str());
            if(nret != dbSuccess)
            {
                LOG_ERROR("Query sql failed. CREATE TRIGGER \"logging_iris_change\" ");
                return -1;
            }

            //3、删除原有v_person_detail视图
            std::ostringstream sql3;
            sql3 << "DROP VIEW v_person_detail;";
            nret = dbAccess.Query(sql3.str());
            if(nret != dbSuccess)
            {
                LOG_ERROR("Query sql failed. DROP VIEW v_person_detail; ");
                return -1;
            }

            // 4、创建v_person_detail视图
            std::ostringstream sql4;
            sql4    << " CREATE OR REPLACE VIEW v_person_detail AS"
                    << " SELECT p.personid, p.name, p.sex, p.cardid, p.idcard, p.memo,"
                    << " p.departid, p.departname, p.disabletime, p.worksn, p.superid, "
                    << " CASE"
                    << "     WHEN  p.hasirisdata=1 THEN '是'::text"
                    << "     ELSE '否'::text"
                    << " END AS hasirisdata"
                    << " FROM person p;"
                    << " ALTER TABLE v_person_detail"
                    << " OWNER TO postgres;";
            nret = dbAccess.Query(sql4.str());
            if(nret != dbSuccess)
            {
                LOG_ERROR("Query sql failed. CREATE OR REPLACE VIEW v_person_detail AS ");
                return -1;
            }

            //-- 5、person表中hasirisdata字段更新为0
            //--    当现有人员数据较多时，可能费时较长
            //--    例如7万人数据，更新时间为5893ms
            std::ostringstream sql5;
            sql5 << "update person set hasirisdata=0;";
            nret = dbAccess.Query(sql5.str());
            if(nret != dbSuccess)
            {
                LOG_ERROR("Query sql failed. update person set hasirisdata=0;");
                return -1;
            }

            // 6、person表中有虹膜的人员的hasirisdata字段更新为1
            std::ostringstream sql6;
            sql6 << "update person set hasirisdata=1 WHERE personid in (select personid from irisdatabase group by personid);";
            nret = dbAccess.Query(sql6.str());
            if(nret != dbSuccess)
            {
                LOG_ERROR("Query sql failed. pdate person set hasirisdata=1 WHERE personid in (select personid from irisdatabase group by personid);");
                return -1;
            }
        }
    }

}


/*******************************************************************
 *                    设置应用的编解码方式
 * 版权所有：北京中科虹霸科技有限公司 (IrisKing Inc.)
 * 函数名称：SetAppCodec
 * 功    能：将指定的编解码方式设置应用的编解码方式
 * 说    明：
 * 参    数：
 *          输入参数
 *          codecType —— 编解码方式名称
 * 返 回 值：
 * 创 建 人：Wang.L
 * 创建日期：2015/10/10
 *******************************************************************/
void SetAppCodec(const char* codecType)
{

    if(nullptr == codecType)
    {
        LOG_ERROR("codecType is  null");
        return;
    }

    QTextCodec* codec = QTextCodec::codecForName(codecType);

    QTextCodec::setCodecForTr(codec);
    QTextCodec::setCodecForLocale(codec);
    QTextCodec::setCodecForCStrings(codec);
}

