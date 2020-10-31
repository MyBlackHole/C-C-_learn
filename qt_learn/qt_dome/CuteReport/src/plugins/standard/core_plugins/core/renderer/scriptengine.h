#ifndef SCRIPTENGINE_H
#define SCRIPTENGINE_H

#include "rendererprocessor.h"
#include "plugins_common.h"

#include <QScriptEngine>

USING_SUIT_NAMESPACE

SUIT_BEGIN_NAMESPACE
class ScriptEngine : public QScriptEngine
{
    Q_OBJECT
public:
    explicit ScriptEngine(RendererProcessor *processor);

    RendererProcessor * processor() {return m_processor;}
private:
    RendererProcessor *m_processor;
};

SUIT_END_NAMESPACE

#endif // SCRIPTENGINE_H
