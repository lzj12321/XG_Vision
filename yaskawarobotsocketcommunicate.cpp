#include "yaskawarobotsocketcommunicate.h"

YaskawaRobotSocketCommunicate::YaskawaRobotSocketCommunicate()
{

}

bool YaskawaRobotSocketCommunicate::sendData(std::string dataStr)
{
    QString dataQStr=QString::fromStdString(dataStr);
    QByteArray dataByte=dataQStr.toLatin1();
    client.write(dataByte);
    if(client.waitForBytesWritten(waitForReadAndWriteTime))
        return true;
    else
        return false;
}

bool YaskawaRobotSocketCommunicate::sendData(STR_Vision_Result &resultData)
{
    return sendData(dataTransferToStr(resultData));
}

std::string YaskawaRobotSocketCommunicate::dataTransferToStr(STR_Vision_Result &resultData)
{
    std::string dataStr;
    double x=0;
    double y=0;
    double z=0;
    double u=0;
    double v=0;
    double angle=0;

    char x_char[256];
    char y_char[256];
    char z_char[256];
    char angle_char[256];
    char u_char[256];
    char v_char[256];

    if(resultData.x_flag){
        x=resultData.x;
    }
    if(resultData.y_flag){
        y=resultData.y;
    }
    if(resultData.z_flag){
        z=resultData.z;
    }
    if(resultData.angle_flag){
        angle=resultData.angle;
    }
    if(resultData.u_flag){
        u=resultData.u;
    }
    if(resultData.v_flag){
        v=resultData.v;
    }

    std::string x_str,y_str,z_str,u_str,v_str,angle_str;

    sprintf(x_char,"%lf",x);
    sprintf(y_char,"%lf",y);
    sprintf(z_char,"%lf",z);
    sprintf(angle_char,"%lf",angle);
    sprintf(u_char,"%lf",u);
    sprintf(v_char,"%lf",v);

    x_str=std::string(x_char);
    y_str=std::string(y_char);
    z_str=std::string(z_char);
    angle_str=std::string(angle_char);
    u_str=std::string(u_char);
    v_str=std::string(v_char);


    dataStr=std::string("5")+std::string(",") + std::string("0") + std::string(",") +x_str+ std::string(",") + y_str + std::string(",") +z_str+ std::string(",") +u_str+ std::string(",") +v_str + std::string(",") + angle_str + std::string(";");
    return dataStr;
}
