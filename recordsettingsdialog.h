#ifndef RECORDSETTINGSDIALOG_H
#define RECORDSETTINGSDIALOG_H

#include <QDialog>
#include"vision.h"
#include "opencv2/core/core.hpp"

namespace Ui {
class RecordSettingsDialog;
}

class RecordSettingsDialog : public QDialog
{
    Q_OBJECT

public:
    explicit RecordSettingsDialog(QWidget *parent = 0);
    ~RecordSettingsDialog();
    void recordWidgetIni(std::vector<strVisionParam>*);
    void recordWidgetIni(std::vector<strVisionParam>*,QString& stationName);
    void recordSave(QString& stationName,cv::Mat&,STR_Vision_Result&);
private slots:
    void on_pushButton_3_clicked();
    void on_comboBox_currentIndexChanged(int index);
    void on_checkBox_clicked(bool checked);
    void on_pushButton_clicked();
    void on_pushButton_2_clicked();
    void on_lineEdit_textChanged(const QString &arg1);
    void on_pushButton_4_clicked();

private:
    Ui::RecordSettingsDialog *ui;
    std::vector<strVisionParam>* vecParamPtr;
    void imgRecordSave(QString& stationName,cv::Mat&mat);
    void dataRecordSave(QString& stationName,STR_Vision_Result&);
};

#endif // RECORDSETTINGSDIALOG_H
