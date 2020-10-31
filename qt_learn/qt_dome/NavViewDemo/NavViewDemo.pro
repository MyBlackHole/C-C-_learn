#-------------------------------------------------
#
# Project created by QtCreator 2015-02-10T15:41:46
#
#-------------------------------------------------

QT       += core gui xml

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = NavViewDemo
TEMPLATE = app

DESTDIR = ../NavViewDemo/bin


SOURCES += main.cpp\
        mainwindow.cpp \
    navview.cpp \
    navmodel.cpp \
    navdelegate.cpp

HEADERS  += mainwindow.h \
    navview.h \
    navmodel.h \
    navdelegate.h \
    colordefines.h

FORMS    += mainwindow.ui

RESOURCES += \
    res.qrc
