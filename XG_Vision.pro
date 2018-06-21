#-------------------------------------------------
#
# Project created by QtCreator 2018-03-22T09:35:00
#
#-------------------------------------------------


INCLUDEPATH+=D:\pylon\Development\include
INCLUDEPATH+=D:\pylon\Development\include\base
INCLUDEPATH+=D:\pylon\Development\include\clprotocol
INCLUDEPATH+=D:\pylon\Development\include\genapi
INCLUDEPATH+=D:\pylon\Development\include\genapic
INCLUDEPATH+=D:\pylon\Development\include\pylon
INCLUDEPATH+=D:\pylon\Development\include\pylonc
INCLUDEPATH+=D:\MER-Series\Samples\CPPSDK\inc

LIBS+=D:\pylon\Development\lib\x64\GCBase_MD_VC120_v3_0_Basler_pylon_v5_0.lib
LIBS+=D:\pylon\Development\lib\x64\GenApi_MD_VC120_v3_0_Basler_pylon_v5_0.lib
LIBS+=D:\pylon\Development\lib\x64\PylonBase_MD_VC120_v5_0.lib
LIBS+=D:\pylon\Development\lib\x64\PylonC_MD_VC120.lib
LIBS+=D:\pylon\Development\lib\x64\PylonUtility_MD_VC120_v5_0.lib
LIBS+=D:\MER-Series\Samples\CPPSDK\lib\x64\GxIAPICPP.lib


INCLUDEPATH+=F:\opencv\build\include
INCLUDEPATH+=F:\opencv\build\include\opencv
INCLUDEPATH+=F:\opencv\build\include\opencv2
CONFIG(debug,debug|release){
LIBS+=F:\opencv\build\x64\vc14\lib\opencv_world310d.lib
}else{
LIBS+=F:\opencv\build\x64\vc14\lib\opencv_world310.lib
}

QT       += core gui
QT += network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = XG_Vision
TEMPLATE = app

# The following define makes your compiler emit warnings if you use
# any feature of Qt which as been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS
QMAKE_CXXFLAGS+=/openmp

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0


SOURCES += main.cpp\
        xg_vision.cpp \
    ncc_match.cpp \
    gigecamera.cpp \
    dahenggigecamera.cpp \
    pylongigecamera.cpp \
    calibrate.cpp \
    calibration.cpp \
    nccmatchwidget.cpp \
    cvandui.cpp \
    camerasettingswidget.cpp \
    communicationsettngswidget.cpp \
    socketcommunicate.cpp \
    fxplcsocketcommunicate.cpp \
    qplcsocketcommunicate.cpp \
    yaskawarobotsocketcommunicate.cpp \
    aboutdialog.cpp \
    recordsettingsdialog.cpp \
    standardsocketcommunicate.cpp

HEADERS  += xg_vision.h \
    ncc_match.h \
    gigecamera.h \
    dahenggigecamera.h \
    pylongigecamera.h \
    calibrate.h \
    calibration.h \
    nccmatchwidget.h \
    cvandui.h \
    camerasettingswidget.h \
    communicationsettngswidget.h \
    socketcommunicate.h \
    fxplcsocketcommunicate.h \
    qplcsocketcommunicate.h \
    yaskawarobotsocketcommunicate.h \
    vision.h \
    aboutdialog.h \
    recordsettingsdialog.h \
    standardsocketcommunicate.h

UI_DIR=./UI

FORMS    += xg_vision.ui \
    calibration.ui \
    nccmatchwidget.ui \
    camerasettingswidget.ui \
    communicationsettngswidget.ui \
    aboutdialog.ui \
    recordsettingsdialog.ui
