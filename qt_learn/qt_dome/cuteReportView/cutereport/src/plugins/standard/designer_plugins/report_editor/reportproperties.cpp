/***************************************************************************
 *   This file is part of the CuteReport project                           *
 *   Copyright (C) 2012-2015 by Alexander Mikhalov                         *
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
#include "reportproperties.h"
#include "ui_reportproperties.h"
#include "reportinterface.h"
#include "reportcore.h"
#include "core.h"
#include "printerinterface.h"
#include "rendererinterface.h"
#include "storageinterface.h"
#include "propertyeditor.h"

#include <QDebug>
#include <QMenu>

namespace PropertyEditor{

ReportProperties::ReportProperties(CuteDesigner::Core * core, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ReportProperties),
    m_core(core),
    m_report(0)
{
    ui->setupUi(this);

//    int maxWidth = qMax(ui->labelPrinting->width(), ui->labelRendering->width());
//    ui->labelPrinting->setMinimumWidth(maxWidth);
//    ui->labelRendering->setMinimumWidth(maxWidth);

//    maxWidth = qMax(ui->renderingModules->width(), ui->printModules->width());
//    ui->renderingModules->setMinimumWidth(maxWidth);
//    ui->printModules->setMinimumWidth(maxWidth);

    ui->tabWidget->setCurrentIndex(0);

    QMenu * menu = new QMenu(ui->bAddStorage);
    foreach (CuteReport::ReportPluginInterface* module, m_core->reportCore()->modules(CuteReport::StorageModule)) {
        QAction * action = new QAction(module->moduleShortName(), menu);
        action->setData(module->moduleFullName());
        connect(action, SIGNAL(triggered()), this, SLOT(setNewStorageModule()));
        menu->addAction(action);
    }
    ui->bAddStorage->setMenu(menu);

    QMenu * menuRenderers = new QMenu(ui->bAddRenderer);
    foreach (CuteReport::ReportPluginInterface* module, m_core->reportCore()->modules(CuteReport::RendererModule)) {
        QAction * action = new QAction(module->moduleFullName().replace("::", " "), menuRenderers);
        action->setData(module->moduleFullName());
        connect(action, SIGNAL(triggered()), this, SLOT(setNewRendererModule()));
        menuRenderers->addAction(action);
    }
    ui->bAddRenderer->setMenu(menuRenderers);

    QMenu * menuPrinters = new QMenu(ui->bAddPrinter);
    foreach (CuteReport::ReportPluginInterface* module, m_core->reportCore()->modules(CuteReport::PrinterModule)) {
        QAction * action = new QAction(module->moduleFullName().replace("::", " "), menuPrinters);
        action->setData(module->moduleFullName());
        connect(action, SIGNAL(triggered()), this, SLOT(setNewPrinterModule()));
        menuPrinters->addAction(action);
    }
    ui->bAddPrinter->setMenu(menuPrinters);


    connect(ui->bDeleteStorage, SIGNAL(clicked()), this, SLOT(deleteCurrentStorage()));
    connect(ui->bDefaultStorage, SIGNAL(clicked()), this, SLOT(setDefaultStorage()));
    connect(ui->bResetDefaultStorage, SIGNAL(clicked()), this, SLOT(clearDefaultStorage()));
    connect(ui->bStorageRename, SIGNAL(clicked()), this, SLOT(renameStorage()));
    connect(ui->storageList, SIGNAL(currentItemChanged(QTreeWidgetItem*,QTreeWidgetItem*)), this, SLOT(storagesListIndexChanged(QTreeWidgetItem*,QTreeWidgetItem*)));

    connect(ui->bDeleteRenderer, SIGNAL(clicked()), this, SLOT(deleteCurrentRenderer()));
    connect(ui->bDefaultRenderer, SIGNAL(clicked()), this, SLOT(setDefaultRenderer()));
    connect(ui->bResetDefaultRenderer, SIGNAL(clicked()), this, SLOT(clearDefaultRenderer()));
    connect(ui->bRendererRename, SIGNAL(clicked()), this, SLOT(renameRenderer()));
    connect(ui->rendererList, SIGNAL(currentItemChanged(QTreeWidgetItem*,QTreeWidgetItem*)), this, SLOT(rendererListIndexChanged(QTreeWidgetItem*,QTreeWidgetItem*)));

    connect(ui->bDeletePrinter, SIGNAL(clicked()), this, SLOT(deleteCurrentPrinter()));
    connect(ui->bDefaultPrinter, SIGNAL(clicked()), this, SLOT(setDefaultPrinter()));
    connect(ui->bResetDefaultPrinter, SIGNAL(clicked()), this, SLOT(clearDefaultPrinter()));
    connect(ui->bPrinterRename, SIGNAL(clicked()), this, SLOT(renamePrinter()));
    connect(ui->printerList, SIGNAL(currentItemChanged(QTreeWidgetItem*,QTreeWidgetItem*)), this, SLOT(printerListIndexChanged(QTreeWidgetItem*,QTreeWidgetItem*)));

    connect(ui->variables, SIGNAL(itemChanged(QTableWidgetItem*)), this, SLOT(variableItemChanged(QTableWidgetItem*)));
}

ReportProperties::~ReportProperties()
{
    delete ui;
}


void ReportProperties::saveSettings()
{
//    m_core->setSettingValue("DatasetEditor/splitterState", ui->splitter->saveState());
//    m_core->setSettingValue("DatasetEditor/splitter2State", ui->splitterTestPage->saveState());
//    m_core->setSettingValue("DatasetEditor/tabMode", ui->datasetTabs->mode());
//    m_core->setSettingValue("DatasetEditor/propertiesShown", ui->bProperties->isChecked());
}


void ReportProperties::reloadSettings()
{
//    ui->splitter->restoreState((m_core->getSettingValue("DatasetEditor/splitterState").toByteArray()));
//    ui->splitterTestPage->restoreState((m_core->getSettingValue("DatasetEditor/splitter2State").toByteArray()));
//    ui->bProperties->setChecked(m_core->getSettingValue("DatasetEditor/propertiesShown").toBool());

//    ui->datasetTabs->SetCurrentIndex(0);
//    FancyTabWidget::Mode default_mode = FancyTabWidget::Mode_LargeSidebar;
//    ui->datasetTabs->SetMode(FancyTabWidget::Mode(m_core->getSettingValue("Preview/tabMode", default_mode).toInt()));
//    ui->stackedWidget->setCurrentIndex(0);
}


void ReportProperties::addRendererPropertyEditor(PropertyEditor::EditorWidget * propertyEditor)
{
    m_rendererPropertyEditor = propertyEditor;
    ui->rendererHelperLayout->addWidget(propertyEditor);
}


void ReportProperties::addPrinterPropertyEditor(PropertyEditor::EditorWidget * propertyEditor)
{
    m_printerPropertyEditor = propertyEditor;
    ui->printerHelperLayout->addWidget(propertyEditor);
}


void ReportProperties::connectReport(CuteReport::ReportInterface * report)
{
    if (m_report)
        disconnectReport();

    m_report = report;

    if (!m_report)
        return;

    ui->url->setText(m_report->fileUrl());
    ui->reportName->setText(m_report->name());
    ui->author->setText(m_report->author());
    ui->description->setText(m_report->description());

//    ui->printModules->clear();
//    ui->renderingModules->clear();
//    ui->printModules->addItems(m_core->reportCore()->moduleNames(CuteReport::PrinterModule));
//    ui->renderingModules->addItems(m_core->reportCore()->moduleNames(CuteReport::RendererModule));

//    ui->rendererName->setText(m_report->renderer() ? m_report->renderer()->moduleFullName() : "");
//    ui->printerName->setText(m_report->printer() ? m_report->printer()->moduleFullName() : "");
//    ui->lGlobalRenderer->setVisible(!m_report->renderer());
//    ui->lGlobalPrinter->setVisible(!m_report->printer());

    foreach (CuteReport::StorageInterface * storage, m_report->storages())
        addGUIReportStorage(storage);
    ui->leDefaultStorage->setText(m_report->defaultStorageName());

    foreach (CuteReport::RendererInterface *renderer, m_report->renderers())
        addGUIReportRenderer(renderer);
    ui->leDefaultRenderer->setText(m_report->defaultRendererName());

    foreach (CuteReport::PrinterInterface * printer, m_report->printers())
        addGUIReportPrinter(printer);

    ui->leDefaultPrinter->setText(m_report->defaultPrinterName());

    connect(m_report.data(), SIGNAL(fileUrlChanged(QString)), this, SLOT(setGUIReportFilePath(QString)));
    connect(m_report.data(), SIGNAL(nameChanged(QString)), this, SLOT(setGUIReportName(QString)));
    connect(m_report.data(), SIGNAL(authorChanged(QString)), this, SLOT(setGUIReportAuthor(QString)));
    connect(m_report.data(), SIGNAL(descriptionChanged(QString)), this, SLOT(setGUIReportDescription(QString)));
    connect(m_report.data(), SIGNAL(storageAdded(CuteReport::StorageInterface*)), this, SLOT(addGUIReportStorage(CuteReport::StorageInterface*)));
    connect(m_report.data(), SIGNAL(storageDeleted(CuteReport::StorageInterface*)), this, SLOT(removeGUIReportStorage(CuteReport::StorageInterface*)));
    connect(m_report.data(), SIGNAL(defaultStorageChanged(QString)), this, SLOT(setGUIDefaultStorage(QString)));
    connect(m_report.data(), SIGNAL(rendererAdded(CuteReport::RendererInterface*)), this, SLOT(addGUIReportRenderer(CuteReport::RendererInterface*)));
    connect(m_report.data(), SIGNAL(rendererDeleted(CuteReport::RendererInterface*)), this, SLOT(removeGUIReportRenderer(CuteReport::RendererInterface*)));
    connect(m_report.data(), SIGNAL(defaultRendererChanged(QString)), this, SLOT(setGUIDefaultRenderer(QString)));
    connect(m_report.data(), SIGNAL(printerAdded(CuteReport::PrinterInterface*)), this, SLOT(addGUIReportPrinter(CuteReport::PrinterInterface*)));
    connect(m_report.data(), SIGNAL(printerDeleted(CuteReport::PrinterInterface*)), this, SLOT(removeGUIReportPrinter(CuteReport::PrinterInterface*)));
    connect(m_report.data(), SIGNAL(defaultPrinterChanged(QString)), this, SLOT(setGUIDefaultPrinter(QString)));

    connect(m_report.data(), SIGNAL(variablesChanged()), this, SLOT(updateGUIvariables()));

    connect(ui->reportName, SIGNAL(editingFinished ()), this, SLOT(saveAll()));
    connect(ui->author, SIGNAL(editingFinished ()), this, SLOT(saveAll()));
    connect(ui->description, SIGNAL(textChanged()), this, SLOT(saveAll()));
}


void ReportProperties::disconnectReport()
{
    if (m_report) {
        disconnect(m_report, 0, this, 0);
    }
}


//void ReportProperties::syncData()
//{
//}


void ReportProperties::setGUIReportFilePath(const QString & url)
{
    if (ui->url->text() != url)
        ui->url->setText(url);
}


void ReportProperties::setGUIReportName(const QString & reportName)
{
    if (ui->reportName->text() != reportName)
        ui->reportName->setText(reportName);
}


void ReportProperties::setGUIReportAuthor(const QString & reportAuthor)
{
    if (ui->author->text() != reportAuthor)
        ui->author->setText(reportAuthor);
}


void ReportProperties::setGUIReportDescription(const QString & reportDescription)
{
    if (ui->description->toPlainText() != reportDescription)
        ui->description->setPlainText(reportDescription);
}


//void ReportProperties::setGUIReportPrinter(CuteReport::PrinterInterface* printer)
//{
//    ui->printerName->blockSignals(true);
//    ui->printerName->setText(printer ? printer->moduleFullName() : "");
//    ui->lGlobalPrinter->setVisible(!printer);
//    m_printerPropertyEditor->setVisible(printer);
//    ui->printerName->blockSignals(false);
//}


//void ReportProperties::setGUIReportRenderer(CuteReport::RendererInterface* renderer)
//{
//    ui->rendererName->blockSignals(true);
//    ui->rendererName->setText(renderer ? renderer->moduleFullName() : "");
//    ui->lGlobalRenderer->setVisible(!renderer);
//    m_rendererPropertyEditor->setVisible(renderer);
//    ui->rendererName->blockSignals(false);
//}


void ReportProperties::addGUIReportStorage(CuteReport::StorageInterface * storage)
{
    QTreeWidgetItem * newTreeItem = new QTreeWidgetItem ( ui->storageList, QStringList() << storage->objectName() << storage->moduleFullName() );
    ui->storageList->addTopLevelItem(newTreeItem);
    newTreeItem->setData(0, Qt::UserRole, (qint64)storage);
    connect(storage, SIGNAL(objectNameChanged(QString)), this, SLOT(slotStorageNameChanged(QString)));
}


void ReportProperties::removeGUIReportStorage(CuteReport::StorageInterface *storage)
{
    qDeleteAll(ui->storageList->findItems(storage->objectName(), Qt::MatchExactly));
    updateLayout();
}


void ReportProperties::setGUIDefaultStorage(const QString & storageName)
{
    ui->leDefaultStorage->setText(storageName);
}


void ReportProperties::addGUIReportRenderer(CuteReport::RendererInterface * object)
{
    QTreeWidgetItem * newTreeItem = new QTreeWidgetItem ( ui->rendererList, QStringList() << object->objectName() << object->moduleFullName() );
    ui->rendererList->addTopLevelItem(newTreeItem);
    newTreeItem->setData(0, Qt::UserRole, (qint64)object);
    connect(object, SIGNAL(objectNameChanged(QString)), this, SLOT(slotRendererNameChanged(QString)));
}


void ReportProperties::removeGUIReportRenderer(CuteReport::RendererInterface *object)
{
    qDeleteAll(ui->rendererList->findItems(object->objectName(), Qt::MatchExactly));
    updateLayout();
}


void ReportProperties::setGUIDefaultRenderer(const QString & objectName)
{
    ui->leDefaultRenderer->setText(objectName);
}


void ReportProperties::addGUIReportPrinter(CuteReport::PrinterInterface * object)
{
    QTreeWidgetItem * newTreeItem = new QTreeWidgetItem ( ui->printerList, QStringList() << object->objectName() << object->moduleFullName() );
    ui->printerList->addTopLevelItem(newTreeItem);
    newTreeItem->setData(0, Qt::UserRole, (qint64)object);
    connect(object, SIGNAL(objectNameChanged(QString)), this, SLOT(slotPrinterNameChanged(QString)));
}


void ReportProperties::removeGUIReportPrinter(CuteReport::PrinterInterface *object)
{
    qDeleteAll(ui->printerList->findItems(object->objectName(), Qt::MatchExactly));
    updateLayout();
}


void ReportProperties::setGUIDefaultPrinter(const QString & objectName)
{
    ui->leDefaultPrinter->setText(objectName);
}



void ReportProperties::updateGUIvariables()
{
    ui->variables->setRowCount(m_report->variables().keys().size());

    int row = -1;
    foreach (const QString & key, m_report->variables().keys()) {
        ++row;
        QTableWidgetItem *newName = new QTableWidgetItem(key);
//        qDebug() << m_report->variables().value(key);
        QTableWidgetItem *newValue = new QTableWidgetItem(m_report->variables().value(key).toString());
        newName->setFlags(Qt::ItemIsEnabled);
        newValue->setFlags(Qt::ItemIsEnabled | Qt::ItemIsEditable | Qt::ItemIsSelectable);
        ui->variables->setItem(row, 0, newName);
        ui->variables->setItem(row, 1, newValue);
    }
}


void ReportProperties::variableItemChanged(QTableWidgetItem * item)
{
    if (!m_report || !item || item->column() != 1)
        return;

    int row = item->row();
    m_report->setVariableValue(ui->variables->item(row,0)->text(), item->text());
}


void ReportProperties::slotStorageNameChanged(QString name)
{
    CuteReport::StorageInterface *object = dynamic_cast<CuteReport::StorageInterface*> (sender());
    if (!object)
        return;
    for (int i=0; i<ui->storageList->topLevelItemCount(); ++i) {
        QTreeWidgetItem * item = ui->storageList->topLevelItem(i);
        if ((qint64)object == item->data(0, Qt::UserRole).toLongLong()) {
            item->setText(0, object->objectName());
            break;
        }
    }
}


void ReportProperties::slotRendererNameChanged(QString name)
{
    CuteReport::RendererInterface *object = dynamic_cast<CuteReport::RendererInterface*> (sender());
    if (!object)
        return;
    for (int i=0; i<ui->rendererList->topLevelItemCount(); ++i) {
        QTreeWidgetItem * item = ui->rendererList->topLevelItem(i);
        if ((qint64)object == item->data(0, Qt::UserRole).toLongLong()) {
            item->setText(0, object->objectName());
            break;
        }
    }
}


void ReportProperties::slotPrinterNameChanged(QString name)
{
    CuteReport::PrinterInterface *object = dynamic_cast<CuteReport::PrinterInterface*> (sender());
    if (!object)
        return;
    for (int i=0; i<ui->printerList->topLevelItemCount(); ++i) {
        QTreeWidgetItem * item = ui->printerList->topLevelItem(i);
        if ((qint64)object == item->data(0, Qt::UserRole).toLongLong()) {
            item->setText(0, object->objectName());
            break;
        }
    }
}


void ReportProperties::setNewRendererModule()
{
    QAction * action = qobject_cast<QAction*>(sender());
    Q_ASSERT(action);
    QString moduleName = action->data().toString();
    const CuteReport::RendererInterface * renderer = static_cast<const CuteReport::RendererInterface*>(m_core->reportCore()->module(CuteReport::RendererModule, moduleName));
    emit requestForNewRenderer(renderer);
}


void ReportProperties::deleteCurrentRenderer()
{
    if (!ui->rendererList->currentItem())
        return;
    QString name = ui->rendererList->currentItem()->text(0);
    emit requestForDeleteRenderer(name);
}


void ReportProperties::setDefaultRenderer()
{
    if (!ui->rendererList->currentItem())
        return;
    QString name = ui->rendererList->currentItem()->text(0);
    emit requestForDefaultRenderer(name);
}


void ReportProperties::clearDefaultRenderer()
{
    emit requestForDefaultRenderer(QString());
}


void ReportProperties::renameRenderer()
{
    if (!ui->rendererList->currentItem())
        return;
    QString name = ui->rendererList->currentItem()->text(0);
    emit requestForNewRendererName(name);
}


void ReportProperties::setNewPrinterModule()
{
    QAction * action = qobject_cast<QAction*>(sender());
    Q_ASSERT(action);
    QString moduleName = action->data().toString();
    const CuteReport::PrinterInterface * printer = static_cast<const CuteReport::PrinterInterface*>(m_core->reportCore()->module(CuteReport::PrinterModule, moduleName));
    emit requestForNewPrinter(printer);
}


void ReportProperties::deleteCurrentPrinter()
{
    if (!ui->printerList->currentItem())
        return;
    QString name = ui->printerList->currentItem()->text(0);
    emit requestForDeletePrinter(name);
}


void ReportProperties::setDefaultPrinter()
{
    if (!ui->printerList->currentItem())
        return;
    QString name = ui->printerList->currentItem()->text(0);
    emit requestForDefaultPrinter(name);
}


void ReportProperties::clearDefaultPrinter()
{
    emit requestForDefaultPrinter(QString());
}


void ReportProperties::renamePrinter()
{
    if (!ui->printerList->currentItem())
        return;
    QString name = ui->printerList->currentItem()->text(0);
    emit requestForNewPrinterName(name);
}


void ReportProperties::setNewStorageModule()
{
    QAction * action = qobject_cast<QAction*>(sender());
    Q_ASSERT(action);
    QString moduleName = action->data().toString();
    const CuteReport::StorageInterface * storage = static_cast<const CuteReport::StorageInterface*>(m_core->reportCore()->module(CuteReport::StorageModule, moduleName));
    emit requestForNewStorage(storage);
}


void ReportProperties::deleteCurrentStorage()
{
    if (!ui->storageList->currentItem())
        return;
    QString storageName = ui->storageList->currentItem()->text(0);
    emit requestForDeleteStorage(storageName);
}


void ReportProperties::setDefaultStorage()
{
    if (!ui->storageList->currentItem())
        return;
    QString storageName = ui->storageList->currentItem()->text(0);
    emit requestForDefaultStorage(storageName);
}


void ReportProperties::renameStorage()
{
    if (!ui->storageList->currentItem())
        return;
    QString storageName = ui->storageList->currentItem()->text(0);
    emit requestForNewStorageName(storageName);
}


void ReportProperties::clearDefaultStorage()
{
    emit requestForDefaultStorage(QString());
}


void ReportProperties::storagesListIndexChanged(QTreeWidgetItem* current, QTreeWidgetItem* previous)
{
    Q_UNUSED(previous)
    if (!current) {
        delete m_currentStorageHelper;
        return;
    }

    QString storageName = current->text(0);
    CuteReport::StorageInterface * storage = 0;
    foreach (CuteReport::StorageInterface * st, m_report->storages()) {
        if (st->objectName() == storageName) {
            storage = st;
            break;
        }
    }

    if (!storage)
        return;

    if (m_currentStorageHelper)
        m_currentStorageHelper->save();
    delete m_currentStorageHelper;

    m_currentStorageHelper = storage->helper();
    ui->storageHelperLayout->addWidget(m_currentStorageHelper);
    ui->storageType->setText(storage->moduleFullName());
    updateLayout();
}


void ReportProperties::rendererListIndexChanged(QTreeWidgetItem *current, QTreeWidgetItem *previous)
{
    Q_UNUSED(previous)
    if (!current) {
        delete m_currentRendererHelper;
        return;
    }

    CuteReport::RendererInterface * object = m_report->renderer(current->text(0));
    if (!object)
        return;

    if (m_currentRendererHelper) {
        CuteReport::RendererHelperInterface * rHelper = qobject_cast<CuteReport::RendererHelperInterface *>(m_currentRendererHelper);
        if (rHelper)
            rHelper->save();
    }
    delete m_currentRendererHelper;

    m_currentRendererHelper = object->helper();
    if (!m_currentRendererHelper) {
        PropertyEditor::EditorWidget * editor = m_core->createPropertyEditor(this);
        editor->setObject(object);
        m_currentRendererHelper = editor;
    }
    ui->rendererHelperLayout->addWidget(m_currentRendererHelper);
    ui->rendererType->setText(object->moduleFullName());
    updateLayout();
}


void ReportProperties::printerListIndexChanged(QTreeWidgetItem *current, QTreeWidgetItem *previous)
{
    Q_UNUSED(previous)
    if (!current) {
        delete m_currentPrinterHelper;
        return;
    }
    CuteReport::PrinterInterface * object = m_report->printer(current->text(0));
    if (!object)
        return;

    if (m_currentPrinterHelper) {
        CuteReport::PrinterHelperInterface * pHelper = qobject_cast<CuteReport::PrinterHelperInterface *>(m_currentPrinterHelper);
        if (pHelper)
            pHelper->save();
    }
    delete m_currentPrinterHelper;

    m_currentPrinterHelper = object->helper();
    if (!m_currentPrinterHelper) {
        PropertyEditor::EditorWidget * editor = m_core->createPropertyEditor(this);
        editor->setObject(object);
        m_currentPrinterHelper = editor;
    }
    ui->printerHelperLayout->addWidget(m_currentPrinterHelper);
    ui->printerType->setText(object->moduleFullName());
    updateLayout();
}


void ReportProperties::updateLayout()
{
//    if (m_currentStorageHelper) {
//        ui->storageHSpacer->changeSize(0,0, QSizePolicy::Maximum, QSizePolicy::Maximum);
//    } else {
//        ui->storageHSpacer->changeSize(0,0, QSizePolicy::MinimumExpanding, QSizePolicy::Maximum);
//    }
}


void ReportProperties::saveAll()
{
    if (!m_report)
        return;

    if (m_report->name() != ui->reportName->text())
        m_report->setName(ui->reportName->text());
    if (m_report->author() != ui->author->text())
        m_report->setAuthor(ui->author->text());
    if (m_report->description() != ui->description->toPlainText())
        m_report->setDescription(ui->description->toPlainText());

    if (m_currentStorageHelper)
        m_currentStorageHelper->save();
}

}
