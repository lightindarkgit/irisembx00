/*****************************************************************************
** 文 件 名：DialogLogin.h
** 主 要 类：DialogLogin
**
** Copyright (c) 中科虹霸有限公司
**
** 创 建 人：刘中昌
** 创建时间：20013-10-15
**
** 修 改 人：
** 修改时间：
** 描  述:   登陆对话框
** 主要模块说明: 管理员登陆验证
**
** 版  本:   1.0.0
** 备  注:
**
*****************************************************************************/
#ifndef DIALOGLOGIN_H
#define DIALOGLOGIN_H

#include <QDialog>
#include "QDragEnterEvent"
#include "QDragLeaveEvent"
#include "QDragMoveEvent"
#include "QPoint"
#include "QTimer"

#include <dialogresetpw.h>

#include "../databaseLib/IKDatabaseLib/include/IKDatabaseLib.h"


namespace Ui {
class DialogLogin;
}

class DialogLogin : public QDialog
{
    Q_OBJECT
    
public:
    explicit DialogLogin(QWidget *parent = 0);
    ~DialogLogin();
    void move ( int, int );
    void move (const QPoint &);
signals:
    //信号，登陆是否成功
   void sigResult(bool isSucceed);



private slots:
    void judgeUser();
    void cancel();
    void showEvent(QShowEvent *);
    void reStartTime(QString );
    void closeDialog();
    void timerEvent(QTimerEvent *);
    void GetFocused();

    void on_btnResetPw_clicked();

private:
    void SetStyleSheet(bool normalStyle);

private:
    Ui::DialogLogin *ui;
    int _countdown;
    PNCODE m_StorePncode;

    //数据库操作类
    DatabaseAccess _dbAccess;
};

#endif // DIALOGLOGIN_H
