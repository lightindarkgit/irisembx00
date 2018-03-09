#include "dialogresetpw.h"
#include "ui_dialogresetpw.h"
#include <QTime>
#include <QDate>
#include <QFile>
#include "ikmessagebox.h"
#include "../Common/Exectime.h"

QString _GLO_pnCode   = "";
QString _GLO_CodeDate = "";

dialogResetPW::dialogResetPW( QWidget *parent) :
	QDialog(parent),
	ui(new Ui::dialogResetPW)
{
	Exectime etm(__FUNCTION__);
	ui->setupUi(this);
	//隐藏对话框标题栏及对话框位置调整
	setWindowFlags(Qt::FramelessWindowHint);
	setGeometry(QRect(0, 0, 1024, 600));
	//Fill background
	//this->setAutoFillBackground(true);
	//this->setStyleSheet("background-color:transparent");

	this->setWindowOpacity(1);
	this->setAttribute(Qt::WA_TranslucentBackground);

	ui->centralWgt->setAutoFillBackground(true);
	ui->centralWgt->setBackgroundColor(QColor(230,230,230));
	init();

}

dialogResetPW::~dialogResetPW()
{
	Exectime etm(__FUNCTION__);
	delete ui;
}
void dialogResetPW::init()
{
	Exectime etm(__FUNCTION__);
	QString inputPnCode       = _GLO_pnCode;
	QString codeDate          = _GLO_CodeDate;
	ui->btn_PNcode->hide();
	m_returnType == 0;
	ui->txt_PNcode->setEnabled(false);
	if((inputPnCode != "") && (codeDate == QDate::currentDate().toString("yyyyMMdd")) )
	{
		ui->txt_PNcode->setText(inputPnCode);
		ui->btn_PNcode->setEnabled(false);
	}
	else
	{
		//ui->btn_PNcode->setEnabled(true);
		on_btn_PNcode_clicked();
	}
}



void dialogResetPW::on_btn_PNcode_clicked()
{
	Exectime etm(__FUNCTION__);
	qsrand(QTime(0,0,0).secsTo(QTime::currentTime()));
	int     rand        = qrand()%100000000;
	m_pncode            = QString::number(rand);
	ui->txt_PNcode->setText(m_pncode);
	ui->txt_PNcode->setEnabled(false);

	_GLO_pnCode     = m_pncode;
	_GLO_CodeDate   = QDate::currentDate().toString("yyyyMMdd");

}

void dialogResetPW::on_btnResetPW_clicked()
{
	Exectime etm(__FUNCTION__);
	int PNcode      = ui->txt_PNcode->text().toInt();
	int verifyCode  = ui->txtverificationCode->text().toInt();
	int curdate     = QDate::currentDate().toString("yyyyMMdd").toInt();
	int tempt       = (PNcode + curdate)*3%100000000;
	if(tempt == verifyCode)
	{
		this->accept();
		m_returnType = 1;
		_GLO_pnCode     = "";
		_GLO_CodeDate   = "";

	}
	else
	{
		IKMessageBox errNote(this, QString::fromUtf8("密码重置"), QString::fromUtf8("验证码输入有误"), errorMbx);
		errNote.exec();

	}


}

void dialogResetPW::on_btnCancel_clicked()
{
	Exectime etm(__FUNCTION__);
	this->reject();
	m_returnType = 0;
}
//PNCODE dialogResetPW::getPncode()
//{
//    PNCODE pncode;
//    pncode.pncode = m_pncode;
//    pncode.codedate = QDate::currentDate().toString("yyyyMMdd");
//    return pncode;
//}
