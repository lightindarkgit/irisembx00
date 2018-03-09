#include "devciecontrol.h"
#include "ui_devciecontrol.h"

DevcieControl::DevcieControl(QWidget *parent) :
    QWidget(parent),
    _ui(new Ui::DevcieControl)
{
    _ui->setupUi(this);
}

DevcieControl::~DevcieControl()
{
    delete _ui;
}
