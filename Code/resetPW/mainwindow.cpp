#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QDate>
#include <QMessageBox>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->txtPNCode->setMaxLength(8);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_btn_verifyCode_clicked()
{

    int rand            = ui->txtPNCode->text().toInt();
    QString curdate     = QDate::currentDate().toString("yyyyMMdd");
    int     curdateNum  = curdate.toInt();
    int     verifyCode  = (rand + curdateNum)*3%100000000;

    ui->txtVerifyCode->setText(QString::number(verifyCode));

}

