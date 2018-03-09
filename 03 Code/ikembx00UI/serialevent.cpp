#include "serialevent.h"

SerialEvent::SerialEvent()
{
}


void SerialEvent::onGetICCardNum(QString cardNum)
{
    emit GetICCrad(cardNum);
}
