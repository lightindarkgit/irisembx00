#include "simplepersoninfo.h"
#include "ui_simplepersoninfo.h"

SimplePersonInfo::SimplePersonInfo(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::SimplePersonInfo)
{
    ui->setupUi(this);
}

SimplePersonInfo::~SimplePersonInfo()
{
    delete ui;
}
