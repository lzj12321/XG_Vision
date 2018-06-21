#include"communicationsettngswidget.h"
#include "ui_communicationsettngswidget.h"
#include<QDebug>
#include<QMessageBox>

typedef struct strComm{
    int model;
    QHostAddress ip;
    int port;
    int registerAddr;
}strComm;

CommunicationSettngsWidget::CommunicationSettngsWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::CommunicationSettngsWidget)
{
    ui->setupUi(this);
}

CommunicationSettngsWidget::~CommunicationSettngsWidget()
{
    delete ui;
}

void CommunicationSettngsWidget::communicateWidgetIni(int index)
{
    serialNum=index;
    paramLoad();
    socketInitialize();
    uiInitialize();
    connectServer(ip,port);
}

void CommunicationSettngsWidget::setResultFormat(STR_Vision_Result& tempResult)
{
    ///////set the communicate data format///////////////
    str_filterResult=tempResult;
}

bool CommunicationSettngsWidget::sendResult(STR_Vision_Result strResult)
{
    /////////filter the result to send///////////
    if(typeid(socketPtr).name()!=typeid(StandardSocketCommunicate*).name())
    {
        resultFilter(strResult);
    }
    if(socketPtr->getSocketConnectStatus())
        return socketPtr->sendData(strResult);
    else{
        if(connectServer(ip,port))
            return socketPtr->sendData(strResult);
        else
            return false;
    }
}

bool CommunicationSettngsWidget::getCommunicateStatus()
{
    return socketPtr->getSocketConnectStatus();
}

void CommunicationSettngsWidget::uiInitialize()
{
    ui->lineEdit_2->setReadOnly(true);
    ui->comboBox_4->addItem(QString("FX PLC"));
    ui->comboBox_4->addItem(QString("Q PLC"));
    ui->comboBox_4->addItem(QString("YASKAWA ROBOT"));
    ui->comboBox_4->addItem(QString("STANDARD SOCKET"));
    ui->comboBox_4->setCurrentIndex(communicateType);
    ui->lineEdit_6->setText(ip.toString());
    ui->lineEdit_4->setText(QString::number(port,10));
    setIpInputReg(ui->lineEdit_6);
}

void CommunicationSettngsWidget::resultComboxIni(QComboBox*comboxPtr)
{
    if(str_filterResult.x_flag)
        comboxPtr->addItem(QString("x"));
    if(str_filterResult.y_flag)
        comboxPtr->addItem(QString("y"));
    if(str_filterResult.z_flag)
        comboxPtr->addItem(QString("z"));
    if(str_filterResult.angle_flag)
        comboxPtr->addItem(QString("angle"));
    if(str_filterResult.u_flag)
        comboxPtr->addItem(QString("u"));
    if(str_filterResult.v_flag)
        comboxPtr->addItem(QString("v"));
    if(str_filterResult.errorFlag_flag)
        comboxPtr->addItem(QString("errorFlag"));
    if(str_filterResult.model_flag)
        comboxPtr->addItem(QString("model"));
}

void CommunicationSettngsWidget::setIpInputReg(QLineEdit *lineEditPtr)
{
    QRegExp rx("^((2[0-4]\\d|25[0-5]|[01]?\\d\\d?)\\.){3}(2[0-4]\\d|25[0-5]|[01]?\\d\\d?)$");
    QRegExpValidator* m_IPValidator = new QRegExpValidator(rx, this);
    lineEditPtr->setValidator(m_IPValidator);
}

void CommunicationSettngsWidget::socketInitialize()
{
    switch (communicateType){
    case FX_PLC:{
        socketPtr=new FxPlcSocketCommunicate;
    }break;
    case Q_PLC:{
        socketPtr=new QPlcSocketCommunicate;
    }break;
    case YASKAWA_ROBOT:{
        socketPtr=new YaskawaRobotSocketCommunicate;
    }break;
    case STANDARD_SOCKET:{
        socketPtr=new StandardSocketCommunicate;
    }break;
    default:
        break;
    }
    connect(socketPtr,SIGNAL(signal_socketStateChanged(QAbstractSocket::SocketState)),this,SLOT(slot_connectStatusChanged(QAbstractSocket::SocketState)));
    connect(socketPtr,SIGNAL(signal_VisionRun(int)),this,SLOT(slot_VisionRun(int)));
    connect(socketPtr,SIGNAL(signal_VisionRun(int,int)),this,SIGNAL(signal_VisionRun(int,int)));
}

bool CommunicationSettngsWidget::connectServer(QHostAddress ip,uint port)
{
    return socketPtr->connectServer(ip,port);
}

void CommunicationSettngsWidget::disConnectServer()
{
    socketPtr->disConnectServer();
}

void CommunicationSettngsWidget::connectedUiInitialize()
{
    ui->lineEdit_2->setStyleSheet("color:blue;");
}

void CommunicationSettngsWidget::disconnectUiInitialize()
{
    ui->lineEdit_2->setStyleSheet("color:red;");
}

void CommunicationSettngsWidget::paramLoad()
{
    QSettings paramRead(paramPath+"/communicateParam.ini",QSettings::IniFormat);
    ip=QHostAddress(paramRead.value("CommunicateParam/ip").toString());
    port=paramRead.value("CommunicateParam/port").toInt();
    communicateType=paramRead.value("CommunicateParam/communicateType").toInt();
    choosedResultLoad();
}

void CommunicationSettngsWidget::choosedResultLoad()
{
    QSettings paramRead(paramPath+"/communicateParam.ini",QSettings::IniFormat);
    bool x_flag=paramRead.value("STR_Vision_Result/x_flag").toBool();
    str_sendResult.x_flag=x_flag;
    if(x_flag){
        int addr=paramRead.value("STR_Vision_Result/x_addr").toInt();
        str_sendResult.x_addr=addr;
        addResultCombox("x",addr);
    }
    bool y_flag=paramRead.value("STR_Vision_Result/y_flag").toBool();
    str_sendResult.y_flag=y_flag;
    if(y_flag){
        int addr=paramRead.value("STR_Vision_Result/y_addr").toInt();
        str_sendResult.y_addr=addr;
        addResultCombox("y",addr);
    }
    bool z_flag=paramRead.value("STR_Vision_Result/z_flag").toBool();
    str_sendResult.z_flag=z_flag;
    if(z_flag){
        int addr=paramRead.value("STR_Vision_Result/z_addr").toInt();
        str_sendResult.z_addr=addr;
        addResultCombox("z",addr);
    }
    bool angle_flag=paramRead.value("STR_Vision_Result/angle_flag").toBool();
    str_sendResult.angle_flag=angle_flag;
    if(angle_flag){
        int addr=paramRead.value("STR_Vision_Result/angle_addr").toInt();
        str_sendResult.angle_addr=addr;
        addResultCombox("angle",addr);
    }
    bool model_flag=paramRead.value("STR_Vision_Result/model_flag").toBool();
    str_sendResult.model_flag=model_flag;
    if(model_flag){
        int addr=paramRead.value("STR_Vision_Result/model_addr").toInt();
        str_sendResult.model_addr=addr;
        addResultCombox("model",addr);
    }
    bool u_flag=paramRead.value("STR_Vision_Result/u_flag").toBool();
    str_sendResult.u_flag=u_flag;
    if(u_flag){
        int addr=paramRead.value("STR_Vision_Result/u_addr").toInt();
        addResultCombox("u",addr);
    }
    bool v_flag=paramRead.value("STR_Vision_Result/v_flag").toBool();
    str_sendResult.v_flag=v_flag;
    if(v_flag){
        int addr=paramRead.value("STR_Vision_Result/v_addr").toInt();
        addResultCombox("v",addr);
    }
    bool errorFlag_flag=paramRead.value("STR_Vision_Result/errorFlag_flag").toBool();
    str_sendResult.errorFlag_flag=errorFlag_flag;
    if(errorFlag_flag){
        int addr=paramRead.value("STR_Vision_Result/errorFlag_addr").toInt();
        str_sendResult.errorFlag_addr=addr;
        addResultCombox("errorFlag",addr);
    }
}

void CommunicationSettngsWidget::addResultCombox(QString text,int addr)
{
    if(vecCombox.size()>=5)return;
    QComboBox* qcomb=new QComboBox(this);
    resultComboxIni(qcomb);
    if(qcomb->findText(text)==-1){
        delete qcomb;
        return;
    }
    if(text!="null")
        qcomb->setCurrentText(text);
    qcomb->setGeometry(combox_X,combox_Y+heightDiff*vecCombox.size(),combox_W,combox_H);
    vecCombox.push_back(qcomb);
    qcomb->show();
    QLineEdit* qline=new QLineEdit(this);
    qline->setGeometry(lineEdit_X,combox_Y+heightDiff*vecLineEdit.size(),combox_W,combox_H);
    if(addr!=-1)
        qline->setText(QString::number(addr,10));
    vecLineEdit.push_back(qline);
    qline->show();
    ui->gridLayout->addWidget(qcomb);
    ui->gridLayout->addWidget(qline);
}

void CommunicationSettngsWidget::paramSave()
{
    QSettings paramWrite(paramPath+"/communicateParam.ini",QSettings::IniFormat);
    paramWrite.setValue("CommunicateParam/ip",ip.toString());
    paramWrite.setValue("CommunicateParam/port", port);
    paramWrite.setValue("CommunicateParam/communicateType",communicateType);
    choosedResultSave();
}

void CommunicationSettngsWidget::choosedResultSave()
{
    QSettings paramWrite(paramPath+"/communicateParam.ini",QSettings::IniFormat);
    paramWrite.setValue("STR_Vision_Result/x_flag",str_sendResult.x_flag);
    if(str_sendResult.x_flag){
        paramWrite.setValue("STR_Vision_Result/x_addr",str_sendResult.x_addr);
    }
    paramWrite.setValue("STR_Vision_Result/y_flag",str_sendResult.y_flag);
    if(str_sendResult.y_flag){
        paramWrite.setValue("STR_Vision_Result/y_addr",str_sendResult.y_addr);
    }
    paramWrite.setValue("STR_Vision_Result/z_flag",str_sendResult.z_flag);
    if(str_sendResult.z_flag){
        paramWrite.setValue("STR_Vision_Result/z_addr",str_sendResult.z_addr);
    }
    paramWrite.setValue("STR_Vision_Result/angle_flag",str_sendResult.angle_flag);
    if(str_sendResult.angle_flag){
        paramWrite.setValue("STR_Vision_Result/angle_addr",str_sendResult.angle_addr);
    }
    paramWrite.setValue("STR_Vision_Result/model_flag",str_sendResult.model_flag);
    if(str_sendResult.model_flag){
        paramWrite.setValue("STR_Vision_Result/model_addr",str_sendResult.model_addr);
    }
    paramWrite.setValue("STR_Vision_Result/errorFlag_flag",str_sendResult.errorFlag_flag);
    if(str_sendResult.errorFlag_flag){
        paramWrite.setValue("STR_Vision_Result/errorFlag_addr",str_sendResult.errorFlag_addr);
    }
    paramWrite.setValue("STR_Vision_Result/u_flag",str_sendResult.u_flag);
    paramWrite.setValue("STR_Vision_Result/v_flag",str_sendResult.v_flag);
}

void CommunicationSettngsWidget::resultFilter(STR_Vision_Result &result)
{
    result.x_addr=str_sendResult.x_addr;
    result.x_flag=str_sendResult.x_flag;
    result.y_addr=str_sendResult.y_addr;
    result.y_flag=str_sendResult.y_flag;
    result.z_addr=str_sendResult.z_addr;
    result.z_flag=str_sendResult.z_flag;

    result.angle_addr=str_sendResult.angle_addr;
    result.angle_flag=str_sendResult.angle_flag;
    result.model_addr=str_sendResult.model_addr;
    result.model_flag=str_sendResult.model_flag;
    result.errorFlag_addr=str_sendResult.errorFlag_addr;
    result.errorFlag_flag=str_sendResult.errorFlag_flag;
}

void CommunicationSettngsWidget::getUiChoosedResultStr()
{
    STR_Vision_Result tempResult;
    str_sendResult=tempResult;
    for(int i=0;i<vecCombox.size();++i){
        judgeResult(vecCombox[i]->currentText(),vecLineEdit[i]->text().toInt(),str_sendResult);
    }
}

void CommunicationSettngsWidget::judgeResult(QString &str,int addr,STR_Vision_Result&str_result)
{
    if(str=="x"){
        str_result.x_flag=true;
        str_result.x_addr=addr;
        //qDebug()<<"x:"<<addr;
    }
    else if(str=="y"){
        str_result.y_flag=true;
        //qDebug()<<"y:"<<addr;
        str_result.y_addr=addr;
    }
    else if(str=="z"){
        str_result.z_flag=true;
        //qDebug()<<"z:"<<addr;
        str_result.z_addr=addr;
    }
    else if(str=="angle"){
        str_result.angle_flag=true;
        //qDebug()<<"angle:"<<addr;
        str_result.angle_addr=addr;
    }
    else if(str=="model"){
        str_result.model_flag=true;
        //qDebug()<<"model:"<<addr;
        str_result.model_addr=addr;
    }
    else if(str=="errorFlag"){
        str_result.errorFlag_flag=true;
        //qDebug()<<"errorFlag:"<<addr;
        str_result.errorFlag_addr=addr;
    }
    else if(str=="u"){
        str_result.u_flag=true;
        //qDebug()<<"u:"<<addr;
    }
    else if(str=="v"){
        str_result.v_flag=true;
        //qDebug()<<"v:"<<addr;
    }
}

void CommunicationSettngsWidget::slot_VisionRun(int model)
{
    emit signal_VisionRun(serialNum,model);
}

void CommunicationSettngsWidget::setParamPatch(QString path)
{
    paramPath=path;
}

void CommunicationSettngsWidget::on_pushButton_clicked()
{
    if(vecCombox.size()>=5)return;
    QComboBox* qcomb=new QComboBox(this);
    resultComboxIni(qcomb);
    qcomb->setGeometry(combox_X,combox_Y+heightDiff*vecCombox.size(),combox_W,combox_H);
    vecCombox.push_back(qcomb);
    qcomb->show();
    QLineEdit* qline=new QLineEdit(this);
    qline->setGeometry(lineEdit_X,combox_Y+heightDiff*vecLineEdit.size(),combox_W,combox_H);
    vecLineEdit.push_back(qline);
    qline->show();
    ui->gridLayout->addWidget(qcomb);
    ui->gridLayout->addWidget(qline);
}

void CommunicationSettngsWidget::on_pushButton_3_clicked()
{
    close();
}

void CommunicationSettngsWidget::on_pushButton_2_clicked()
{
    QHostAddress _ip=QHostAddress(ui->lineEdit_6->text());
    uint _port=ui->lineEdit_4->text().toInt();
    int _communicateType=ui->comboBox_4->currentIndex();
    if(_port!=port||_ip!=ip||_communicateType!=communicateType){
        QMessageBox::StandardButton button;
        button = QMessageBox::question(this, "Caution",
                                       QString("Paramer changed,confirm and apply it?"),
                                       QMessageBox::Yes | QMessageBox::No);
        if(button == QMessageBox::No){
            return;
        }
    }

    if(_ip!=ip||_port!=port||_communicateType!=communicateType){
        ip=_ip;
        port=_port;
        if(socketPtr->getSocketConnectStatus())
            disConnectServer();
        if(_communicateType!=communicateType)
        {
            communicateType=_communicateType;
            delete socketPtr;
            socketInitialize();
        }
        connectServer(ip,port);
    }
    getUiChoosedResultStr();
    paramSave();
    close();
}

void CommunicationSettngsWidget::slot_connectStatusChanged(QAbstractSocket::SocketState status)
{
    if(socketPtr->getSocketConnectStatus()){
        emit signal_communicateStatusChanged(serialNum-1,true);
        connectedUiInitialize();
    }else{
        emit signal_communicateStatusChanged(serialNum-1,false);
        disconnectUiInitialize();
    }
    switch(status)
    {
    case QAbstractSocket::UnconnectedState:
    {
        ui->lineEdit_2->setText(QString("UnconnectedState"));
    }break;
    case QAbstractSocket::HostLookupState:
    {
        ui->lineEdit_2->setText(QString("HostLookupState"));
    }break;
    case QAbstractSocket::ConnectingState:
    {
        ui->lineEdit_2->setText(QString("ConnectingState"));
    }break;
    case QAbstractSocket::ConnectedState:
    {
        ui->lineEdit_2->setText(QString("ConnectedState"));
    }break;
    case QAbstractSocket::BoundState:
    {
        ui->lineEdit_2->setText(QString("BoundState"));
    }break;
    case QAbstractSocket::ClosingState:
    {
        ui->lineEdit_2->setText(QString("UnconnectedState"));
    }break;
    case QAbstractSocket::ListeningState:
    {
        ui->lineEdit_2->setText(QString("ListeningState"));
    }break;
    }
}

void CommunicationSettngsWidget::on_pushButton_5_clicked()
{
    if(vecCombox.size()!=0){
        vecCombox[vecCombox.size()-1]->setVisible(true);
        delete vecCombox[vecCombox.size()-1];
        vecCombox.pop_back();

        vecLineEdit[vecLineEdit.size()-1]->setVisible(true);
        delete vecLineEdit[vecLineEdit.size()-1];
        vecLineEdit.pop_back();
    }
}

void CommunicationSettngsWidget::on_pushButton_6_clicked()
{
    emit signal_VisionRun(serialNum,1);
}

void CommunicationSettngsWidget::on_pushButton_7_clicked()
{
    QHostAddress ip=QHostAddress(ui->lineEdit_6->text());
    uint port=ui->lineEdit_4->text().toInt();
    connectServer(ip,port);
}
