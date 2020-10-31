#include <QtGui>

#include "DocumentManager.h"
#include "PluginManager.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    Plugins::PluginManager plugins;
    qDebug() << "Appication path" << a.applicationFilePath();

    qDebug() << "started";
    Docs::DocumentManager docMan;
    docMan.setVisible(true);
    return a.exec();
}
