#include "Node.h"

#include <QVariant>
#include <QFont>
#include <QBrush>
#include <QVariant>
#include "DocumentManager.h"
#include "IDocumentGenerator.h"

namespace Docs {

class NodePrivate
{
public:
    NodePrivate(Node *pq)
        : q(pq)
    {
        mParent = 0;
    }

private:
    Node *q;
    QMap<int, QVariant> mData;
    CatalogNode *mParent;

    friend class Node;
    friend class CatalogNodePrivate;
};

Node::Node(QObject *parent) :
    QObject(parent)
  , d(new NodePrivate(this))
{

}

Node::~Node()
{
    if (d) {
        delete d;
    }
}

QVariant Node::data(int role) const
{
    return d->mData.value(role, QVariant());
}

void Node::setData(QVariant data, int role)
{
    d->mData.insert(role, data);
}

CatalogNode *Node::parentNode()
{
    return d->mParent;
}

GeneratorNode *Node::getGeneratorNode()
{
    if (type() == DocumentGeneratorType) {
        return static_cast<GeneratorNode*>(this);
    }

    Node *nextParent = d->mParent;
    while (nextParent && nextParent->type() != DocumentGeneratorType) {
       nextParent = nextParent->d->mParent;
    }

    return qobject_cast<GeneratorNode*>(nextParent);
}

int Node::type() const {
    return NoType;
}

QIcon Node::displayIcon() const
{
    return QIcon();
}

QFont Node::displayFont() const
{
    return QFont();
}

QBrush Node::backgroundColor() const
{
    return QBrush();
}

QBrush Node::foregroundColor() const
{
    return QBrush();
}

class CatalogNodePrivate
{
public:
    CatalogNodePrivate(CatalogNode *pq)
        : q(pq)
    {
    }
    ~CatalogNodePrivate()
    {
        qDeleteAll(mChildrenNodes);
    }

    void insertChild(int position, Node *pNode)
    {
        if (pNode) {
            mChildrenNodes.insert(position, pNode);
            pNode->d->mParent = q;
        }
    }

    void removechild(int position)
    {
        if (position < 0 || position > mChildrenNodes.count() - 1) {
            return;
        }

        Node *curChild = mChildrenNodes[position];
        if (curChild->isCatalog()) {
            CatalogNode *nextCatalog = static_cast<CatalogNode*>(curChild);
            while (nextCatalog->d->mChildrenNodes.count()) {
                nextCatalog->removechild(0);
            }
        }

        mChildrenNodes.removeAt(position);
        delete curChild;
    }

private:
    CatalogNode *q;
    Node::ListNode mChildrenNodes;

    friend class CatalogNode;
    friend class NodePrivate;
};

CatalogNode::CatalogNode(QObject *parent) :
    Node(parent)
  , d(new CatalogNodePrivate(this))
{
}

CatalogNode::~CatalogNode()
{
    if (d) {
        delete d;
    }
}

bool CatalogNode::isCatalog() const
{
    return true;
}

int CatalogNode::type() const
{
    return CatalogType;
}

QString CatalogNode::displayName() const
{
    return data(Docs::displayNameRole).toString();
}
QIcon CatalogNode::displayIcon() const
{
    return data(Docs::displayIconRole).value<QIcon>();
}
void CatalogNode::addChild(Node *pNode)
{
    d->insertChild(d->mChildrenNodes.count(), pNode);
}

void CatalogNode::insertChild(int position, Node *pNode)
{
    d->insertChild(position, pNode);
}

void CatalogNode::removechild(int position)
{
    d->removechild(position);
}


Node::ListNode CatalogNode::childrenNodes()
{
    return d->mChildrenNodes;
}

int GeneratorNode::type() const
{
    return DocumentGeneratorType;
}

GeneratorNode::GeneratorNode(IDocumentGenerator *pGenerator)
    :mGenerator(pGenerator)
{

}

QString GeneratorNode::displayName() const
{
     return mGenerator->displayText();
}

QIcon GeneratorNode::displayIcon() const
{
    return mGenerator->icon();
}

QString RootNode::displayName() const
{
    return "root";
}

int RootNode::type() const
{
    return RootType;
}


} //namespace Docs

