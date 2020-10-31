#ifndef EXPLORERMODEL_H
#define EXPLORERMODEL_H

#include "DocumentManager_global.h"

#include <QObject>
#include <QAbstractItemModel>

namespace Docs {

class ExplorerModelPrivate;
class Node;
class CatalogNode;
class IDocumentGenerator;


class DOCUMENTSSHARED_EXPORT ExplorerModel : public QAbstractItemModel
{
    Q_OBJECT
public:
    explicit ExplorerModel(QObject *parent = 0);
    ~ExplorerModel();

    QModelIndex index(int row, int column, const QModelIndex &parent) const;
    QModelIndex parent(const QModelIndex &child) const;

//   reimplemented
    int rowCount(const QModelIndex &parent) const;
    int columnCount(const QModelIndex &parent) const;
    QVariant data(const QModelIndex &index, int role) const;
    Node *nodeFromIndex(const QModelIndex &pIndex) const;
    void registerGenerator(IDocumentGenerator *pGenerator);
    QModelIndex firstGeneratorIndex() const;
    
private:
    ExplorerModelPrivate *d;

    friend class ExplorerModelPrivate;
};

} //namespace Docs
#endif // EXPLORERMODEL_H
