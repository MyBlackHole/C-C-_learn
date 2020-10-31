include(../Cshnick_Qt_dynamicList.pri)

defineReplace(includeDependency) {
     return(../$$1)
}

win32 {
    DLLDESTDIR = $$APP_DIR
}

DESTDIR = $$LIB_PATH

include(rpath.pri)

CONFIG += shared dll
INCLUDEPATH *= $$OUT_PWD
