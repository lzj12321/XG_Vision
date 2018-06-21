#include "standardsocketcommunicate.h"
#include<QJsonObject>
#include<QJsonArray>

StandardSocketCommunicate::StandardSocketCommunicate()
{
    connect(&client,SIGNAL(readyRead()),this,SLOT(slot_readSignal()));
}

StandardSocketCommunicate::~StandardSocketCommunicate()
{
}

bool StandardSocketCommunicate::sendData(STR_Vision_Result &resultData)
{
    QJsonObject jsonData;
    jsonData.insert("x",resultData.x);
    jsonData.insert("y",resultData.y);
    jsonData.insert("angle",resultData.angle);
    jsonData.insert("model",resultData.model);
    jsonData.insert("errorFlag",resultData.errorFlag);
    QJsonDocument rectJsonDoc;
    rectJsonDoc.setObject(jsonData);
    QByteArray dataByte = rectJsonDoc.toJson(QJsonDocument::Compact);

    if(client.write(dataByte,dataByte.length())==dataByte.length())
        return true;
    else
        return false;
}

void StandardSocketCommunicate::slot_readSignal()
{
    QByteArray signalData=client.readAll();
    QJsonDocument doc = QJsonDocument::fromJson(signalData);
    QJsonObject obj = doc.object();
    int stationNum=0;
    stationNum=obj.value("station").toInt();
    int modelNum=0;
    modelNum=obj.value("model").toInt();
    emit signal_VisionRun(stationNum,modelNum);
}

