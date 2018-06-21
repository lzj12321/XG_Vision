#ifndef SOCKETCOMMUNICATE_H
#define SOCKETCOMMUNICATE_H
#include<QtNetwork/QtNetwork>
#include<QDebug>
#include<vision.h>

class SocketCommunicate:public QObject
{
    Q_OBJECT
public:
    SocketCommunicate();
    virtual ~SocketCommunicate();
    bool connectServer(const QHostAddress ip,const uint port);
    void disConnectServer();
    virtual bool sendData(STR_Vision_Result&resultData)=0;
    bool getSocketConnectStatus();
protected:
    QHostAddress Ip;
    uint Port;
    QTcpSocket client;
    bool connectStatus=false;
    int writeDataError;
    int readResponseError;
signals:
    void signal_socketStateChanged(QAbstractSocket::SocketState);
    void signal_VisionRun(int);
    void signal_VisionRun(int,int);
public slots:
    void slot_socketStateChanged(QAbstractSocket::SocketState socketState);
};

#endif // SOCKETCOMMUNICATE_H
