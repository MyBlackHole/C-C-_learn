#-------------------------------------------------
#
# Project created by QtCreator 2013-03-18T23:40:40
#
#-------------------------------------------------

QT       += core gui
CONFIG   += debug

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = GitUI
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    repository.cpp \
    git.cpp \
    file.cpp \
    branch.cpp \
    remoting.cpp \
    remote.cpp \
    clone.cpp \
    push.cpp \
    fetch.cpp \
    usersettings.cpp \
    helpindeks.cpp \
    About.cpp

HEADERS  += mainwindow.h \
    repository.h \
    git.h \
    file.h \
    branch.h \
    remoting.h \
    remote.h \
    clone.h \
    push.h \
    fetch.h \
    usersettings.h \
    helpindeks.h \
    About.h

FORMS    += mainwindow.ui \
    repository.ui \
    file.ui \
    branch.ui \
    remoting.ui \
    clone.ui \
    push.ui \
    fetch.ui \
    usersettings.ui \
    helpindeks.ui \
    about.ui
QT  +=webkitwidgets
