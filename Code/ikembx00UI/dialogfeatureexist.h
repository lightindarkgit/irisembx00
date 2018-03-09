/*****************************************************************************
** 文 件 名：dialogfeatureexist.h
** 主 要 类：DialogFeatureExist
**
** Copyright (c) 中科虹霸有限公司
**
** 创 建 人：lizhl
** 创建时间：2014-02-26
**
** 修 改 人：
** 修改时间：
** 描  述:   注册查重后，如果注册特征与已有特征相似，将弹出特征存在对话框，由用户选择处理方式
** 主要模块说明: 特征存在类
**
** 版  本:   1.0.0
** 备  注:
**
*****************************************************************************/
#ifndef DIALOGFEATUREEXIST_H
#define DIALOGFEATUREEXIST_H

#include <QDialog>

//特征存在对话框返回类型
enum TypeOfFeatureExistReturn
{
    QuitFeature,    //不保存特征
    CoverFeature,   //本次注册特征覆盖已有的类似特征
    AddFeature      //已有特征不变，增加本次注册特征
};

namespace Ui {
class DialogFeatureExist;
}

class DialogFeatureExist : public QDialog
{
    Q_OBJECT
    
public:
    explicit DialogFeatureExist(QWidget *parent = 0);
    ~DialogFeatureExist();

    //注册界面将相似特征的索引、对应用户名传递给特征存在对话框
    void XferExistedPersonInfo(std::string leftDepart, std::string leftName, std::string rightDepart, std::string rightName,int personexist = 0);
    
private:
    Ui::DialogFeatureExist *ui;

private slots:
    void on_btnReturn_clicked();
    void on_btnAddFeature_clicked();
    void on_btnCoverFeature_clicked();

public:
    TypeOfFeatureExistReturn returnType;
};

#endif // DIALOGFEATUREEXIST_H
