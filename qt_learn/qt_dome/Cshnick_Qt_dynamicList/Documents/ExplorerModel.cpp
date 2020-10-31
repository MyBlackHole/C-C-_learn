#include "ExplorerModel.h"

//#include <QFont>
//#include <QBrush>
#include <QtGui>

#include "Node.h"
#include "IDocumentGenerator.h"

namespace Docs {

class ExplorerModelPrivate
{
public:
    ExplorerModelPrivate(ExplorerModel *pq)
        : q(pq)
    {
        mRootNode = new RootNode();
    }
    ~ExplorerModelPrivate()
    {
        if (mRootNode) {
            delete mRootNode;
        }
    }
    QModelIndex index(int row, int column, const QModelIndex &parent) const
    {
        if (!mRootNode || row < 0 || column < 0) {
            return QModelIndex();
        }

        CatalogNode *nodeParent = catalogNodeFromIndex(parent);
        if (!nodeParent || row > nodeParent->childrenNodes().count() - 1) {
            return QModelIndex();
        }

        Node *requiredNode = nodeParent->childrenNodes().at(row);
        if(!requiredNode) {
            return QModelIndex();
        }

        QModelIndex resIndex = q->createIndex(row, column, requiredNode);

        return resIndex;
    }

    QModelIndex parent(const QModelIndex &child) const
    {
        Node *nodeChild = nodeFromIndex(child);
        if (!nodeChild) {
            return QModelIndex();
        }

        CatalogNode *nodeParent = nodeChild->parentNode();
        if (!nodeParent) {
            return QModelIndex();
        }

        CatalogNode *nodePreParent = nodeParent->parentNode();
        if (!nodePreParent) {
            return QModelIndex();
        }

        int row = nodePreParent->childrenNodes().indexOf(nodeParent);

        QModelIndex resIndex = q->createIndex(row, 0, nodeParent);

        return resIndex;
    }

    int rowCount(const QModelIndex &parent) const
    {
        if (parent.column() > 0) {
            return 0;
        }

        CatalogNode *nodeParent = catalogNodeFromIndex(parent);
        if (!nodeParent || !nodeParent->isCatalog()) {
            return 0;
        }

        return nodeParent->childrenNodes().count();
    }

    int columnCount(const QModelIndex &parent) const
    {
        Q_UNUSED(parent)
        return 1;
    }

    Node *nodeFromIndex(const QModelIndex &pIndex) const
    {
        if (pIndex.isValid()) {
            return static_cast<Node*>(pIndex.internalPointer());
        } else {
            return mRootNode;
        }
    }

    CatalogNode *catalogNodeFromIndex(const QModelIndex &pIndex) const
    {
        if (pIndex.isValid()) {
            return static_cast<CatalogNode*>(pIndex.internalPointer());
        } else {
            return mRootNode;
        }
    }

private:
    ExplorerModel *q;
    RootNode *mRootNode;
    QList<IDocumentGenerator *> mRegisteredGenerators;


    friend class ExplorerModel;
    friend class DocumentManagerPrivate;
};

ExplorerModel::ExplorerModel(QObject *parent) :
    QAbstractItemModel(parent)
  , d(new ExplorerModelPrivate(this))
{
}

ExplorerModel::~ExplorerModel()
{
    if (d) {
        delete d;
    }
}

QModelIndex ExplorerModel::index(int row, int column, const QModelIndex &parent) const
{
    return d->index(row, column, parent);
}
QModelIndex ExplorerModel::parent(const QModelIndex &child) const
{
    return d->parent(child);
}
int ExplorerModel::rowCount(const QModelIndex &parent) const
{
    return d->rowCount(parent);
}
int ExplorerModel::columnCount(const QModelIndex &parent) const
{
    return d->columnCount(parent);
}
QVariant ExplorerModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid()) {
        return QVariant();
    }

    Node *dataNode = d->nodeFromIndex(index);
    if (!dataNode)
        return QVariant();

    if (index.column() == 0) {

        switch (role) {
        case (Qt::DecorationRole) :
            return dataNode->displayIcon();
            break;

        case (Qt::DisplayRole) :
            return dataNode->displayName();
            break;

        case (Qt::UserRole +1):
            return QVariant::fromValue(dataNode);
            break;

        case (Qt::FontRole) :
            if (dataNode->displayFont() != QFont()) {
                dataNode->displayFont();
            }
            break;

        case (Qt::ForegroundRole) :
            if (dataNode->backgroundColor().style() != Qt::NoBrush) {
                return dataNode->backgroundColor();
            }
            break;

        case (Qt::BackgroundRole) :
            if (dataNode->foregroundColor() != Qt::NoBrush) {
                return dataNode->foregroundColor();
            }
            break;

        default:
//            return QAbstractItemModel::data(index, role);
            break;
        }
    }

    return QVariant();
}
Node *ExplorerModel::nodeFromIndex(const QModelIndex &pIndex) const
{
    return d->nodeFromIndex(pIndex);
}

void ExplorerModel::registerGenerator(IDocumentGenerator *pGenerator)
{
    beginResetModel();
    d->mRegisteredGenerators.append(pGenerator);
    d->mRootNode->addChild(pGenerator->rootNode());
    endResetModel();
}

QModelIndex ExplorerModel::firstGeneratorIndex() const
{
    if (d->mRegisteredGenerators.count()) {
        return createIndex(0, 0, d->mRegisteredGenerators.first());
    }

    return QModelIndex();
}

} //namespace Docs
