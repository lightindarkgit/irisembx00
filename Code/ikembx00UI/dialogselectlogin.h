#ifndef DIALOGSELECTLOGIN_H
#define DIALOGSELECTLOGIN_H

#include <QDialog>

namespace Ui {
class DialogSelectLogin;
}

class DialogSelectLogin : public QDialog
{
    Q_OBJECT
    
public:
    explicit DialogSelectLogin(QWidget *parent = 0);
    ~DialogSelectLogin();
    int result() const;
    void setResult(int r);

    enum DialogResult { Cancel =0, Iris, Account };
private slots:
    void timerEvent(QTimerEvent *);
    void selectAccountLogin();
    void selectIrisLogin();

private:
    Ui::DialogSelectLogin *ui;
    int _countdown;
    DialogResult _result;
};

#endif // DIALOGSELECTLOGIN_H
