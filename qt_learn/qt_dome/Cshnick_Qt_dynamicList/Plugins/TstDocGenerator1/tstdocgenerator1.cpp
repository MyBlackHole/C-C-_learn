#include "tstdocgenerator1.h"

#include <QDomDocument>
#include <QFileInfo>
#include <QDir>
#include <QDebug>
#include <QFile>
#include <QList>
#include "Node.h"
#include "ThumbnailManager.h"
#include "PluginManager.h"

static const QString tRDF = "RDF";
static const QString tDescription = "Description";

static const QString tTitle = "title";
static const QString tType = "type";
static const QString tDate = "date";
static const QString tFormat = "format";
static const QString tIdentifer = "indentifer";
static const QString tVersion = "version";
static const QString tSize = "size";
static const QString tUpdatedAt = "updated-at";
static const QString tSessionTitle = "sessionTitle";
static const QString tSessionAuthors = "sessionAuthors";
static const QString tSessionObjectives = "sessionObjectives";
static const QString tSessionKeywords = "sessionKeywords";
static const QString tSessionGradeLevel = "sessionGradeLevel";
static const QString tSessionSubjects = "sessionSubjects";
static const QString tSessionType = "sessionType";
static const QString tSessionLicence = "sessionLicence";

static const QString cTrashName = "_Trash:";
static const QString cMyDocumentsName = "MyDocuments";
static const QString cUntitledDocumentsName = "UntitledDocuments";
static const QString cModelName = "Models";

static const QString rcDocumentIcon = ":Images/marker.png";
static const QString rcCatalogIcon = ":Images/folder.png";
static const QString rcGeneratorIcon = ":Images/uniboard.png";
static const QString rcConstantCatalogIcon = ":Images/ApplicationsCategory.svg";
static const QString rcCurrentDocumentIcon = ":Images/currentDocument.png";

class MetaDataHandler {
public:
    MetaDataHandler(const QDomDocument &doc = QDomDocument())
        : mData(doc)
    {
    }
public:

    QDomElement takeDataContainerElement()
    {
        QDomElement rootElement  = mData.documentElement();
        if (rootElement.tagName() != tRDF) {
            qDebug() << "Firts element within metadata.rdf has't \"RDC\" tag.";
            return QDomElement();
        }
        QDomElement descElement = rootElement.firstChildElement(tDescription);
        if (descElement.isNull()) {
            qDebug() << "There is no element within metadata.rdf that has a \"Description\" tag.";
        }

        return descElement;
    }

    QString queryData(const QString &key)
    {
        QDomElement requiredElement = takeDataContainerElement().firstChildElement(key);
        if (requiredElement.isNull()) {
            qDebug() << "There is no element within metadata.rdf acceptable for the \"" + key + "\" tag.";
            return QString();
        }

        return requiredElement.text();
    }

    void insertData(const QString &key, const QString &data)
    {
        QDomElement descElement = takeDataContainerElement();
        if (descElement.isNull()  ) {
            return;
        }

        QDomElement requiredElement = descElement.firstChildElement(key);
        if (!requiredElement.isNull()) {
            descElement.removeChild(requiredElement);
        }

        requiredElement = mData.createElement(key);
        QDomText text = mData.createTextNode(data);
        requiredElement.appendChild(text);

        descElement.appendChild(requiredElement);
    }

    void print()
    {
        QDomElement nextDataElement = takeDataContainerElement().firstChildElement();
        qDebug() << "Printing metadata";
        while (!nextDataElement.isNull()) {
            qDebug() << "<" << nextDataElement.tagName() << ">" << nextDataElement.text();
            nextDataElement = nextDataElement.nextSiblingElement();
        }
    }

    QString queryName(const QString &defaultValue = QString())
    {
        QString result = queryData(tTitle);
        if (result.isNull()) {
            result = defaultValue;
        }

        return result;
    }

    QString queryDir(const QString &defaultValue = QString())
    {
        QString result = queryData(tType);
        if (result.isNull()) {
            result = defaultValue;
        }

        return result;
    }

    bool isValidMeta() const
    {
        return true;
    }

private:
    QDomDocument mData;
};

class DocumentNodePrivate
{
    MetaDataHandler hl;
    QUrl mOwnDir;

    friend class DocumentNode;
};

DocumentNode::DocumentNode(QObject *parent)
    : Docs::Node(parent)
    , d(new DocumentNodePrivate)
{
}
DocumentNode::~DocumentNode()
{
    if (d) {
        delete d;
    }
}

int DocumentNode::type() const
{
    return DocumentNodeType;
}

QString DocumentNode::displayName() const
{
    return d->hl.queryName("Undefined");
}

QIcon DocumentNode::displayIcon() const
{
    return QIcon(rcDocumentIcon);
}

bool DocumentNode::isCatalog() const
{
    return false;
}

void DocumentNode::setMetadata(const QDomDocument &data)
{
    d->hl = MetaDataHandler(data);
}

void DocumentNode::setOwnDir(const QUrl &pDir)
{
    d->mOwnDir = pDir;
}

QUrl DocumentNode::ownDir() const
{
    return d->mOwnDir;
}

class TstDocGenerator1Private
{
public:
    TstDocGenerator1Private(TstDocGenerator1 *pq)
        : q(pq)
        , mRootNode(0)
        , mMyDocumentsNode(0)
        , mUntitledDocumentsNode(0)
        , mModelsNode(0)
        , mTrashNode(0)
        , mThumbServicer(0)
    {
        mRootPath = QUrl::fromLocalFile("/home/ilia/.local/share/data/Sankore/document");
    }
    ~TstDocGenerator1Private()
    {
    }
    QString metaDataFilePath(const QString &pDir)
    {
        QString result = pDir + "/metadata.rdf";
        Q_ASSERT(QFileInfo(result).exists());
        return result;
    }
    QDomDocument metadataFromDir(const QString metadataDir)
    {
        QString contentFile(metaDataFilePath(metadataDir));
        QDomDocument xmlDom;
        QFile inFile(contentFile);
        if (inFile.open(QIODevice::ReadOnly)) {
            QString domString(inFile.readAll());

            int errorLine = 0; int errorColumn = 0;
            QString errorStr;

            if (xmlDom.setContent(domString, true, &errorStr, &errorLine, &errorColumn)) {
//                loadFolderTreeFromXml("", xmlDom.firstChildElement());
            } else {
                qDebug() << "Error reading content of " << contentFile << endl
                         << "Error:"  << inFile.errorString()
                         << "Line:"   << errorLine
                         << "Column:" << errorColumn;
            }
            inFile.close();
        } else {
            qDebug() << "Error reading" << contentFile << endl
                     << "Error:" << inFile.errorString();
        }

        return xmlDom;
    }

    Docs::CatalogNode *nodeFromDir(const QString &dir)
    {
        QStringList pathList = dir.split("/", QString::SkipEmptyParts);

        if (pathList.isEmpty()) {
            return mUntitledDocumentsNode;
        }

        if (pathList.first() != mMyDocumentsNode->data(Docs::internalNameRole).toString()
                && pathList.first() != mUntitledDocumentsNode->data(Docs::internalNameRole).toString()
                && pathList.first() != mModelsNode->data(Docs::internalNameRole).toString()
                && pathList.first() != mTrashNode->data(Docs::internalNameRole).toString()) {
            pathList.prepend(mMyDocumentsNode->data(Docs::internalNameRole).toString());
        }

        Docs::CatalogNode *parentNode = static_cast<Docs::CatalogNode*>(mRootNode);
        bool searchingNode = true;
        while (!pathList.isEmpty())
        {
            QString curLevelName = pathList.takeFirst();
            if (searchingNode) {
                searchingNode = false;
                for (int i = 0; i < parentNode->childrenNodes().count() && parentNode->childrenNodes().at(i)->isCatalog(); ++i) {
                    Docs::CatalogNode *curChildCatalog = static_cast<Docs::CatalogNode *>(parentNode->childrenNodes().at(i));
                    if (curChildCatalog->data(Docs::internalNameRole).toString() == curLevelName) {
                        searchingNode = true;
                        parentNode = curChildCatalog;
                        break;
                    }
                }
            }

            if (!searchingNode) {
                Docs::CatalogNode *newChild = new Docs::CatalogNode();
                newChild->setData(curLevelName, Docs::internalNameRole);
                newChild->setData(curLevelName, Docs::displayNameRole);
                newChild->setData(QIcon(rcCatalogIcon), Docs::displayIconRole);
                parentNode->addChild(newChild);
                parentNode = newChild;
            }
        }

        return parentNode;
    }

    void createConstantNodes()
    {
        QIcon constIcon = QIcon(rcConstantCatalogIcon);

        mRootNode = new Docs::GeneratorNode(q);
        mMyDocumentsNode = new Docs::CatalogNode;
        mMyDocumentsNode->setData(QObject::tr("My Documents"), Docs::displayNameRole);
        mMyDocumentsNode->setData(cMyDocumentsName, Docs::internalNameRole);
        mMyDocumentsNode->setData(constIcon, Docs::displayIconRole);
        mUntitledDocumentsNode = new Docs::CatalogNode;
        mUntitledDocumentsNode->setData(QObject::tr("Untitled documents"), Docs::displayNameRole);
        mUntitledDocumentsNode->setData(cUntitledDocumentsName, Docs::internalNameRole);
        mUntitledDocumentsNode->setData(constIcon, Docs::displayIconRole);
        mModelsNode = new Docs::CatalogNode;
        mModelsNode->setData(QObject::tr("Models"), Docs::displayNameRole);
        mModelsNode->setData(cModelName, Docs::internalNameRole);
        mModelsNode->setData(constIcon, Docs::displayIconRole);
        mTrashNode = new Docs::CatalogNode;
        mTrashNode->setData(QObject::tr("Trash"), Docs::displayNameRole);
        mTrashNode->setData(cTrashName, Docs::internalNameRole);
        mTrashNode->setData(constIcon, Docs::displayIconRole);

        mRootNode->addChild(mMyDocumentsNode);
            mMyDocumentsNode->addChild(mUntitledDocumentsNode);
        mRootNode->addChild(mModelsNode);
        mRootNode->addChild(mTrashNode);
    }

private:
    TstDocGenerator1 *q;
    Docs::GeneratorNode *mRootNode;
    Docs::CatalogNode *mMyDocumentsNode;
    Docs::CatalogNode *mUntitledDocumentsNode;
    Docs::CatalogNode *mModelsNode;
    Docs::CatalogNode *mTrashNode;

    QUrl mRootPath;
    DPImageServicer *mThumbServicer;


    friend class TstDocGenerator1;

};

TstDocGenerator1::TstDocGenerator1(QObject *parent)
    : Docs::IDocumentGenerator(parent)
    , d(new TstDocGenerator1Private(this))
{

}

QString TstDocGenerator1::displayText() const
{
    return "DocumentGenerator1";
}

QIcon TstDocGenerator1::icon() const
{
    return QIcon(rcGeneratorIcon);
}

void TstDocGenerator1::createNodeTree()
{
    d->createConstantNodes();

    QDir rootDir(d->mRootPath.toLocalFile());
    QFileInfoList contentList = rootDir.entryInfoList(QDir::Dirs | QDir::NoDotAndDotDot, QDir::Time | QDir::Reversed);
    foreach(QFileInfo path, contentList) {
        QString fullPath = path.absoluteFilePath();
        QDomDocument metaData = d->metadataFromDir(fullPath);
        Q_ASSERT(!metaData.isNull());

        MetaDataHandler hl(metaData);
        hl.print();
        if (!hl.isValidMeta()) {
            continue;
        }

        Docs::CatalogNode *nodeDir = d->nodeFromDir(hl.queryDir());
        DocumentNode *docNode = new DocumentNode;
        docNode->setMetadata(metaData);
        docNode->setOwnDir(QUrl::fromLocalFile(fullPath));

        nodeDir->addChild(docNode);
    }

    QList<DPImageServicer*> availServicers = Plugins::PluginManager::getObjects<DPImageServicer*>();
    if (availServicers.count()) {
        d->mThumbServicer = availServicers.first();
    }
}

Docs::GeneratorNode *TstDocGenerator1::rootNode() const
{
    return d->mRootNode;
}

DPImageServicer *TstDocGenerator1::thumbServicer()
{
    return d->mThumbServicer;
}

void TstDocGenerator1::onNodeChanged(Docs::Node *pCurrent, Docs::Node *pPrevious)
{
    Q_UNUSED(pPrevious)
    if (!d->mThumbServicer) {
        return;
    }
    QString data;

    if (pCurrent->type() == DocumentNodeType) {
        DocumentNode *curDocNode = static_cast<DocumentNode*>(pCurrent);
        data = curDocNode->ownDir().toLocalFile();
    }

    d->mThumbServicer->setData(data, Globals::InternalPathRole);
    qDebug() << "internal path role" << d->mThumbServicer->data(Globals::InternalPathRole).toString();
}

Q_EXPORT_PLUGIN2(TstDocGenerator1, TstDocGenerator1)

