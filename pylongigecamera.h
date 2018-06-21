#ifndef PYLONGIGECAMERA_H
#define PYLONGIGECAMERA_H
#include"gigecamera.h"
#include <pylon/PylonIncludes.h>
#include <pylon/gige/_BaslerGigECameraParams.h>
#include <pylon/gige/BaslerGigEInstantCamera.h>
using namespace Pylon;
using namespace Basler_GigECameraParams;
using namespace GenApi;

class pylonImageEventHandler:public QObject,public CImageEventHandler
{
    Q_OBJECT
public:
    pylonImageEventHandler();
    void OnImageGrabbed(CInstantCamera &camera, const CGrabResultPtr &grabResult);
    const cv::Mat &getGrabbedMat()const;
signals:
    void Esignal_grabbedImage(std::string,cv::Mat&);
private:
   cv::Mat grabbedMat;
};

class pylonCameraEventHandler:public QObject,public CConfigurationEventHandler
{
    Q_OBJECT
public:
    pylonCameraEventHandler();
    void OnCameraDeviceRemoved(CInstantCamera& camera);
    void OnOpened(CInstantCamera& camera);
    void OnAttach(CInstantCamera& camera);
    void OnAttached(CInstantCamera& camera);
signals:
    void Esignal_cameraRemoved(std::string);
    void Esignal_cameraOpened(std::string);
};


class pylonGigeCamera:public GigeCamera
{
    Q_OBJECT
public:
    pylonGigeCamera();
    ~pylonGigeCamera();
    static void PylonEnvInitialize();
    static void PylonEnvTerminate();
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
    pylonCameraEventHandler* cameraEventPtr;
    pylonImageEventHandler* imageEventPtr;
private:
    CInstantCamera gigeCamera;
    std::string cameraSerialNumber;
    INodeMap* nodemap;
    int isCameraEverOpened;
    CDeviceInfo openedCameraInfo;
private:
    bool cameraCreate(const std::string);
    void cameraInit();
    void cameraDestroy();
signals:
    void signal_connectedCamera(bool);
private slots:
    void slot_cameraRemoved(const std::string);
//public slots:
//    void slot_connectCamera(const std::string);
};
#endif // PYLONGIGECAMERA_H
