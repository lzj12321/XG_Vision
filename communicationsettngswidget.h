#ifndef COMMUNICATIONSETTNGSWIDGET_H
#define COMMUNICATIONSETTNGSWIDGET_H

#include <QWidget>
#include<QLineEdit>
#include<QComboBox>
#include"fxplcsocketcommunicate.h"
#include"qplcsocketcommunicate.h"
#include"yaskawarobotsocketcommunicate.h"
#include"standardsocketcommunicate.h"
#include<QTcpServer>

#define FX_PLC 0
#define Q_PLC 1
#define YASKAWA_ROBOT 2
#define STANDARD_SOCKET 3

namespace Ui {
class CommunicationSettngsWidget;
}

class CommunicationSettngsWidget : public QWidget
{
    Q_OBJECT
public:
    explicit CommunicationSettngsWidget(QWidget *parent = 0);
    virtual ~CommunicationSettngsWidget();
    void setParamPatch(QString path);
    void communicateWidgetIni(int index);
    void setResultFormat(STR_Vision_Result&);
    bool sendResult(STR_Vision_Result);
    bool getCommunicateStatus();
private slots:
    void on_pushButton_clicked();
    void on_pushButton_3_clicked();
    void on_pushButton_2_clicked();
    void slot_connectStatusChanged(QAbstractSocket::SocketState);
    void on_pushButton_5_clicked();
private:
    int serialNum;
    Ui::CommunicationSettngsWidget *ui;

private:
    void uiInitialize();
    void resultComboxIni(QComboBox*);
    void setIpInputReg(QLineEdit*);
    void socketInitialize();
    bool connectServer(QHostAddress ip,uint port);
    void disConnectServer();
    void connectedUiInitialize();
    void disconnectUiInitialize();
    void paramLoad();
    void choosedResultLoad();
    void paramSave();
    void choosedResultSave();
    void resultFilter(STR_Vision_Result&result);
    void addResultCombox(QString text="null",int addr=-1);
    void getUiChoosedResultStr();
    void judgeResult(QString &str,int addr,STR_Vision_Result&str_filterResult);
private:
    STR_Vision_Result str_filterResult;
    STR_Vision_Result str_sendResult;
    int communicateType;
    SocketCommunicate* socketPtr;
    QString paramPath;
    QHostAddress ip;
    uint port;
    std::vector<QComboBox*> vecCombox;
    int combox_X=390;
    int combox_Y=280;
    int combox_W=91;
    int combox_H=22;
    int lineEdit_X=550;
    int lineEdit_Y=280;
    int heightDiff=50;
    std::vector<QLineEdit*> vecLineEdit;
    QTcpSocket* clientPtr;
private slots:
    void slot_VisionRun(int);
    void on_pushButton_6_clicked();
    void on_pushButton_7_clicked();
signals:
    void signal_VisionRun(int,int);
    void signal_communicateStatusChanged(int,bool);
};

#endif // COMMUNICATIONSETTNGSWIDGET_H

