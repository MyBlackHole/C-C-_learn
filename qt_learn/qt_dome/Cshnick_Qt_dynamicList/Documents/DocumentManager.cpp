#include "DocumentManager.h"

#include <QPushButton>
#include <QSplitter>
#include <QVBoxLayout>
#include <QMenu>
#include <QAction>
#include <QDebug>

#include "PluginManager.h"
#include "PInfoHandler.h"
#include "Node.h"
#include "ThumbnailManager.h"
#include "ExplorerModel.h"
#include "ExplorerView.h"
#include "IDocumentGenerator.h"
#include "ICommonInterface.h"

static const QString expectedGeneratorName = "TstGenerator";

namespace Docs {

class DocumentManagerPrivate
{
public:
    DocumentManagerPrivate(DocumentManager *pq)
        : q(pq)
        , mExplorerModel(0)
        , mExplorerView(0)
        , mTopWidget(0)
        , mPluginBox(0)
        , mPluginsMenu(0)
        , mThumbsManager(0)
    {
        mExplorerModel = new ExplorerModel();
        mExplorerView = new ExplorerView();
        mExplorerView->setModel(mExplorerModel);
        mThumbsManager = new DynPicturesManager();
        setupUi();

        QObject::connect(mExplorerView->selectionModel(), SIGNAL(currentChanged(QModelIndex,QModelIndex))
                         , q, SLOT(selectionChanged_slot(QModelIndex,QModelIndex)));
        QObject::connect(Plugins::PluginManager::instance(), SIGNAL(pluginDynamiclyLoaded(QPluginLoader*))
                         , q, SLOT(onPluginDynamiclyLoaded(QPluginLoader*)));

        QList<IDocumentGenerator*> availGenerators = Plugins::PluginManager::getObjects<IDocumentGenerator*>();
        foreach (IDocumentGenerator *nextGenerator, availGenerators) {
            nextGenerator->createNodeTree();
            registerGenerator(nextGenerator);
        }

    }

    ~DocumentManagerPrivate()
    {
        if (mTopWidget) {
            delete mTopWidget;
        }
        if (mExplorerModel) {
            delete mExplorerModel;
        }

    }
    void registerGenerator(IDocumentGenerator *pGenerator) {
        if (mRegisteredGenerators.contains(pGenerator) && !pGenerator) {
            return;
        }
        mRegisteredGenerators.append(pGenerator);
        mExplorerModel->registerGenerator(pGenerator);
        if (mRegisteredGenerators.count() < 2 && false) {
            mExplorerView->setRootIndex(mExplorerModel->index(0, 0, QModelIndex()));
        } else {
            mExplorerView->setRootIndex(QModelIndex());
        }
        mExplorerView->expandAll();
    }

    void setupUi()
    {
        mTopWidget = new QSplitter();

        QWidget *explorerContainer = new QWidget;

        QVBoxLayout *mainLayout = new QVBoxLayout;
        mPluginsMenu = new QMenu("Plugins", 0);
        Plugins::PluginManager::PInfoList allInfos = Plugins::PluginManager::pluginMetas();
        foreach (Plugins::PInfoHandler nextMeta, allInfos) {
            if (!nextMeta) {
                continue;
            }
            QAction *pluginAction = new QAction(nextMeta.displayName(), mPluginsMenu);
            pluginAction->setCheckable(true);
            pluginAction->setChecked(nextMeta.isEnabled());
            pluginAction->setData(QVariant::fromValue(nextMeta));
            QObject::connect(pluginAction, SIGNAL(toggled(bool)), q, SLOT(actionMenuChecked(bool)));
            mPluginsMenu->addAction(pluginAction);

        }

        QPushButton *mPluginBox = new QPushButton("Plugins");
        mPluginBox->setMenu(mPluginsMenu);
        mainLayout->addWidget(mPluginBox);
        mainLayout->addWidget(mExplorerView);
        mainLayout->setContentsMargins(0, 0, 0, 0);

        explorerContainer->setLayout(mainLayout);

        QSplitter *splitterTop = static_cast<QSplitter*>(mTopWidget);
        splitterTop->addWidget(explorerContainer);
        splitterTop->addWidget(mThumbsManager->widget());
        splitterTop->setStretchFactor(0, 0);
        splitterTop->setStretchFactor(1, 1);
        mThumbsManager->widget()->setVisible(true);
        splitterTop->setGeometry(0, 0, 1066, 600);
    }

    void selectionChanged_slot(const QModelIndex &pNew, const QModelIndex &pOld) {
        Node *oldNode = mExplorerModel->nodeFromIndex(pOld);
        Node *newNode = mExplorerModel->nodeFromIndex(pNew);

        if (oldNode->getGeneratorNode() != newNode->getGeneratorNode()) {
            mThumbsManager->installPageGenerator(newNode->getGeneratorNode()->docGenerator()->thumbServicer());
        }

        newNode->getGeneratorNode()->docGenerator()->onNodeChanged(newNode, oldNode);
        mThumbsManager->reload();
        emit q->nodeChanged(newNode, oldNode);
    }

    void onPluginDynamiclyLoaded(QPluginLoader *newLoader) {
        IDocumentGenerator *generatorCandidate = qobject_cast<IDocumentGenerator*>(newLoader->instance());
        generatorCandidate->createNodeTree();
        registerGenerator(generatorCandidate);
    }

private:
    DocumentManager *q;

    QList<IDocumentGenerator *> mRegisteredGenerators;
    ExplorerModel *mExplorerModel;
    ExplorerView *mExplorerView;
    QWidget *mTopWidget;
    QPushButton *mPluginBox;
    QMenu *mPluginsMenu;
    DynPicturesManager *mThumbsManager;

    friend class DocumentManager;
};


DocumentManager::DocumentManager(QObject *parent)
    : QObject(parent)
    , d(new DocumentManagerPrivate(this))
{
}

DocumentManager::~DocumentManager()
{
    if (d) {
        delete d;
    }
}

void DocumentManager::registerGenerator(IDocumentGenerator *pGenerator)
{
    d->registerGenerator(pGenerator);
}

void DocumentManager::print()
{
    qDebug() << "DocumentmManager print";
}

void DocumentManager::setVisible (bool pVisible)
{
    if (d->mTopWidget) {
        d->mTopWidget->setVisible(pVisible);
    }
}

QWidget *DocumentManager::topWidget() const
{
    return d->mTopWidget;
}

void DocumentManager::actionMenuChecked(bool checked)
{
    QAction *senderAction = qobject_cast<QAction*>(sender());
    if (senderAction) {
        Plugins::PInfoHandler hl = senderAction->data().value<Plugins::PInfoHandler>();
        hl.setEnabled(checked);
        hl.save();
        if (checked && !Plugins::PluginManager::loadPlugin(hl)) {
            qDebug() << "Plugin is not being loaded";
        }

    }
}

void DocumentManager::selectionChanged_slot(const QModelIndex &pNew, const QModelIndex &pOld)
{
    d->selectionChanged_slot(pNew, pOld);
}

void DocumentManager::onPluginDynamiclyLoaded(QPluginLoader *newLoader)
{
    d->onPluginDynamiclyLoaded(newLoader);
}

} //namespace Docs

