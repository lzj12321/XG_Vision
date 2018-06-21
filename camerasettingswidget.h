#ifndef CAMERASETTINGSWIDGET_H
#define CAMERASETTINGSWIDGET_H

#include <QWidget>
#include<QCloseEvent>
#include"cvandui.h"
#include"pylongigecamera.h"
#include"dahenggigecamera.h"

#define PYLON_GIGE_CAMERA 1
#define DAHENG_GIGE_CAMERA 2

typedef struct CameraParamerStr
{
    int cameraSerialNumber;
    int cameraExposeTime;
    bool cameraStatus;
}CameraParamerStr;

namespace Ui {
class CameraSettingsWidget;
}

class CameraSettingsWidget : public QWidget,cvAndUi
{
    Q_OBJECT
public:
    explicit CameraSettingsWidget(QWidget *parent = 0);
    virtual ~CameraSettingsWidget();
    void cameraWidgetInitialize(std::map<std::string,int>*);
    bool grabOneImg(std::string&cameraSerialNum,cv::Mat&r);
    void setCameraTabIndex(std::string*cameraSerialNum);
    bool getCameraStatus(const std::string&cameraSerialNum);
protected:
    void closeEvent(QCloseEvent *event);
private slots:
    void on_pushButton_clicked();
    void on_pushButton_13_clicked();
    void on_pushButton_14_clicked();
private:
    Ui::CameraSettingsWidget *ui;

private:
    int comboxIndex=0;
    std::string*cameraSerialPtr=nullptr;
    std::map<std::string,GigeCamera*>cameraMap;
    std::map<int,std::string>cameraSerialNumIndexMap;
    std::map<std::string,int>indexCameraSerialNumMap;
    std::map<std::string,int>*mapCameraExposeTimePtr=nullptr;
private:
    bool connectCamera(int cameraType,std::string serialNum);
    void uiInitialize();
    ///refresh camera list////////
    void cameraRefresh();
    void clearOffLineCamera();
    void connectNewOnLineCamera();
    void uiCameraChangeed(std::string cameraSerialNum);
private slots:
    void slot_cameraGrabbedImage(std::string,cv::Mat&);
    void slot_cameraOffLine(std::string);
    void slot_cameraOnLine(std::string);

    void on_pushButton_16_clicked();
    void on_pushButton_15_clicked();
    void on_comboBox_currentIndexChanged(int index);
    void on_pushButton_2_clicked();
    void on_horizontalSlider_sliderMoved(int position);
signals:
    void cameraStatusChanged(std::string,bool);
};

#endif // CAMERASETTINGSWIDGET_H
