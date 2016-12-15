#-------------------------------------------------
#
# Project created by QtCreator 2016-10-31T06:11:17
#
#-------------------------------------------------

QT       += core gui serialport qml quick quickwidgets

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = Compass
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp

HEADERS  += mainwindow.h

FORMS    += mainwindow.ui

DISTFILES += \
    compass_qml.qml

RESOURCES += \
    main.qrc
