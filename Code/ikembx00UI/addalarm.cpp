/******************************************************************************************
 ** 文件名:   addalarm.cpp
 ×× 主要类:   (无)
 **
 ** Copyright (c) 中科虹霸有限公司
 ** 创建人:   Wang.L
 ** 日  期:   2015-01-28
 ** 修改人:
 ** 日  期:
 ** 描  述:   闹铃编辑模块定义
 ** ×× 主要模块说明：
 **
 ** 版  本:   1.0.0
 ** 备  注:
 **
 ******************************************************************************************/
#include "addalarm.h"
#include "ui_addalarm.h"
#include <iostream>
#include <QDir>
#include <ikmessagebox.h>
#include <alarm.h>
#include "commononqt.h"
#include "../Common/Exectime.h"


//static QString GetXmlFilePath()
//{
//    QDir appDir(QCoreApplication::applicationDirPath());
//    QString xmlFile("%1%2%3");
//    xmlFile = xmlFile.arg(appDir.absolutePath(), QDir::separator(), "ikembx00.xml");

//    if(QFile::exists(xmlFile))
//    {
//        return xmlFile;
//    }

//    return  QString("");
//}


AddAlarm::AddAlarm(QWidget *parent) :
	QDialog(parent),
	ui(new Ui::AddAlarm)
{
	Exectime etm(__FUNCTION__);
	ui->setupUi(this);
	init();

	//去除对话框标题栏，坐标重定位和填充背景
	setWindowFlags(Qt::FramelessWindowHint);
	setGeometry(QRect(0, 0, 1024, 600));
}

AddAlarm::AddAlarm(const QString &alarmer, const QString& curAlarmTag, QWidget *parent) :
	QDialog(parent),
	ui(new Ui::AddAlarm),
	alarmTag(curAlarmTag)
{
	Exectime etm(__FUNCTION__);
	//TO DO
	ui->setupUi(this);
	init(alarmer);

	//去除对话框标题栏，坐标重定位和填充背景
	setWindowFlags(Qt::FramelessWindowHint);
	setGeometry(QRect(0, 0, 1024, 600));
}

AddAlarm::~AddAlarm()
{
	Exectime etm(__FUNCTION__);
	//更新闹铃时间
	Alarm::getInstance()->updateAlarms();

	delete ui;
}


void AddAlarm::init()
{
	Exectime etm(__FUNCTION__);
	QPixmap timePix(":/image/alarmclock.ico");
	ui->labTimeIcon->setPixmap(timePix.scaled(ui->labTimeIcon->size(), Qt::KeepAspectRatio));

	connect(ui->btnBack, SIGNAL(clicked()), this, SLOT(slotBack()));
	connect(ui->btnSave, SIGNAL(clicked()), this, SLOT(slotSave()));
	connect(ui->btnDelete, SIGNAL(clicked()), this, SLOT(slotDelete()));

	connect(ui->btnAdd, SIGNAL(clicked()), this, SLOT(slotAdd()));
	connect(ui->btnMinus, SIGNAL(clicked()), this, SLOT(slotMinus()));
}

void AddAlarm::init(const QString& time)
{
	Exectime etm(__FUNCTION__);
	init();
	ui->alarmTime->setTime(QTime::fromString(time, QString("HH:mm:ss")));
}

bool AddAlarm::slotBack()
{
	Exectime etm(__FUNCTION__);
	return close();
}

bool AddAlarm::slotDelete()
{
	Exectime etm(__FUNCTION__);
	//TO DO
	IKMessageBox warnNote(this, QString("删除闹铃"), QString("是否删除？"), questionMbx);

	warnNote.exec();
	if(QDialog::Rejected == warnNote.result())
	{
		close();
		return true;
	}

	IKXml::IKXmlDocument sysCfg;

	if(sysCfg.LoadFromFile(GetXmlFilePath()))
	{
		bool rtnVal = sysCfg.SetElementValue(QString("/IKEMBX00/configures/alarm/%1").arg(alarmTag).toStdString(), "");
		rtnVal &= sysCfg.SaveToFile(GetXmlFilePath());
		close();

		return rtnVal;
	}
	else
	{
		IKMessageBox errorNote(this, QString("删除闹铃"), QString("无法打开系统配置文件！"), errorMbx);

		errorNote.exec();
		return false;
	}
}

bool AddAlarm::slotSave()
{
	Exectime etm(__FUNCTION__);
	//TO DO
	IKXml::IKXmlDocument sysCfg;

	if(!sysCfg.LoadFromFile(GetXmlFilePath()))
	{
		IKMessageBox errorNote(this, QString("保存闹铃信息"), QString("无法打开系统配置文件！"), errorMbx);

		errorNote.exec();
		return false;
	}

	//判断是否是新添加闹铃，如果是，就分配一个可用的闹铃标签
	if(alarmTag.isEmpty())
	{
		//TO DO
		alarmTag = getAlarmTag(sysCfg);
		if(alarmTag.isEmpty())
		{
			IKMessageBox errorNote(this, QString("保存闹铃信息"), QString("无法获取可用的闹铃名称！"), errorMbx);

			errorNote.exec();
			return false;
		}
	}

	if(saveAlarmInfo(ui->alarmTime->text(), alarmTag, sysCfg))
	{
		close();
		return true;
	}

	return false;
}


void AddAlarm::slotAdd()
{
	Exectime etm(__FUNCTION__);
	//TO DO
	//std::cout << "current alarm time: " << ui->alarmTime->text().toStdString() << std::endl;
	QTime curAlarmTime = QTime::fromString(ui->alarmTime->text(), "HH:mm:ss");


	switch(ui->alarmTime->currentSectionIndex())
	{
		case 0:
			curAlarmTime = curAlarmTime.addSecs(3600);
			break;
		case 1:
			curAlarmTime = curAlarmTime.addSecs(60);
			break;
		case 2:
		default:
			curAlarmTime = curAlarmTime.addSecs(1);
			break;
	}

	ui->alarmTime->setTime(curAlarmTime);
}


void AddAlarm::slotMinus()
{
	Exectime etm(__FUNCTION__);
	//TO DO
	//std::cout << "current alarm time: " << ui->alarmTime->text().toStdString() << " time section selected:" << ui->alarmTime->currentSectionIndex() << std::endl;
	QTime curAlarmTime = QTime::fromString(ui->alarmTime->text(), "HH:mm:ss");


	switch(ui->alarmTime->currentSectionIndex())
	{
		case 0:
			curAlarmTime = curAlarmTime.addSecs(-3600);
			break;
		case 1:
			curAlarmTime = curAlarmTime.addSecs(-60);
			break;
		case 2:
		default:
			curAlarmTime = curAlarmTime.addSecs(-1);
			break;
	}

	ui->alarmTime->setTime(curAlarmTime);
}


QString AddAlarm::getAlarmTag(const IKXml::IKXmlDocument &cfgFile)
{
	Exectime etm(__FUNCTION__);
	//TO DO
	QStringList alarmTags;
	alarmTags << "alarm1st" << "alarm2nd" << "alarm3rd" << "alarm4th" << "alarm5th" << "alarm6th" << "alarm7th" << "alarm8th";

	std::string alarmTime;
	for(int iter = 0; iter < alarmTags.size(); iter++)
	{
		alarmTime = cfgFile.GetElementValue(QString("/IKEMBX00/configures/alarm/%1").arg(alarmTags[iter]).toStdString());
		if(alarmTime.empty())
		{
			return alarmTags[iter];
		}
	}

	return QString("");
}

bool AddAlarm::saveAlarmInfo(const QString &time, const QString &alarmTag, const IKXml::IKXmlDocument &cfgFile)
{
	Exectime etm(__FUNCTION__);
	//TO DO
	QString alarmPath(QString("/IKEMBX00/configures/alarm/%1").arg(alarmTag));

	if(!cfgFile.Exist(alarmPath.toStdString()))
	{
		cfgFile.AddElement("/IKEMBX00/configures/alarm", alarmTag.toStdString(), time.toStdString());
	}
	else
	{
		cfgFile.SetElementValue(alarmPath.toStdString(), time.toStdString());
	}

	return cfgFile.SaveToFile(GetXmlFilePath());
}

