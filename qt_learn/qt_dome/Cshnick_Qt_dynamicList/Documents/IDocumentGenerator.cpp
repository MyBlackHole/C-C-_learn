#include "IDocumentGenerator.h"

namespace Docs {
IDocumentGenerator::IDocumentGenerator(QObject *parent)
    : QObject(parent)
{
}
IDocumentGenerator::~IDocumentGenerator()
{
}

void IDocumentGenerator::onNodeChanged(Node *pCurrent, Node *pPrevious)
{
    Q_UNUSED(pCurrent);
    Q_UNUSED(pPrevious);
}

void IDocumentGenerator::onNodeChanged_slot(Node *pCurrent, Node *pPrevious)
{
    onNodeChanged(pCurrent, pPrevious);
}

} //namespace Docs
