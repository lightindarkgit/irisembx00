#include <QTextCodec>
#include <QToolButton>
#include <QPushButton>
#include <QSpacerItem>
#include <QMessageBox>


//用户定义的头文件
#include "mngform.h"
#include "ui_mngform.h"
#include "mngperson.h"
#include "devcfg.h"
#include "xportdata.h"
#include "devciecontrol.h"


MngForm::MngForm(QWidget *parent) :
    QWidget(parent),
    _ui(new Ui::MngForm)
{
    QTextCodec::setCodecForTr(QTextCodec::codecForName("System"));
    QTextCodec::setCodecForLocale(QTextCodec::codecForName("System"));
    QTextCodec::setCodecForCStrings(QTextCodec::codecForName("System"));

    _ui->setupUi(this);


//    QPushButton *pushButton = new QPushButton(_ui->mgrCenter);
//    pushButton->setObjectName(QString::fromUtf8("pushButton"));
//    pushButton->setText("WangLei");

//    _ui->verticalLayout->addWidget(pushButton);

//    QSpacerItem *verticalSpacer = new QSpacerItem(20, 361, QSizePolicy::Minimum, QSizePolicy::Expanding);

//    _ui->verticalLayout->addItem(verticalSpacer);
    //增加人员管理菜单选项
    QVBoxLayout *layout_manage = new QVBoxLayout(_ui->mgrCenter);
    layout_manage->setMargin(0);


    _personMng = new QToolButton;
    _personMng->setText(tr("人员管理"));

    //_departMng = new QToolButton;
    //_departMng->setText(tr("部门管理"));

    layout_manage->addWidget(_personMng);
    //layout_manage->addWidget(_departMng);

    QSpacerItem *verticalSpacer = new QSpacerItem(20, 361, QSizePolicy::Minimum, QSizePolicy::Expanding);
    layout_manage->addItem(verticalSpacer);


    //增加系统选项菜单选项
    QVBoxLayout *layout_sys_opt = new QVBoxLayout(_ui->sysOpts);
    layout_sys_opt->setMargin(0);

    _xportOpt = new QToolButton;            //导入导出子菜单
    _xportOpt->setText(tr("导入导出"));


    _devCtrlOpt = new QToolButton;
    _devCtrlOpt->setText(tr("设备控制"));

    layout_sys_opt->addWidget(_xportOpt);
    layout_sys_opt->addWidget(_devCtrlOpt);
    layout_sys_opt->addItem(verticalSpacer);   //设置顶端对齐

    //增加设备管理菜单选项
    QVBoxLayout *layout_dev_cfg = new QVBoxLayout(_ui->devCfg);
    layout_dev_cfg->setMargin(0);

    _devCfg = new QToolButton;
    _devCfg->setText(tr("设备配置"));

    layout_dev_cfg->addWidget(_devCfg);  //添加设备配置控件到ToolBox的设备配置页
    layout_dev_cfg->addItem(verticalSpacer);

    _ui->tbMenus->layout()->setSpacing(0);    //设置ToolBox页内控件间距


    //连接信号与槽
    connect(_personMng, SIGNAL(clicked()), this, SLOT(slotPersonMng()));
    //connect(_departMng, SIGNAL(clicked()), this, SLOT(slotDepartMng()));
    connect(_xportOpt, SIGNAL(clicked()), this, SLOT(slotXportOpt()));
    connect(_devCtrlOpt, SIGNAL(clicked()), this, SLOT(slotDevCtrlOpt()));
    connect(_devCfg, SIGNAL(clicked()), this, SLOT(slotDevCfg()));


    //初始化人员信息显示视图

    //默认显示人员管理菜单选项
    _pDispWidget = new MngPerson(_ui->wgtMainArea);
    _pDispWidget->show();
}

MngForm::~MngForm()
{
    delete _personMng;
    //delete _departMng;
    delete _xportOpt;
    delete _devCtrlOpt;
    delete _devCfg;

    _pDispWidget->close();     //《Qt Creator快速入门》（北航出版）的相关章节中提到可以不调用delete来释放
    delete _pDispWidget;
    delete _ui;
}


void MngForm::slotPersonMng()
{
    //TO DO
    _pDispWidget->close();
    _pDispWidget = new MngPerson(_ui->wgtMainArea);
    _pDispWidget->show();
}

//void MngForm::slotDepartMng()
//{
//    //TO DO
//}

void MngForm::slotXportOpt()
{
    //TO DO
    _pDispWidget->close();
    _pDispWidget = new XportData(_ui->wgtMainArea);
    _pDispWidget->show();
}

void MngForm::slotDevCtrlOpt()
{
    //TO OD
    _pDispWidget->close();
    _pDispWidget = new DevcieControl(_ui->wgtMainArea);
    _pDispWidget->show();
}

void MngForm::slotDevCfg()
{
    //TO DO
    _pDispWidget->close();
    _pDispWidget = new DevCfg(_ui->wgtMainArea);
    _pDispWidget->show();
}
