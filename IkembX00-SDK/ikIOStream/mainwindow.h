#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
//#include "ikiostream.h"
#include "interface.h"
#include <vector>
#include "thread"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT
    
public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
private:
    static void * DealDatagThread1(void * arg);
private slots:
    void exportPerson();
    void importPerson();
    
private:
    Ui::MainWindow *ui;
    std::thread _thread;
    //IkIOStream _ikIOStream;
    //std::vector<Person> _personList;
    std::vector<Person> _personInfoList;
    std::vector<PersonImage> _personImageList;
};

#endif // MAINWINDOW_H
