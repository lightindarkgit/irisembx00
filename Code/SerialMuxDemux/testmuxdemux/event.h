#ifndef EVENT_H
#define EVENT_H
#include <string>
#include <QObject>

class MyEvent : public QObject
{
    Q_OBJECT
public:
    MyEvent();
    ~MyEvent();

    void onRecvData(int val);
    void onSendData(int val);
    void onRecvCard(QString s);

signals:
    void RecvData(int);
    void SendData(int);
    void ReadCard(QString);

};

#endif // EVENT_H
