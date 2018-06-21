#include "DaHengGigeCamera.h"

DaHengImageEventHandler::DaHengImageEventHandler(std::string tmpCameraSerialNum)
{
    cameraSerialNum=tmpCameraSerialNum;
}

void DaHengImageEventHandler::DoOnImageCaptured(CImageDataPointer& grabResult, void* pUserParam)
{
    if (grabResult->GetStatus()==GX_FRAME_STATUS_SUCCESS)
    {
        uchar* grabPtr=(uchar*)grabResult->GetBuffer();
        int rows = grabResult->GetHeight();
        int columns = grabResult->GetWidth();
        int nCols = rows*columns;
        grabbedMat=cv::Mat(rows, columns, CV_8UC1);
        uchar* matPtr;
        matPtr=grabbedMat.data;
        if (grabbedMat.isContinuous())
        {
            for (int i = 0; i < nCols; i++){
                matPtr[i] = grabPtr[i];
            }
            emit Esignal_grabbedImage(cameraSerialNum,grabbedMat);
        }
    }
}

cv::Mat &DaHengImageEventHandler::getGrabbedMat()
{
    return grabbedMat;
}

DaHengCameraEventHandler::DaHengCameraEventHandler(std::string tmpCameraSerialNum)
{
    cameraSerialNum=tmpCameraSerialNum;
}

void DaHengCameraEventHandler::DoOnDeviceOfflineEvent(void* pUserParam)
{
    emit Esignal_cameraRemoved(cameraSerialNum);
}

DaHengGigeCamera::DaHengGigeCamera()
{
    hDeviceOffline = nullptr;
}

DaHengGigeCamera::~DaHengGigeCamera()
{ 
    if(cameraStatus)
        cameraDestroy();
}

void DaHengGigeCamera::DaHengEnvInitialize()
{
    IGXFactory::GetInstance().Init();
}

void DaHengGigeCamera::DaHengEnvTerminate()
{
    IGXFactory::GetInstance().Uninit();
}

void DaHengGigeCamera::EnumerateCameras(std::vector<std::string>&vecCameraSerial)
{
    GxIAPICPP::gxdeviceinfo_vector vectorDeviceInfo;
    IGXFactory::GetInstance().UpdateDeviceList(1000, vectorDeviceInfo);
    for(int i=0;i<vectorDeviceInfo.size();++i){
        vecCameraSerial.push_back(std::string(vectorDeviceInfo[i].GetSN().c_str()));
    }
}

bool DaHengGigeCamera::connectCamera(const std::string tmpSerialNum)
{
    cameraStatus=cameraCreate(tmpSerialNum);
    if(cameraStatus)
        cameraInit();
    emit signal_connectedCamera(cameraStatus);
    return cameraStatus;
}

bool DaHengGigeCamera::cameraCreate(const std::string tmpCameraSerialNum)
{
    try
    {
        cameraSerialNumber=std::string(tmpCameraSerialNum.c_str());
        GxIAPICPP::gxdeviceinfo_vector vectorDeviceInfo;
        IGXFactory::GetInstance().UpdateDeviceList(1000, vectorDeviceInfo);

        //打开链表中的指定设备
        if (vectorDeviceInfo.size()==0)return false;
        GxIAPICPP::gxstring strSN = GxIAPICPP::gxstring(tmpCameraSerialNum.c_str());
        CGXDeviceInfo cameraInfo;
        for(int i=0;i<vectorDeviceInfo.size();++i){
            if(tmpCameraSerialNum==std::string(vectorDeviceInfo[i].GetSN().c_str()))
                cameraInfo=vectorDeviceInfo[i];
        }
        if(cameraInfo.GetAccessStatus()!=GX_ACCESS_STATUS_READWRITE)
        {
            return false;
        }

        gigeCameraPtr = IGXFactory::GetInstance().OpenDeviceBySN(strSN, GX_ACCESS_EXCLUSIVE);
        if (gigeCameraPtr.IsNull())
        {
            cameraStatus=false;
            return false;
        }
        else
        {
            cameraStatus=true;
            emit signal_cameraOpened(cameraSerialNumber);
            return true;
        }
    }
    catch (CGalaxyException& e)
    {
        cameraStatus=false;
        return false;
    }
}

void DaHengGigeCamera::cameraInit()
{
    if(!cameraStatus)
        return;
    imageEventPtr=new DaHengImageEventHandler(cameraSerialNumber);
    cameraEventPtr=new DaHengCameraEventHandler(cameraSerialNumber);
    featureEventPtr=new DaHengCameraFeatureEventHandler(cameraSerialNumber);

    connect(cameraEventPtr,SIGNAL(Esignal_cameraRemoved(std::string)),this,SIGNAL(signal_cameraRemoved(std::string)));
    connect(imageEventPtr,SIGNAL(Esignal_grabbedImage(std::string,cv::Mat&)),this,SIGNAL(signal_grabbedImage(std::string,cv::Mat&)));

    gigeCameraStreamPtr= gigeCameraPtr->OpenStream(0);
    gigeCameraFeatureControlPtr = gigeCameraPtr->GetRemoteFeatureControl();
    ///////注册相机掉线事件和拍照回调事件////////////////////
    //gigeCameraStreamPtr->RegisterCaptureCallback(imageEventPtr, NULL);
    hDeviceOffline = gigeCameraPtr->RegisterDeviceOfflineCallback(cameraEventPtr, NULL);

    GX_FEATURE_CALLBACK_HANDLE hFeatureEvent = NULL;
    hFeatureEvent = gigeCameraFeatureControlPtr->RegisterFeatureCallback("EventExposureEnd", featureEventPtr, NULL);
    isCameraInited=true;
}

void DaHengGigeCamera::cameraDestroy()
{
    //    if(!cameraStatus)
    //        return;
    //在关闭设备之前一定要注销事件：
    if(!isCameraInited)return;
    gigeCameraStreamPtr->UnregisterCaptureCallback();
    gigeCameraPtr->UnregisterDeviceOfflineCallback(hDeviceOffline);
    gigeCameraFeatureControlPtr->UnregisterFeatureCallback(featureEventPtr);

    delete cameraEventPtr;
    delete imageEventPtr;
    delete featureEventPtr;

    cameraEventPtr = NULL;
    imageEventPtr=NULL;
    featureEventPtr=NULL;
    //关闭流通道及设备
    gigeCameraStreamPtr->Close();
    gigeCameraPtr->Close();
    cameraStatus=false;
}

double DaHengGigeCamera::getCameraExpose()
{
    if(!cameraStatus)
        return 0;
    CFloatFeaturePointer objIntPtr = gigeCameraFeatureControlPtr->GetFloatFeature("ExposureTime");
    double exposureTime = objIntPtr->GetValue();//获取当前值
    return exposureTime;
}

void DaHengGigeCamera::setCameraExpose(double value)
{
    if(!cameraStatus)
        return;
    CFloatFeaturePointer objIntPtr = gigeCameraFeatureControlPtr->GetFloatFeature("ExposureTime");
    double nMax = objIntPtr->GetMax();//获取最大值
    double nMin = objIntPtr->GetMin();//获取最小值
    if (value>nMin&&value<nMax)
        objIntPtr->SetValue(value);//设置当前值
}

void DaHengGigeCamera::cameraStartGrab()
{
    if(!cameraStatus)
        return;
    gigeCameraFeatureControlPtr->GetCommandFeature("AcquisitionStart")->Execute();
    gigeCameraStreamPtr->RegisterCaptureCallback(imageEventPtr, NULL);
    //设置采集模式为连续采集模式
    gigeCameraFeatureControlPtr->GetEnumFeature("AcquisitionMode")->SetValue("Continuous");
    gigeCameraStreamPtr->StartGrab();
}

void DaHengGigeCamera::cameraStopGrab()
{
    if(!cameraStatus)
        return;
    CGXFeatureControlPointer _gigeCameraFeatureControlPtr = gigeCameraPtr->GetRemoteFeatureControl();
    _gigeCameraFeatureControlPtr->GetCommandFeature("AcquisitionStop")->Execute();
    gigeCameraStreamPtr->StopGrab();
    gigeCameraStreamPtr->UnregisterCaptureCallback();
}

void DaHengGigeCamera::setHardWareTrigger()
{
    if(!cameraStatus)
        return;
}

void DaHengGigeCamera::setSoftTrigger()
{
    if(!cameraStatus)
        return;
}

int DaHengGigeCamera::getCameraWidth()
{
    if(!cameraStatus)
        return 0;
    CIntFeaturePointer objIntPtr = gigeCameraFeatureControlPtr->GetIntFeature("Width");
    int64_t nValue = objIntPtr->GetValue();//获取当前值
    return nValue;
}

void DaHengGigeCamera::setCameraWidth(int value)
{
    if(!cameraStatus)
        return;
    CIntFeaturePointer objIntPtr = gigeCameraFeatureControlPtr->GetIntFeature("Width");
    int64_t nMax = objIntPtr->GetMax();//获取最大值
    int64_t nMin = objIntPtr->GetMin();//获取最小值
    if(value>nMin&&value<nMax)
        objIntPtr->SetValue(value);//设置当前值
}

int DaHengGigeCamera::getCameraHeight()
{
    if(!cameraStatus)
        return 0;
    CIntFeaturePointer objIntPtr = gigeCameraFeatureControlPtr->GetIntFeature("Height");
    int64_t nValue = objIntPtr->GetValue();//获取当前值
    return nValue;
}

void DaHengGigeCamera::setCameraHeight(int value)
{
    if(!cameraStatus)
        return;
    CIntFeaturePointer objIntPtr = gigeCameraFeatureControlPtr->GetIntFeature("Height");
    int64_t nMax = objIntPtr->GetMax();//获取最大值
    int64_t nMin = objIntPtr->GetMin();//获取最小值
    if (value>nMin&&value<nMax)
        objIntPtr->SetValue(value);//设置当前值
}

void DaHengGigeCamera::setCameraDebouncerTime(double value)
{
    if(!cameraStatus)
        return;
}

void DaHengGigeCamera::closeHardWareTrigger()
{
    if(!cameraStatus)
        return;
}

void DaHengGigeCamera::closeSoftTrigger()
{
    if(!cameraStatus)
        return;
}

std::string DaHengGigeCamera::getCameraSerialNum()const
{
    // GxIAPICPP::gxstring serialNum=gigeCameraPtr->GetDeviceInfo().GetSN();
    return cameraSerialNumber;
}

double DaHengGigeCamera::getCameraFrameRate()
{
    if(!cameraStatus)
        return 0;
    return 1;
}

void DaHengGigeCamera::setCameraFrameRate(double)
{
    if(!cameraStatus)
        return;
}

bool DaHengGigeCamera::grabOneImage(cv::Mat&mat)
{
    try
    {
        if(!cameraStatus)
            return false;
        gigeCameraStreamPtr->SetAcqusitionBufferNumber(10);

        //开启流通道采集
        gigeCameraStreamPtr->StartGrab();
        //给设备发送开采命令
        gigeCameraFeatureControlPtr->GetCommandFeature("AcquisitionStart")->Execute();

        //采单帧
        CImageDataPointer grabResult;
        //接口描述：采集单帧。参数1为超时时间，单位毫秒。如果超出此时间还没获取到图像则返回超时错误。
        //功能需求：如果用户注册了采集委托函数，则不允许使用GetImage接口，如果强行调用此接口返回非法调用异常。
        grabResult = gigeCameraStreamPtr->GetImage(2000);//超时时间使用500ms，用户可以自行设定
        gigeCameraFeatureControlPtr->GetCommandFeature("AcquisitionStop")->Execute();
        gigeCameraStreamPtr->StopGrab();

        if (grabResult->GetStatus()==GX_FRAME_STATUS_SUCCESS)
        {
            uchar* grabPtr=(uchar*)grabResult->GetBuffer();
            int rows = grabResult->GetHeight();
            int columns = grabResult->GetWidth();

            int nCols = rows*columns;
            mat=cv::Mat(rows, columns, CV_8UC1);
            uchar* matPtr=mat.data;
            if (mat.isContinuous())
            {
                for (int i = 0; i < nCols; i++){
                    matPtr[i] = grabPtr[i];
                }
            }
            else
            {
                mat=cv::Mat(rows, columns, CV_8UC1);
                matPtr=mat.data;
                if (mat.isContinuous())
                {
                    for (int i = 0; i < nCols; i++){
                        matPtr[i] = grabPtr[i];
                    }
                }
                else
                    return false;
            }
            return true;
        }
        return false;
    }
    catch (CGalaxyException& e)
    {
        cameraStatus=false;
        return false;
    }
}

void DaHengGigeCamera::executeSoftTrigger()
{
    if(!cameraStatus)
        return;
}

void DaHengGigeCamera::slot_cameraRemoved(const std::string tmpCameraSerialNum)
{
    cameraStatus=false;
    cameraDestroy();
}

//void DaHengGigeCamera::slot_connectCamera(const std::string tmpSerialNum)
//{
//    cameraStatus=cameraCreate(tmpSerialNum);
//    if(cameraStatus)
//        cameraInit();
//    emit signal_connectedCamera(cameraStatus);
//}

DaHengCameraFeatureEventHandler::DaHengCameraFeatureEventHandler(const std::string tmpCameraSerialNum)
{
    cameraSerialNum=tmpCameraSerialNum;
}

void DaHengCameraFeatureEventHandler::DoOnFeatureEvent(const gxstring &strFeatureName, void *pUserParam)
{
    exposureStatus=true;
}
