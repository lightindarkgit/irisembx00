#ifndef DEVCIECONTROL_H
#define DEVCIECONTROL_H

#include <QWidget>

namespace Ui {
class DevcieControl;
}

class DevcieControl : public QWidget
{
    Q_OBJECT
    
public:
    explicit DevcieControl(QWidget *parent = 0);
    ~DevcieControl();
    
private:
    Ui::DevcieControl *_ui;
};

#endif // DEVCIECONTROL_H
