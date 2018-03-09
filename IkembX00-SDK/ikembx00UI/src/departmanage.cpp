/*****************************************************************************
 ** 文 件 名：departmanage.cpp
 ** 主 要 类：DepartManage
 **
 ** Copyright (c) 中科虹霸有限公司
 **
 ** 创 建 人：L.Wang
 ** 创建时间：20013-10-18
 **
 ** 修 改 人：
 ** 修改时间：
 ** 描  述:   部门管理
 ** 主要模块说明: 部门浏览， 增加，修改和删除部门
 **
 ** 版  本:   1.0.0
 ** 备  注:
 **
 *****************************************************************************/
#include <QStringList>
#include <QSqlQuery>
#include <QMessageBox>
#include <QSqlError>
#include <QItemDelegate>
#include <QInputDialog>
#include <QTableWidget>
#include <QTableWidgetItem>

#include <errno.h>
#include <string.h>

#include "ikmessagebox.h"
#include "departmanage.h"
#include "ui_departmanage.h"

//引用数据库封装
#include "../../../ikIOStream/interface.h"
#include "../../../databaseLib/IKDatabaseLib/include/IKDatabaseLib.h"
#include "../Common/Logger.h"
#include "../Common/Exectime.h"


DepartManage::DepartManage(QWidget *parent) :
	QDialog(parent),
	ui(new Ui::DepartManage),
	isEditting(false),
	defaultDeptName(QString::fromUtf8("请输入部门名称")),
	curOpt(noneOpt),
	hasModified(false)
{
	
	ui->setupUi(this);

	//隐藏对话框标题栏及对话框位置调整和背景填充
	setWindowFlags(Qt::FramelessWindowHint);
	setGeometry(QRect(0, 0, 1024, 600));
	setBackgroundColor(QColor(242,241,240));


	//初始化部门列表
	//    initDepartListModel();        //初始化数据模型
	initDepartListWidget();       //初始化数据显示格式
	initDepartList();

	//连接信号与槽函数
	connect(ui->btnBack, SIGNAL(clicked()), this, SLOT(slotBack()));        //返回
	connect(ui->btnAdd, SIGNAL(clicked()), this, SLOT(slotAdd()));           //增加
	connect(ui->btnEdit,SIGNAL(clicked()), this, SLOT(slotEdit()));          //修改
	connect(ui->btnDelete, SIGNAL(clicked()), this, SLOT(slotDelete()));     //删除
	connect(ui->btnSave, SIGNAL(clicked()), this, SLOT(slotSave()));
	connect(ui->twgtDeparts, SIGNAL(itemChanged(QTableWidgetItem*)), this, SLOT(slotDepartInfoChanged(QTableWidgetItem*)));
}

DepartManage::~DepartManage()
{
	
	delete ui;
}


/*****************************************************************************
 *                        初始化部门动态显示
 *  函 数 名：initDepartList
 *  功    能：初始化部门管理中的部门列表
 *  说    明：
 *  参    数：
 *  返 回 值：
 *  创 建 人：L.Wang
 *  创建时间：2013-10-18
 *  修 改 人：
 *  修改时间：
 *****************************************************************************/
void DepartManage::initDepartList()
{
	
	//TO DO
	ui->twgtDeparts->setRowCount(0);

	//设置结束编辑标志
	isEditting = false;


	//查询数据库
	DatabaseAccess dbAc;
	std::vector<Depart> departs;
	std::string query("SELECT departid, departname, parentid, memo FROM depart ORDER BY departid ASC");

    int nret = dbAc.Query(query, departs);
	if(nret != dbSuccess)
	{
		LOG_ERROR("Query sql failed. [%s]",query.c_str());
		return;
	}

	if(departs.empty())
	{
		std::cout << __FUNCTION__ << ": no records of departs are found" << std::endl;
		LOG_ERROR(": no records of departs are found");
		return;
	}

	int totalDepts = departs.size();
	Depart depart;

	for(int iter = 0; iter < totalDepts; iter++)
	{
		depart = departs.at(iter);

		ui->twgtDeparts->insertRow(iter);
		creatNewRecord(iter, depart);
		//        ui->twgtDeparts->setItem(iter, 0, new QTableWidgetItem(QString::fromUtf8("%1").arg(depart.departID)));
		//        ui->twgtDeparts->setItem(iter, 1, new QTableWidgetItem(QString::fromUtf8("%1").arg(depart.departName.c_str())));
		//        ui->twgtDeparts->setItem(iter, 2, new QTableWidgetItem(QString::fromUtf8("%1").arg(depart.parentID)));
		//        ui->twgtDeparts->setItem(iter, 3, new QTableWidgetItem(QString::fromUtf8("%1").arg(depart.memo.c_str())));

	}

}


/*****************************************************************************
 *                        返回操作
 *  函 数 名：initDepartListWidget
 *  功    能：槽函数，返回操作处理函数
 *  说    明：
 *  参    数：
 *  返 回 值：
 *  创 建 人：L.Wang
 *  创建时间：2013-10-18
 *  修 改 人：
 *  修改时间：
 *****************************************************************************/
void DepartManage::initDepartListWidget()
{
	
	//设置部门管理表列数
	ui->twgtDeparts->setColumnCount(4);

	//设置列的名称
	ui->twgtDeparts->setHorizontalHeaderLabels(QStringList() << "部门编号" << "部门名称" << "所属部门" << "备注");

	//设置字体样式
	//    QFont hviewFont = ui->twgtDeparts->horizontalHeader()->font();
	//    hviewFont.setPointSize(20);
	//    hviewFont.setBold(true);
	//    ui->twgtDeparts->horizontalHeader()->setFont(hviewFont);
	ui->twgtDeparts->horizontalHeader()->setStyleSheet("QHeaderView{font:20pt}");

	//设置表头不塌陷
	ui->twgtDeparts->horizontalHeader()->setHighlightSections(false);

	//设置表头字段禁用点击操作
	ui->twgtDeparts->horizontalHeader()->setDisabled(true);

	//隐藏序号栏
	ui->twgtDeparts->verticalHeader()->setVisible(false);

	//隐藏部门编号列
	ui->twgtDeparts->setColumnHidden(0, true);
	//隐藏父部门编号列
	ui->twgtDeparts->setColumnHidden(2, true);
	//隐藏部门备注列
	ui->twgtDeparts->setColumnHidden(3, true);


	//设置单元格编辑触发方式
	ui->twgtDeparts->setEditTriggers(QAbstractItemView::NoEditTriggers);
	//仅允许按行为单位进行选择
	ui->twgtDeparts->setSelectionBehavior(QAbstractItemView::SelectRows);
	//每次进允许选择一行
	ui->twgtDeparts->setSelectionMode(QAbstractItemView::SingleSelection);


	//根据内容调整列宽
	ui->twgtDeparts->resizeColumnsToContents();
	//列拉伸，以填充整个控件
	ui->twgtDeparts->horizontalHeader()->setStretchLastSection(true);
	//不显示网格
	ui->twgtDeparts->setShowGrid(false);

	//设置默认焦点在返回键上
	ui->btnBack->setFocus();
}

/*****************************************************************************
 *                        新建一个部门记录
 *  函 数 名：creatNewRecord
 *  功    能：创建一条新的部门记录，并把相关数据注入该记录中
 *  说    明：
 *  参    数：
 *           row，新添加的记录所在的行号
 *           depart，新纪录的相关数据
 *  返 回 值：
 *  创 建 人：L.Wang
 *  创建时间：2014-04-1
 *  修 改 人：
 *  修改时间：
 *****************************************************************************/
void DepartManage::creatNewRecord(int row, const Depart &depart)
{
	
	ui->twgtDeparts->setItem(row, 0, new QTableWidgetItem(QString::fromUtf8("%1").arg(depart.departID)));
	ui->twgtDeparts->setItem(row, 1, new QTableWidgetItem(QString::fromUtf8("%1").arg(depart.departName.c_str())));
	ui->twgtDeparts->setItem(row, 2, new QTableWidgetItem(QString::fromUtf8("%1").arg(depart.parentID)));
	ui->twgtDeparts->setItem(row, 3, new QTableWidgetItem(QString::fromUtf8("%1").arg(depart.memo.c_str())));
}

/*****************************************************************************
 *                        返回操作
 *  函 数 名：slotBack
 *  功    能：槽函数，返回操作处理函数
 *  说    明：
 *  参    数：
 *  返 回 值：
 *  创 建 人：L.Wang
 *  创建时间：2013-10-18
 *  修 改 人：
 *  修改时间：
 *****************************************************************************/
void DepartManage::slotBack()
{
	
	close();
}

///*****************************************************************************
//*                        增加部门
//*  函 数 名：slotAdd
//*  功    能：槽函数，增加部门操作处理函数
//*  说    明：
//*  参    数：
//*  返 回 值：
//*  创 建 人：L.Wang
//*  创建时间：2013-10-18
//*  修 改 人：
//*  修改时间：
//*****************************************************************************/
//void DepartManage::slotAdd()
//{
//    //TO DO
//    //为将要添加的部门获取部门IP
//    int deptID = GetUsableDepartID();
//    if(deptID < 0)
//    {
//        QMessageBox::warning(this, QString::fromUtf8("添加部门"), QString::fromUtf8("操作失败：%1\n请重试！"));
//        return;
//    }

//    //设置部门信息
//    Depart depart;
//    depart.departID = deptID;
//    depart.departName = QString::fromUtf8("请输入部门名称").toStdString();
//    depart.parentID = 0;
//    depart.memo = "";


//    //将新添加的部门更新到数据库
//    DatabaseAccess dbAc;
//    std::string insert("INSERT INTO depart (departid, departname, parentid, memo) "
//                      "VALUES (:departid, :departname, :parentid, :memo)");
//    dbAc.Insert(insert, depart);


//    //更新增加心部门后的部门列表
//    refresh();

//    //记录当前修改的记录索引
//    curEditedIndex = getNewAddedIndex();
//    //标记是否为用户操作
//    isEditting = true;

//    if(curEditedIndex.isValid())
//    {
//        //设置新添加的部门进入编辑状态
//        ui->twgtDeparts->edit(curEditedIndex);   //设置新添加的行为正在编辑状态
//    }
//}


/*****************************************************************************
 *                        增加部门
 *  函 数 名：slotAdd
 *  功    能：槽函数，增加部门操作处理函数
 *  说    明：
 *  参    数：
 *  返 回 值：
 *  创 建 人：L.Wang
 *  创建时间：2013-10-18
 *  修 改 人：
 *  修改时间：
 *****************************************************************************/
void DepartManage::slotAdd()
{
	
	//TO DO
	//为将要添加的部门获取部门IP
	int deptID = getDepartID();
	if(deptID < 0)
	{
		//        QMessageBox::warning(this, QString::fromUtf8("添加部门"), QString::fromUtf8("操作失败：%1\n请重试！"));
		IKMessageBox warnNote(this, QString::fromUtf8("添加部门"), QString::fromUtf8("操作失败，无法为新部门分配编号！"), warningMbx);

		warnNote.exec();
		return;
	}

	//设置部门信息
	Depart depart;
	depart.departID = deptID + 1;
	depart.departName = QString::fromUtf8("请输入部门名称").toStdString();
	depart.parentID = 0;
	depart.memo = "";

	//记录当前部门名称
	curDeptName = depart.departName.c_str();


	//备份操作记录
	newDeparts.push_back(depart);



	//在UI上新增加一个部门
	ui->twgtDeparts->insertRow(ui->twgtDeparts->rowCount());
	creatNewRecord(ui->twgtDeparts->rowCount() - 1, depart);


	//记录当前修改的记录索引
	ui->twgtDeparts->setCurrentCell(ui->twgtDeparts->rowCount() - 1, 1);
	curEditedIndex = ui->twgtDeparts->currentIndex();

	//标记是否为用户操作
	hasModified = true;
	isEditting = true;
	curOpt = departAdd;

	if(curEditedIndex.isValid())
	{
		//设置新添加的部门进入编辑状态
		ui->twgtDeparts->edit(curEditedIndex);   //设置新添加的行为正在编辑状态
	}
}

/*****************************************************************************
 *                        编辑部门
 *  函 数 名：slotEdit
 *  功    能：槽函数，编辑部门操作处理函数
 *  说    明：
 *  参    数：
 *  返 回 值：
 *  创 建 人：L.Wang
 *  创建时间：2013-10-18
 *  修 改 人：
 *  修改时间：
 *****************************************************************************/
void DepartManage::slotEdit()
{
	
	//判断是否指定了要修改的部门
	if(!ui->twgtDeparts->currentIndex().isValid())
	{
		IKMessageBox infoNote(this, QString::fromUtf8("修改部门信息"), QString::fromUtf8("请先选择要修改的部门！"), informationMbx);
		infoNote.exec();
		return;
	}

	//记录当前修改的记录索引
	curEditedIndex = ui->twgtDeparts->currentIndex();
	if(!curEditedIndex.isValid())
	{
		return;
	}

	//记录当前部门名称
	curDeptName = ui->twgtDeparts->item(ui->twgtDeparts->currentRow(), 1)->text().trimmed();

	//检查当前修改的记录是否已经被缓存
	if(!alreadInList(ui->twgtDeparts->item(ui->twgtDeparts->currentRow(), 0)->text().toInt()))
	{
		Depart dept;
		dept.departID = ui->twgtDeparts->item(ui->twgtDeparts->currentRow(), 0)->text().toInt();
		dept.departName = ui->twgtDeparts->item(ui->twgtDeparts->currentRow(), 1)->text().trimmed().toStdString();
		dept.parentID = ui->twgtDeparts->item(ui->twgtDeparts->currentRow(), 2)->text().toInt();
		dept.memo = ui->twgtDeparts->item(ui->twgtDeparts->currentRow(), 3)->text().trimmed().toStdString();

		editedDeparts.push_back(dept);
	}


	//标记是否为用户操作
	isEditting = true;
	curOpt = departEdit;

	//设置选中的部门为正在编辑状态
	ui->twgtDeparts->edit(curEditedIndex);
}

/*****************************************************************************
 *                        删除部门
 *  函 数 名：slotDelete
 *  功    能：槽函数，处理删除部门操作
 *  说    明：
 *  参    数：
 *  返 回 值：
 *  创 建 人：L.Wang
 *  创建时间：2013-10-18
 *  修 改 人：
 *  修改时间：
 *****************************************************************************/
void DepartManage::slotDelete()
{
	
	//TO DO
	//判断当前是否有选中行
	if(ui->twgtDeparts->currentIndex().row() < 0)
	{
		//        QMessageBox::information(this, QString::fromUtf8("删除人员信息"), QString::fromUtf8("请先选择要删除的部门！"));
		IKMessageBox infoNote(this, QString::fromUtf8("修改部门信息"), QString::fromUtf8("请先选择要删除的部门！"), informationMbx);

		infoNote.exec();
		return;
	}


	//通过Model/View模式来删除指定的部门
	//    int userOpt;
	//    userOpt = QMessageBox::question(this, QString::fromUtf8("删除人员信息"), QString::fromUtf8("确定要删除吗?"),
	//                                    QMessageBox::Ok|QMessageBox::Cancel, QMessageBox::Ok);
	IKMessageBox questionNote(this, QString::fromUtf8("删除人员信息"), QString::fromUtf8("是否删除？"), questionMbx);

	questionNote.exec();
	if(QDialog::Accepted == questionNote.result())
	{
		//删除选中的部门
		int deptID = ui->twgtDeparts->item(ui->twgtDeparts->currentIndex().row(), 0)->text().toInt();


		DatabaseAccess dbAc;
		QString query("DELETE FROM depart WHERE departid = %1");
		query = query.arg(deptID);


		if(dbSuccess == dbAc.Query(query.toStdString()))
		{
			//            QMessageBox::information(this, QString::fromUtf8("删除部门"), QString::fromUtf8("操作成功"));
			ui->twgtDeparts->removeRow(ui->twgtDeparts->currentRow());
		}
		else
		{
			//            QMessageBox::warning(this, QString::fromUtf8("删除部门"), QString::fromUtf8("无法删除选定部门！"));
			IKMessageBox warnNote(this, QString::fromUtf8("删除部门"), QString::fromUtf8("无法删除选定部门！"), warningMbx);

			warnNote.exec();
		}

		//        //刷新部门列表
		//        refresh();
	}

	return;
}


void DepartManage::slotSave()
{
	
	//TO DO
	if(!hasModified)
	{
		IKMessageBox infoNote(this, QString::fromUtf8("更新部门信息"), QString::fromUtf8("部门信息未曾修改！"), informationMbx);
		infoNote.exec();

		return;
	}


	if(!addDeparts())
	{
		LOG_ERROR("addDeparts() failed");
		return;
	}

	if(!editDeparts())
	{
		LOG_ERROR("editDeparts() failed");
		return;
	}

	if(!deleteDeparts())
	{
		LOG_ERROR("deleteDeparts() failed");
		return;
	}

	//Update depart list
	refresh();

	//复位操作状态
	curOpt = noneOpt;
	//清空新添加部门列表缓存
	newDeparts.clear();
	//清空修改部门列表缓存
	editedDeparts.clear();
	//清空删除部门列表缓存
	rmedDeparts.clear();

	IKMessageBox infoNote(this, QString::fromUtf8("更新部门信息"), QString::fromUtf8("操作成功！"), informationMbx);
	infoNote.exec();
}


bool DepartManage::alreadInList(const int id)
{
	
	for(int iter = 0; iter < newDeparts.size(); iter++)
	{
		if(id == newDeparts[iter].departID)
		{
			return true;
		}
	}

	for(int iter = 0; iter < editedDeparts.size(); iter++)
	{
		if(id == editedDeparts[iter].departID)
		{
			return true;
		}
	}

	return false;
}

void DepartManage::mousePressEvent(QMouseEvent *event)
{
	
	//TO DO
	if(event->buttons() || Qt::LeftButton)
	{
		//        std::cout << "[MouseMoved]: X:" << event->pos().x() << " Y:" << event->pos().y() << std::endl;
		ui->twgtDeparts->selectRow(curEditedIndex.row() == 0 ? curEditedIndex.row() + 1 : curEditedIndex.row() - 1);
		ui->twgtDeparts->selectRow(curEditedIndex.row());

	}

	QDialog::mouseMoveEvent(event);
}

//void DepartManage::slotDepartInfoChanged(QTableWidgetItem* item)
//{
//    //TO DO
//    if(!isEditting)
//    {
//        return;
//    }

//    //判断部门名称是否为空
//    if(!curEditedIndex.data().toString().trimmed().isEmpty())
//    {    //如果部门名称不为空，则更新至数据库
//        DatabaseAccess dbAc;
//        std::string query("UPDATE depart set departname = :departname WHERE departid = :departid");

//        Depart dpt;
//        dpt.departID = ui->twgtDeparts->item(curEditedIndex.row(), 0)->text().toInt();
//        dpt.departName = ui->twgtDeparts->item(curEditedIndex.row(), 1)->text().toStdString();

//        if(dbSuccess == dbAc.Update(query, dpt))
//        {
//            //QMessageBox::information(this, QString::fromUtf8("部门信息修改"), QString::fromUtf8("操作成功"));
//        }
//        else
//        {
//            QMessageBox::information(this, QString::fromUtf8(""), QString::fromUtf8("无法更新部门信息到数据库"));
//        }

//    }
//    else
//    {
//        QMessageBox::warning(this, QString::fromUtf8("部门信息修改"), QString::fromUtf8("部门名称不能为空"));
//    }

//    //重置编辑标志
//    isEditting = false;
//    //刷新部门列表
//    refresh();
//}


void DepartManage::slotDepartInfoChanged(QTableWidgetItem* item)
{
	
	//TO DO
	if(!isEditting)
	{
		return;
	}

	hasModified = true;
	//判断部门名称是否为空
	if(!curEditedIndex.data().toString().trimmed().isEmpty())
	{
		updateLocal(item->row());
	}
	else
	{
		//        QMessageBox::warning(this, QString::fromUtf8("部门信息修改"), QString::fromUtf8("部门名称不能为空！"));
		IKMessageBox warnNote(this, QString::fromUtf8("部门信息修改"), QString::fromUtf8("部门名称不能为空！"), warningMbx);

		warnNote.exec();
		item->setText(curDeptName);
	}

	//重置编辑标志
	isEditting = false;
}


bool DepartManage::updateLocal(int row)
{
	
	for(int iter = 0; iter < newDeparts.size(); iter++)
	{
		if(newDeparts[iter].departID == ui->twgtDeparts->item(row, 0)->text().toInt())
		{
			newDeparts[iter].departName = ui->twgtDeparts->item(row, 1)->text().trimmed().toStdString();
		}
	}

	for(int iter = 0; iter < editedDeparts.size(); iter++)
	{
		if(editedDeparts[iter].departID == ui->twgtDeparts->item(row, 0)->text().toInt())
		{
			editedDeparts[iter].departName = ui->twgtDeparts->item(row, 1)->text().trimmed().toStdString();
		}
	}
}


void DepartManage::refresh()
{
	
	//TO DO
	initDepartList();
}


QModelIndex DepartManage::getNewAddedIndex()
{
	
	int rows = ui->twgtDeparts->rowCount();

	for(int row = 0; row < rows; row++)
	{
		QString val = ui->twgtDeparts->item(row, 1)->text();
		if(ui->twgtDeparts->item(row, 1)->text() == (defaultDeptName))
		{
			ui->twgtDeparts->setCurrentCell(row, 1);


			return ui->twgtDeparts->currentIndex();
		}
	}

	return QModelIndex();
}

/*****************************************************************************
 *                        获取部门ID
 *  函 数 名：GetDepartID
 *  功    能：首先查询本地是否已有新增部门，如果有则在新增部门最后新增部门ID的基础上增加1得到
 *           新增部门ID，否则通过查询当前部门表中最大部门编号增加1得到新增部门的部门编号
 *  说    明：
 *  参    数：
 *  返 回 值：新增部门的编号
 *  创 建 人：L.Wang
 *  创建时间：2013-10-18
 *  修 改 人：
 *  修改时间：
 *****************************************************************************/
int DepartManage::getDepartID()
{
	
	if(getMaxDepartIDFromLocal() < 0)
	{
		return getMaxDepartIDFromDB();
	}

	return getMaxDepartIDFromLocal();

}


/*****************************************************************************
 *                        从数据库获取部门ID
 *  函 数 名：getMaxDepartIDFromDB
 *  功    能：通过查询当前部门表中最大部门编号得到新增部门的部门编号
 *  说    明：
 *  参    数：
 *  返 回 值：新增部门的编号
 *  创 建 人：L.Wang
 *  创建时间：2013-10-18
 *  修 改 人：
 *  修改时间：
 *****************************************************************************/
int DepartManage::getMaxDepartIDFromDB()
{
	
	int maxID = -1;
	DatabaseAccess dbAc;
	std::string query("SELECT max(departid) FROM depart");
    int nret = dbAc.Query(query, maxID);
	if(nret != dbSuccess)
	{
		LOG_ERROR("Query sql failed. [%s]",query.c_str());
		return;
	}

	return maxID;
	//    if(dbSuccess == dbAc.Query(query, maxID))
	//    {
	//        return departID;
	//    }
	//    else
	//    {
	//        return -1;
	//    }
}

/*****************************************************************************
 *                        获取部门ID
 *  函 数 名：getMaxDepartIDFromLocal
 *  功    能：通过查询新增部门列表中最大部门编号得到新增部门的部门编号
 *  说    明：
 *  参    数：
 *  返 回 值：新增部门的编号
 *  创 建 人：L.Wang
 *  创建时间：2013-10-18
 *  修 改 人：
 *  修改时间：
 *****************************************************************************/
int DepartManage::getMaxDepartIDFromLocal()
{
	
	int maxID = -1;

	//    //这样也是可以的，但不是绝对可靠
	//    if(newDeparts.size())
	//    {
	//        return newDeparts[newDeparts.size() -1].departID;
	//    }

	for(int iter = 0; iter < newDeparts.size(); iter++)
	{
		if(maxID < newDeparts[iter].departID)
		{
			maxID = newDeparts[iter].departID;
		}
	}

	return maxID;
}


bool DepartManage::addDeparts()
{
	
	//TO DO
	if(newDeparts.size())
	{
		QString sql("INSERT INTO depart(departid, departname, parentid, memo) VALUES(:departid, :departname, :parentid, :memo)");
		DatabaseAccess dbAc;

		if(dbSuccess != dbAc.Insert(sql.toStdString(), newDeparts))
		{
			IKMessageBox errorNote(this, QString::fromUtf8("更新部门信息"), QString::fromUtf8("操作失败！"), errorMbx);
			errorNote.exec();

			return false;
		}
	}


	return true;
}

bool DepartManage::editDeparts()
{
	
	//TO DO
	if(editedDeparts.size())
	{
		QString sql("UPDATE depart SET departname = :departname WHERE departid = :departid");
		DatabaseAccess dbAc;

		bool optRes;
		for(int iter = 0; iter < editedDeparts.size(); iter++)
		{
			optRes &= (dbSuccess == dbAc.Update(sql.toStdString(), editedDeparts[iter]));
		}


		if(optRes)
		{
			IKMessageBox errorNote(this, QString::fromUtf8("更新部门信息"), QString::fromUtf8("操作失败！"), errorMbx);
			errorNote.exec();

			return false;
		}
	}


	return true;
}

bool DepartManage::deleteDeparts()
{
	
	//TO DO
	return true;
}

