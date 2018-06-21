#include "socketcommunicate.h"

SocketCommunicate::SocketCommunicate()
{
    connect(&client,SIGNAL(stateChanged(QAbstractSocket::SocketState)),this,SLOT(slot_socketStateChanged(QAbstractSocket::SocketState)));
}

SocketCommunicate::~SocketCommunicate()
{
    if(connectStatus)
        client.disconnectFromHost();
}

bool SocketCommunicate::connectServer(const QHostAddress ip,const uint port)
{
    try
    {
        Ip=ip;
        Port=port;
        client.connectToHost(ip, port);
        return client.waitForConnected(500);
    }
    catch (QException& exception)
    {
        connectStatus=false;
        qDebug()<<"connect error!"<<exception.what()<<endl;
        return connectStatus;
    }
}

void SocketCommunicate::disConnectServer()
{
    client.disconnectFromHost();
}

bool SocketCommunicate::getSocketConnectStatus()
{
    return connectStatus;
}

void SocketCommunicate::slot_socketStateChanged(QAbstractSocket::SocketState socketState)
{
    switch(socketState)
    {
    case QAbstractSocket::UnconnectedState:
    {
        connectStatus=false;
        emit signal_socketStateChanged(QAbstractSocket::UnconnectedState);
    }break;
        //0 The socket is not connected.
    case QAbstractSocket::HostLookupState:
    {
        emit signal_socketStateChanged(QAbstractSocket::HostLookupState);
    }break;
        //1 The socket is performing a host name lookup.
    case QAbstractSocket::ConnectingState:
    {
        emit signal_socketStateChanged(QAbstractSocket::ConnectingState);
    }break;
        //2 The socket has started establishing a connection.
    case QAbstractSocket::ConnectedState:
    {
        connectStatus=true;
        emit signal_socketStateChanged(QAbstractSocket::ConnectedState);
    }break;
        //3 A connection is established.
    case QAbstractSocket::BoundState:
    {
        emit signal_socketStateChanged(QAbstractSocket::BoundState);
    }break;
        //4 The socket is bound to an address and port.
    case QAbstractSocket::ClosingState:
    {
        emit signal_socketStateChanged(QAbstractSocket::ClosingState);
    }break;
        //6 The socket is about to close (data may still be waiting to be written).
    case QAbstractSocket::ListeningState:
    {
        emit signal_socketStateChanged(QAbstractSocket::ListeningState);
    }break;
        //5 For internal use only.
    }
}
