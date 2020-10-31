#include "settingsstore.h"

// Global static pointer used to ensure a single instance of the class.
SettingsSaver* SettingsStore::aSaver= new SettingsSaver();
QString SettingsStore::aCfgPath= QString();

void SettingsStore::registerWindow(QWidget *w)
{
    w->installEventFilter(aSaver);
}

SettingsSaver *SettingsStore::saver()
{
    return aSaver;
}

void SettingsStore::cfgPathSet(const QString &path)
{
    aCfgPath= path;
}

QString SettingsStore::cfgPath()
{
    return aCfgPath;
}
