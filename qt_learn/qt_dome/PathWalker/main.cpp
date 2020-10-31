#include <QApplication>
#include <QStandardPaths>

#include "entityinfo.h"
#include "entityinstance.h"
#include "styleproxymenu.h"
#include "direntrylistview.h"
#include "settingsstore/isettings.h"
#include "settingsstore/settingsstore.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    QCoreApplication::setOrganizationName("DNK");
    QCoreApplication::setApplicationName("PathWalker");

    SettingsStore::cfgPathSet(QStandardPaths::writableLocation(QStandardPaths::DataLocation)
                              +"/"+QApplication::applicationName()+".ini");

    a.setStyle(new StyleProxyMenu());

    DirEntryListView *w = new DirEntryListView();
    w->show();
    if(QCoreApplication::arguments().count()>1) w->setPath(QCoreApplication::arguments().at(1));

    return a.exec();
}
