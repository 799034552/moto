#-------------------------------------------------
#
# Project created by QtCreator 2019-06-01T20:03:58
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = 123
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    SerialPort.cpp \
    stdafx.cpp \
    mywidght.cpp

HEADERS  += mainwindow.h \
    SerialPort.h \
    stdafx.h \
    targetver.h \
    mywidght.h

FORMS    += mainwindow.ui

LIBS += -lWs2_32
