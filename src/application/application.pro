#-------------------------------------------------
#
# Project created by QtCreator 2012-12-30T17:45:12
#
#-------------------------------------------------

QT       += core gui widgets

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = application
TEMPLATE = app

SOURCES += main.cpp\
        mainwindow.cpp

HEADERS  += mainwindow.h \
    imageconversion.h \
    utils.h

FORMS    += mainwindow.ui

RESOURCES += \
    application.qrc

INCLUDEPATH += /usr/local/include/opencv
LIBS += -L/usr/local/lib \
-lopencv_core \
-lopencv_imgproc \
-lopencv_highgui \
-lopencv_ml \
-lopencv_video \
-lopencv_features2d \
-lopencv_calib3d \
-lopencv_objdetect \
-lopencv_contrib \
-lopencv_legacy \
-lopencv_flann
