#include "PluginManager.h"
#include "ICommonInterface.h"

#include <QUrl>
#include <QDir>
#include <QDebug>
#include <QApplication>
#include "globals.h"

namespace Plugins {

PluginManager *PluginManager::mInstance = 0;

class PluginManagerPrivate {
public:
    PluginManagerPrivate(PluginManager *pq)
        : q(pq)
    {
        mSearchDirs.append(q->makeAbsolute(pluginInfoPath));
        loadPlugins();
    }

    void loadPlugins()
    {
        foreach (QUrl nextDir, mSearchDirs) {
            QDir dir(nextDir.toLocalFile());
            qDebug() << "Loading from dir" << dir.absolutePath();
            Q_ASSERT(dir.exists());

            QFileInfoList pluginCandidates = dir.entryInfoList(QStringList() << "*"+plugInfoSuffix, QDir::Files | QDir::Readable | QDir::NoDotAndDotDot | QDir::NoSymLinks);
            foreach (QFileInfo nextPluginInfo, pluginCandidates) {
                qDebug() << nextPluginInfo.fileName();

                PInfoHandler hl(nextPluginInfo.absoluteFilePath());
                if (!hl) {
                    continue;
                }
                mInfos.append(hl);

                if (!hl.isEnabled()) {
                    continue;
                }

                loadPlugin(hl);
            }
        }
    }

    bool checkAndLoadPlugin(const PInfoHandler &metaData)
    {
        if (!checkMeta(metaData) || !metaData.isEnabled()) {
            return false;
        }
        return loadPlugin(metaData, true);
    }

    bool loadPlugin(const PInfoHandler &metaData, bool emitSignal = false)
    {
        QPluginLoader *loader = new QPluginLoader(metaData.absolutePluginPath());
        loader->load();
        if (!loader->isLoaded()) {
            qDebug() << "Can't load" << metaData.absolutePluginPath();
            return false;
        }
        ICommonInterface *commonPlugin = qobject_cast<ICommonInterface*>(loader->instance());
        if (!commonPlugin) {
            qDebug() << "plugin" << metaData.absolutePluginPath() << "does not implement the plugin IcommonInterface";
            loader->unload();
            return false;
        }
        commonPlugin->setPluginMeta(metaData);
        commonPlugin->setLoader(loader);

        mPluginLoaders.append(loader);

        if (emitSignal) {
            emit q->pluginDynamiclyLoaded(loader);
        }
        return true;
    }

    bool checkMeta(const PInfoHandler &metaData)
    {
        if (!metaData) {
            qDebug() << "Metadata is not valid or disabled directly";
            return false;
        }
        QList<ICommonInterface*> commonObjList = PluginManager::getObjects<ICommonInterface*>();
        foreach (ICommonInterface* curInterface, commonObjList) {
            if (curInterface->pluginMeta().sameOwnPath(metaData)) {
                qDebug() << "Plugin" << curInterface->pluginMeta().absolutePluginPath() << "is allready loaded";
                return false;
            }
        }
        bool append = true;
        foreach (PInfoHandler nextHandler, mInfos) {
            if (metaData.sameOwnPath(nextHandler)) {
                append = false;
                break;
            }
        }
        if (append) {
            mInfos.append(metaData);
        }

        return true;
    }

    static QString adjustedPluginName(const QString &pFileName) {
        QString filename = pFileName.section("/", -1);
        if (filename.isNull()) {
            return QString();
        }

        if (!filename.toLower().startsWith("lib")) {
            filename.prepend("lib");
        }
        if (!filename.endsWith(suffixForCurrentPlatformPatern())) {
            int firstNumberInex = -1;

            //checking library trailing version names for unix like
            QStringList extList = filename.split(".");
            for (int i = extList.count() - 1; i >= 0; i--) {
                bool ok = false;
                extList.at(i).toInt(&ok);
                if (ok) {
                    firstNumberInex = i;
                } else {
                    break;
                }
            }
            if (firstNumberInex != -1) {
                if (extList.at(firstNumberInex - 1) != suffixForCurrentPlatformPatern().remove(".")) {
                    return QString();
                }
            } else {
                filename.append(suffixForCurrentPlatformPatern());
            }
        }

        return pFileName.section("/", 0, -2) + "/" + filename;
    }

    static QString suffixForCurrentPlatformPatern()
    {
#if defined Q_OS_LINUX
        return lLibLinuxSuffix;
#elif defined Q_WS_MAC
        return lLibMacXSuffix;
#elif defined Q_OS_WIN
        return lLibWindowsSuffix;
#endif
        return QString();
    }

private:
    PluginManager *q;
    QList<QUrl> mSearchDirs;
    QList<QPluginLoader*> mPluginLoaders;
    PluginManager::PInfoList mInfos;

    friend class PluginManager;

};

PluginManager::PluginManager()
    : d(new PluginManagerPrivate(this))
{
    mInstance = this;
}

PluginManager::~PluginManager()
{
    if (d) {
        delete d;
        d = 0;
    }
}

QList<QPluginLoader*> PluginManager::getAllPlugins()
{
    if (mInstance) {
        return mInstance->d->mPluginLoaders;
    }

    return QList<QPluginLoader*>();
}

PluginManager::PInfoList PluginManager::pluginMetas()
{
    if (mInstance) {
        return mInstance->d->mInfos;
    }

    return PInfoList();
}

QString PluginManager::suffixForCurrentPlatform()
{
    return PluginManagerPrivate::suffixForCurrentPlatformPatern();
}

QString PluginManager::adjustedPluginName(const QString &givenName)
{
    return PluginManagerPrivate::adjustedPluginName(givenName);
}

bool PluginManager::removeObject(QObject *object)
{
    ICommonInterface *commonPlugin = qobject_cast<ICommonInterface*>(object);
    if (!commonPlugin) {
        qDebug() << "can't cast object to ICommonInterface for deletion";
        return false;
    }

    foreach (QPluginLoader *loader, mInstance->d->mPluginLoaders) {
        if (loader && loader == commonPlugin->loader()) {
            loader->unload();
            if (loader->isLoaded()) {
                return false;
            }
            mInstance->d->mPluginLoaders.removeAll(loader);
            delete loader;
        }
    }

    return true;
}

bool PluginManager::loadPlugin(const PInfoHandler &metaData)
{
    return mInstance->d->checkAndLoadPlugin(metaData);
}

QString PluginManager::makeAbsolute(const QString &relativePath)
{
    if (!qApp) {
        qDebug() << Q_FUNC_INFO << "No application instance created";
        return relativePath;
    }
    if (relativePath.startsWith("/")) {
        qDebug() << Q_FUNC_INFO << "Allready absolute";
        return relativePath;
    }
    qDebug() << Q_FUNC_INFO << "relative path is " << relativePath;
    return qApp->applicationDirPath() + "/" + relativePath;

}

} // namespace Plugins
