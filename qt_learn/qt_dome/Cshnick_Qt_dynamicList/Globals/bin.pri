include(../Cshnick_Qt_dynamicList.pri)

defineReplace(includeDependency) {
     return(../$$1)
}

DESTDIR = $$APP_DIR

include(rpath.pri)

INCLUDEPATH *= $$OUT_PWD
