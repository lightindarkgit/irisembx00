/*****************************************************************************
 ** 文 件 名：datamanage.cpp
 ** 主 要 类：DataManage
 **
 ** Copyright (c) 中科虹霸有限公司
 **
 ** 创 建 人：L.Wang
 ** 创建时间：20013-10-18
 **
 ** 修 改 人：
 ** 修改时间：
 ** 描  述:   数据管理
 ** 主要模块说明: 人员信息， 虹膜， 识别记录的导入导出
 **
 ** 版  本:   1.0.0
 ** 备  注:
 **
 *****************************************************************************/
#include "datamanage.h"
#include "buffering.h"
#include "ui_datamanage.h"
#include "ikmessagebox.h"
#include "../Common/Logger.h"
#include "../Common/Exectime.h"
#include "commononqt.h"
#include "Global.h"

#include <QFile>
#include <QDir>
#include <QMessageBox>
#include<QProcess>
#include <QDebug>
#include <QDate>


#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <sys/unistd.h>
#include <vector>



MountUsb::MountUsb()
{

    system("echo '123456' | sudo ./mountusb.sh");
}

MountUsb::~MountUsb()
{

    system("echo '123456' | sudo ./umountusb.sh");
}


/*****************************************************************************
 *                        获取当前USB存储盘
 *  函 数 名：GetUDiskPaths
 *  功    能：获取可用的USB存储盘列表
 *  说    明：
 *  参    数：
 *  返 回 值：USB存储盘列表
 *  创 建 人：L.Wang
 *  创建时间：2013-10-18
 *  修 改 人：
 *  修改时间：
 *****************************************************************************/
std::vector<QString> GetUDiskPaths();

/*****************************************************************************
 *                        删除文件
 *  函 数 名：DelFile
 *  功    能：
 *  说    明：
 *  参    数：
 *  返 回 值：
 *  创 建 人：liuzhch
 *  创建时间：2014-07-17
 *  修 改 人：
 *  修改时间：
 *****************************************************************************/
bool DelFile(QString sourceDir);
/*****************************************************************************
 *                        拷贝文件
 *  函 数 名：CopyFile
 *  功    能：
 *  说    明：
 *  参    数：
 *  返 回 值：
 *  创 建 人：liuzhch
 *  创建时间：2014-07-16
 *  修 改 人：
 *  修改时间：
 *****************************************************************************/
bool CopyFile(QString sourceDir ,QString toDir, bool coverFileIfExist) ;
/*****************************************************************************
 *                        获取导入数据的文件路径
 *  函 数 名：GetImportDBDataFilePath
 *  功    能：
 *  说    明：
 *  参    数：
 *  返 回 值：导入数据的文件路径
 *  创 建 人：L.Wang
 *  创建时间：2013-10-18
 *  修 改 人：
 *  修改时间：
 *****************************************************************************/
QString GetDBDataFileName();

/*****************************************************************************
 *                        获取导入数据的文件路径
 *  函 数 名：GetImportDBDataFilePath
 *  功    能：
 *  说    明：
 *  参    数：
 *  返 回 值：导入数据的文件路径
 *  创 建 人：L.Wang
 *  创建时间：2013-10-18
 *  修 改 人：
 *  修改时间：
 *****************************************************************************/
QString GetImportDBDataFilePath();

/*****************************************************************************
 *                        获取导出数据的文件路径
 *  函 数 名：GetExportDBDataFilePath
 *  功    能：
 *  说    明：
 *  参    数：
 *  返 回 值：导入数据的文件路径
 *  创 建 人：L.Wang
 *  创建时间：2013-10-18
 *  修 改 人：
 *  修改时间：
 *****************************************************************************/
QString GetExportDBDataFilePath();






DataManage::DataManage(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DataManage)
{

    ui->setupUi(this);
    initUI();


    setStyleSheet("QPushButton{"
                  "color: white;"
                  "background-color: rgb(32, 90, 160);"
                  "border: 2px groove gray;"
                  "border-radius: 10px;"
                  "padding: 2px 4px}\n"
                  );/*"DataManage{background-image:url(:/image/bkground.jpg);}"*/


    //连接信号与槽参数，使相应的信号得到想对应的处理
    connect(ui->btnBack, SIGNAL(clicked()), this, SIGNAL(sigGoBack()));
    connect(ui->btnImport, SIGNAL(clicked()), this, SLOT(slotImport()));
    connect(ui->btnExport, SIGNAL(clicked()), this, SLOT(slotExport()));

    ui->box_irisData->setEnabled(false);

    //connect(ui->chkPersonInfo, SIGNAL(stateChanged(int)), this, SLOT(slotPersonInfoSelectChanged()));
    //liu
    //connect(ui->chkRecord, SIGNAL(stateChanged(int)), this, SLOT(slotExportOnlyItemSelectChanged(int)));
    //connect(ui->chkFacePhoto, SIGNAL(stateChanged(int)), this, SLOT(slotExportOnlyItemSelectChanged(int)));
}

DataManage::~DataManage()
{

    delete ui;
}

/*****************************************************************************
 *                        初始化导入导出选项
 *  函 数 名：initUI
 *  功    能：设置所有checkbox为未选中状态
 *  说    明：
 *  参    数：
 *  返 回 值：选项列表字符串
 *  创 建 人：L.Wang
 *  创建时间：2013-10-18
 *  修 改 人：
 *  修改时间：
 *****************************************************************************/
bool DataManage::initUI()
{


    isPersonInfoChecked =   false;
    isIrisChecked       =   false;
    isRecordChedked     =   false;
    isFacePhotoChedked  =   false;
    isErroReportChecked =   false;
    ui->box_facePhoto->setChecked(false);
    ui->box_identRec->setChecked(false);
    ui->box_irisData->setChecked(false);
    ui->box_personInfo->setChecked(false);
    ui->btn_errReport->setChecked(false);

    ui->box_irisData->setEnabled(isPersonInfoChecked);

    //设置默认焦点
    ui->btnBack->setFocus();


    return true;
}

/*****************************************************************************
 *                        获取数据导入导出的选项列表
 *  函 数 名：getCheckedItems
 *  功    能：获取用户进行数据导入导出的选项列表
 *  说    明：
 *  参    数：
 *  返 回 值：选项列表字符串
 *  创 建 人：L.Wang
 *  创建时间：2013-10-18
 *  修 改 人：
 *  修改时间：
 *****************************************************************************/
bool DataManage::getCheckedItems()
{

    //TO DO
    tables.clear();

    if(isPersonInfoChecked)
    {
        tables.push_back(TBPerson);
        //tables.push_back(TBPersonImage);
        //tables.push_back(TBDepart);
    }

    if(isIrisChecked)
    {
        tables.push_back(TBIrisDataBase);
    }

    if(isRecordChedked)
    {
        tables.push_back(TBPersonRecLog);
    }

    return true;
}


/*****************************************************************************
 *                        是否已指定导入对象
 *  函 数 名：IsAtLeastOneItemSelected
 *  功    能：判断用户是否已选择至少一项导入选项
 *  说    明：
 *  参    数：
 *  返 回 值：
 *  创 建 人：L.Wang
 *  创建时间：2015-10-10
 *  修 改 人：
 *  修改时间：
 *****************************************************************************/
bool DataManage::IsAtLeastOneItemSelected()
{

    //由于要选中虹膜必然已选中人员信息，因此虹膜选项是否选中无需要判断，除非虹膜和人员信息的依赖关系被改变
    return (isPersonInfoChecked ||
            isRecordChedked     ||
            isFacePhotoChedked  ||
            isErroReportChecked);
}



/*****************************************************************************
 *                        数据导入操作
 *  函 数 名：slotImport
 *  功    能：槽函数，导入数据操作处理函数
 *  说    明：
 *  参    数：
 *  返 回 值：
 *  创 建 人：L.Wang
 *  创建时间：2013-10-18
 *  修 改 人：
 *  修改时间：
 *****************************************************************************/
void DataManage::slotImport()
{

    //TO DO
    //获取导入选项
    getCheckedItems();

    //如果没有选择要导入的数据则给出提示并直接返回
    if(!IsAtLeastOneItemSelected())
    {
        IKMessageBox errNote(this->parent(), QString::fromUtf8("数据导入"), QString::fromUtf8("请选择要导入的数据！"), errorMbx);

        errNote.exec();
        return;
    }

    MountUsb mu;
    //创建等待界面
    Buffering bufferingUI(this,"数据导入中，请不要拔出U盘！");
    bufferingUI.move(30,70);
    connect(this, SIGNAL(sigCloseBufferingDlg()), &bufferingUI, SLOT(close()));

    //创建线程执行数据的导入，以免界面被后台程序冻结
    pthread_t threadID;
    pthread_create(&threadID, nullptr, importData, this);

    //显示导出操作等待界面
    bufferingUI.exec();


    //等待导出线程结束
    void* threadStatus;
    pthread_join(threadID, &threadStatus);  //有可能导致界面卡

    ////////////////////////////////////////////////////

    int nret = CheckCardIDAndGen();
    if(nret != 0)
    {
        LOG_ERROR("CheckCardIDAndGen(pInfo.personID) failed");
    }

    //提示用户导入操作完成
    //-liu去掉 GetTips() +
    IKMessageBox feedback(this, QString::fromUtf8("数据导入"), res, informationMbx);
    feedback.exec();
    return;
}

/*****************************************************************************
 *                        数据导出操作
 *  函 数 名：slotExport
 *  功    能：槽函数，导出数据处理函数
 *  说    明：
 *  参    数：
 *  返 回 值：
 *  创 建 人：L.Wang
 *  创建时间：2013-10-18
 *  修 改 人：
 *  修改时间：
 *****************************************************************************/
void DataManage::slotExport()
{

    //TO DO
    //获取导出选项
    getCheckedItems();

    //如果没有选择要导出的数据则给出提示并直接返回
    if(!IsAtLeastOneItemSelected())
    {
        IKMessageBox error(this, QString::fromUtf8("数据导出"), QString::fromUtf8("请选择要导出的数据！"), errorMbx);
        error.exec();
        return;
    }


    MountUsb mu;
    //创建等待界面
    Buffering bufferingUI(this,"数据导出中，请不要拔出U盘！");
    bufferingUI.move(30,70);
    connect(this, SIGNAL(sigCloseBufferingDlg()), &bufferingUI, SLOT(close()));

    //创建线程执行数据的导出，以免界面被后台程序冻结
    pthread_t threadID;
    pthread_create(&threadID, nullptr, exportData, this);

    //@dj
    pthread_detach(threadID);

    ui->btnBack->setEnabled(false);

    //显示导出操作等待界面
    bufferingUI.exec();

//    //等待导出线程结束
//    void* threadStatus;
//    pthread_join(threadID, &threadStatus);


    //提示用户导出操作完成
    IKMessageBox feedback(this, QString::fromUtf8("数据导出"), res, informationMbx);
    feedback.exec();

    system("echo '123456' | sudo ./umountusb.sh");
    ui->btnBack->setEnabled(true);

}

/*****************************************************************************
 *                        人员信息选择状态变化处理函数
 *  函 数 名：slotPersonInfoSelectChanged
 *  功    能：槽函数，处理人员信息选择状态变化的情况，保证虹膜选项只有在人员信息选中状态下可选
 *  说    明：
 *  参    数：
 *  返 回 值：
 *  创 建 人：L.Wang
 *  创建时间：2013-10-18
 *  修 改 人：
 *  修改时间：
 *****************************************************************************/
void DataManage::slotPersonInfoSelectChanged()
{

    //    if(ui->chkPersonInfo->isChecked())
    //    {
    //        ui->chkIris->setCheckable(true);
    //        ui->chkIris->setEnabled(true);

    //    }
    //    else
    //    {
    //        ui->chkIris->setChecked(false);
    //        ui->chkIris->setCheckable(false);
    //        ui->chkIris->setEnabled(false);
    //    }
}

/*****************************************************************************
 *                        只可导出记录信息选择状态变化处理函数
 *  函 数 名：slotRecogInfoSelectChanged
 *  功    能：保证记录只能导出不能导入
 *  说    明：
 *  参    数：
 *  返 回 值：
 *  创 建 人：liuzhch
 *  创建时间：2014-07-10
 *  修 改 人：
 *  修改时间：
 *****************************************************************************/
void DataManage::slotExportOnlyItemSelectChanged()
{


    if( isFacePhotoChedked || isRecordChedked || isErroReportChecked )
    {
        ui->btnImport->setEnabled(false);
    }
    else
    {
        ui->btnImport->setEnabled(true);
    }
}

/*****************************************************************************
 *                        显示数据导入导出结果
 *  函 数 名：slotShowRes
 *  功    能：
 *  说    明：
 *  参    数：
 *  返 回 值：
 *  创 建 人：L.Wang
 *  创建时间：2013-10-18
 *  修 改 人：
 *  修改时间：
 *****************************************************************************/
void DataManage::slotShowRes()
{

    IKMessageBox error(this, QString::fromUtf8("数据导出"), QString::fromUtf8("数据导出成功！"), errorMbx);
    error.exec();
}

/*****************************************************************************
 *                        屏蔽Esc键信号
 *  函 数 名：reject
 *  功    能：
 *  说    明：
 *  参    数：
 *  返 回 值：
 *  创 建 人：L.Wang
 *  创建时间：2014-08-14
 *  修 改 人：
 *  修改时间：
 *****************************************************************************/
void DataManage::reject()
{

    //DO NOTHING
}

/*****************************************************************************
 *                        导入数据
 *  函 数 名：importData
 *  功    能：独立线程用于处理用户的数据导入操作
 *  说    明：
 *  参    数：
 *          arg， 数据导入模块指针
 *  返 回 值：
 *  创 建 人：L.Wang
 *  创建时间：2013-10-18
 *  修 改 人：
 *  修改时间：
 *****************************************************************************/
void* DataManage::importData(void* arg)
{

    //TO DO
    DataManage* dm = (DataManage*)arg;
    //获取待导入数据的文件路径
    QString fileName = GetImportDBDataFilePath();

    if(fileName.isEmpty())    //如果没有要导入的文件
    {
        dm->res = QString::fromUtf8("导入失败，文件不存在！");
    }
    else if(-1 == access(fileName.toStdString().c_str(), R_OK))      //如果对要到如的文件没有读权限
    {
        dm->res = QString::fromUtf8("导入失败，无文件读取权限！");
    }
    else
    {
        if(CopyFile(fileName,(QCoreApplication::applicationDirPath() + QDir::separator() + GetDBDataFileName()),true))
        {
            ikIOStream *importDB = ikIOStream::GetIkIOStream();

            int rtnVal = importDB->ImportFromMoreFile((QCoreApplication::applicationDirPath()
                                                       + QDir::separator() + GetDBDataFileName()).toStdString(), dm->tables);

            QString err;
            switch(rtnVal)
            {
            case 0:
                err = QString::fromUtf8("操作成功！");
                break;
            case 1:
                err = QString::fromUtf8("'%1'导入失败！").arg(GetTableName(dm->tables[0]));
                break;
            case 2:
                err = QString::fromUtf8("'%1'导入失败！").arg(GetTableName(dm->tables[1]));
                break;
            case 3:
                err = QString::fromUtf8("'%1和%2'导入失败！").arg(GetTableName(dm->tables[0]), GetTableName(dm->tables[1]));
                break;
            case 4:
                err = QString::fromUtf8("'%1'导入失败！").arg(GetTableName(dm->tables[2]));
                break;
            case 5:
                err = QString::fromUtf8("'%1和%2'导入失败！").arg(GetTableName(dm->tables[0]), GetTableName(dm->tables[2]));
                break;
            case 6:
                err = QString::fromUtf8("'%1和%2'导入失败！").arg(GetTableName(dm->tables[1]), GetTableName(dm->tables[2]));
                break;
            case 7:
            default:
                err = QString::fromUtf8("导入失败！");
                break;
            }
            dm->res =err;
        }

        //记录操作结果信息
        //dm->res = !rtnVal ? QString::fromUtf8("操作成功") : QString::fromUtf8("导入失败");
    }

    //sleep(8);
    //关闭数据导入时的等待界面
    emit dm->sigCloseBufferingDlg();

    return (void *)0;
}

QString DataManage::GetTableName(TableType type)
{

    switch(type)
    {
    case TBPersonRecLog:
        return QString::fromUtf8("识别记录表");
        break;
    case TBIrisDataBase:
        return QString::fromUtf8("虹膜数据表");
        break;
    case TBPerson:
        return QString::fromUtf8("人员信息表");
        break;
    case TBPersonImage:
        return QString::fromUtf8("人员图像表");
        break;
    default:
        break;
    }

    return "";
}

/*****************************************************************************
 *                        导出数据
 *  函 数 名：exportData
 *  功    能：独立线程用于处理用户的数据导出操作
 *  说    明：
 *  参    数：
 *          arg， 数据导入模块指针
 *  返 回 值：
 *  创 建 人：L.Wang
 *  创建时间：2013-10-18
 *  修 改 人：
 *  修改时间：
 *****************************************************************************/
void* DataManage::exportData(void* arg)
{

    //TO DO
    DataManage* dm = (DataManage*)arg;
    QString err;

    //导出虹膜数据
    if(dm->isPersonInfoChecked || dm->isRecordChedked )
    {
        //获取数据导出文件路径
        QString fileName = GetExportDBDataFilePath();
        QString tmpDBFile = QCoreApplication::applicationDirPath() + "/" + GetDBDataFileName();

        if(fileName.isEmpty())
        {
            dm->res = QString::fromUtf8("导出失败，未发现USB存储设备！");
            LOG_ERROR("导出失败，未发现USB存储设备！");
        }
        else
        {
            //获取数据导出对象
            ikIOStream *exportDB = ikIOStream::GetIkIOStream();


            DelFile(tmpDBFile);
            int rtnVal = exportDB->ExportToMoreFile(tmpDBFile.toStdString(), dm->tables);

            //liu--加

            switch(rtnVal)
            {
            case 0:
                if(0 == system(QString("echo '123456' | sudo mv -f %1 %2").arg(tmpDBFile, fileName).toStdString().c_str()))
                {
                    err = QString::fromUtf8("操作成功！");
                }
                else
                {
                    err = QString::fromUtf8("导出失败！");
                }
                break;
            case 1:
                err = QString::fromUtf8("'%1'导出失败！").arg(GetTableName(dm->tables[0]));
                break;
            case 2:
                err = QString::fromUtf8("'%1'导出失败！").arg(GetTableName(dm->tables[1]));
                break;
            case 3:
                err = QString::fromUtf8("'%1和%2'导出失败！").arg(GetTableName(dm->tables[0]), GetTableName(dm->tables[1]));
                break;
            case 4:
                err = QString::fromUtf8("'%1'导出失败！").arg(GetTableName(dm->tables[2]));
                break;
            case 5:
                err = QString::fromUtf8("'%1和%2'导出失败！").arg(GetTableName(dm->tables[0]), GetTableName(dm->tables[2]));
                break;
            case 6:
                err = QString::fromUtf8("'%1和%2'导出失败！").arg(GetTableName(dm->tables[1]), GetTableName(dm->tables[2]));
                break;
            case 7:
            default:
                err = QString::fromUtf8("导出失败！");
                break;
            }


        }

        LOG_INFO("%s",err.toStdString().c_str());
    }


    //导出人脸
    if(dm->isFacePhotoChedked)
    {
        if(dm->ExportFaceImgs("",""))
        {
            err = QString::fromUtf8("操作成功！");

        }
        else
        {
            err = QString::fromUtf8("导出人脸失败");
        }
    }

    //到处日志记录
    if(dm->isErroReportChecked)
    {
        if(dm->ExportErroLogs("",""))
        {
            err = QString::fromUtf8("操作成功！");

        }
        else
        {
            err = QString::fromUtf8("导出日志失败");
        }

    }

    bool isUsbExit = dm->CheckUsbExit();
    if(!isUsbExit)
    {
        err = QString::fromUtf8("检测不到USB设备，请插入U盘");

    }


    dm->res = err;


    //记录操作结果信息 liu注释
    //dm->res = !rtnVal ? QString::fromUtf8("操作成功") : QString::fromUtf8("导出失败");



    //关闭数据导出时的等待界面
    emit dm->sigCloseBufferingDlg();

    return (void *)0;
}


/*****************************************************************************
 *                        获取导入导出选项信息
 *  函 数 名：GetTips
 *  功    能：跟踪用户选择的数据导入导出项
 *  说    明：
 *  参    数：
 *  返 回 值：
 *  创 建 人：L.Wang
 *  创建时间：2013-10-18
 *  修 改 人：
 *  修改时间：
 *****************************************************************************/
QString DataManage::GetTips()
{

    //TO DO
    QStringList tips;
    std::vector<TableType>::iterator table = tables.begin();
    std::vector<TableType>::iterator tablesEnd = tables.end();

    for(; table != tablesEnd; ++table)
    {
        switch(*table)
        {
        case TBPerson:
            tips << QString::fromUtf8("人员信息");
            break;
        case TBIrisDataBase:
            tips << QString::fromUtf8("虹膜特征");
            break;
        case TBPersonRecLog:
            tips << QString::fromUtf8("识别记录");
            break;
        case TBPersonImage:
            tips << QString::fromUtf8("人员图像");
            break;
        default:
            std::cout << "[import error]: nothing needs to import" << std::endl;
            LOG_INFO("nothing needs to import");
            break;
        }
    }

    return tips.join(QString::fromUtf8("、"));
}


///*****************************************************************************
//*                        获取当前USB存储盘
//*  函 数 名：GetUDiskPaths
//*  功    能：获取可用的USB存储盘列表
//*  说    明：
//*  参    数：
//*  返 回 值：USB存储盘列表
//*  创 建 人：L.Wang
//*  创建时间：2013-10-18
//*  修 改 人：
//*  修改时间：
//*****************************************************************************/
//std::vector<QString> GetUDiskPaths()
//{
//    //TO DO
//    QDir dir("/media/");

//    std::vector<QString> uDisks;
//    foreach (QString subDir, dir.entryList(QDir::NoSymLinks | QDir::Dirs | QDir::NoDotAndDotDot))
//    {
//        if(!subDir.contains("flop"))
//        {
//            uDisks.push_back(dir.absoluteFilePath(subDir));
//        }
//    }

//    return uDisks;
//}


/*****************************************************************************
 *                        获取当前USB存储盘
 *  函 数 名：GetUDiskPaths
 *  功    能：挂载U盘并返回U盘节点的矢量表
 *  说    明：
 *  参    数：
 *  返 回 值：USB存储盘列表
 *  创 建 人：L.Wang
 *  创建时间：2014-11-18
 *  修 改 人：
 *  修改时间：
 *****************************************************************************/
std::vector<QString> GetUDiskPaths()
{

    //TO DO
    std::vector<QString> uDisks;
    //        QDir dir("/media/");
    //    foreach (QString subDir, dir.entryList(QDir::NoSymLinks | QDir::Dirs | QDir::NoDotAndDotDot))
    //    {
    //        if(!subDir.contains("flop"))
    //        {
    //            uDisks.push_back(dir.absoluteFilePath(subDir));
    //        }
    //    }

    system("echo '123456' | sudo ./mountusb.sh");
    QDir dir("/mnt/ikusb");
    if(dir.exists())
    {
        uDisks.push_back("/mnt/ikusb");
    }


    return uDisks;
}


/*****************************************************************************
 *                        获取导入导出文件名称
 *  函 数 名：GetDBDataFileName
 *  功    能：
 *  说    明：
 *  参    数：
 *  返 回 值：导入导出的文件路径
 *  创 建 人：L.Wang
 *  创建时间：2013-10-18
 *  修 改 人：
 *  修改时间：
 *****************************************************************************/
QString GetDBDataFileName()
{

    //TO DO
    return QString("ikdatabase.db");
}

/*****************************************************************************
 *                        拷贝文件
 *  函 数 名：CopyFile
 *  功    能：
 *  说    明：
 *  参    数：
 *  返 回 值：
 *  创 建 人：liuzhch
 *  创建时间：2014-07-16
 *  修 改 人：
 *  修改时间：
 *****************************************************************************/
bool CopyFile(QString sourceDir ,QString toDir, bool coverFileIfExist)
{

    try
    {
        toDir.replace("\\","/");
        if (sourceDir == toDir)
        {
            return true;
        }
        if (!QFile::exists(sourceDir))
        {
            return false;
        }
        //        QDir *createfile     = new QDir;
        //        bool exist = createfile->exists(toDir);
        //        if (exist)
        //        {
        //            if(coverFileIfExist)
        //            {
        //                if(createfile->remove(toDir))
        //                {
        //                    //删除
        //                    std::cout<<"删除成功！toDir :"<<toDir.toStdString()<<std::endl;
        //                }
        //            }
        //        }//end if
        if(coverFileIfExist)
        {
            DelFile(toDir);
        }
        qDebug()<<sourceDir<<"++++++++++"<<toDir<<endl;
        if(!QFile::copy(sourceDir, toDir))
        {
            return false;
        }

    }catch(...)
    {
        return false;
    }

    return true;
}

/*****************************************************************************
 *                        删除文件
 *  函 数 名：DelFile
 *  功    能：
 *  说    明：
 *  参    数：
 *  返 回 值：
 *  创 建 人：liuzhch
 *  创建时间：2014-07-17
 *  修 改 人：
 *  修改时间：
 *****************************************************************************/
bool DelFile(QString sourceDir)
{

    return 0 == system(QString("test -f %1 && rm -f %1").arg(sourceDir).toStdString().c_str());

}


/*****************************************************************************
 *                        获取导入数据的文件路径
 *  函 数 名：GetImportDBDataFilePath
 *  功    能：
 *  说    明：
 *  参    数：
 *  返 回 值：导入数据的文件路径
 *  创 建 人：L.Wang
 *  创建时间：2013-10-18
 *  修 改 人：
 *  修改时间：
 *****************************************************************************/
QString GetImportDBDataFilePath()
{

    //TO DO
    std::vector<QString> uDisks = GetUDiskPaths();     //所有U盘根路径
    QString dbFileName = GetDBDataFileName();      //数据库数据文件名

    //获取数据库数据文件路径
    foreach (QString disk, uDisks)
    {
        QDir sDir(disk);
        if(sDir.exists(dbFileName))
        {
            return sDir.absoluteFilePath(dbFileName);
        }
    }


    return  QString("");
}


/*****************************************************************************
 *                        获取导出数据的文件路径
 *  函 数 名：GetExportDBDataFilePath
 *  功    能：
 *  说    明：
 *  参    数：
 *  返 回 值：导入数据的文件路径
 *  创 建 人：L.Wang
 *  创建时间：2013-10-18
 *  修 改 人：
 *  修改时间：
 *****************************************************************************/
QString GetExportDBDataFilePath()
{

    //TO DO
    std::vector<QString> uDirs = GetUDiskPaths();
    if(uDirs.empty())
    {
        return QString("");
    }

    QDir targetDir(uDirs.at(0));


    return  targetDir.absoluteFilePath(GetDBDataFileName());
}

/*****************************************************************************
 *                        刷新页面
 *  函 数 名：refresh
 *  功    能：
 *  说    明：
 *  参    数：
 *  返 回 值：导入数据的文件路径
 *  创 建 人：L.Wang
 *  创建时间：2013-10-18
 *  修 改 人：
 *  修改时间：
 *****************************************************************************/
void DataManage::refresh()
{

    initUI();
}

/*****************************************************************************
 *                        导出面部照片
 *  函 数 名：ExportFaceImgs
 *  功    能：
 *  说    明：
 *  参    数：
 *  返 回 值：成功返回true，其他返回false
 *  创 建 人：L.Wang
 *  创建时间：2015-09-18
 *  修 改 人：
 *  修改时间：
 *****************************************************************************/
bool DataManage::ExportFaceImgs(QString begTime, QString endTime)
{
    //TODO
    int unbackedupImgExpt = 0;
    int backedupImgExpt = 0;

    unbackedupImgExpt = ExportUnbackupedFaceImages();
    qDebug() << QString("[DataManage.ExportFaceImgs]unbacked up image exported: ") << unbackedupImgExpt;
    backedupImgExpt = ExportBackupedFaceImages();
    qDebug() << QString("[DataManage.ExportFaceImgs]backed up image exported: ") << backedupImgExpt;

    return !(unbackedupImgExpt + backedupImgExpt < 0);
}

/*****************************************************************************
 *                        导出log
 *  函 数 ExportErroLogs
 *  功    能：
 *  说    明：
 *  参    数：
 *  返 回 值：成功返回true，其他返回false
 *  创 建 人：guodj
 *  创建时间：2016-07-5
 *  修 改 人：
 *  修改时间：
 *****************************************************************************/
bool DataManage::ExportErroLogs(QString begTime, QString endTime)
{

    QDir dir(QCoreApplication::applicationDirPath());
    QString logsFolder("log");

    if(dir.exists(logsFolder))
    {
        return CopyDirectory(dir.filePath(logsFolder), GetLogsExportToPath(),7);
    }

    return false;
}
/*****************************************************************************
 *                        导出已备份的面部照片
 *  函 数 名：ExportBackupedFaceImages
 *  功    能：
 *  说    明：
 *  参    数：
 *  返 回 值：返回导出的面部照片数量
 *  创 建 人：L.Wang
 *  创建时间：2015-10-10
 *  修 改 人：
 *  修改时间：
 *****************************************************************************/
int DataManage::ExportBackupedFaceImages()
{

    QDir dir(QCoreApplication::applicationDirPath());
    QString facePhotoDirName("recogFaceBK");

    if(dir.exists(facePhotoDirName))
    {
        return CopyDirectory(dir.filePath(facePhotoDirName), GetFacePhotoExportToPath());
    }

    return 0;
}

/*****************************************************************************
 *                        导出新近存储的面部照片
 *  函 数 名：ExportUnbackupedFaceImages
 *  功    能：
 *  说    明：
 *  参    数：
 *  返 回 值：返回导出的面部照片数量
 *  创 建 人：L.Wang
 *  创建时间：2015-10-10
 *  修 改 人：
 *  修改时间：
 *****************************************************************************/
int DataManage::ExportUnbackupedFaceImages()
{

    QDir dir(QCoreApplication::applicationDirPath());
    qDebug()<< "dir=" <<dir<<endl;
    QString facePhotoDirName("recogface");

    if(dir.exists(facePhotoDirName))
    {
        LOG_INFO("recogface path =%s",dir.filePath(facePhotoDirName).toStdString().c_str());

        return CopyDirectory(dir.filePath(facePhotoDirName), GetFacePhotoExportToPath());
    }
    else
    {
        LOG_ERROR("照片文件夹不存在");
    }


    return 0;
}


int DataManage::CopyDirectory(const QString& sPath, const QString& dPath)
{

    bool checkUdisk = true;
    if(sPath.isEmpty() || dPath.isEmpty())
    {
        LOG_ERROR("图像路径不存在或U盘未连接上！");
//        IKMessageBox error(this, QString::fromUtf8("数据导出"), QString::fromUtf8("图像路径不存在或U盘未连接上！"), errorMbx);
//        error.exec();

        return -1;
    }

    //拷贝子目录
    QDir dstDir(dPath);
    QDir srcDir(sPath);

    //文件拷贝计数器
    int cpyCnt = 0;
    QFileInfoList files = srcDir.entryInfoList();
    foreach(QFileInfo f, files)
    {
        //过滤掉非目标文件或文件夹
        if("." == f.fileName() || ".." == f.fileName())
        {
            continue;
        }

        //如果为目录则递归拷贝
        if(f.isDir())
        {
            cpyCnt += CopyDirectory(f.filePath(), dPath);
        }
        else
        {
            //默认覆盖同名文件
            if(dstDir.exists(f.fileName()))
            {
                dstDir.remove(f.fileName());
            }

            std::cout << "[DataManage.CopyDirectory]curent img: " << f.filePath().toStdString()
                      << " d: " << dstDir.filePath(f.fileName()).toStdString() << std::endl;

            //文件拷贝，成功则递增文件拷贝计数器
            /// convert char code
            QString ffilename = f.fileName();

            LOG_INFO("f.filePath() = %s",f.fileName().toStdString().c_str());
            QString dstctr = dstDir.filePath(ffilename);
            LOG_INFO("dstDir.filePath(ffilename)) = %s",dstctr.toStdString().c_str());
            if(QFile::copy(f.filePath(), dstDir.filePath(ffilename)))

            {
                LOG_INFO("current img:%s",f.fileName().toStdString().c_str());

                qDebug() << "[DataManage.CopyDirectory]curent img: " << dstDir.filePath(f.fileName());
                cpyCnt++;
            }
            else
            {
                LOG_ERROR("copy recimg failed!");
            }
        }
    }

    return cpyCnt;
}

int DataManage::CopyDirectory(const QString& sPath, const QString& dPath, int count)
{


    isudiskConnect = true;
    if(sPath.isEmpty() || dPath.isEmpty())
    {
        return -1;
    }

    //拷贝子目录
    QDir dstDir(dPath);
    QDir srcDir(sPath);

    //文件拷贝计数器
    int cpyCnt = 0;
    srcDir.setSorting(QDir::Time);
    QFileInfoList files = srcDir.entryInfoList();
    foreach(QFileInfo f, files)
    {
        //过滤掉非目标文件或文件夹
        if("." == f.fileName() || ".." == f.fileName())
        {
            continue;
        }

        //        //如果为目录则递归拷贝
        //        if(f.isDir())
        //        {
        //            cpyCnt += CopyDirectory(f.filePath(), dPath);
        //        }
        //        else

        //默认覆盖同名文件
        if(dstDir.exists(f.fileName()))
        {
            dstDir.remove(f.fileName());
        }

        QString logdate =f.fileName();
        logdate = logdate.mid(12);
        int year = logdate.mid(0,4).toInt();
        int month = logdate.mid(5,2).toInt();
        int day = logdate.mid(8,2).toInt();
        int curyear     =   QDate::currentDate().year();
        int curmonty    =   QDate::currentDate().month();
        int curday      =   QDate::currentDate().day();
        int logcount       =   curyear*365 + curmonty*31 + curday - (year*365 + month*31 + day);
        if(logcount>7 && cpyCnt>1)
            continue;
        ///message.log.2016-07-08;

        //文件拷贝，成功则递增文件拷贝计数器
        if(QFile::copy(f.filePath(), dstDir.filePath(f.fileName())))
        {
            qDebug() << "LOG" << dstDir.filePath(f.fileName());
            cpyCnt++;

        }

    }

    return cpyCnt;
}


QString DataManage::GetFacePhotoExportToPath()
{
    //
    QString fileName("FaceImages");
    std::vector<QString> uDirs =  GetUDiskPaths();

    if(! uDirs.empty())
    {
        QDir targetDir(uDirs[0]);
        if(targetDir.mkpath(fileName))
        {
            return targetDir.filePath(fileName);
        }
    }


    return QString("");
}

QString DataManage::GetLogsExportToPath()
{

    //
    QString fileName("log");
    std::vector<QString> uDirs =  GetUDiskPaths();

    if(! uDirs.empty())
    {
        QDir targetDir(uDirs[0]);
        if(targetDir.mkpath(fileName))
        {
            return targetDir.filePath(fileName);
        }
    }


    return QString("");
}

void DataManage::on_btn_errReport_clicked(bool checked)
{

    isErroReportChecked = checked;
    slotExportOnlyItemSelectChanged();
}

QString DataManage::GetlogName(int index)
{

    QString logname = "message.log";
    int year = QDate::currentDate().year();
    int month = QDate::currentDate().month();
    int day  =  QDate::currentDate().day();
    QString smnth;
    QString sday;

    if(day - index>0)
    {
        day -= index;
    }
    else
    {
        day = day+31-index;
        month -- ;
        if(month<0)
        {
            year --;
            month =12;
        }

    }
    if(month<10)
    {
        smnth = "0"+QString::number(month);
    }
    if(day<10)
    {
        sday = "0"+QString::number(day);

    }
    logname = logname +"."+ QString::number(year)+"-"+smnth+"-"+sday;
    qDebug()<<"index = "<<index<<"name"<<logname<<endl;
    return logname;
}

bool DataManage::CheckUsbExit()
{
    char* cmd = "echo '123456' | sudo fdisk -l";
    // char  out[2048*1024] = "0";
    char out[1024*1024];
    exec_cmd(cmd,out);
    QString result = QString(QLatin1String(out));

    int strCount;
    int resultIndex = 0;
    while(resultIndex >= 0)
    {
        resultIndex =  result.indexOf("Disk identifier:",resultIndex);
        if(resultIndex <0)
        {
            break;
        }
        resultIndex ++;
        strCount++;
    }

    bool isUsbExit = (strCount>1);
    return isUsbExit;
//    if(strCount <= 1 )
//    {
//       return false;
//    }
//    else
//    {
//        //std::cout<<"***********"<<out<<"**********"<<std::endl;
//        return  true;
//    }
}


void DataManage::on_box_personInfo_clicked(bool checked)
{
    isPersonInfoChecked = checked;
    ui->box_irisData->setEnabled(checked);
    if(!checked)
    {
        ui->box_irisData->setChecked(checked);
    }

}

void DataManage::on_box_irisData_clicked(bool checked)
{

    isIrisChecked = checked;
    slotExportOnlyItemSelectChanged();
}

void DataManage::on_box_identRec_clicked(bool checked)
{
    isRecordChedked = checked;
    slotExportOnlyItemSelectChanged();
}

void DataManage::on_box_facePhoto_clicked(bool checked)
{
    isFacePhotoChedked = checked;
    slotExportOnlyItemSelectChanged();
}
