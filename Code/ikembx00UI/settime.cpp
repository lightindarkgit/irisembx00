#include "settime.h"
#include "ui_settime.h"
#include "../Common/Exectime.h"

SetTime::SetTime(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::SetTime)
{
	Exectime etm(__FUNCTION__);
    ui->setupUi(this);
}

SetTime::~SetTime()
{
	Exectime etm(__FUNCTION__);
    delete ui;
}
