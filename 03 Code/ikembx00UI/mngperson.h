#ifndef MNGPERSON_H
#define MNGPERSON_H

#include <QWidget>

namespace Ui {
class MngPerson;
}

class MngPerson : public QWidget
{
    Q_OBJECT

public:
    explicit MngPerson(QWidget *parent = 0);
    ~MngPerson();

protected:
    void InitDataTable(QWidget *parent = 0);
protected slots:
    void slotSearchPersonInfo();
    void slotEnrollIris();
    void slotAddEditPersonInfo();    //增加或修改人员信息
    void slotDeletePersonInfo();

private:
    Ui::MngPerson *_ui;
};

#endif // MNGPERSON_H
