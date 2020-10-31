#ifndef ICOMMONINTERFACE_H
#define ICOMMONINTERFACE_H

#include "PluginManager_global.h"
#include <QtPlugin>
#include "PInfoHandler.h"
QT_BEGIN_NAMESPACE
class QPluginLoader;
QT_END_NAMESPACE


namespace Plugins {

class PLUGINMANAGERSHARED_EXPORT ICommonInterface
{
public:
    ICommonInterface()
        : mLoader(0) {}
    PInfoHandler pluginMeta() const {return mHandler;}
    void setPluginMeta(const PInfoHandler &pHandler){mHandler = pHandler;}
    void setLoader(QPluginLoader *loader) {mLoader = loader;}
    QPluginLoader *loader() {return mLoader;}

private:
    PInfoHandler mHandler;
    QPluginLoader *mLoader;
};

} // namespace Plugins

Q_DECLARE_INTERFACE(Plugins::ICommonInterface, "org.opensankore.ICommonInterface")

#endif // ICOMMONINTERFACE_H
