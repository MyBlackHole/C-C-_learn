TEMPLATE = app
TARGET = phototonic

INCLUDEPATH += ./exiv2/include

QT += widgets

QMAKE_CXXFLAGS += $$(CXXFLAGS)
QMAKE_CFLAGS += $$(CFLAGS)
QMAKE_LFLAGS += $$(LDFLAGS)


# Input
HEADERS += \
    thumbview.h \
    tags.h \
    mainwindow.h \
    infoview.h \
    imageview.h \
    global.h \
    fstree.h \
    dircompleter.h \
    dialogs.h \
    croprubberband.h \
    bookmarks.h

SOURCES += \
    thumbview.cpp \
    tags.cpp \
    mainwindow.cpp \
    main.cpp \
    infoview.cpp \
    imageview.cpp \
    global.cpp \
    fstree.cpp \
    dircompleter.cpp \
    dialogs.cpp \
    croprubberband.cpp \
    bookmarks.cpp

RESOURCES += phototonic.qrc

target.path = /usr/bin/

icon.files = images/phototonic.png
icon.path = /usr/share/icons/hicolor/48x48/apps

icon16.files = images/icon16/phototonic.png
icon16.path = /usr/share/icons/hicolor/16x16/apps

iconPixmaps.files = images/icon16/phototonic.png
iconPixmaps.path = /usr/share/pixmaps

desktop.files = phototonic.desktop
desktop.path = /usr/share/applications

INSTALLS += target icon icon16 iconPixmaps desktop

TRANSLATIONS = translations/phototonic_en.ts \
               translations/phototonic_pl.ts \
               translations/phototonic_de.ts \
               translations/phototonic_ru.ts \
               translations/phototonic_cs.ts \
               translations/phototonic_fr.ts

win32:CONFIG(release, debug|release): LIBS += -L$$PWD/exiv2/lib/ -lexiv2
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/exiv2/lib/ -lexiv2d

INCLUDEPATH += $$PWD/exiv2/include
DEPENDPATH += $$PWD/exiv2/include

win32:CONFIG(release, debug|release): PRE_TARGETDEPS += $$PWD/exiv2/lib/exiv2.lib
else:win32:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$PWD/exiv2/lib/exiv2d.lib
