#-------------------------------------------------
#
# Project created by QtCreator 2015-08-27T10:48:14
#
#-------------------------------------------------

QT       += core gui network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = QNetworkAccessManagerDemo
TEMPLATE = app

DESTDIR = ../QNetworkAccessManagerDemo/bin

SOURCES += main.cpp\
        MainWindow.cpp \
    NetWorker.cpp

HEADERS  += MainWindow.h \
    NetWorker.h

FORMS    += MainWindow.ui

CONFIG += c++11
