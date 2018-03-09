#include "xportdata.h"
#include "ui_xportdata.h"
#include "../Common/Exectime.h"

XportData::XportData(QWidget *parent) :
    QWidget(parent),
    _ui(new Ui::XportData)
{
	Exectime etm(__FUNCTION__);
    _ui->setupUi(this);
}

XportData::~XportData()
{
	Exectime etm(__FUNCTION__);
    delete _ui;
}
