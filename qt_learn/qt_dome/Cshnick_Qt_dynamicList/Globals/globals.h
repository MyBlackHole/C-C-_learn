#ifndef GLOBALS_H
#define GLOBALS_H

#include <QString>

namespace Globals {

const int defaultCellSize = 80;
const int maxCellSize = 250;
const int pageRole = Qt::UserRole + 1;
const Qt::TransformationMode defTRansformationMode = Qt::SmoothTransformation;
const int cleanerTimerInterval = 10000; //10 seconds
const int saveMemoryMultipler = 2; //Allways save current viewport and cache indexes above and under the visible area;
//save visible QModelIndex count * saveMemoryMultipler on top and on the bottom
const bool useMemoryCleaner = true;

enum Role {
    InternalPathRole = 0
    , CustomRole = 100
};
} //namespace Globals

namespace Plugins {

const QString pluginPath1 = "../Plugins/TstDocGenerator1";
const QString pluginPath2 = "../Plugins/TstGenerator";
const QString pluginInfoPath = "../Plugins";
const QString plugInfoSuffix = ".pinfo";

const QString tEnabled = "Enabled";
const QString tTopTag = "misc";
const QString tDisplayName = "displayname";
const QString tIconSource = "icon";
const QString tPluginPath = "pathToPlugin";

const QString argTrue = "true";
const QString argFalse = "false";

const QString lLibPrefix = "lib";
const QString lLibLinuxSuffix = ".so";
const QString lLibMacXSuffix = ".dylib";
const QString lLibWindowsSuffix = ".dll";

const QString dDefaultDisplayName = "Undefined pluglin";
} //namespace Docs

#endif //GLOBALS_H
