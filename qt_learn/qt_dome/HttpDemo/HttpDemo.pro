#-------------------------------------------------
#
# Project created by QtCreator 2015-08-26T23:27:46
#
#-------------------------------------------------

QT       += core gui
QT       += network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = HttpDemo
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    getdatathread.cpp

HEADERS  += mainwindow.h \
    getdatathread.h

FORMS    += mainwindow.ui
