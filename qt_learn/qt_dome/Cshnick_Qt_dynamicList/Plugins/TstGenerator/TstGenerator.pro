TARGET = TstGenerator
TEMPLATE = lib

DEFINES += TSTGENERATOR_LIBRARY

include($$PWD/../../Globals/plugin.pri)

INCLUDEPATH += $$includeDependency(Globals) \
               $$includeDependency(PluginManager) \
               $$includeDependency(ThumbnailManager)

LIBS += $$libraryDependency(Documents) \
        $$libraryDependency(PluginManager)

SOURCES += tstgenerator.cpp

HEADERS += tstgenerator.h\
        TstGenerator_global.h

