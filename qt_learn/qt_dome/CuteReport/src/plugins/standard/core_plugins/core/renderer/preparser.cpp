#include "preparser.h"

#include <QRegExp>

SUIT_BEGIN_NAMESPACE

PreParser::PreParser()
{
}

void PreParser::registerScriptObjects(ScriptEngine *scriptEngine)
{

}


bool PreParser::mainScriptPreprocess(QString &script, QStringList *errors)
{
    Q_UNUSED(errors);
    datasetFieldReplace(script);
    return true;
}


bool PreParser::initialItemScriptPreprocess(QString &script, const CuteReport::BaseItemInterface *item, QStringList *errors)
{
    Q_UNUSED(item);
    Q_UNUSED(errors);
    datasetFieldReplace(script);
    return true;
}


bool PreParser::itemScriptPreprocess(QString &script, const CuteReport::BaseItemInterface *item, QStringList *errors)
{
    Q_UNUSED(item);
    Q_UNUSED(errors);
    datasetFieldReplace(script);
    return true;
}


QString PreParser::itemScriptPostprocess(const QScriptValue &value, const CuteReport::BaseItemInterface *item, QStringList *errors)
{
    return QString();
}


void PreParser::datasetFieldReplace(QString &script)
{
    QRegExp rx("(\\w+)\\.\\\"(.*)\\\"");
    rx.setMinimal(true);

    int pos = 0;
    while ((pos = rx.indexIn(script, pos)) != -1) {
        int length = rx.matchedLength();

        QString result = QString("%1.getValue(\"%2\")").arg(rx.cap(1)).arg(rx.cap(2));

        script.replace(pos, length, result);
        pos += result.length();
    }
}


SUIT_END_NAMESPACE
