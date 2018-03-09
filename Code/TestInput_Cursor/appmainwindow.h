#ifndef APPMAINWINDOW_H
#define APPMAINWINDOW_H

#include <QMainWindow>

namespace Ui {
class AppMainWindow;
}

class AppMainWindow : public QMainWindow
{
    Q_OBJECT
    
public:
    explicit AppMainWindow(QWidget *parent = 0);
    ~AppMainWindow();
    
private:
    Ui::AppMainWindow *ui;
};

#endif // APPMAINWINDOW_H
