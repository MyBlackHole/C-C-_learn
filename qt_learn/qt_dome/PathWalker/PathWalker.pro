#-------------------------------------------------
#
# Project created by QtCreator 2014-12-16T20:35:35
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets winextras

TARGET = PathWalker
TEMPLATE = app


SOURCES += main.cpp\
    entityinfo.cpp \
    entityinstance.cpp \
    direntry.cpp \
    entitylistmodel.cpp \
    direntrylistview.cpp \
    columnheader.cpp \
    fileiconcache.cpp \
    direntrysrc.cpp \
    asyncentitylistarranger.cpp \
    busydialog.cpp \
    styleproxymenu.cpp \
    entitycompare.cpp \
    asyncentitylistfilter.cpp \
    dirpathhistory.cpp \
    direntrynavibar.cpp \
    settingsstore/settingsstore.cpp \
    settingsstore/isettings.cpp \
    pathchangewatcher.cpp \
    quickfilter.cpp \
    visualkeyboardnavigator/visualkeyboardnavigator.cpp \
    visualkeyboardnavigator/visualkeyboardnavigatoreventfilter.cpp \
    visualkeyboardnavigator/visualtag.cpp \
    visualkeyboardnavigator/visualtaginfo.cpp \
    direntrymodelinfo.cpp \
    aboutpathwalker.cpp \
    bookmarksdialog.cpp \
    historydialog.cpp \
    entitylistmodelinfo.cpp \
    task.cpp \
    asynctaskpipeline.cpp

HEADERS  += \
    entityinfo.h \
    entityinstance.h \
    direntry.h \
    def.h \
    entitylistmodel.h \
    direntrylistview.h \
    columnheader.h \
    fileiconcache.h \
    direntrysrc.h \
    asyncentitylistarranger.h \
    busydialog.h \
    styleproxymenu.h \
    entitycompare.h \
    asyncentitylistfilter.h \
    dirpathhistory.h \
    direntrynavibar.h \
    settingsstore/settingsstore.h \
    settingsstore/isettings.h \
    pathchangewatcher.h \
    quickfilter.h \
    visualkeyboardnavigator/visualkeyboardnavigator.h \
    visualkeyboardnavigator/visualkeyboardnavigatoreventfilter.h \
    visualkeyboardnavigator/visualtag.h \
    visualkeyboardnavigator/visualtaginfo.h \
    direntrymodelinfo.h \
    aboutpathwalker.h \
    bookmarksdialog.h \
    historydialog.h \
    entitylistmodelinfo.h \
    task.h \
    asynctaskpipeline.h

RESOURCES += \
    res.qrc

RC_FILE = icon.rc

OTHER_FILES += \
    todo.txt \
    desc.txt

FORMS    += mainwindow.ui
