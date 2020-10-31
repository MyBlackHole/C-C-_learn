#include "scriptengine.h"

SUIT_BEGIN_NAMESPACE

ScriptEngine::ScriptEngine(RendererProcessor *processor) :
    QScriptEngine(processor), m_processor(processor)
{
}

SUIT_END_NAMESPACE
