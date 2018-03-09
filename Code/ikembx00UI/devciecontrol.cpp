#include "devciecontrol.h"
#include "ui_devciecontrol.h"
#include "../Common/Exectime.h"

DevcieControl::DevcieControl(QWidget *parent) :
	QWidget(parent),
	_ui(new Ui::DevcieControl)
{
	Exectime etm(__FUNCTION__);
	_ui->setupUi(this);
}

DevcieControl::~DevcieControl()
{
	Exectime etm(__FUNCTION__);
	delete _ui;
}
