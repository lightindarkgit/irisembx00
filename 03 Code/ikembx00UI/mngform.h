#ifndef MNGFORM_H
#define MNGFORM_H

#include <QWidget>

class QToolButton;
class QTableView;

namespace Ui {
class MngForm;
}

class MngForm : public QWidget
{
    Q_OBJECT

public:
    explicit MngForm(QWidget *parent = 0);
    ~MngForm();

protected:
    void InitDataTable(QWidget *parent = 0);

protected slots:
    void slotPersonMng();
    //void slotDepartMng();
    void slotXportOpt();
    void slotDevCtrlOpt();
    void slotDevCfg();

private:
    Ui::MngForm *_ui;
    QToolButton *_personMng;
    //QToolButton *_departMng;
    QToolButton *_xportOpt;
    QToolButton *_devCtrlOpt;
    QToolButton *_devCfg;

    QWidget *_pDispWidget;
};

#endif // MNGFORM_H
