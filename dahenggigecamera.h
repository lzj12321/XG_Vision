#ifndef DAHENGGIGECAMERA_H
#define DAHENGGIGECAMERA_H
#include"gigecamera.h"
#include<GalaxyIncludes.h>

class DaHengImageEventHandler:public QObject,public ICaptureEventHandler
{
    Q_OBJECT
public:
    DaHengImageEventHandler(std::string);
    void DoOnImageCaptured(CImageDataPointer& objImageDataPointer, void* pUserParam);
    cv::Mat& getGrabbedMat();
signals:
    void Esignal_grabbedImage(std::string,cv::Mat&);
private:
    std::string cameraSerialNum;
    cv::Mat grabbedMat;
};

class DaHengCameraEventHandler:public QObject,public IDeviceOfflineEventHandler
{
    Q_OBJECT
private:
    std::string cameraSerialNum;
public:
    DaHengCameraEventHandler(std::string);
    void DoOnDeviceOfflineEvent(void* pUserParam);
signals:
    void Esignal_cameraRemoved(std::string);
};

class DaHengCameraFeatureEventHandler:public QObject,public IFeatureEventHandler
{
private:
    bool exposureStatus;
    std::string cameraSerialNum;
public:
    DaHengCameraFeatureEventHandler(const std::string);
    void DoOnFeatureEvent(const GxIAPICPP::gxstring& strFeatureName, void* pUserParam);
};

class DaHengGigeCamera:public GigeCamera
{
    Q_OBJECT
public:
    DaHengGigeCamera();
    virtual ~DaHengGigeCamera();
    static void DaHengEnvInitialize();
    static void DaHengEnvTerminate();
    static void EnumerateCameras(std::vector<std::string>&);
    bool connectCamera(const std::string);
    double getCameraExpose();
    void setCameraExpose(double value);
    void cameraStartGrab();
    void cameraStopGrab();
    void setHardWareTrigger();
    void setSoftTrigger();
    int getCameraWidth();
    void setCameraWidth(int value);
    int getCameraHeight();
    void setCameraHeight(int value);
    void setCameraDebouncerTime(double value);
    void closeHardWareTrigger();
    void closeSoftTrigger();
    std::string getCameraSerialNum()const;
    double getCameraFrameRate();
    void setCameraFrameRate(double);
    bool grabOneImage(cv::Mat&);
    void executeSoftTrigger();
private:
    bool cameraCreate(const std::string);
    void cameraInit();
    void cameraDestroy();
private:
    DaHengCameraEventHandler* cameraEventPtr;
    DaHengImageEventHandler* imageEventPtr;
    DaHengCameraFeatureEventHandler* featureEventPtr;
private:
    CGXDevicePointer gigeCameraPtr;
    CGXStreamPointer gigeCameraStreamPtr;
    GX_DEVICE_OFFLINE_CALLBACK_HANDLE hDeviceOffline;
    CGXFeatureControlPointer gigeCameraFeatureControlPtr;
private:
    std::string cameraSerialNumber;
    int width;
    int height;
    int exposeTime;
    int frameRate;
    bool isCameraInited=false;
signals:
    void signal_connectedCamera(bool);
private slots:
    void slot_cameraRemoved(const std::string);
    //public slots:
    //    void slot_connectCamera(const std::string);
};
#endif // DAHENGGIGECAMERA_H
