#include "settime.h"
#include "ui_settime.h"

SetTime::SetTime(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::SetTime)
{
    ui->setupUi(this);
}

SetTime::~SetTime()
{
    delete ui;
}
