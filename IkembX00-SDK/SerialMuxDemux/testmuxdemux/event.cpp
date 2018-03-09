#include "event.h"

MyEvent::MyEvent()
{
}

MyEvent::~MyEvent()
{
}

void MyEvent::onRecvData(int val)
{
    emit RecvData(val);
}


void MyEvent::onSendData(int val)
{
    emit SendData(val);
}

void MyEvent::onRecvCard(QString s)
{
    emit ReadCard(s);
}
