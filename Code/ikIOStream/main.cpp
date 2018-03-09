#include <QtGui/QApplication>
#include "mainwindow.h"
#include "../Common/Exectime.h"

int main(int argc, char *argv[])
{
	Exectime etm(__FUNCTION__);
	QApplication a(argc, argv);
	MainWindow w;
	w.show();

	return a.exec();
}
