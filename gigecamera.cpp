#include "gigecamera.h"
GigeCamera::GigeCamera()
{
    cameraStatus=cameraClosed;
    connect(this,SIGNAL(signal_cameraRemoved(std::string)),this,SLOT(slot_cameraRemoved(std::string)));
    connect(this,SIGNAL(signal_cameraOpened(std::string)),this,SLOT(slot_cameraOpened(std::string)));
}

GigeCamera::~GigeCamera()
{

}

bool GigeCamera::connectCamera(const std::string)
{
    return false;
}

bool GigeCamera::getCameraStatus()
{
    return cameraStatus;
}

void GigeCamera::cameraDestroy()
{
    cameraStatus=cameraClosed;
}

void GigeCamera::setCameraExpose(double value)
{

}

void GigeCamera::setHardWareTrigger()
{

}

void GigeCamera::setSoftTrigger()
{

}

int GigeCamera::getCameraWidth()
{
    return 1;
}

void GigeCamera::setCameraWidth(int value)
{

}

int GigeCamera::getCameraHeight()
{
    return 1;
}

void GigeCamera::setCameraHeight(int value)
{

}

bool GigeCamera::grabOneImage(cv::Mat&)
{
    return false;
}

std::string GigeCamera::getCameraSerialNum()const
{
    return std::string("NULL");
}

void GigeCamera::cameraStopGrab()
{

}

double GigeCamera::getCameraExpose()
{
    return 0;
}

void GigeCamera::cameraStartGrab()
{

}

void GigeCamera::slot_cameraRemoved(const std::string)
{
    cameraStatus=cameraClosed;
}

void GigeCamera::slot_cameraOpened(const std::string)
{
    cameraStatus=cameraOpened;
}
