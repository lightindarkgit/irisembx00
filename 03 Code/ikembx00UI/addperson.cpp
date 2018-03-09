#include <QMessageBox>

#include "addperson.h"
#include "ui_addperson.h"

#include "enroll.h"
#include "qmessageboxdefine.h"

//#include "enrolliris.h"
//#include "ui_enrolliris.h"


AddPerson::AddPerson(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::AddPerson)
{
    ui->setupUi(this);

    _enroll = new Enroll(this);    //创建Enroll对象

    QStringList items;           //为combobox控件添加选项的临时对象

    items.clear();
    items << tr("男") << tr("女");
    ui->cmbGender->addItems(items);    //设置性别选项列表

    //清空定位卡和虹膜信息
    ui->txtCard->setText(tr("无"));
    ui->txtIris->setText(tr("无"));

    ui->btnAsUser->setChecked(true);
    ui->btnAsAdmin->setChecked(false);

    //关联信号与槽
    //connect(ui->btnAddPhoto, SIGNAL(clicked()), this, SLOT(slotAddPhoto()));
    //connect(ui->btnDeletePhoto, SIGNAL(clicked()), this, SLOT(slotDeletePhoto()));

    connect(ui->btnAddCard, SIGNAL(clicked()), this, SLOT(slotAddCard()));
    connect(ui->btnDeleteCard, SIGNAL(clicked()), this, SLOT(slotDeletCard()));

    connect(ui->btnEnrollIris, SIGNAL(clicked()), this, SLOT(slotEnrollIris()));
    connect(ui->btnDeleteIris, SIGNAL(clicked()), this, SLOT(slotDeleteIris()));

    connect(ui->btnSave, SIGNAL(clicked()), this, SLOT(slotSave()));
    connect(ui->btnCancel, SIGNAL(clicked()), this, SLOT(slotCancel()));
    connect(_enroll, SIGNAL(sigEnrollResult(bool)), this, SLOT(slotGetEnrollResult(bool)));
}

AddPerson::~AddPerson()
{
    delete ui;
}


void AddPerson::slotAddCard()
{
    //TO DO
}


void AddPerson::slotDeletCard()
{
    //TO DO
}

void AddPerson::slotEnrollIris()
{
    if(NULL == _enroll)
        _enroll = new Enroll(this);
    _enroll->exec();
}

void AddPerson::slotDeleteIris()
{
    //TO DO
}

void AddPerson::slotSave()
{
    //TO DO
    QMessageBox::information(this, tr("保存人员信息"), tr("人员信息已成功保存"));
    return;
}


void AddPerson::slotCancel()
{
    //TO DO
    switch(QMessageBox::question(this, tr("退出警告"), tr("确定要退出吗?"), QMessageBox::Ok|QMessageBox::Cancel, QMessageBox::Ok))
    {
    case QMessageBox::Ok:
        close();
        break;
    case QMessageBox::Cancel:
    default:
        break;
    }
    return;
}

void AddPerson::slotGetEnrollResult(bool sigEnrollResult)
{
    if(sigEnrollResult)
    {
        QMessageBox::information(this, tr("虹膜注册"), tr("注册成功"));
        _enroll->close();
        return;
    }
    else
    {
        QMessageBox::critical(this, tr("虹膜注册"), tr("注册失败，请重新注册"));
        //_enroll->close();
        return;
    }
}
