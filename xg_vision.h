#ifndef XG_VISION_H
#define XG_VISION_H

#include <QMainWindow>
#include<QCloseEvent>
#include"ncc_match.h"
#include<QTime>
#include<QTimer>
#include"dahenggigecamera.h"
#include"pylongigecamera.h"
#include"calibration.h"
#include"cvandui.h"
#include"nccmatchwidget.h"
#include"camerasettingswidget.h"
#include"communicationsettngswidget.h"
#include<QMenuBar>
#include<QAction>
#include<aboutdialog.h>
#include<recordsettingsdialog.h>
#include<QInputDialog>
#include<vision.h>

namespace Ui {
class XG_Vision;
}

class XG_Vision : public QMainWindow,cvAndUi
{
    Q_OBJECT
public:
    explicit XG_Vision(QWidget *parent = 0);
    ~XG_Vision();
protected:
    void closeEvent(QCloseEvent *event);
private:
    Ui::XG_Vision *ui;
    AboutDialog* aboutDailog;
    std::vector<cv::Mat>vecSourceMat;
private:
    std::vector<nccMatchWidget*>vecNccWidget;
    std::vector<CommunicationSettngsWidget*>vecCommWidget;
    CameraSettingsWidget cameraWidget;
    RecordSettingsDialog recordWidget;
private:
    int stationNum=0;
    int tabIndex=0;
    int recordNum=0;
    int matchUsedTime=0;
    std::vector<calibration*>vecCalibrateWidget;
    std::vector<QLabel*>vecTabLabelPtr;
    std::vector<strVisionParam>vecVisionParam;
    std::map<std::string,int>mapCameraExposeTime;
private:
    void uiInitialize();
    void cameraInitialize(std::map<std::string,int>*exposeMap=nullptr);
    void addStation(QString&tabName);
    void delAStation(int index);
    void paramInitialize();
    void paramLoad();
    void cameraParamLoad();
    void paramSave();
    void createNewStation(QString&tabName);
    void resultOutPut(const STR_Vision_Result&);
    void displayRunSignal(int,int);
private slots:
    void on_pushButton_clicked();
    void on_pushButton_2_clicked();
    void cameraStatusChanged(bool);
    void communicateStatusChanged(bool);
    void on_tabWidget_currentChanged(int index);
    void slot_menuAction(QAction*act);
    void on_pushButton_4_clicked();
    void on_pushButton_5_clicked();
    void on_pushButton_6_clicked();
    void slot_VisionRun(int,int);
    void slot_cameraStatusChanged(std::string,bool);
    void slot_communicateStatusChanged(int,bool);
signals:
    void signal_mainWindowClosed();
    void signal_sendResult(QString);
};

#endif // XG_VISION_H
