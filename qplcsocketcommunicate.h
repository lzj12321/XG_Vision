#ifndef QPLCSOCKETCOMMUNICATE_H
#define QPLCSOCKETCOMMUNICATE_H
#include"socketcommunicate.h"
#include<QByteArray>


class QPlcSocketCommunicate:public SocketCommunicate
{
    Q_OBJECT
public:
    QPlcSocketCommunicate();
    virtual ~QPlcSocketCommunicate();
    bool sendData(STR_Vision_Result&resultData);
private:
    QByteArray writeDataIntialize(const quint16 regAddr, const quint16 regNum, const int data,const char registerFlag);
    QByteArray readDataIntialize(const quint16 regAddr, const quint16 regNum,QByteArray& result,const char registerFlag);
    bool readData(int&data,const int plcRegister,const char plcRegisterFlag=0x44);
    bool sendData(const int message, const int plcRegister,const char plcRegisterFlag=0x44);
    void readDataFromPlc(const int readNum,QByteArray& result);
    void writeDataToPlc(const QByteArray data);
    void readOneRegFromPLC(const quint16 regAddr,const  quint16 regNum,QByteArray& result,const  char bit30);
    void writeOneRegToPLC(const quint16 regAddr, const quint16 regNum, const int data, const char bit30);
private:
    int writeDataError=0;
    int readResponseError=0;
    int waitForReadAndWriteTime=2000;
};

#endif // QPLCSOCKETCOMMUNICATE_H
