#ifndef PLUGINMANAGER_H
#define PLUGINMANAGER_H

#include "PluginManager_global.h"

#include <QObject>
#include <QReadWriteLock>
#include <QPluginLoader>
QT_BEGIN_NAMESPACE
class QDomDocument;
QT_END_NAMESPACE

namespace Plugins {

enum Platform {
   linuxOS
    ,macosx
    ,windowsOS
};

class PInfoHandler;

class PluginManagerPrivate;
class PLUGINMANAGERSHARED_EXPORT PluginManager : public QObject
{
    Q_OBJECT

public:

    typedef QList<PInfoHandler> PInfoList;
    static PluginManager *instance() {return mInstance;}

    PluginManager();
    ~PluginManager();

    static QList<QPluginLoader*> getAllPlugins();
    static PInfoList pluginMetas();

    static QString suffixForCurrentPlatform();
    static QString adjustedPluginName(const QString &givenName);

    template <typename T>
    static QList<T> getObjects() {
        QList<T> result;
        foreach (QPluginLoader *loader, getAllPlugins()) {
            T nextCandidate = qobject_cast<T>(loader->instance());
            if (nextCandidate) {
                result.append(nextCandidate);
            }
        }
        return result;
    }

    static bool removeObject(QObject *object);
    static bool loadPlugin(const PInfoHandler &metaData);
    static QString makeAbsolute(const QString &relativePath);

signals:
    void pluginDynamiclyLoaded(QPluginLoader *newPluginLoader);

private:
    PluginManagerPrivate *d;
    static PluginManager *mInstance;
    mutable QReadWriteLock mLocker;

    friend class PluginManagerPrivate;
};

} //namespace Plugins

#endif // PLUGINMANAGER_H
