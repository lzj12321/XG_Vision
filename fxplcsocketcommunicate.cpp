#include "fxplcsocketcommunicate.h"

FxPlcSocketCommunicate::FxPlcSocketCommunicate()
{

}

FxPlcSocketCommunicate::~FxPlcSocketCommunicate()
{

}

bool FxPlcSocketCommunicate::readData(int&data,const int plcRegister, const char plcRegisterFlag)
{
    bool transferResultFlag;
    int intReadResult;
    QByteArray bytearrayReadResult=ReadInt16PlcFromMemory(plcRegister);
    qDebug()<<"data:"<<bytearrayReadResult.toInt();
    QString str=(QString)bytearrayReadResult.toStdString().data();
    intReadResult=bytearrayReadResult.toInt(&transferResultFlag,16);
    qDebug()<<"read data:"<<str<<endl;
    data=str.toInt();
    return true;
}

bool FxPlcSocketCommunicate::sendData(const int message, const int plcRegister, const char plcRegisterFlag)
{
    qint16 int16Msg=(qint16)message;
    WriteInt16ToPLC(plcRegister, int16Msg);
    return true;
}

bool FxPlcSocketCommunicate::sendData(STR_Vision_Result &resultData)
{
    if(resultData.x_flag){
        sendData(resultData.x,resultData.x_addr);
    }
    if(resultData.y_flag){
        sendData(resultData.y,resultData.y_addr);
    }
    if(resultData.angle_flag){
        sendData(resultData.angle,resultData.angle_addr);
    }
    if(resultData.model_flag){
        sendData(resultData.model,resultData.model_addr);
    }
    if(resultData.errorFlag_flag){
        sendData(resultData.errorFlag,resultData.errorFlag_addr);
    }
    return true;
}

QByteArray FxPlcSocketCommunicate::writeDataIntialize(const quint16 regAddr,const  quint16 regNum,const  QByteArray data)
{
    QByteArray frame;
    if ((regNum * 2) != data.length())
    {
        writeDataError = 104;
       // emit signal_readOrWriteError(writeDataError);
        return frame;
    }
    frame.resize(regNum * 2 + 12);
    frame[0] = 0x03;
    frame[1] = 0xFF;
    frame[2] = 0x0A;
    frame[3] = 0x00;
    frame[4] =(uchar)regAddr;
    frame[5] =(uchar)regAddr>>8;
    frame[6] =(uchar)regAddr>>16;
    frame[7] =(uchar)regAddr>>24;
    frame[8] = 0x20;
    frame[9] = 0x44;
    frame[10]=(uchar)regNum;
    frame[11] = 0x00;

    for (int i = 0; i < data.length(); i++)  //将要发送的数据添加到发送缓冲区
    {
        frame[12 + i] = data[i];
    }
    return frame;
}

QByteArray FxPlcSocketCommunicate::readDataIntialize(const quint16 regAddr, const quint16 regNum,const quint8 readNum)
{
    QByteArray frame;
    if ((regNum * 2) != readNum)
    {
        writeDataError = 104;
       // emit signal_readOrWriteError(writeDataError);
        return frame;
    }

    frame.resize(12);
    frame[0] = 0x01;
    frame[1] = 0xFF;
    frame[2] = 0x0A;
    frame[3] = 0x00;


    frame[4] =(uchar)regAddr;
    frame[5] =(uchar)regAddr>>8;
    frame[6] =(uchar)regAddr>>16;
    frame[7] =(uchar)regAddr>>24;

    frame[8] = 0x20;
    frame[9] = 0x44;

    frame[10]=(uchar)regNum;
    frame[11] = 0x00;

    return frame;
}

QByteArray FxPlcSocketCommunicate::readDataFromPlc(const int readNum)
{
    QByteArray buffer;
    buffer.resize(readNum);
    try
    {
        if(connectStatus)
        {
            int readResultFlag;
            if(client.waitForReadyRead(waitForReadAndWriteTime))
                readResultFlag=client.read(buffer.data(),buffer.length());
            if(readResultFlag==-1)
            {
                readResponseError=105;
               // emit signal_readOrWriteError(readResponseError);
                return buffer;
            }
            return buffer;
        }
        else
        {
            readResponseError=103;
           // emit signal_readOrWriteError(readResponseError);
            return buffer;
        }
    }
    catch (QException& e)
    {
        readResponseError = 101;
        //emit signal_readOrWriteError(readResponseError);
        qDebug()<<e.what()<<endl;
        return buffer;
    }
}

void FxPlcSocketCommunicate::writeDataToPlc(const QByteArray writeData)
{
    try
    {
        int writeDataFlag;
        if(connectStatus)
        {
            writeDataFlag=client.write(writeData.data(),writeData.length());
            if(!client.waitForBytesWritten(waitForReadAndWriteTime))
            {
                writeDataError=102;
             //   emit signal_readOrWriteError(writeDataError);
                return;
            }
            if(writeDataFlag!=writeData.length())
            {
                writeDataError=106;
               // emit signal_readOrWriteError(writeDataError);
                return;
            }
        }
    }
    catch (QException& e)
    {
        writeDataError = 100;
      //  emit signal_readOrWriteError(writeDataError);
        qDebug()<<e.what()<<endl;
        return;
    }
}

void FxPlcSocketCommunicate::WriteHoldingReg(const quint16 regAddr,const  quint16 regNum,const  QByteArray data)
{
    writeDataError=0;
    readResponseError=0;
    QByteArray frame=writeDataIntialize(regAddr, regNum, data);
    if(frame.isEmpty())
        return;
    writeDataToPlc(frame);

    /****************************************************************************************/
    //读取mc主机的响应帧
    QByteArray buffer;
    buffer=readDataFromPlc(2);
}

void FxPlcSocketCommunicate::ReadHoldingReg(const int regAddr, const quint16 regNum, QByteArray& result)
{
    writeDataError=0;
    readResponseError=0;
    QByteArray frame=readDataIntialize(regAddr, regNum,result.length());
    if(frame.isEmpty())
        return;
    writeDataToPlc(frame);

    quint16 offset = (quint16)(2 + regNum * 2);
    QByteArray buffer;
    buffer.resize(offset);
    buffer=readDataFromPlc(offset);

    int j = 0;
    for (int i = 2; i < buffer.length(); i++)
    {
        result[j++] = buffer[i];
    }
}

void FxPlcSocketCommunicate::WriteInt16ToPLC(const quint16 regAddr,const  qint16 data)
{
    QByteArray fval;
    fval.resize(2);
    fval[0]=(uchar)data;
    fval[1]=(uchar)data>>8;
    WriteHoldingReg(regAddr, 1, fval);
}

QByteArray FxPlcSocketCommunicate::ReadInt16PlcFromMemory(const quint16 regAddr)
{
    QByteArray result;
    result.resize(2);
    ReadHoldingReg(regAddr, 1, result);
    return result;
}
