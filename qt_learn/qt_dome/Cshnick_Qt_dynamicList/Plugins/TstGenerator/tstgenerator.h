#ifndef TSTGENERATOR_H
#define TSTGENERATOR_H

#include "TstGenerator_global.h"
#include "ThumbnailManager.h"
#include "ICommonInterface.h"
#include "PluginManager.h"

class TSTGENERATORSHARED_EXPORT TstGenerator : public DPImageServicer, public Plugins::ICommonInterface
{
    Q_OBJECT
    Q_INTERFACES(DPImageServicer Plugins::ICommonInterface)

public:

    TstGenerator(QObject *parent = 0);

    qint64 imageCount() const;
    QImage imageForindex(int index) const;

};

#endif // TSTGENERATOR_H
