TEMPLATE = app
QT += gui

include($$PWD/../Globals/bin.pri)

INCLUDEPATH += $$includeDependency(Globals) \
               $$includeDependency(Documents) \
               $$includeDependency(PluginManager)

LIBS += $$libraryDependency(PluginManager) \
        $$libraryDependency(Documents)

SOURCES += \
    main.cpp
