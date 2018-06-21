#ifndef STANDARDSOCKETCOMMUNICATE_H
#define STANDARDSOCKETCOMMUNICATE_H
#include"socketcommunicate.h"
#include<QByteArray>

class StandardSocketCommunicate:public SocketCommunicate
{
    Q_OBJECT
public:
    StandardSocketCommunicate();
    virtual ~StandardSocketCommunicate();
    bool sendData(STR_Vision_Result&resultData);
private slots:
    void slot_readSignal();
};

#endif // STANDARDSOCKETCOMMUNICATE_H
