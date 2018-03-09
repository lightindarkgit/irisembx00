/*****************************************************************************
** 文 件 名：departmanage.h
** 主 要 类：DepartManage
**
** Copyright (c) 中科虹霸有限公司
**
** 创 建 人：L.Wang
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
#include <QtGui>
#include <QModelIndex>
#include "../../../ikIOStream/interface.h"


//class DepartOpt : Depart
//{
//public:
//    enum RecordOpt{noneOpt = 0, depatAdd, departEdit, departDelete};

//    DepartOpt():
//        _opt(noneOpt)
//    {
//    }

////    DepartOpt(const Depart& depart):
////        departID(depart.departID),
////        departName(depart.departName),
////        parentID(depart.parentID),
////        memo(depart.memo),
////        _opt(noneOpt)
////    {
////    }


//    DepartOpt(const Depart& depart)
//    {
//        departID = depart.departID;
//        departName = depart.departName;
//        parentID = depart.parentID;
//        memo = depart.memo;
//        _opt = noneOpt;
//    }

//    int GetID()const
//    {
//        return departID;
//    }

//    std::string GetName()const
//    {
//        return departName;
//    }

//    int GetParentID()const
//    {
//        return parentID;
//    }

//    std::string GetMemo()const
//    {
//        return memo;
//    }

//    RecordOpt GetOpt()const
//    {
//        return _opt;
//    }

//    bool SetID(const int& id)
//    {
//        departID = id;
//        return true;
//    }

//    bool SetName(const std::string& name)
//    {
//        departName = name;
//        return true;
//    }

//    bool SetParentID(const int& id)
//    {
//        parentID = id;
//        return true;
//    }

//    bool SetMemo(const std::string& memo)
//    {
//        this->memo = memo;
//        return true;
//    }

//    bool SetOpt(const RecordOpt& opt)
//    {
//        _opt = opt;
//        return true;
//    }

//private:
////    int departID;
////    std::string departName;
////    int parentID;
////    std::string memo;
//    RecordOpt _opt;
//};

namespace Ui {
class DepartManage;
}




class DepartManage : public QDialog
{
    Q_OBJECT
    
public:
    explicit DepartManage(QWidget *parent = 0);
    ~DepartManage();
    void refresh();

protected:
    void initDepartListWidget();
    void initDepartList();
    void creatNewRecord(int row, const Depart& depart);

    int getDepartID();
    int getMaxDepartIDFromDB();
    int getMaxDepartIDFromLocal();
    bool updateLocal(int row);
    bool alreadInList(const int id);

    bool addDeparts();
    bool editDeparts();
    bool deleteDeparts();

    QModelIndex getNewAddedIndex();

    void mousePressEvent(QMouseEvent *);

protected slots:
    void slotBack();
    void slotAdd();
    void slotEdit();
    void slotDelete();
    void slotSave();
    void slotDepartInfoChanged(QTableWidgetItem*);

    
private:
    enum RecordOpt{noneOpt = 0, departAdd, departEdit, departDelete};
    Ui::DepartManage* ui;
    QModelIndex curEditedIndex;
    QString defaultDeptName;
    QString curDeptName;
    bool isEditting;
    bool hasModified;
    RecordOpt curOpt;
    std::vector<Depart> newDeparts;
    std::vector<Depart> editedDeparts;
    std::vector<Depart> rmedDeparts;
};



#endif // DEPARTMANAGE_H
