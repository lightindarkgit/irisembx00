/*****************************************************************************
** 文 件 名： SuspendBar.h
** 主 要 类： SuspendBar
**
** Copyright (c) 中科虹霸有限公司
**
** 创 建 人：刘中昌
** 创建时间：20013-10-24
**
** 修 改 人：
** 修改时间：
** 描  述:   弹出管理员登陆按钮
** 主要模块说明: 弹出管理员登陆按钮
**
** 版  本:   1.0.0
** 备  注:
**
*****************************************************************************/
#ifndef SUSPENDBAR_H
#define SUSPENDBAR_H

#include <QWidget>

namespace Ui {
class SuspendBar;
}

class SuspendBar : public QWidget
{
    Q_OBJECT
    
public:
    explicit SuspendBar(QWidget *parent = 0);
    ~SuspendBar();
    //void SetShowType(bool isIrisLogin);

 signals:
    void sigAdminLogin();
    void sigIrisLogin();
    void sigIdent();
public slots:
    void showBtnIdent();
    void hideBtnIdent();
private:
    Ui::SuspendBar *ui;
};

#endif // SUSPENDBAR_H
