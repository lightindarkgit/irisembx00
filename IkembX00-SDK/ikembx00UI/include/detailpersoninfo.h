/*****************************************************************************
** 文 件 名：detailpersoninfo.h
** 主 要 类：DetailPersonInfo
**
** Copyright (c) 中科虹霸有限公司
**
** 创 建 人：L.Wang
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
#include "../ikIOStream/interface.h"
#include "../SerialMuxDemux/SerialMuxDemux/include/serialmuxdemux.h"

const QString FACE_IMAGE_FORMAT = "jpg";
QString getFacePhotoName(const QString& pid);
QString getFacePhotoPath();
bool deletFacePhoto(const QString& pid);



namespace Ui {
class DetailPersonInfo;
}

class CapturePhoto;
class QButtonGroup;

class DetailPersonInfo : public QDialog
{
    Q_OBJECT
    
public:
    typedef enum {WKSN = 0, IDNUM, ICNUM}QueryType;

    explicit DetailPersonInfo(QWidget *parent = 0);
    DetailPersonInfo(const PersonInfo& personInfo, QWidget *parent = 0);

    ~DetailPersonInfo();
    void closeThis();
    void setData(const PersonInfo& personInfo);

protected:
    void initSerialMuxDemux();
    void initDeptLst();
    void initUI(bool modifyOpt = false);
    void initSigSlots();

    bool queryDepartIDByName(int& deptId, const QString &deptName);


    //Add a new person infomation
    bool addPersonInfo(const Person &pInfo);
    //Update person information
    bool updatePersonInfo(const Person &pInfo);

    bool checkNoneEmptyItems();
    bool checkIDCard();
    bool checkKeyItemsValid();
    bool checkInPutLength();
    int  getIputLength(QString str); //汉字占4字节

    bool isUsed(const QString& p, QueryType qType);


    //Whether has face photo
    bool hasFacePhoto(const QString& pid);
    std::string getFacePhotoID(const std::string& pid);

    //Save current face photo to database
    bool saveFaceImage(DatabaseAccess& da);
    bool deletFacePhoto(const QString &pid);

    //Display person info and update departlist
    void show();

    // check person workSn
    int checkPersonWorkSn(QString workSn);

    bool IsDataEdited();



signals:
    void sigGoBackFromDP(bool);
    void sigLoadFaceImage();
    void sigCloseCapturePhtoto(bool);
    void sigNewPersonAdded(QString);

protected slots:
    void slotBack(bool isAutoBack = false);
    void slotSave();
    void slotCapturePhoto();
    void slotDeletePhoto();
    void slotManageDepart();
    void slotGetPhoto(QImage);
    void slotGetICCardNum(QString);
    void slotLoadFaceImage();
    void slotMemoTextChanged();
    void slotDeleteICCard();
//    void slotICCardNumChanged();

private slots:
    void on_txtNote_textChanged();

    void on_txtName_textChanged(const QString &arg1);

    void on_cmbDepart_textChanged(const QString &arg1);

    void on_txtIDNum_textEdited(const QString &arg1);

    void on_txtWorkSN_textEdited(const QString &arg1);

    void on_rbtnMale_clicked(bool checked);

    void on_rbtnFamale_clicked(bool checked);

    void on_rbtnUser_clicked(bool checked);

    void on_rbtnAdmin_clicked(bool checked);

private:
    Ui::DetailPersonInfo *ui;
    CapturePhoto _capturePhoto;
    QButtonGroup _btngrpGender;
    QButtonGroup _btnGrpUsrType;
    bool isUpdateOpt;
    SerialMuxDemux* sm;

    bool newImageUnsaved;
    std::string perID;
    std::string workSN;
    std::string IDNum;
    std::string ICNum;
    int namelength;
    int departNamelength;
    int memoLength;
    bool _isDataChanged;
    bool _isMale[2];
    bool _isAdmin[2]; //为了检测f是否  有改动用。。

};

#endif // DETAILPERSONINFO_H
