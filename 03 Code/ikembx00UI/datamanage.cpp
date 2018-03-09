/*****************************************************************************
** 文 件 名：datamanage.cpp
** 主 要 类：DataManage
**
** Copyright (c) 中科虹霸有限公司
**
** 创 建 人：王磊
** 创建时间：20013-10-18
**
** 修 改 人：
** 修改时间：
** 描  述:   数据管理
** 主要模块说明: 人员信息， 虹膜， 识别记录的导入导出
**
** 版  本:   1.0.0
** 备  注:
**
*****************************************************************************/
#include <QMessageBox>
#include "datamanage.h"
#include "ui_datamanage.h"

DataManage::DataManage(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DataManage)
{
    ui->setupUi(this);

    //连接信号与槽参数，使相应的信号得到想对应的处理
    connect(ui->btnBack, SIGNAL(clicked()), this, SIGNAL(sigGoBack()));
    connect(ui->btnImport, SIGNAL(clicked()), this, SLOT(slotImport()));
    connect(ui->btnExport, SIGNAL(clicked()), this, SLOT(slotExport()));
    connect(ui->chkPersonInfo, SIGNAL(stateChanged(int)), this, SLOT(slotPersonInfoSelectChanged()));
}

DataManage::~DataManage()
{
    delete ui;
}

/*****************************************************************************
*                        获取数据导入导出的选项列表
*  函 数 名：getCheckedItems
*  功    能：获取用户进行数据导入导出的选项列表
*  说    明：
*  参    数：
*  返 回 值：选项列表字符串
*  创 建 人：王磊
*  创建时间：2013-10-18
*  修 改 人：
*  修改时间：
*****************************************************************************/
QString DataManage::getCheckedItems()
{
    //TO DO
    QStringList tipLst;

    if(ui->chkPersonInfo->isChecked())
    {
        tipLst << ui->chkPersonInfo->text();
    }

    if(ui->chkIris->isChecked())
    {
        tipLst << ui->chkIris->text();
    }

    if(ui->chkRecord->isChecked())
    {
        tipLst << ui->chkRecord->text();
    }

    return tipLst.join(QString::fromUtf8("，"));
}

/*****************************************************************************
*                        数据导入操作
*  函 数 名：slotImport
*  功    能：槽函数，导入数据操作处理函数
*  说    明：
*  参    数：
*  返 回 值：
*  创 建 人：王磊
*  创建时间：2013-10-18
*  修 改 人：
*  修改时间：
*****************************************************************************/
void DataManage::slotImport()
{
    //TO DO
    QString tips = getCheckedItems();

    if(tips.isEmpty())
    {
        QMessageBox::critical(this, QString::fromUtf8("数据导入"), QString::fromUtf8("请选择要导入的数据！"));
        return;
    }

    QMessageBox::information(this, QString::fromUtf8("数据导入"), tips + QString::fromUtf8("数据导入成功"));
}

/*****************************************************************************
*                        数据导出操作
*  函 数 名：slotExport
*  功    能：槽函数，导出数据处理函数
*  说    明：
*  参    数：
*  返 回 值：
*  创 建 人：王磊
*  创建时间：2013-10-18
*  修 改 人：
*  修改时间：
*****************************************************************************/
void DataManage::slotExport()
{
    //TO DO
    QString tips = getCheckedItems();

    if(tips.isEmpty())
    {
        QMessageBox::critical(this, QString::fromUtf8("数据导出"), QString::fromUtf8("请选择要导出的数据！"));
        return;
    }

    QMessageBox::information(this, QString::fromUtf8("数据导出"), tips + QString::fromUtf8("数据导出成功"));
}

/*****************************************************************************
*                        人员信息选择状态变化处理函数
*  函 数 名：slotPersonInfoSelectChanged
*  功    能：槽函数，处理人员信息选择状态变化的情况，保证虹膜选项只有在人员信息选中状态下可选
*  说    明：
*  参    数：
*  返 回 值：
*  创 建 人：王磊
*  创建时间：2013-10-18
*  修 改 人：
*  修改时间：
*****************************************************************************/
void DataManage::slotPersonInfoSelectChanged()
{
    if(ui->chkPersonInfo->isChecked())
    {
       ui->chkIris->setCheckable(true);
    }
    else
    {
        ui->chkIris->setChecked(false);
        ui->chkIris->setCheckable(false);
    }
}
