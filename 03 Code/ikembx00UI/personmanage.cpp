/*****************************************************************************
** 文 件 名：personmanage.cpp
** 主 要 类：PersonManage
**
** Copyright (c) 中科虹霸有限公司
**
** 创 建 人：王磊
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


#include "personmanage.h"
#include "ui_personmanage.h"

#include "detailpersoninfo.h"
#include "enroll.h"
#include "../common.h"


//static bool createConnection(QWidget* parent)
//{
//    //TO DO
//    QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE");
//    db.setDatabaseName("../database/personinfo.db");

//    if(!db.open())
//    {
//        QMessageBox::critical(parent, QString::fromUtf8("连接数据库"), QString::fromUtf8("数据库打开失败！"), QMessageBox::Ok);
//        return false;
//    }

//    if(!QFile::exists("../database/personinfo.db"))
//    {
//        QSqlQuery query;
//        query.exec(QString("CREATE TABLE person (workid int primary key not NULL, name varchar, gender varchar,"
//                           "idnum varchar, departid int, iris varchar, card varchar, note varchar)"));

//        if(!query.isActive())
//        {
//            QMessageBox::warning(parent, QString::fromUtf8("创建数据库"), query.lastError().text());
//            return false;
//        }

//        query.exec(QString("CREATE TABLE depart (id int primary key not NULL, name varchar)"));
//        if(!query.isActive())
//        {
//            QMessageBox::warning(parent, QString::fromUtf8("创建数据库"), query.lastError().text());
//            return false;
//        }
//    }

//    return true;
//}


PersonManage::PersonManage(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::PersonManage),
    currentPage(1),
    totalPage(1),
    pageInfo(QString::fromUtf8("第%1/%2页")),
    model(NULL),
    headView(NULL),
    orderBy("workid"),
    enroll(NULL)
{
    ui->setupUi(this);
    ui->txtKeyWords->setFocus();


//    //连接数据库
//    createConnection(this);

    //设置模型
    model = new QSqlQueryModel(this);
    headView = ui->tblResult->horizontalHeader();
    ui->labPageInfo->setText(pageInfo.arg(1).arg(totalPage));

    //初始化数据视图
    initView();

    //初始化注册页面
    initEnrollUI();

    //信号与槽的连接
    connect(ui->btnBack, SIGNAL(clicked()), this, SIGNAL(sigGoBack()));   //返回按钮

    connect(ui->btnAdd, SIGNAL(clicked()), this, SLOT(slotAddNew()));     //增加人员
    connect(ui->btnEdit, SIGNAL(clicked()), this, SLOT(slotEdit()));      //修改人员
    connect(ui->btnDelete, SIGNAL(clicked()), this, SLOT(slotDelete()));  //删除人员
    connect(ui->btnFind, SIGNAL(clicked()), this, SLOT(slotFind()));      //查询人员

    connect(ui->btnEnrollIris, SIGNAL(clicked()), this, SLOT(slotEnrollIris()));      //注册虹膜

    connect(ui->btnHeadtPage, SIGNAL(clicked()), this, SLOT(slotHeadPage()));         //首页
    connect(ui->btnPreviousPage, SIGNAL(clicked()), this, SLOT(slotPreviousPage()));  //前一页
    connect(ui->btnNextPage, SIGNAL(clicked()), this, SLOT(slotNextPage()));          //下一页
    connect(ui->btnTailPage, SIGNAL(clicked()), this, SLOT(slotTailPage()));          //尾页

    connect(headView, SIGNAL(sortIndicatorChanged(int,Qt::SortOrder)),
            this, SLOT(slotOrderBy(int, Qt::SortOrder)));                  //人员排序
}

PersonManage::~PersonManage()
{
    if(NULL != enroll)
    {
        delete enroll;
    }

    delete model;
    delete ui;
}


bool PersonManage::initView()
{
    //获取满足查询条件的记录总数
    queryAll();

    currentPage = 1;                   //设置首页为当前页
    getSpecifiedPage(currentPage);     //获取当前页

    //设置表头
    model->setHeaderData(0, Qt::Horizontal, QString::fromUtf8("工号"));
    model->setHeaderData(1, Qt::Horizontal, QString::fromUtf8("姓名"));
    model->setHeaderData(2, Qt::Horizontal, QString::fromUtf8("性别"));
    model->setHeaderData(3, Qt::Horizontal, QString::fromUtf8("部门"));
    model->setHeaderData(4, Qt::Horizontal, QString::fromUtf8("虹膜"));
    model->setHeaderData(5, Qt::Horizontal, QString::fromUtf8("定位卡"));
    model->setHeaderData(6, Qt::Horizontal, QString::fromUtf8("备注"));

    ui->tblResult->setModel(model);     //设置视图与view的绑定
    ui->tblResult->setSelectionBehavior(QAbstractItemView::SelectRows);   //设置选择模式，最小选中单位为行

    ui->tblResult->resizeColumnsToContents();   //设置列宽适应文本大小
    ui->tblResult->resizeRowsToContents();      //设置行高适应文本大小
    ui->tblResult->horizontalHeader()->setStretchLastSection(true);       //设置最后一列拉伸
    //ui->tblResult->verticalHeader()->setStretchLastSection(true);

    return true;
}

bool PersonManage::initEnrollUI()
{
    if(NULL == enroll)
    {
        enroll = new  Enroll(this);
        enroll->hide();

        if(NULL != enroll)
        {
            connect(enroll, SIGNAL(sigEnrollResult(bool)), this, SLOT(slotGetEnrollResult(bool)));    //关联注册虹膜结果信号与槽
        }
    }

    return NULL != enroll;
}

/*****************************************************************************
*                        删除指定人员
*  函 数 名：slotDelete
*  功    能：槽函数，删除人员操作处理函数
*  说    明：
*  参    数：
*  返 回 值：
*  创 建 人：王磊
*  创建时间：2013-10-18
*  修 改 人：
*  修改时间：
*****************************************************************************/
void PersonManage::slotDelete()
{
    //TO DO
    deleteSelected(NULL);

    return;
}


int PersonManage::deleteSelected(const QItemSelectionModel* selMdl)
{
    int curRow = ui->tblResult->currentIndex().row();
    QModelIndex index = ui->tblResult->currentIndex();

    if(!index.isValid())
    {
        QMessageBox::information(this, QString::fromUtf8("删除人员信息"), QString::fromUtf8("请先选择要删除的人员"));
        return -1;
    }

    //获取要删除人员的ID
    int id = index.sibling(curRow, 0).data().toInt();

    QSqlQuery query = model->query();
    query.prepare("delete from person where workid =:id");
    query.bindValue("id", id);

    //询问用户删除确认
    if(QMessageBox::Ok == QMessageBox::question(this, QString::fromUtf8("删除人员信息"), QString::fromUtf8("确定要删除吗?"),
                                 QMessageBox::Ok|QMessageBox::Cancel, QMessageBox::Ok))
    {
        query.exec();          //同步删除数据库中该人员信息
        showData(this);        //刷新数据
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
*  创 建 人：王磊
*  创建时间：2013-10-18
*  修 改 人：
*  修改时间：
*****************************************************************************/
void PersonManage::slotAddNew()
{
    //TO DO
    DetailPersonInfo personInfo(this);
    personInfo.exec();

    showData(this);
}

/*****************************************************************************
*                        编辑人员信息
*  函 数 名：slotEdit
*  功    能：槽函数，编辑人员信息操作处理函数
*  说    明：
*  参    数：
*  返 回 值：
*  创 建 人：王磊
*  创建时间：2013-10-18
*  修 改 人：
*  修改时间：
*****************************************************************************/
void PersonManage::slotEdit()
{
    //TO DO
    PersonInfo personInfo;

    int curRow = ui->tblResult->currentIndex().row();
    QModelIndex index = ui->tblResult->currentIndex();

    if(!index.isValid())
    {
        QMessageBox::information(this, QString::fromUtf8("修改人员信息"), QString::fromUtf8("请先选择要修改的人员"));
        return;
    }


    personInfo.id = index.sibling(curRow, 0).data().toInt();
    personInfo.name = index.sibling(curRow, 1).data().toByteArray().data();
    personInfo.depart = index.sibling(curRow, 3).data().toByteArray().data();


    DetailPersonInfo detailPersonInfo(personInfo, this);
    detailPersonInfo.exec();

    showData(this);
}

/*****************************************************************************
*                        注册虹膜
*  函 数 名：slotEnrollIris
*  功    能：槽函数， 为指定人员注册虹膜操作处理函数
*  说    明：
*  参    数：
*  返 回 值：
*  创 建 人：王磊
*  创建时间：2013-10-18
*  修 改 人：
*  修改时间：
*****************************************************************************/
void PersonManage::slotEnrollIris()
{
    //TO DO
    QModelIndex index = ui->tblResult->currentIndex();

    if(!index.isValid())
    {
        QMessageBox::information(this, QString::fromUtf8("注册虹膜"), QString::fromUtf8("请先选择要注册虹膜的人员"));
        return;
    }

    if(!initEnrollUI())
    {

        QMessageBox::critical(this, QString::fromUtf8("启动人员注册"),  QString::fromUtf8("无法获取注册虹膜所需的内存资源"),
                              QMessageBox::Ok);
        return;
    }


    //获取待注册虹膜的人员信息
    person.id = index.sibling(index.row(), 0).data().toInt();
    person.name = index.sibling(index.row(), 1).data().toByteArray().data();
    person.depart = index.sibling(index.row(), 3).data().toByteArray().data();

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
*  创 建 人：王磊
*  创建时间：2013-10-18
*  修 改 人：
*  修改时间：
*****************************************************************************/
void PersonManage::slotFind()
{
    //TO DO
    queryAll();  //获取查询记录的总页数

    currentPage = 1;
    getSpecifiedPage(currentPage);
//    ui->labPageInfo->setText(pageInfo.arg(currentPage).arg(totalPage));
}

/*****************************************************************************
*                        浏览首页人员信息
*  函 数 名：slotHeadPage23
*  功    能：查看首页人员信息
*  说    明：
*  参    数：
*  返 回 值：
*  创 建 人：王磊
*  创建时间：2013-10-18
*  修 改 人：
*  修改时间：
*****************************************************************************/
void PersonManage::slotHeadPage()
{
    //TO DO
    if(1 >= currentPage)
    {
        QMessageBox::information(this, QString::fromUtf8("浏览数据"), QString::fromUtf8("当前页已经是首页"));
        return;
    }

    currentPage = 1;
    getSpecifiedPage(currentPage);

//    ui->labPageInfo->setText(pageInfo.arg(currentPage).arg(totalPage));
}

/*****************************************************************************
*                        查看上一页人员信息
*  函 数 名：slotPreviousPage
*  功    能：槽函数，查看上一页人员信息
*  说    明：
*  参    数：
*  返 回 值：
*  创 建 人：王磊
*  创建时间：2013-10-18
*  修 改 人：
*  修改时间：
*****************************************************************************/
void PersonManage::slotPreviousPage()
{
    //TO DO
    if(1 >= currentPage)
    {
        QMessageBox::information(this, QString::fromUtf8("浏览数据"), QString::fromUtf8("当前页已经是首页"));
        return;
    }

    getSpecifiedPage(--currentPage);
    ui->labPageInfo->setText(pageInfo.arg(currentPage).arg(totalPage));
}

/*****************************************************************************
*                        查看下一页人员信息
*  函 数 名：slotNextPage
*  功    能：槽函数，查看下一页人员信息
*  说    明：
*  参    数：
*  返 回 值：
*  创 建 人：王磊
*  创建时间：2013-10-18
*  修 改 人：
*  修改时间：
*****************************************************************************/
void PersonManage::slotNextPage()
{
    //TO DO
    if(totalPage <= currentPage)
    {
        QMessageBox::information(this, QString::fromUtf8("浏览数据"), QString::fromUtf8("当前页已经是尾页"));
        return;
    }

    getSpecifiedPage(++currentPage);
    ui->labPageInfo->setText(pageInfo.arg(currentPage).arg(totalPage));
}

/*****************************************************************************
*                        查看尾页人员信息
*  函 数 名：slotTailPage
*  功    能：槽函数，查看人员信息尾页
*  说    明：
*  参    数：
*  返 回 值：
*  创 建 人：王磊
*  创建时间：2013-10-18
*  修 改 人：
*  修改时间：
*****************************************************************************/
void PersonManage::slotTailPage()
{
    //TO DO
    if(totalPage <= currentPage)
    {
        QMessageBox::information(this, QString::fromUtf8("浏览数据"), QString::fromUtf8("当前页已经是尾页"));
        return;
    }

    currentPage = totalPage;
    getSpecifiedPage(totalPage);
    ui->labPageInfo->setText(pageInfo.arg(currentPage).arg(totalPage));
}

/*****************************************************************************
*                        获得虹膜注册结果
*  函 数 名：slotGetEnrollResult
*  功    能：槽函数，获得虹膜注册结果处理函数
*  说    明：
*  参    数：
*  返 回 值：
*  创 建 人：王磊
*  创建时间：2013-10-18
*  修 改 人：
*  修改时间：
*****************************************************************************/
void PersonManage::slotGetEnrollResult(bool isSucceed)
{
    //TO DO
//    delete enroll;      //释放注册页面资源
//    enroll = NULL;      //避免重复释放和野指针出现
    if(!isSucceed)
    {
        //TO DO
        return;

    }

    QSqlQuery query;
    query.prepare("UPDATE person SET iris = :irisStatus WHERE workid = :workID");
    query.bindValue(":irisStatus", QString::fromUtf8("已注册").trimmed());
    query.bindValue(":workID", person.id);

    query.exec();
    if(!query.isActive())
    {
        //TO DO
        QMessageBox::warning(this ,QString::fromUtf8("注册虹膜"), QString::fromUtf8("更新注册结果:") + query.lastError().text());
        return;
    }

    showData(this);
}

void PersonManage::slotOrderBy(int index, Qt::SortOrder orderStyle)
{
    //TO DO
    switch(index)
    {
    case 0:
        orderBy = QString("workid");
        break;
    case 1:
        orderBy = QString("personName");
        break;
    case 2:
        orderBy = QString("gender");
        break;
    case 3:
        orderBy = QString("departName");
        break;
    case 4:
        orderBy = QString("iris");
        break;
    case 5:
        orderBy = QString("card");
        break;
    case 6:
        orderBy = QString("note");
        break;
    default:
        orderBy = QString("workid");
        break;
    }

    if(Qt::AscendingOrder == orderStyle)
    {
        orderBy += " ASC";
    }
    else
    {
        orderBy += " DESC";
    }

    queryAll();
    currentPage = 1;
    getSpecifiedPage(currentPage);
}

void PersonManage::show()
{
    QDialog::show();
    showData(this);
}

/*****************************************************************************
*                        显示人员信息
*  函 数 名：showData
*  功    能：显示条件查询人员信息
*  说    明：
*  参    数：
*  返 回 值：
*  创 建 人：王磊
*  创建时间：2013-10-18
*  修 改 人：
*  修改时间：
*****************************************************************************/
void PersonManage::showData(QWidget *parent)
{
    queryAll();
    getSpecifiedPage(currentPage);
}


/*****************************************************************************
*                        获取条件查询的所有记录
*  函 数 名：queryAll
*  功    能：初始化人员信息视图
*  说    明：
*  参    数：
*  返 回 值：所有满足查询条件记录的页数
*  创 建 人：王磊
*  创建时间：2013-10-18
*  修 改 人：
*  修改时间：
*****************************************************************************/
int PersonManage::queryAll()
{
    //TO DO
    QSqlQuery query;
    query.prepare("SELECT count(*)\n"
                  "FROM person AS t1 LEFT JOIN depart AS t2 on t1.departid = t2.id\n"
                  "WHERE t1.workid LIKE :workID OR \n"
                  "   t1.name LIKE :personName OR \n"
                  "   t1.gender LIKE :gender OR \n"
                  "   t2.name LIKE :departName OR \n"
                  "   t1.iris LIKE :irisStatus OR \n"
                  "   t1.card LIKE :cardNum OR \n"
                  "   t1.idnum LIKE :idNum OR \n"
                  "   t1.note LIKE :note \n");


    //构造查询条件表达式
    QString condExpr;
    condExpr = condExpr.sprintf("%%%s%%", ui->txtKeyWords->text().toUtf8().data());

    query.bindValue(":workID", condExpr);
    query.bindValue(":personName", condExpr);
    query.bindValue(":gender", condExpr);
    query.bindValue(":departName", condExpr);
    query.bindValue(":irisStatus", condExpr);
    query.bindValue(":cardNum", condExpr);
    query.bindValue(":idNum", condExpr);
    query.bindValue(":note", condExpr);

    int rows = 0;
    query.exec();
    if(query.isActive())
    {
        if (query.next())
        {
            rows = query.value(0).toInt();
            //获取查询结果总页数
            totalPage = rows / pageSize + (rows % pageSize > 0 ? 1: 0);
        }
        else
        {
            QMessageBox::critical(this, QString::fromUtf8("查询"), QString::fromUtf8("无符合条件的记录"), QMessageBox::Ok);
            return 0;
        }
    }
    else
    {
        QMessageBox::critical(this, QString::fromUtf8("查询"), query.lastError().text(), QMessageBox::Ok);
        return 0;
    }

    return rows;
}


/*****************************************************************************
*                        获取指定的分页数据
*  函 数 名：getSpecifiedPage
*  功    能：获取指定的分页数据
*  说    明：支持模糊查询，任何字段中
*  参    数：toPageNum， 跳转到的页码
*  返 回 值：
*  创 建 人：王磊
*  创建时间：2013-10-18
*  修 改 人：
*  修改时间：
*****************************************************************************/
bool PersonManage::getSpecifiedPage(const int toPageNum)
{
    //TO DO
    QSqlQuery queryPage;

    QString sql =
            "SELECT t1.workid, t1.name as personName, t1.gender, t2.name as departName, \n"
            "   t1.iris, t1.card, t1.note\n"
            "FROM person AS t1 LEFT JOIN depart AS t2 on t1.departid = t2.id \n"
            "WHERE t1.workid LIKE :workID OR \n"
            "   t1.name LIKE :personName OR \n"
            "   t1.gender LIKE :gender OR \n"
            "   t2.name LIKE :departName OR \n"
            "   t1.iris LIKE :irisStatus OR \n"
            "   t1.card LIKE :cardNum OR \n"
            "   t1.idnum LIKE :idNum OR \n"
            "   t1.note LIKE :note \n"
            "ORDER BY " + orderBy + "\n"
            "LIMIT :pageSize OFFSET :pageNum";
    queryPage.prepare(sql);


    QString condExpr;
    condExpr = condExpr.sprintf("%%%s%%", ui->txtKeyWords->text().toUtf8().data()).trimmed();    //获取查询条件

    queryPage.bindValue(":workID", condExpr);
    queryPage.bindValue(":personName", condExpr);
    queryPage.bindValue(":gender", condExpr);
    queryPage.bindValue(":departName", condExpr);
    queryPage.bindValue(":irisStatus", condExpr);
    queryPage.bindValue(":cardNum", condExpr);
    queryPage.bindValue(":idNum", condExpr);
    queryPage.bindValue(":note", condExpr);
    queryPage.bindValue(":pageSize", pageSize);
    queryPage.bindValue(":pageNum", (toPageNum - 1) * pageSize);
    queryPage.exec();

    if(queryPage.isActive())
    {
        if(!queryPage.next())
        {
            QMessageBox::critical(this, QString::fromUtf8("查询"), QString::fromUtf8("无符合条件的记录"), QMessageBox::Ok);
            return false;
        }
        else
        {
            model->setQuery(queryPage);       //同步到数据模型
            ui->labPageInfo->setText(pageInfo.arg(toPageNum).arg(totalPage));     //设置翻页状态

            return true;
        }
    }
    else
    {
        QMessageBox::critical(this, QString::fromUtf8("查询"), /*sql +*/ queryPage.lastError().databaseText(), QMessageBox::Ok);
        return false;
    }
}


//bool PersonManage::isHeadPage()
//{
//    return (1 == currentPage);
//}

//bool PersonManage::isTailPage()
//{
//    return (currentPage == totalPage);
//}
