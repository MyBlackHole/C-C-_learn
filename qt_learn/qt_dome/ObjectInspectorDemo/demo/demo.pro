#-------------------------------------------------
#
# Project created by QtCreator 2013-12-10T17:12:13
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = demo
TEMPLATE = app

INCLUDEPATH += $$PWD/../objectinspector\
               $$PWD/../objectinspector/model

DEPENDPATH += $$PWD/../objectinspector

SOURCES += main.cpp\
        mainwindow.cpp

HEADERS  += mainwindow.h

FORMS    += mainwindow.ui

win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../objectinspector/release/ -lobjectinspector
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../objectinspector/debug/ -lobjectinspector
