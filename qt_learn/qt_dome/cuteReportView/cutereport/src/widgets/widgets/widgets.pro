! include( ../../../common.pri ) {
    error( Can not find the common.pri file! )
}

TARGET = CuteReportWidgets
TEMPLATE =  lib

lessThan(QT_MAJOR_VERSION, 5) {
    CONFIG += designer
    QT += webkit
} else {
    QT += designer
}

INCLUDEPATH = ../../core
DEPENDPATH = $$INCLUDEPATH

LIBS += -L../../../$$BUILD_DIR -lCuteReport

include (combobox.pri)
include (objectpreview.pri)
include (reportpreview.pri)
include (stdstoragedialog.pri)
include (emptydialog.pri)
include (exportdialog.pri)

WIDGETS_HEADERS.files += widgets_export.h
INSTALLS += WIDGETS_HEADERS

contains(DEFINES, SYSTEMINSTALL) {
    DESTDIR = ../../../$$BUILD_DIR
    target.path += $$REPORT_LIBS_PATH
    INSTALLS += target
    WIDGETS_HEADERS.path = $$REPORT_HEADERS_PATH
} else {
    DESTDIR = ../../../$$BUILD_DIR/$$REPORT_LIBS_PATH
    WIDGETS_HEADERS.path = $$OUT_PWD/../../$$BUILD_DIR/$$REPORT_HEADERS_PATH
}

HEADERS += \
    functions_gui.h

SOURCES += \
    functions_gui.cpp

contains(DEFINES, STATIC_WIDGETS) {
    CONFIG += static
    CONFIG += uitools
} else {
    win32 : TARGET_EXT = .dll
}

win32 {
    DEFINES += CUTEREPORT_WIDGET_EXPORTS
}
