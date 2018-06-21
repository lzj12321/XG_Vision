#ifndef YASKAWAROBOTSOCKETCOMMUNICATE_H
#define YASKAWAROBOTSOCKETCOMMUNICATE_H
#include"socketcommunicate.h"

class YaskawaRobotSocketCommunicate:public SocketCommunicate
{
public:
    YaskawaRobotSocketCommunicate();
    bool sendData(STR_Vision_Result&resultData);
private:
    bool sendData(const std::string);
    std::string dataTransferToStr(STR_Vision_Result&resultData);
private:
    int waitForReadAndWriteTime=2000;
};

#endif // YASKAWAROBOTSOCKETCOMMUNICATE_H
