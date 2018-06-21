#ifndef FXPLCSOCKETCOMMUNICATE_H
#define FXPLCSOCKETCOMMUNICATE_H
#include"socketcommunicate.h"
#include<QByteArray>


class FxPlcSocketCommunicate:public SocketCommunicate
{
    Q_OBJECT
public:
    FxPlcSocketCommunicate();
    virtual ~FxPlcSocketCommunicate();
    bool sendData(STR_Vision_Result&resultData);
private:
    bool readData(int&data,const int plcRegister,const char plcRegisterFlag=0x44);
    bool sendData(const int message, const int plcRegister,const char plcRegisterFlag=0x44);
    QByteArray writeDataIntialize(const quint16 regAddr,const  quint16 regNum,const  QByteArray data);
    QByteArray readDataIntialize(const quint16 regAddr, const quint16 regNum,const quint8 readNum);
    QByteArray readDataFromPlc(const int readNum);
    void writeDataToPlc(const QByteArray data);
    void WriteHoldingReg(const quint16 regAddr, const quint16 regNum, const QByteArray data);
    void ReadHoldingReg(const int regAddr, const quint16 regNum, QByteArray& result);
    void WriteInt16ToPLC(const quint16 regAddr, const qint16 data);
    QByteArray ReadInt16PlcFromMemory(const quint16 regAddr);
private:
    int waitForReadAndWriteTime=2000;
};

#endif // FXPLCSOCKETCOMMUNICATE_H
