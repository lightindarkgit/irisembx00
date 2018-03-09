#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "ikiostream.h"
#include "iimportandexport.h"
#include "../Common/Exectime.h"

MainWindow::MainWindow(QWidget *parent) :
	QMainWindow(parent),
	ui(new Ui::MainWindow)
{
	Exectime etm(__FUNCTION__);
	ui->setupUi(this);

	_thread = std::thread(DealDatagThread1, (void*)this);
}

MainWindow::~MainWindow()
{
	Exectime etm(__FUNCTION__);
	_thread.join();
	delete ui;
}

void * MainWindow::DealDatagThread1(void * arg)
{
	Exectime etm(__FUNCTION__);
	//IImportAndExport isData;
	//IImportAndExport *io = IImportAndExport::GetIImportAndExport();
	return (void*)0;
}

void MainWindow::exportPerson()
{
	Exectime etm(__FUNCTION__);
	//    ikIOStream *io = ikIOStream::GetIkIOStream();
	IImportAndExport *io = IImportAndExport::GetIImportAndExport();
	//io->TestSQL();
	//    std::cout <<io->ExportToFile(ui->linExport->text().toStdString(),TBPerson)<<std::endl;
	//    std::cout <<io->ExportToFile(ui->linExport->text().toStdString(),TBPersonRecLog)<<std::endl;
	//    std::cout <<io->ExportToFile(ui->linExport->text().toStdString(),TBPersonImage)<<std::endl;
	std::cout <<io->ExportToFile(ui->linExport->text().toStdString(),TBIrisDataBase)<<std::endl;
}

void MainWindow::importPerson()
{
	Exectime etm(__FUNCTION__);
	ikIOStream *io = ikIOStream::GetIkIOStream();

	//    io->ImportFromFile(ui->linImport->text().toStdString(),TBPerson);
	//    io->ImportFromFile(ui->linImport->text().toStdString(),TBPersonRecLog);
	//    io->ImportFromFile(ui->linImport->text().toStdString(),TBPersonImage);
	struct timeval t_start,t_end;
	gettimeofday(&t_start,nullptr);
	io->ImportFromFile(ui->linImport->text().toStdString(),TBIrisDataBase);
	gettimeofday(&t_end,nullptr);
	double timediff =(t_end.tv_sec - t_start.tv_sec)* 1000000 + t_end.tv_usec -t_start.tv_usec;
	std::cout<<timediff<<std::endl;
}
























