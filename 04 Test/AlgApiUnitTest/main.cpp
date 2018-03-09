#include <QtCore/QCoreApplication>
#include <iostream>
#include "gtest/gtest.h"

#include "VirtualReadImg.h"

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    //AlgApiµ¥Ôª²âÊÔ

    testing::GTEST_FLAG(output) = "xml:";
    testing::GTEST_FLAG(output) = "xml:/home/fpc/MyIKEMB/build-algapiubuntu/result.xml";
    testing::InitGoogleTest(&argc, argv);
    RUN_ALL_TESTS();

    return a.exec();

}
