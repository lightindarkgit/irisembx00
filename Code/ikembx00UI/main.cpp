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

#include <fcntl.h>


// 获得需要装载语言文件名（*.qm）
bool getLanguageFile(QString &translateFile);
//设置应用的编解码方式
void SetAppCodec(const char* codecType);
//设置多语言支持
void SetMultiLangSupport(Application& app);
// 检查和更新数据表结构
int	CheckAndUpdateTable();

bool        g_binited;

int main(int argc, char *argv[])
{
	Exectime etm(__FUNCTION__);
	Application a(argc, argv);
	a.setStyle("macintosh");
    g_binited = false;


    IKXml::IKXmlDocument cfgFile;
    cfgFile.LoadFromFile(GetXmlFilePath());

    bool bshowcursor = false;
    std::string duration = cfgFile.GetElementValue("/IKEMBX00/configures/cursor");
    if(duration.empty())
    {
        LOG_ERROR("unable to get [arlarm]/IKEMBX00/configures/cursor] duration from configure file");
    }
    else
    {
        bshowcursor = (atoi(duration.c_str()) == 1);
        LOG_INFO("cursor status: %s,ret: %d",duration.c_str(),bshowcursor);
    }

    if(!bshowcursor)
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

	//获取设备的工作模式，是否为单机模式，注：非单机模式或联机联网模式外将
	//不允许对人员信息进行增删改操作，配置该模式的方式需要修改xml配置文件
	WorkMode::SetWorkMode(SysNetInterface::isSingalworkMode());

	//设置应用支持多语言
	SetMultiLangSupport(a);

	////////////////////////////
	SetAppCodec("UTF-8");

	if(access("./log",0) == -1)
	{
		system("mkdir ./log");
		//system("echo '123456' | sudo mkdir ./log");
	}



	// 检查更新表结构 2015-07-13 by lius
	int nret = CheckAndUpdateTable();
	if(nret != 0)
	{
		LOG_ERROR("check db.table failed");
		return -1;
	}


	KeyboardInput keyboard;
	a.setInputContext(keyboard.allocaInputContext());

	//Add by: Wang.L @2014-11-19 for anti-screenshutdown by simulating
	pthread_t pid;
	if(pthread_create(&pid, nullptr, wakescreend, nullptr) < 0)
	{
		LOG_ERROR("cannot create the wakescreend thread！");
		return system("echo 'cannot create the wakescreend thread'");
	}

	const char* pdns = "echo \"nameserver 202.106.196.105\nnameserver 8.8.8.8\" > /etc/resolv.conf";
	system(pdns);

	MainWindow w;
	w.show();

	a.SetWindowInstance(&w);

	//加载闹铃模块
	Alarm::getInstance();

	//    system("setterm -blank 0");
	//    system("vbetool dpms off");

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
	Exectime etm(__FUNCTION__);
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
	Exectime etm(__FUNCTION__);
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
		dbAccess.Query("select setval('person_id_seq',100,false)");
	}

	nret = CheckCardIDAndGen();
	if(nret != 0)
	{
		LOG_ERROR("CheckCardIDAndGen failed.");
	}

	return 0;
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
	Exectime etm(__FUNCTION__);
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

