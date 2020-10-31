QT += core gui
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = example
TEMPLATE = app

SOURCES += main.cpp \
    navbarwidget.cpp

HEADERS += \
    navbar.h \
    navbarheader.h \
    navbaroptionsdialog.h \
    navbarpage.h \
    navbarpagelistwidget.h \
    navbarsplitter.h \
    navbarwidget.h

RESOURCES += \
    navbardemo.qrc

win32:CONFIG(release, debug|release): LIBS += -L$$PWD/../lib/ -lnavbar
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/../lib/ -lnavbard

INCLUDEPATH += $$PWD/../
DEPENDPATH += $$PWD/../

win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$PWD/../lib/libnavbar.a
else:win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$PWD/../lib/libnavbard.a
else:win32:!win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$PWD/../lib/navbar.lib
else:win32:!win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$PWD/../lib/navbard.lib

FORMS += \
    navbaroptionsdialog.ui
