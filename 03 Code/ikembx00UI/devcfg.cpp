#include <QStringList>
#include <QMessageBox>


#include "devcfg.h"
#include "ui_devcfg.h"
#include "qmessageboxdefine.h"

DevCfg::DevCfg(QWidget *parent) :
    QWidget(parent),
    _ui(new Ui::DevCfg)
{
    _ui->setupUi(this);

    QStringList items;

    items << tr("tty0") << tr("tty1") << tr("tty2") << tr("ttyUSB0") << tr("ttyUSB1") << tr("ttyUSB2");
    _ui->cmbPort->addItems(items);

    items.clear();
    items << tr("38400") << tr("19200") << tr("9600") << tr("4800") << tr("2400");
    _ui->cmbBaud->addItems(items);

    items.clear();
    items << tr("禁用") << tr("奇校验") << tr("偶校验") << tr("无校验");
    _ui->cmbParity->addItems(items);

    items.clear();
    items << tr("8") << tr("7");
    _ui->cmbDataBits->addItems(items);

    items.clear();
    items << tr("1") << tr("0");
    _ui->cmbStopBits->addItems(items);

    connect(_ui->btnSave, SIGNAL(clicked()), this, SLOT(slotSave()));
    connect(_ui->btnCancel, SIGNAL(clicked()), this, SLOT(slotCancel()));
}

DevCfg::~DevCfg()
{
    delete _ui;
}


void DevCfg::slotSave()
{
    //TO DO
    QMessageBox::information(this, tr("保存设备配置信息"), tr("设备配置信息已成功保存"));
    return;
}

void DevCfg::slotCancel()
{
    //TO DO
    _ui->txtIP->setText(tr(""));
    _ui->txtDNS->setText(tr(""));
    _ui->txtMaskCode->setText(tr(""));
    _ui->txtNetGate->setText(tr(""));
}
