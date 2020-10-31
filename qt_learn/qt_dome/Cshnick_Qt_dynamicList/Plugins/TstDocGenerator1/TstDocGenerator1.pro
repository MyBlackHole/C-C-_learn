TARGET = TstDocGenerator1
TEMPLATE = lib

DEFINES += TSTDOCGENERATOR1_LIBRARY
QT += xml

include($$PWD/../../Globals/plugin.pri)

INCLUDEPATH += $$includeDependency(Globals) \
               $$includeDependency(PluginManager) \
               $$includeDependency(Documents) \
               $$includeDependency(ThumbnailManager) \
               $$includeDependency(TstGenerator) \

LIBS += $$libraryDependency(PluginManager) \
        $$libraryDependency(Documents)

SOURCES += tstdocgenerator1.cpp

HEADERS += tstdocgenerator1.h\
        TstDocGenerator1_global.h

RESOURCES += \
    res.qrc
