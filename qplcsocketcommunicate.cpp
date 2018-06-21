#include "qplcsocketcommunicate.h"

QPlcSocketCommunicate::QPlcSocketCommunicate()
{

}

QPlcSocketCommunicate::~QPlcSocketCommunicate()
{

}

bool QPlcSocketCommunicate::readData(int&data,const int plcRegister, const char plcRegisterFlag)
{
    QByteArray result;
    result.resize(4);
    quint16 readResult;
    for (int i = 0; i < 4; i++)
    {
        result[i] = 0;
    }
    readOneRegFromPLC(plcRegister, 1, result, plcRegisterFlag);
    bool transferResultFlag;
    readResult=result.toInt(&transferResultFlag,16);
    data=readResult;
    return true;
}

bool QPlcSocketCommunicate::sendData(const int message, const int plcRegister, const char plcRegisterFlag)
{
    writeOneRegToPLC(plcRegister,1, message, plcRegisterFlag);
    return true;
}

bool QPlcSocketCommunicate::sendData(STR_Vision_Result &resultData)
{
    if(resultData.x_flag&&resultData.x_addr!=-1){
        qDebug()<<"x_flag:"<<resultData.x_flag;
        sendData(resultData.x,resultData.x_addr);
    }
    if(resultData.y_flag&&resultData.y_addr!=-1){
        qDebug()<<"y_flag:"<<resultData.y_flag;
        sendData(resultData.y,resultData.y_addr);
    }
    if(resultData.angle_flag&&resultData.angle_addr!=-1){
        qDebug()<<"angle_flag:"<<resultData.angle_flag;
        sendData(resultData.angle,resultData.angle_addr);
    }
    if(resultData.model_flag&&resultData.model_addr!=-1){
        qDebug()<<"model_flag:"<<resultData.model_flag;
        sendData(resultData.model,resultData.model_addr);
    }
    if(resultData.errorFlag_flag&&resultData.errorFlag_addr!=-1){
        qDebug()<<"errorFlag_flag:"<<resultData.errorFlag_flag;
        sendData(resultData.errorFlag,resultData.errorFlag_addr);
    }
    return true;
}

QByteArray QPlcSocketCommunicate::writeDataIntialize(const quint16 regAddr, const quint16 regNum, const int data, const char registerFlag)
{
    writeDataError = 0;
    readResponseError=0;

    QByteArray frame;
    QByteArray res;
    res.resize(4);
    res[0] = 0x30;
    res[1] = 0x30;
    res[2] = 0x30;
    res[3] = 0x30;
    QString strData=QString("%1").arg(data,4,16,QLatin1Char('0'));
    QByteArray byteArrayData=strData.toLatin1();
    if (byteArrayData.length() == 1)
    {
        res[3] =byteArrayData[0];
    }
    else if (byteArrayData.length() == 2)
    {
        res[2] = byteArrayData[0];
        res[3] = byteArrayData[1];
    }
    else if (byteArrayData.length() == 3)
    {
        res[1] = byteArrayData[0];
        res[2] = byteArrayData[1];
        res[3] = byteArrayData[2];
    }
    else if (byteArrayData.length() == 4)
    {
        res[0] = byteArrayData[0];
        res[1] = byteArrayData[1];
        res[2] = byteArrayData[2];
        res[3] = byteArrayData[3];
    }

    if ((regNum * 4) != res.length())
    {
        writeDataError = 104;
        return frame;
    }
    frame.resize(42 + regNum * 4);
    frame[0] = 0x35;
    frame[1] = 0x30;
    frame[2] = 0x30;
    frame[3] = 0x30;
    frame[4] = 0x30;
    frame[5] = 0x30;
    frame[6] = 0x46;
    frame[7] = 0x46;
    frame[8] = 0x30;
    frame[9] = 0x33;
    frame[10] = 0x46;
    frame[11] = 0x46;
    frame[12] = 0x30;
    frame[13] = 0x30;

    int writeNumOfReg = 24 + regNum * 4;
    QString addstr=QString("%1").arg(writeNumOfReg,4,16,QLatin1Char('0'));
    QByteArray addchar=addstr.toLatin1();
    addchar=addchar.toUpper();
    frame[14] = addchar[0];
    frame[15] = addchar[1];
    frame[16] = addchar[2];
    frame[17] = addchar[3];

    frame[18] = 0x30;
    frame[19] = 0x30;
    frame[20] = 0x31;
    frame[21] = 0x30;
    frame[22] = 0x31;
    frame[23] = 0x34;
    frame[24] = 0x30;
    frame[25] = 0x31;
    frame[26] = 0x30;
    frame[27] = 0x30;
    frame[28] = 0x30;
    frame[29] = 0x30;
    frame[30] = registerFlag;
    frame[31] = 0x2a;

    frame[32] = 0x30;                //寄存器地址
    frame[33] = 0x30;
    frame[34] = 0x30;
    frame[35] = 0x30;
    frame[36] = 0x30;
    frame[37] = 0x30;
    addstr = "";
    if (registerFlag == 0x57)//W 87  0x57十六进制 字
    {
        addstr=QString::number(regAddr,16);
    }
    if (registerFlag == 0x44)//D 68  0x44十进制 字
    {
        addstr=QString::number(regAddr,10);
    }
    addchar=addstr.toLatin1();
    if (addchar.length() == 1)
    {
        frame[37] = addchar[0];
    }
    else if (addchar.length() == 2)
    {
        frame[36] = addchar[0];
        frame[37] = addchar[1];
    }
    else if (addchar.length() == 3)
    {
        frame[35] = addchar[0];
        frame[36] = addchar[1];
        frame[37] =addchar[2];
    }
    else if (addchar.length() == 4)
    {
        frame[34] = addchar[0];
        frame[35] =addchar[1];
        frame[36] = addchar[2];
        frame[37] =addchar[3];
    }
    else if (addchar.length() == 5)
    {
        frame[33] =addchar[0];
        frame[34] = addchar[1];
        frame[35] = addchar[2];
        frame[36] = addchar[3];
        frame[37] = addchar[4];
    }
    else if (addchar.length() == 6)
    {
        frame[32] = addchar[0];
        frame[33] = addchar[1];
        frame[34] = addchar[2];
        frame[35] =addchar[3];
        frame[36] = addchar[4];
        frame[37] = addchar[5];
    }

    frame[38] = 0x30;   //寄存器个数
    frame[39] = 0x30;
    frame[40] = 0x30;
    frame[41] = 0x30;
    addstr=QString("%1").arg(regNum,4,16,QLatin1Char('0'));
    addchar=addstr.toLatin1();
    if (addchar.length() == 1)
    {
        frame[41] = addchar[0];
    }
    else if (addchar.length() == 2)
    {
        frame[40] = addchar[0];
        frame[41] = addchar[1];
    }
    else if (addchar.length() == 3)
    {
        frame[39] =addchar[0];
        frame[40] =addchar[1];
        frame[41] = addchar[2];
    }
    else if (addchar.length() == 4)
    {
        frame[38] = addchar[0];
        frame[39] = addchar[1];
        frame[40] = addchar[2];
        frame[41] = addchar[3];
    }

    for (int i = 0; i < res.length(); i++)  //将要发送的数据添加到发送缓冲区
    {
        frame[42 + i] = res[i];
    }
    return frame;
}

QByteArray QPlcSocketCommunicate::readDataIntialize(const quint16 regAddr, const quint16 regNum, QByteArray& result, const char registerFlag)
{
    QByteArray frame;
    if ((regNum * 4) != result.length())
    {
        writeDataError = 104;
        return frame;
    }

    frame.resize(42);
    frame[0] = 0x35;
    frame[1] = 0x30;
    frame[2] = 0x30;
    frame[3] = 0x30;
    frame[4] = 0x30;
    frame[5] = 0x30;
    frame[6] = 0x46;
    frame[7] = 0x46;
    frame[8] = 0x30;
    frame[9] = 0x33;
    frame[10] = 0x46;
    frame[11] = 0x46;
    frame[12] = 0x30;
    frame[13] = 0x30;
    frame[14] = 0x30;
    frame[15] = 0x30;
    frame[16] = 0x31;
    frame[17] = 0x38;
    frame[18] = 0x30;
    frame[19] = 0x30;
    frame[20] = 0x31;
    frame[21] = 0x30;
    frame[22] = 0x30;
    frame[23] = 0x34;
    frame[24] = 0x30;
    frame[25] = 0x31;
    frame[26] = 0x30;
    frame[27] = 0x30;
    frame[28] = 0x30;
    frame[29] = 0x30;
    frame[30] = registerFlag;
    frame[31] = 0x2a;

    frame[32] = 0x30;                //寄存器地址
    frame[33] = 0x30;
    frame[34] = 0x30;
    frame[35] = 0x30;
    frame[36] = 0x30;
    frame[37] = 0x30;
    QString addstr = "";
    if (registerFlag == 0x57)//W 87  0x57十六进制 字
    {
        addstr=QString::number(regAddr,16);
    }
    if (registerFlag == 0x44)//D 68  0x44十进制 字
    {
        addstr=QString::number(regAddr,10);
    }

    QByteArray addchar=addstr.toLatin1();

    if (addchar.length() == 1)
    {
        frame[37] = addchar[0];
    }
    else if (addchar.length()== 2)
    {
        frame[36] = addchar[0];
        frame[37] = addchar[1];
    }
    else if (addchar.length()== 3)
    {
        frame[35] = addchar[0];
        frame[36] = addchar[1];
        frame[37] = addchar[2];
    }
    else if (addchar.length()== 4)
    {
        frame[34] = addchar[0];
        frame[35] = addchar[1];
        frame[36] = addchar[2];
        frame[37] = addchar[3];
    }
    else if (addchar.length()== 5)
    {
        frame[33] = addchar[0];
        frame[34] = addchar[1];
        frame[35] = addchar[2];
        frame[36] = addchar[3];
        frame[37] = addchar[4];
    }
    else if (addchar.length() == 6)
    {
        frame[32] = addchar[0];
        frame[33] = addchar[1];
        frame[34] = addchar[2];
        frame[35] = addchar[3];
        frame[36] = addchar[4];
        frame[37] = addchar[5];
    }

    frame[38] = 0x30;   //寄存器个数
    frame[39] = 0x30;
    frame[40] = 0x30;
    frame[41] = 0x30;
    addstr=QString("%1").arg(regNum,4,16,QLatin1Char('0'));
    addchar = addstr.toLatin1();
    if (addchar.length() == 1)
    {
        frame[41] = addchar[0];
    }
    else if (addchar.length() == 2)
    {
        frame[40] = addchar[0];
        frame[41] =addchar[1];
    }
    else if (addchar.length() == 3)
    {
        frame[39] =addchar[0];
        frame[40] =addchar[1];
        frame[41] = addchar[2];
    }
    else if (addchar.length() == 4)
    {
        frame[38] =addchar[0];
        frame[39] = addchar[1];
        frame[40] = addchar[2];
        frame[41] =addchar[3];
    }
    return frame;
}

void QPlcSocketCommunicate::readDataFromPlc(const int readNum,QByteArray& result)
{
    QByteArray buffer;
    buffer.resize(readNum);
    try
    {
        int readResultFlag;
        if(connectStatus)
        {
            if(client.waitForReadyRead(waitForReadAndWriteTime))
                readResultFlag=client.read(buffer.data(),readNum);
            if(readResultFlag==-1)
            {
                readResponseError=105;
                //emit signal_readOrWriteError(readResponseError);
                return ;
            }
            if ((qint16)buffer[18] != 48 || (qint16)buffer[19] != 48 || (qint16)buffer[20] != 48 ||(qint16)buffer[21] != 48)
            {
                readResponseError = 101;
                // emit signal_readOrWriteError(readResponseError);
                return ;
            }
            else
            {
                for(int j=0,i=22;i<buffer.length();i++,j++)
                {
                    result[j]=buffer[i];
                }
            }
        }
    }
    catch (QException& communicateException)
    {
        readResponseError = 101;
        qDebug()<<communicateException.what()<<endl;
        return;
    }
}

void QPlcSocketCommunicate::writeDataToPlc(const QByteArray data)
{
    try
    {
        if(connectStatus)
        {
            int writeDataFlag;
            writeDataFlag=client.write(data,data.length());
            if(!client.waitForBytesWritten(waitForReadAndWriteTime))
            {
                writeDataError = 102;
                return;
            }
            if(writeDataFlag!=data.length())
            {
                writeDataError = 106;
                return;
            }
        }
    }
    catch (QException& communicateException)
    {
        writeDataError = 100;
        //emit signal_readOrWriteError(writeDataError);
        qDebug()<<communicateException.what()<<endl;
        return;
    }
}

void QPlcSocketCommunicate::readOneRegFromPLC(const quint16 regAddr,const  quint16 regNum, QByteArray& result, const char registerFlag)
{
    QByteArray frame;
    frame=readDataIntialize( regAddr,  regNum, result, registerFlag);
    if(frame.isEmpty())
        return;
    writeDataToPlc(frame);

    quint16 offset = (quint16)(22 + regNum * 4);

    readDataFromPlc(offset,result);
}

void QPlcSocketCommunicate::writeOneRegToPLC(const quint16 regAddr,const  quint16 regNum, const int data, const char registerFlag)
{
    QByteArray frame;
    frame=writeDataIntialize(regAddr,regNum, data,registerFlag);
    if(frame.isEmpty())
        return;
    writeDataToPlc(frame);

    QByteArray buffer;
    buffer.resize(22);
    readDataFromPlc(22,buffer);
}
