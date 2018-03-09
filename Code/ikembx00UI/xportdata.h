#ifndef XPORTDATA_H
#define XPORTDATA_H

#include <QWidget>

namespace Ui {
class XportData;
}

class XportData : public QWidget
{
    Q_OBJECT
    
public:
    explicit XportData(QWidget *parent = 0);
    ~XportData();
    
private:
    Ui::XportData *_ui;
};

#endif // XPORTDATA_H
