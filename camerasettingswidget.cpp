#include "camerasettingswidget.h"
#include "ui_camerasettingswidget.h"
#include <QTextCodec>
#include<QSettings>
#include<QTextStream>
#include<QMessageBox>
#include<iostream>
#include<QTime>

CameraSettingsWidget::CameraSettingsWidget(QWidget *parent):
    QWidget(parent),
    ui(new Ui::CameraSettingsWidget)
{
    ui->setupUi(this);
    pylonGigeCamera::PylonEnvInitialize();
    DaHengGigeCamera::DaHengEnvInitialize();
}

CameraSettingsWidget::~CameraSettingsWidget()
{
    auto iter=cameraMap.begin();
    for(;iter!=cameraMap.end();++iter){
        delete iter->second;
    }
    pylonGigeCamera::PylonEnvTerminate();
    DaHengGigeCamera::DaHengEnvTerminate();
    delete ui;
}

void CameraSettingsWidget::cameraWidgetInitialize(std::map<std::string,int>*tempMapCameraExposeTimePtr)
{
    mapCameraExposeTimePtr=tempMapCameraExposeTimePtr;
    cameraRefresh();
    uiInitialize();
    if(cameraMap.size()==0){
        ui->textEdit->setTextColor(Qt::red);
        ui->textEdit->append(QTime::currentTime().toString());
        ui->textEdit->append(QString("Didn't detect any camera!"));
    }
}

void CameraSettingsWidget::cameraRefresh()
{
    //////////clear the offline camera///////////////
    clearOffLineCamera();
    //////////connect the new camera////////////////
    connectNewOnLineCamera();
}

void CameraSettingsWidget::clearOffLineCamera()
{
    //////////clear the offline camera///////////////
    for(auto iter=cameraMap.begin();iter!=cameraMap.end();++iter){
        if(!iter->second->getCameraStatus()){
            std::string offLineCameraSerial;
            offLineCameraSerial=iter->first;
            delete iter->second;
            iter=cameraMap.erase(iter);
            int index=indexCameraSerialNumMap[offLineCameraSerial];
            indexCameraSerialNumMap.erase(offLineCameraSerial);
            cameraSerialNumIndexMap.erase(index);
            continue;
        }
    }
}

void CameraSettingsWidget::connectNewOnLineCamera()
{
    //////////connect the new pylon camera////////////////
    std::vector<std::string>pylonAccessibleCameraSerialVec;
    pylonGigeCamera::EnumerateCameras(pylonAccessibleCameraSerialVec);
    for(int i=0;i<pylonAccessibleCameraSerialVec.size();++i){
        if(cameraMap.find(pylonAccessibleCameraSerialVec[i])==cameraMap.end()){
            connectCamera(PYLON_GIGE_CAMERA,pylonAccessibleCameraSerialVec[i]);
        }
    }
    ///////////connect the new daheng camera/////////////////
    std::vector<std::string>DahengAccessibleCameraSerialVec;
    DaHengGigeCamera::EnumerateCameras(DahengAccessibleCameraSerialVec);
    for(int i=0;i<DahengAccessibleCameraSerialVec.size();++i){
        if(cameraMap.find(DahengAccessibleCameraSerialVec[i])==cameraMap.end()){
            connectCamera(DAHENG_GIGE_CAMERA,DahengAccessibleCameraSerialVec[i]);
        }
    }
}

bool CameraSettingsWidget::grabOneImg(std::string&cameraSerialNum,cv::Mat&grabMat)
{
    if(cameraMap.find(cameraSerialNum)==cameraMap.end())return false;
    if(cameraMap[cameraSerialNum]->getCameraStatus()){
        if(cameraMap[cameraSerialNum]->grabOneImage(grabMat))
            return true;
    }else{
        ///// try to reconnect camera if camera is offline //////////////
        if(cameraMap[cameraSerialNum]->connectCamera(cameraSerialNum)){
            if(cameraMap[cameraSerialNum]->grabOneImage(grabMat))
                return true;
        }
    }
    return false;
}

void CameraSettingsWidget::setCameraTabIndex(std::string*cameraSerialNum)
{
    if(cameraMap.find(*cameraSerialNum)!=cameraMap.end()){
        if(indexCameraSerialNumMap.find(*cameraSerialNum)!=indexCameraSerialNumMap.end()){
            ui->comboBox->setCurrentIndex(indexCameraSerialNumMap[*cameraSerialNum]);
        }
    }
    cameraSerialPtr=cameraSerialNum;
}

bool CameraSettingsWidget::getCameraStatus(const std::string &cameraSerialNum)
{
    if(cameraMap.find(cameraSerialNum)!=cameraMap.end())
        return cameraMap[cameraSerialNum]->getCameraStatus();
    return false;
}

void CameraSettingsWidget::closeEvent(QCloseEvent *event)
{
    auto iter=cameraMap.begin();
    for(;iter!=cameraMap.end();++iter){
        iter->second->cameraStopGrab();
    }
}

void CameraSettingsWidget::uiInitialize()
{
    comboxIndex=0;
    ui->textEdit->setReadOnly(true);
    ui->lineEdit_14->setReadOnly(true);
    ui->comboBox->clear();
    indexCameraSerialNumMap.clear();
    cameraSerialNumIndexMap.clear();
    if(cameraMap.size()==0)
    {
        ui->comboBox->setEnabled(false);
        ui->horizontalSlider->setEnabled(false);
        ui->pushButton->setEnabled(false);
        ui->pushButton_13->setEnabled(false);
        ui->pushButton_14->setEnabled(false);
        ui->pushButton_16->setEnabled(false);
        return;
    }else{
        ui->comboBox->setEnabled(true);
        ui->horizontalSlider->setEnabled(true);
        ui->pushButton->setEnabled(true);
        ui->pushButton_13->setEnabled(true);
        ui->pushButton_14->setEnabled(true);
        ui->pushButton_16->setEnabled(true);
    }
    auto iter=cameraMap.begin();
    int i=0;
    for(;iter!=cameraMap.end();++iter){
        if((iter->second)->getCameraStatus()){
            indexCameraSerialNumMap.insert(std::pair<std::string,int>(iter->first,i));
            cameraSerialNumIndexMap.insert(std::pair<int,std::string>(i,iter->first));
            ui->comboBox->addItem(QString("camera "+QString(iter->first.c_str())+":online"));
        }else{
            indexCameraSerialNumMap.insert(std::pair<std::string,int>(iter->first,i));
            cameraSerialNumIndexMap.insert(std::pair<int,std::string>(i,iter->first));
            ui->comboBox->addItem("camera "+QString(iter->first.c_str())+":offline");
        }
        ++i;
    }
    ui->lineEdit_14->setText(QString::number((int)cameraMap[cameraSerialNumIndexMap[0]]->getCameraExpose(),10));
    ui->horizontalSlider->setRange(0,64000);
    ui->horizontalSlider->setValue((int)cameraMap[cameraSerialNumIndexMap[0]]->getCameraExpose());
}

void CameraSettingsWidget::uiCameraChangeed(std::string cameraSerialNum)
{
    ui->lineEdit_14->setText(QString::number((int)cameraMap[cameraSerialNum]->getCameraExpose(),10));
    ui->horizontalSlider->setRange(0,64000);
    ui->horizontalSlider->setValue((int)cameraMap[cameraSerialNum]->getCameraExpose());
}

void CameraSettingsWidget::slot_cameraGrabbedImage(std::string cameraSerialNum,cv::Mat& grabbedMat)
{
    showMatOnDlg(grabbedMat,ui->label);
}

void CameraSettingsWidget::on_pushButton_clicked()
{
    if(cameraMap[cameraSerialNumIndexMap[comboxIndex]]->getCameraStatus()){
        cv::Mat grabMat;
        if(cameraMap[cameraSerialNumIndexMap[comboxIndex]]->grabOneImage(grabMat))
            showMatOnDlg(grabMat,ui->label);
        else{
            ui->textEdit->setTextColor(Qt::red);
            ui->textEdit->append(QTime::currentTime().toString());
            ui->textEdit->append(QString("failed to grab image!"));
        }
    }
}

void CameraSettingsWidget::on_pushButton_13_clicked()
{
    auto iter=cameraMap.begin();
    for(;iter!=cameraMap.end();++iter){
        iter->second->cameraStopGrab();
    }

    if(cameraMap[cameraSerialNumIndexMap[comboxIndex]]->getCameraStatus()){
        cameraMap[cameraSerialNumIndexMap[comboxIndex]]->cameraStartGrab();
    }

    ui->pushButton_13->setEnabled(false);
    ui->pushButton->setEnabled(false);
    ui->pushButton_14->setEnabled(true);
}

void CameraSettingsWidget::on_pushButton_14_clicked()
{
    if(cameraMap[cameraSerialNumIndexMap[comboxIndex]]->getCameraStatus()){
        cameraMap[cameraSerialNumIndexMap[comboxIndex]]->cameraStopGrab();
    }
    ui->pushButton_13->setEnabled(true);
    ui->pushButton_14->setEnabled(false);
    ui->pushButton->setEnabled(true);
}

void CameraSettingsWidget::slot_cameraOffLine(std::string cameraSerial)
{
    ui->textEdit->setTextColor(Qt::red);
    ui->textEdit->append(QTime::currentTime().toString()+": "+QString(cameraSerial.c_str())+QString(" offline!"));
    emit cameraStatusChanged(cameraSerial,false);
    cameraRefresh();
    if(cameraMap.size()==0){
        ui->textEdit->setTextColor(Qt::red);
        ui->textEdit->append(QTime::currentTime().toString());
        ui->textEdit->append(QString("Didn't detect any camera!"));
    }
    uiInitialize();
}

void CameraSettingsWidget::slot_cameraOnLine(std::string cameraSerial)
{
    ui->textEdit->setTextColor(Qt::blue);
    ui->textEdit->append(QTime::currentTime().toString()+": "+QString(cameraSerial.c_str())+QString(" online!"));
    emit cameraStatusChanged(cameraSerial,true);
}

bool CameraSettingsWidget::connectCamera(int cameraType,std::string cameraSerialNum)
{
    GigeCamera* cameraPtr=nullptr;
    switch(cameraType){
    case PYLON_GIGE_CAMERA:{
        cameraPtr =new pylonGigeCamera;
    }break;
    case DAHENG_GIGE_CAMERA:{
        cameraPtr =new DaHengGigeCamera;
    }break;
    default:
        break;
    }
    if(cameraPtr==nullptr)return false;
    connect(cameraPtr,SIGNAL(signal_grabbedImage(std::string,cv::Mat&)),this,SLOT(slot_cameraGrabbedImage(std::string,cv::Mat&)));
    connect(cameraPtr,SIGNAL(signal_cameraRemoved(std::string)),this,SLOT(slot_cameraOffLine(std::string)));
    connect(cameraPtr,SIGNAL(signal_cameraOpened(std::string)),this,SLOT(slot_cameraOnLine(std::string)));
    if(cameraPtr->connectCamera(cameraSerialNum))
    {
        cameraMap.insert(std::pair<std::string,GigeCamera*>(cameraSerialNum,cameraPtr));
        if(mapCameraExposeTimePtr->find(cameraSerialNum)!=mapCameraExposeTimePtr->end())
            cameraPtr->setCameraExpose((*mapCameraExposeTimePtr)[cameraSerialNum]);
        return true;
    }
    else
    {
        delete cameraPtr;
        return false;
    }
}

void CameraSettingsWidget::on_pushButton_16_clicked()
{
    //// stop grab before close dialog ///////////
    auto iter=cameraMap.begin();
    for(;iter!=cameraMap.end();++iter){
        iter->second->cameraStopGrab();
    }
    //// confirm the change of camera //////////////
    if(*cameraSerialPtr!=cameraSerialNumIndexMap[comboxIndex]){
        QMessageBox::StandardButton button;
        button = QMessageBox::question(this, tr("Caution"),
                                       QString(tr("重新选择了相机！是否保存?")),
                                       QMessageBox::Yes | QMessageBox::No);
        if(button == QMessageBox::Yes){
            *cameraSerialPtr=cameraSerialNumIndexMap[comboxIndex];
            emit cameraStatusChanged(*cameraSerialPtr,true);
        }
    }
    close();
}

void CameraSettingsWidget::on_pushButton_15_clicked()
{
    close();
}

void CameraSettingsWidget::on_comboBox_currentIndexChanged(int index)
{
    if(index<0){
        ui->lineEdit_14->setText("NULL");
        ui->horizontalSlider->setRange(0,0);
        ui->horizontalSlider->setValue(0);
        return;
    }
    comboxIndex=index;
    uiCameraChangeed(cameraSerialNumIndexMap[comboxIndex]);

    auto iter=cameraMap.begin();
    for(;iter!=cameraMap.end();++iter){
        iter->second->cameraStopGrab();
    }

    ui->pushButton_13->setEnabled(true);
    ui->pushButton_14->setEnabled(false);
}

void CameraSettingsWidget::on_pushButton_2_clicked()
{
    cameraRefresh();
    if(cameraMap.size()==0){
        ui->textEdit->setTextColor(Qt::red);
        ui->textEdit->append(QTime::currentTime().toString());
        ui->textEdit->append(QString("Didn't detect any camera!"));
    }
    uiInitialize();
}

void CameraSettingsWidget::on_horizontalSlider_sliderMoved(int position)
{
    if(position==0)return;
    cameraMap[cameraSerialNumIndexMap[comboxIndex]]->setCameraExpose(position);
    (*mapCameraExposeTimePtr)[cameraSerialNumIndexMap[comboxIndex]]=position;
    ui->lineEdit_14->setText(QString::number(position,10));
}
