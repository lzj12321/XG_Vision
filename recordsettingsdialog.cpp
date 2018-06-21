#include "recordsettingsdialog.h"
#include "ui_recordsettingsdialog.h"
#include<QProcess>
#include<QDir>
#include<opencv2/highgui.hpp>
#include<QTime>
#include<QDateTime>
#include<QTextStream>
#include<QMessageBox>
#include<iostream>

RecordSettingsDialog::RecordSettingsDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::RecordSettingsDialog)
{
    ui->setupUi(this);
}

RecordSettingsDialog::~RecordSettingsDialog()
{
    delete ui;
}

void RecordSettingsDialog::recordWidgetIni(std::vector<strVisionParam> *visionParamPtr)
{
    ui->comboBox->clear();
    vecParamPtr=visionParamPtr;
}

void RecordSettingsDialog::recordWidgetIni(std::vector<strVisionParam>*visionParamPtr,QString& stationName)
{
    ui->comboBox->clear();
    vecParamPtr=visionParamPtr;
    auto iterx=(*visionParamPtr).begin();
    for(auto iter=(*visionParamPtr).begin();iter!=(*visionParamPtr).end();++iter){
        ui->comboBox->addItem((*iter).stationName);
        if((*iter).stationName==stationName)
            iterx=iter;
    }
    ui->checkBox->setChecked((*iterx).isSaveRecord);
    ui->comboBox->setCurrentText(stationName);
    ui->lineEdit->setText(QString::number((*iterx).maxImgSaveNum,10));
}

void RecordSettingsDialog::recordSave(QString& stationName,cv::Mat &mat, STR_Vision_Result &data)
{
    imgRecordSave(stationName,mat);
    dataRecordSave(stationName,data);
}

void RecordSettingsDialog::imgRecordSave(QString &stationName, cv::Mat &mat)
{
    if(!ui->checkBox->isChecked())return;
    if(mat.empty())return;
    QString imgRecordPath;
    imgRecordPath=QCoreApplication::applicationDirPath()+"/settings/"+stationName+"/RecordImg";
    QDir dir;
    if(!dir.exists(imgRecordPath)){
        if(dir.mkdir(imgRecordPath))
        {
            auto iterx=(*vecParamPtr).begin();
            for(;iterx!=(*vecParamPtr).end();++iterx){
                if((*iterx).stationName==stationName)
                {
                    dir.setPath(imgRecordPath);
                    if((dir.count()-2)<iterx->maxImgSaveNum){
                        std::string imgPath=(imgRecordPath+"/"+QDateTime::currentDateTime().toString("yyyy.MM.dd.hh.mm.ss")+".bmp").toStdString();
                        cv::imwrite(imgPath,mat);
                    }
                    break;
                }
            }
        }
    }else{
        dir.setPath(imgRecordPath);
        for(std::vector<strVisionParam>::iterator iterx=(*vecParamPtr).begin();iterx!=(*vecParamPtr).end();++iterx){
            if(iterx->stationName==stationName)
            {
                if((dir.count()-2)<iterx->maxImgSaveNum){
                    std::string imgPath=(imgRecordPath+"/"+QDateTime::currentDateTime().toString("yyyy.MM.dd.hh.mm.ss")+".bmp").toStdString();
                    cv::imwrite(imgPath,mat);
                }
                break;
            }
        }
    }
}

void RecordSettingsDialog::dataRecordSave(QString &stationName, STR_Vision_Result &data)
{
    QString dataRecordPath;
    dataRecordPath=QCoreApplication::applicationDirPath()+"/settings/"+stationName+"/RecordData";
    QDir dir;
    if(!dir.exists(dataRecordPath)){
        dir.mkdir(dataRecordPath);
    }
    QFile recordFile(dataRecordPath+"/"+QDateTime::currentDateTime().toString("yyyy.MM.dd")+".record.txt");
    if(recordFile.open(QFile::WriteOnly|QIODevice::Append)){
        QTextStream out(&recordFile);
        out<<"Run Time:"<<QTime::currentTime().toString()<<"\r\n";
        out<<"runStation:"<<data.runStation<<"\t";
        out<<"runModel:"<<data.runModel<<"\r\n";

        out<<"x:"<<data.x<<"\t";
        out<<"y:"<<data.y<<"\t";
        out<<"angle:"<<data.angle<<"\t";
        out<<"errorFlag:"<<data.errorFlag<<"\t";
        out<<"socore:"<<data.socore<<"\t";
        out<<"matchStatus:"<<data.matchStatus<<"\t";
        out<<"model:"<<data.model<<"\t";
        out<<"\r\n";
        QString sendData=QString::number(data.model,10)+","+QString::number((int)data.angle,10)+","+QString::number(data.errorFlag,10);
        out<<"sendData:"<<sendData<<"\r\n";
        out<<"\r\n";
    }
}

void RecordSettingsDialog::on_pushButton_3_clicked()
{
    QString recordPath;
    recordPath=QCoreApplication::applicationDirPath()+"/settings/"+ui->comboBox->currentText()+"/RecordImg";
    QDir dir;
    if(!dir.exists(recordPath)){
        dir.mkdir(recordPath);
    }
    recordPath.replace("/","\\");
    QProcess::startDetached("explorer "+recordPath);
}

void RecordSettingsDialog::on_comboBox_currentIndexChanged(int index)
{
    if(index==-1)return;
    ui->checkBox->setChecked((*vecParamPtr)[index].isSaveRecord);
    ui->lineEdit->setText(QString::number((*vecParamPtr)[index].maxImgSaveNum,10));
    QString imgRecordPath;
    imgRecordPath=QCoreApplication::applicationDirPath()+"/settings/"+ui->comboBox->currentText()+"/RecordImg";
    QDir dir;
    dir.setPath(imgRecordPath);
    int num=dir.count();
    if(num>0)num-=2;
    if(num>=(*vecParamPtr)[index].maxImgSaveNum){
        ui->lineEdit_2->setStyleSheet("color:red;");
        ui->lineEdit_2->setText("saved image num out range!");
    }else{
        ui->lineEdit_2->setStyleSheet("color:blue;");
        ui->lineEdit_2->setText(QString::number(num,10));
    }
}

void RecordSettingsDialog::on_checkBox_clicked(bool checked)
{
    (*vecParamPtr)[ui->comboBox->currentIndex()].isSaveRecord=checked;
}

void RecordSettingsDialog::on_pushButton_clicked()
{
    close();
}

void RecordSettingsDialog::on_pushButton_2_clicked()
{
    close();
}

void RecordSettingsDialog::on_lineEdit_textChanged(const QString &arg1)
{
    (*vecParamPtr)[ui->comboBox->currentIndex()].maxImgSaveNum=arg1.toInt();
}

void RecordSettingsDialog::on_pushButton_4_clicked()
{
    QString dataRecordPath;
    dataRecordPath=QCoreApplication::applicationDirPath()+"/settings/"+ui->comboBox->currentText()+"/RecordData"+"/"+QDateTime::currentDateTime().toString("yyyy.MM.dd")+".record.txt";
    QFile recordFile;
    recordFile.setFileName(dataRecordPath);
    if(!recordFile.exists())
    {
        QMessageBox msgBox;
        msgBox.setText("Record file didn't exist!");
        msgBox.exec();
        return;
    }
    QProcess* process = new QProcess(this);
    QString notepadPath = "notepad.exe "+dataRecordPath;
    process->start(notepadPath);
}
