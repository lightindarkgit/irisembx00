/*****************************************************************************
** 文 件 名：departmanage.cpp
** 主 要 类：DepartManage
**
** Copyright (c) 中科虹霸有限公司
**
** 创 建 人：王磊
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
#include <QStringListModel>
#include <QStringList>
#include <QSqlQueryModel>
#include <QSqlQuery>
#include <QMessageBox>
#include <QSqlError>
#include <QItemDelegate>
#include <QInputDialog>

#include "departmanage.h"
#include "ui_departmanage.h"


DepartManage::DepartManage(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DepartManage),
    model(NULL)
{
    ui->setupUi(this);

    //隐藏对话框标题栏及对话框位置调整和背景填充
    setWindowFlags(Qt::FramelessWindowHint);
    setGeometry(QRect(0, 0, 800, 600));
    setBackgroundColor(QColor(242,241,240));


    //初始化部门列表
    initDepartListModel();        //初始化数据模型
    initDepartListWidget();       //初始化数据显示格式
    initDepartList();

    //连接信号与槽函数
    connect(ui->btnBack, SIGNAL(clicked()), this, SLOT(slotBack()));        //返回
    connect(ui->btnAdd, SIGNAL(clicked()), this, SLOT(slotAdd()));           //增加
    connect(ui->btnEdit,SIGNAL(clicked()), this, SLOT(slotEdit()));          //修改
    connect(ui->btnDelete, SIGNAL(clicked()), this, SLOT(slotDelete()));     //删除
}

DepartManage::~DepartManage()
{
    delete ui;
}

/*****************************************************************************
*                        返回操作
*  函 数 名：slotBack
*  功    能：槽函数，返回操作处理函数
*  说    明：
*  参    数：
*  返 回 值：
*  创 建 人：王磊
*  创建时间：2013-10-18
*  修 改 人：
*  修改时间：
*****************************************************************************/
void DepartManage::initDepartListModel()
{
    //TO DO
    model = new QSqlTableModel(this);     //创建数据表模型
    model->setEditStrategy(QSqlTableModel::OnFieldChange);     //设置数据库修改更新策略

    model->setTable("depart");            //指定与模绑定的数据表
//    model->removeColumn(0);               //不显示部门编号

    model->setHeaderData(0, Qt::Horizontal, QString::fromUtf8("部门"));

    model->select();         //查询部门表
    model->sort(0, Qt::AscendingOrder);      //部门排序
}

/*****************************************************************************
*                        返回操作
*  函 数 名：initDepartListWidget
*  功    能：槽函数，返回操作处理函数
*  说    明：
*  参    数：
*  返 回 值：
*  创 建 人：王磊
*  创建时间：2013-10-18
*  修 改 人：
*  修改时间：
*****************************************************************************/
void DepartManage::initDepartListWidget()
{
    ui->tbvDepart->setEditTriggers(QAbstractItemView::SelectedClicked);
    ui->tbvDepart->setModel(model);           //设置视图与模型的绑定
    ui->tbvDepart->setColumnHidden(0, true);

    ui->tbvDepart->resizeColumnsToContents();
    ui->tbvDepart->horizontalHeader()->setStretchLastSection(true);
}

/*****************************************************************************
*                        返回操作
*  函 数 名：slotBack
*  功    能：槽函数，返回操作处理函数
*  说    明：
*  参    数：
*  返 回 值：
*  创 建 人：王磊
*  创建时间：2013-10-18
*  修 改 人：
*  修改时间：
*****************************************************************************/
void DepartManage::slotBack()
{
//    model->submitAll();
    close();
}

/*****************************************************************************
*                        增加部门
*  函 数 名：slotAdd
*  功    能：槽函数，增加部门操作处理函数
*  说    明：
*  参    数：
*  返 回 值：
*  创 建 人：王磊
*  创建时间：2013-10-18
*  修 改 人：
*  修改时间：
*****************************************************************************/
void DepartManage::slotAdd()
{
    //TO DO

//-------弹框式输入新部门名称------------
//    QString departName;
//    bool ok;

//    departName = QInputDialog::getText(this, QString::fromUtf8("添加部门"), QString::fromUtf8("请输入部门名称"),
//                                       QLineEdit::Normal, QString::fromUtf8(""), &ok);

//    if(!ok || departName.isEmpty())
//    {
//        QMessageBox::warning(this, QString::fromUtf8("添加部门"), QString::fromUtf8("确定要取消添加部门"));
//        return;
//    }


//    ---------通过Query添加人员信息-------
//                          QSqlQuery query;
//    query.prepare("INNSERT INTO depart (name) VALUES(:name)");
//    query.bindValue(":name", departName);
//    query.exec();
//    if(!query.isActive())
//    {
//        QMessageBox::warning(this, QString::fromUtf8("添加部门"), query.lastError().text());
//        return;
//    }



    if(!model->insertRow(model->rowCount()))
    {
        QMessageBox::warning(this, QString::fromUtf8("添加人员"), model->lastError().text());
        return;
    }
    ui->tbvDepart->edit(model->index(model->rowCount() - 1, 1));   //设置新添加的行为正在编辑状态


//    optRes = model->setData(model->index(tailRow, 1), departName.trimmed());
//    optRes = model->submitAll();
}

/*****************************************************************************
*                        编辑部门
*  函 数 名：slotEdit
*  功    能：槽函数，编辑部门操作处理函数
*  说    明：
*  参    数：
*  返 回 值：
*  创 建 人：王磊
*  创建时间：2013-10-18
*  修 改 人：
*  修改时间：
*****************************************************************************/
void DepartManage::slotEdit()
{
    //判断是否指定了要修改的部门
    if(!ui->tbvDepart->currentIndex().isValid())
    {
        QMessageBox::information(this, QString::fromUtf8("修改部门信息"), QString::fromUtf8("请先选择要修改的部门！"));
        return;
    }
    //设置选中的部门为正在编辑状态
    ui->tbvDepart->edit(ui->tbvDepart->currentIndex());



//    QModelIndex editingRowIndex;
//    QString departName, oldDepartName;
//    oldDepartName = editingRowIndex.data().toString();

//    //获取新部门名称
//    departName = QInputDialog::getText(this, QString::fromUtf8("修改部门"), QString::fromUtf8("请输入新部门名称"),
//                                       QLineEdit::Normal, editingRowIndex.data().toString());
//    if(departName.isEmpty())
//    {
//        QMessageBox::warning(this, QString::fromUtf8("修改部门"), QString::fromUtf8("尚未指定修改后的部门名称！"));
//        return;
//    }


//    int departID;
//    QSqlQuery query;
//    query.prepare("SELECT id FROM depart WHERE name = :name");
//    query.bindValue(":name", oldDepartName.toUtf8());
//    query.exec();
//    if(query.next())
//    {
//        departID = query.value(0).toInt();
//    }
//    else
//    {
//        //TO DO
//    return;
//    }

//    query.prepare("UPDATE depart set name = :name WHERE id = :id");
//    query.bindValue(":name", departName.toUtf8());
//    query.bindValue(":id", departID);
//    query.exec();

//    if(!query.isActive())
//    {
//        QMessageBox::warning(this, QString::fromUtf8("修改部门"), query.lastError().text());
//        return;
//    }

//    //更新部门信息到数据库
//    model->setData(model->index(editingRowIndex.row(), 1), departName.trimmed());
//    model->submitAll();
}

/*****************************************************************************
*                        删除部门
*  函 数 名：slotDelete
*  功    能：槽函数，处理删除部门操作
*  说    明：
*  参    数：
*  返 回 值：
*  创 建 人：王磊
*  创建时间：2013-10-18
*  修 改 人：
*  修改时间：
*****************************************************************************/
void DepartManage::slotDelete()
{
    //TO DO
    if(ui->tbvDepart->currentIndex().row() < 0)
    {
        QMessageBox::information(this, QString::fromUtf8("删除人员信息"), QString::fromUtf8("请先选择要删除的部门！"));
        return;
    }

    //通过Model/View模式来删除指定的部门
    int userOpt;
    userOpt = QMessageBox::question(this, QString::fromUtf8("删除人员信息"), QString::fromUtf8("确定要删除吗?"),
                                    QMessageBox::Ok|QMessageBox::Cancel, QMessageBox::Ok);
    if(QMessageBox::Ok == userOpt)
    {
        //删除选中的部门
        bool resVal;
        int row = ui->tbvDepart->currentIndex().row();
        resVal = model->removeRow(row);
    }
    else
    {
        return;
    }



//    int userOpt;
//    userOpt = QMessageBox::question(this, QString::fromUtf8("删除人员信息"), QString::fromUtf8("确定要删除吗?"),
//                                    QMessageBox::Ok|QMessageBox::Cancel, QMessageBox::Ok);
//    if(QMessageBox::Ok == userOpt)
//    {
//        model->submitAll();
//    }
//    else
//    {
//        model->revertAll();
//    }


//    QSqlQuery query;
//    query.prepare("DELETE FROM depart WHERE name = :departName");
//    query.bindValue(":departName", ui->lstwDepart->currentItem()->text());
//    query.exec();
//    if(query.isActive())
//    {
//        //TO DO
//        int i = 0;
//        initDepartList();
//    }
//    else
//    {
//        int i = 0;
//    }
}

/*****************************************************************************
*                        初始化部门动态显示
*  函 数 名：initDepartList
*  功    能：初始化部门管理中的部门列表
*  说    明：
*  参    数：
*  返 回 值：
*  创 建 人：王磊
*  创建时间：2013-10-18
*  修 改 人：
*  修改时间：
*****************************************************************************/
void DepartManage::initDepartList()
{
    //TO DO

}
