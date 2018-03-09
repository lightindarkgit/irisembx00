#pragma once
#include <QObject>

class SerialEvent : public QObject
{
    Q_OBJECT
public:
    SerialEvent();
    void onGetICCardNum(QString);

signals:
    void GetICCrad(QString);
};
