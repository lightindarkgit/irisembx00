/*****************************************************************************
** 文 件 名：personmanage.h
** 主 要 类：PersonManage
**
** Copyright (c) 中科虹霸有限公司
**
** 创 建 人：L.Wang
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
#include "../../IKDatabaseLib/include/IKDatabaseLib.h"
#include "../ikIOStream/interface.h"

#include "IKUSBSDK.h"
#include <QTimer>

//虹膜注册UI类
class Enroll;
//网络通讯类
class IkSocket;


namespace Ui {
class PersonManage;
}


class PersonManage : public QDialog
{
    Q_OBJECT
    
public:
    explicit PersonManage(QWidget *parent = 0);
    ~PersonManage();
    void refresh();
    void setInactive();

//    bool isHeadPage();
//    bool isTailPage();
private:
    bool initView();
    bool initEnrollUI();

    //更新显示的数据
    void showData(QWidget *parent = 0);
    //删除选中的人员信息
    int deleteSelected(const QItemSelectionModel* selMdl);

    //删除人员信息
    bool deletePersonInfo(DatabaseAccess& dbConn, const QString& pid);

    //删除虹膜特征
    bool deleteIrisData(DatabaseAccess& dbConn, const QString& pid);

    //删除人脸图像
    bool deletePersonImage(DatabaseAccess& dbConn, const QString& pid);
    //删除识别记录
    bool deleteIdenRecord(DatabaseAccess& dbConn, const QString& pid);

    //查询所有记录的页数
    int queryAll();
    //获取指定分页
    bool getSpecifiedPage(const int toPageNum);

    void goToHeadPage();

    bool findBy(const QString &key, std::vector<PersonDetail> &persons,int num = 0);

    int findByPersonID(QString key,std::string persongID);

    QString AddEscapeInKey(QString key);

    bool dispPageHasGivenRecord(const std::string &recPersonID, const int pageSize);

    template<typename InputIterator>
    void dispData(InputIterator beg, InputIterator end);

signals:
    void sigGoBack();
    void sigSetInActive(bool isAutoBack);
    void sigShowEnrollCameraImg(IKEnrIdenStatus enrIdenStatus); //dj add for sdk

protected slots:
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
    void slotOrderBy(int index, Qt::SortOrder orderStyle);

    void slotNetworkStateChanged(bool availableState);
    void slotNewPersonAdded(QString);
    void slotUpdatePersonInfo();

    //@ add for sdk
    //接受识别界面发送的sdk数据 并将数据发给私有成员 enroll供enroll模块显示摄像头数据
    void slotShowCameraImage(IKEnrIdenStatus enrIdenStatus);


    //Mask 'Esc' key event
    void reject();

private:
    Ui::PersonManage *ui;
    enum{pageSize = 10};
    int currentPage;
    int totalPage;
    QString pageInfo;

    QSqlQueryModel* model;
    QSqlRelationalTableModel* rmodel;
    QHeaderView* headView;
//    Enroll* enroll;
    PersonInfo person;
    DatabaseAccess acc;
    QString orderBy;
    QString orderDirection;
    bool isFindOpt;
    bool isNetAvailable;
    IkSocket* socket;
    Enroll* enroll;

    bool isFindByManual;
    bool isStartEnroll;
    QTimer updatePerinfoTimer;
    QString _updateID;
};

#endif // PERSONMANAGE_H
