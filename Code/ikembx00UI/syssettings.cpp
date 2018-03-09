/*****************************************************************************
 ** 文 件 名：syssettings.cpp
 ** 主 要 类：SysSettings
 **
 ** Copyright (c) 中科虹霸有限公司
 **
 ** 创 建 人：L.Wang
 ** 创建时间：20013-10-18
 **
 ** 修 改 人：
 ** 修改时间：
 ** 描  述:   系统设置
 ** 主要模块说明: IP配置管理， 系统时间配置
 **
 ** 版  本:   1.0.0
 ** 备  注:
 **
 *****************************************************************************/
//QT头文件
#include <QStringList>
#include <QDebug>
#include <QProcess>
#include <QMessageBox>
#include <QRegExp>
#include <QDir>


//标准库头文件
#include <string.h>
#include <time.h>
#include <ifaddrs.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
//#define _GNU_SOURCE

//应用头文件
#include "../databaseLib/IKDatabaseLib/include/IKDatabaseLib.h"
#include "../ikIOStream/interface.h"
#include "../common.h"
#include "../XML/xml/IKXmlApi.h"      //XML接口头文件
#include "ikmessagebox.h"      //消息框头文件
#include "syssettings.h"
#include "ui_syssettings.h"
#include "network.h"
#include "iksocket.h"      //与服务器网络交互头文件
#include "ikxmlhelper.h"
#include "addalarm.h"      //闹铃操作相关头文件
#include "sysnetinterface.h"       //系统信息操作接口
#include "../Common/Logger.h"
#include "../Common/Exectime.h"
#include "interaction.h"

extern int G_BellTime;
extern int G_MaxTempt; //記錄cpu溫度最高


/*****************************************************************************
 *                        检查IP的合法性
 *  函 数 名：isValid
 *  功    能：对输入参数，确定指定的IP地址是否合法
 *  说    明：
 *  参    数：
 *  返 回 值：
 *  创 建 人：L.Wang
 *  创建时间：2014-01-08
 *  修 改 人：
 *  修改时间：
 *****************************************************************************/
bool isIPValid(const QString& addr)
{
	Exectime etm(__FUNCTION__);
	//TO DO
	//    const QRegExp ipPatten("^\\d{1,3}.\\d{1,3}.\\d{1,3}.\\d{1,3}$");
	const QRegExp ipPatten("^(((0?0?\\d)|(0?[1-9]\\d)|(1\\d{2})|(2[0-4]\\d)|(25[0-5]))(\\.((0?0?\\d)|(0?[1-9]\\d)|(1\\d{2})|(2[0-4]\\d)|(25[0-5]))){3})$");

	return ipPatten.exactMatch(addr.trimmed());
}





SysSettings::SysSettings(QWidget *parent) :
	QDialog(parent),
	ui(new Ui::SysSettings),
	isSettingsChanged(false),
	isDevNetInfoChanged(false),
	isServerCfgChanged(false),
	cfgFile("ikembx00.xml"),
	socket(nullptr),
	sigIns(nullptr),
	needReboot(true)
	//waitStatus(this, "正在同步系统时间...")
{
	Exectime etm(__FUNCTION__);
	ui->setupUi(this);

	//获取网络socket实例
	socket = IkSocket::GetIkSocketInstance();

	//获取信号实例
	sigIns = SysStatusSignal::GetInstance();

	//初始化UI
	initUI();


	//初始化AlarmTags
	alarmTags << "alarm1st" << "alarm2nd" << "alarm3rd" << "alarm4th" << "alarm5th" << "alarm6th" << "alarm7th" << "alarm8th";


	//回退事件
	connect(ui->btnBack, SIGNAL(clicked()), this, SIGNAL(sigGoBack()));
	//保存事件
	connect(ui->btnSaveNetworkSettings, SIGNAL(clicked()), this, SLOT(slotSaveNetworkSettings()));
	//修改管理员密码
	connect(ui->btnCommit, SIGNAL(clicked()), this, SLOT(slotSaveAdminInfo()));

	//定时器事件
	connect(&timer, SIGNAL(timeout()), this, SLOT(slotTimerOut()));
	//时间上调事件
	connect(ui->btnAdd, SIGNAL(clicked()), this, SLOT(slotAdd()));
	//时间下调事件
	connect(ui->btnMinus, SIGNAL(clicked()), this, SLOT(slotMinus()));
	//应用调整后的时间事件
	connect(ui->btnApply, SIGNAL(clicked()), this, SLOT(slotApplyTimeSettings()));
	//同步服务器时间
	connect(ui->btnAsyncTime, SIGNAL(clicked()), this, SLOT(slotAsyncTime()));
	//启动等待界面
	connect(this, SIGNAL(sigStartWaitingStatus()), this, SLOT(slotAsyncTimeWaiting()));

	//修改本机IP地址事件
	connect(ui->txtIPAddr, SIGNAL(textChanged(QString)), this, SLOT(slotIPChanged()));
	//修改服务端IP地址事件
	connect(ui->txtServerIP, SIGNAL(textChanged(QString)), this, SLOT(slotServerIPChanged()));
	//修改服务端端口事件
	connect(ui->txtServerPort, SIGNAL(textChanged(QString)), this, SLOT(slotSrvPortChanged()));
	//修改子网掩码事件
	connect(ui->txtMaskCode, SIGNAL(textChanged(QString)), this, SLOT(slotNetMaskChanged()));
	//修改网关事件
	connect(ui->txtNetGate, SIGNAL(textChanged(QString)), this, SLOT(slotGateChanged()));
	//    //修改DNS事件
	//    connect(ui->txtDNS, SIGNAL(textChanged(QString)), this, SLOT(slotDNSChanged()));
	//    //修改备用DNS事件
	//    connect(ui->txtViceDNS, SIGNAL(textChanged(QString)), this, SLOT(slotViceDNSChanged()));

	//存储识别选项设置
	connect(ui->btnSaveIdenSettings, SIGNAL(clicked()), this, SLOT(slotSaveIdenSettings()));
	//延长门铃响铃时间
	connect(ui->btnAddBellTime, SIGNAL(clicked()), this, SLOT(slotAddBellTime()));
	//缩短门铃响铃时间
	connect(ui->btnMinuBellTime, SIGNAL(clicked()), this, SLOT(slotMinusBellTime()));

	//保存其他配置信息
	connect(ui->btnSaveOthers, SIGNAL(clicked()), this, SLOT(slotSaveOtherSettings()));

	//调节休眠时间
	connect(ui->btnAddSleepTime, SIGNAL(clicked()), this, SLOT(slotAddSleepTime()));
	connect(ui->btnMinuSleepTime, SIGNAL(clicked()), this, SLOT(slotMinusSleepTime()));

	//调节系统音量
	connect(ui->btnTurnUpVolume, SIGNAL(clicked()), this, SLOT(slotTurnUpVolume()));
	connect(ui->btnTurnDownVolume, SIGNAL(clicked()), this, SLOT(slotTurnDownVolume()));
	//connect(ui->txtVolume, SIGNAL(textChanged(QString)), this, SLOT(slotChangeVolume()));


	//增加闹铃
	connect(ui->btnAddAlarm, SIGNAL(clicked()), this, SLOT(slotAddNewAlarm()));
	//修改闹铃
	connect(ui->twgtAlarms, SIGNAL(clicked(QModelIndex)), this, SLOT(slotEditAlarm(QModelIndex)));

	//恢复出厂设置
	connect(ui->btnFactorySet, SIGNAL(clicked()), this, SLOT(slotFactorySet()));

	//监视同步系统时间状态
	connect(sigIns, SIGNAL(sigSysTimeUpdated()), this, SLOT(slotAsyncTimeOK()));

	//同步服务器时间超时
	connect(&asyTimeTimer, SIGNAL(timeout()), this, SLOT(slotAsyncTimeTimeout()));

	//关联重启信号与对应的槽
	connect(this, SIGNAL(sigReboot()), this, SLOT(slotReboot()));

	//    //关联等待界面关闭的信号与相应的槽
	//    connect(this, SIGNAL(sigStopWaitingStauts()), &waitStatus, SLOT(close()));
}

SysSettings::~SysSettings()
{
	Exectime etm(__FUNCTION__);
	sigIns->Release();
	delete ui;
}

/*****************************************************************************
 *                        初始化系统管理UI
 *  函 数 名：initUI
 *  功    能：设置IP地址的输入格式控制，以及密码长度控制（暂设置为30个字符，可以再议）
 *  说    明：
 *  参    数：
 *  返 回 值：
 *  创 建 人：L.Wang
 *  创建时间：2013-10-18
 *  修 改 人：
 *  修改时间：
 *****************************************************************************/
bool SysSettings::initUI()
{
	Exectime etm(__FUNCTION__);
	//初始化系统时间设置
	initSysTime();

	//初始化网络设置
	initNetwork();

	//初始化管理员设置
	initAdminManage();

	//初始化识别设置
	initIdenSettings();

	//初始化其他设置
	initOthers();

	//初始化闹铃设置
	initAlarm();

	//设置网络设置页为默认页
	ui->tbwSysSettings->setCurrentWidget(ui->tabSetNet);

	return true;
}


/*****************************************************************************
 *                        系统时间设置初始化
 *  函 数 名：initSysTime
 *  功    能：初始化系统设置，设置系统时间更新定时器及启动
 *  说    明：
 *  参    数：
 *  返 回 值：
 *  创 建 人：L.Wang
 *  创建时间：2013-10-18
 *  修 改 人：
 *  修改时间：
 *****************************************************************************/
bool SysSettings::initSysTime()
{
	Exectime etm(__FUNCTION__);
	//初始化系统时间设置
	timer.setSingleShot(false);
	timer.start(1000);

	//显示系统时间
	ui->dateTimeEditor->setDateTime(QDateTime::currentDateTime());

	//联机设备网络不可用时不可执行时间同步操作
	IKXmlHelper syscfg(GetXmlFilePath());
	if(IKXmlHelper::DevWorkMode::ONLINE_MODE != syscfg.GetWorkMode())
	{
		ui->btnAsyncTime->hide();
	}
	else
	{
		ui->btnAsyncTime->show();
	}

	return true;
}


/*****************************************************************************
 *                        初始化网络模块
 *  函 数 名：initNetwork
 *  功    能：
 *  说    明：
 *  参    数：
 *  返 回 值：
 *  创 建 人：L.Wang
 *  创建时间：2013-10-18
 *  修 改 人：
 *  修改时间：
 *****************************************************************************/
bool SysSettings::initNetwork()
{
	Exectime etm(__FUNCTION__);
	//TO DO
	//设置网络校验格式
	QRegExp inputRegexp("^(((0?0?\\d)|(0?[1-9]\\d)|(1\\d{2})|(2[0-4]\\d)|(25[0-5]))(\\.((0?0?\\d)|(0?[1-9]\\d)|(1\\d{2})|(2[0-4]\\d)|(25[0-5]))){3})$");
	//Client end
	ui->txtIPAddr->setValidator(new QRegExpValidator(inputRegexp, this));
	ui->txtMaskCode->setValidator(new QRegExpValidator(inputRegexp, this));
	ui->txtNetGate->setValidator(new QRegExpValidator(inputRegexp, this));
	//ui->txtDNS->setValidator(new QRegExpValidator(inputRegexp, this));
	//ui->txtViceDNS->setValidator(new QRegExpValidator(inputRegexp, this));

	//Server end
	ui->txtServerIP->setValidator(new QRegExpValidator(inputRegexp, this));
	inputRegexp.setPattern("([0-5]?[0-9]{0,4}) ||(6[0-4][0-9]{3,3}) || (65[0-4][0-9]{2,2}) || (655[0-2][0-9]) || (6553[0-5])");
	ui->txtServerPort->setValidator(new QRegExpValidator(inputRegexp, this));

	//配置设备网络信息
	ui->txtIPAddr->setText(SysNetInterface::GetDevIP().c_str());    //配置IP地址
	ui->txtMaskCode->setText(NetInfo::GetNetMask().c_str());    //配置子网掩码
	ui->txtNetGate->setText(NetInfo::GetGateWay().c_str());    //配置网关

	//配置服务器网络信息
	ui->txtServerIP->setText(SysNetInterface::GetSrvIP().c_str());
	ui->txtServerPort->setText(SysNetInterface::GetSrvPort().c_str());


	//Hide the server network settings in case of device working under the offline mode
	IKXmlHelper xhelper(GetXmlFilePath());
	if(IKXmlHelper::DevWorkMode::ONLINE_MODE != xhelper.GetWorkMode())
	{
		ui->wgtServerNetWork->hide();
	}
	else
	{
		ui->wgtServerNetWork->show();
	}


	//设置返回键默认焦点
	ui->btnBack->setFocus();

	return true;
}


/*****************************************************************************
 *                        初始化管理员信息修改模块
 *  函 数 名：initAdminManage
 *  功    能：
 *  说    明：
 *  参    数：
 *  返 回 值：
 *  创 建 人：L.Wang
 *  创建时间：2013-10-18
 *  修 改 人：
 *  修改时间：
 *****************************************************************************/
bool SysSettings::initAdminManage()
{
	Exectime etm(__FUNCTION__);
	//TO DO
	ui->ledtCurPwd->clear();
	ui->ledtNewPwd->clear();
	ui->ledtCheckNewPwd->clear();

	//设置管理员密码最大长度
	ui->ledtCurPwd->setMaxLength(30);
	ui->ledtNewPwd->setMaxLength(30);
	ui->ledtCheckNewPwd->setMaxLength(30);

	return true;
}


bool SysSettings::initIdenSettings()
{
	Exectime etm(__FUNCTION__);
	//TO DO
	//ui->rbtnCardOrIrisIden->hide();
	IKXmlHelper syscfg(GetXmlFilePath());

    //ui->rbtnCardOrIrisIden->setChecked(false);
	switch(syscfg.GetIdenMode())
	{
		case IKXmlHelper::IDenMode::IRIS_AND_CARD:
			ui->rbtnCardAndIrisIden->setChecked(true);
			ui->rbtnIrisIden->setChecked(false);
			ui->rbtnCardOrIrisIden->setChecked(false);
			break;

		case IKXmlHelper::IDenMode::IRIS_OR_CARD:
			ui->rbtnCardAndIrisIden->setChecked(false);
			ui->rbtnIrisIden->setChecked(false);
			ui->rbtnCardOrIrisIden->setChecked(true);
			break;

		case IKXmlHelper::IDenMode::IRIS_ONLY:
		case IKXmlHelper::IDenMode::CARD_ONLY:
		case IKXmlHelper::IDenMode::UNKNOWN_MODE:
		default:
			ui->rbtnCardAndIrisIden->setChecked(false);
			ui->rbtnCardOrIrisIden->setChecked(false);
			ui->rbtnIrisIden->setChecked(true);
			break;


	}

	switch(syscfg.GetOutputTo())
	{
		case IKXmlHelper::OutputInfo::TO_WIEGAND:
			ui->cbxOutToWiegand->setChecked(true);
			ui->cbxOutToIO->setChecked(false);
			break;
		case IKXmlHelper::OutputInfo::TO_RELAY:
			ui->cbxOutToWiegand->setChecked(false);
			ui->cbxOutToIO->setChecked(true);
			break;
		case IKXmlHelper::OutputInfo::TO_WIEGAND_AND_RELAY:
			ui->cbxOutToWiegand->setChecked(true);
			ui->cbxOutToIO->setChecked(true);
			break;
		case IKXmlHelper::OutputInfo::TO_NONE:
		default:
			ui->cbxOutToWiegand->setChecked(false);
			ui->cbxOutToIO->setChecked(false);
			break;
	}

	//初始化门铃响铃时间限定条件
	QRegExp bellTimeRegexp("^[0-9]{0,2}$");
	ui->txtBellTime->setValidator(new QRegExpValidator(bellTimeRegexp, this));
	ui->txtBellTime->setText(QString("%1").arg(syscfg.GetDoorBellDuration()));

	return true;
}


bool SysSettings::initAlarm()
{
	Exectime etm(__FUNCTION__);
	//TO DO
	ui->twgtAlarms->setRowCount(0);
	ui->twgtAlarms->setColumnCount(2);
	ui->twgtAlarms->hideColumn(1);

	IKXmlHelper syscfg(GetXmlFilePath());
	const std::map<std::string, std::string> alarms = syscfg.GetAlarms();
	std::string alarmTime;

	for(int iter = 0; iter < alarmTags.size(); iter++)
	{
		alarmTime = alarms[(alarmTags[iter]).toStdString()];
		if(!alarmTime.empty())
		{
			ui->twgtAlarms->insertRow(ui->twgtAlarms->rowCount());

			QTableWidgetItem* item = new QTableWidgetItem(alarmTime.c_str());
			item->setIcon(QIcon(":/image/Alarm-icon.ico"));
			ui->twgtAlarms->setItem(ui->twgtAlarms->rowCount() - 1, 0, item);
			ui->twgtAlarms->setItem(ui->twgtAlarms->rowCount() - 1, 1, new QTableWidgetItem(alarmTags[iter]));
		}
	}

	//按照时间的先后顺序进行排序
	ui->twgtAlarms->sortByColumn(0,Qt::AscendingOrder);

	return true;
}


bool SysSettings::initOthers()
{
	Exectime etm(__FUNCTION__);
	//TO DO
	//Init System volume
	showVolume();

	IKXmlHelper syscfg(GetXmlFilePath());

	//Show sleep time settings
	ui->txtSleepTime->setText(QString("%1").arg(syscfg.GetSysPowerSaveTime()));
	ui->txtSleepTime->setDisabled(true);
	ui->btnAddSleepTime->hide();
	ui->btnMinuSleepTime->hide();

	//Show system language settings
	switch(syscfg.GetLanguage())
	{
		case IKXmlHelper::LanguageInfo::LANG_EN:
			ui->rbtnLanguageEn->setChecked(true);
			ui->rbtnLanguageCn->setChecked(false);
			primLanguage = "english";
			break;
		case IKXmlHelper::LanguageInfo::LANG_CN:
		default:
			ui->rbtnLanguageEn->setChecked(false);
			ui->rbtnLanguageCn->setChecked(true);
			primLanguage = "chinese";
			break;
	}

	ui->wgtSysLanguage->hide();


	return true;
}

void SysSettings::showVolume()
{
	Exectime etm(__FUNCTION__);
	if(getVolume(volumeVal))
	{
		if(0 == volumeVal)
		{
			ui->txtVolume->setText(QString::fromUtf8("静音"));
		}
		else if(volumeVal <= 45)
		{
			ui->txtVolume->setText(QString::fromUtf8("小"));
		}
		else if(volumeVal <= 75)
		{
			ui->txtVolume->setText(QString::fromUtf8("中"));
		}
		else if(volumeVal <= 95)
		{
			ui->txtVolume->setText(QString::fromUtf8("大"));
		}
		else
		{
			ui->txtVolume->setText(QString::fromUtf8("最大"));
		}
	}
}


/*****************************************************************************
 *                        存储系统设置
 *  函 数 名：slotSaveNetworkSettings
 *  功    能：槽函数，存储系统设置,主要用来存储网络设置
 *  说    明：
 *  参    数：
 *  返 回 值：
 *  创 建 人：L.Wang
 *  创建时间：2013-10-18
 *  修 改 人：
 *  修改时间：
 *****************************************************************************/
void SysSettings::slotSaveNetworkSettings()
{
	Exectime etm(__FUNCTION__);
	ui->btnSaveNetworkSettings->setEnabled(false);
	//TO DO
	//存储本地网络设置
	if(isDevNetInfoChanged)
	{
		if(ui->txtIPAddr->text().isEmpty() || ui->txtMaskCode->text().isEmpty() || ui->txtNetGate->text().isEmpty()
				|| !(isIPValid(ui->txtIPAddr->text()) && isIPValid(ui->txtMaskCode->text()) && isIPValid(ui->txtNetGate->text())))
		{
			IKMessageBox errorNote(this, QString::fromUtf8("设置网络"), QString::fromUtf8("本地网络配置选项非法！"), errorMbx);

			errorNote.exec();
			ui->btnSaveNetworkSettings->setEnabled(true);
			return;
		}

		if(!SysNetInterface::SetDevNetInfo(ui->txtIPAddr->text().toStdString(), ui->txtMaskCode->text().toStdString(), ui->txtNetGate->text().toStdString()))
		{
			IKMessageBox errorNote(this, QString::fromUtf8("设置网络"), QString::fromUtf8("保存设备网络信息失败！"), errorMbx);

			errorNote.exec();
			ui->btnSaveNetworkSettings->setEnabled(true);
			return;
		}
	}

	//更新服务器配置，如果其已被修改
	if(isServerCfgChanged)
	{
		if(!SysNetInterface::SetSrvNetInfo(ui->txtServerIP->text().toStdString(),ui->txtServerPort->text().toInt()))
		{
			IKMessageBox errorNote(this, QString::fromUtf8("设置网络"), QString::fromUtf8("保存服务器网络信息失败！"), errorMbx);

			errorNote.exec();
			ui->btnSaveNetworkSettings->setEnabled(true);
			return;
		}
		else
		{
			//通知服务器使用者服务器配置更新
			emit sigServerCfgUpdated(true);
		}
	}

	if(isDevNetInfoChanged || isServerCfgChanged)
	{
		isDevNetInfoChanged = false;
		isServerCfgChanged = false;

		IKMessageBox questionNote(this, QString::fromUtf8("设置网络"), QString::fromUtf8("操作成功！需要重新启动设备以使设置生效，是否现在重启？"), questionMbx);
		questionNote.exec();

		if(QDialog::Accepted == questionNote.result())
		{
			//emit sigReboot();
			pwMgr.Reboot();
		}
	}
	else
	{
		//提示更新网络信息成功
		IKMessageBox infoNote(this, QString::fromUtf8("设置网络"), QString::fromUtf8("网络信息无变化，无需保存！"), informationMbx);
		infoNote.exec();
	}

	ui->btnSaveNetworkSettings->setEnabled(true);

}

/*****************************************************************************
 *                        存储系统设置
 *  函 数 名：slotSaveAdminInfo
 *  功    能：槽函数，存储系统设置,主要用来存储管理员密码
 *  说    明：
 *  参    数：
 *  返 回 值：
 *  创 建 人：L.Wang
 *  创建时间：2013-10-18
 *  修 改 人：
 *  修改时间：
 *****************************************************************************/
void SysSettings::slotSaveAdminInfo()
{
	Exectime etm(__FUNCTION__);
	//TO DO
	//检查密码是否为空
	if(!checkPwdsEmpty())
	{
		//如果密码为空，则返回
		return;
	}


	//检查确认密码是否和新密码相同
	if(checkNewPwds())
	{
		//相同则返回
		return;
	}

	//验证新密码是否与当前密码相同
	if(!checkNewPwdNotSameToOld())
	{
		//相同则返回
		return;
	}

	//验证当前密码
	if(!checkOldPwd())
	{
		//如果验证失败则返回
		return;
	}

	//更新管理员密码
	updateNewPwd();
}


void SysSettings::slotAddNewAlarm()
{
	Exectime etm(__FUNCTION__);
	//TO DO
	if(ui->twgtAlarms->rowCount() >= alarmLimits)
	{
		IKMessageBox infoNote(this, QString("添加闹铃"), QString("闹铃数量达到限制！"), informationMbx);

		infoNote.exec();
		return;
	}

	AddAlarm addAlarm(this);
	addAlarm.exec();

	initAlarm();
}

void SysSettings::slotEditAlarm(QModelIndex index)
{
	Exectime etm(__FUNCTION__);
	//TO DO
	AddAlarm editAlarm(index.data().toString(), index.sibling(index.row(), 1).data().toString(), this);
	editAlarm.exec();

	initAlarm();
}

/*****************************************************************************
 *                        检验IP地址的有效性
 *  函 数 名：checkIPValid
 *  功    能：
 *  说    明：
 *          err， 返回错误信息
 *
 *  参    数：
 *  返 回 值：
 *  创 建 人：L.Wang
 *  创建时间：2013-10-18
 *  修 改 人：
 *  修改时间：
 *****************************************************************************/
bool SysSettings::checkIPValid(QString& err, const QString &ip, const QString ipInfo)
{
	Exectime etm(__FUNCTION__);
	//TO DO
	if(ip.trimmed().isEmpty())
	{
		if(ipInfo.contains("DNS"))  //允许DNS或备用DNS为空
		{
			return true;
		}

		err.append(QString::fromUtf8("%1为空！\n").arg(ipInfo));
		LOG_ERROR("%s",err.toStdString().c_str());
		return false;
	}
	else
	{
		if(!isIPValid(ip))
		{
			err.append(QString::fromUtf8("%1格式非法！\n").arg(ipInfo));
			LOG_ERROR("%s",err.toStdString().c_str());
			return false;
		}
	}

	return true;
}

bool SysSettings::checkIPs()
{
	Exectime etm(__FUNCTION__);
	QString errMsg;
	errMsg.clear();

	if(!checkIPValid(errMsg, ui->txtIPAddr->text(), QString::fromUtf8("本地IP地址")) ||
			!checkIPValid(errMsg, ui->txtServerIP->text(), QString("服务端IP地址"))||
			!checkIPValid(errMsg, ui->txtMaskCode->text(), QString::fromUtf8("子网掩码")) ||
			!checkIPValid(errMsg, ui->txtNetGate->text(), QString::fromUtf8("网关"))/* ||
																					   !checkIPValid(errMsg, ui->txtDNS->text(), QString::fromUtf8("DNS")) ||
																					   !checkIPValid(errMsg, ui->txtViceDNS->text(), QString::fromUtf8("备用DNS"))*/)
	{
		IKMessageBox errorNote(this, QString::fromUtf8("网络设置"), QString::fromUtf8(errMsg), errorMbx);

		errorNote.exec();
		return false;
	}
	else
	{
		return true;
	}
}


/*****************************************************************************
 *                        系统时间显示定时器超时处理
 *  函 数 名：slotTimerOut
 *  功    能：槽函数，处理定时器超时情况
 *  说    明：
 *  参    数：
 *  返 回 值：
 *  创 建 人：L.Wang
 *  创建时间：2013-10-18
 *  修 改 人：
 *  修改时间：
 *****************************************************************************/
void SysSettings::slotTimerOut()
{
	Exectime etm(__FUNCTION__);
	//TO DO
	ui->dateTimeEditor->setDateTime(QDateTime::currentDateTime());
}

/*****************************************************************************
 *                        时间增量调整
 *  函 数 名：slotAdd
 *  功    能：槽函数，增加指定时间分量
 *  说    明：
 *  参    数：
 *  返 回 值：
 *  创 建 人：L.Wang
 *  创建时间：2013-10-18
 *  修 改 人：
 *  修改时间：
 *****************************************************************************/
void SysSettings::slotAdd()
{
	Exectime etm(__FUNCTION__);
	//TO DO
	if(timer.isActive())
	{
		timer.stop();
	}
	//修改时间
	QDateTime dateTime = ui->dateTimeEditor->dateTime();

	//ui->btnAdd->setText(dateTime.toString("yyyy-MM-dd hh:mm:ss"));
	switch(ui->dateTimeEditor->currentSectionIndex())
	{
		case 0:
			dateTime = dateTime.addYears(1);
			break;
		case 1:
			dateTime = dateTime.addMonths(1);
			break;
		case 2:
			dateTime = dateTime.addDays(1);
			break;
		case 3:
			dateTime = dateTime.addSecs(3600);
			break;
		case 4:
			dateTime = dateTime.addSecs(60);
			break;
		case 5:
		default:
			dateTime = dateTime.addSecs(1);
			break;
	}
	//ui->btnMinus->setText(dateTime.toString("yyyy-MM-dd hh:mm:ss"));
	ui->dateTimeEditor->setDateTime(dateTime);
}

/*****************************************************************************
 *                        时间增量
 *  函 数 名：slotShutdown
 *  功    能：槽函数，减少指定时间分量
 *  说    明：
 *  参    数：
 *  返 回 值：
 *  创 建 人：L.Wang
 *  创建时间：2013-10-18
 *  修 改 人：
 *  修改时间：
 *****************************************************************************/
void SysSettings::slotMinus()
{
	Exectime etm(__FUNCTION__);
	//TO DO
	if(timer.isActive())
	{
		timer.stop();
	}

	//修改时间
	QDateTime dateTime = ui->dateTimeEditor->dateTime();

	switch(ui->dateTimeEditor->currentSectionIndex())
	{
		case 0:
			dateTime = dateTime.addYears(-1);
			break;
		case 1:
			dateTime = dateTime.addMonths(-1);
			break;
		case 2:
			dateTime = dateTime.addDays(-1);
			break;
		case 3:
			dateTime = dateTime.addSecs(-3600);
			break;
		case 4:
			dateTime = dateTime.addSecs(-60);
			break;
		case 5:
		default:
			dateTime = dateTime.addSecs(-1);
			break;
	}

	ui->dateTimeEditor->setDateTime(dateTime);
}

/*****************************************************************************
 *                        应用时间设置
 *  函 数 名：slotApplyTimeSettings
 *  功    能：槽函数，使当前设置的时间生效 和城市信息有效
 *  说    明：
 *  参    数：
 *  返 回 值：
 *  创 建 人：L.Wang
 *  创建时间：2013-10-18
 *  修 改 人：guodj
 *  修改时间：20160822
 *****************************************************************************/
bool SysSettings::slotApplyTimeSettings()
{

Exectime etm(__FUNCTION__);
    G_MaxTempt = ui->cpuTemptValue->text().toInt();

    if(ui->isSetCity->isChecked())
    {
        QString cityName = ui->cityNameLine->text();
        if(isCityExist(cityName))
        {
            // 城市信息
            IKXmlHelper syscfg(GetXmlFilePath());
            syscfg.SetCityName(cityName.toStdString());
            syscfg.SaveToFile(GetXmlFilePath());
            emit sigUpdateCity(m_isFromNet,cityName);

        }
        else
        {
            IKMessageBox infoNote(this, QString::fromUtf8("设置城市"), QString::fromUtf8("找不到您所输入的城市，请核对后重新输入"), informationMbx);
            infoNote.exec();
            return false;
        }
    }

    //TO DO
    if(SysNetInterface::SetSysTime(ui->dateTimeEditor->dateTime().toString("yyyy-MM-dd hh:mm:ss").toStdString()))
    {
        IKMessageBox infoNote(this, QString::fromUtf8("设置系统时间"), QString::fromUtf8("操作成功！"), informationMbx);
        infoNote.exec();
    }
    else
    {
        IKMessageBox errorNote(this, QString::fromUtf8("网络设置"), QString::fromUtf8(strerror(errno)), errorMbx);
        errorNote.exec();
    }


    //启动时间定时器刷新时间
    timer.start(1000);

    return true;
}

/*****************************************************************************
 *                        同步服务器时间
 *  函 数 名：slotAsyncTime
 *  功    能：槽函数，同步服务器时间处理函数
 *  说    明：设备向服务器同步服务器的时间
 *  参    数：
 *  返 回 值：
 *  创 建 人：L.Wang
 *  创建时间：2014-05-31
 *  修 改 人：
 *  修改时间：
 *****************************************************************************/
void SysSettings::slotAsyncTime()
{
	Exectime etm(__FUNCTION__);
	//TO DO
	if(nullptr == socket || !(socket->IsNetOK()))
	{
		IKMessageBox infoNote(this, QString::fromUtf8("同步系统时间"), QString::fromUtf8("无法连接到服务器！请确认网络后重试！"), informationMbx);
		infoNote.exec();

		return;
	}

	pthread_t thrID;
	pthread_create(&thrID, nullptr, threadAsyncTime, (void *)this);

	//开启同步时间超时定时器
	asyTimeTimer.setSingleShot(true);
	asyTimeTimer.start(4 * 5000);

	emit sigStartWaitingStatus();
}


/*****************************************************************************
 *                        同步服务器时间成功
 *  函 数 名：slotAsyncTimeOK
 *  功    能：槽函数，与服务器同步时间成功时的处理函数
 *  说    明：
 *  参    数：
 *  返 回 值：
 *  创 建 人：L.Wang
 *  创建时间：2014-06-05
 *  修 改 人：
 *  修改时间：
 *****************************************************************************/
void SysSettings::slotAsyncTimeOK()
{
	Exectime etm(__FUNCTION__);
	//启动刷新时间定时器
	timer.start(1000);


	//停止同步服务器时间定时器
	if(asyTimeTimer.isActive())
	{
		asyTimeTimer.stop();
		emit sigStopWaitingStauts();

		IKMessageBox infoNote(this, QString::fromUtf8("设置系统时间"), QString::fromUtf8("同步成功！"), informationMbx);
		infoNote.exec();
	}


}


void SysSettings::slotAsyncTimeTimeout()
{
	Exectime etm(__FUNCTION__);
	//启动刷新时间定时器
	timer.start(1000);

	//发信号关闭等待界面
	emit sigStopWaitingStauts();


	IKMessageBox infoNote(this, QString::fromUtf8("设置系统时间"), QString::fromUtf8("设备与服务器已断开，请稍后重试！"), informationMbx);
	infoNote.exec();


}

void SysSettings::slotAsyncTimeWaiting()
{
	Exectime etm(__FUNCTION__);
	//启动同步时间等待界面
	Buffering waitStatus(this, "正在同步系统时间...");

	//关联等待界面关闭的信号与相应的槽
	connect(this, SIGNAL(sigStopWaitingStauts()), &waitStatus, SLOT(close()));

	waitStatus.exec();
}

/*****************************************************************************
 *                        编辑IP地址槽函数
 *  函 数 名：slotIPChanged
 *  功    能：槽函数，当IP地址发生变化
 *  说    明：
 *  参    数：
 *  返 回 值：
 *  创 建 人：L.Wang
 *  创建时间：2013-10-18
 *  修 改 人：
 *  修改时间：
 *****************************************************************************/
void SysSettings::slotIPChanged()
{
	Exectime etm(__FUNCTION__);
	//TO DO
	if(!isIPValid(ui->txtIPAddr->text()))
	{
		ui->txtIPAddr->setStyleSheet("color:rgb(255,0,0);"
				"background-color: rgb(255, 255, 255);"
				"border-radius: 8px; "
				"border:2px groove gray;"
				"padding: 2px 2px; ");
	}
	else
	{
		ui->txtIPAddr->setStyleSheet("color:rgb(0,0,0);"
				"background-color: rgb(255, 255, 255);"
				"border-radius: 8px; "
				"border:2px groove gray;"
				"padding: 2px 2px; ");
	}
	isDevNetInfoChanged = true;
}

/*****************************************************************************
 *                        编辑子网掩码
 *  函 数 名：slotNetMaskChanged
 *  功    能：
 *  说    明：
 *  参    数：
 *  返 回 值：
 *  创 建 人：L.Wang
 *  创建时间：2014-02-18
 *  修 改 人：
 *  修改时间：
 *****************************************************************************/
void SysSettings::slotNetMaskChanged()
{
	Exectime etm(__FUNCTION__);
	//TO DO
	if(!isIPValid(ui->txtMaskCode->text()))
	{
		ui->txtMaskCode->setStyleSheet("color:rgb(255,0,0);"
				"background-color: rgb(255, 255, 255);"
				"border-radius: 8px; "
				"border:2px groove gray;"
				"padding: 2px 2px; ");
	}
	else
	{
		ui->txtMaskCode->setStyleSheet("color:rgb(0,0,0);"
				"background-color: rgb(255, 255, 255);"
				"border-radius: 8px; "
				"border:2px groove gray;"
				"padding: 2px 2px; " );
	}

	isDevNetInfoChanged = true;
}

/*****************************************************************************
 *                        编辑网关
 *  函 数 名：slotGateChanged
 *  功    能：
 *  说    明：
 *  参    数：
 *  返 回 值：
 *  创 建 人：L.Wang
 *  创建时间：2014-02-18
 *  修 改 人：
 *  修改时间：
 *****************************************************************************/
void SysSettings::slotGateChanged()
{
	Exectime etm(__FUNCTION__);
	//TO DO
	if(!isIPValid(ui->txtNetGate->text()))
	{
		ui->txtNetGate->setStyleSheet("color:rgb(255,0,0);"
				"background-color: rgb(255, 255, 255);"
				"border-radius: 8px; "
				"border:2px groove gray;"
				"padding: 2px 2px; ");
	}
	else
	{
		ui->txtNetGate->setStyleSheet("color:rgb(0,0,0);"
				"background-color: rgb(255, 255, 255);"
				"border-radius: 8px; "
				"border:2px groove gray;"
				"padding: 2px 2px; ");
	}

	isDevNetInfoChanged = true;
}

///*****************************************************************************
//*                        修改DNS
//*  函 数 名：slotDNSChanged
//*  功    能：
//*  说    明：
//*  参    数：
//*  返 回 值：
//*  创 建 人：L.Wang
//*  创建时间：2013-10-18
//*  修 改 人：
//*  修改时间：
//*****************************************************************************/
//void SysSettings::slotDNSChanged()
//{
//    //TO DO
//    if(!isIPValid(ui->txtDNS->text()))
//    {
//        ui->txtDNS->setStyleSheet("color:rgb(255,0,0)");
//    }
//    else
//    {
//        ui->txtDNS->setStyleSheet("color:rgb(0,0,0)");
//    }
//}

///*****************************************************************************
//*                        修改DNS
//*  函 数 名：slotDNSChanged
//*  功    能：
//*  说    明：
//*  参    数：
//*  返 回 值：
//*  创 建 人：L.Wang
//*  创建时间：2013-10-18
//*  修 改 人：
//*  修改时间：
//*****************************************************************************/
//void SysSettings::slotViceDNSChanged()
//{
//    //TO DO
//    if(!isIPValid(ui->txtViceDNS->text()))
//    {
//        ui->txtViceDNS->setStyleSheet("color:rgb(255,0,0)");
//    }
//    else
//    {
//        ui->txtViceDNS->setStyleSheet("color:rgb(0,0,0)");
//    }
//}

/*****************************************************************************
 *                        编辑服务端IP地址
 *  函 数 名：slotServerIPChanged
 *  功    能：槽函数，当服务端IP地址发生变化
 *  说    明：
 *  参    数：
 *  返 回 值：
 *  创 建 人：L.Wang
 *  创建时间：2013-10-18
 *  修 改 人：
 *  修改时间：
 *****************************************************************************/
void SysSettings::slotServerIPChanged()
{
	Exectime etm(__FUNCTION__);
	//TO DO
	if(!isIPValid(ui->txtServerIP->text()))
	{
		ui->txtServerIP->setStyleSheet("color:rgb(255,0,0);"
				"background-color: rgb(255, 255, 255);"
				"border-radius: 8px; "
				"border:2px groove gray;"
				"padding: 2px 2px; ");
	}
	else
	{
		ui->txtServerIP->setStyleSheet("color:rgb(0,0,0);"
				"background-color: rgb(255, 255, 255);"
				"border-radius: 8px; "
				"border:2px groove gray;"
				"padding: 2px 2px; ");
	}

	isServerCfgChanged = true;
}

/*****************************************************************************
 *                        编辑服务端端口
 *  函 数 名：slotSrvPortChanged
 *  功    能：槽函数，当服务端端口发生变化
 *  说    明：
 *  参    数：
 *  返 回 值：
 *  创 建 人：L.Wang
 *  创建时间：2014-06-12
 *  修 改 人：
 *  修改时间：
 *****************************************************************************/
void SysSettings::slotSrvPortChanged()
{
	Exectime etm(__FUNCTION__);
	//TO DO
	isServerCfgChanged = true;
}


/*****************************************************************************
 *                        屏蔽Esc键信号
 *  函 数 名：reject
 *  功    能：
 *  说    明：
 *  参    数：
 *  返 回 值：
 *  创 建 人：L.Wang
 *  创建时间：2014-08-14
 *  修 改 人：
 *  修改时间：
 *****************************************************************************/
void SysSettings::reject()
{
	Exectime etm(__FUNCTION__);
	//DO NOTHING
}


/*****************************************************************************
 *                       获取本机IP地址
 *  函 数 名：loadDeviceIP
 *  功    能：从系统获取本机IP地址
 *  说    明：
 *  参    数：
 *  返 回 值：IP地址字符串
 *  创 建 人：L.Wang
 *  创建时间：2014-3-25
 *  修 改 人：
 *  修改时间：
 *****************************************************************************/
QString SysSettings::loadDeviceIP()
{
	Exectime etm(__FUNCTION__);
	//TO DO
	return NetInfo::GetIP().c_str();
}

/*****************************************************************************
 *                       获取本机IP地址
 *  函 数 名：saveDeviceIP
 *  功    能：更新本机IP地址
 *  说    明：
 *  参    数：
 *  返 回 值：IP地址字符串
 *  创 建 人：L.Wang
 *  创建时间：2014-3-25
 *  修 改 人：
 *  修改时间：
 *****************************************************************************/
bool SysSettings::saveDeviceIP(const QString &serverIP)
{
	Exectime etm(__FUNCTION__);
	//TO DO
	if(!checkIPs())
	{
		LOG_ERROR("checkIPs failed");
		return false;
	}

	//获取网卡
	interfaceNames = NetInfo::GetEths();
	NetWork nw;

	//填充网络配置参数
	nw.SetInterfaceName(*interfaceNames.begin());                //设置网卡名称
	nw.SetIP(ui->txtIPAddr->text().toUtf8().data());             //设置IP地址
	nw.SetNetMask(ui->txtMaskCode->text().toUtf8().data());      //设置子网掩码
	nw.SetGateWay(ui->txtNetGate->text().toUtf8().data());       //设置网关
	//    nw.SetDNS(ui->txtDNS->text().toUtf8().data());             //设置DNS
	//    nw.SetViceDNS(ui->txtViceDNS->text().toUtf8().data());     //设置备用DNS


	//将网络配置写入系统配置文件
	if(!nw.UpdateToSystem(staticSet))
	{
		IKMessageBox errorNote(this, QString::fromUtf8("设置网络"), QString::fromUtf8(strerror(errno)), errorMbx);

		errorNote.exec();
		return false;
	}
	else
	{
		return true;
	}
}

/****************************************************************************
 *                       获取本机IP地址
 *  函 数 名：loadServerIP
 *  功    能：从配置文件中获取服务端IP地址
 *  说    明：
 *  参    数：
 *  返 回 值：IP地址字符串
 *  创 建 人：L.Wang
 *  创建时间：2014-3-25
 *  修 改 人：
 *  修改时间：
 ****************************************************************************/
const ServerSettings SysSettings::loadServerSettings()
{
	Exectime etm(__FUNCTION__);
	//TO DO
	IKXmlHelper cfgFile(GetXmlFilePath());
	ServerSettings st;

	st.SetIP(cfgFile.GetServerIP());
	st.SetPort(cfgFile.GetServerPort());


	return st;
}

/*****************************************************************************
 *                       保存服务端IP地址
 *  函 数 名：saveServerIP
 *  功    能：将服务端IP地址写入配置文件
 *  说    明：
 *  参    数：
 *  返 回 值：成功返回true，其他返回fasle
 *  创 建 人：L.Wang
 *  创建时间：2014-3-25
 *  修 改 人：
 *  修改时间：
 *****************************************************************************/
bool SysSettings::saveServerSettings(/*const ServerSettings &serverSettings*/)
{
	Exectime etm(__FUNCTION__);
	//TO DO
	IKXmlHelper syscfg(GetXmlFilePath());

	if(!syscfg.IsValid() && !syscfg.CreatXmlFile(GetXmlFilePath()))
	{
		IKMessageBox errorNote(this, QString::fromUtf8("设置网络"), QString::fromUtf8("保存服务端IP地址失败！"), errorMbx);

		errorNote.exec();
		return false;
	}

	if(syscfg.SetServerIP(ui->txtServerIP->text().toStdString()) &&
			syscfg.SetServerPort(ui->txtServerPort->text().toStdString()) &&
			syscfg.SaveToFile())
	{
		return true;
	}
	else
	{
		IKMessageBox errorNote(this, QString::fromUtf8("设置网络"), QString::fromUtf8("保存服务端网络失败"), errorMbx);

		errorNote.exec();
		return false;
	}
}


void SysSettings::slotSaveIdenSettings()
{
	Exectime etm(__FUNCTION__);
	//TO DO
	isIdenSettingsChanged = false;

	IKXmlHelper syscfg(GetXmlFilePath());
	IKXmlHelper::IDenMode idenMode;

	/// only iris
	if(ui->rbtnIrisIden->isChecked())
	{
		idenMode = IKXmlHelper::IDenMode::IRIS_ONLY;
	}
	/// iris and card
	if(ui->rbtnCardAndIrisIden->isChecked())
	{
		idenMode = IKXmlHelper::IDenMode::IRIS_AND_CARD;
	}
	///////////////////////////////
	// added at 20150826 by yqhe
	// 增加刷卡或虹膜识别方式
	if (ui->rbtnCardOrIrisIden->isChecked())
	{
		idenMode = IKXmlHelper::IDenMode::IRIS_OR_CARD;
	}
	///////////////////////////////

	syscfg.SetIdenMode(idenMode);


	//Save output information
	IKXmlHelper::OutputInfo outputInfo;
	if(ui->cbxOutToWiegand->isChecked())
	{
		if(ui->cbxOutToIO->isChecked())
		{
			outputInfo = IKXmlHelper::OutputInfo::TO_WIEGAND_AND_RELAY;
		}
		else
		{
			outputInfo = IKXmlHelper::OutputInfo::TO_WIEGAND;
		}
	}
	else
	{
		if(ui->cbxOutToIO->isChecked())
		{
			outputInfo = IKXmlHelper::OutputInfo::TO_RELAY;
		}
		else
		{
			outputInfo = IKXmlHelper::OutputInfo::TO_NONE;
		}
	}
	syscfg.SetOutputTo(outputInfo);


	//Save doorbell ringing duration
	G_BellTime = ui->txtBellTime->text().toInt();

	std::cout<<"<-----------set Bell Time----->"<<G_BellTime<<endl;
	syscfg.SetDoorBellDuration(ui->txtBellTime->text().toInt());

	if(syscfg.SaveToFile())
	{
		IKMessageBox infoNote(this, QString::fromUtf8("设置识别配置"), QString::fromUtf8("操作成功！"), informationMbx);

		infoNote.exec();

	}
	else
	{
		IKMessageBox errorNote(this, QString::fromUtf8("设置识别配置"), QString::fromUtf8("无法存储识别配置信息！"), errorMbx);

		errorNote.exec();

	}   
}


bool SysSettings::slotAddBellTime()
{
	Exectime etm(__FUNCTION__);
	int curVal = ui->txtBellTime->text().toInt();
	if(99 != curVal)
	{
		ui->txtBellTime->setText(QString("%1").arg(ui->txtBellTime->text().toInt() + 1));
	}
	else
	{
		ui->txtBellTime->setText(QString("%1").arg(curVal));
	}

	return true;
}


bool SysSettings::slotMinusBellTime()
{
	Exectime etm(__FUNCTION__);
	int curVal = ui->txtBellTime->text().toInt();
	if(0 != curVal)
	{
		ui->txtBellTime->setText(QString("%1").arg(curVal - 1));
	}
	else
	{
		ui->txtBellTime->setText(QString("%1").arg(curVal));
	}

	return true;
}

void SysSettings::slotIdenSettingsChanged()
{
	Exectime etm(__FUNCTION__);
	isIdenSettingsChanged = true;
}


void SysSettings::slotOtherSettingsChanged()
{
	Exectime etm(__FUNCTION__);
	isOtherSettingsChanged = true;
}


void SysSettings::slotSaveOtherSettings()
{
	Exectime etm(__FUNCTION__);
	//TO DO
	if(!changeVolume(volumeVal))
	{
		IKMessageBox errorNote(this, QString::fromUtf8("保存其它配置"), QString::fromUtf8("设置系统音量失败！"), errorMbx);

		errorNote.exec();
		return;
	}

	IKXmlHelper syscfg(GetXmlFilePath());
	if(!syscfg.IsValid())
	{
		IKMessageBox errorNote(this, QString::fromUtf8("保存其它配置"), QString::fromUtf8("无法打开系统配置文件！"), errorMbx);

		errorNote.exec();
		return;
	}

	syscfg.SetSysPowerSaveTime(ui->txtSleepTime->text().toInt());

	//获取语言设置
	QString newLanguage;
	if(ui->rbtnLanguageCn->isChecked())
	{
		syscfg.SetLanguage(IKXmlHelper::LanguageInfo::LANG_CN);
		newLanguage = "chinese";
	}
	else
	{
		syscfg.SetLanguage(IKXmlHelper::LanguageInfo::LANG_EN);
		newLanguage = "english";
	}

	if(!syscfg.SaveToFile())
	{
		IKMessageBox errorNote(this, QString::fromUtf8("其它配置"), QString::fromUtf8("无法存储配置信息！"), errorMbx);

		errorNote.exec();
	}
	else
	{
		if(primLanguage == newLanguage)
		{
			IKMessageBox infoNote(this, QString::fromUtf8("其它配置"), QString::fromUtf8("操作成功！"), informationMbx);

			infoNote.exec();
		}
		else
		{
			IKMessageBox questionNote(this, QString::fromUtf8("其它配置"), QString::fromUtf8("操作成功！需要重启以完成语言设置，立刻重启？"), questionMbx);

			questionNote.exec();
			if(QDialog::Accepted == questionNote.result())
			{
				pwMgr.Reboot();
			}
			primLanguage = newLanguage;
		}
	}

	isIdenSettingsChanged = false;
}

void SysSettings::slotAddSleepTime()
{
	Exectime etm(__FUNCTION__);
	//TO DO
	int curSleepTime = ui->txtSleepTime->text().toInt();
	if(60 != curSleepTime)
	{
		ui->txtSleepTime->setText(QString("%1").arg(curSleepTime + 1));
	}
	else
	{
		ui->txtSleepTime->setText(QString("%1").arg(curSleepTime));
	}
}

void SysSettings::slotMinusSleepTime()
{
	Exectime etm(__FUNCTION__);
	//TO DO
	int curSleepTime = ui->txtSleepTime->text().toInt();
	if(0 != curSleepTime)
	{
		ui->txtSleepTime->setText(QString("%1").arg(curSleepTime - 1));
	}
	else
	{
		ui->txtSleepTime->setText(QString("%1").arg(curSleepTime));
	}
}

void SysSettings::slotTurnUpVolume()
{
	Exectime etm(__FUNCTION__);
	//TO DO
	QString curVol = ui->txtVolume->text();
	if(!curVol.compare(QString::fromUtf8("静音")))
	{
		volumeVal = 40;
		ui->txtVolume->setText(QString::fromUtf8("小"));
	}

	if(!curVol.compare(QString::fromUtf8("小")))
	{
		volumeVal = 70;
		ui->txtVolume->setText(QString::fromUtf8("中"));
	}

	if(!curVol.compare(QString::fromUtf8("中")))
	{
		volumeVal = 90;
		ui->txtVolume->setText(QString::fromUtf8("大"));
	}

	if(!curVol.compare(QString::fromUtf8("大")))
	{
		volumeVal = 100;
		ui->txtVolume->setText(QString::fromUtf8("最大"));
	}

	qDebug() << "current volume: " << volumeVal;
}

void SysSettings::slotTurnDownVolume()
{
	Exectime etm(__FUNCTION__);
	QString curVol = ui->txtVolume->text();
	if(!curVol.compare(QString::fromUtf8("最大")))
	{
		volumeVal = 90;
		ui->txtVolume->setText(QString::fromUtf8("大"));
	}

	if(!curVol.compare(QString::fromUtf8("大")))
	{
		volumeVal = 70;
		ui->txtVolume->setText(QString::fromUtf8("中"));
	}

	if(!curVol.compare(QString::fromUtf8("中")))
	{
		volumeVal = 40;
		ui->txtVolume->setText(QString::fromUtf8("小"));
	}

	if(!curVol.compare(QString::fromUtf8("小")))
	{
		volumeVal = 0;
		ui->txtVolume->setText(QString::fromUtf8("静音"));
	}

	qDebug() << "current volume: " << volumeVal;
}


/*****************************************************************************
 *                        语音播报音量调整
 *  函 数 名：changeVolume
 *  功    能：
 *  说    明：
 *  参    数：
 *  返 回 值：
 *  创 建 人：L.Wang
 *  创建时间：2014-04-16
 *  修 改 人：
 *  修改时间：
 *****************************************************************************/
bool SysSettings::changeVolume(int ratio)
{
	Exectime etm(__FUNCTION__);
	//TO DO
	QString volOpt("amixer set Master %1%");
	volOpt = volOpt.arg(ratio);

	int rtnVal = system(volOpt.toStdString().c_str());
	if(rtnVal < 0)
	{
		std::cout << GetLogHead(__FUNCTION__)
			<< "failed to set system volume to " << volumeVal
			<< std::endl;
		IKMessageBox errorNote(this, QString::fromUtf8("设置系统音量"), QString::fromUtf8("设置音量失败！"), errorMbx);

		errorNote.exec();
		return false;
	}

	return true;
}


/*****************************************************************************
 *                        语音播报音量调整
 *  函 数 名：slotChangeVolume
 *  功    能：
 *  说    明：
 *  参    数：
 *  返 回 值：
 *  创 建 人：L.Wang
 *  创建时间：2014-04-16
 *  修 改 人：
 *  修改时间：
 *****************************************************************************/
void SysSettings::slotChangeVolume()
{
	Exectime etm(__FUNCTION__);
	//TODO
	changeVolume(volumeVal);
}

/*****************************************************************************
 *                       判断配置文件是否存在
 *  函 数 名：isCfgFileExist
 *  功    能：判断可执行程序所在目录中是否有配置文件存在
 *  说    明：
 *  参    数：
 *  返 回 值：存在返回true， 其他返回false
 *  创 建 人：L.Wang
 *  创建时间：2014-3-25
 *  修 改 人：
 *  修改时间：
 *****************************************************************************/
bool SysSettings::isCfgFileExist()
{
	Exectime etm(__FUNCTION__);
	//TO DO
	QDir appDir(QCoreApplication::applicationDirPath());

	return appDir.exists(cfgFile);
}



/*****************************************************************************
 *                        恢复出厂设置
 *  函 数 名：slotFactorySet
 *  功    能：
 *  说    明：
 *  参    数：
 *  返 回 值：
 *  创 建 人：L.Wang
 *  创建时间：2014-06-03
 *  修 改 人：
 *  修改时间：
 *****************************************************************************/
void SysSettings::slotFactorySet()
{
	Exectime etm(__FUNCTION__);
	IKMessageBox warnNote(this, QString("警告！"), QString("请确认将设备恢复为单机版或联网版，该操作将清空所有数据"), questionMbx,true);
	IKXmlHelper syscfg(GetXmlFilePath());
	IKXmlHelper::DevWorkMode workmode =syscfg.GetWorkMode();
	if(workmode == IKXmlHelper::DevWorkMode::OFFLINE_MODE)
	{
		warnNote.SetOfflineMode();
	}
	else if(workmode == IKXmlHelper::DevWorkMode::ONLINE_MODE)
	{
		warnNote.SetOnlineMode();
	}


	warnNote.exec();
	if(QDialog::Accepted == warnNote.result())
	{

		if(truncateTables() &&
				deleteUserLocalFiles() &&
				restorConfigures(warnNote.isOnlineMode))
		{
			std::cout << GetLogHead(__FUNCTION__)
				<< "restore factory set successfully!"
				<< std::endl;

			IKMessageBox questionNote(this, QString::fromUtf8("恢复出厂设置"), QString::fromUtf8("操作成功,需要重新启动设备，是否现在重启？"), questionMbx);
			questionNote.exec();

			if(QDialog::Accepted == questionNote.result())
			{
				//emit sigReboot();
				pwMgr.Reboot();
			}
		}
		else
		{
			std::cout << GetLogHead(__FUNCTION__)
				<< "failed to restore factory set!"
				<< std::endl;

			IKMessageBox errNote(this, QString::fromUtf8("恢复出厂设置"), QString::fromUtf8("操作失败！"), errorMbx);
			errNote.exec();
		}
	}
}

/*****************************************************************************
 *                       清空表数据
 *  函 数 名：slotReboot
 *  功    能：重新启动设备
 *  说    明：
 *  参    数：
 *  返 回 值：成功返回true；其他返回false
 *  创 建 人：L.Wang
 *  创建时间：2014-08-12
 *  修 改 人：
 *  修改时间：
 *****************************************************************************/
void SysSettings::slotReboot()
{
	Exectime etm(__FUNCTION__);
	//TO DO
	Interaction::GetInteractionInstance()->CloseUsb();
	int rtnVal = system("shutdown -rf now");
	if(!rtnVal)
	{
		needReboot = true;
	}
}

/*****************************************************************************
 *                       清空表数据
 *  函 数 名：truncateTables
 *  功    能：清空本地数据库中所有表中的用户数据，然后恢复超级用户登陆信息
 *  说    明：
 *  参    数：
 *  返 回 值：成功返回true；其他返回false
 *  创 建 人：L.Wang
 *  创建时间：2014-06-03
 *  修 改 人：
 *  修改时间：
 *****************************************************************************/
bool SysSettings::truncateTables()
{
	Exectime etm(__FUNCTION__);
	DatabaseAccess dbAc;

	//将子增ID设为0；

	// QString sql2("SELECT tablename FROM pg_tables WHERE schemaname = 'public'");
	// QString sql2 ="select setval('person_id_seq',1,false)";
	// QString sqlresetID("select setval('person_id_seq',1,false);");

	QString sql("SELECT tablename FROM pg_tables WHERE schemaname = 'public'");
	std::vector<std::string> tblNames;

	//清空所有数据表
	if(dbSuccess != dbAc.Query(sql.toStdString(), tblNames))
	{
		std::cout << GetLogHead(__FUNCTION__)
			<< "faile to query tablenames information!"
			<< std::endl;
		LOG_ERROR("query sql failed , %s",sql.toStdString().c_str());
		return false;
	}


	//TO DO
	QStringList tblNamesLst;
	for(size_t iter = 0; iter != tblNames.size(); ++iter)
	{
		tblNamesLst << tblNames[iter].c_str();
	}

	if(tblNamesLst.size())
	{
		sql = QString("TRUNCATE TABLE %1").arg(tblNamesLst.join(","));
		dbAc.Query(sql.toStdString());

	}
	else
	{
		std::cout << GetLogHead(__FUNCTION__)
			<< "no table name has been queried!"
			<< std::endl;
		LOG_ERROR("no table name has been queried!");
		return false;
	}

	dbAc.Query("select setval('id',100,false)");
	// dbAc.Query("select setval('person_id_seq',100,false)");

	//初始化超级管理员设置
	if(dbSuccess != dbAc.Query("INSERT INTO operator (operatorid, name, password) VALUES(1, 'admin', '123456'); "
				"INSERT INTO token (persontoken, iristoken, imagetoken) VALUES(0, 0, 0)"))
	{
		std::cout << GetLogHead(__FUNCTION__)
			<< "failed to inited database tables!"
			<< std::endl;
		LOG_ERROR("failed to inited database tables!");
		return false;
	}


	return true;
}

/*****************************************************************************
 *                       删除本地的人脸图像和虹膜图像
 *  函 数 名：deleteUserLocalFiles
 *  功    能：清空本地已注册人员的相关虹膜图像和识别通过后采集的人脸图像
 *  说    明：
 *  参    数：
 *  返 回 值：成功返回true；其他返回false
 *  创 建 人：L.Wang
 *  创建时间：2014-06-03
 *  修 改 人：
 *  修改时间：
 *****************************************************************************/
bool SysSettings::deleteUserLocalFiles()
{
	Exectime etm(__FUNCTION__);
	//TO DO
	//虹膜识别通过的人脸图像目录
	QDir recogFacePhotoPath((QCoreApplication::applicationDirPath(),
				QDir::separator(),
				"recogface"));

	//删除识别成功时采集的人脸图像文件
	if(recogFacePhotoPath.exists())
	{
		if(rmDir(recogFacePhotoPath.absolutePath()))
		{
			LOG_ERROR("faile to delete local face photo captured when identifying!");
			return false;
		}
	}
	else
	{
		LOG_ERROR("no local face photos to delete captured when identifying!");
	}

	//删除log日志
	QDir logPath((QCoreApplication::applicationDirPath(),
				QDir::separator(),
				"log"));
	if(logPath.exists())
	{
		if(rmDir(logPath.absolutePath()))
		{
			LOG_ERROR("faile to delete log files");
			return false;
		}
	}
	else
	{
		LOG_ERROR("no local log to delete !");
	}
	//删除注册图像
	QDir EnrolledImagePath((QCoreApplication::applicationDirPath(),
				QDir::separator(),
				"EnrolledImage"));
	if(EnrolledImagePath.exists())
	{
		if(rmDir(EnrolledImagePath.absolutePath()))
		{
			LOG_ERROR("faile to delete log files");
			return false;
		}
	}
	else
	{
		LOG_ERROR("no local EnrolledImage to delete !");
	}

	//删除识别图像
	QDir idenImagePath((QCoreApplication::applicationDirPath(),
				QDir::separator(),
				"IdenImage"));
	if(idenImagePath.exists())
	{
		if(rmDir(idenImagePath.absolutePath()))
		{
			LOG_ERROR("faile to delete idenImage files");
			return false;
		}
	}
	else
	{
		LOG_ERROR("no local idenImage to delete !");
	}

	return true;
}

/*****************************************************************************
 *                       恢复默认配置信息
 *  函 数 名：restorConfigures
 *  功    能：将系统配置信息恢复到出厂状态
 *  说    明：
 *  参    数：
 *  返 回 值：成功返回true；其他返回false
 *  创 建 人：L.Wang
 *  创建时间：2014-06-03
 *  修 改 人：
 *  修改时间：
 *****************************************************************************/
bool SysSettings::restorConfigures(bool isOnline)
{
	Exectime etm(__FUNCTION__);
	//TO DO
	QString filePath;
	filePath = QString("%1%2%3").arg(QCoreApplication::applicationDirPath(), QDir::separator(), "ikembx00.xml.bak");
	if(QFile::exists(GetXmlFilePath().c_str()))
	{
		if(!QFile::remove(GetXmlFilePath().c_str()))
		{
			std::cout << GetLogHead(__FUNCTION__)
				<< "failed to delete old config file: " << filePath.toStdString()
				<< std::endl;
			LOG_ERROR("failed to delete old config file: %s",filePath.toStdString().c_str());
		}
	}

	QString cpFileCMD(QString("cp -f %1 %2").arg(filePath, GetXmlFilePath().c_str()));
	std::cout << GetLogHead(__FUNCTION__)
		<< "to execute: " << cpFileCMD.toStdString()
		<< std::endl;
	int cmdRes = 0;
	cmdRes = system(cpFileCMD.toStdString().c_str());
	//    IKMessageBox selectNote(this, QString("恢复出厂设置"), QString::fromLocal8Bit("请确认将设备恢复成单机版还是联网版？"), questionMbx,true);
	//    selectNote.exec();
	//   if(QDialog::Accepted == selectNote.result())
	//   {
	IKXmlHelper syscfg(GetXmlFilePath());
	if(isOnline )
	{
		syscfg.SetWorkMode(IKXmlHelper::DevWorkMode::ONLINE_MODE);
	}
	else
	{
		syscfg.SetWorkMode(IKXmlHelper::DevWorkMode::OFFLINE_MODE);
	}

	//   }

	return QFile::exists(filePath);
}

/*****************************************************************************
 *                       删除目录
 *  函 数 名：rmDir
 *  功    能：删除参数指定的目录path
 *  说    明：
 *  参    数：path，指定删除目录
 *  返 回 值：成功返回true；其他返回false
 *  创 建 人：L.Wang
 *  创建时间：2014-06-04
 *  修 改 人：
 *  修改时间：
 *****************************************************************************/
bool SysSettings::rmDir(const QString &path)
{
	Exectime etm(__FUNCTION__);
	std::cout << GetLogHead(__FUNCTION__)
		<< "deleting " << path.toStdString() << "..."
		<< std::endl;
	int  delFileRes = system(QString("rm -rf %1").arg(path).toStdString().c_str());
	QDir dir(path);
	return dir.exists();
}


/*****************************************************************************
 *                        验证密码是否空
 *  函 数 名：checkPwdsEmpty
 *  功    能：
 *  说    明：
 *  参    数：
 *  返 回 值：
 *  创 建 人：L.Wang
 *  创建时间：2014-03-08
 *  修 改 人：
 *  修改时间：
 *****************************************************************************/
bool SysSettings::checkPwdsEmpty()
{
	Exectime etm(__FUNCTION__);
	if(ui->ledtCurPwd->text().isEmpty())
	{
		IKMessageBox infoNote(this, QString::fromUtf8("修改密码"), QString::fromUtf8("请输入当前密码！"), informationMbx);

		infoNote.exec();
		return false;
	}

	if(ui->ledtNewPwd->text().isEmpty())
	{
		IKMessageBox infoNote(this, QString::fromUtf8("修改密码"), QString::fromUtf8("请输入新密码！"), informationMbx);

		infoNote.exec();
		return false;
	}

	if(ui->ledtCheckNewPwd->text().isEmpty())
	{
		IKMessageBox infoNote(this, QString::fromUtf8("修改密码"), QString::fromUtf8("请输入确认密码！"), informationMbx);

		infoNote.exec();
		return false;
	}

	return true;
}

/*****************************************************************************
 *                        验证新密码和确认新密码是否一致
 *  函 数 名：checkNewPwds
 *  功    能：
 *  说    明：
 *  参    数：
 *  返 回 值：
 *  创 建 人：L.Wang
 *  创建时间：2014-03-08
 *  修 改 人：
 *  修改时间：
 *****************************************************************************/
bool SysSettings::checkNewPwds()
{
	Exectime etm(__FUNCTION__);
	if(ui->ledtNewPwd->text().trimmed() != ui->ledtCheckNewPwd->text().trimmed())
	{
		IKMessageBox warnNote(this, QString::fromUtf8("修改密码"), QString::fromUtf8("2次输入的新密码不一致！"), warningMbx);

		warnNote.exec();
		return true;
	}

	return false;
}


/*****************************************************************************
 *                        验证新密码是否一致
 *  函 数 名：checkNewPwdNotSameToOld
 *  功    能：
 *  说    明：
 *  参    数：
 *  返 回 值：
 *  创 建 人：L.Wang
 *  创建时间：2014-03-08
 *  修 改 人：
 *  修改时间：
 *****************************************************************************/
bool SysSettings::checkNewPwdNotSameToOld()
{
	Exectime etm(__FUNCTION__);
	if(ui->ledtNewPwd->text().trimmed() == ui->ledtCurPwd->text().trimmed())
	{
		IKMessageBox warnNote(this, QString::fromUtf8("修改密码"), QString::fromUtf8("新密码和当前密码一致！"), warningMbx);

		warnNote.exec();
		return false;
	}


	return true;
}

/*****************************************************************************
 *                        验证当前密码
 *  函 数 名：checkOldPwd
 *  功    能：
 *  说    明：
 *  参    数：
 *  返 回 值：
 *  创 建 人：L.Wang
 *  创建时间：2013-10-18
 *  修 改 人：
 *  修改时间：
 *****************************************************************************/
bool SysSettings::checkOldPwd()
{
	Exectime etm(__FUNCTION__);
	//TO DO
	DatabaseAccess dbAc;
	std::string query;
	query.append("SELECT count(*) FROM operator \n"
			"WHERE name = ").append("'admin'").append(" AND password = '").append(ui->ledtCurPwd->text().toStdString()).append("'");

	int count = 0;
	EnumReturnValue rtnVal = dbAc.Query(query, count);
	if(dbSuccess != rtnVal || !(count > 0))
	{
		IKMessageBox warnNote(this, QString::fromUtf8("修改密码"), QString::fromUtf8("当前密码错误，身份认证失败！"), warningMbx);

		warnNote.exec();
		return false;
	}


	return true;
}

/*****************************************************************************
 *                        验证当前密码
 *  函 数 名：checkOldPwd
 *  功    能：
 *  说    明：
 *  参    数：
 *  返 回 值：
 *  创 建 人：L.Wang
 *  创建时间：2013-10-18
 *  修 改 人：
 *  修改时间：
 *****************************************************************************/
bool SysSettings::updateNewPwd()
{
	Exectime etm(__FUNCTION__);
	DatabaseAccess dbAc;
	Operator opt;
	opt.password = ui->ledtNewPwd->text().toStdString();
	std::string query("UPDATE operator SET password = :password WHERE name = ");
	query.append("'admin'");


	if(dbSuccess == dbAc.Update(query, opt))
	{
		IKMessageBox infoNote(this, QString::fromUtf8("修改密码"), QString::fromUtf8("操作成功！"), informationMbx);

		infoNote.exec();
		initAdminManage();
		return true;
	}
	else
	{
		IKMessageBox errorNote(this, QString::fromUtf8("修改密码"), QString::fromUtf8("操作失败！"), errorMbx);

		errorNote.exec();
		return false;
	}
}


bool SysSettings::getVolume(int &curVol)
{
	Exectime etm(__FUNCTION__);
	//TO DO
	FILE *fp;

	fp = popen("amixer get Master", "r");
	if(nullptr == fp)
	{
		std::cout << GetLogHead(__FUNCTION__)
			<< "failed to open file for getting system volume"
			<< std::endl;

		LOG_ERROR("failed to open file for getting system volume");
		return false;
	}


	QFile sysVolFile(this);
	if(!sysVolFile.open(fp, QIODevice::ReadOnly))
	{
		std::cout << GetLogHead(__FUNCTION__)
			<< "failed to open file for getting system volume"
			<< std::endl;

		LOG_ERROR("failed to open file for getting system volume");
		return false;
	}

	curVol = -1;
	QTextStream sysVolStream(&sysVolFile);
	QString line;
	QRegExp volExpr("(\\d{1,3})(?:\\%)");
	while(!sysVolStream.atEnd())
	{
		line = sysVolStream.readLine();
		int pos = line.indexOf(volExpr);

		if(pos > 0 && pos != line.length())
		{
			curVol = volExpr.cap(1).toInt();
		}
	}

	return curVol >= 0;
}


/*****************************************************************************
 *                        刷新页面
 *  函 数 名：refresh
 *  功    能：
 *  说    明：
 *  参    数：
 *  返 回 值：
 *  创 建 人：L.Wang
 *  创建时间：2013-10-18
 *  修 改 人：
 *  修改时间：
 *****************************************************************************/
void SysSettings::refresh()
{
	Exectime etm(__FUNCTION__);
	initUI();
}


/*****************************************************************************
 *                        同步服务器时间线程
 *  函 数 名：threadAsyncTime
 *  功    能：同步服务器时间，通过网络通讯类想服务器发送时间同步请求
 *  说    明：由于同步时间请求的内部实现为阻塞方式，为避免界面冻结，采用单独线程进行处理
 *  参    数：ins, SysSettings实例对象指针
 *  返 回 值：
 *  创 建 人：L.Wang
 *  创建时间：2013-10-18
 *  修 改 人：
 *  修改时间：
 *****************************************************************************/
void* SysSettings::threadAsyncTime(void *ins)
{
	Exectime etm(__FUNCTION__);
	//向服务器发送同步时间请求
	IkSocket* s = IkSocket::GetIkSocketInstance();
	s && s->AsyncDevTime();

	return (void *)0;
}



void SysSettings::on_isSetCity_clicked(bool checked)
{
	Exectime etm(__FUNCTION__);
	ui->cityNameLine->setEnabled(checked);
	m_isFromNet = !checked;

}

bool SysSettings::isCityExist(QString &cityName)
{
	Exectime etm(__FUNCTION__);
	QString cityCode;

	QFile cityInf("./citycodeutf8.txt");

	if (!cityInf.open(QIODevice::ReadOnly | QIODevice::Text))
	{
		qDebug()<< cityInf.errorString();
	}

	while (!cityInf.atEnd())
	{
		QString strc = cityInf.readLine().trimmed();
		QString name  = strc.mid(10).toLocal8Bit();
		QString name2 = name+QString::fromLocal8Bit("市");

		if (name == cityName || name2 == cityName )
		{
			cityName = name;
			cityCode = strc.left(9);
			cityInf.close();
			return true;
		}

	}

	cityInf.close();
	return false;
}

