/*****************************************************************************
** 文 件 名：departmanage.h
** 主 要 类：DepartManage
**
** Copyright (c) 中科虹霸有限公司
**
** 创 建 人：王磊
** 创建时间：20013-10-18
**
** 修 改 人：
** 修改时间：
** 描  述:   部门管理
** 主要模块说明: 部门浏览， 增加，修改和删除部门
**
** 版  本:   1.0.0
** 备  注:
**
*****************************************************************************/
#ifndef DEPARTMANAGE_H
#define DEPARTMANAGE_H

#include <QDialog>
#include <QStringListModel>
#include <QSqlTableModel>

namespace Ui {
class DepartManage;
}

class DepartManage : public QDialog
{
    Q_OBJECT
    
public:
    explicit DepartManage(QWidget *parent = 0);
    ~DepartManage();

protected:
    void initDepartListModel();
    void initDepartListWidget();
    void initDepartList();

protected slots:

    void slotBack();
    void slotAdd();
    void slotEdit();
    void slotDelete();
    
private:
    Ui::DepartManage* ui;
    QStringListModel departListModel;
    QSqlTableModel* model;
};

#endif // DEPARTMANAGE_H
