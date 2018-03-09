#ifndef ADDPERSON_H
#define ADDPERSON_H

#include <QDialog>

namespace Ui {
class AddPerson;
}

class Enroll;

class AddPerson : public QDialog
{
    Q_OBJECT

public:
    explicit AddPerson(QWidget *parent = 0);
    ~AddPerson();

protected slots:
    void slotAddCard();
    void slotDeletCard();

    void slotEnrollIris();
    void slotDeleteIris();

    void slotSave();
    //void slotClear();
    void slotCancel();
    void slotGetEnrollResult(bool sigEnrollResult);

private:
    Ui::AddPerson *ui;
    Enroll *_enroll;

};

#endif // ADDPERSON_H
