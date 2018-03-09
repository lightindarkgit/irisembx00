/*****************************************************************************
** 文 件 名：detailpersoninfo.h
** 主 要 类：DetailPersonInfo
**
** Copyright (c) 中科虹霸有限公司
**
** 创 建 人：王磊
** 创建时间：20013-10-18
**
** 修 改 人：
** 修改时间：
** 描  述:   人员详细信息
** 主要模块说明: 人员详细信息查看，增加，修改人员信息，人脸照片更换和删除
**
** 版  本:   1.0.0
** 备  注:
**
*****************************************************************************/
#ifndef DETAILPERSONINFO_H
#define DETAILPERSONINFO_H

#include <QDialog>
#include <QButtonGroup>
#include <QSqlRelationalTableModel>
#include "capturephoto.h"
#include "../common.h"
#include "../bin/serialmuxdemux.h"

const QString IMAGE_FORMAT("jpg");


namespace Ui {
class DetailPersonInfo;
}

class CapturePhoto;
class QButtonGroup;

class DetailPersonInfo : public QDialog
{
    Q_OBJECT
    
public:
    explicit DetailPersonInfo(QWidget *parent = 0);
    DetailPersonInfo(const PersonInfo& personInfo, QWidget *parent = 0);

    ~DetailPersonInfo();

protected:
    void initSerialMuxDemux();
    void initDeptLst();
    void initUI(bool modifyOpt = false);
    void initSigSlots();
    void fillData(const PersonInfo& personInfo);
    bool queryDepartIDByName(int& deptId, const QString &deptName);
    bool isCurrentIDExist(const QString &id);
    bool updatePersonInfo();
    bool addPersonInfo();

    bool saveFaceImage();
    void show();

    QString getFacePhotoName();
    QString getFacePhotoPath();

signals:
    void sigGoBack(bool);
    void sigLoadFaceImage();

protected slots:
    void slotBack();
    void slotSave();
    void slotCapturePhoto();
    void slotDeletePhoto();
    void slotManageDepart();
    void slotGetPhoto(QImage);
    void slotGetICCardNum(QString);
    void slotLoadFaceImage();

private:
    Ui::DetailPersonInfo *ui;
    CapturePhoto _capturePhoto;
    QButtonGroup _btngrpGender;
    QButtonGroup _btnGrpUsrType;
    bool isUpdateOpt;
    SerialMuxDemux* sm;

    bool newImageUnsaved;
};

#endif // DETAILPERSONINFO_H
