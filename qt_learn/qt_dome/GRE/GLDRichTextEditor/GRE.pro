#-------------------------------------------------
#
# Project created by QtCreator 2015-03-07T10:27:59
#
#-------------------------------------------------

QT       += core gui axcontainer

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = GRE
TEMPLATE = app
DESTDIR = ../bin

#CONFIG += staticlib \
#    debug_and_release

#CONFIG(debug, debug|release){
#    contains(QMAKE_HOST.arch, x86_64) {
#        DESTDIR = ../../../lib/Debug/X64
#    } else {
#        DESTDIR = ../../../lib/Debug/X86
#    }
#    unix:TARGET=$$join(TARGET,,,_debug)
#    win32:TARGET=$$join(TARGET,,,d)
#}

#CONFIG(release, debug|release){
#    contains(QMAKE_HOST.arch, x86_64) {
#        DESTDIR = ../../../lib/Release/X64
#    } else {
#        DESTDIR = ../../../lib/Release/X86
#    }
#}

DEFINES += SRCDIR=\\\"$$PWD/src/\\\" \
    INITGUID \
    GDP_QT

INCLUDEPATH += include \
            ../GRE

#include($(GLDRS)/Glodon/shared/VLD.pri)

SOURCES += \
        src\GLDBitmapCompress.cpp\
        src\GLDImageObject.cpp\
        src\GLDRichTextEdit.cpp\
        src\GLDRichTextEditAddTableDlg.cpp\
        src\GLDRichTextEditFindAndReplaceWidget.cpp\
        src\GLDRichTextEditGlobal.cpp\
        src\GLDRichTextEditNativeEventFilter.cpp\
        src\GLDRichTextEditOleCallback.cpp\
        src\GLDRichTextEditor.cpp\
        src\GLDRichTextEditTableValueTest.cpp\
        src\main.cpp

HEADERS += \
        ../GRE/GLDRichTextEdit.h\
        ../GRE/GLDRichTextEditor.h\
        include/GLDBitmapCompress.h\
        include/GLDImageObject.h\
        include/GLDRichTextEditAddTableDlg.h\
        include/GLDRichTextEditFindAndReplaceWidget.h\
        include/GLDRichTextEditGlobal.h\
        include/GLDRichTextEditNativeEventFilter.h\
        include/GLDRichTextEditOleCallback.h\
        include/GLDRichTextEditTableValueTest.h

FORMS += \
      ui/GLDRichTextEditAddTableDlg.ui\
      ui/GLDRichTextEditFindAndReplaceWidget.ui\
      ui/GLDRichTextEditor.ui\
      ui/GLDRichTextEditTableValueTest.ui

RESOURCES += \
          res/GLDRichTextEditor.qrc

