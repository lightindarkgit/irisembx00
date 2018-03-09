#include <QMessageBox>
//the followings are for testing
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QSqlQueryModel>
#include <QSqlTableModel>
#include <QSqlRelationalTableModel>
#include <QTableView>
//end for test


#include "mngperson.h"
#include "ui_mngperson.h"
#include "addperson.h"

//引用注册模块的头文件
#include "enroll.h"



MngPerson::MngPerson(QWidget *parent) :
    QWidget(parent),
    _ui(new Ui::MngPerson)
{
    _ui->setupUi(this);

    InitDataTable(this);

    connect(_ui->btnAdd, SIGNAL(clicked()), this, SLOT(slotAddEditPersonInfo()));
    connect(_ui->btnEdit, SIGNAL(clicked()), this, SLOT(slotAddEditPersonInfo()));
    connect(_ui->btnDelete, SIGNAL(clicked()), this, SLOT(slotDeletePersonInfo()));
    connect(_ui->btnSearch, SIGNAL(clicked()), this, SLOT(slotSearchPersonInfo()));

    connect(_ui->btnEnrollIris, SIGNAL(clicked()), this, SLOT(slotEnrollIris()));
}

MngPerson::~MngPerson()
{
    delete _ui;
}

void MngPerson::slotSearchPersonInfo()
{
    //TO DO
}

void MngPerson::slotAddEditPersonInfo()
{
    AddPerson add_person(this);
    add_person.exec();
}

void MngPerson::slotDeletePersonInfo()
{
    //TO DO
}

void MngPerson::slotEnrollIris()
{
    //TO DO
    Enroll enroll(this);
    enroll.exec();
}


void MngPerson::InitDataTable(QWidget *parent)
{
    QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName("personinfo.db");
    if(!db.open())
    {
        QMessageBox::critical(parent, tr("连接数据库"), tr("数据库打开失败！"));
        return;
    }
    //QMessageBox::information(parent, tr("连接数据库"), tr("数据库打开成功！"));



    bool rtn = false;
    QSqlQuery query;
    rtn = query.exec(QString("create table person(workid int primary key, name varchar,"
                       "gender varchar, depart varchar, card varchar, iris varchar, note varchar)"));
    rtn = query.exec(QString("insert into person values(1101, '张三', '男', '技术部', '无', '无', '')"));
    rtn = query.exec(QString("insert into person values(1102, '夏美', '女', '文艺部', '有', '无', '主持节目')"));
    rtn = query.exec(QString("insert into person values(1103, '许诺', '女', '财经科', '无', '无', '会使用算盘')"));
    rtn = query.exec(QString("insert into person values(1105, '李斯', '男', '会计部', '有', '有', '经验丰富')"));
    rtn = query.exec(QString("insert into person values(1108, '王五', '男', '生产部', '有', '无', '')"));
    rtn = query.exec(QString("insert into person values(1121, '赵克', '男', '体育部', '有', '有', '马拉松冠军')"));
    rtn = query.exec(QString("insert into person values(1179, '郑明', '男', '采购部', '无', '有', '谈判经验丰富')"));

    QSqlQueryModel *model = new QSqlQueryModel(this);
    model->setQuery("select * from person");
    model->setHeaderData(0, Qt::Horizontal, tr("工号"));
    model->setHeaderData(1, Qt::Horizontal, tr("姓名"));
    model->setHeaderData(2, Qt::Horizontal, tr("性别"));
    model->setHeaderData(3, Qt::Horizontal, tr("部门"));
    model->setHeaderData(4, Qt::Horizontal, tr("定位卡"));
    model->setHeaderData(5, Qt::Horizontal, tr("虹膜"));
    model->setHeaderData(6, Qt::Horizontal, tr("备注"));

    //_ui->tableView->setColumnWidth(0, 90);
    //_ui->tableView->setColumnWidth(1, 150);
    //_ui->tableView->setColumnWidth(2, 30);
    _ui->tableView->setColumnWidth(3, 200);
    //_ui->tableView->setColumnWidth(4, 30);
    //_ui->tableView->setColumnWidth(5, 30);
    //_ui->tableView->setColumnWidth(6, 150);

    _ui->tableView->setModel(model);
}
