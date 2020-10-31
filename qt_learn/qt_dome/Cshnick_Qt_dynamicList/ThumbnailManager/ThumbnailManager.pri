#-------------------------------------------------
#
# Project created by QtCreator 2013-02-22T12:17:53
#
#-------------------------------------------------

#TARGET = picturesmodelview
#TEMPLATE = lib

DEFINES += THUMBNAILMANAGER_LIBRARY

#SRCDIR =
SOURCES += $$PWD/ThumbnailManager.cpp

HEADERS += $$PWD/ThumbnailManager.h\
        $$PWD/picturesmodelview_global.h

unix:!symbian {
    maemo5 {
        target.path = /opt/usr/lib
    } else {
        target.path = /usr/lib
    }
    INSTALLS += target
}

SERVICE_DIR =  $$OUT_PWD/service
MOC_DIR = $$SERVICE_DIR
OBJECTS_DIR = $$SERVICE_DIR
