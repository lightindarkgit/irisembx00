

/*****************************************************************************
** 文 件 名： main.h
** 主 要 类： 主程序
**
** Copyright (c) 中科虹霸有限公司
**
** 创 建 人：刘中昌
** 创建时间：20013-10-24
**
** 修 改 人：
** 修改时间：
** 描  述:    项目入口
** 主要模块说明: 项目入口
**
** 版  本:   1.0.0
** 备  注:
**
*****************************************************************************/
//#include <QtGui/QApplication>
#include <QTranslator>
#include "mainwindow.h"
#include <QTextCodec>
#include <QApplication>
#include "keyboardinput.h"

int main(int argc, char *argv[])
{
    QTranslator oTranslator;
    oTranslator.load(":/qt_zh_CN");
    QApplication a(argc, argv);
//    a.setStyle("macintosh");
//    a.installTranslator(&oTranslator);   //汉化消息框按钮


    //增加中文的显示
//    QCoreApplication::addLibraryPath("./plugins");
//    QCoreApplication::addLibraryPath("./qt.plugins");
//    QTextCodec::setCodecForTr(QTextCodec::codecForName("System"));
//    QTextCodec::setCodecForLocale(QTextCodec::codecForName("System"));
//    QTextCodec::setCodecForCStrings(QTextCodec::codecForName("System"));
    QTextCodec::setCodecForTr(QTextCodec::codecForName("UTF-8"));
    QTextCodec::setCodecForLocale(QTextCodec::codecForName("UTF-8"));
    QTextCodec::setCodecForCStrings(QTextCodec::codecForName("UTF-8"));

    //QTextCodec::setCodecForTr(QTextCodec::codecForName("GB18030"));
    KeyboardInput keyboard;
    a.setInputContext(keyboard.allocaInputContext());

    MainWindow w;
    w.show();

    return a.exec();
}

