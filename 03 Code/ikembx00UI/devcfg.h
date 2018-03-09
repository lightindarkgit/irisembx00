#ifndef DEVCFG_H
#define DEVCFG_H

#include <QWidget>

namespace Ui {
class DevCfg;
}

class DevCfg : public QWidget
{
    Q_OBJECT
    
public:
    explicit DevCfg(QWidget *parent = 0);
    ~DevCfg();
protected slots:
    void slotSave();
    void slotCancel();

private:
    Ui::DevCfg *_ui;
};

#endif // DEVCFG_H
