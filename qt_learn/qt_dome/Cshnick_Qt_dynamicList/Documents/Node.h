#ifndef DOCNODE_H
#define DOCNODE_H

#include "DocumentManager_global.h"

#include "DocumentManager.h"
#include <QObject>
#include <QMetaType>

namespace Docs {

class CatalogNode;
class GeneratorNode;

enum NodeType {
    NoType = 0
    ,DocumentType
    ,CatalogType
    ,DocumentGeneratorType
    ,RootType
    ,UserType = 100
};

class NodePrivate;
class DOCUMENTSSHARED_EXPORT Node : public QObject
{
    Q_OBJECT

public:
    typedef QList<Node*> ListNode;

    explicit Node(QObject *parent = 0);
    virtual ~Node();
    QVariant data(int role = CustomRole) const;
    void setData(QVariant data, int role = CustomRole);

    CatalogNode *parentNode();
    GeneratorNode *getGeneratorNode();

public: //virtual
    virtual bool isCatalog() const = 0;
    virtual int type() const;
    virtual QString displayName() const = 0;
    virtual QIcon displayIcon() const;
    virtual QFont displayFont() const;
    virtual QBrush backgroundColor() const;
    virtual QBrush foregroundColor() const;
    
private:
    NodePrivate *d;

    friend class NodePrivate;
    friend class CatalogNodePrivate;
};

class CatalogNodePrivate;
class DOCUMENTSSHARED_EXPORT CatalogNode : public Node
{
    Q_OBJECT
public:
    CatalogNode(QObject *parent = 0);
    ~CatalogNode();

    bool isCatalog() const;
    virtual int type() const;
    virtual QString displayName() const;
    virtual QIcon displayIcon() const;
    virtual void addChild(Node *pNode);
    virtual void insertChild(int position, Node *pNode);
    virtual void removechild(int position);
    ListNode childrenNodes();

private:
    CatalogNodePrivate *d;

    friend class CatalogNodePrivate;

};

class DOCUMENTSSHARED_EXPORT GeneratorNode : public CatalogNode
{
    Q_OBJECT

public:
    GeneratorNode(IDocumentGenerator *pGenerator);
    void setDocGenerator(IDocumentGenerator *other) {mGenerator = other;}
    IDocumentGenerator *docGenerator() {return mGenerator;}
    int type() const;

    QString displayName() const;
    QIcon displayIcon() const;

private:
    GeneratorNode(QObject *) {;}
    GeneratorNode() {;}
    IDocumentGenerator *mGenerator;
};

class RootNode : public CatalogNode {
    Q_OBJECT

public:
    QString displayName() const;
    int type() const;
};


} //namespace Docs
Q_DECLARE_METATYPE(Docs::Node*)

#endif // DOCNODE_H

//class UBDocumentTreeNode
//{
//public:
//    friend class UBDocumentTreeModel;

//    enum Type {
//        Catalog = 0
//        , Document
//    };

//    UBDocumentTreeNode(Type pType, const QString &pName, const QString &pDisplayName = QString(), UBDocumentProxy *pProxy = 0);
//    UBDocumentTreeNode() : mType(Catalog), mParent(0), mProxy(0) {;}
//    ~UBDocumentTreeNode();

//    QList<UBDocumentTreeNode*> children() const {return mChildren;}
//    UBDocumentTreeNode *parentNode() {return mParent;}
//    Type nodeType() const {return mType;}
//    QString nodeName() const {return mName;}
//    QString displayName() const {return mDisplayName;}
//    void setNodeName(const QString &str) {mName = str; mDisplayName = str;}
//    void addChild(UBDocumentTreeNode *pChild);
//    void insertChild(int pIndex, UBDocumentTreeNode *pChild);
//    void removeChild(int index);
//    UBDocumentProxy *proxyData() const {return mProxy;}
//    bool isRoot() {return !mParent;}
//    bool isTopLevel() {return mParent && !mParent->mParent;}
//    UBDocumentTreeNode *clone();
//    QString dirPathInHierarchy();

//private:
//    Type mType;
//    QString mName;
//    QString mDisplayName;
//    UBDocumentTreeNode *mParent;
//    QList<UBDocumentTreeNode*> mChildren;
//    QPointer<UBDocumentProxy> mProxy;
//};
