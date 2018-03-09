#include "ikmessagebox.h"
#include "ui_ikmessagebox.h"
#include <QtGui>
#include "../Common/Exectime.h"


IKMessageBox::IKMessageBox(QWidget *parent, const QString& title, const QString& info, MessageBoxType type,bool showRadio) :
	QDialog(parent),
	ui(new Ui::IKMessageBox)
{
	
	//Init UI
	ui->setupUi(this);
	ui->labTitle->setText(title);
	ui->labMsgBody->setText(info);
	ui->btnOK->setFocus();
	ui->btnCancel->hide();
	ui->onlineRadio->setHidden(!showRadio);
	ui->offlineRadio->setHidden(!showRadio);
	ui->onlineRadio->setChecked(true);


	//Fill title background
	ui->wgtTitle->setAutoFillBackground(true);
	ui->wgtTitle->setBackgroundColor(QColor(71,71,71));
	ui->labTitle->setStyleSheet("color:white");

    //Hide the title flag
    setAttribute(Qt::WA_TranslucentBackground);
    setWindowFlags(Qt::FramelessWindowHint);


	//Set position
	int topLeftX = (parent->width()- this->width())/2;
	int topLeftY = (parent->height() - this->height())/2;
	this->setGeometry(topLeftX, topLeftY, this->width(), this->height());


	//Set visiable buttons
	setButtons(type);

	//Set note icon
	setIcon(type);


	//Connect signals and corresponding slots
    ui->btnOK->setFocus();

	connect(ui->btnOK, SIGNAL(clicked()), this, SLOT(Accept()));
	connect(ui->btnCancel, SIGNAL(clicked()), this, SLOT(Reject()));
    //add close timer by dj
    connect(&closeTimer,SIGNAL(timeout()),this,SLOT(SlotCount()));
    closeTimer.start(1000);
    _count = 10;
}

void IKMessageBox::Accept()
{
    closeTimer.stop();
	
	if(ui->onlineRadio->isChecked())
	{
		isOnlineMode = true;
	}
	else if(ui->offlineRadio->isChecked())
	{
		isOnlineMode = false;
	}
	accept();
}
void IKMessageBox::Reject()
{
    closeTimer.stop();
	
    reject();
}

void IKMessageBox::SlotCount()
{
    ui->closeCount->setText(QString::number(_count));

    _count --;
    if(_count <0)
    {
        if(ui->btnCancel->isShown())
        {
           Reject();

        }
        else
        {
            Accept();

        }


    }
}

IKMessageBox::~IKMessageBox()
{
	
	delete ui;
}


void IKMessageBox::setIcon(MessageBoxType type)
{
	
	//TO DO
	switch(type)
	{
		case informationMbx:
			dispIcon(":/image/info.ico");
			break;
		case warningMbx:
			dispIcon(":/image/warn.ico");
			break;
		case errorMbx:
			dispIcon(":/image/error.ico");
			break;
		case questionMbx:
			dispIcon(":/image/question.ico");
			break;
		default:
			dispIcon(":/image/info.ico");
			break;
	}
}


void IKMessageBox::dispIcon(const QString& iconFile)
{
	
	//TO DO
	QImage iconImg;
	iconImg.load(iconFile);


	ui->labIcon->setPixmap(QPixmap::fromImage(iconImg.scaled(ui->labIcon->width(), ui->labIcon->height(), Qt::KeepAspectRatio)));
}

void IKMessageBox::setButtons(MessageBoxType type)
{
	
	//TO DO
	if(questionMbx == type)
	{
		ui->btnCancel->show();
	}
}
void IKMessageBox::SetOnlineMode()
{
	
	ui->onlineRadio->setChecked(true);
	ui->offlineRadio->setChecked(false);
}
void IKMessageBox::SetOfflineMode()
{
	
	ui->offlineRadio->setChecked(true);
	ui->onlineRadio->setChecked(false);
}

//void IKMessageBox::paintEvent(QPaintEvent *e)
//{
//    QPainter painter(this);
//    QPainterPath path;

//    path.addRoundRect(this->rect(), 5); //圆角设置

//    //半透明背景
//    painter.fillPath(path, QColor(92, 155, 203, 100));
//}

