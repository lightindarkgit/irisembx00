#ifndef SIMPLEPERSONINFO_H
#define SIMPLEPERSONINFO_H

#include <QWidget>

namespace Ui {
class SimplePersonInfo;
}

class SimplePersonInfo : public QWidget
{
    Q_OBJECT
    
public:
    explicit SimplePersonInfo(QWidget *parent = 0);
    ~SimplePersonInfo();
    
private:
    Ui::SimplePersonInfo *ui;
};

#endif // SIMPLEPERSONINFO_H
