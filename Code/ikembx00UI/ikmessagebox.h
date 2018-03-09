#ifndef IKMESSAGEBOX_H
#define IKMESSAGEBOX_H

#include <QDialog>

namespace Ui {
class IKMessageBox;
}

enum MessageBoxType{informationMbx, warningMbx, errorMbx, questionMbx};

class IKMessageBox : public QDialog
{
    Q_OBJECT

public:
    IKMessageBox(QWidget *parent, const QString& title, const QString& info, MessageBoxType type,bool showRadio = false);
    ~IKMessageBox();
    void SetOnlineMode();
    void SetOfflineMode();


protected:
    void setIcon(MessageBoxType type);
    //void paintEvent(QPaintEvent *);

private:
    void dispIcon(const QString& iconFile);

    void setButtons(MessageBoxType type);
private slots:
    void Accept();
    void Reject();

private:
    Ui::IKMessageBox *ui;
public:
    bool isOnlineMode;
};

#endif // IKMESSAGEBOX_H
