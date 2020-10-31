#-------------------------------------------------
#
# Project created by QtCreator 2015-05-02T09:33:59
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = Paint
TEMPLATE = app


SOURCES += main.cpp \
    PaintWindow.cpp \
    PaintWidget.cpp \
    Line.cpp \
    Curve.cpp \
    Brush.cpp \
    BaseLine.cpp

HEADERS  += \
    PaintWindow.h \
    PaintWidget.h \
    Line.h \
    ITool.h \
    Curve.h \
    Brush.h \
    BaseLine.h

FORMS    +=

RESOURCES += \
    res.qrc
