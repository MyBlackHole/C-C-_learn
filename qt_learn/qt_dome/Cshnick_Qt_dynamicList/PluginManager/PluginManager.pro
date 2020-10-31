TARGET = PluginManager
TEMPLATE = lib

QT += xml widgets
DEFINES += PLUGINMANAGER_LIBRARY

include($$PWD/../Globals/shared_lib.pri)

SOURCES += PluginManager.cpp \
           PInfoHandler.cpp

HEADERS += PluginManager.h\
        PluginManager_global.h \
        ICommonInterface.h \
        PInfoHandler.h

INCLUDEPATH += $$includeDependency(Globals)
