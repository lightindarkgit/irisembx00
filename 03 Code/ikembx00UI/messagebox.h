/*****************************************************************************
** 文 件 名：QMessageBoxDefine.h
** 主 要 类：QMessageBoxDefine
**
** Copyright (c) 中科虹霸有限公司
**
** 创 建 人：刘中昌
** 创建时间：20013-10-26
**
** 修 改 人：
** 修改时间：
** 描  述:   串口通信数据处理类
** 主要模块说明: 自定义提示对话框--解决button无法显示中文问题
**
** 版  本:   1.0.0
** 备  注:
**
*****************************************************************************/
#ifndef MESSAGEBOX_H
#define MESSAGEBOX_H

#include <QWidget>
#include "QMessageBox"

namespace Ui {
class MessageBox;
}

class MessageBox : public QDialog
{
    Q_OBJECT
    
public:
    explicit MessageBox(QWidget *parent = 0);
    ~MessageBox();
 QMessageBox::StandardButton information (const QString & title, const QString & text,
                                                    QMessageBox::StandardButtons buttons = QMessageBox::Ok,
                                                    QMessageBox::StandardButton defaultButton = QMessageBox::NoButton );
private:
   Ui::MessageBox *ui;
};

#endif // MESSAGEBOX_H
