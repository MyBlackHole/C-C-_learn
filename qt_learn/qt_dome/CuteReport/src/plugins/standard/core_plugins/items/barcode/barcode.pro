! include( ../../plugins.pri ) {
    error( Could not find the common.pri file! )
}

! include ( barcode.pri ) {
    error( Could not find item .pri file! )
}


TARGET = Barcode

DEFINES += ZINT_VERSION=\\\"2.4.4\\\"
DEFINES += NO_PNG

win32-msvc* {
    DEFINES += _CRT_SECURE_NO_WARNINGS
    QMAKE_CFLAGS += /TP /wd4018 /wd4244 /wd4305
    QMAKE_CXXFLAGS += /TP /wd4018 /wd4244 /wd4305
}

INCLUDEPATH += \
    ../../../../../thirdparty/ \
    ../../../../../thirdparty/zint \
    ../../../../../thirdparty/zint/backend \
    ../../../../../thirdparty/zint/backend_qt4

DEPENDPATH  += $$INCLUDEPATH

HEADERS += \
    $$PWD/../../../../../thirdparty/zint/backend_qt4/qzint.h \
    $$PWD/../../../../../thirdparty/zint/backend/zint.h \
    $$PWD/../../../../../thirdparty/zint/backend/sjis.h \
    $$PWD/../../../../../thirdparty/zint/backend/rss.h \
    $$PWD/../../../../../thirdparty/zint/backend/reedsol.h \
    $$PWD/../../../../../thirdparty/zint/backend/qr.h \
    $$PWD/../../../../../thirdparty/zint/backend/pdf417.h \
    $$PWD/../../../../../thirdparty/zint/backend/ms_stdint.h \
    $$PWD/../../../../../thirdparty/zint/backend/maxipng.h \
    $$PWD/../../../../../thirdparty/zint/backend/maxicode.h \
    $$PWD/../../../../../thirdparty/zint/backend/large.h \
    $$PWD/../../../../../thirdparty/zint/backend/gs1.h \
    $$PWD/../../../../../thirdparty/zint/backend/gridmtx.h \
    $$PWD/../../../../../thirdparty/zint/backend/gb2312.h \
    $$PWD/../../../../../thirdparty/zint/backend/font.h \
    $$PWD/../../../../../thirdparty/zint/backend/dmatrix.h \
    $$PWD/../../../../../thirdparty/zint/backend/composite.h \
    $$PWD/../../../../../thirdparty/zint/backend/common.h \
    $$PWD/../../../../../thirdparty/zint/backend/code49.h \
    $$PWD/../../../../../thirdparty/zint/backend/code1.h \
    $$PWD/../../../../../thirdparty/zint/backend/aztec.h \

SOURCES += \
    $$PWD/../../../../../thirdparty/zint/backend_qt4/qzint.cpp \
    $$PWD/../../../../../thirdparty/zint/backend/library.c \
    $$PWD/../../../../../thirdparty/zint/backend/upcean.c \
    $$PWD/../../../../../thirdparty/zint/backend/telepen.c \
    $$PWD/../../../../../thirdparty/zint/backend/svg.c \
    $$PWD/../../../../../thirdparty/zint/backend/rss.c \
    $$PWD/../../../../../thirdparty/zint/backend/render.c \
    $$PWD/../../../../../thirdparty/zint/backend/reedsol.c \
    $$PWD/../../../../../thirdparty/zint/backend/qr.c \
    $$PWD/../../../../../thirdparty/zint/backend/ps.c \
    $$PWD/../../../../../thirdparty/zint/backend/postal.c \
    $$PWD/../../../../../thirdparty/zint/backend/png.c \
    $$PWD/../../../../../thirdparty/zint/backend/plessey.c \
    $$PWD/../../../../../thirdparty/zint/backend/pdf417.c \
    $$PWD/../../../../../thirdparty/zint/backend/medical.c \
    $$PWD/../../../../../thirdparty/zint/backend/maxicode.c \
    $$PWD/../../../../../thirdparty/zint/backend/large.c \
    $$PWD/../../../../../thirdparty/zint/backend/imail.c \
    $$PWD/../../../../../thirdparty/zint/backend/gs1.c \
    $$PWD/../../../../../thirdparty/zint/backend/gridmtx.c \
    $$PWD/../../../../../thirdparty/zint/backend/dmatrix.c \
    $$PWD/../../../../../thirdparty/zint/backend/dllversion.c \
    $$PWD/../../../../../thirdparty/zint/backend/composite.c \
    $$PWD/../../../../../thirdparty/zint/backend/common.c \
    $$PWD/../../../../../thirdparty/zint/backend/code128.c \
    $$PWD/../../../../../thirdparty/zint/backend/code49.c \
    $$PWD/../../../../../thirdparty/zint/backend/code16k.c \
    $$PWD/../../../../../thirdparty/zint/backend/code1.c \
    $$PWD/../../../../../thirdparty/zint/backend/code.c \
    $$PWD/../../../../../thirdparty/zint/backend/aztec.c \
    $$PWD/../../../../../thirdparty/zint/backend/auspost.c \
    $$PWD/../../../../../thirdparty/zint/backend/2of5.c

