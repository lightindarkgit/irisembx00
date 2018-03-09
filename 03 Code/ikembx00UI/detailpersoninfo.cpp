/*****************************************************************************
** 文 件 名：detailpersoninfo.cpp
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
#include <QMessageBox>
#include <QSqlQuery>
#include <QSqlError>
#include <QImage>
#include <QPixmap>
#include <QFile>
#include <QDir>


#include "detailpersoninfo.h"
#include "ui_detailpersoninfo.h"
#include "departmanage.h"
#include "../common.h"

#include "serialevent.h"
#include "../bin/asyncserial.h"
#include "../bin/basictypes.h"


//串口封装事件
SerialEvent se;

void RecvCardNum(const unsigned char* data, const int size)
{
    //TO DO
    QString val, s;
    if(NULL != data)
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
    sm(SerialMuxDemux::GetInstance()),
    newImageUnsaved(false)
{
    initSerialMuxDemux();
    initUI();          //初始化UI
    initSigSlots();    //初始化信号与槽
}

DetailPersonInfo::DetailPersonInfo(const PersonInfo& personInfo, QWidget *parent):
    QDialog(parent),
    ui(new Ui::DetailPersonInfo),
    sm(SerialMuxDemux::GetInstance()),
    newImageUnsaved(false)
{
    //TO DO
    initSerialMuxDemux();
    initUI(true);          //初始化UI
    initSigSlots();    //初始化信号与槽
    fillData(personInfo);

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
*  创 建 人：王磊
*  创建时间：2013-10-18
*  修 改 人：
*  修改时间：
*****************************************************************************/
void DetailPersonInfo::initSerialMuxDemux()
{
    unsigned char cmd = 0x0f;
    if(true != sm->AddCallback(cmd, RecvCardNum))
    {
        QMessageBox::warning(this, QString::fromUtf8("注册回调函数"), QString::fromUtf8("注册失败"));
    }

    if(E_SERIAL_OK != sm->Init("ttys1"))
    {
        QMessageBox::warning(this, QString::fromUtf8("初始化串口"), QString::fromUtf8("初始化串口失败"));
    }
//    else
//    {
//        QMessageBox::warning(this, QString::fromUtf8("初始化串口"), QString::fromUtf8("初始化串口成功"));
//    }
}

/*****************************************************************************
*                        初始化人员信息页面
*  函 数 名：init
*  功    能：初始化人员详细信息页面
*  说    明：
*  参    数：
*  返 回 值：
*  创 建 人：王磊
*  创建时间：2013-10-18
*  修 改 人：
*  修改时间：
*****************************************************************************/
void DetailPersonInfo::initDeptLst()
{
    //TO DO
    QStringList slstDepart;
    QSqlQuery query;        //数据库连接已经在人员管理界面中打开，此处可以使用默认连接来共享已打开的数据库
    query.exec(QString("SELECT name from depart"));

    while(query.next())
    {
        slstDepart << (QString)query.value(0).toCString();
    }

    ui->cmbDepart->clear();
    ui->cmbDepart->addItems(slstDepart);
}


/*****************************************************************************
*                        初始化人员信息页面UI
*  函 数 名：init
*  功    能：初始化人员详细信息页面UI
*  说    明：
*  参    数：
*  返 回 值：
*  创 建 人：王磊
*  创建时间：2013-12-17
*  修 改 人：
*  修改时间：
*****************************************************************************/
void DetailPersonInfo::initUI(bool modifyOpt)
{
    //TO DO
    ui->setupUi(this);

    if(modifyOpt)
    {
        isUpdateOpt = true;
        ui->txtWorkSN->setEnabled(false);
    }
    else
    {
        isUpdateOpt = false;
    }

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


    //隐藏对话框标题栏及对话框位置调整和背景填充
    setWindowFlags(Qt::FramelessWindowHint);
    setGeometry(QRect(0, 0, 800, 600));
    setBackgroundColor(QColor(242,241,240));

    setAttribute(Qt::WA_TranslucentBackground, false);

    initDeptLst();           //初始化部门列表
    ui->txtName->setFocus();          //设置人员姓名为输入焦点的默认位置
}


/*****************************************************************************
*                        初始化人员信息页面信号与槽
*  函 数 名：init
*  功    能：初始化人员详细信息页面信号与槽
*  说    明：
*  参    数：
*  返 回 值：
*  创 建 人：王磊
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
    connect(this, SIGNAL(sigLoadFaceImage()), this, SLOT(slotLoadFaceImage()));
}


void DetailPersonInfo::fillData(const PersonInfo& personInfo)
{
    //TO DO
    if(NULL == ui)
    {
        return;
    }

    QSqlQuery query;
    query.prepare("SELECT t1.workid, t1.name, t1.gender, t1.idnum, t2.name, t1.iris, t1.card, t1.note\n"
                  "FROM person AS t1,"
                  "    depart AS t2\n"
                  "WHERE t1.departid = t2.id AND\n"
                  "    t1.workid = :workid");
    query.bindValue(":workid", personInfo.id);
    query.exec();

    if(query.next())
    {
        ui->txtWorkSN->setText(query.value(0).toString());         //设置工号
        ui->txtName->setText(query.value(1).toString());      //设置人员姓名

        if(0 == query.value(2).toString().compare(QString::fromUtf8("男")))                    //设置性别
        {
            ui->rbtnMale->setChecked(true);
            ui->rbtnFamale->setChecked(false);
        }
        else
        {
            ui->rbtnMale->setChecked(false);
            ui->rbtnFamale->setChecked(true);
        }

        ui->txtIDNum->setText(query.value(3).toString());          //设置身份证号

//        if(PERSON_USER == query.value(3).personType)          //设置用户类型
//        {
//            ui->rbtnUser->setChecked(true);
//            ui->rbtnAdmin->setChecked(false);
//        }
//        else
//        {
//            ui->rbtnUser->setChecked(false);
//            ui->rbtnAdmin->setChecked(true);
//        }

        ui->cmbDepart->setCurrentText(query.value(4).toString());          //设置部门名称

        ui->labIrisStatusValue->setText(query.value(5).toString());        //设置虹膜状态
        ui->labICCardNum->setText(query.value(6).toString());               //设置IC卡号

        ui->txtNote->setText(query.value(7).toString());            //设置备注
    }

    emit this->sigLoadFaceImage();

//    if(QFile::exists(QString::fromUtf8("%1.%2").arg(ui->txtWorkSN->text()).arg(IMAGE_FORMAT)))
//    {
//        QImage img;
//        img.load(QString::fromUtf8("%1.%2").arg(ui->txtWorkSN->text()).arg(IMAGE_FORMAT), IMAGE_FORMAT.toUtf8().data());
//        img = img.scaled(ui->labDispPhoto->size(), Qt::KeepAspectRatio);

//        ui->labDispPhoto->setPixmap(QPixmap::fromImage(img));
//    }
}


/*****************************************************************************
*                        返回操作
*  函 数 名：slotBack
*  功    能：槽函数， 从人员详细信息页面返回到人员管理页面的操作处理函数
*  说    明：
*  参    数：
*  返 回 值：
*  创 建 人：王磊
*  创建时间：2013-10-18
*  修 改 人：
*  修改时间：
*****************************************************************************/
void DetailPersonInfo::slotBack()
{
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
*  创 建 人：王磊
*  创建时间：2013-10-18
*  修 改 人：
*  修改时间：
*****************************************************************************/
void DetailPersonInfo::slotSave()
{
    //TO DO
    if(ui->txtWorkSN->text().isEmpty())
    {
        QMessageBox::information(this, QString::fromUtf8("添加人员信息"), QString::fromUtf8("工号不能为空"));
        return;
    }


    //存储人员头像
    saveFaceImage();

    //获取当前部门ID
    int departId;
    if(!queryDepartIDByName(departId, ui->cmbDepart->currentText()))
    {
        return;
    }

    QSqlQuery query;
    if(isUpdateOpt)
    {
        //更新人员信息
        query.clear();
        query.prepare("UPDATE person SET idnum = :idnum,\n"
                      "departid = :departid,\n"
                      "name = :name,\n"
                      "workid = :workid,\n"
                      "gender = :gender,\n"
                      "card = :card,\n"
                      "iris = :iris,\n"
                      "note = :note\n"
                      "where workid = :tworkid");
        query.bindValue(":tworkid", ui->txtWorkSN->text());
    }
    else
    {
        query.clear();
        query.prepare("SELECT workid from person where workid = :workid");
        query.bindValue(":workid", ui->txtWorkSN->text());
        query.exec();

        if(query.next())
        {
            //询问用户确认
            QMessageBox::warning(this, QString::fromUtf8("添加人员信息"), QString::fromUtf8("该工号已被使用，请重新分配工号"), QMessageBox::Ok);
            return;
        }

        //添加人员信息
        query.clear();
        query.prepare("INSERT INTO person (workid, name, gender, iris, card, note, idnum, departid) "
                                      "VALUES(:workid, :name, :gender, :iris, :card, :note, :idnum, :departid)");
    }

    //获取性别
    QString gender = QString::fromUtf8("男");
    if(ui->rbtnFamale->isChecked())
    {
        gender = QString::fromUtf8("女");
    }

    query.bindValue(":workid", ui->txtWorkSN->text().trimmed());
    query.bindValue(":name", ui->txtName->text().trimmed());
    query.bindValue(":gender", gender.trimmed());
    query.bindValue(":iris", ui->labIrisStatusValue->text().trimmed());
    query.bindValue(":card", ui->labICCardNum->text().trimmed());
    query.bindValue(":note", ui->txtNote->text().trimmed());
    query.bindValue(":idnum", ui->txtIDNum->text().trimmed());
    query.bindValue(":departid", departId);


    query.exec();
    if(query.isActive())
    {
        QMessageBox::information(this, QString::fromUtf8("添加人员"), QString::fromUtf8("操作成功"), QMessageBox::Ok);

        close();     //关闭当前窗口
    }
    else
    {
        QMessageBox::critical(this, QString::fromUtf8("添加人员"), query.lastError().text(), QMessageBox::Ok);
    }

    return;
}

/*****************************************************************************
*                        采集人脸照片
*  函 数 名：slotCapturePhoto
*  功    能：槽函数，处理采集人脸照片操作
*  说    明：
*  参    数：
*  返 回 值：
*  创 建 人：王磊
*  创建时间：2013-10-18
*  修 改 人：
*  修改时间：
*****************************************************************************/
void DetailPersonInfo::slotCapturePhoto()
{
    //TO DO
    if(ui->txtName->text().isEmpty() || ui->txtWorkSN->text().isEmpty())
    {
        QMessageBox::information(this, QString::fromUtf8("采集人脸图像"), QString::fromUtf8("请先输入人员姓名和工号"));
        return;
    }

    //初始化人脸图像采集设备
    if(!_capturePhoto.initCamera())
    {
        return;
    }

    //设置人脸图像人员信息
    _capturePhoto.setFileNameInfo(getFacePhotoName());
    _capturePhoto.exec();
}

/*****************************************************************************
*                        读取IC卡卡号
*  函 数 名：slotGetICCardNum
*  功    能：
*  说    明：
*  参    数：
*  返 回 值：
*  创 建 人：王磊
*  创建时间：2014-01-16
*  修 改 人：
*  修改时间：
*****************************************************************************/
void DetailPersonInfo::slotGetICCardNum(QString cardNum)
{
    //TO DO
    ui->labICCardNum->setText(cardNum);
}

/*****************************************************************************
*                        删除人脸照片
*  函 数 名：slotDeletePhoto
*  功    能：槽函数，删除人脸照片操作处理函数
*  说    明：
*  参    数：
*  返 回 值：
*  创 建 人：王磊
*  创建时间：2013-10-18
*  修 改 人：
*  修改时间：
*****************************************************************************/
void DetailPersonInfo::slotDeletePhoto()
{
    //TO DO
    if(QMessageBox::Ok != QMessageBox::question(this, QString::fromUtf8("删除人脸图像"), QString::fromUtf8("确定要删除吗?"),
                                 QMessageBox::Ok|QMessageBox::Cancel, QMessageBox::Ok))
    {
        return;
    }

    if(!newImageUnsaved)
    {
        QString fileName(getFacePhotoName());
        if(QFile::exists(fileName))
        {
            QFile::remove(fileName);
        }
    }
    else
    {
        //更新人脸图像存储操作标记
        newImageUnsaved = false;
    }

    //更新人脸图像
    emit sigLoadFaceImage();

    return;
}

/*****************************************************************************
*                        管理部门
*  函 数 名：slotManageDepart
*  功    能：槽函数，管理部门操作处理函数
*  说    明：
*  参    数：
*  返 回 值：
*  创 建 人：王磊
*  创建时间：2013-10-18
*  修 改 人：
*  修改时间：
*****************************************************************************/
void DetailPersonInfo::slotManageDepart()
{
    //TO DO
    DepartManage dpm(this);
    dpm.exec();
    show();
}

/*****************************************************************************
*                        获取人脸照片
*  函 数 名：slotGetPhoto
*  功    能：获取人脸照片操作处理函数
*  说    明：
*  参    数：
*  返 回 值：
*  创 建 人：王磊
*  创建时间：2013-10-18
*  修 改 人：
*  修改时间：
*****************************************************************************/
void DetailPersonInfo::slotGetPhoto(QImage img)
{
    if(!img.isNull())
    {
        ui->labDispPhoto->setPixmap(QPixmap::fromImage(img));
        newImageUnsaved = true;
    }
    else
    {
        emit sigLoadFaceImage();   //加载本地的已有人脸图像
    }

}

void DetailPersonInfo::slotLoadFaceImage()
{
    QString fileName(getFacePhotoName());
    if(QFile::exists(fileName))
    {
        QImage localeImage(IMAGE_WIDTH, IMAGE_HEIGHT, QImage::Format_RGB888);
        localeImage.load(fileName, IMAGE_FORMAT.toUtf8().data());

        ui->labDispPhoto->setPixmap(QPixmap::fromImage(localeImage));
    }
    else
    {
        ui->labDispPhoto->setText(QString::fromUtf8("请添加照片"));
    }
}

void DetailPersonInfo::show()
{
    QDialog::show();
    initDeptLst();
}

bool DetailPersonInfo::queryDepartIDByName(int &deptID, const QString &deptName)
{
    //TO DO
    QSqlQuery query;
    query.prepare("SELECT id FROM depart where name = :name");   //通过查询部门表获取部门id
    query.bindValue(":name", ui->cmbDepart->currentText());
    query.exec();

    //获取当前部门ID
    if(query.next())
    {
        deptID = query.value(0).toInt();
        return true;
    }
    else
    {
        QMessageBox::warning(this, QString::fromUtf8("添加人员信息"), QString::fromUtf8("该部门不存在，请重新选择部门"), QMessageBox::Ok);
        return false;
    }
}


bool DetailPersonInfo::addPersonInfo()
{
    return true;
}

bool DetailPersonInfo::updatePersonInfo()
{
    return true;
}


bool DetailPersonInfo::saveFaceImage()
{
    if(!newImageUnsaved)
    {
        return true;
    }
    
    QString fileName(getFacePhotoName());
    //如果文件已存在，是否需要覆盖
    if(QFile::exists(fileName))
    {
        if(QMessageBox::Ok != QMessageBox::warning(this, QString::fromUtf8("保存人员头像"), QString::fromUtf8("是否替换已有头像照片"),
                                                   QMessageBox::Ok | QMessageBox::Cancel, QMessageBox::Ok))
        {
            return true;
        }
    }

    //更新图像存储操作标记
    newImageUnsaved = false;

    return ui->labDispPhoto->pixmap()->save(getFacePhotoName(), IMAGE_FORMAT.toUtf8().data());
}


QString DetailPersonInfo::getFacePhotoName()
{
    return QString::fromUtf8("%1%2%3.%4").arg(getFacePhotoPath()).arg(QDir::separator()).arg(ui->txtWorkSN->text().trimmed()).arg(IMAGE_FORMAT);
}

QString DetailPersonInfo::getFacePhotoPath()
{
    QString facePhotoDir("facephoto");
    QDir dir(QDir::currentPath());
    if(!dir.exists(facePhotoDir))
    {
        if(!dir.mkdir(facePhotoDir))
        {
            QMessageBox::warning(this, QString::fromUtf8("创建目录"), QString::fromUtf8("无法创建保存人员头像照片的目录"));
        }

        system("sudo chmod 777 *");
    }

    dir.cd(facePhotoDir);
    return dir.dirName();
}
