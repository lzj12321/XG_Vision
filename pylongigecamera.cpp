#include "pylongigecamera.h"
#include<QException>
#include<QString>
#include<iostream>

pylonGigeCamera::pylonGigeCamera()
{
    isCameraEverOpened=false;

    cameraEventPtr=new pylonCameraEventHandler;
    imageEventPtr=new pylonImageEventHandler;

    connect(cameraEventPtr,SIGNAL(Esignal_cameraOpened(std::string)),this,SIGNAL(signal_cameraOpened(std::string)));
    connect(cameraEventPtr,SIGNAL(Esignal_cameraRemoved(std::string)),this,SIGNAL(signal_cameraRemoved(std::string)));
    connect(imageEventPtr,SIGNAL(Esignal_grabbedImage(std::string,cv::Mat&)),this,SIGNAL(signal_grabbedImage(std::string,cv::Mat&)));
}

pylonGigeCamera::~pylonGigeCamera()
{
    ////////////在相机掉线的时候自动清除//////////////////
    //deallcoate the resource when camera offline//
}

void pylonGigeCamera::PylonEnvInitialize()
{
    PylonInitialize();
}

void pylonGigeCamera::PylonEnvTerminate()
{
    PylonTerminate();
}

void pylonGigeCamera::EnumerateCameras(std::vector<std::string>& vecCameraSerial)
{
    vecCameraSerial.clear();
    CTlFactory& tlFactory = CTlFactory::GetInstance();
    DeviceInfoList_t devices;
    size_t cameraNum=tlFactory.EnumerateDevices(devices);
    for(size_t i=0;i<cameraNum;++i){
        vecCameraSerial.push_back(std::string(devices[i].GetSerialNumber().c_str()));
    }
}

bool pylonGigeCamera::connectCamera(const std::string tmpCameraSerialNumber)
{
    cameraStatus=cameraCreate(tmpCameraSerialNumber);
    if(cameraStatus)
        cameraInit();
    emit signal_connectedCamera(cameraStatus);
    return cameraStatus;
}

bool pylonGigeCamera::cameraCreate(const std::string cameraNum)
{
    try
    {
        CDeviceInfo cameraInfo;
        cameraSerialNumber= cameraNum;
        String_t STR_cameraSerialNum=String_t(cameraNum.c_str());
        cameraInfo.SetSerialNumber(STR_cameraSerialNum);
        if(!isCameraEverOpened)
        {
            DeviceInfoList_t tmpCameraInfoList;
            DeviceInfoList_t detectedCamerDevices;
            tmpCameraInfoList.push_back(cameraInfo);
            CTlFactory& cameraFactory = CTlFactory::GetInstance();
            if(cameraFactory.EnumerateDevices(detectedCamerDevices,tmpCameraInfoList)>0)
            {
                if(!cameraFactory.IsDeviceAccessible(cameraInfo)){
                    return false;
                }
                gigeCamera.Attach(cameraFactory.CreateDevice(detectedCamerDevices[0]));
                if(gigeCamera.IsPylonDeviceAttached())
                {
                    cameraStatus=true;
                    nodemap=&gigeCamera.GetNodeMap();
                    GenApi::CIntegerPtr heartBeatTimePtr(NULL);
                    heartBeatTimePtr=gigeCamera.GetTLNodeMap().GetNode("HeartbeatTimeout");
                    int64_t NewValue=1000;
                    int64_t correctedValue = NewValue - (NewValue % heartBeatTimePtr->GetInc());
                    heartBeatTimePtr->SetValue(correctedValue);
                    return true;
                }
                else
                    return false;
            }
            else
                return false;
        }
        else
        {
            //////////////////////reconnect camera part////////////////////////////
            DeviceInfoList_t tmpCameraInfoList;
            DeviceInfoList_t detectedCamerDevices;
            tmpCameraInfoList.push_back(openedCameraInfo);
            CTlFactory& cameraFactory = CTlFactory::GetInstance();
            if(cameraFactory.EnumerateDevices(detectedCamerDevices,tmpCameraInfoList)>0)
            {
                gigeCamera.Attach(cameraFactory.CreateDevice(detectedCamerDevices[0]));
                WaitObject::Sleep(250);
                if(gigeCamera.IsPylonDeviceAttached())
                {
                    nodemap=&gigeCamera.GetNodeMap();
                    cameraStatus=true;
                    return true;
                }
                else
                    return false;
            }
            else
                return false;
        }
    }
    catch(GenericException e)
    {
        return false;
    }
    catch(QException& ex)
    {
        return false;
    }
}

void pylonGigeCamera::cameraInit()
{
    try
    {
        if(!isCameraEverOpened)
        {
            gigeCamera.RegisterConfiguration(new CAcquireContinuousConfiguration,RegistrationMode_ReplaceAll,Cleanup_Delete);//注册实时模式
            gigeCamera.RegisterConfiguration(cameraEventPtr,RegistrationMode_Append,Cleanup_Delete);
            gigeCamera.RegisterImageEventHandler(imageEventPtr,RegistrationMode_Append,Cleanup_Delete);
        }
        gigeCamera.Open();
        if(gigeCamera.IsOpen())
            isCameraEverOpened=true;
    }
    catch(GenericException ex)
    {
    }
}

void pylonGigeCamera::cameraDestroy()
{
    gigeCamera.DestroyDevice();
}

double pylonGigeCamera::getCameraExpose()
{
    try
    {
        if(cameraStatus)
        {
            CFloatPtr ExpTimeCur(nodemap->GetNode("ExposureTimeAbs"));
            return ((double)ExpTimeCur->GetValue());
        }
        else
            return 0;
    }
    catch(GenericException& ex)
    {
        return 0;
    }
}

void pylonGigeCamera::setCameraExpose(double value)
{
    if(cameraStatus)
    {
        CEnumerationPtr ExpAuto(nodemap->GetNode("ExposureAuto"));
        if (IsWritable(ExpAuto))
        {
            ExpAuto->FromString("Off");
        }
        CFloatPtr SetExpTime(nodemap->GetNode("ExposureTimeAbs"));
        if (value <SetExpTime->GetMin())
        {
            SetExpTime->SetValue(SetExpTime->GetMin());
        }
        else if (value > SetExpTime->GetMax())
        {
            SetExpTime->SetValue(SetExpTime->GetMax());
        }
        else
        {
            SetExpTime->SetValue(value);
        }
    }
}

void pylonGigeCamera::cameraStartGrab()
{
    if(cameraStatus)
    {
        if(!gigeCamera.IsGrabbing())
            gigeCamera.StartGrabbing(GrabStrategy_OneByOne,GrabLoop_ProvidedByInstantCamera);
    }
}

void pylonGigeCamera::cameraStopGrab()
{
    if(cameraStatus)
    {
        if(gigeCamera.IsGrabbing())
            gigeCamera.StopGrabbing();
    }
}

void pylonGigeCamera::setHardWareTrigger()
{
    if(cameraStatus)
    {
        CEnumerationPtr triggerSelector(nodemap->GetNode("TriggerSelector"));
        CEnumerationPtr triggerMode(nodemap->GetNode("TriggerMode"));
        CEnumerationPtr triggerSource(nodemap->GetNode("TriggerSource"));

        setCameraDebouncerTime(10000);
        triggerSelector->FromString("FrameStart");
        triggerMode->FromString("On");
        triggerSource->FromString("Line1");
    }
}

void pylonGigeCamera::setSoftTrigger()
{
    if(cameraStatus)
    {
        CEnumerationPtr triggerSelector(nodemap->GetNode("TriggerSelector"));
        CEnumerationPtr triggerMode(nodemap->GetNode("TriggerMode"));
        CEnumerationPtr triggerSource(nodemap->GetNode("TriggerSource"));

        triggerSelector->FromString("FrameStart");
        triggerMode->FromString("On");
        triggerSource->FromString("Software");
    }
}

int pylonGigeCamera::getCameraWidth()
{
    if(cameraStatus)
    {
        CIntegerPtr CWidth(nodemap->GetNode("SensorWidth"));
        return CWidth->GetValue();
    }
    else
        return 0;
}

void pylonGigeCamera::setCameraWidth(int value)
{
    if(cameraStatus)
    {
        CIntegerPtr CHeight(nodemap->GetNode("SensorHeight"));
        CHeight->SetValue(value);
    }
}

int pylonGigeCamera::getCameraHeight()
{
    if(cameraStatus)
    {
        CIntegerPtr CHeight(nodemap->GetNode("SensorHeight"));
        return CHeight->GetValue();
    }
    else
        return 0;
}

void pylonGigeCamera::setCameraHeight(int value)
{
    if(cameraStatus)
    {
        CIntegerPtr CHeight(nodemap->GetNode("SensorHeight"));
        CHeight->SetValue(value);
    }
}

void pylonGigeCamera::setCameraDebouncerTime(double value)
{
    if(cameraStatus)
    {
        CEnumerationPtr Lselector(nodemap->GetNode("LineSelector"));
        Lselector->SetIntValue(LineSelector_Line1);
        CFloatPtr lineDeb=nodemap->GetNode("LineDebouncerTimeAbs");
        lineDeb->SetValue(value);
    }
}

void pylonGigeCamera::closeHardWareTrigger()
{
    if(cameraStatus)
    {
        CEnumerationPtr triggerMode(nodemap->GetNode("TriggerMode"));
        triggerMode->FromString("Off");
    }
}

void pylonGigeCamera::closeSoftTrigger()
{
    if(cameraStatus)
    {
        CEnumerationPtr triggerMode(nodemap->GetNode("TriggerMode"));
        triggerMode->FromString("Off");
    }
}

std::string pylonGigeCamera::getCameraSerialNum()const
{
    return cameraSerialNumber;
}

double pylonGigeCamera::getCameraFrameRate()
{
    if(cameraStatus)
    {
        CFloatPtr FrameRate(nodemap->GetNode("ResultingFrameRateAbs"));
        return ((double)FrameRate->GetValue());
    }
    else
        return 0;
}

void pylonGigeCamera::setCameraFrameRate(double value)
{
    if(cameraStatus)
    {
        try
        {
            CFloatPtr FrameRate(nodemap->GetNode("ResultingFrameRateAbs"));
            FrameRate->SetValue(value);
        }
        catch(GenericException e)
        {
        }
    }
}

bool pylonGigeCamera::grabOneImage(cv::Mat&mat)
{
    if(cameraStatus)
    {
        if(gigeCamera.IsGrabbing())
            cameraStopGrab();
        CGrabResultPtr ptrGrabResult;
        gigeCamera.GrabOne(1000, ptrGrabResult, TimeoutHandling_ThrowException);
        if(ptrGrabResult->GrabSucceeded()&&ptrGrabResult)
        {
            imageEventPtr->getGrabbedMat().copyTo(mat);
            return true;
        }
    }
    else
        return false;
}

void pylonGigeCamera::executeSoftTrigger()
{
    if(cameraStatus)
    {
        CEnumerationPtr triggerMode(nodemap->GetNode("TriggerMode"));
        CEnumerationPtr triggerSource(nodemap->GetNode("TriggerSource"));
        if(triggerMode->GetIntValue()==1&&triggerSource->GetIntValue()==0)
        {
            if (gigeCamera.CanWaitForFrameTriggerReady())
            {
                cameraStartGrab();
                if (gigeCamera.WaitForFrameTriggerReady( 500, TimeoutHandling_ThrowException))
                {
                    gigeCamera.ExecuteSoftwareTrigger();
                }
            }
        }
    }
}

void pylonGigeCamera::slot_cameraRemoved(const std::string)
{
    cameraStatus=cameraClosed;
    openedCameraInfo.SetDeviceClass(gigeCamera.GetDeviceInfo().GetDeviceClass());
    openedCameraInfo.SetSerialNumber(gigeCamera.GetDeviceInfo().GetSerialNumber());
    cameraDestroy();
}

pylonImageEventHandler::pylonImageEventHandler()
{

}

void pylonImageEventHandler::OnImageGrabbed(CInstantCamera &camera, const CGrabResultPtr &grabResult)
{
    std::string strCameraNum=std::string(camera.GetDeviceInfo().GetSerialNumber().c_str());
    if(grabResult->GrabSucceeded()&&grabResult)
    {
        int rows = grabResult->GetHeight();
        int columns = grabResult->GetWidth();
        uchar* grabPtr=(uchar*)grabResult->GetBuffer();
        int nCols = rows*columns;
        grabbedMat=cv::Mat(rows, columns, CV_8UC1);
        uchar* matPtr;
        matPtr =grabbedMat.data;
        if (grabbedMat.isContinuous())
        {
            for (int i = 0; i < nCols; i++){
                matPtr[i] = grabPtr[i];
            }
            emit Esignal_grabbedImage(strCameraNum,grabbedMat);
        }
    }
}

const cv::Mat &pylonImageEventHandler::getGrabbedMat() const
{
    return grabbedMat;
}

pylonCameraEventHandler::pylonCameraEventHandler()
{
}

void pylonCameraEventHandler::OnCameraDeviceRemoved(CInstantCamera &camera)
{
    std::string strCameraNum=std::string(camera.GetDeviceInfo().GetSerialNumber().c_str());
    emit Esignal_cameraRemoved(strCameraNum);
    camera.DestroyDevice();
    camera.DetachDevice();
}

void pylonCameraEventHandler::OnOpened(CInstantCamera &camera)
{
    std::string strCameraNum=std::string(camera.GetDeviceInfo().GetSerialNumber().c_str());
    emit Esignal_cameraOpened(strCameraNum);
}

void pylonCameraEventHandler::OnAttach(CInstantCamera &camera)
{
    std::string strCameraNum=std::string(camera.GetDeviceInfo().GetSerialNumber().c_str());
}

void pylonCameraEventHandler::OnAttached(CInstantCamera &camera)
{
    std::string strCameraNum=std::string(camera.GetDeviceInfo().GetSerialNumber().c_str());
    emit Esignal_cameraOpened(strCameraNum);
}
