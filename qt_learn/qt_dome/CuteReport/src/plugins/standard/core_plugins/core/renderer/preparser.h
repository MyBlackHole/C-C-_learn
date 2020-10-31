#ifndef PREPARSER_H
#define PREPARSER_H

#include <QString>
#include <QScriptValue>

#include "plugins_common.h"

namespace CuteReport {
class DatasetInterface;
class BandInterface;
class ReportInterface;
class BaseItemInterface;
}

SUIT_BEGIN_NAMESPACE
class ScriptEngine;
SUIT_END_NAMESPACE

USING_SUIT_NAMESPACE

SUIT_BEGIN_NAMESPACE
class PreParser
{
public:
    PreParser();

    void registerScriptObjects(ScriptEngine * scriptEngine);
    bool mainScriptPreprocess(QString & script, QStringList *errors = 0);

    bool initialItemScriptPreprocess(QString &script, const CuteReport::BaseItemInterface* item, QStringList * errors);
    bool itemScriptPreprocess(QString &script, const CuteReport::BaseItemInterface *item, QStringList * errors);
    QString itemScriptPostprocess(const QScriptValue &value, const CuteReport::BaseItemInterface* item, QStringList *errors);

private:
    void datasetFieldReplace(QString & script);

};

SUIT_END_NAMESPACE
#endif // PREPARSER_H
