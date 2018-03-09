/*****************************************************************************
** 文 件 名：personmanage.h
** 主 要 类：PersonManage
**
** Copyright (c) 中科虹霸有限公司
**
** 创 建 人：王磊
** 创建时间：20013-10-18
**
** 修 改 人：
** 修改时间：
** 描  述:   人员管理
** 主要模块说明: 人员查询，排序，增加，修改，删除人员信息及虹膜注册，
**
** 版  本:   1.0.0
** 备  注:
**
*****************************************************************************/
#ifndef PERSONMANAGE_H
#define PERSONMANAGE_H

#include <QDialog>
#include <QSqlQuery>
#include <QSqlQueryModel>
#include <QSqlRelationalTableModel>
#include <QItemSelectionModel>
#include <QHeaderView>
#include "../common.h"


class Enroll;

namespace Ui {
class PersonManage;
}


class PersonManage : public QDialog
{
    Q_OBJECT
    
public:
    explicit PersonManage(QWidget *parent = 0);
    ~PersonManage();

//    bool isHeadPage();
//    bool isTailPage();
protected:
    bool initView();
    bool initEnrollUI();

    //更新显示的数据
    void showData(QWidget *parent = 0);
    //删除选中的人员信息
    int deleteSelected(const QItemSelectionModel* selMdl);

    //查询所有记录的页数
    int queryAll();
    //获取指定分页
    bool getSpecifiedPage(const int toPageNum);

//    bool getEnroolPersonInfo();

signals:
    void sigGoBack();

protected slots:
    void show();
    void slotAddNew();
    void slotEdit();
    void slotEnrollIris();
    void slotDelete();
    void slotFind();

    void slotHeadPage();
    void slotPreviousPage();
    void slotNextPage();
    void slotTailPage();
    void slotGetEnrollResult(bool);
    void slotOrderBy(int index, Qt::SortOrder oderStyle);

private:
    Ui::PersonManage *ui;
    enum{pageSize = 10};
    int currentPage;
    int totalPage;

    QString pageInfo;

    QSqlQueryModel* model;
    QSqlRelationalTableModel* rmodel;
    QHeaderView* headView;
    QString orderBy;
    Enroll* enroll;
    PersonInfo person;
};

#endif // PERSONMANAGE_H
