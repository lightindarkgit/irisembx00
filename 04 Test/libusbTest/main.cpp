#include <QtGui/QApplication>
#include "mainwindow.h"
#include "usbtest.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    testing::GTEST_FLAG(output) = "xml:";
    testing::GTEST_FLAG(output) = "xml:/home/fjf/Desktop/result.xml";
    testing::InitGoogleTest(&argc,argv);
    RUN_ALL_TESTS();
    return a.exec();
}
