/*****************************************************************************
 ** 文 件 名：detailpersoninfo.cpp
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
//#include <QMessageBox>
#include <QSqlQuery>
#include <QSqlError>
//#include <QImage>
//#include <QPixmap>
//#include <QFile>
//#include <QDir>
//#include <QRegExp>
#include <QtGui>


//以流的方式往字符串中写数据
#include <sstream>

#include <string.h>
#include "detailpersoninfo.h"
#include "ui_detailpersoninfo.h"
#include "departmanage.h"
#include "../common.h"
#include "ikmessagebox.h"
#include "commononqt.h"

#include "ikevent.h"
#include "../serial/serial/include/basictypes.h"
#include "../IrisAppLib/IrisManager/uuidControl.h"
#include "../databaseLib/IKDatabaseLib/include/IKDatabaseLib.h"
#include "dboperationinterface.h"
#include "../IrisAppLib/IrisManager/dbTable.h"
#include "../Common/Logger.h"
#include "../Common/Exectime.h"

#include "iksocket.h"
#include "../IrisAppLib/IrisManager/irisManager.h"
#include "../IrisAppLib/IrisManager/singleControl.h"

//串口封装事件
SerialEvent se;

//读取IC卡卡号线程
void RecvCardNum(const unsigned char* data, const int size)
{

    //TO DO
    QString val, s;
    if(nullptr != data)
    {
        for(int i = 0; i < size; i++)
        {
            //TO DO
            val += s.sprintf("%02x", data[i]);
        }
        printf("卡号: %s\n", val.toUtf8().data());
        se.onGetICCardNum(val);
    }
}


DetailPersonInfo::DetailPersonInfo(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DetailPersonInfo),
    isUpdateOpt(false),
    sm(SerialMuxDemux::GetInstance()),
    newImageUnsaved(false)
{
    Exectime etm(__FUNCTION__);

    initSerialMuxDemux();
    initUI();          //初始化UI
    initSigSlots();    //初始化信号与槽

    namelength = 0;
    departNamelength = 0;
    memoLength = 0;
    _isDataChanged = false;
    //ui->txtName->installEventFilter(this);
//    this->setMouseTracking(true);
//    this->setAttribute(Qt::WA_AcceptTouchEvents);
   // ui->txtName->setAttribute(Qt::WA_AcceptTouchEvents);

    _isAdmin[0] = false;
    _isAdmin[1] = false;
    _isMale[0]  = false;
    _isMale[1] = false;
}

DetailPersonInfo::DetailPersonInfo(const PersonInfo& personInfo, QWidget *parent):
    QDialog(parent),
    ui(new Ui::DetailPersonInfo),
    isUpdateOpt(true),
    sm(SerialMuxDemux::GetInstance()),
    newImageUnsaved(false)
{

    //TO DO
    Exectime etm(__FUNCTION__);
    initSerialMuxDemux();
    initUI(true);          //初始化UI
    initSigSlots();    //初始化信号与槽
    setData(personInfo);
    _isDataChanged = false;

//    ui->txtName->installEventFilter(this);
//    this->setMouseTracking(true);
}

DetailPersonInfo::~DetailPersonInfo()
{

    sm->ReleaseInstance();
    delete ui;
}

/*****************************************************************************
 *                        初始化人串口分发类
 *  函 数 名：initSerialMuxDemux
 *  功    能：初始化串口分发类，并注册读取IC卡的回调函数
 *  说    明：
 *  参    数：
 *  返 回 值：
 *  创 建 人：L.Wang
 *  创建时间：2013-10-18
 *  修 改 人：
 *  修改时间：
 *****************************************************************************/
void DetailPersonInfo::initSerialMuxDemux()
{


    Exectime etm(__FUNCTION__);

    unsigned char cmd = 0x0f;

    if(true != sm->AddCallback(cmd, RecvCardNum))
    {
        std::cout << "[ERROR@" << __FUNCTION__ << "]:" << "enroll callback function for READING IC card failed" << std::endl;
        LOG_ERROR("enroll callback function for READING IC card failed");
    }

    if(E_SERIAL_OK != sm->Init(GetSerialPortName()))
    {
        std::cout << "[ERROR@" << __FUNCTION__ << "]:" << "init serial port failed" << std::endl;
        LOG_ERROR("init serial port failed");
        return;
    }

}

/*****************************************************************************
 *                        初始化人员信息页面
 *  函 数 名：init
 *  功    能：初始化人员详细信息页面
 *  说    明：
 *  参    数：
 *  返 回 值：
 *  创 建 人：L.Wang
 *  创建时间：2013-10-18
 *  修 改 人：
 *  修改时间：
 *****************************************************************************/
void DetailPersonInfo::initDeptLst()
{

    //TO DO
    //清空当前部门列表
    ui->cmbDepart->clear();

    //更新部门列表
    DatabaseAccess dbAc;
    QString query("SELECT departname FROM (SELECT DISTINCT departname FROM person) tmptbl ORDER BY convert_to(departname, E'GBK')");
    std::vector<DepartName> departs;


    if(dbSuccess != dbAc.Query(query.toStdString(), departs))
    {
        LOG_ERROR("Query sql failed. %s",query.toStdString().c_str());
        return;
    }


    DepartName depart;
    //    for(int offset = 0; offset < persons.size(); offset++)
    //    {
    //        person = persons.at(offset);
    //        ui->cmbDepart->addItem(person.departName.c_str());
    //    }

    for(string::size_type offset = 0; offset < departs.size(); offset++)
    {
        depart = departs.at(offset);
        ui->cmbDepart->addItem(QString(depart.value.c_str()).trimmed());
    }


    if(ui->cmbDepart->findText(QString("")) < 0)
    {
        //添加一个默认的空部门
        ui->cmbDepart->addItem(QString(""));
    }

    ui->cmbDepart->setCurrentItem(ui->cmbDepart->findText(""));
}


/*****************************************************************************
 *                        初始化人员信息页面UI
 *  函 数 名：init
 *  功    能：初始化人员详细信息页面UI
 *  说    明：
 *  参    数：
 *  返 回 值：
 *  创 建 人：L.Wang
 *  创建时间：2013-12-17
 *  修 改 人：
 *  修改时间：
 *****************************************************************************/
void DetailPersonInfo::initUI(bool modifyOpt)
{

    //TO DO
    ui->setupUi(this);

    //将单选按钮依分类分组
    _btngrpGender.addButton(ui->rbtnMale);
    _btngrpGender.addButton(ui->rbtnFamale);
    _btnGrpUsrType.addButton(ui->rbtnUser);
    _btnGrpUsrType.addButton(ui->rbtnAdmin);

    //初始化单选按钮的选中状态
    ui->rbtnMale->setChecked(true);
    ui->rbtnFamale->setChecked(false);
    ui->rbtnUser->setChecked(true);
    ui->rbtnAdmin->setChecked(false);

    //设置删除IC卡卡号的按钮为禁用状态
    ui->btnDeleteICCard->setEnabled(false);

    //标识必填项
    ui->labName->setText("姓<span>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;</span>名<font color=red>*</font>");
    ui->labWorkID->setText("工<span>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;</span>号<font color=red>*</font>");

    //设置人员姓名为输入焦点的默认位置
    ui->txtName->setFocus();

    //隐藏对话框标题栏及对话框位置调整和背景填充
    setWindowFlags(Qt::FramelessWindowHint);
    setGeometry(((QDialog*)this->parent())->rect());
    setBackgroundColor(QColor(242,241,240));
    setAttribute(Qt::WA_TranslucentBackground, false);

    //初始化部门列表
    initDeptLst();

    //    //设置是否是人员信息更新
    //    isUpdateOpt = modifyOpt ? true : false;


    //设置输入约束
   // QRegExp inputExpr("([0-9]{15,15})|([0-9]{17}[0-9xX])");
//    ui->txtIDNum->setValidator(new QRegExpValidator(inputExpr, this));

    //inputExpr.setPattern("[^%]{1,64}");
    //    inputExpr.setPattern("{1,64}");
    //	ui->txtName->setValidator(new QRegExpValidator(inputExpr, this));

//    inputExpr.setPattern("[a-zA-Z0-9]{0,32}");
//    ui->txtWorkSN->setValidator(new QRegExpValidator(inputExpr, this));

    //inputExpr.setPattern("^[\u4E00-\u9FFFA-Za-z0-9]{0,64}$");
    //    inputExpr.setPattern("[^%]{1,64}");
    ////    inputExpr.setPattern("{1,64}");
    //	ui->cmbDepart->setValidator(new QRegExpValidator(inputExpr, this));


    ui->btnManageDepart->hide();
    //@@@
    ui->btnCapturePhoto->setEnabled(false);
}


/*****************************************************************************
 *                        初始化人员信息页面信号与槽
 *  函 数 名：init
 *  功    能：初始化人员详细信息页面信号与槽
 *  说    明：
 *  参    数：
 *  返 回 值：
 *  创 建 人：L.Wang
 *  创建时间：2013-12-17
 *  修 改 人：
 *  修改时间：
 *****************************************************************************/
void DetailPersonInfo::initSigSlots()
{

    //TO DO
    connect(ui->btnBack, SIGNAL(clicked()), this, SLOT(slotBack()));
    connect(ui->btnSave, SIGNAL(clicked()), this, SLOT(slotSave()));
    connect(ui->btnCapturePhoto, SIGNAL(clicked()), this, SLOT(slotCapturePhoto()));
    connect(ui->btnDeletePhoto, SIGNAL(clicked()), this, SLOT(slotDeletePhoto()));
    connect(ui->btnManageDepart,SIGNAL(clicked()), this, SLOT(slotManageDepart()));
    connect(&_capturePhoto, SIGNAL(sigImgaeCaptured(QImage)), this, SLOT(slotGetPhoto(QImage)));
    connect(&se,SIGNAL(GetICCrad(QString)), this, SLOT(slotGetICCardNum(QString)));          //读取IC卡卡号槽函数
    connect(ui->btnDeleteICCard, SIGNAL(clicked()), this, SLOT(slotDeleteICCard()));         //删除IC卡卡号

    connect(this, SIGNAL(sigLoadFaceImage()), this, SLOT(slotLoadFaceImage()));
    connect(ui->txtNote, SIGNAL(textChanged()), this, SLOT(slotMemoTextChanged()));
}


/*****************************************************************************
 *                        人员信息数据显示
 *  函 数 名：setData
 *  功    能：初始化人员详细信息页面
 *  说    明：
 *  参    数：personInfo，人员信息简要
 *  返 回 值：
 *  创 建 人：L.Wang
 *  创建时间：2013-12-17
 *  修 改 人：
 *  修改时间：
 *****************************************************************************/
void DetailPersonInfo::setData(const PersonInfo& personInfo)
{

    //TO DO
    if(nullptr == ui)
    {
        return;
    }


    //记录当前人员信息的personID
    perID = personInfo.id;
    QString query("SELECT t1.personid, t1.name, t1.sex, t1.idcard, t1.superid, t1.cardid, t1.worksn, "
                  "t1.disabletime, t1.departid, t1.departname, t1.hasirisdata, t1.memo "
                  "FROM person AS t1 "
                  "WHERE t1.personid = '%1'");
    query = query.arg(personInfo.id.c_str());

    DatabaseAccess dbAcc;
    std::vector<Person> persons;  //查询结果只有一条，不会溢出


    //验证查询结果
    if(dbSuccess != dbAcc.Query(query.toStdString(), persons))
    {
        IKMessageBox warnNote(this, QString::fromUtf8("查询"), QString::fromUtf8("查询人员详细信息失败！"), warningMbx);

        warnNote.exec();
        this->close();

        return;
    }

    if(persons.size())
    {
        Person& person = *(persons.begin());

        //保存当前工号、身份证号和卡号
        workSN = person.workSn;
        IDNum = person.IDcard;
        ICNum = person.cardID;

        //设置人员姓名
        ui->txtName->setText(QString::fromUtf8(person.name.c_str()).trimmed());

        //设置性别
        bool isMale = QString::fromUtf8(person.sex.c_str()).contains(QString::fromUtf8("男"), Qt::CaseInsensitive);
        isMale ? (ui->rbtnMale->setChecked(true), ui->rbtnFamale->setChecked(false)) :
                 (ui->rbtnMale->setChecked(false), ui->rbtnFamale->setChecked(true));
        _isMale[0] = isMale;
        _isMale[1] = isMale;

        //设置身份证号
        ui->txtIDNum->setText(QString(person.IDcard.c_str()).trimmed());

        //设置用户类型
        PERSON_USER == person.superID ? (ui->rbtnUser->setChecked(true), ui->rbtnAdmin->setChecked(false)) :
                                     (ui->rbtnUser->setChecked(false), ui->rbtnAdmin->setChecked(true));

        _isAdmin[0]  =  (PERSON_ADMIN == person.superID);
        _isAdmin[1]  = _isAdmin[0];

        //设置工号
        ui->txtWorkSN->setText(QString(person.workSn.c_str()).trimmed());
        //设置部门
        ui->cmbDepart->setCurrentText(QString::fromUtf8(person.departName.c_str()));          //设置部门名称
        //设置虹膜是否已注册
        ui->labIrisStatusValue->setText(QString::fromUtf8("%1").arg(person.hasIrisData > 0 ? "是" : "否"));
        //设置IC卡号
        if(!person.cardID.empty())
        {
            ui->btnDeleteICCard->setEnabled(true);
        }

        //        long icCardNum = QString::fromUtf8(person.cardID.c_str()).trimmed().toLong(NULL, 16);
        ui->labICCardNum->setText(person.cardID.c_str());

        //设置备注
        ui->txtNote->setText(QString::fromUtf8(person.memo.c_str()).trimmed());

        //加载人脸图像
        emit this->sigLoadFaceImage();
    }
    else
    {
        IKMessageBox warnNote(this, QString("获取人员信息"), QString::fromUtf8("未能从数据库中获取到该人员的信息！"), warningMbx);

        warnNote.exec();
        this->close();
    }
}

/*****************************************************************************
 *                        返回操作
 *  函 数 名：slotBack
 *  功    能：槽函数， 从人员详细信息页面返回到人员管理页面的操作处理函数
 *  说    明：
 *  参    数：
 *  返 回 值：
 *  创 建 人：L.Wang
 *  创建时间：2013-10-18
 *  修 改 人：
 *  修改时间：
 *****************************************************************************/
void DetailPersonInfo::slotBack(bool isAutoBack)
{
    emit sigCloseCapturePhtoto(isAutoBack);

    bool isDataEdited = IsDataEdited();
    bool isAdminEdited = (_isAdmin[0] !=_isAdmin[1]);
    bool isSexEdited = (_isMale[0] !=_isMale[1]);
    if((isDataEdited || isAdminEdited || isSexEdited)&& !isAutoBack)
    {
        IKMessageBox Note(this, QString::fromUtf8("提示"), QString::fromUtf8("修改人员数据未保存，请确认是否退出"), questionMbx);
        Note.exec();
        if(QDialog::Rejected == Note.result() )
        {
            return;
        }

    }
    //TO DO

    close();
}


/*****************************************************************************
 *                        保存人员信息
 *  函 数 名：slotSave
 *  功    能：槽函数，保存人员信息操作处理函数
 *  说    明：
 *  参    数：
 *  返 回 值：
 *  创 建 人：L.Wang
 *  创建时间：2013-10-18
 *  修 改 人：
 *  修改时间：
 *****************************************************************************/
void DetailPersonInfo::slotSave()
{



    //ui->btnSave->setEnabled(false);

    if(!checkInPutLength())
    {
        ui->btnSave->setEnabled(true);
        return ;
    }
    //TO DO
    //Check the items whether empty
    if(!checkNoneEmptyItems())
    {
        //必选项为空
        ui->btnSave->setEnabled(true);
        return;
    }

    //Check ID Card number format
    if(!checkIDCard())
    {
        //TO DO
        ui->btnSave->setEnabled(true);
        return;
    }


    //Check person information conflict
    if(!checkKeyItemsValid())
    {
        ui->btnSave->setEnabled(true);
        return;
    }


    //获取当前部门ID
    int departId;
    if(!queryDepartIDByName(departId, ui->cmbDepart->currentText()))
    {

        ui->btnSave->setEnabled(true);
        LOG_ERROR("queryDepartIDByName failed");
        return;
    }


    Person person;

    //获取性别
    QString sex = ui->rbtnFamale->isChecked() ? QString::fromUtf8("女") : QString::fromUtf8("男");
    //是否为管理员
    int personType = ui->rbtnAdmin->isChecked() ? PERSON_ADMIN : PERSON_USER;
    //获取人员的其它信息
    person.name = ui->txtName->text().trimmed().toStdString();
    person.sex = sex.trimmed().toStdString();
    person.IDcard = ui->txtIDNum->text().trimmed().toStdString();
    person.superID = personType;
    person.workSn = ui->txtWorkSN->text().trimmed().toStdString();

    person.departID = departId;
    person.departName = ui->cmbDepart->currentText().trimmed().toStdString();
    //person.hasIrisData = ui->labIrisStatusValue->text().trimmed().toStdString();
    person.cardID = ui->labICCardNum->text().trimmed().toStdString();
    person.memo = ui->txtNote->text().trimmed().toStdString();

    //操作结果提示
    QString msgBoxTitle = isUpdateOpt ? QString::fromUtf8("更新人员信息") : QString::fromUtf8("添加人员信息");

    if(isUpdateOpt)
    {
        //Update person information to database
        person.personID = perID;
        //@20170119
        //		std::vector<Person> pers;
        //		pers.push_back(person);

        if(!updatePersonInfo(person))
        {
            IKMessageBox errorNote(this, msgBoxTitle, QString::fromUtf8("更新人员操作失败！"), errorMbx);

            errorNote.exec();
            ui->btnSave->setEnabled(true);
            return;
        }
    }
    else
    {
        //Add the new person informatin to database
        UuidControl uc;
        perID = uc.GetUUIDString();
        person.personID = perID;

        //@20170119
        //		std::vector<Person> pers;
        //		pers.push_back(person);

        if(!addPersonInfo(person))
        {
            IKMessageBox errorNote(this, msgBoxTitle, QString::fromUtf8("添加人员操作失败！"), errorMbx);

            errorNote.exec();
            return;
        }

        //@emit sigNewPersonAdded(perID.c_str());
    }


    if(IkSocket::GetIkSocketInstance()->isSocketConnet())
    {
        IkSocket::GetIkSocketInstance()->UploadTemp();
    }
    else
    {
        LOG_INFO("网络未连接到服务器");
    }


    ////	// add by lius
    //    IkSocket::GetIkSocketInstance()->UploadTemp();

    //存储结果提示
    IKMessageBox infoNote(this, msgBoxTitle, QString::fromUtf8("操作成功！"), informationMbx);
    infoNote.exec();
    _isDataChanged = false;




    emit sigNewPersonAdded(perID.c_str());
    close();     //关闭当前窗口
    ui->btnSave->setEnabled(true);
}



/*****************************************************************************
 *                        采集人脸照片
 *  函 数 名：slotCapturePhoto
 *  功    能：槽函数，处理采集人脸照片操作
 *  说    明：
 *  参    数：
 *  返 回 值：
 *  创 建 人：L.Wang
 *  创建时间：2013-10-18
 *  修 改 人：
 *  修改时间：
 *****************************************************************************/
void DetailPersonInfo::slotCapturePhoto()
{

    //TO DO
    //初始化人脸图像采集设备
    if(!_capturePhoto.initCamera())
    {
        IKMessageBox errorNote(this, QString::fromUtf8("人脸图像采集"), QString::fromUtf8("人脸摄像头设备初始化失败！"), errorMbx);
        errorNote.exec();
        return;
    }

    //设置人脸图像人员信息
    connect(this, SIGNAL(sigCloseCapturePhtoto(bool)), &_capturePhoto, SLOT(slotBack(bool)));
    usleep(400000);
    _capturePhoto.exec();
    _isDataChanged = _capturePhoto.IsPhotoSaved();
}

/*****************************************************************************
 *                        读取IC卡卡号
 *  函 数 名：slotGetICCardNum
 *  功    能：
 *  说    明：
 *  参    数：
 *  返 回 值：
 *  创 建 人：L.Wang
 *  创建时间：2014-01-16
 *  修 改 人：
 *  修改时间：
 *****************************************************************************/
void DetailPersonInfo::slotGetICCardNum(IN QString cardNum)
{

    //TO DO
    //icCardNum = cardNum.trimmed().toStdString();
    //ICNum = ConvertCardNumToDec(cardNum).toStdString();      //更新当前的卡号，以便于存储到数据库, 以十进制方式存储
    _isDataChanged = true;
    ui->labICCardNum->setText(ConvertCardNumToDec(cardNum));
    if(!cardNum.isEmpty())
    {
        ui->btnDeleteICCard->setEnabled(true);
    }
}


/*****************************************************************************
 *                        删除人脸照片
 *  函 数 名：slotDeletePhoto
 *  功    能：槽函数，删除人脸照片操作处理函数
 *  说    明：
 *  参    数：
 *  返 回 值：
 *  创 建 人：L.Wang
 *  创建时间：2013-10-18
 *  修 改 人：
 *  修改时间：
 *****************************************************************************/
void DetailPersonInfo::slotDeletePhoto()
{


    //TO DO
    if(nullptr == ui->labFacePhoto->pixmap())
    {
        LOG_ERROR("nullptr == ui->labFacePhoto->pixmap()");
        return;
    }

    //是否删除人脸图像
    IKMessageBox questionNote(this, QString::fromUtf8("删除人脸图像"), QString::fromUtf8("是否删除？"), questionMbx);

    questionNote.exec();
    if(QDialog::Accepted != questionNote.result())
    {
        return;
    }

    // _isDataChanged = true;

    //判断人脸图像的来源
    if(!newImageUnsaved)
    {
        deletFacePhoto(perID.c_str());
    }
    else
    {
        //更新人脸图像存储操作标记
        newImageUnsaved = false;
    }

    //更新人脸图像
    emit sigLoadFaceImage();

    //IkSocket::GetIkSocketInstance()->UploadTemp();

    if(IkSocket::GetIkSocketInstance()->isSocketConnet())
    {
        IkSocket::GetIkSocketInstance()->UploadTemp();
    }
    else
    {
        LOG_INFO("网络未连接到服务器");
    }

    return;
}


/*****************************************************************************
 *                        管理部门
 *  函 数 名：slotManageDepart
 *  功    能：槽函数，管理部门操作处理函数
 *  说    明：
 *  参    数：
 *  返 回 值：
 *  创 建 人：L.Wang
 *  创建时间：2013-10-18
 *  修 改 人：
 *  修改时间：
 *****************************************************************************/
void DetailPersonInfo::slotManageDepart()
{
    _isDataChanged = true;

    //TO DO
    QString curDept = ui->cmbDepart->currentText();

    DepartManage dpm(this);
    dpm.exec();
    show();

    //设置当前人员的部门名称
    ui->cmbDepart->setCurrentIndex(ui->cmbDepart->findText(curDept));
}

/*****************************************************************************
 *                        获取人脸照片
 *  函 数 名：slotGetPhoto
 *  功    能：获取人脸照片操作处理函数
 *  说    明：
 *  参    数：
 *  返 回 值：
 *  创 建 人：L.Wang
 *  创建时间：2013-10-18
 *  修 改 人：
 *  修改时间：
 *****************************************************************************/
void DetailPersonInfo::slotGetPhoto(IN QImage img)
{

    if(!img.isNull())
    {
        ui->labFacePhoto->setPixmap(QPixmap::fromImage(img));
        newImageUnsaved = true;
    }
    else
    {
        emit sigLoadFaceImage();   //加载本地的已有人脸图像
    }

}



/*****************************************************************************
 *                        获取人脸照片
 *  函 数 名：slotGetPhoto
 *  功    能：获取人脸照片操作处理函数
 *  说    明：
 *  参    数：
 *  返 回 值：
 *  创 建 人：L.Wang
 *  创建时间：2013-10-18
 *  修 改 人：
 *  修改时间：
 *****************************************************************************/
void DetailPersonInfo::slotLoadFaceImage()
{

    DatabaseAccess dbAc;
    std::vector<PersonImage> perImgs;

    if(dbSuccess == dbAc.Query(QString("SELECT * FROM personimage WHERE personid = '%1'").arg(perID.c_str()).toStdString(), perImgs))
    {
        if(perImgs.size() > 0)
        {
            QImage localeImage(IMAGE_WIDTH, IMAGE_HEIGHT, QImage::Format_RGB888);
            PersonImage& perImg = perImgs[0];
            unsigned char imgBuf[1024 * 1024 * 2];
            size_t imgSize = perImg.personImage.Size();

            perImg.personImage.Get(imgBuf, imgSize);
            localeImage.loadFromData(imgBuf, perImg.personImage.Size(), FACE_IMAGE_FORMAT.toStdString().c_str());

            ui->labFacePhoto->setPixmap(QPixmap::fromImage(localeImage));
            //ui->labFacePhoto->pixmap()->save("faceimage.jpg", "jpg");

            return;
        }
    }

    ui->labFacePhoto->setText(QString::fromUtf8("请添加照片"));
}



/*****************************************************************************
 *                        注释文本长度控制
 *  函 数 名：slotMemoTextChanged
 *  功    能：获取人脸照片操作处理函数
 *  说    明：
 *  参    数：
 *  返 回 值：
 *  创 建 人：L.Wang
 *  创建时间：2013-10-18
 *  修 改 人：
 *  修改时间：
 *****************************************************************************/
void DetailPersonInfo::slotMemoTextChanged()
{
    _isDataChanged = true;

    QString textContent = ui->txtNote->toPlainText();
    int length = textContent.count();
    int maxLength = 128; // 最大字符数
    if(length > maxLength)
    {
        int position = ui->txtNote->textCursor().position();
        QTextCursor textCursor = ui->txtNote->textCursor();
        textContent.remove(position - (length - maxLength), length - maxLength);
        ui->txtNote->setText(textContent);
        textCursor.setPosition(position - (length - maxLength));
        ui->txtNote->setTextCursor(textCursor);
    }
}

/*****************************************************************************
 *                        删除IC卡卡号
 *  函 数 名：slotDeleteICCard
 *  功    能：删除当前人员的IC卡卡号
 *  说    明：
 *  参    数：
 *  返 回 值：
 *  创 建 人：L.Wang
 *  创建时间：2013-10-18
 *  修 改 人：
 *  修改时间：
 *****************************************************************************/
void DetailPersonInfo::slotDeleteICCard()
{
    _isDataChanged = true;

    ui->labICCardNum->clear();
    ICNum = "";
    ui->btnDeleteICCard->setEnabled(false);
}


/*****************************************************************************
 *                        获取人脸照片
 *  函 数 名：slotGetPhoto
 *  功    能：获取人脸照片操作处理函数
 *  说    明：
 *  参    数：
 *  返 回 值：
 *  创 建 人：L.Wang
 *  创建时间：2013-10-18
 *  修 改 人：
 *  修改时间：
 *****************************************************************************/
void DetailPersonInfo::show()
{

    QDialog::show();
    initDeptLst();
}


bool DetailPersonInfo::checkNoneEmptyItems()
{

    QStringList notes;
    //Check ID Card number format
    if(ui->txtWorkSN->text().trimmed().isEmpty())
    {
        notes << QString::fromUtf8("工号");
    }

    //Check ID Card number format
    if(ui->txtName->text().trimmed().isEmpty())
    {
        notes << QString::fromUtf8("姓名");
    }

    if(notes.size())
    {
        IKMessageBox infoNote(this, QString::fromUtf8("保存人员信息"), QString::fromUtf8("%1不能为空！").arg(notes.join("、")), informationMbx);

        infoNote.exec();
        return false;
    }

    return true;
}
bool DetailPersonInfo::checkInPutLength()
{

    if(getIputLength(ui->txtName->text()) > 64)
    {
        IKMessageBox infoNote(this, QString::fromUtf8("保存人员信息"), QString::fromUtf8("姓名输入过长！"), informationMbx);
        infoNote.exec();
        return false;
    }
    if(getIputLength(ui->txtNote->text()) > 128)
    {
        IKMessageBox infoNote(this, QString::fromUtf8("保存人员信息"), QString::fromUtf8("备注信息输入过长！"), informationMbx);
        infoNote.exec();
        return false;
    }
    if(getIputLength(ui->cmbDepart->currentText()) > 64)
    {
        IKMessageBox infoNote(this, QString::fromUtf8("保存人员信息"), QString::fromUtf8("部门信息输入过长！"), informationMbx);
        infoNote.exec();
        return false;
    }

    return true;
}

int DetailPersonInfo::getIputLength(QString str)
{

    int count = str.count();
    int length = 0;
    qDebug()<<"count"<<count<<endl;
    for(int i = 0;i<count;i++)
    {
        QString str2 = str.mid(i,1);

        if(str2.local8Bit().length()>1)
        {
            length += 4;
        }
        else
        {
            length +=1;
        }
    }
    qDebug()<<"inputlength"<<length<<endl;
    return length;

}

/*****************************************************************************
 *                        根据部门名称查询部门ID
 *  函 数 名：queryDepartIDByName
 *  功    能：
 *  说    明：
 *  参    数：
 *           deptID， 部门ID，返回值；
 *           deptName, 部门名称，输入参数
 *  返 回 值：
 *  创 建 人：L.Wang
 *  创建时间：2013-10-18
 *  修 改 人：
 *  修改时间：
 *****************************************************************************/
bool DetailPersonInfo::queryDepartIDByName(OUT int &deptID, IN const QString &deptName)
{

    //TO DO
    DatabaseAccess ac;
    //QString query("SELECT departid FROM depart WHERE departname = '%1'");
    QString query("SELECT departid FROM depart WHERE departname = '%1'");
    query.arg(deptName);

    //获取当前部门ID
    if(dbSuccess == ac.Query(query.toStdString(), deptID))
    {
        return true;
    }
    else
    {
        LOG_ERROR("Query sql failed. %s",query.toStdString().c_str());
        IKMessageBox infoNote(this, QString::fromUtf8("添加人员信息"), QString::fromUtf8("该部门不存在，请重新选择部门！"), informationMbx);

        infoNote.exec();
        return false;
    }
}


/*****************************************************************************
 *                        添加人员信息
 *  函 数 名：addPersonInfo
 *  功    能：
 *  说    明：
 *  参    数：
 *  返 回 值：
 *  创 建 人：L.Wang
 *  创建时间：2013-10-18
 *  修 改 人：
 *  修改时间：
 *****************************************************************************/
bool DetailPersonInfo::addPersonInfo(const Person &pInfo)
{

    //Check worksn conflict
    if(isUsed(ui->txtWorkSN->text(), WKSN))
    {
        IKMessageBox infoNote(this, QString::fromUtf8("保存人员信息"), QString::fromUtf8("工号冲突，请重新分配！"), informationMbx);


        infoNote.exec();
        return false;
    }

    //Check ID number conflict
    if((!ui->txtIDNum->text().isEmpty()) && (isUsed(ui->txtIDNum->text(), IDNUM)))
    {
        IKMessageBox infoNote(this, QString::fromUtf8("保存人员信息"), QString::fromUtf8("身份证号已占用，请重和对后重试！"), informationMbx);

        infoNote.exec();
        return false;
    }

    //Check IC card conflict
    if((!ui->txtIDNum->text().isEmpty()) && isUsed(ui->labICCardNum->text(), ICNUM))
    {
        IKMessageBox infoNote(this, QString::fromUtf8("保存人员信息"), QString::fromUtf8("IC卡号冲突，请重新读取！"), informationMbx);

        infoNote.exec();
        return false;
    }

    QString sql("INSERT INTO person (personid, name, sex, idcard, superid, worksn, departid, departname, hasirisdata, cardid, memo, disabletime) "
                "VALUES(:personid, :name, :sex, :idcard, :superid, :worksn, :departid, :departname, :hasirisdata, :cardid, :memo, :disabletime);");

    //保存操作事项到对应的人员临时表中，以便设备向服务器同步设备上的人员信息更新
    QString sqlToTmpTable("INSERT INTO tempperson (personid, operation) VALUES(:personid, 0);");
    TempPerson tper(pInfo.personID, 0);


    DatabaseAccess dbAc;
    Transaction trn(*(dbAc.GetSession()));

    std::vector<Person> pers; // insert to DB
    pers.push_back(pInfo);

    std::vector<TempPerson> tPers; //insert to DB
    tPers.push_back(tper);

    bool rtn = dbSuccess == dbAc.InsertWithoutTrans(sql.toStdString(), pers) &&
            dbSuccess == dbAc.InsertWithoutTrans(sqlToTmpTable.toStdString(), tPers) && saveFaceImage(dbAc);

    if(rtn)
    {
        trn.Commit();

        // 生成卡号
        //        if(WorkMode::IsSingleWorkMode())
        {
            int nret = CheckCardIDAndGen(pInfo.personID);
            if(nret != 0)
            {
                LOG_ERROR("CheckCardIDAndGen(pInfo.personID) failed");
            }
        }

        return true;
    }
    else
    {
        trn.Rollback();
        std::cout << "[" << __FUNCTION__  << "@DetailPersonInfo]"
                  << "faile to add person information(pid:" << pInfo.personID.Unparse() << ") to database!"
                  << std::endl;
        LOG_ERROR("faile to add person information");
        return false;
    }
}


/*****************************************************************************
 *                        更新人员信息
 *  函 数 名：updatePersonInfo
 *  功    能：更新人员信息
 *  说    明：
 *  参    数：
 *  返 回 值：
 *  创 建 人：L.Wang
 *  创建时间：2013-10-18
 *  修 改 人：
 *  修改时间：
 *****************************************************************************/
bool DetailPersonInfo::updatePersonInfo(const Person &pInfo)
{   

    // modified at 20170105 by yqhe
    // 不修改是否虹膜已注册的标记

    //	QString sql(" UPDATE person SET  "
    //			"    name = :name,  "
    //			"    sex = :sex,  "
    //			"    idcard = :idcard,  "
    //			"    superid = :superid, "
    //			"    worksn = :worksn, "
    //			"    departid = :departid, "
    //			"    departname = :departname, "
    //			"    hasirisdata = :hasirisdata,  "
    //			"    cardid = :cardid, "
    //			"    memo = :memo "
    //			" WHERE personid = :personid;");

    QString sql(" UPDATE person SET  "
                "    name = :name,  "
                "    sex = :sex,  "
                "    idcard = :idcard,  "
                "    superid = :superid, "
                "    worksn = :worksn, "
                "    departid = :departid, "
                "    departname = :departname, "
                "    cardid = :cardid, "
                "    memo = :memo "
                " WHERE personid = :personid;");

    //保存操作事项到对应的人员临时表中，以便设备向服务器同步设备上的人员信息更新
    QString sqlToTmpTable("INSERT INTO tempperson (personid, operation) VALUES(:personid, 2);");
    TempPerson tper(pInfo.personID, 2);

    std::vector<Person> pers;  //update db 1 条记录
    pers.push_back(pInfo);

    std::vector<TempPerson> tPers; // insert 1条记录
    tPers.push_back(tper);


    //更新数据库
    DatabaseAccess dbAc;
    Transaction trn(*(dbAc.GetSession()));
    bool rtn = (dbSuccess == dbAc.Update(sql.toStdString(), pers)
                && dbSuccess == dbAc.InsertWithoutTrans(sqlToTmpTable.toStdString(), tPers)
                && saveFaceImage(dbAc));


    //////////////////////////////////////////////////////////////////////////
    InfoData    infodata;
    infodata.PeopleData.Name = pInfo.name;
    infodata.PeopleData.CardID = pInfo.cardID;
    infodata.PeopleData.WorkSn = pInfo.workSn;
    infodata.PeopleData.Depart = pInfo.departName;

    uuid_t personid;
    pInfo.personID.Get(personid);
    uuid_copy(infodata.PeopleData.Id,personid);

    int nmemret = SingleControl<IrisManager>::CreateInstance().UpdatePersonInfo(infodata);
    //////////////////////////////////////////////////////////////////////////


    if(rtn)
    {
        trn.Commit();
        return true;
    }
    else
    {
        trn.Rollback();
        std::cout << "[" << __FUNCTION__  << "@DetailPersonInfo]"
                  << "faile to update person(pid:" << pInfo.personID.Unparse() << ") information to database!"
                  << std::endl;
        LOG_ERROR("faile to update person");
        return false;
    }
}




/*****************************************************************************
 *                        校验身份证号
 *  函 数 名：checkIDCard
 *  功    能：确认身份证号是否格式正确
 *  说    明：
 *  参    数：
 *  返 回 值：
 *  创 建 人：L.Wang
 *  创建时间：2013-10-18
 *  修 改 人：
 *  修改时间：
 *****************************************************************************/
bool DetailPersonInfo::checkIDCard()
{

    //TO DO
    const QRegExp idCardFormatFst("^\\d{15,15}$");
    const QRegExp idCardFormatSec("^\\d{17,17}[Xx\\d]$");


    if(ui->txtIDNum->text().isEmpty()
            || idCardFormatFst.exactMatch(ui->txtIDNum->text().trimmed())
            || idCardFormatSec.exactMatch(ui->txtIDNum->text().trimmed()))
    {
        //身份证号为空或格式正确
        LOG_ERROR("身份证号为空或格式正确");
        return true;
    }
    else
    {
        IKMessageBox infoNote(this, QString::fromUtf8("保存人员信息"), QString::fromUtf8("身份证号格式错误！"), informationMbx);

        infoNote.exec();
        return false;
    }
}

bool DetailPersonInfo::checkKeyItemsValid()
{

    //TO DO
    QStringList notes;
    if(isUpdateOpt)
    {
        //查询人员工号是否已存在
        if(workSN != ui->txtWorkSN->text().toStdString() && isUsed(ui->txtWorkSN->text(), WKSN))
        {
            notes << QString::fromUtf8("工号");
        }

        //查询人员身份证号是否已存在
        if(IDNum != ui->txtIDNum->text().toStdString() && isUsed(ui->txtIDNum->text(), IDNUM))
        {
            if(!ui->txtIDNum->text().isEmpty())
            notes << QString::fromUtf8("身份证号");
        }

        //查询射频卡卡号是否已存在
        if(ICNum != ui->labICCardNum->text().toStdString() && isUsed(ui->labICCardNum->text(), ICNUM))
        {
            notes << QString::fromUtf8("射频卡号");
        }
    }
    else
    {
        //查询人员工号是否已存在
        if(! ui->txtWorkSN->text().isEmpty() && isUsed(ui->txtWorkSN->text(), WKSN))
        {
            notes << QString::fromUtf8("工号");
        }

        //查询人员身份证号是否已存在
        if(! ui->txtIDNum->text().isEmpty() && isUsed(ui->txtIDNum->text(), IDNUM))
        {
            notes << QString::fromUtf8("身份证号");
        }

        //查询射频卡卡号是否已存在
        if(! ui->labICCardNum->text().isEmpty() && isUsed(ui->labICCardNum->text(), ICNUM))
        {
            notes << QString::fromUtf8("射频卡号");
        }
    }

    if(notes.size())
    {
        IKMessageBox infoNote(this, QString::fromUtf8("保存人员信息"), QString::fromUtf8("%1已占用，请核对后重试！").arg(notes.join("、")), informationMbx);

        infoNote.exec();
        return false;
    }

    return true;
}


bool DetailPersonInfo::isUsed(const QString& pInfo, QueryType qType)
{

    //TO DO
    QString queryType;
    switch(qType)
    {
    case WKSN:
        queryType = "worksn";
        break;
    case IDNUM:
        queryType = "idcard";
        break;
    case ICNUM:
        queryType = "cardid";
        break;
    default:
        printf("%scheck duplicate record: unknown item type to check!queryItemType: %d\n",
               GetLogHead(__FUNCTION__).c_str(), qType);
        return false;
    }

    QString queryStr("SELECT count(*) FROM person WHERE %1 = '%2'");

    queryStr = queryStr.arg(queryType, pInfo.trimmed());
    printf("%scheck duplicate record: (SQL)%s\n",
           GetLogHead(__FUNCTION__).c_str(), queryStr.toStdString().c_str());

    qDebug()<<queryStr<<endl;
    int count;
    DatabaseAccess dbAc;
    if(dbSuccess == dbAc.Query(queryStr.toStdString(), count))
    {
        if(count > 0)
        {
            return true;
        }
    }
    else
    {
        std::cout << "[" << __FUNCTION__  << "@DetailPersonInfo]" << "faile to query worksn" << std::endl;
        LOG_ERROR("faile to query worksn");
    }

    return false;
}


/*****************************************************************************
 *                        存储人脸照片
 *  函 数 名：saveFaceImage
 *  功    能：
 *  说    明：将未保存的人脸图像信息存入数据库
 *  参    数：
 *  返 回 值：成功返回true, 其他返回false
 *  创 建 人：L.Wang
 *  创建时间：2014-04-24
 *  修 改 人：
 *  修改时间：
 *****************************************************************************/
bool DetailPersonInfo::saveFaceImage(DatabaseAccess& da)
{

    if(!newImageUnsaved || ui->labFacePhoto->pixmap()->isNull())
    {
        return true;
    }

    //Generate face photo image id
    UuidControl uid;
    PersonImage perImg;
    QString sql;
    QString sqlToTempTable;
    TempPersonImage tImg;


    bool isUpdateFacePhoto = hasFacePhoto(perID.c_str());
    //如果文件已存在，是否需要覆盖
    if(isUpdateFacePhoto)
    {
        IKMessageBox questionNote(this, QString::fromUtf8("保存人员头像"), QString::fromUtf8("是否替换已有头像照片？"), questionMbx);

        questionNote.exec();
        if(QDialog::Accepted != questionNote.result())
        {
            return true;
        }

        //Delete current facephoto failed then return false,otherwise add a new face photo for the person
        if(!deletFacePhoto(perID.c_str()))
        {
            LOG_ERROR("Delete current facephoto failed");
            return false;
        }
    }


    //添加人员图像信息
    sql = "INSERT INTO personimage (imageid, personid, personimage, memo) VALUES(:imageid, :personid, :personimage, :memo)";
    sqlToTempTable = "INSERT INTO temppersonimage (personid, imageid, operation) VALUES(:personid, :imageid, 0)";

    perImg.imageID = uid.GetUUIDString();
    tImg.imageID = perImg.imageID;


    //人脸图像对应的人员编号
    perImg.personID = perID;

    //以指定格式将人脸图像转存到内存
    QByteArray faceImg;
    QBuffer imgBuf(&faceImg);

    //获取当前图像数据
    imgBuf.open(QIODevice::WriteOnly);
    ui->labFacePhoto->pixmap()->save(&imgBuf, FACE_IMAGE_FORMAT.toStdString().c_str());
    imgBuf.close();

    //填充图像数据
    perImg.personImage.Set(faceImg.data(), faceImg.size());


    tImg.imageID = perImg.imageID;
    tImg.personID = perImg.personID;

    std::vector<PersonImage> perImgs;
    perImgs.push_back(perImg);

    std::vector<TempPersonImage> tPerImgs;
    tPerImgs.push_back(tImg);

    Transaction trn(*(da.GetSession()));
    //确认是否已注册人脸图像
    bool rtnVal = (dbSuccess == da.InsertWithoutTrans(sql.toStdString(), perImgs)
                   && dbSuccess == da.InsertWithoutTrans(sqlToTempTable.toStdString(), tPerImgs));

    if(rtnVal)
    {
        //更新图像存储操作标记
        trn.Commit();
        newImageUnsaved = false;
        std::cout << "[" << __FUNCTION__ << "@DetailPersonInfo]"
                  << "we have save face photo(pid:" << perID << ", imgid: " << perImg.imageID.Unparse() << ") to database table personimgae!" << std::endl;
        LOG_INFO("we have save face photo");
    }
    else
    {
        trn.Rollback();
        std::cout << "[" << __FUNCTION__ << "@DetailPersonInfo]"
                  << "failed to save face photo(pid:" << perID << ", imgid: " << perImg.imageID.Unparse() << ") to database table personimgae!!" << std::endl;
        LOG_ERROR("failed to save face photo");
    }

    return rtnVal;
}


/*****************************************************************************
 *                        是否已有人脸照片
 *  函 数 名：hasFacePhoto
 *  功    能：判断该人员是否已经注册过人脸图像
 *  说    明：
 *  参    数：pid, 指定人员的人员ID
 *  返 回 值：成功返回true，其他返回false
 *  创 建 人：L.Wang
 *  创建时间：2014-04-24
 *  修 改 人：
 *  修改时间：
 *****************************************************************************/
bool DetailPersonInfo::hasFacePhoto(const QString& pid)
{

    //TO DO
    DatabaseAccess dbAc;
    QString sql = QString("SELECT count(*) FROM personimage WHERE personid = '%1'").arg(pid);
    int counter = -1;
    if(dbSuccess == dbAc.Query(sql.toStdString(), counter))
    {
        if(counter > 0)
        {
            return true;
        }
        else
        {
            LOG_ERROR("Query sql failed. %s",sql.toStdString().c_str());
            return false;
        }
    }

    std::cout << "[" << __FUNCTION__ << "@DetailPersonInfo]" << "database query face image(pid:" << perID << ") operation failed!" << std::endl;
    LOG_INFO("database query face image");
    return false;
}


/*****************************************************************************
 *                        获取人脸照片ID
 *  函 数 名：getFacePhotoID
 *  功    能：获取人脸图像
 *  说    明：
 *  参    数：pid, 指定人员的人员ID
 *  返 回 值：返回图像ID，如果成功imageid不为空则有效，其他无效
 *  创 建 人：L.Wang
 *  创建时间：2014-04-24
 *  修 改 人：
 *  修改时间：
 *****************************************************************************/
std::string DetailPersonInfo::getFacePhotoID(const std::string& pid)
{

    DatabaseAccess dbAc;
    std::string imgID;
    if(dbSuccess != dbAc.Query(QString("SELECT imageid FROM personimage WHERE personid = '%1'").arg(pid.c_str()).toStdString(), imgID))
    {
        std::cout << "[" << __FUNCTION__ << "@DetailPersonInfo]"
                  << "database query face image id(pid:" << pid << ") operation failed!"
                  << std::endl;
        LOG_INFO("database query face image id");
    }

    return imgID;
}



/*****************************************************************************
 *                        获取人脸照片名称
 *  函 数 名：getFacePhotoName
 *  功    能：获取人脸照片名称
 *  说    明：
 *  参    数：workSN,人员工号
 *  返 回 值：
 *  创 建 人：L.Wang
 *  创建时间：2013-10-18
 *  修 改 人：
 *  修改时间：
 *****************************************************************************/
QString getFacePhotoName(const QString& pid)
{

    QString facePhotoPath = getFacePhotoPath();
    if(facePhotoPath.isEmpty())
    {
        return "";
    }
    else
    {
        return QString::fromUtf8("%1%2%3.%4").arg(facePhotoPath).arg(QDir::separator()).arg(pid.trimmed()).arg(FACE_IMAGE_FORMAT);
    }

}

/*****************************************************************************
 *                        获取人脸照片路径
 *  函 数 名：getFacePhotoPath
 *  功    能：获取人脸照片存储路径
 *  说    明：
 *  参    数：
 *  返 回 值：
 *  创 建 人：L.Wang
 *  创建时间：2013-10-18
 *  修 改 人：
 *  修改时间：
 *****************************************************************************/
QString getFacePhotoPath()
{

    QString facePhotoDir("facephoto");
    QDir dir(QCoreApplication::applicationDirPath());
    std::cout << "application dir: " << dir.absolutePath().toStdString() << std::endl;
    if(!dir.exists(facePhotoDir))
    {
        if(!dir.mkdir(facePhotoDir))
        {
            std::cout << __FUNCTION__ << ":can not create directory for saving face photo" << std::endl;
            LOG_ERROR("can not create directory for saving face photo");
            return "";
        }

        system("sudo chmod 777 *");
    }

    dir.cd(facePhotoDir);
    return dir.absolutePath();
}

///*****************************************************************************
//*                        删除人脸照片
//*  函 数 名：deletFacePhoto
//*  功    能：如果人脸照片存在则删除掉该人员的人脸照片
//*  说    明：
//*  参    数：workSN,人员工号
//*  返 回 值：成功返回true，其他返回false
//*  创 建 人：L.Wang
//*  创建时间：2013-10-18
//*  修 改 人：
//*  修改时间：
//*****************************************************************************/
//bool deletFacePhoto(const QString& pid)
//{
//    QString file(getFacePhotoName(pid));
//    std::cout << "facephoto path @" << __FUNCTION__ << ":is " << file.toStdString() << std::endl;
//    if(QFile::exists(file))
//    {
//        return QFile::remove(file);
//    }

//    return true;
//}


/*****************************************************************************
 *                        删除人脸照片
 *  函 数 名：deletFacePhoto
 *  功    能：如果人脸照片存在则删除掉该人员的人脸照片
 *  说    明：
 *  参    数：id, 人员编号
 *  返 回 值：成功返回true，其他返回false
 *  创 建 人：L.Wang
 *  创建时间：2013-10-18
 *  修 改 人：
 *  修改时间：
 *****************************************************************************/
bool DetailPersonInfo::deletFacePhoto(const QString& id)
{


    //TO DO
    DatabaseAccess dbAc;
    //Transaction trn(*(dbAc.GetSession()));

    QString sql = QString("DELETE FROM personimage WHERE personid = '%1';"
                          "INSERT INTO temppersonimage (personid, imageid, operation)"
                          "VALUES('%1', '%2', 1)").arg(id, getFacePhotoID(id.toStdString()).c_str());


    //操作码(operation)为1表示删除记录
    if(dbSuccess == dbAc.Query(sql.toStdString()))
    {
        //trn.Commit();
        return true;
    }
    else
    {
        //trn.Rollback();
        std::cout << "[" << __FUNCTION__ << "@DetailPersonInfo]" << "database query face image id(pid:" << id.toStdString() << ") operation failed!" << std::endl;
        LOG_ERROR("database query face image id");
        return false;
    }
}



void DetailPersonInfo::closeThis()
{

    emit sigGoBackFromDP(false);
}

/*****************************************************************************
 *                        check workSn
 *  函 数 名：check workSn
 *  功    能：check workSn
 *  说    明：
 *  参    数：
 *  返 回 值： 0:正常通過，1：工号前3位验证不通过；2：工号后5位验证不通过
 *  创 建 人：guodj
 *  创建时间：2016-5-19
 *  修 改 人：
 *  修改时间：
 *****************************************************************************/
int DetailPersonInfo::checkPersonWorkSn(QString workSn)
{

    QString head = workSn.left(3);
    QString tail = workSn.right(5);

    uint32_t digit_head = head.toUInt(0,10);
    uint32_t digit_tail = tail.toUInt(0,10);

    if(digit_head   <   0 ||  digit_head    >   255)
    {
        return 1;
    }

    if(digit_tail   <   0 ||  digit_tail    >0xffff)
    {
        return 2;
    }

    return 0;

}

bool DetailPersonInfo::IsDataEdited()
{

    return _isDataChanged;

}


void DetailPersonInfo::on_txtNote_textChanged()
{
    _isDataChanged = true;
    QString textContent = ui->txtNote->toPlainText();
    int inputLength = getIputLength(textContent);
    int srcNum = textContent.count();

    int maxLength = 128; // 最大字符数
    if(inputLength > maxLength )
    {
        int position = ui->txtNote->textCursor().position();
        QTextCursor textCursor = ui->txtNote->textCursor();
        textContent.remove(position - 1, 1);
        ui->txtNote->setText(textContent);
        textCursor.setPosition(position - 1);
        ui->txtNote->setTextCursor(textCursor);
        QPoint pos(200,450);
        QToolTip::showText(pos,"输入长度超过限制");
    }

}


void DetailPersonInfo::on_txtName_textChanged(const QString &arg1)
{
    _isDataChanged = true;


    QString textContent = arg1;
    //    if(textContent.indexOf("%") >=0)
    //    {
    //        QToolTip::showText(QCursor::pos(),"输入不能包含下列字符:%");
    //    }

    int inputLength = getIputLength(textContent);

    int maxLength = 64; // 最大字符数
    qDebug()<<inputLength<<endl;
    if(inputLength > maxLength || textContent.indexOf("%") >= 0)
    {
        int position = ui->txtName->cursorPosition();
        textContent.remove(position - 1, 1);
        ui->txtName->setText(textContent);
        ui->txtName->setCursorPosition(position -1);
        QPoint pos(200,90);


        if(inputLength > maxLength)
        {
           QToolTip::showText(pos,"输入长度超过限制");
        }
        else
        {
            QToolTip::showText(pos,"输入不能包含下列字符:%");
        }

    }
}

void DetailPersonInfo::on_cmbDepart_textChanged(const QString &arg1)
{
    _isDataChanged = true;

    QString textContent = arg1;
    int inputLength = getIputLength(textContent);

    int maxLength = 64; // 最大字符数
    if(inputLength > maxLength || (textContent.indexOf("%") >= 0))
    {
        int count = arg1.count();
        QString str = textContent.left(count -1);
        ui->cmbDepart->setCurrentText(str);
        QPoint pos(200,320);
        if(inputLength > maxLength)
        {
            QToolTip::showText(pos,"输入超过长度限制");
        }
        else
        {
            QToolTip::showText(pos,"输入不能包含下列字符:%");

        }


    }

}


void DetailPersonInfo::on_txtIDNum_textEdited(const QString &arg1)
{
    _isDataChanged = true;
    QString strValue = arg1;
    QRegExp inputExpr("([0-9]{15,15})|([0-9]{17}[0-9xX])");
    QRegExpValidator validator(inputExpr);
    int index = strValue.count();
    if(QValidator::Invalid == validator.validate(strValue,index))
    {
        int count = strValue.count();
        QString str = strValue.left(count -1);
        ui->txtIDNum->setText(str);
        QPoint pos(200,160);
        if(count > 18)
        {
           QToolTip::showText(pos,"输入长度不能超过18");
        }
        else
        {
           QToolTip::showText(pos,"只能输入数字，末尾只允许输入数字或字母X");
        }

    }

}

void DetailPersonInfo::on_txtWorkSN_textEdited(const QString &arg1)
{
    _isDataChanged = true;
    QString strValue = arg1;
    QRegExp inputExpr("[a-zA-Z0-9]{0,32}");
    QRegExpValidator validator(inputExpr);
    int index = strValue.count();
    if(QValidator::Invalid == validator.validate(strValue,index))
    {
        int count = strValue.count();
        QString str = strValue.left(count -1);
        ui->txtWorkSN->setText(str);
        QPoint pos(200,260);
        if(count > 32)
        {
           QToolTip::showText(pos,"输入长度不能超过32");
        }
        else
        {
           QToolTip::showText(pos,"只允许输入数字或英文字母");
        }

    }

}

void DetailPersonInfo::on_rbtnMale_clicked(bool checked)
{
    _isMale[0] = checked;


}

void DetailPersonInfo::on_rbtnFamale_clicked(bool checked)
{
    _isMale[0] = !checked;

}

void DetailPersonInfo::on_rbtnUser_clicked(bool checked)
{
    _isAdmin[0] = !checked;

}

void DetailPersonInfo::on_rbtnAdmin_clicked(bool checked)
{
    _isAdmin[0] = checked;
}
