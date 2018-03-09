#include "xportdata.h"
#include "ui_xportdata.h"

XportData::XportData(QWidget *parent) :
    QWidget(parent),
    _ui(new Ui::XportData)
{
    _ui->setupUi(this);
}

XportData::~XportData()
{
    delete _ui;
}
