#include "dialogselectlogin.h"
#include "ui_dialogselectlogin.h"
#include "../Common/Exectime.h"

DialogSelectLogin::DialogSelectLogin(QWidget *parent) :
	QDialog(parent),
	ui(new Ui::DialogSelectLogin)
{
	Exectime etm(__FUNCTION__);
	ui->setupUi(this);
	//Fill background
	ui->centralWgt->setAutoFillBackground(true);
	//    ui->widget->setBackgroundColor(QColor(230,230,230));

	//    ui->widgetTitle->setBackgroundColor(QColor(71,71,71));


	//隐藏对话框标题栏及对话框位置调整
	setWindowFlags(Qt::FramelessWindowHint);
	setGeometry(QRect(0, 0, 1024, 600));

	//    //Fill title background
	//    ui->wgtTitle->setAutoFillBackground(true);
	//    ui->wgtTitle->setBackgroundColor(QColor(71,71,71));
	_countdown = 10;
	this->startTimer(1000);

}

DialogSelectLogin::~DialogSelectLogin()
{
	Exectime etm(__FUNCTION__);
	delete ui;
}

void DialogSelectLogin::selectAccountLogin()
{
	Exectime etm(__FUNCTION__);
	//emit sigAccountLogin();
	accept();
	setResult(Account);
}

void DialogSelectLogin::selectIrisLogin()
{
	Exectime etm(__FUNCTION__);
	accept();
	setResult(Iris);
	//emit sigIrisLogin();
}
int DialogSelectLogin::result() const
{
	Exectime etm(__FUNCTION__);
	return _result;
}
void DialogSelectLogin::setResult(int r)
{
	Exectime etm(__FUNCTION__);
	_result = r;
	//QDialog::setResult(r);
}

/*****************************************************************************
 *                        时间事件函数
 *  函 数 名：timerEvent
 *  功    能：界面消失倒计时
 *  说    明：
 *  参    数：
 *  返 回 值：
 *  创 建 人：刘中昌
 *  创建时间：2014-05-13
 *  修 改 人：
 *  修改时间：
 *****************************************************************************/
void DialogSelectLogin::timerEvent(QTimerEvent *)
{
	Exectime etm(__FUNCTION__);
	if(_countdown>= 0)
	{
		ui->labCountDown->setText(QString::number(_countdown));
		if(_countdown == 0)
		{
			setResult(Cancel);
			reject();
		}
		_countdown--;
	}
}

