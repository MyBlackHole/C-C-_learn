#-------------------------------------------------
#
# Project created by QtCreator 2016-01-22T13:36:43
#
#-------------------------------------------------

QT       += core

QT       -= gui

TARGET = QSystemInfo
CONFIG   += console
CONFIG   -= app_bundle

unix:QMAKE_CXXFLAGS += -std=c++11

TEMPLATE = app


SOURCES += \
    main.cpp \
    GLDSysInfoUtils.cpp

HEADERS += \
    GLDSysInfoUtils.h
