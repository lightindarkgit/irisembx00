#include "ikmessagebox.h"
#include "ui_ikmessagebox.h"
#include <QtGui>
#include "../Common/Exectime.h"

//MessageBox::MessageBox(QWidget *parent, const QString& title, const QString& info, MessageBoxType type) :
//    QDialog(parent),
//    ui(new Ui::IKMessageBox)
//{
//    ui->setupUi(parent);
//    ui->labTitle->setText(title);
//    ui->labMsgBody->setText("这是一个测试消息框程序，只是用于测试消息框的外观和操作方便性，正如你看到的，消息的内容要比一般的消息长度长很多，但实际并非如此!");
//    ui->btnOK->setFocus();

//    setWindowFlags(Qt::FramelessWindowHint);

//    QPalette palette;
//    palette.setColor(QPalette::Window, "rgb(200,200,200)");
//    ui->labTitle->setStyleSheet("background-color: red");

//    connect(ui->btnOK, SIGNAL(clicked()), this, SLOT(accept()));
//    connect(ui->btnCancel, SIGNAL(clicked()), this, SLOT(reject()));
//}

IKMessageBox::IKMessageBox(QWidget *parent, const QString& title, const QString& info, MessageBoxType type,bool showRadio) :
	QDialog(parent),
	ui(new Ui::IKMessageBox)
{
	Exectime etm(__FUNCTION__);
	//Init UI
	ui->setupUi(this);
	ui->labTitle->setText(title);
	ui->labMsgBody->setText(info);
	ui->btnOK->setFocus();
	ui->btnCancel->hide();
	ui->onlineRadio->setHidden(!showRadio);
	ui->offlineRadio->setHidden(!showRadio);
	ui->onlineRadio->setChecked(true);

	//ui->labIcon->setAttribute(Qt::WA_TranslucentBackground);
	//ui->labIcon->setStyleSheet("background:transparent");

	//Fill background
	ui->foreFrame->setAutoFillBackground(true);
	ui->foreFrame->setBackgroundColor(QColor(230,230,230));

	//Fill title background
	ui->wgtTitle->setAutoFillBackground(true);
	ui->wgtTitle->setBackgroundColor(QColor(71,71,71));
	ui->labTitle->setStyleSheet("color:white");

	setWindowFlags(Qt::FramelessWindowHint);
	setAttribute(Qt::WA_TranslucentBackground);




	//Set position
	int topLeftX = (parent->width()- this->width())/2;
	int topLeftY = (parent->height() - this->height())/2;
	this->setGeometry(topLeftX, topLeftY, this->width(), this->height());

	//Hide the title flag
	setWindowFlags(Qt::FramelessWindowHint);

	//Set visiable buttons
	setButtons(type);

	//Set note icon
	setIcon(type);

	//    //Set dialog as modal dialog
	//    setModal(true);

	//Set the background color for title
	//    QPalette palette;
	//    palette.setColor(QPalette::Window, "rgb(200,200,200)");
	//    ui->labTitle->setStyleSheet("background-color: red");

	//Connect signals and corresponding slots
	connect(ui->btnOK, SIGNAL(clicked()), this, SLOT(Accept()));
	connect(ui->btnCancel, SIGNAL(clicked()), this, SLOT(Reject()));
}

void IKMessageBox::Accept()
{
	Exectime etm(__FUNCTION__);
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
	Exectime etm(__FUNCTION__);
	reject();
}

IKMessageBox::~IKMessageBox()
{
	Exectime etm(__FUNCTION__);
	delete ui;
}


void IKMessageBox::setIcon(MessageBoxType type)
{
	Exectime etm(__FUNCTION__);
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
	Exectime etm(__FUNCTION__);
	//TO DO
	QImage iconImg;
	iconImg.load(iconFile);


	ui->labIcon->setPixmap(QPixmap::fromImage(iconImg.scaled(ui->labIcon->width(), ui->labIcon->height(), Qt::KeepAspectRatio)));
}

void IKMessageBox::setButtons(MessageBoxType type)
{
	Exectime etm(__FUNCTION__);
	//TO DO
	if(questionMbx == type)
	{
		ui->btnCancel->show();
	}
}
void IKMessageBox::SetOnlineMode()
{
	Exectime etm(__FUNCTION__);
	ui->onlineRadio->setChecked(true);
	ui->offlineRadio->setChecked(false);
}
void IKMessageBox::SetOfflineMode()
{
	Exectime etm(__FUNCTION__);
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

