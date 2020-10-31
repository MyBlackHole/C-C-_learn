/***************************************************************************
 *   This file is part of the CuteReport project                           *
 *   Copyright (C) 2012-2014 by Alexander Mikhalov                         *
 *   alexander.mikhalov@gmail.com                                          *
 *                                                                         *
 **                   GNU General Public License Usage                    **
 *                                                                         *
 *   This library is free software: you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation, either version 3 of the License, or     *
 *   (at your option) any later version.                                   *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program.  If not, see <http://www.gnu.org/licenses/>. *
 *                                                                         *
 *                                                                         *
 *   This library is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 ***************************************************************************/
#include "pageeditor.h"
#include "pageeditorcontainer.h"
#include "propertyeditor.h"
#include "pageinterface.h"
#include "reportinterface.h"
#include "reportplugininterface.h"
#include "iteminterface.h"
#include "bandinterface.h"
#include "objectinspector.h"
#include "reportcore.h"
#include "renamedialog.h"
#include "mainwindow.h"
#include "fonteditor.h"
#include "alignmenteditor.h"
#include "frameeditor.h"

#include <QLabel>
#include <QMenu>

const char * MODULENAME = "PageEditor";

using namespace CuteDesigner;

inline void initMyResource() { Q_INIT_RESOURCE(page_editor); }


bool pageOrderLessThan(CuteReport::PageInterface * p1, CuteReport::PageInterface * p2)
{
    return p1->order() < p2->order();
}


PageEditor::PageEditor(QObject *parent) :
    ModuleInterface(parent),
    m_currentPage(0),
    m_activeObject(0),
    m_currentManipulator(0),
    m_isActive(false),
    m_blockPageOrders(false),
    m_copyPaste(0)
{
}


PageEditor::~PageEditor()
{
    delete ui;
    delete m_copyPaste;
}


void PageEditor::init(Core *core)
{
    initMyResource();
    ModuleInterface::init(core);

    if (core->getSettingValue("PageEditor/tabMode").isNull())
        core->setSettingValue("PageEditor/tabMode", 2);

    ui = new PageEditorContainer(this);
    ui->init();
    ui->addPagePlugins(core->reportCore()->modules(CuteReport::PageModule));

    m_propertyEditor = core->createPropertyEditor(ui);
    ui->addPropertyEditor(m_propertyEditor);
    m_objectInspector = new ObjectInspector(ui);
    ui->addObjectInspector(m_objectInspector);

    m_stdActions = Core::StdActions(Core::ActionCopy | Core::ActionPaste);

    foreach (CuteReport::ReportPluginInterface* plugin, core->reportCore()->modules(CuteReport::ItemModule)) {
        CuteReport::BaseItemInterface* itemPlugin = reinterpret_cast<CuteReport::BaseItemInterface*>(plugin);
        if (dynamic_cast<CuteReport::BandInterface*>(itemPlugin))
            ui->addItem(itemPlugin->itemIcon(), itemPlugin->moduleShortName(), itemPlugin->suitName(), itemPlugin->itemGroup());
    }

    foreach (CuteReport::ReportPluginInterface* plugin, core->reportCore()->modules(CuteReport::ItemModule)) {
        CuteReport::BaseItemInterface* itemPlugin = reinterpret_cast<CuteReport::BaseItemInterface*>(plugin);
        if (!dynamic_cast<CuteReport::BandInterface*>(itemPlugin)) {
            ui->addItem(itemPlugin->itemIcon(), itemPlugin->moduleShortName(), itemPlugin->suitName(),  itemPlugin->itemGroup());
        }
    }

    //connect(core, SIGNAL(loadReport_after(CuteReport::ReportInterface*)), this, SLOT(slotReportCreated(CuteReport::ReportInterface*)));
    //connect(core, SIGNAL(newReport_after(CuteReport::ReportInterface*)), this, SLOT(slotReportCreated(CuteReport::ReportInterface*)));
    connect(core, SIGNAL(currentReportChanged(CuteReport::ReportInterface*)), this, SLOT(slotReportChanged(CuteReport::ReportInterface*)));
    connect(m_objectInspector, SIGNAL(objectChanged(QObject*)), this, SLOT(slotActiveObjectChanged(QObject*)));
    connect(m_objectInspector, SIGNAL(selectionChanged()), this, SLOT(slotSelectionChanged()));

    connect(ui.data(), SIGNAL(requestForCreatePage(QString)), this, SLOT(slotRequestForCreatePage(QString)));
    connect(ui.data(), SIGNAL(requestForClonePage(QString)), this, SLOT(slotRequestForClonePage(QString)));
    connect(ui.data(), SIGNAL(requestForDeletePage(QString)), this, SLOT(slotRequestForDeletePage(QString)));
    connect(ui.data(), SIGNAL(currentTabChanged(QString)), this, SLOT(slotCurrentPageChangedByGUI(QString)));
    connect(ui.data(), SIGNAL(requestForRenamePage(QString)), this, SLOT(slotRequestForRenamePage(QString)));

    ui->setEnabled(core->currentReport());
}


void PageEditor::reloadSettings()
{
    if (ui)
        ui->reloadSettings();
}


void PageEditor::saveSettings()
{
    if (ui)
        ui->saveSettings();
}


void PageEditor::activate()
{
    core()->reportCore()->log(CuteReport::LogDebug, MODULENAME, "activate");
    if (m_currentPage && m_currentManipulator)
        foreach (QLabel * label, m_currentManipulator->statusBarLabels()) {
            label->show();
            m_core->addToStatusBar(label);
        }

    m_isActive = true;
}


void PageEditor::deactivate()
{
    core()->reportCore()->log(CuteReport::LogDebug, MODULENAME, "deactivate");
    if (m_currentPage && m_currentManipulator) {
        foreach (QLabel * label, m_currentManipulator->statusBarLabels())
            label->hide();
    }
    m_isActive = false;
}


void PageEditor::sync()
{
    // no need to do something
    // changes commit immediately
}


QWidget * PageEditor::view()
{
    //    if (!ui)
    //        ui = new PageEditorContainer;
    return ui;
}


QIcon PageEditor::icon()
{
    return QIcon(":images/editor_48.png");
}


QString PageEditor::name()
{
    return QString("Pages");
}


QList<DesignerMenu *> PageEditor::mainMenu()
{
    QWidget * parent = core()->mainWindow();

    QList<CuteDesigner::DesignerMenu*> menus;

    CuteDesigner::DesignerMenu * reportMenu = new CuteDesigner::DesignerMenu(parent, "Page", 400, 400); // very high priority;  very high priority;
    menus.append(reportMenu);

    reportMenu->menu->addAction(createAction("actionNewPage", "New Page", ":/images/document-new.png", "Alt+P, Alt+N", SLOT(slotNewPage())));
    reportMenu->menu->addAction(createAction("actionDeletePage", "Delete Page", ":/images/document-close.png", "Alt+P, Alt+D", SLOT(slotDeletePage())));

    return menus;
}


Core::StdActions PageEditor::stdActions()
{
    return Core::StdActions(Core::ActionCopy | Core::ActionPaste);
}


void PageEditor::stdActionTriggered(Core::StdAction actionType, QAction *action)
{
    qDebug() << actionType;

    switch (actionType) {
        case Core::ActionCopy: {
                delete m_copyPaste;
                m_copyPaste = new CopyPasteStruct;
                CuteReport::BaseItemInterface * item = dynamic_cast<CuteReport::BaseItemInterface *> (m_activeObject.data());
                m_copyPaste->page = m_currentPage ? m_currentPage->objectName() : QString();
                m_copyPaste->item = item ? item->objectName() : QString();
                m_copyPaste->cut = false;
                if (item)
                    m_copyPaste->data = core()->reportCore()->serialize(item);
                else {      // only for items implemented
                    delete m_copyPaste;
                    m_copyPaste = 0;
                }
            }
            break;
        case Core::ActionPaste: {
                if (!m_copyPaste || !m_currentReport)
                    return;
                QObject * object = core()->reportCore()->deserialize(m_copyPaste->data);
                CuteReport::BaseItemInterface * item = dynamic_cast<CuteReport::BaseItemInterface *> (object);
                CuteReport::BaseItemInterface * currentItem = dynamic_cast<CuteReport::BaseItemInterface *> (m_activeObject.data());
                if (!item || !currentItem) {
                    //TODO log add
                    delete object;
                    return;
                }

                if (currentItem->objectName() == item->objectName())
                    currentItem = currentItem->parentItem();

                if (!currentItem) {
                    //TODO log add
                    delete object;
                    return;
                }

                CuteReport::PageInterface * page = currentItem->page();
                item->setParentItem(currentItem);
                item->setObjectName( core()->reportCore()->uniqueName(object, object->objectName(), m_currentReport));
                item->init();
                QRectF geom = item->absoluteGeometry(CuteReport::Millimeter);
                geom.moveTopLeft(QPointF(qrand()% 10, qrand()% 10));
                item->setAbsoluteGeometry(geom);
                page->addItem(item);
            }
    }

}


void PageEditor::slotActiveObjectChanged(QObject * object)
{
    if (object && object == m_activeObject)
        return;

    if (m_activeObject && dynamic_cast<CuteReport::BaseItemInterface *> (m_activeObject.data())) {
        disconnect(m_activeObject, SIGNAL(parentItemChanged(CuteReport::BaseItemInterface*)), this,  SLOT(slotUpdateObjectInspector()));
    } /*else if (m_activeObject && dynamic_cast<CuteReport::PageInterface *> (m_activeObject.data())) {
        disconnect(m_activeObject, SIGNAL(afterItemRemoved(CuteReport::BaseItemInterface*,QString,bool)), this, SLOT(slotUpdateObjectInspector()));
    }*/

    m_activeObject = object;

    if (object)
        qDebug() << object->objectName();

    m_propertyEditor->setObject(object);

    if (!qobject_cast<ObjectInspector *> (sender())) {
        m_objectInspector->blockSignals(true);
        m_objectInspector->setRootObject(object ? m_currentPage : 0);
        if (m_currentPage) {
            foreach(CuteReport::BaseItemInterface * item, m_currentPage->selectedItems())
                m_objectInspector->selectObject(item, QItemSelectionModel::Select);
        }
        m_objectInspector->blockSignals(false);
    }

    CuteReport::BaseItemInterface * item = qobject_cast<CuteReport::BaseItemInterface *> (object);
    if (m_currentPage && item) {
        m_currentPage->setCurrentItem(item);
        connect(item, SIGNAL(parentItemChanged(CuteReport::BaseItemInterface*)), this, SLOT(slotUpdateObjectInspector()));
    }

    /*else {
        CuteReport::PageInterface * page = qobject_cast<CuteReport::PageInterface *> (object);
        if (page) {
            connect(page, SIGNAL(afterItemRemoved(CuteReport::BaseItemInterface*,QString,bool)), this, SLOT(slotUpdateObjectInspector()));
        }
    }*/

    updateStdEditors();
}


void PageEditor::slotUpdateObjectInspector()
{
    m_objectInspector->setRootObject(m_currentPage);
    m_objectInspector->selectObject(m_activeObject);
}


void PageEditor::slotSelectionChanged()
{
    QList<CuteReport::BaseItemInterface *> list;
    foreach (QObject * object, m_objectInspector->selectedObjects()) {
        CuteReport::BaseItemInterface * item = qobject_cast<CuteReport::BaseItemInterface *>(object);
        if (item)
            list.append(item);
    }
    if (m_currentPage)
        m_currentPage->setSelectedItems(list);
}


void PageEditor::slotReportChanged(CuteReport::ReportInterface * report)
{
    ui->setEnabled(report);
    ui->removeAllTabs();
    foreach (CuteReport::PageInterface * page, m_pages) {
        if (page)
            page->disconnect(this, 0);
    }
    m_pages.clear();
    m_pageNames.clear();
    if (m_currentReport) {
        m_currentReport->disconnect(this, 0);
    }

    if (report) {
        QList<CuteReport::PageInterface *> pages = report->pages();
        qSort(pages.begin(), pages.end(), pageOrderLessThan);
        foreach (CuteReport::PageInterface * page, pages) {
            _processNewPage(page);
        }
        slotChangeCurrentPage( report->pages().count() ? report->pages()[0] : 0);
        if (m_currentPage)
            ui->setCurrentTab(m_currentPage->objectName());
        connect(report, SIGNAL(pageAdded(CuteReport::PageInterface*)), this, SLOT(slotPageCreatedOutside(CuteReport::PageInterface*)));
        connect(report, SIGNAL(pageDeleted(CuteReport::PageInterface*)), this, SLOT(slotPageDeletedOutside(CuteReport::PageInterface*)));
    } else {
        m_currentPage = 0;
        m_activeObject = 0;
        m_objectInspector->setRootObject(0);
        m_propertyEditor->setObject(0);
    }
    m_currentReport = report;
}


void PageEditor::slotRequestForCreatePage(QString moduleName)
{
    CuteReport::PageInterface * page = core()->reportCore()->createPageObject(moduleName, core()->currentReport());
    if (!page)
        return;
    page->init();
    core()->currentReport()->addPage(page);
}


void PageEditor::slotPageCreatedOutside(CuteReport::PageInterface *page)
{
    if (!page || m_pages.contains(page))
        return;
    _processNewPage(page);
    slotChangeCurrentPage(page);
    ui->setCurrentTab(page->objectName());
}


void PageEditor::slotRequestForDeletePage(QString pageName)
{
    CuteReport::PageInterface * page = core()->reportCore()->pageByName(pageName, core()->currentReport());
    if (!page)
        return;

    QList<CuteReport::PageInterface*> pages = core()->currentReport()->findChildren<CuteReport::PageInterface*>();
    int index = -1;
    for (int i=0; i<pages.count(); i++)
        if (pages[i]->objectName() == pageName) {
            index = i;
            break;
        }

    ui->removeTab(pageName);
    //    int index = m_pages.indexOf(page);
    m_pages.removeAt(index);
    m_pageNames.removeAt(index);
    m_currentReport->deletePage(page);
    //    m_currentPage = 0;

    //    pages = core()->currentReport()->findChildren<CuteReport::PageInterface*>();

    //    if (index > pages.count() -1)
    //        index = pages.count() -1;
    //    if (index < 0)
    //        index = 0;

    //    slotChangeCurrentPage( pages.count() ? pages[index] : 0);

    //    if (m_currentPage)
    //        ui->setCurrentTab(m_currentPage->objectName());
}


void PageEditor::slotPageDeletedOutside(CuteReport::PageInterface *page)
{
    int index = m_pages.indexOf(page);
    m_pages.removeAt(index);
    m_pageNames.removeAt(index);
    ui->removeTab(page->objectName());
    if (m_currentPage == page) {
        m_currentPage = 0;
        int index = -1;
        if (index > m_pages.count() -1)
            index = m_pages.count() -1;
        if (index < 0)
            index = 0;
        slotChangeCurrentPage( m_pages.count() ? m_pages[index] : 0);
        if (m_currentPage)
            ui->setCurrentTab(m_currentPage->objectName());
    }
}


void PageEditor::slotRequestForClonePage(QString pageName)
{
    Q_UNUSED(pageName)
}


void PageEditor::slotCurrentPageChangedByGUI(QString pageName)
{
    CuteReport::PageInterface * newPage = core()->reportCore()->pageByName(pageName, core()->currentReport());
    if (newPage != m_currentPage) {
        slotChangeCurrentPage(newPage);
        core()->setCurrentPage(newPage);
    }
}


void PageEditor::slotCurrentPageChangedByCore(CuteReport::PageInterface* page)
{
    if (page == m_currentPage)
        return;

    slotChangeCurrentPage(page);
    ui->setCurrentTab(page->objectName());
}


void PageEditor::slotRequestForRenamePage(QString pageName)
{
    CuteReport::PageInterface * page = core()->reportCore()->pageByName(pageName, core()->currentReport());

    if (page) {
        RenameDialog d(page, core()->currentReport(), core()->mainWindow());
        d.setWindowTitle("Page renaming");
        if (d.exec() == QDialog::Accepted) {
            page->setObjectName(d.newName());
            //            ui->setNewPageName( pageName, d.newName());
        }
    }
}


void PageEditor::slotPageNameChangedOutside(const QString & name)
{
    Q_UNUSED(name);
    CuteReport::PageInterface * page = qobject_cast<CuteReport::PageInterface*>(sender());
    Q_ASSERT(page);
    int index = m_pages.indexOf(page);
    QString newName = page->objectName();
    QString oldName = m_pageNames.at(index);
    ui->setNewPageName(oldName, newName);
    m_pageNames[index] = newName;
}


void PageEditor::slotNewPage()
{
    QStringList modules = core()->reportCore()->moduleNames(CuteReport::PageModule);

    if (modules.size() == 0) {
        CuteReport::ReportCore::log(CuteReport::LogWarning, MODULENAME, "There is over no page modules");
        return;
    }

    if (modules.size() > 1)
        CuteReport::ReportCore::log(CuteReport::LogWarning, MODULENAME, "There are over 1 page modules.");

    slotRequestForCreatePage(modules.at(0));
}


void PageEditor::slotDeletePage()
{
    if (!m_currentPage)
        return;

    slotRequestForDeletePage(m_currentPage->objectName());
}


void PageEditor::slotPageMoveFront()
{
    if (!m_currentReport)
        return;
    QList<CuteReport::PageInterface *> pages = m_currentReport->pages();
    if (pages.size() < 1)
        return;
    qSort(pages.begin(), pages.end(), pageOrderLessThan);
    int curIndex = pages.indexOf(m_currentPage);
    if (curIndex == 0)
        return;
    CuteReport::PageInterface * otherPage = pages.at(curIndex-1);
    m_blockPageOrders = true;
    int otherPageOrder = otherPage->order();
    otherPage->setOrder(m_currentPage->order());
    m_currentPage->setOrder(otherPageOrder);

    pages.swap(otherPageOrder, curIndex);
    ui->removeAllTabs();
    foreach (CuteReport::PageInterface * page, pages)
        ui->addTab(page->createView(), page->icon(), page->objectName());
    ui->setCurrentTab(m_currentPage->objectName());

    m_blockPageOrders = false;
}


void PageEditor::slotPageMoveBack()
{
    if (!m_currentReport)
        return;
    QList<CuteReport::PageInterface *> pages = m_currentReport->pages();
    if (pages.size() < 1)
        return;
    qSort(pages.begin(), pages.end(), pageOrderLessThan);
    int curIndex = pages.indexOf(m_currentPage);
    if (curIndex == pages.size()-1)
        return;
    CuteReport::PageInterface * otherPage = pages.at(pages.size()-1);
    m_blockPageOrders = true;
    int otherPageOrder = otherPage->order();
    otherPage->setOrder(m_currentPage->order());
    m_currentPage->setOrder(otherPageOrder);

    pages.swap(otherPageOrder, curIndex);
    ui->removeAllTabs();
    foreach (CuteReport::PageInterface * page, pages)
        ui->addTab(page->createView(), page->icon(), page->objectName());
    ui->setCurrentTab(m_currentPage->objectName());

    m_blockPageOrders = false;
}


//void PageEditor::slotReportCreated(CuteReport::ReportInterface * report)
//{
//    connect(report, SIGNAL(pageAdded(CuteReport::PageInterface*)), this
//}


void PageEditor::_processNewPage(CuteReport::PageInterface *page)
{
    if (!page)
        return;
    m_pages.append(page);
    m_pageNames.append(page->objectName());
    ui->addTab(page->createView(), page->icon(), page->objectName());
    connect(page, SIGNAL(objectNameChanged(QString)), this, SLOT(slotPageNameChangedOutside(QString)));
}


void PageEditor::setStdActions(Core::StdActions actions)
{
    m_stdActions = actions;
    stdActionsChanged(m_stdActions);
}


void PageEditor::slotChangeCurrentPage(CuteReport::PageInterface* page)
{
    if (m_currentPage) {
        disconnect(m_currentPage, SIGNAL(activeObjectChanged(QObject*)), this, SLOT(slotActiveObjectChanged(QObject*)));
        if (m_currentManipulator)
            foreach (QLabel * label, m_currentManipulator->statusBarLabels())
                label->hide();
    }

    m_currentPage = page;

    if (!m_currentPage) {
        slotActiveObjectChanged(0);
        if (m_currentManipulator)
            m_currentManipulator->setActivePage(0);
        return;
    }

    connect(m_currentPage, SIGNAL(activeObjectChanged(QObject*)), this, SLOT(slotActiveObjectChanged(QObject*)));
    connect(m_currentPage, SIGNAL(afterItemRemoved(CuteReport::BaseItemInterface*, QString, bool)), this, SLOT(slotUpdateObjectInspector()));

    /// manage manipulator
    CuteReport::PageManipulatorInterface * oldManipulator = m_currentManipulator;
    if (!m_currentManipulator) {
        m_currentManipulator = m_currentPage->createManupulator(this);
        m_pageManipulators.insert(m_currentManipulator->pageManupilatorId(), m_currentManipulator);
    } else
        if (m_currentManipulator && m_currentPage->pageManupilatorID() != m_currentManipulator->pageManupilatorId()) {
            if (m_pageManipulators.contains(m_currentPage->pageManupilatorID())) {
                m_currentManipulator = m_pageManipulators.value(m_currentPage->pageManupilatorID());
            } else {
                m_currentManipulator = m_currentPage->createManupulator(this);
                m_pageManipulators.insert(m_currentManipulator->pageManupilatorId(), m_currentManipulator);
            }
        }

    if (m_currentManipulator) {
        m_currentManipulator->setActivePage(m_currentPage);

        if (m_isActive)
            foreach (QLabel * label, m_currentManipulator->statusBarLabels()) {
                label->show();
                m_core->addToStatusBar(label);
            }

        if (oldManipulator != m_currentManipulator)
            ui->setPageActions(m_currentManipulator->actions());
    }

    slotActiveObjectChanged(m_currentPage->currentItem() ? (QObject*)m_currentPage->currentItem() : (QObject*)m_currentPage);
}


void PageEditor::updateStdEditors()
{
    CuteReport::StdEditorPropertyList list;
    //QObject * object = (m_currentPage && m_currentPage->currentItem()) ? (QObject*)m_currentPage->currentItem() : (QObject*)m_currentPage;
    if (CuteReport::BaseItemInterface * bItem = qobject_cast<CuteReport::BaseItemInterface*>(m_activeObject))
        list = bItem->stdEditorList();
    else if (CuteReport::PageInterface * page = qobject_cast<CuteReport::PageInterface*>(m_activeObject))
        list = page->stdEditorList();

    bool fontEditor = false;
    bool alignmentEditor = false;
    bool frameEditor = false;

    foreach (const CuteReport::StdEditorProperty & ed, list) {
        switch (ed.first) {
            case CuteReport::EDFont: ui->fontEditor()->setFontPropertyName(ed.second); fontEditor = true; break;
            case CuteReport::EDFontColor: ui->fontEditor()->setColorPropertyName(ed.second); fontEditor = true; break;
            case CuteReport::EDTextAlignment: ui->alignmentEditor()->setAlignPropertyName(ed.second); alignmentEditor = true; break;
            case CuteReport::EDFrame: ui->frameEditor()->setFramePropertyName(ed.second); frameEditor = true; break;
        }
    }

    QObjectList selection;

    if (m_currentPage && (fontEditor || alignmentEditor || frameEditor) ) {
        foreach (CuteReport::BaseItemInterface * item, m_currentPage->selectedItems()) {
            selection << static_cast<QObject*>(item);
        }
    }

    if (fontEditor) {
        if (!m_currentPage || selection.size() < 2)
            ui->fontEditor()->setObject(m_activeObject);
        else
            ui->fontEditor()->setObjectList(selection);
        ui->fontEditor()->update();
    } else {
        ui->fontEditor()->clear();
    }
    ui->fontEditor()->setEnabled(fontEditor);

    if (alignmentEditor) {
        if (!m_currentPage || selection.size() < 2)
            ui->alignmentEditor()->setObject(m_activeObject);
        else
            ui->alignmentEditor()->setObjectList(selection);
        ui->alignmentEditor()->update();
    } else {
        ui->alignmentEditor()->clear();
    }
    ui->alignmentEditor()->setEnabled(alignmentEditor);

    if (frameEditor) {
        ui->frameEditor()->setObject(m_activeObject);
        ui->frameEditor()->update();
    } else {
        ui->frameEditor()->clear();
    }
    ui->frameEditor()->setEnabled(frameEditor);
}


#if QT_VERSION < 0x050000
Q_EXPORT_PLUGIN2(PageEditor, PageEditor)
#endif
