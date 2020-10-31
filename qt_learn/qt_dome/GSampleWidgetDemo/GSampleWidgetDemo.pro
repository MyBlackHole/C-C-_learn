#-------------------------------------------------
#
# Project created by QtCreator 2015-02-05T14:52:08
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = GSampleWidgetDemo
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    gsamplewidget.cpp \
    gsampleitemdelegate.cpp

HEADERS  += mainwindow.h \
    gsampleitem_p.h \
    gsampleitemdelegate.h \
    gsamplewidget.h \
    gsamplewidget_p.h

FORMS += \
    gsamplewidget.ui
