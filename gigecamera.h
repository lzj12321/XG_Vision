#ifndef GIGECAMERA_H
#define GIGECAMERA_H
#include<QObject>
#include<opencv2\core.hpp>

#define cameraOpened true
#define cameraClosed false

class GigeCamera:public QObject
{
    Q_OBJECT
public:
    GigeCamera();
    virtual ~GigeCamera();
    virtual bool connectCamera(const std::string);
    virtual void cameraDestroy();

    virtual void setCameraExpose(double value);
    virtual void setHardWareTrigger();
    virtual void setSoftTrigger();
    virtual void setCameraWidth(int value);
    virtual void setCameraHeight(int value);
    virtual bool grabOneImage(cv::Mat&);
    virtual void cameraStopGrab();
    virtual void cameraStartGrab();

    virtual int getCameraWidth();
    virtual int getCameraHeight();
    virtual std::string getCameraSerialNum()const;
    virtual double getCameraExpose();
    virtual bool getCameraStatus();
protected:
    bool cameraStatus=false;
signals:
    void signal_cameraOpened(std::string cameraSerialNum);
    void signal_cameraRemoved(std::string cameraSerialNum);
    void signal_grabbedImage(std::string cameraSerialNum,cv::Mat&);
protected slots:
    virtual void slot_cameraRemoved(const std::string);
    virtual void slot_cameraOpened(const std::string);
};


#endif // GIGECAMERA_H
