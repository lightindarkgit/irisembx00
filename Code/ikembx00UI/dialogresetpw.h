#ifndef DIALOGRESETPW_H
#define DIALOGRESETPW_H

#include <QWidget>
#include <QDialog>

extern QString _GLO_pnCode;
extern QString _GLO_CodeDate;


struct PNCODE
{
    QString pncode;
    QString codedate;
};

namespace Ui {
class dialogResetPW;
}

class dialogResetPW : public QDialog
{
    Q_OBJECT
    
public:
    explicit dialogResetPW(QWidget *parent = 0);
    ~dialogResetPW();
    
private slots:


    void on_btn_PNcode_clicked();

    void on_btnResetPW_clicked();

    void on_btnCancel_clicked();

private:
    Ui::dialogResetPW *ui;
    void init();
private:
    QString m_pncode;
    QString m_inputPnCode ;
public:
    int m_returnType;
   // PNCODE getPncode();
};

#endif // DIALOGRESETPW_H
