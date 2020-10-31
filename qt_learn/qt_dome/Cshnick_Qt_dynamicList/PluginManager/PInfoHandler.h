#ifndef PINFOHANDLER_H
#define PINFOHANDLER_H

#include "PluginManager_global.h"
#include <QMetaType>

QT_BEGIN_NAMESPACE
class QDomDocument;
QT_END_NAMESPACE

namespace Plugins {

    class PInfoHandlerPrivate;
    class PLUGINMANAGERSHARED_EXPORT PInfoHandler
    {
    public:
        PInfoHandler();
        PInfoHandler(const PInfoHandler &other);
        PInfoHandler(const QDomDocument &data);
        PInfoHandler(const QString &dataFile);
        ~PInfoHandler();
        void setData(const QDomDocument &pData);
        void setFileData(const QString &fileData);
        void setEnabled(bool pEnabled);
        bool isEnabled() const;
        bool isValid() const;
        operator bool() const {return isValid();}
        void save();
        QString absolutePluginPath() const;
        QString displayName() const;

        static QString stringForBool(bool pArgument);
        static bool boolForString(const QString &pArgument);
        PInfoHandler &operator=(const PInfoHandler &other);
        bool sameOwnPath(const PInfoHandler &other) const;

    private:
        PInfoHandlerPrivate *d;

        friend class PInfoHandlerPrivate;
    };
} //namespace Plugins

Q_DECLARE_METATYPE(Plugins::PInfoHandler)

#endif // PINFOHANDLER_H
