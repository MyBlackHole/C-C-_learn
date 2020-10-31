#ifndef TSTDOCGENERATOR1_H
#define TSTDOCGENERATOR1_H

#include "TstDocGenerator1_global.h"

#include <QUrl>
#include "ICommonInterface.h"
#include "IDocumentGenerator.h"
#include "Node.h"
#include "DocumentManager.h"
#include "ThumbnailManager.h"

class QDomDocument;
class DynPicturesManager;

enum {
    DocumentNodeType = Docs::UserType
};

class DocumentNodePrivate;
class DocumentNode : public Docs::Node
{
    Q_OBJECT

public:
    DocumentNode(QObject *parent = 0);
    ~DocumentNode();
    int type() const;

    QString displayName() const;
    QIcon displayIcon() const;
    bool isCatalog() const;
    void setMetadata(const QDomDocument &data);
    void setOwnDir(const QUrl &pDir);
    QUrl ownDir() const;

private:
    DocumentNodePrivate *d;

    friend class DocumentNodePrivate;

};

class TstDocGenerator1Private;
class TSTDOCGENERATOR1SHARED_EXPORT TstDocGenerator1 : public Docs::IDocumentGenerator, public Plugins::ICommonInterface
{
    Q_OBJECT
    Q_INTERFACES(Docs::IDocumentGenerator Plugins::ICommonInterface)

public:
    TstDocGenerator1(QObject *parent = 0);

    QString displayText() const;
    QIcon icon() const;

    void createNodeTree();
    Docs::GeneratorNode *rootNode() const;
    DPImageServicer *thumbServicer();
    void onNodeChanged(Docs::Node *pCurrent, Docs::Node *pPrevious);

private:
    TstDocGenerator1Private *d;

    friend class TstDocGenerator1Private;
};

#endif // TSTDOCGENERATOR1_H
