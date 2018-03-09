/******************************************************************************************
** 文件名:   addalarm.h
×× 主要类:   (无)
**
** Copyright (c) 中科虹霸有限公司
** 创建人:   Wang.L
** 日  期:   2015-01-28
** 修改人:
** 日  期:
** 描  述:   闹铃编辑模块声明
** ×× 主要模块说明：
**
** 版  本:   1.0.0
** 备  注:
**
******************************************************************************************/
#ifndef ADDALARM_H
#define ADDALARM_H

#include <QDialog>
#include "../XML/xml/IKXmlApi.h"

namespace Ui {
class AddAlarm;
}

class AddAlarm : public QDialog
{
    Q_OBJECT
    
public:
    explicit AddAlarm(QWidget *parent = 0);
    AddAlarm(const QString& alarmer, const QString& curAlarmTag, QWidget *parent = 0);

    ~AddAlarm();

protected:
    void init();
    void init(const QString& time);
    QString getAlarmTag(const IKXml::IKXmlDocument& cfgFile);
    bool saveAlarmInfo(const QString& time, const QString& alarmTag, const IKXml::IKXmlDocument& cfgFile);

protected slots:
    bool slotBack();
    bool slotDelete();
    bool slotSave();
    void slotAdd();
    void slotMinus();
    
private:
    Ui::AddAlarm *ui;
    QString alarmTag;
};

#endif // ADDALARM_H
