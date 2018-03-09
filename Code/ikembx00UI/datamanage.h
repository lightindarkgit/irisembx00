/*****************************************************************************
** 文 件 名：datamanage.h
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
#ifndef DATAMANAGE_H
#define DATAMANAGE_H

#include <QDialog>
//数据导入导出接口
#include "../ikIOStream/ikiostream.h"
#include "../ikIOStream/interface.h"


namespace Ui {
class DataManage;
}

class DataManage : public QDialog
{
    Q_OBJECT
public:
    enum{fileNotExist = 2, fileUnreadable = 3, fileUnWritable};

public:
    explicit DataManage(QWidget *parent = 0);
    ~DataManage();

    //页面刷新
    void refresh();

protected:
    //初始化
    bool initUI();
    //获取导入导出选项
    bool getCheckedItems();

    //是否已指定导入/出对象
    bool IsAtLeastOneItemSelected();

    //Get the earliest time of all face images
    QString GetTheEarliestTimeOfFaceImgs();


    //Export faces images
    bool ExportFaceImgs(QString begTime, QString endTime);

    /// Export log files
    bool ExportErroLogs(QString begTime,QString endTime);



    //数据导入线程
    static void* importData(void* arg);
    //数据导出线程
    static void* exportData(void* arg);

    static QString GetTableName(TableType type);
signals:
    //TO DO
    void sigGoBack();
    void sigSuccess(bool);
    void sigError(bool);
    void sigAbort();
    void sigCloseBufferingDlg();

protected slots:
    void slotPersonInfoSelectChanged();
    void slotExportOnlyItemSelectChanged();
    void slotImport();
    void slotExport();
    void slotShowRes();
    
    //屏蔽Esc键信号
    void reject();
private slots:
    void on_btnPersonInfo_clicked();

    void on_btnIris_clicked();

    void on_btnRecord_clicked();

    void on_btnFacePho_clicked();


    void on_btn_errReport_clicked(bool checked);

private:
    bool getDBUpgradeFile();
    QString GetTips();

    //获取人脸图像导出路径名称
    QString GetFacePhotoExportToPath();
    QString GetLogsExportToPath();

    int ExportBackupedFaceImages();
    int ExportUnbackupedFaceImages();
    //拷贝指定目录下的文件到指定的目录中
    int CopyDirectory(const QString& sPath, const QString& dPath);
    int CopyDirectory(const QString& sPath, const QString& dPath,int count);
    QString GetlogName(int index);

private:
    Ui::DataManage *ui;
    std::vector<TableType> tables;
    QString res;
public:
    bool    isPersonInfoChecked ;
    bool    isIrisChecked       ;
    bool    isRecordChedked     ;
    bool    isFacePhotoChedked  ;
    bool    isErroReportChecked ;
protected:
    bool    isudiskConnect;

};



class MountUsb
{
public:
      MountUsb();
      ~MountUsb();
};

#endif // DATAMANAGE_H
