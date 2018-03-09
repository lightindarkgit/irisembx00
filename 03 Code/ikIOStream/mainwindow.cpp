#include "mainwindow.h"
#include "ui_mainwindow.h"


MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    Person pInfo[] = {{"sdsd1","的速度","0","速度速度","ewew","ghjghjg",233,"ggggg","2013-12-04 50:26:52:456"}
                     ,{"sdsd2","的速度","0","速度速度","ewew","ghjghjg",233,"ggggg","2013-12-05 50:26:52:456"}
                     ,{"sdsd3","的速度","0","速度速度","ewew","ghjghjg",233,"ggggg","2013-12-06 50:26:52:456"}
                     ,{"sdsd4","的速度","0","速度速度","ewew","ghjghjg",233,"ggggg","2013-12-07 50:26:52:456"}
                     ,{"sdsd5","的速度","2","速度速度","ewew","ghjghjg",233,"ggggg","2013-12-08 50:26:52:456"}
                     ,{"sdsd6","的速度","2","速度速度","ewew","ghjghjg",233,"ggggg","2013-12-09 50:26:52:456"}
                     ,{"sdsd7","的速度","1","速度速度","ewew","ghjghjg",233,"ggggg","2013-12-01 50:26:52:456"}
                     ,{"sdsd8","的速度","1","速度速度","ewew","ghjghjg",233,"ggggg","2013-12-02 50:26:52:456"}
                     ,{"sdsd9","的速度","1","速度速度","ewew","ghjghjg",233,"ggggg","2013-12-03 50:26:52:456"}};

//    char imageID[16];
//    char personID[16];
//    int  personImageLength;
//    char memo[128];
//    char * personImage;
    PersonImage pImage[] ={{"sd1","1fdgsdg",12,"sdfgsdfuytd","sddddfgfffff"},
                          {"sdg2","2fdgsdg",15,"sdfgsdftyttttt","aaasddddfgfffff"},
                          {"sdgsf3","3fdgsdg",10,"sdfgsdfht","dddfgfffff"},
                          {"sdgsf4","4fdgsdg",12,"sdfgsdftyrf","sddddfgfffff"}};
    for(int index =0;index <9;index++)
    {
        _personInfoList.push_back(pInfo[index]);
    }

    for(int index =0;index <4;index++)
    {
        _personImageList.push_back(pImage[index]);
    }


}

MainWindow::~MainWindow()
{
    delete ui;
}
void MainWindow::exportPerson()
{
    IkIOStream ikiostream;

//    ikiostream.ExportToFile(ui->label->text().toStdString().data(),_personList);
   // ikiostream.ExportToFile(ui->label->text().toStdString().data(),_personInfoList);//"personInfo.txt"
   ikiostream.ExportToFile(ui->label->text().toStdString().data(),_personImageList);
    //ikiostream.ExportToFileBySQLite("./person.db",_personList);
    //_ikIOStream.ExportToFile(ui->label->text(),_personList);
}

void MainWindow::importPerson()
{
    //_ikIOStream.ExportToFile(ui->label->text(),_personList);
    IkIOStream ikio;
    //_personList.clear();
//    ikio.ImportFromFileBySQLite("./person.db",_personList);
//    for (int index =0; index <_personList.size();index++)
//    {
//        std::cout<<_personList[index].personID<<" "<<_personList[index].name<<" "<<std::endl;
//    }

//    std::vector<Person> personList;
//    ikio.ImportFromFile(ui->label->text().toStdString().data(),personList);//"personInfo.txt"

//    for (int index =0; index <personList.size();index++)
//    {
//        std::cout<<personList[index].personID<<" "<<personList[index].name<<" "<<std::endl;
//    }


    std::vector<PersonImage> personList;
    ikio.ImportFromFile(ui->label->text().toStdString().data(),personList);//"personInfo.txt"

    for (int index =0; index <personList.size();index++)
    {
        std::cout<<personList[index].personID<<" "<<personList[index].imageID<<" "<<std::endl;
    }
}
