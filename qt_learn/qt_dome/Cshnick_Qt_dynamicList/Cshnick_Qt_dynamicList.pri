!isEmpty(CSHNICK_QT_DYNAMICLIST_PRI):error("Cshnick_Qt_dynamicList.pri must be included only once")
CSHNICK_QT_DYNAMICLIST_PRI = 1

defineReplace(libraryDependency) {
    return(-l$$1)
}

TOP_LEVEL_SRC_DIR = $$PWD
#message(PWD $$PWD)
#message(OUT_PWD $$OUT_PWD)
isEmpty(TOP_LEVEL_BUILD_DIR) {
  relativeDir = $$_PRO_FILE_PWD_
  relativeDir ~= s,^$$re_escape($$PWD),,
  TOP_LEVEL_BUILD_DIR = $$OUT_PWD
  TOP_LEVEL_BUILD_DIR ~= s,$$re_escape($$relativeDir)$,,
  TOP_LEVEL_BUILD_DIR = $$TOP_LEVEL_BUILD_DIR/build
#  message(TOP_LEVEL_BUILD_DIR $$TOP_LEVEL_BUILD_DIR)
}

APP_DIR = $$TOP_LEVEL_BUILD_DIR/bin
!macx {
  APP_TARGET = "docManager"
  LIB_PATH = $$TOP_LEVEL_BUILD_DIR/lib
  PLUGIN_PATH = $$TOP_LEVEL_BUILD_DIR/Plugins
  EXEC_PATH = $$APP_DIR
#  message(APP_TARGET $$APP_TARGET)
#  message(LIB_PATH $$LIB_PATH)
#  message(PLUGIN_PATH $$PLUGIN_PATH)
#  message(EXEC_PATH $$EXEC_PATH)

} else { #macx
  APP_TARGET = "bin"
  PLUGIN_PATH = $$APP_DIR/$${APP_TARGET}.app/Contents/PlugIns
  LIB_PATH = $$PLUGIN_PATH
  EXEC_PATH = $$APP_DIR/$${APP_TARGET}.app/Contents/MacOS
}

CONFIG += depend_includepath

LIBS *= -L$$LIB_PATH

SERVICE_DIR = $${OUT_PWD}/.service
CONFIG(debug, debug|release) {
  DEBUG_POSTFIX = debug
  OBJECTS_DIR = $$SERVICE_DIR/$$DEBUG_POSTFIX
  MOC_DIR = $$SERVICE_DIR/$$DEBUG_POSTFIX
}
CONFIG(release, debug|release) {
  RELEASE_POSTFIX = release
  OBJECTS_DIR = $$SERVICE_DIR/$$RELEASE_POSTFIX
  MOC_DIR = $$SERVICE_DIR/$$RELEASE_POSTFIX
}


