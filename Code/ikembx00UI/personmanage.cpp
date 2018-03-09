/*****************************************************************************
 ** 文 件 名：personmanage.cpp
 ** 主 要 类：PersonManage
 **
 ** Copyright (c) 中科虹霸有限公司
 **
 ** 创 建 人：L.Wang
 ** 创建时间：20013-10-18
 **
 ** 修 改 人：
 ** 修改时间：
 ** 描  述:   人员管理
 ** 主要模块说明: 人员查询，排序，增加，修改，删除人员信息及虹膜注册，
 **
 ** 版  本:   1.0.0
 ** 备  注:
 **
 *****************************************************************************/
#include <QMessageBox>
#include <QSqlDatabase>
#include <QSqlTableModel>
#include <QSqlQueryModel>
#include <QSqlQuery>
#include <QSqlRelationalTableModel>
#include <QSqlError>
#include <QFile>
#include <QKeyEvent>

#include "ikmessagebox.h"
#include "personmanage.h"
#include "ui_personmanage.h"
#include "dbconnection.h"
#include "detailpersoninfo.h"
#include "enroll.h"
#include "../common.h"
#include "../../IKDatabaseLib/include/IKDatabaseLib.h"
#include "../../IKDatabaseLib/include/IKTransaction.h"
#include "../Common/Exectime.h"

//Network communication relevant head files
#include "iksocket.h"
#include "CodecNetData.h"
#include "dboperationinterface.h"
#include "sysnetinterface.h"
#include "commononqt.h"
#include "../Common/Logger.h"


PersonManage::PersonManage(QWidget *parent) :
	QDialog(parent),
	ui(new Ui::PersonManage),
	currentPage(1),
	totalPage(1),
	pageInfo(QString::fromUtf8("第%1/%2页")),
	model(nullptr),
	headView(nullptr),
	orderBy("worksn"),
	orderDirection("ASC"),
	isFindOpt(false),
	isNetAvailable(true),
	socket(nullptr),
	enroll(nullptr),
	isFindByManual(true)
{
	Exectime etm(__FUNCTION__);
	ui->setupUi(this);


	//隐藏对话框标题栏及对话框位置调整和背景填充
	setWindowFlags(Qt::FramelessWindowHint);
	setGeometry(QRect(0, 0, 1024, 600));

	//设置选中行的背景颜色
	ui->twgtPerson->setStyleSheet("QTableView::item:selected{"
			"color:white;"
			"background:rgb(108, 166, 205);"
			"}");

	//查询条件输入栏为默认焦点位置
	ui->txtKeyWords->setFocus();

	//初始化网络通讯业务处理对象
	socket = IkSocket::GetIkSocketInstance();

	//初始化数据视图
	initView();
	//初始化注册页面
	initEnrollUI();

	//信号与槽的连接
	connect(ui->btnBack, SIGNAL(clicked()), this, SIGNAL(sigGoBack()));   //返回按钮
	connect(ui->btnAdd, SIGNAL(clicked(bool)), this, SLOT(slotAddNew()));     //增加人员
	connect(ui->btnEdit, SIGNAL(clicked()), this, SLOT(slotEdit()));      //修改人员
	connect(ui->btnDelete, SIGNAL(clicked()), this, SLOT(slotDelete()));  //删除人员
	connect(ui->btnFind, SIGNAL(clicked()), this, SLOT(slotFind()));      //查询人员
	connect(ui->btnEnrollIris, SIGNAL(clicked()), this, SLOT(slotEnrollIris()));      //注册虹膜
	connect(ui->btnHeadPage, SIGNAL(clicked()), this, SLOT(slotHeadPage()));         //首页
	connect(ui->btnPreviousPage, SIGNAL(clicked()), this, SLOT(slotPreviousPage()));  //前一页
	connect(ui->btnNextPage, SIGNAL(clicked()), this, SLOT(slotNextPage()));          //下一页
	connect(ui->btnTailPage, SIGNAL(clicked()), this, SLOT(slotTailPage()));          //尾页
	connect(ui->twgtPerson->horizontalHeader(), SIGNAL(sortIndicatorChanged(int,Qt::SortOrder)),
			this, SLOT(slotOrderBy(int, Qt::SortOrder)));                  //人员排序
}


PersonManage::~PersonManage()
{
	Exectime etm(__FUNCTION__);
	if(nullptr != enroll)
	{
		delete enroll;
	}

	delete model;
	delete ui;
}


/*****************************************************************************
 *                        初始化人员表UI
 *  函 数 名：initView
 *  功    能：完成显示人员表数据的QTableWidget初始化
 *  说    明：
 *  参    数：
 *  返 回 值：成功返回true， 否则返回false
 *  创 建 人：L.Wang
 *  创建时间：2013-10-18
 *  修 改 人：
 *  修改时间：
 *****************************************************************************/
bool PersonManage::initView()
{
	Exectime etm(__FUNCTION__);
	//获取满足查询条件的记录总数
	QStringList headerLabels;
	headerLabels << QString::fromUtf8("人员编号") << QString::fromUtf8("工号") << QString::fromUtf8("姓名")
		<< QString::fromUtf8("性别") << QString::fromUtf8("身份证号") << QString::fromUtf8("部门")
		<< QString::fromUtf8("虹膜") << QString::fromUtf8("ID卡号") << QString::fromUtf8("备注");

	//设置列数
	ui->twgtPerson->setColumnCount(headerLabels.size());
	//设置表头
	ui->twgtPerson->setHorizontalHeaderLabels(headerLabels);
	//设置水平标题样式
	ui->twgtPerson->horizontalHeader()->setStyleSheet("QHeaderView{background-color: rgb(32, 90, 160); color: rgb(255, 255, 255); font:16pt};");
	//设置垂直标题样式
	ui->twgtPerson->verticalHeader()->setStyleSheet("QHeaderView{background-color: rgb(32, 90, 160); width: 50px; color: rgb(255, 255, 255); font:16pt};");
	//设置水平表头不塌陷
	ui->twgtPerson->horizontalHeader()->setHighlightSections(false);
	//设置选择约束
	ui->twgtPerson->setSelectionBehavior(QAbstractItemView::SelectRows);   //设置选择模式，最小选中单位为行
	ui->twgtPerson->setSelectionMode(QAbstractItemView::SingleSelection);  //每次仅允许选择一行



	//设置编辑触发条件
	ui->twgtPerson->setEditTriggers(QAbstractItemView::NoEditTriggers);
	//表格行背景颜色交变
	ui->twgtPerson->setAlternatingRowColors(true);
	//ui->twgtPerson->setBackgroundColor();

	//设置行高
	ui->twgtPerson->verticalHeader()->setDefaultSectionSize(ui->twgtPerson->height() + 8);
	//设置垂直表头不塌陷
	ui->twgtPerson->verticalHeader()->setHighlightSections(false);
	//设置列宽适应文本大小
	ui->twgtPerson->resizeColumnsToContents();
	//设置最后一列拉伸
	ui->twgtPerson->horizontalHeader()->setStretchLastSection(true);


	//隐藏人员编号列
	ui->twgtPerson->setColumnHidden(0, true);

     //大军从新疆发来的修改 begin
    //隐藏身份证号列
    //@ui->twgtPerson->setColumnHidden(4, true);
    //隐藏备注
    ui->twgtPerson->setColumnHidden(8, true);
    //隐藏部门
    ui->twgtPerson->setColumnHidden(5, true);
    //隱藏工号
    ui->twgtPerson->setColumnHidden(1, true);
    //大军从新疆发来的修改 end

	//填充数据
	showData();

	return true;
}


/*****************************************************************************
 *                        初始化注册模块
 *  函 数 名：initEnrollUI
 *  功    能：完成虹膜注册前的模块初始化
 *  说    明：
 *  参    数：
 *  返 回 值：成功返回true， 否则返回false
 *  创 建 人：L.Wang
 *  创建时间：2013-10-18
 *  修 改 人：
 *  修改时间：
 *****************************************************************************/
bool PersonManage::initEnrollUI()
{
	Exectime etm(__FUNCTION__);
	if(nullptr == enroll)
	{
		enroll = new  Enroll(this);
		enroll->hide();

		if(nullptr != enroll)
		{
			connect(enroll, SIGNAL(sigEnrollResult(bool)), this, SLOT(slotGetEnrollResult(bool)));    //关联注册虹膜结果信号与槽
		}
	}

	return nullptr != enroll;
}

/*****************************************************************************
 *                        删除指定人员
 *  函 数 名：slotDelete
 *  功    能：槽函数，删除人员操作处理函数
 *  说    明：
 *  参    数：
 *  返 回 值：
 *  创 建 人：L.Wang
 *  创建时间：2013-10-18
 *  修 改 人：
 *  修改时间：
 *****************************************************************************/
void PersonManage::slotDelete()
{
	Exectime etm(__FUNCTION__);
	//TO DO
	if(!WorkMode::IsSingleWorkMode() && !(socket && socket->IsNetOK()))
	{
		IKMessageBox errNote(this, QString::fromUtf8("删除人员"), QString::fromUtf8("当前无法连接到服务器，请确认设备网络后重试！"), errorMbx);
		errNote.exec();

		return;
	}

	deleteSelected(nullptr);

	IkSocket::GetIkSocketInstance()->UploadTemp();

	return;
}

/*****************************************************************************
 *                        删除指定人员
 *  函 数 名：slotDelete
 *  功    能：槽函数，删除人员操作处理函数
 *  说    明：
 *  参    数：
 *  返 回 值：
 *  创 建 人：L.Wang
 *  创建时间：2013-10-18
 *  修 改 人：
 *  修改时间：
 *****************************************************************************/
int PersonManage::deleteSelected(const QItemSelectionModel* selMdl)
{

	Exectime etm(__FUNCTION__);
	//判断当前是否选择了要删除的记录
	if(!ui->twgtPerson->currentIndex().isValid())
	{
		IKMessageBox infoNote(this, QString::fromUtf8("删除人员信息"), QString::fromUtf8("请先选择要删除的人员！"), informationMbx);

		infoNote.exec();
		return 0;
	}

    //询问用户删除确认
	IKMessageBox questionNote(this, QString::fromUtf8("删除人员信息"), QString::fromUtf8("确定要删除吗?"), questionMbx);

	questionNote.exec();
	if(QDialog::Accepted == questionNote.result())
	{
		DatabaseAccess dbAc;

        QString pid = ui->twgtPerson->item(ui->twgtPerson->currentRow(), 0)->text();
		//        //获取要删除人员的personID
		//        PersonImage perImg;
		//        QString sql = QString("SELECT * FROM personimage WHERE personid = '%1'").arg(pid);
		//        dbAc.Query(sql.toStdString(), perImg);

		//        //查询待删除虹膜信息
		//        IrisDataBase irisRec;
		//        sql = QString("SELECT * FROM irisdatabase WHERE personid = '%1'").arg(pid);
		//        dbAc.Query(sql.toStdString(), irisRec);


		//        Transaction trn(*(dbAc.GetSession()));
		//        //删除人员信息
		//        QString query("DELETE FROM irisdatabase WHERE personid = '%1';\n"
		//                      "DELETE FROM personimage WHERE personid = '%1';\n"
		//                      "DELETE FROM person WHERE personid = '%1';\n"
		//                      "INSERT INTO tempirisdatabase (personid, irisdataid, operation) VALUES('%1', '%2', 1);\n"
		//                      "INSERT INTO temppersonimage (personid, imageid, operation) VALUES('%1', '%3', 1);\n"
		//                      "INSERT INTO tempperson (personid, operation) VALUES('%1', 1);\n");
		//        query = query.arg(pid, irisRec.irisDataID.Unparse().c_str(), perImg.imageID.Unparse().c_str());

		Transaction trn(*(dbAc.GetSession()));
		if(deleteIrisData(dbAc, pid) && deletePersonImage(dbAc, pid) && deletePersonInfo(dbAc, pid))
		{
            trn.Commit();

            // Add by liushu del memfeature
            int nmemret = 0;
            {
                InfoData    infodata;
                string personid = pid.toStdString();
                UuidControl _uuid;
                uuid_t ud;
                _uuid.UuidFromString(personid,ud);
                uuid_parse(personid.c_str(), infodata.PeopleData.Id);
                nmemret = SingleControl<IrisManager>::CreateInstance().DeletePersonAndFeatureData(infodata);
                LOG_INFO("Local DeletePersonAndFeatureData: pid: [%s] result: %d",personid.c_str(),nmemret);
            }
		}
		else
		{
			trn.Rollback();
		}

		//测试人员信息网络接口
		//        std::vector<std::string> imgIDs;
		//        imgIDs.push_back(perImg.imageID.Unparse());

		//        std::vector<std::string> perIDs;
		//        perIDs.push_back(pid.toStdString());

		//        PersonInfoInterface::DeleteFaceImg(imgIDs);
		//        PersonInfoInterface::DeletePersonInfo(perIDs);

		//        //刷新数据
		//        refresh();
		getSpecifiedPage(currentPage);
	}


	return 1;       //删除人员信息的记录数
}

/*****************************************************************************
 *                        增加新人员信息
 *  函 数 名：slotAddNew
 *  功    能：槽函数，增加人员操作处理函数
 *  说    明：
 *  参    数：
 *  返 回 值：
 *  创 建 人：L.Wang
 *  创建时间：2013-10-18
 *  修 改 人：
 *  修改时间：
 *****************************************************************************/
void PersonManage::slotAddNew()
{
	Exectime etm(__FUNCTION__);
	//TO DO
	if(!WorkMode::IsSingleWorkMode() && !(socket && socket->IsNetOK()))
	{
		IKMessageBox errNote(this, QString::fromUtf8("添加人员"), QString::fromUtf8("当前无法连接到服务器，请确认设备网络后重试！"), errorMbx);
		errNote.exec();

		return;
	}

	DetailPersonInfo perInfo(this);
	//进入省电模式前，强行关闭增加人员信息页面
	connect(this, SIGNAL(sigSetInActive()), &perInfo, SLOT(slotBack()));
	connect(&perInfo, SIGNAL(sigNewPersonAdded(QString)), this, SLOT(slotNewPersonAdded(QString)));
	perInfo.exec();
}

void PersonManage::slotNewPersonAdded(QString perID)
{
	Exectime etm(__FUNCTION__);
	dispPageHasGivenRecord(perID.toStdString(), pageSize);
}


/*****************************************************************************
 *                        编辑人员信息
 *  函 数 名：slotEdit
 *  功    能：槽函数，编辑人员信息操作处理函数
 *  说    明：
 *  参    数：
 *  返 回 值：
 *  创 建 人：L.Wang
 *  创建时间：2013-10-18
 *  修 改 人：
 *  修改时间：
 *****************************************************************************/
void PersonManage::slotEdit()
{
	Exectime etm(__FUNCTION__);
	//TO DO
	if(!WorkMode::IsSingleWorkMode() && !(socket && socket->IsNetOK()))
	{
		IKMessageBox errNote(this, QString::fromUtf8("修改人员"), QString::fromUtf8("当前无法连接到服务器，请确认设备网络后重试！"), errorMbx);
		errNote.exec();

		return;
	}


	if(!ui->twgtPerson->currentIndex().isValid())
	{
		IKMessageBox infoNote(this, QString::fromUtf8("修改人员信息"), QString::fromUtf8("请先选择要修改的人员！"), informationMbx);

		infoNote.exec();
		return;
	}

	//Cache the current row
	int curRow = ui->twgtPerson->currentRow();

	//获取要修改人员的信息
	PersonInfo personInfo;
	personInfo.id = ui->twgtPerson->item(ui->twgtPerson->currentRow(), 0)->text().toStdString();
	personInfo.name = ui->twgtPerson->item(ui->twgtPerson->currentRow(), 2)->text().toStdString();
	personInfo.depart = ui->twgtPerson->item(ui->twgtPerson->currentRow(), 5)->text().toStdString();


	//切换人员详细信息编辑页面
	DetailPersonInfo detailPersonInfo(personInfo, this);

	//进入省电模式时，强行关闭修改人员信息页面
	connect(this, SIGNAL(sigSetInActive()), &detailPersonInfo, SLOT(slotBack()));
	detailPersonInfo.exec();


	//显示修改后该记录所在的页，并标记该记录
	dispPageHasGivenRecord(personInfo.id, pageSize);
}

/*****************************************************************************
 *                        注册虹膜
 *  函 数 名：slotEnrollIris
 *  功    能：槽函数， 为指定人员注册虹膜操作处理函数
 *  说    明：
 *  参    数：
 *  返 回 值：
 *  创 建 人：L.Wang
 *  创建时间：2013-10-18
 *  修 改 人：
 *  修改时间：
 *****************************************************************************/
void PersonManage::slotEnrollIris()
{
	Exectime etm(__FUNCTION__);
	//TO DO
	if(0 == totalPage)
	{
		IKMessageBox infoNote(this, QString::fromUtf8("注册虹膜"), QString::fromUtf8("请先添加人员！"), informationMbx);

		infoNote.exec();
		return;
	}

	if(!ui->twgtPerson->currentIndex().isValid())
	{
		IKMessageBox infoNote(this, QString::fromUtf8("注册虹膜"), QString::fromUtf8("请先选择要注册虹膜的人员！"), informationMbx);

		infoNote.exec();
		return;
	}



	//初始化注册模块
	if(!initEnrollUI())
	{
		IKMessageBox errorNote(this, QString::fromUtf8("启动人员注册"),  QString::fromUtf8("无法获取注册虹膜所需的内存资源！"), errorMbx);

		errorNote.exec();
		return;
	}


	//获取待注册虹膜的人员信息    
	person.id = ui->twgtPerson->item(ui->twgtPerson->currentRow(), 0)->text().toStdString();
	person.name = ui->twgtPerson->item(ui->twgtPerson->currentRow(), 2)->text().toStdString();
	person.depart = ui->twgtPerson->item(ui->twgtPerson->currentRow(), 5)->text().toStdString();

	person.hasEnrolledIris = false;
	if(ui->twgtPerson->item(ui->twgtPerson->currentRow(),6)->text().contains("是"))
	{
		person.hasEnrolledIris = true;
	}


	//设置当前注册人员
	PersonInfo personInfo(person);
	enroll->SetPersonInfo(&personInfo);

	//显示注册页面
	enroll->exec();
}


/*****************************************************************************
 *                        查询人员信息
 *  函 数 名：slotFind
 *  功    能：槽函数，查询人员信息
 *  说    明：支持模糊查询
 *  参    数：
 *  返 回 值：
 *  创 建 人：L.Wang
 *  创建时间：2013-10-18
 *  修 改 人：
 *  修改时间：
 *****************************************************************************/
void PersonManage::slotFind()
{
	Exectime etm(__FUNCTION__);
	//TO DO
	if(isFindByManual)
	{
		isFindOpt = true;
	}
	getSpecifiedPage(currentPage = 1);
	isFindOpt = false;
	isFindByManual = true;
}

/*****************************************************************************
 *                        浏览首页人员信息
 *  函 数 名：slotHeadPage23
 *  功    能：查看首页人员信息
 *  说    明：
 *  参    数：
 *  返 回 值：
 *  创 建 人：L.Wang
 *  创建时间：2013-10-18
 *  修 改 人：
 *  修改时间：
 *****************************************************************************/
void PersonManage::slotHeadPage()
{
	Exectime etm(__FUNCTION__);
	//TO DO
	if(1 >= currentPage)
	{
		IKMessageBox infoNote(this, QString::fromUtf8("浏览数据"), QString::fromUtf8("当前页已经是首页！"), informationMbx);

		infoNote.exec();
		return;
	}

	getSpecifiedPage(currentPage = 1);
}

/*****************************************************************************
 *                        查看上一页人员信息
 *  函 数 名：slotPreviousPage
 *  功    能：槽函数，查看上一页人员信息
 *  说    明：
 *  参    数：
 *  返 回 值：
 *  创 建 人：L.Wang
 *  创建时间：2013-10-18
 *  修 改 人：
 *  修改时间：
 *****************************************************************************/
void PersonManage::slotPreviousPage()
{
	Exectime etm(__FUNCTION__);
	//TO DO
	if(1 >= currentPage)
	{
		IKMessageBox infoNote(this, QString::fromUtf8("浏览数据"), QString::fromUtf8("当前页已经是首页！"), informationMbx);

		infoNote.exec();
		return;
	}

	getSpecifiedPage(--currentPage);
}

/*****************************************************************************
 *                        查看下一页人员信息
 *  函 数 名：slotNextPage
 *  功    能：槽函数，查看下一页人员信息
 *  说    明：
 *  参    数：
 *  返 回 值：
 *  创 建 人：L.Wang
 *  创建时间：2013-10-18
 *  修 改 人：
 *  修改时间：
 *****************************************************************************/
void PersonManage::slotNextPage()
{
	Exectime etm(__FUNCTION__);
	//TO DO
	if(totalPage <= currentPage)
	{
		IKMessageBox infoNote(this, QString::fromUtf8("浏览数据"), QString::fromUtf8("当前页已经是尾页！"), informationMbx);

		infoNote.exec();
		return;
	}

	getSpecifiedPage(++currentPage);
}

/*****************************************************************************
 *                        查看尾页人员信息
 *  函 数 名：slotTailPage
 *  功    能：槽函数，查看人员信息尾页
 *  说    明：
 *  参    数：
 *  返 回 值：
 *  创 建 人：L.Wang
 *  创建时间：2013-10-18
 *  修 改 人：
 *  修改时间：
 *****************************************************************************/
void PersonManage::slotTailPage()
{
	Exectime etm(__FUNCTION__);
	//TO DO
	if(totalPage <= currentPage)
	{
		IKMessageBox infoNote(this, QString::fromUtf8("浏览数据"), QString::fromUtf8("当前页已经是尾页！"), informationMbx);

		infoNote.exec();
		return;
	}


	getSpecifiedPage(currentPage = totalPage);
}

/*****************************************************************************
 *                        获得虹膜注册结果
 *  函 数 名：slotGetEnrollResult
 *  功    能：槽函数，获得虹膜注册结果处理函数
 *  说    明：
 *  参    数：
 *  返 回 值：
 *  创 建 人：L.Wang
 *  创建时间：2013-10-18
 *  修 改 人：
 *  修改时间：
 *****************************************************************************/
void PersonManage::slotGetEnrollResult(bool isSucceed)
{
	Exectime etm(__FUNCTION__);
	//TO DO
	if(!isSucceed)
	{
		//TO DO
		LOG_ERROR("EnrollResult failed.");
		return;
	}


	//Cache the current row
	int curRow = ui->twgtPerson->currentRow();

	//刷新数据
	showData();

	//Reselect the row previous selected
	ui->twgtPerson->selectRow(curRow);
}

/*****************************************************************************
 *                        捕获排序字段选项
 *  函 数 名：slotOrderBy
 *  功    能：
 *  说    明：
 *  参    数：
 *  返 回 值：
 *  创 建 人：L.Wang
 *  创建时间：2013-10-18
 *  修 改 人：
 *  修改时间：
 *****************************************************************************/
void PersonManage::slotOrderBy(int index, Qt::SortOrder orderStyle)
{
	Exectime etm(__FUNCTION__);
	//TO DO
	//获取排序字段
	switch(index)
	{
		case 1:
			orderBy = QString("worksn");
			break;
		case 2:
			orderBy = QString("name");
			break;
		case 3:
			orderBy = QString("sex");
			break;
		case 4:
			orderBy = QString("idcard");
			break;
		case 5:
			orderBy = QString("t1.departname");
			break;
		case 6:
			orderBy = QString("hasirisdata");
			break;
		case 7:
			orderBy = QString("cardid");
			break;
		case 8:
			orderBy = QString("t1.memo");
			break;
		default:
			orderBy = QString("worksn");
			break;
	}

	//排序方式
	if(Qt::AscendingOrder == orderStyle)
	{
		orderDirection = "ASC";
	}
	else
	{
		orderDirection = "DESC";
	}

	//刷新数据显示排序后的首页
	isFindByManual  =  false;
	QKeyEvent mokeKeyPressed(QEvent::KeyPress, Qt::Key_Backspace, Qt::NoModifier);
	QCoreApplication::sendEvent(this, &mokeKeyPressed);
}

/*****************************************************************************
 *                        刷新人员管理界面
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
void PersonManage::refresh()
{
	Exectime etm(__FUNCTION__);
	QDialog::show();

	//清空查询条件
	ui->txtKeyWords->clear();

	//设置默认排序规则
	orderBy = "worksn";
	orderDirection = "ASC";
	ui->twgtPerson->horizontalHeader()->setSortIndicator(1, Qt::AscendingOrder);

	//刷新数据并跳转到首页
	getSpecifiedPage(currentPage = 1);

	//设置搜索关键字输入框获得默认焦点
	ui->txtKeyWords->setFocus();

	std::cout << "[personmanage page]:show() was called" << std::endl;
}


/*****************************************************************************
 *                        监测网络状态
 *  函 数 名：slotNetworkStateChanged
 *  功    能：被动监测网络状态，如果不可用，则不能对人员信息进行操作
 *  说    明：
 *  参    数：availableState,网络是否可用，为true则可用，为false则不可用
 *  返 回 值：void
 *  创 建 人：L.Wang
 *  创建时间：2014-05-04
 *  修 改 人：
 *  修改时间：
 *****************************************************************************/
void PersonManage::slotNetworkStateChanged(bool availableState)
{
	Exectime etm(__FUNCTION__);
	isNetAvailable = availableState;
}


void PersonManage::reject()
{
	Exectime etm(__FUNCTION__);
	//DO NOTHING
}


/*****************************************************************************
 *                        显示人员信息
 *  函 数 名：showData
 *  功    能：显示条件查询人员信息
 *  说    明：
 *  参    数：
 *  返 回 值：
 *  创 建 人：L.Wang
 *  创建时间：2013-10-18
 *  修 改 人：
 *  修改时间：
 *****************************************************************************/
void PersonManage::showData(QWidget *parent)
{
	Exectime etm(__FUNCTION__);
	//TO DO
	getSpecifiedPage(currentPage);
}


/*****************************************************************************
 *                        获取条件查询的所有记录
 *  函 数 名：queryAll
 *  功    能：初始化人员信息视图
 *  说    明：获取符合条件的所有记录数，并根据显示规则计算出记录的总页数
 *  参    数：
 *  返 回 值：成功返回所有满足查询条件记录数，失败返回-1
 *  创 建 人：L.Wang
 *  创建时间：2013-10-18
 *  修 改 人：
 *  修改时间：2014-04-29
 *****************************************************************************/
int PersonManage::queryAll()
{
	Exectime etm(__FUNCTION__);
	//TO DO
	QString querySql("SELECT count(*) "
			"FROM V_Person_Detail AS t1 "             //数据源表
			"WHERE  t1.worksn LIKE '%%1%' "    //查询条件
			"   OR  t1.name LIKE '%%1%' "
			"   OR  t1.sex LIKE '%%1%' "
            "   OR  t1.idcard LIKE '%%1%' "         // 大军从新疆发来的修改，取消注释
			"   OR  t1.departname LIKE '%%1%' "
			"   OR  t1.hasirisdata LIKE '%%1%' "
			"   OR  t1.cardid LIKE '%%1%' "
			//                   "   OR  t1.disabletime = '%%1%' "
			"   OR  t1.memo LIKE '%%1%' ");
	//注入查询条件
	querySql = querySql.arg(ui->txtKeyWords->text());




	//获取查询结果
	int totalRecords = 0;
	DatabaseAccess dbAc;
	EnumReturnValue rtnVal = dbAc.Query(querySql.toStdString(), totalRecords);

	//根据查询结果确定是禁止还是允许排序
	ui->twgtPerson->horizontalHeader()->setEnabled(totalRecords > 0);

	if(dbSuccess == rtnVal)
	{
		if(totalRecords)
		{
			totalPage = totalRecords / pageSize + (totalRecords % pageSize > 0 ? 1: 0);
		}

		return totalRecords;
	}
	else
	{
		LOG_ERROR("Query sql failed.%s",querySql.toStdString().c_str());
		return -1;
	}
}


/*****************************************************************************
 *                        获取指定的分页数据
 *  函 数 名：getSpecifiedPage
 *  功    能：获取指定的分页数据
 *  说    明：支持模糊查询，任何字段中包含该关键字都将在查询结果中体现出来
 *  参    数：toPageNum， 跳转到的页码
 *  返 回 值：成功返回true，其他返回false
 *  创 建 人：L.Wang
 *  创建时间：2013-10-18
 *  修 改 人：
 *  修改时间：
 *****************************************************************************/
bool PersonManage::getSpecifiedPage(const int toPageNum)
{
	Exectime etm(__FUNCTION__);
	//TO DO
	//清空当前人员信息数据
	ui->twgtPerson->setRowCount(0);

	//查询指定域的人员数据
	std::vector<PersonDetail> persons;

	if(!findBy(ui->txtKeyWords->text(), persons) && isFindOpt)
	{
		IKMessageBox errorNote(this, QString::fromUtf8("查询"), QString::fromUtf8("操作失败！"), errorMbx);
		errorNote.exec();

		return false;
	}


	if(0 == persons.size() && isFindOpt)
	{
		IKMessageBox warnNote(this, QString::fromUtf8("查询"), QString::fromUtf8("无符合条件的记录！"), warningMbx);

		warnNote.exec();
		return false;
	}


	//获取查询结果中的记录数
	dispData(persons.begin() + (toPageNum - 1) * pageSize, min(persons.begin() + toPageNum * pageSize, persons.end()));
	currentPage = toPageNum;

	return true;
}



/*****************************************************************************
 *                        显示查询结果
 *  函 数 名：dispData
 *  功    能：显示给定的数据
 *  说    明：
 *  参    数：persons， 待显示的数据集；
 *           begin,待显示数据的起始显示数据；
 *           end待显示数据集的结尾显示数据
 *  返 回 值：
 *  创 建 人：L.Wang
 *  创建时间：2014-08-15
 *  修 改 人：
 *  修改时间：
 *****************************************************************************/
	template<typename InputIterator>
void PersonManage::dispData(InputIterator beg, InputIterator end)
{
	Exectime etm(__FUNCTION__);
	//TO DO
	for(int offset = 0; offset < pageSize && beg != end; offset++)
	{
		PersonDetail &person = *beg++;

		ui->twgtPerson->insertRow(offset);
		ui->twgtPerson->setItem(offset, 0, new QTableWidgetItem(QString::fromUtf8("%1").arg(person.personID.Unparse().c_str()).trimmed()));
		ui->twgtPerson->setItem(offset, 1, new QTableWidgetItem(QString::fromUtf8("%1").arg(person.workSn.c_str()).trimmed()));
		ui->twgtPerson->setItem(offset, 2, new QTableWidgetItem(QString::fromUtf8("%1").arg(person.name.c_str()).trimmed()));
		ui->twgtPerson->setItem(offset, 3, new QTableWidgetItem(QString::fromUtf8("%1").arg(person.sex.c_str()).trimmed()));

		ui->twgtPerson->setItem(offset, 4, new QTableWidgetItem(QString::fromUtf8("%1").arg(person.IDcard.c_str()).trimmed()));
		ui->twgtPerson->setItem(offset, 5, new QTableWidgetItem(QString::fromUtf8("%1").arg(person.departName.c_str()).trimmed()));
		ui->twgtPerson->setItem(offset, 6, new QTableWidgetItem(QString::fromUtf8("%1").arg(person.hasIrisData.c_str()).trimmed()));


		ui->twgtPerson->setItem(offset, 7, new QTableWidgetItem(QString::fromUtf8("%1").arg(person.cardID.c_str()).trimmed()));
		ui->twgtPerson->setItem(offset, 8, new QTableWidgetItem(QString::fromUtf8("%1").arg(person.memo.c_str()).trimmed()));
	}

	//设置列宽适应文本大小
	ui->twgtPerson->resizeColumnsToContents();
	//设置最后一列拉伸
	ui->twgtPerson->horizontalHeader()->setStretchLastSection(true);
	//设置翻页状态
	ui->labPageInfo->setText(pageInfo.arg(currentPage).arg(totalPage));
}

bool PersonManage::deleteIrisData(DatabaseAccess& dbConn, const QString &pid)
{
	Exectime etm(__FUNCTION__);
	//TO DO
	std::vector<IrisDataBase> irisDatas;

	if(dbSuccess != dbConn.Query(QString("SELECT * FROM irisdatabase WHERE personid = '%1'").arg(pid).toStdString(), irisDatas))
	{
		LOG_ERROR("Query sql failed. SELECT * FROM irisdatabase WHERE personid = '%1'");
		return false;
	}

	if(0 == irisDatas.size())
	{
		return true;
	}

	QStringList sqls;
	sqls << QString("DELETE FROM irisdatabase WHERE personid = '%1';").arg(pid);

	for(size_t iter = 0; iter < irisDatas.size(); ++iter)
	{
		sqls << QString(" INSERT INTO tempirisdatabase (personid, irisdataid, operation)"
				" VALUES('%1', '%2', 1);").arg(pid, irisDatas[iter].irisDataID.Unparse().c_str());
	}

	return dbSuccess == dbConn.Query(sqls.join("\n").toStdString());
}


bool PersonManage::deletePersonImage(DatabaseAccess& dbConn, const QString &pid)
{
	Exectime etm(__FUNCTION__);
	//TO DO
	std::vector<PersonImage> perImgs;
	QString sql("SELECT * FROM personimage WHERE personid = '%1';");

	if(dbSuccess != dbConn.Query(sql.arg(pid).toStdString(), perImgs))
	{
		LOG_ERROR("Query sql,%s",sql.toStdString().c_str());
		return false;
	}

	if(0 == perImgs.size())
	{
		return true;
	}

	sql = QString("DELETE FROM personimage WHERE personid = '%1';\n"
			"INSERT INTO temppersonimage (personid, imageid, operation) VALUES('%1', '%2', 1);\n");

	return dbSuccess == dbConn.Query(sql.arg(pid, perImgs[0].imageID.Unparse().c_str()).toStdString());
}

//暂未使用，由于没有一个好的方案来处理对应与删除识别记录之后和服务器交换信息
bool PersonManage::deleteIdenRecord(DatabaseAccess &dbConn, const QString &pid)
{
	Exectime etm(__FUNCTION__);
	//TO DO
	QString sql("DELETE FROM personreclog WHERE personid = '%1';\n"
			/*"INSERT INTO tempperson (personid, operation) VALUES('%1', 1);\n"*/);

	return dbSuccess == dbConn.Query(sql.arg(pid).toStdString());
}


bool PersonManage::deletePersonInfo(DatabaseAccess& dbConn, const QString &pid)
{
	Exectime etm(__FUNCTION__);
	//TO DO
	QString sql("DELETE FROM person WHERE personid = '%1';\n"
			"INSERT INTO tempperson (personid, operation) VALUES('%1', 1);\n");

	return dbSuccess == dbConn.Query(sql.arg(pid).toStdString());
}

void PersonManage::goToHeadPage()
{
	Exectime etm(__FUNCTION__);
	currentPage = 1;
}


void PersonManage::setInactive()
{
	Exectime etm(__FUNCTION__);
	if(enroll)
	{
		enroll->ExitEnrollUI();
	}

	emit sigSetInActive();
}

//bool PersonManage::isHeadPage()
//{
//    return (1 == currentPage);
//}

//bool PersonManage::isTailPage()
//{
//    return (currentPage == totalPage);
//}


bool PersonManage::findBy(const QString &key, std::vector<PersonDetail> &persons)
{
	Exectime etm(__FUNCTION__);
	int resVal = queryAll();

	if(resVal < 0)
	{
		LOG_ERROR("queryAll < 0");
		return false;
	}

	if(0 == resVal)
	{
		return true;
	}

	//装配sql查询语句
	QString sql = "SELECT t1.personid, t1.worksn, t1.name, t1.sex, t1.idcard,  t1.departid, "          //检出字段
		"  t1.departname, t1.hasirisdata, t1.cardid, t1.disabletime, t1.memo, t1.superid "
		"FROM V_Person_Detail AS t1 "          //数据源表
		"WHERE  t1.worksn LIKE '%%1%' "              //查询条件
		"  OR t1.name LIKE '%%1%' %2 "
		"  OR t1.sex LIKE '%%1%' "
        "  OR t1.idcard LIKE '%%1%' "
		"  OR t1.departname LIKE '%%1%' %2 "
		"  OR t1.hasirisdata LIKE '%%1%' "
		"  OR t1.cardid LIKE '%%1%' "
		//                 "  OR t1.disabletime = '%%1%' "
		"  OR t1.memo LIKE '%%1%' %2 "
		"ORDER BY convert_to(" + orderBy + ", E'GBK') " + orderDirection + ";";              //排序

	//    //装配sql查询语句
	//    QString sql = "SELECT t1.personid, t1.worksn, t1.name, t1.sex, t1.idcard,  t1.departid, "          //检出字段
	//                  "  t1.departname, t2.hasirisdata, t1.cardid, t1.disabletime, t1.memo, t1.superid "
	//                  "FROM person AS t1 "
	//                  "  LEFT JOIN (SELECT personid, CAST(count(*) AS int) hasirisdata FROM irisdatabase GROUP BY personid) AS t2 "
	//                  "  ON t1.personid = t2.personid "            //数据源表
	//                  "WHERE  t1.worksn LIKE '%%1%' "              //查询条件
	//                  "  OR t1.name LIKE '%%1%' %2 "
	//                  "  OR t1.sex LIKE '%%1%' "
	////                 "  OR t1.idcard LIKE '%%1%' "
	//                  "  OR t1.departname LIKE '%%1%' %2 "
	////                 "  OR t1.hasirisdata LIKE '%%1%' "
	//                  "  OR t1.cardid LIKE '%%1%' "
	////                 "  OR t1.disabletime = '%%1%' "
	//                  "  OR t1.memo LIKE '%%1%' %2 "
	//            "ORDER BY convert_to(" + orderBy + ", E'GBK') " + orderDirection + ";";              //排序
	//注入参数
	if(key.contains("%"))
	{
		sql = sql.arg(QString("/%1").arg(key), "ESCAPE '/'");
	}
	else
	{
		sql = sql.arg(key, "");
	}

	//查询指定域的人员数据
	DatabaseAccess dbAc;
	if(dbSuccess != dbAc.Query(sql.toStdString(), persons))
	{
		LOG_ERROR("Query sql failed, %s",sql.toStdString().c_str());
		return false;
	}

	return true;
}



bool PersonManage::dispPageHasGivenRecord(const std::string &recPersonID, const int praPageSize)
{
	Exectime etm(__FUNCTION__);
	//TO DO
	//清空当前人员信息数据
	ui->twgtPerson->setRowCount(0);

	//查询指定域的人员数据
	std::vector<PersonDetail> persons;
	if(!findBy(ui->txtKeyWords->text(), persons) || 0 == persons.size())
	{
		return false;
	}

	int totalPerson = persons.size();
	int iter(0);
	for(iter = 0; iter < totalPerson; ++iter)
	{
		if(recPersonID == persons[iter].personID.Unparse())
		{
			break;
		}
	}

	currentPage = iter / praPageSize + 1;
	//获取查询结果中的记录数
	dispData(persons.begin() + iter - iter % praPageSize, min(persons.begin() + iter - iter % praPageSize + praPageSize, persons.end()));
	ui->twgtPerson->selectRow(iter % praPageSize);

	return true;
}
