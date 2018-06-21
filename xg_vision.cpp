#include "xg_vision.h"
#include "ui_xg_vision.h"
#include<QMessageBox>
#include<QDebug>
#include <QFileDialog>
#include <QTextCodec>
#include<QMessageBox>
#include"calibration.h"
#include <QTextCodec>
#include<QSettings>
#include <QMetaType>
#include<QTextStream>
#include<QTime>

XG_Vision::XG_Vision(QWidget *parent):
    QMainWindow(parent),
    ui(new Ui::XG_Vision)
{
    ui->setupUi(this);

    qRegisterMetaType<std::string>("std::string&");
    qRegisterMetaType<std::string>("std::string");

    qRegisterMetaType<cv::Mat>("cv::Mat&");
    qRegisterMetaType<cv::Mat>("cv::Mat");

    paramInitialize();
    uiInitialize();
    cameraInitialize(&mapCameraExposeTime);
    recordWidget.recordWidgetIni(&vecVisionParam);
    ui->tabWidget->setCurrentIndex(ui->tabWidget->count()-1);
    connect(ui->menuBar,SIGNAL(triggered(QAction*)),this,SLOT(slot_menuAction(QAction*)));
}

XG_Vision::~XG_Vision()
{
    for(int i=0;i<vecNccWidget.size();++i){
        delete vecNccWidget[i];
        delete vecCalibrateWidget[i];
        delete vecCommWidget[i];
    }
    delete aboutDailog;
    delete ui;
}

void XG_Vision::closeEvent(QCloseEvent *event)
{
    for(int i=0;i<vecVisionParam.size();++i){
        vecVisionParam[i].cameraExposeTime=mapCameraExposeTime[vecVisionParam[i].cameraSerialNum];
    }
    paramSave();
    cameraWidget.close();
    recordWidget.close();
    for(int i=0;i<stationNum;++i){
        vecNccWidget[i]->close();
        vecCalibrateWidget[i]->close();
        vecCommWidget[i]->close();
    }
}

void XG_Vision::uiInitialize()
{
    aboutDailog=new AboutDialog(this);
    for(int i=0;i<vecVisionParam.size();++i){
        addStation(vecVisionParam[i].stationName);
    }
}

void XG_Vision::cameraInitialize(std::map<std::string,int>*mapPtr)
{
    connect(&cameraWidget,SIGNAL(cameraStatusChanged(std::string,bool)),this,SLOT(slot_cameraStatusChanged(std::string,bool)));
    cameraWidget.cameraWidgetInitialize(mapPtr);
}

void XG_Vision::addStation(QString&stationName)
{
    cv::Mat t;
    vecSourceMat.push_back(t);

    nccMatchWidget* nw=new nccMatchWidget;
    vecNccWidget.push_back(nw);
    calibration* cw=new calibration;
    vecCalibrateWidget.push_back(cw);
    CommunicationSettngsWidget* commw=new CommunicationSettngsWidget;
    commw->setResultFormat(nw->getNccMatchResultFormat());
    vecCommWidget.push_back(commw);

    QString path=QCoreApplication::applicationDirPath()+"/settings/"+stationName+"Calibration"+".yml";
    cw->calibrateInitialize(path);

    nw->setParamPath(QCoreApplication::applicationDirPath()+"/settings/"+stationName);
    nw->nccInitialize(0);

    commw->setParamPatch(QCoreApplication::applicationDirPath()+"/settings/"+stationName);
    commw->communicateWidgetIni(vecCommWidget.size());
    connect(commw,SIGNAL(signal_VisionRun(int,int)),this,SLOT(slot_VisionRun(int,int)));
    connect(commw,SIGNAL(signal_communicateStatusChanged(int,bool)),this,SLOT(slot_communicateStatusChanged(int,bool)));

    QWidget* widget=new QWidget(this);
    QLabel* label=new QLabel(widget);

    label->setGeometry(10,10,1101,781);
    label->setFrameStyle(QFrame::Panel);
    label->setFrameShadow(QFrame::Sunken);
    label->setLineWidth(5);
    label->setAlignment(Qt::AlignCenter);
    vecTabLabelPtr.push_back(label);
    ui->tabWidget->addTab(widget,stationName);
}

void XG_Vision::paramInitialize()
{
    paramLoad();
}

void XG_Vision::paramLoad()
{
    QSettings paramRead(QCoreApplication::applicationDirPath()+"/settings/xg_vision.ini",QSettings::IniFormat);
    stationNum=paramRead.value("StationNum/num").toInt();
    for(int i=0;i<stationNum;++i){
        strVisionParam param;
        param.stationName=paramRead.value("Station"+QString::number(i,10)+"/stationName").toString();
        param.isEnableCalibrate=paramRead.value("Station"+QString::number(i,10)+"/isEnableCalibrate").toBool();
        param.cameraSerialNum=paramRead.value("Station"+QString::number(i,10)+"/cameraSerialNum").toString().toStdString();
        param.cameraExposeTime=paramRead.value("Station"+QString::number(i,10)+"/cameraExposeTime").toInt();
        param.isSaveRecord=paramRead.value("Station"+QString::number(i,10)+"/isSaveRecord").toBool();
        param.maxImgSaveNum=paramRead.value("Station"+QString::number(i,10)+"/maxImgSaveNum").toInt();
        mapCameraExposeTime.insert(std::pair<std::string,int>(param.cameraSerialNum,param.cameraExposeTime));
        vecVisionParam.push_back(param);
    }
}

void XG_Vision::paramSave()
{
    QSettings paramWrite(QCoreApplication::applicationDirPath()+"/settings/xg_vision.ini",QSettings::IniFormat);
    paramWrite.setValue("StationNum/num",stationNum);
    for(int i=0;i<vecVisionParam.size();++i){
        paramWrite.setValue("Station"+QString::number(i,10)+"/stationName",vecVisionParam[i].stationName);
        paramWrite.setValue("Station"+QString::number(i,10)+"/isEnableCalibrate",vecVisionParam[i].isEnableCalibrate);
        paramWrite.setValue("Station"+QString::number(i,10)+"/cameraSerialNum",QString(vecVisionParam[i].cameraSerialNum.c_str()));
        paramWrite.setValue("Station"+QString::number(i,10)+"/cameraExposeTime",vecVisionParam[i].cameraExposeTime);
        paramWrite.setValue("Station"+QString::number(i,10)+"/isSaveRecord",vecVisionParam[i].isSaveRecord);
        paramWrite.setValue("Station"+QString::number(i,10)+"/maxImgSaveNum",vecVisionParam[i].maxImgSaveNum);
    }
}

void XG_Vision::createNewStation(QString&newStationName)
{
    strVisionParam newStationParam;
    newStationParam.stationName=newStationName;
    vecVisionParam.push_back(newStationParam);
    addStation(newStationName);
    ++stationNum;
}

void XG_Vision::delAStation(int index)
{
    auto iter0=vecVisionParam.begin();
    vecVisionParam.erase(iter0+index);

    auto iter1=vecSourceMat.begin();
    vecSourceMat.erase(iter1+index);

    auto iter3=vecNccWidget.begin();
    delete vecNccWidget[index];
    vecNccWidget.erase(iter3+index);

    auto iter4=vecCalibrateWidget.begin();
    delete vecCalibrateWidget[index];
    vecCalibrateWidget.erase(iter4+index);

    auto iter5=vecCommWidget.begin();
    delete vecCommWidget[index];
    vecCommWidget.erase(iter5+index);

    auto iter6=vecTabLabelPtr.begin();
    delete vecTabLabelPtr[index];
    vecTabLabelPtr.erase(iter6+index);

    --stationNum;
}

void XG_Vision::resultOutPut(const STR_Vision_Result &result)
{
    if(recordNum>200){
        ui->textEdit->clear();
        recordNum=0;
    }
    ++recordNum;
    ui->textEdit->setTextBackgroundColor(Qt::white);
    if(result.matchStatus){
        ui->textEdit->setTextColor(Qt::blue);
        ui->textEdit->append(QTime::currentTime().toString()+" Match Result");
        ui->textEdit->append(QString("Station:")+QString::number(result.runStation,10)+QString(" Model:")+QString::number(result.runModel,10));
        ui->textEdit->append(QString("match socore:%2").arg(result.socore));
        ui->textEdit->append(QString("coordinate(%2").arg(result.x)+QString(",%2").arg(result.y)+QString(")"));
        ui->textEdit->append(QString("angle:%2").arg(result.angle));
        ui->textEdit->append(QString("used time:"+QString::number(matchUsedTime,10)));
    }else{
        ui->textEdit->setTextColor(Qt::red);
        ui->textEdit->append(QTime::currentTime().toString()+" Match Result");
        ui->textEdit->append(QString("Station:")+QString::number(result.runStation,10)+QString(" Model:")+QString::number(result.runModel,10));
        ui->textEdit->append(QString("no match point!"));
        ui->textEdit->append(QString("used time:"+QString::number(matchUsedTime,10)));
    }
    ui->textEdit->setTextBackgroundColor(Qt::gray);
    ui->textEdit->append("..................");
}

void XG_Vision::displayRunSignal(int station, int model)
{
    ui->textEdit->setTextBackgroundColor(Qt::white);
    ui->textEdit->setTextColor(Qt::green);
    ui->textEdit->append(QTime::currentTime().toString()+" Receive Run Signal");
    ui->textEdit->append(QString("Station:")+QString::number(station,10)+QString(" Model:")+QString::number(model,10));
    ui->textEdit->setTextBackgroundColor(Qt::gray);
    ui->textEdit->append("..................");
}

void XG_Vision::cameraStatusChanged(bool status)
{
    if(status){
        ui->pushButton_3->setText(QString("Connected"));
        ui->pushButton_3->setStyleSheet("color:blue;");
    }else{
        ui->pushButton_3->setText(QString("DisConnected"));
        ui->pushButton_3->setStyleSheet("color:red;");
    }
}

void XG_Vision::communicateStatusChanged(bool status)
{
    if(status){
        ui->pushButton_7->setText(QString("Connected"));
        ui->pushButton_7->setStyleSheet("color:blue;");
    }else{
        ui->pushButton_7->setText(QString("DisConnected"));
        ui->pushButton_7->setStyleSheet("color:red;");
    }
}

void XG_Vision::on_pushButton_clicked()
{
    vecNccWidget[tabIndex]->nccWidgetIni(vecSourceMat[tabIndex]);
    vecNccWidget[tabIndex]->show();
}

void XG_Vision::on_pushButton_2_clicked()
{
    QMessageBox::StandardButton button;
    button = QMessageBox::question(this, tr("Caution"),
                                   QString(tr("Yes:打开标定功能 No:关闭标定功能")),
                                   QMessageBox::Yes | QMessageBox::No);
    if(button == QMessageBox::Yes){
        vecVisionParam[tabIndex].isEnableCalibrate=true;
        vecCalibrateWidget[tabIndex]->show();
    }else{
        vecVisionParam[tabIndex].isEnableCalibrate=false;
    }
}

void XG_Vision::on_tabWidget_currentChanged(int index)
{
    tabIndex=index;
    cameraStatusChanged(cameraWidget.getCameraStatus(vecVisionParam[tabIndex].cameraSerialNum));
    communicateStatusChanged(vecCommWidget[index]->getCommunicateStatus());
}

void XG_Vision::slot_menuAction(QAction*act)
{
    if(act->text()=="Add a new station"){
        bool isOK;
        QString stationName = QInputDialog::getText(NULL, "Input Dialog",
                                                    "Please input your station name",
                                                    QLineEdit::Normal,
                                                    "Your new station name",
                                                    &isOK);
        if(isOK){
            QMessageBox::StandardButton button;
            button = QMessageBox::question(this, "Caution",
                                           QString("new station name "+stationName+"?"),
                                           QMessageBox::Yes | QMessageBox::No);
            if(button == QMessageBox::Yes){
                if(stationName.isEmpty())
                {
                    QMessageBox msgBox;
                    msgBox.setText("station name cant be empty!");
                    msgBox.exec();
                    return;
                }
                createNewStation(stationName);
                ui->tabWidget->setCurrentIndex(ui->tabWidget->count()-1);
            }
        }
    }
    if(act->text()=="About"){
        aboutDailog->show();
    }
    if(act->text()=="Del current station"){
        QMessageBox::StandardButton button;
        button = QMessageBox::question(this, "Caution",
                                       QString("Are you sure to delete this station("+ui->tabWidget->tabText(tabIndex)+")!"),
                                       QMessageBox::Yes | QMessageBox::No);
        if(button == QMessageBox::Yes){
            int index=tabIndex;
            ui->tabWidget->removeTab(index);
            delAStation(index);
        }
    }
    if(act->text()=="Camera Settings"){
        cameraWidget.setCameraTabIndex(&vecVisionParam[tabIndex].cameraSerialNum);
        cameraWidget.show();
    }
    if(act->text()=="Match Settings"){
        vecNccWidget[tabIndex]->nccWidgetIni(vecSourceMat[tabIndex]);
        vecNccWidget[tabIndex]->show();
    }
    if(act->text()=="Communicate Settings"){
        vecCommWidget[tabIndex]->show();
    }
    if(act->text()=="Calibration Settings"){
        QMessageBox::StandardButton button;
        button = QMessageBox::question(this, tr("Caution"),
                                       QString(tr("Yes:打开标定功能 No:关闭标定功能")),
                                       QMessageBox::Yes | QMessageBox::No);
        if(button == QMessageBox::Yes){
            vecVisionParam[tabIndex].isEnableCalibrate=true;
            vecCalibrateWidget[tabIndex]->show();
        }else{
            vecVisionParam[tabIndex].isEnableCalibrate=false;
        }
    }
    if(act->text()=="Open And Process Image"){
        STR_Vision_Result strMatchResult;
        QString filename = QFileDialog::getOpenFileName(this,tr("Open Image"),".",tr("Image File (*.jpg *.png *.bmp)"));
        QTextCodec *code = QTextCodec::codecForName("gb18030");
        std::string name = code->fromUnicode(filename).data();
        if(!name.empty())
        {
            cv::Mat src=cv::imread(name,CV_LOAD_IMAGE_GRAYSCALE);
            if(!src.empty())
            {
                showMatOnDlg(src,vecTabLabelPtr[tabIndex]);
                vecSourceMat[tabIndex]=src.clone();
            }
            else
            {
                QMessageBox msgBox;
                msgBox.setText("Image Data Is Null");
                msgBox.exec();
                return;
            }
        }
        QTime timer;
        timer.start();
        if(vecSourceMat[tabIndex].empty())return;
        strMatchResult=vecNccWidget[tabIndex]->ncc_Match(vecSourceMat[tabIndex],vecNccWidget[tabIndex]->currentMatchModel+1);
        if(strMatchResult.matchStatus){
            if(vecVisionParam[tabIndex].isEnableCalibrate){
                if(vecCalibrateWidget[tabIndex]->getCalibrateStatus())
                    vecCalibrateWidget[tabIndex]->computePoint(strMatchResult);
                else{
                    QMessageBox msgBox;
                    msgBox.setText("calibrate initialize error!");
                    msgBox.exec();
                    strMatchResult.errorFlag=CALIBRATE_ERROR;
                }
            }
        }else
            strMatchResult.errorFlag=NCC_MATCH_ERROR;

        vecCommWidget[tabIndex]->sendResult(strMatchResult);
        matchUsedTime=timer.elapsed();
        cv::Mat n=vecNccWidget[tabIndex]->getResultImg(vecSourceMat[tabIndex]);
        showMatOnDlg(n,vecTabLabelPtr[tabIndex]);
        resultOutPut(strMatchResult);
        recordWidget.recordSave(vecVisionParam[tabIndex].stationName,vecSourceMat[tabIndex],strMatchResult);
    }
    if(act->text()=="Grab And Process Image"){
        QTime timer;
        timer.start();
        STR_Vision_Result strMatchResult;
        if(cameraWidget.grabOneImg(vecVisionParam[tabIndex].cameraSerialNum,vecSourceMat[tabIndex])){
            strMatchResult=vecNccWidget[tabIndex]->ncc_Match(vecSourceMat[tabIndex],vecNccWidget[tabIndex]->currentMatchModel+1);
            if(strMatchResult.matchStatus){
                if(vecVisionParam[tabIndex].isEnableCalibrate){
                    if(vecCalibrateWidget[tabIndex]->getCalibrateStatus())
                        vecCalibrateWidget[tabIndex]->computePoint(strMatchResult);
                    else{
                        QMessageBox msgBox;
                        msgBox.setText("calibrate initialize error!");
                        msgBox.exec();
                        strMatchResult.errorFlag=CALIBRATE_ERROR;
                    }
                }
            }else
                strMatchResult.errorFlag=NCC_MATCH_ERROR;
        }else{
            strMatchResult.errorFlag=CAMERA_ERROR;
            cv::Mat t=cv::imread(QString(QCoreApplication::applicationDirPath()+"/settings/cameraOffline.jpg").toStdString(),CV_LOAD_IMAGE_GRAYSCALE);
            showMatOnDlg(t,vecTabLabelPtr[tabIndex]);
        }
        vecCommWidget[tabIndex]->sendResult(strMatchResult);
        matchUsedTime=timer.elapsed();
        cv::Mat n=vecNccWidget[tabIndex]->getResultImg(vecSourceMat[tabIndex]);
        showMatOnDlg(n,vecTabLabelPtr[tabIndex]);
        resultOutPut(strMatchResult);
        recordWidget.recordSave(vecVisionParam[tabIndex].stationName,vecSourceMat[tabIndex],strMatchResult);
    }
    if(act->text()=="Record Settings"){
        recordWidget.recordWidgetIni(&vecVisionParam,ui->tabWidget->tabText(tabIndex));
        recordWidget.show();
    }
}

void XG_Vision::on_pushButton_4_clicked()
{
    std::string savePath=QTime::currentTime().toString("hh.mm.ss").toStdString();
    std::cout<<"save path:"<<savePath<<std::endl;
    savePath+=".bmp";
    if(!vecSourceMat[tabIndex].empty()){
        if(vecSourceMat[tabIndex].type()!=CV_8UC1){
            cv::cvtColor(vecSourceMat[tabIndex],vecSourceMat[tabIndex],CV_RGB2GRAY);
        }
        cv::imwrite(savePath,vecSourceMat[tabIndex]);
    }
}

void XG_Vision::on_pushButton_5_clicked()
{
    showMatOnDlg(vecSourceMat[tabIndex],vecTabLabelPtr[tabIndex]);
    QTime timer;
    timer.start();
    if(vecSourceMat[tabIndex].empty())return;
    STR_Vision_Result strMatchResult;
    strMatchResult.errorFlag=NON_ERROR;
    strMatchResult=vecNccWidget[tabIndex]->ncc_Match(vecSourceMat[tabIndex],vecNccWidget[tabIndex]->currentMatchModel+1);
    if(strMatchResult.matchStatus){
        if(vecVisionParam[tabIndex].isEnableCalibrate){
            if(vecCalibrateWidget[tabIndex]->getCalibrateStatus())
                vecCalibrateWidget[tabIndex]->computePoint(strMatchResult);
            else{
                QMessageBox msgBox;
                msgBox.setText("calibrate initialize error!");
                msgBox.exec();
                strMatchResult.errorFlag=CALIBRATE_ERROR;
            }
        }
    }else
        strMatchResult.errorFlag=NCC_MATCH_ERROR;
    vecCommWidget[tabIndex]->sendResult(strMatchResult);
    matchUsedTime=timer.elapsed();
    cv::Mat n=vecNccWidget[tabIndex]->getResultImg(vecSourceMat[tabIndex]);
    showMatOnDlg(n,vecTabLabelPtr[tabIndex]);
    resultOutPut(strMatchResult);
    recordWidget.recordSave(vecVisionParam[tabIndex].stationName,vecSourceMat[tabIndex],strMatchResult);
}

void XG_Vision::on_pushButton_6_clicked()
{
    QString filename = QFileDialog::getOpenFileName(this,tr("Open Image"),".",tr("Image File (*.jpg *.png *.bmp)"));
    QTextCodec *code = QTextCodec::codecForName("gb18030");
    std::string name = code->fromUnicode(filename).data();
    if(!name.empty())
    {
        cv::Mat src=cv::imread(name,CV_LOAD_IMAGE_GRAYSCALE);
        if(!src.empty())
        {
            showMatOnDlg(src,vecTabLabelPtr[tabIndex]);
            vecSourceMat[tabIndex]=src.clone();
        }
        else
        {
            QMessageBox msgBox;
            msgBox.setText("Image Data Is Null");
            msgBox.exec();
        }
    }
}

void XG_Vision::slot_VisionRun(int station,int model)
{
    STR_Vision_Result strMatchResult;
    displayRunSignal(station,model);
    if(station>vecVisionParam.size()||station<=0){
        strMatchResult.errorFlag=STATION_OR_MODEL_ERROR;
        vecCommWidget[0]->sendResult(strMatchResult);
        return;
    }
    --station;
    QTime timer;
    timer.start();
    strMatchResult.errorFlag=NON_ERROR;
    strMatchResult.model=model;

    if(cameraWidget.grabOneImg(vecVisionParam[station].cameraSerialNum,vecSourceMat[station])){
        //vecSourceMat[station]=cv::imread("C:/Users/lzj/Documents/build-XG_Vision-Desktop_Qt_5_8_0_MSVC2015_64bit-Release/mengjie/123.bmp",0);
        strMatchResult=vecNccWidget[station]->ncc_Match(vecSourceMat[station],model);
        if(strMatchResult.matchStatus){
            if(vecVisionParam[station].isEnableCalibrate){
                if(vecCalibrateWidget[station]->getCalibrateStatus())
                    vecCalibrateWidget[station]->computePoint(strMatchResult);
                else{
                    QMessageBox msgBox;
                    msgBox.setText("calibrate initialize error!");
                    msgBox.exec();
                    strMatchResult.errorFlag=CALIBRATE_ERROR;
                }
            }
        }
        else if(strMatchResult.errorFlag!=STATION_OR_MODEL_ERROR)
        {
            strMatchResult.errorFlag=NCC_MATCH_ERROR;
        }
        cv::Mat n=vecNccWidget[station]->getResultImg(vecSourceMat[station]);
        showMatOnDlg(n,vecTabLabelPtr[station]);
    }else{
        strMatchResult.errorFlag=CAMERA_ERROR;
        cv::Mat t=cv::imread(QString(QCoreApplication::applicationDirPath()+"/settings/cameraOffline.jpg").toStdString(),CV_LOAD_IMAGE_GRAYSCALE);
        showMatOnDlg(t,vecTabLabelPtr[station]);
    }
    vecCommWidget[station]->sendResult(strMatchResult);
    matchUsedTime=timer.elapsed();
    strMatchResult.runModel=model;
    strMatchResult.runStation=station+1;
    recordWidget.recordSave(vecVisionParam[station].stationName,vecSourceMat[station],strMatchResult);
    resultOutPut(strMatchResult);
}

void XG_Vision::slot_cameraStatusChanged(std::string cameraSerialNum, bool status)
{
    QString stationName;
    for(int i=0;i<vecVisionParam.size();++i){
        if(vecVisionParam[i].cameraSerialNum==cameraSerialNum){
            stationName=vecVisionParam[i].stationName;
            break;
        }
    }
    for(int i=0;i<ui->tabWidget->count();++i){
        if(ui->tabWidget->tabText(i)==stationName){
            ui->tabWidget->setCurrentIndex(i);
            cameraStatusChanged(status);
            if(status){
                ui->textEdit->setTextBackgroundColor(Qt::white);
                ui->textEdit->setTextColor(Qt::blue);
                ui->textEdit->append(stationName+" "+QTime::currentTime().toString()+" Camera Status Changed");
                ui->textEdit->append(QString("camera:")+QString::fromStdString(cameraSerialNum)+QString(" online!"));
                ui->textEdit->setTextBackgroundColor(Qt::gray);
                ui->textEdit->append("..................");
            }
            else{
                ui->textEdit->setTextBackgroundColor(Qt::white);
                ui->textEdit->setTextColor(Qt::red);
                ui->textEdit->append(stationName+" "+QTime::currentTime().toString()+" Camera Status Changed");
                ui->textEdit->append(QString("camera:")+QString::fromStdString(cameraSerialNum)+QString(" offline!"));
                ui->textEdit->setTextBackgroundColor(Qt::gray);
                ui->textEdit->append("..................");
            }
            break;
        }
    }
}

void XG_Vision::slot_communicateStatusChanged(int index, bool status)
{
    ui->tabWidget->setCurrentIndex(index);
    communicateStatusChanged(status);
    if(status){
        ui->textEdit->setTextBackgroundColor(Qt::white);
        ui->textEdit->setTextColor(Qt::blue);
        ui->textEdit->append(vecVisionParam[index].stationName+" "+QTime::currentTime().toString()+" Communicate Status Changed");
        ui->textEdit->append(QString("communicate online!"));
        ui->textEdit->setTextBackgroundColor(Qt::gray);
        ui->textEdit->append("..................");
    }
    else{
        ui->textEdit->setTextBackgroundColor(Qt::white);
        ui->textEdit->setTextColor(Qt::red);
        ui->textEdit->append(vecVisionParam[index].stationName+" "+QTime::currentTime().toString()+" Communicate Status Changed");
        ui->textEdit->append(QString("communicate offline!"));
        ui->textEdit->setTextBackgroundColor(Qt::gray);
        ui->textEdit->append("..................");
    }
}
