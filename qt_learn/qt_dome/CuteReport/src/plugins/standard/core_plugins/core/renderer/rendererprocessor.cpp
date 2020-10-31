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
 **                  GNU Lesser General Public License                    **
 *                                                                         *
 *   This library is free software: you can redistribute it and/or modify  *
 *   it under the terms of the GNU Lesser General Public License as        *
 *   published by the Free Software Foundation, either version 3 of the    *
 *   License, or (at your option) any later version.                       *
 *   You should have received a copy of the GNU Lesser General Public      *
 *   License along with this library.                                      *
 *   If not, see <http://www.gnu.org/licenses/>.                           *
 *                                                                         *
 *   This library is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 ***************************************************************************/

#include "rendererprocessor.h"
#include "rendererdata.h"
#include "renderer.h"
#include "datasetinterface.h"
#include "forminterface.h"
#include "iteminterface.h"
#include "renderediteminterface.h"
#include "bandinterface.h"
#include "scriptextensioninterface.h"
#include "reportcore.h"
#include "renderedreport.h"
#include "limits"
#include "aggregatefunctions.h"
#include "scriptengine.h"
#include "preparser.h"

using namespace CuteReport;

USING_SUIT_NAMESPACE

SUIT_BEGIN_NAMESPACE

static bool cmpBands(BandInterface * a, BandInterface * b)
{
    if (a->layoutPriority() == b->layoutPriority())
        return a->order() < b->order();
    else
        return a->layoutPriority() < b->layoutPriority();
}


bool cmpBandsDesc(BandInterface * a, BandInterface * b)
{
    if (a->layoutPriority() == b->layoutPriority())
        return a->order() < b->order();
    else
        return a->layoutPriority() > b->layoutPriority();
}


bool cmpItems(BaseItemInterface * a, BaseItemInterface * b)
{
    return a->order() < b->order();
}


bool cmpPages(PageInterface * a, PageInterface * b)
{
    return a->order() < b->order();
}

Thread::Thread()
{
}

Thread::~Thread() {
    ReportCore::log(LogDebug, "Renderer::Thread", "Rendering Thread successfuly destroyed");
}


RendererProcessor::RendererProcessor(Renderer * renderer, CuteReport::ReportInterface * report)
    :QObject(renderer),
      m_rendererItemInterface(0),
      m_terminate(false),
      m_currentRenderedPage(0),
      m_runs(false),
      m_processEvents(false),
      m_passNumber(0),
      m_passesNeeded(1),
      m_pages(0),
      m_lastItemId(0)
{
    m_antialiasing = renderer->antialiasing();
    m_textAntialiasing = renderer->textAntialiasing();
    m_smoothPixmapTransform = renderer->smoothPixmapTransform();
    m_dpi = renderer->dpi() ? renderer->dpi() : 300;
    m_delay = renderer->delay();

    m_data = new RendererData();
    m_data->origReport = report;
    m_data->renderer = renderer;
    m_data->processor = this;
    m_data->dpi = renderer->dpi();

    QByteArray reportData = m_data->renderer->reportCore()->serialize(m_data->origReport);
    m_data->workingReportCopy = dynamic_cast<CuteReport::ReportInterface*>(m_data->renderer->reportCore()->deserialize(reportData));

    Q_ASSERT(m_data->workingReportCopy);
    if (m_data->workingReportCopy) {
        m_data->workingReportCopy->setParent(this);
        m_data->workingReportCopy->init();
    }

    m_aggregateFunctions = new SUIT_NAMESPACE::AggregateFunctions();
    m_preparser = new SUIT_NAMESPACE::PreParser();
}


RendererProcessor::~RendererProcessor()
{
    emit log(CuteReport::LogDebug, "RendererProcessor DTOR", "");
    if (m_currentRenderedPage)
        delete m_currentRenderedPage;
    delete m_aggregateFunctions;
    delete m_preparser;
    delete m_data;
}


void RendererProcessor::start()
{
    emit log(CuteReport::LogDebug, QString("start thread id: %1  processor thread id: %2").arg((long)QThread::currentThread()), "");

    m_runs = false;
    m_processEvents = qApp->thread() == this->thread();

    emit started();

    initScriptEngine();

    bool resultOk = evaluateScript();

    if (resultOk && !m_data->workingReportCopy->script().contains("engine.run")) {
        m_data->scriptEngine->evaluate("engine.run();");
        if (m_data->scriptEngine->hasUncaughtException()) {
            QString _message = QString("error evaluating engine.run %1")
                               .arg(m_data->scriptEngine->uncaughtException().toString());
            qWarning() << _message;
            m_data->appendError(_message);
            resultOk = false;
        }
    }

    if (!resultOk && !m_runs) { // sometimes script contains error after renderer started.
        _done(false);
    }   
}


void RendererProcessor::terminate()
{
    QMutexLocker locker(&mutex);
    m_terminate = true;
}


void RendererProcessor::_done(bool success)
{
    emit log(CuteReport::LogDebug, QString("_done"), "");
    m_data->dpi = m_dpi;

    RenderedReport * renReport = new RenderedReport();
    renReport->setRenderedPages(m_data->pages);
    renReport->setDpi(m_data->dpi);

    m_data->pages.clear();
    resetScriptEngine();

    emit done(success, renReport);
}


void RendererProcessor::scriptEngineException(QScriptValue value)
{
    emit log(CuteReport::LogError, "Script Exception", value.toString());
}


void RendererProcessor::processEventsIfNeeded()
{
    if (m_processEvents)
        QCoreApplication::processEvents();
}


void RendererProcessor::run()
{
    m_runs = true;

    emit log(CuteReport::LogDebug, QString("run thread id %1").arg((long)QThread::currentThread()), "");

    if (!m_data->workingReportCopy->pages().size())
        emit log(LogError, "report has no pages",  QString("report with name %1 has no pages").arg(m_data->workingReportCopy->objectName()));

    QList<PageInterface *> pages = m_data->workingReportCopy->pages();
    qSort(pages.begin(), pages.end(), cmpPages);

    while (m_passNumber < m_passesNeeded) {
        m_fakePass = m_passNumber < (m_passesNeeded-1);
        m_currentPageNumber = 0;
        m_data->reset();

        setValue("TPAGES", pages.count(), QScriptValue::ReadOnly);
        setValue("PAGES", m_pages, QScriptValue::ReadOnly);

        for (int i=0; i< pages.count() && !terminated(); ++i) {
            CuteReport::PageInterface * page = pages[i];
            setValue("TPAGE", i+1, QScriptValue::ReadOnly);
            emit log(LogDebug, QString("rendering page: %1").arg(page->objectName()), "");
            renderReportPage(page);
        }

        if (!m_fakePass)
            emit m_rendererItemInterface->reportDone();

        resetData();

        m_pages = m_currentPageNumber;
        m_passNumber++;
    }

    if (terminated()) {
        emit log(LogInfo, QString("terminated"), "");
        _done(true);
    } else
        _done(true);


    //    if (m_passNumber < m_passesNeeded) {
    //        emit log(CuteReport::LogDebug, QString("_done"), "");
    //        m_passNumber++;
    //        m_pages = m_currentPageNumber;
    //        setValue("PAGES", m_pages, QScriptValue::ReadOnly);
    //        _run();
    //    } else {
}


void RendererProcessor::initScriptEngine()
{
    Q_ASSERT(m_data);
    m_data->scriptEngine = new ScriptEngine(this);
    ScriptEngine * scriptEngine = m_data->scriptEngine;
    scriptEngine->setProcessEventsInterval(100);
    connect(scriptEngine, SIGNAL(signalHandlerException(QScriptValue)), this, SLOT(scriptEngineException(QScriptValue)));
    scriptEngine->pushContext();

    ReportInterface * report = m_data->workingReportCopy;

    m_rendererItemInterface = new RendererItemInterface(this);
    scriptEngine->globalObject().setProperty("engine", scriptEngine->newQObject(m_rendererItemInterface), QScriptValue::Undeletable);
    scriptEngine->globalObject().setProperty("print", scriptEngine->newFunction( &RendererProcessor::scriptPrint ) );

    /// registering script extensions
    foreach (ReportPluginInterface * module, m_data->renderer->reportCore()->modules(CuteReport::ScriptExtensionModule) ) {
        ScriptExtensionInterface * extension = static_cast<ScriptExtensionInterface *>(module);
        foreach (const QString & key, extension->keys())
            extension->initialize(key, scriptEngine);
    }

    /// registering additional metatypes
    qScriptRegisterQObjectMetaType<DatasetInterface*>(scriptEngine);

    /// adding datasets
    foreach(CuteReport::DatasetInterface * dtst, report->datasets()) {
        emit log(CuteReport::LogDebug, QString("Preparing dataset \'%1\'").arg(dtst->objectName()), "");
        scriptEngine->globalObject().setProperty(dtst->objectName(), scriptEngine->newQObject(dtst), QScriptValue::ReadOnly);
    }

    /// adding forms
    foreach(CuteReport::FormInterface * form, report->forms()){
        form->renderInit();
        scriptEngine->globalObject().setProperty(form->objectName(), scriptEngine->newQObject(form), QScriptValue::ReadOnly);
    }

    /// adding items
    QSet<QString> initedItems;
    foreach(CuteReport::PageInterface * page, report->pages()) {
        foreach(CuteReport::BaseItemInterface * item, page->items()) {
            scriptEngine->globalObject().setProperty(item->objectName(), scriptEngine->newQObject(item), QScriptValue::ReadOnly);
            if (!initedItems.contains(item->moduleFullName())) {
                item->initScript(scriptEngine);
                initedItems.insert(item->moduleFullName());
            }
        }
    }

    setValue("PAGE", 0, QScriptValue::ReadOnly);
    setValue("PAGES", 0, QScriptValue::ReadOnly);
    setValue("LINE", 0, QScriptValue::ReadOnly);
    setValue("PASS", 0, QScriptValue::ReadOnly);
    setValue("PASSES", 0, QScriptValue::ReadOnly);
    setValue("TPAGE", 0, QScriptValue::ReadOnly);
    setValue("TPAGES", 0, QScriptValue::ReadOnly);

    m_preparser->registerScriptObjects(scriptEngine);
    m_aggregateFunctions->registerScriptObjects(scriptEngine);
}


void RendererProcessor::resetScriptEngine()
{
    ReportInterface * report = m_data->workingReportCopy;
    foreach(CuteReport::FormInterface * form, report->forms()){
        form->renderReset();
    }
}


bool RendererProcessor::evaluateScript()
{
    QString script = setVariablesValue( m_data->workingReportCopy->script(), m_data->workingReportCopy->variables() );
    if (!preprocessScript(script))
        return false;

    qDebug() << script;
    m_data->scriptEngine->evaluate( script );

    if (m_data->scriptEngine->hasUncaughtException()) {
        QString _message = QString("script error at line %1 \n%2")
                           .arg(m_data->scriptEngine->uncaughtExceptionLineNumber())
                           .arg(m_data->scriptEngine->uncaughtException().toString());
        m_data->appendError(_message);
        emit log(LogError, QString("Report's script evaluation error"), QString("error message: %1").arg(_message));
        return false;
    }
    return true;
}


void RendererProcessor::resetData()
{
    foreach (DatasetInterface * ds, m_data->workingReportCopy->datasets())
        ds->resetCursor();

    foreach (PageInterface * page, m_data->workingReportCopy->pages())
        foreach (BaseItemInterface * item, page->items())
            item->renderReset();

    m_aggregateFunctions->reset();
    m_bandsDone.clear();
    m_datasetRegister.clear();
    m_currentDatasetLine = 0;
    m_data->scriptEngine->popContext();
    m_data->scriptEngine->pushContext();
}


void RendererProcessor::renderReportPage(CuteReport::PageInterface * page)
{
    emit log(CuteReport::LogDebug, QString("renderReportPage: %1").arg(page->objectName()), "");
    emit beforeTemplatePageRendering(page);

    if (m_dpi > 0)
        page->setDpi(m_dpi);

    topBands.clear();
    bottomBands.clear();
    freeBands.clear();

    page->renderInit(m_rendererItemInterface);

    m_currentPage = page;
    m_currentDatasetLine = 0;
    m_currentPageNumber = 0;
    m_processingBand = 0;
    m_lastProcessedBand = 0;

    setValue("PAGE", m_currentPageNumber, QScriptValue::ReadOnly);
    setValue("LINE", m_currentDatasetLine+0, QScriptValue::ReadOnly);

    QList <BaseItemInterface*> items = page->items();
    for (int i=0; i<items.count() && !terminated(); ++i) {
        CuteReport::BaseItemInterface * item = items.at(i);
        CuteReport::BandInterface* band = qobject_cast<CuteReport::BandInterface*>(item);
        if (!band)
            continue;

        if (band->layoutType() == CuteReport::BandInterface::LayoutTop)
            topBands.append(band);
        if (band->layoutType() == CuteReport::BandInterface::LayoutBottom)
            bottomBands.append(band);
        if (band->layoutType() == CuteReport::BandInterface::LayoutFree)
            freeBands.append(band);
    }

    if (terminated())
        return;

    qSort(topBands.begin(), topBands.end(), cmpBandsDesc);
    qSort(bottomBands.begin(), bottomBands.end(), cmpBandsDesc);

    initBands(topBands);
    initBands(bottomBands);
    initBands(freeBands);

    createNewRenderingPage();

    for (int i=0; i<topBands.count() && !terminated(); ++i) {
        BandInterface * band = topBands.at(i);
        emit log(CuteReport::LogDebug, QString("checking band: %1").arg(band->objectName()), "");
        if (terminated())
            return;

        if (m_bandsDone.contains(band)) //already processed in group iterations
            continue;

        DatasetInterface * dataset = datasetRegisteredTo(band);
        if (dataset)
            processDataset(dataset);
        else
            processBand(band, CuteReport::RenderingNormal);
    }

    m_state = RendererPublicInterface::ContentDone;

    if (m_currentRenderedPage) {
        completePage(m_currentRenderedPage);
    }

    page->renderReset();

    emit afterTemplatePageRendering(page);
}


void RendererProcessor::initBands(QList<CuteReport::BandInterface*> bands)
{
    foreach (CuteReport::BandInterface * band, bands) {
        band->renderInit(m_rendererItemInterface);
        foreach (CuteReport::BaseItemInterface * const child, band->findChildren<CuteReport::BaseItemInterface *>())
            child->renderInit(m_rendererItemInterface);
    }
}


void RendererProcessor::createNewRenderingPage()
{
    bool allowNewPage  =  true;
    emit m_rendererItemInterface->requestNewPage(&allowNewPage);
    if (!allowNewPage)
        return;

    if (m_currentRenderedPage) {
        completePage(m_currentRenderedPage);
    }

    if (terminated())
        return;

    m_state = RendererPublicInterface::EmptyPage;

    ++m_currentPageNumber;

    setValue("PAGE", m_currentPageNumber, QScriptValue::ReadOnly);

    m_currentRenderedPage = m_currentPage->render();

    m_freeSpace = m_currentPage->pageRect();
    if (m_currentPage->columns() > 1)
        m_freeSpace.setWidth(m_currentPage->pageRect().width() / m_currentPage->columns());

    emit processingPage(m_currentPageNumber, 0);
    emit m_rendererItemInterface->pageBefore(m_currentRenderedPage);

    foreach(BandInterface * band, freeBands)   //process listFree first if it want paint on background
        if (band != m_processingBand)
            processBand(band, RenderingNewPage);

    m_state = RendererPublicInterface::BackgroundDone;

    foreach(BandInterface * band, topBands)
        if (band != m_processingBand)
            processBand(band, RenderingNewPage);

    m_state = RendererPublicInterface::HeadersDone;

    for (int i = bottomBands.count()-1; i>=0 ;--i)
        if (bottomBands.at(i) != m_processingBand)
            processBand(bottomBands.at(i), RenderingNewPage);

    m_state = RendererPublicInterface::FootersDone;

    emit m_rendererItemInterface->pagePrepared(m_currentRenderedPage);
}


void RendererProcessor::completePage(RenderedPageInterface* page)
{
    emit m_rendererItemInterface->pageAfter(m_currentRenderedPage);

    foreach(BandInterface * band, freeBands) {   //process listFree first if it want paint on foreground
        if (band != m_processingBand)
            processBand(band, RenderingNormal);
    }

    m_currentRenderedPage = 0;
    m_data->appendPage(page);

    if (m_delay > 0) {
        QWaitCondition waitCondition;
        QMutex mutex;
        waitCondition.wait(&mutex,m_delay);
    }
}


void RendererProcessor::processBand(CuteReport::BandInterface * band, RenderingStage stage)
{
    Q_ASSERT(band);

    bool needToBeRendered = false;
    switch (stage) {
        case RenderingNewPage:  needToBeRendered = band->renderNewPage(); break;
        case RenderingNormal:   needToBeRendered = band->renderPrepare(); break;
    }

    if (!needToBeRendered) {
        band->renderEnd();
        return;
    }

    CuteReport::BandInterface * saveBand = m_processingBand;
    m_processingBand = band;

    emit log(CuteReport::LogDebug, QString("process band: %1").arg(band->objectName()), "");

    emit m_rendererItemInterface->bandBefore(band);

    QRectF geometry = band->absoluteGeometry(Millimeter);
    if (band->layoutType()== BandInterface::LayoutTop)
        geometry.moveTo(m_freeSpace.topLeft());
    else if (band->layoutType()== BandInterface::LayoutBottom)
        geometry.moveBottomLeft(m_freeSpace.bottomLeft());
    band->setAbsoluteGeometry(geometry, Millimeter);

    QList<BaseItemInterface*> children;
    foreach (BaseItemInterface* child, band->findChildren<CuteReport::BaseItemInterface*>())
        if (child->parent() == band)
            children.append(child);
    qSort(children.begin(), children.end(), cmpItems);
    QList<BaseItemInterface*> processedItems ;

    foreach (BaseItemInterface * const item, children)
        item->beforeSiblingsRendering(children);

    foreach (BaseItemInterface * const item, children)
        processItem(item, processedItems, true);

    if (band->stretchable()) {
        foreach (BaseItemInterface * child, children) {
            if (!processedItems.contains(child))
                continue;
            QRectF childRect = child->absoluteBoundingRect(Millimeter);
            if ( childRect.bottom() > geometry.bottom())
                geometry.setBottom(childRect.bottom());
        }
        band->setAbsoluteGeometry(geometry);
    }

    foreach (CuteReport::BaseItemInterface * const item, children)
        item->afterSiblingsRendering(children);

    if (band->layoutType() != BandInterface::LayoutFree && !canFitBandToPage(band) ) {
        createNewRenderingPage();
        geometry = band->absoluteGeometry(Millimeter);
        if (band->layoutType()== BandInterface::LayoutTop)
            geometry.moveTo(m_freeSpace.topLeft());
        else if (band->layoutType()== BandInterface::LayoutBottom)
            geometry.moveBottomLeft(m_freeSpace.bottomLeft());
        band->setAbsoluteGeometry(geometry);
    }

    RenderedItemInterface * renderedBand = band->renderView();
    Q_ASSERT_X(renderedBand, "Item rendering object is NULL", QString("object name: \'%1\', needrendering: %2").arg(band->objectName()).arg(needToBeRendered).toLatin1());

    renderedBand->setId(++m_lastItemId);
    m_lastIdForItem.insert(band->objectName(), m_lastItemId);
    if (band->layoutType() == BandInterface::LayoutFree)
        renderedBand->setZValue(band->order()>=0 ? (100 + band->order()) : (-100 - band->order()) ); // 100 reserverd for top and bottom
    renderedBand->setParentItem(m_currentRenderedPage);
    renderedBand->setAbsoluteGeometry(geometry, Millimeter);
    renderedBand->redraw();

    foreach (CuteReport::BaseItemInterface * const item, children) {
        if (processedItems.contains(item)) {
            deployItem(item, renderedBand, processedItems, true);
        }
    }

    foreach (CuteReport::BaseItemInterface * const item, children) {
        if (processedItems.contains(item)) {
            renderingEndItem(item, processedItems, true);
        }
    }

    if (band->layoutType() == BandInterface::LayoutTop) {
        m_freeSpace.setTop(qMax(m_freeSpace.top(), geometry.bottom()));  // band can be placed in its own logic to any position
    } else if (band->layoutType() == BandInterface::LayoutBottom)
        m_freeSpace.setBottom(qMin(m_freeSpace.bottom(), geometry.top()));

    band->renderEnd();

    m_state = RendererPublicInterface::DrawingContent;
    emit m_rendererItemInterface->bandAfter(band);
    m_lastProcessedBand = band;
    m_processingBand = saveBand;
}


bool RendererProcessor::processItem(CuteReport::BaseItemInterface * item, QList<CuteReport::BaseItemInterface*> & processedList, bool withChildren)
{
    Q_ASSERT(item);
    emit log(CuteReport::LogDebug, QString("processing item: %1").arg(item->objectName()), "");
    processEventsIfNeeded();

    emit m_rendererItemInterface->itemBefore(item);

    bool needProcessing = item->renderPrepare();

    if (!needProcessing) {
        item->renderEnd();
        return false;
    }

    processedList.append(item);

    emit m_rendererItemInterface->itemAfter(item);

    if (withChildren) {
        QList<CuteReport::BaseItemInterface *> list;
        foreach (CuteReport::BaseItemInterface * child, item->findChildren<CuteReport::BaseItemInterface *>())
            if (child->parent() == item)
                list.append(child);
        if (list.count()) {
            qSort(list.begin(), list.end(), cmpItems);
            foreach (CuteReport::BaseItemInterface * const child, list)
                child->beforeSiblingsRendering(list);
            foreach (CuteReport::BaseItemInterface * const child, list)
                processItem(child, processedList, true);
            foreach (CuteReport::BaseItemInterface * const child, list)
                child->afterSiblingsRendering(list);
        }
    }

    return needProcessing;
}


void RendererProcessor::deployItem(BaseItemInterface *item, RenderedItemInterface *parent, QList<BaseItemInterface*> & processedItems, bool withChildren)
{
    emit log(CuteReport::LogDebug, QString("deploy item: %1").arg(item->objectName()), "");
    emit m_rendererItemInterface->itemBefore(item);

    QRectF geometry = item->absoluteGeometry(Millimeter);

    RenderedItemInterface * renderedItem = item->renderView();
    if (!renderedItem)
        return;

    renderedItem->setId(++m_lastItemId);
    m_lastIdForItem.insert(item->objectName(), m_lastItemId);
    renderedItem->setParentItem(parent ? (QGraphicsItem *)parent : (QGraphicsItem *)m_currentRenderedPage);
    renderedItem->setAbsoluteGeometry(geometry, Millimeter);
    renderedItem->redraw();

    emit m_rendererItemInterface->itemAfter(item);

    if (withChildren) {
        QList<CuteReport::BaseItemInterface *> list;
        foreach (CuteReport::BaseItemInterface * child, item->findChildren<CuteReport::BaseItemInterface *>())
            if (child->parent() == item)
                list.append(child);
        qSort(list.begin(), list.end(), cmpItems);
        foreach (CuteReport::BaseItemInterface * const child, list) {
            if (processedItems.contains(child))
                deployItem(child, renderedItem, processedItems, true);
        }
    }
}


void RendererProcessor::renderingEndItem(BaseItemInterface *item, QList<BaseItemInterface *> &processedItems, bool withChildren)
{
    emit log(CuteReport::LogDebug, QString("renderingEndItem item: %1").arg(item->objectName()), "");

    if (withChildren) {
        QList<CuteReport::BaseItemInterface *> list;
        foreach (CuteReport::BaseItemInterface * child, item->findChildren<CuteReport::BaseItemInterface *>())
            if (child->parent() == item)
                list.append(child);
        qSort(list.begin(), list.end(), cmpItems);
        foreach (CuteReport::BaseItemInterface * const child, list) {
            if (processedItems.contains(child))
                renderingEndItem(child, processedItems, withChildren);
        }
    }

    item->renderEnd();
}


void RendererProcessor::processDataset(DatasetInterface * dtst)
{
    emit log(LogDebug, QString("rendering dataset: %1").arg(dtst->objectName()), "");

    emit m_rendererItemInterface->datasetBefore(dtst);

    /// store dynamic data
    int currentDatasetLine = m_currentDatasetLine;
    DatasetInterface * currentDataset = m_currentDataset;

    if (!dtst->isPopulated()) {
        if (!dtst->populate()) {
            emit log(CuteReport::LogError, QString("dataset \"%1\" error").arg(dtst->objectName()),
                     QString("%1: %2").arg(dtst->objectName()).arg(dtst->getLastError()));
            terminate();
            return;
        }
    } else
        dtst->setCurrentRowNumber(0);

    m_currentDataset = dtst;
    m_currentDatasetLine = 1;

    setValue("LINE", 0 , QScriptValue::ReadOnly);

    BandsList currentGroup = bandRegisteredToDataset(dtst->objectName());
    qSort(currentGroup.begin(), currentGroup.end(), cmpBands);

    do {
        if (terminated())
            return;

        setValue("LINE",  getValue("LINE").toInt() + 1, QScriptValue::ReadOnly);

        m_aggregateFunctions->processDatasetIteration(m_currentDataset);

        foreach(BandInterface * band, currentGroup)
            processBand(band, RenderingNormal);

        m_currentDatasetLine++;
    } while (dtst->setNextRow());

    foreach (BandInterface * band, currentGroup)
        if (!m_bandsDone.contains(band))
            m_bandsDone.append(band);

    /// restore dynamic data
    m_currentDatasetLine = currentDatasetLine;
    m_currentDataset = currentDataset;

    emit m_rendererItemInterface->datasetAfter(dtst);
}


bool RendererProcessor::canFitBandToPage(BandInterface * band)
{
    return (m_freeSpace.top() + band->geometry().height() <= m_freeSpace.bottom());
}


bool RendererProcessor::terminated()
{
    QMutexLocker locker(&mutex);
    return m_terminate;
}


void RendererProcessor::setValue(const QString & valueName, const QVariant &value, QScriptValue::PropertyFlags flag)
{
    m_data->scriptEngine->globalObject().setProperty(valueName, m_data->scriptEngine->newVariant(value), flag);
}


QVariant RendererProcessor::getValue(const QString & valueName)
{
    return m_data->scriptEngine->globalObject().property(valueName).toVariant();
}


CuteReport::DatasetInterface * RendererProcessor::datasetRegisteredTo(BandInterface * band)
{
    CuteReport::DatasetInterface * resultDataset = 0;
    foreach (QString dataset, m_datasetRegister.keys()) {
        BandsList list = m_datasetRegister.values(dataset);
        if (list.contains(band))
            resultDataset = m_data->renderer->reportCore()->datasetByName(dataset, m_data->workingReportCopy);
    }
    return resultDataset;
}


void RendererProcessor::registerBandToDatasetIteration(const QString & datasetName, const QString & objectName)
{
    BaseItemInterface * item = m_data->renderer->reportCore()->itemByName(objectName, m_currentPage);
    BandInterface * band = qobject_cast<BandInterface*>(item);
    if (band)
        registerBandToDatasetIteration(datasetName, band);
}


void RendererProcessor::registerBandToDatasetIteration(const QString & datasetName, CuteReport::BandInterface * band)
{
    if (band && !m_datasetRegister.values(datasetName).contains(band) )
        m_datasetRegister.insertMulti(datasetName, band);
}


void RendererProcessor::unregisterBandFromDatasetIteration(const QString & datasetName, const QString & objectName)
{
    BaseItemInterface * item = m_data->renderer->reportCore()->itemByName(objectName, m_currentPage);
    BandInterface * band = qobject_cast<BandInterface*>(item);

    if (band)
        unregisterBandFromDatasetIteration(datasetName, band);
}


void RendererProcessor::unregisterBandFromDatasetIteration(const QString & datasetName, CuteReport::BandInterface * band)
{
    if (band && !m_datasetRegister.values(datasetName).contains(band) ) {
        QMutableHashIterator<QString, BandInterface*> i(m_datasetRegister);
        while (i.hasNext()) {
            i.next();
            if  (i.value() == band)
                i.remove();
        }
    }
}


BandsList RendererProcessor::bandRegisteredToDataset(const QString & datasetName)
{
    return m_datasetRegister.values(datasetName);
}


const CuteReport::BandInterface * RendererProcessor::getBandForItem(const CuteReport::BaseItemInterface * item)
{
    if (!item)
        return 0;

    if (qobject_cast<CuteReport::BandInterface*> (item))
        return static_cast<const CuteReport::BandInterface*> (item);
    else
        return getBandForItem(item->parentItem());
}


QString RendererProcessor::processString(const QString & string, const QString & delimiterBegin, const QString & delimiterEnd, const CuteReport::BaseItemInterface * item)
{
    // escaping all delimiter characters for prevent problems
    QString regexp;
    for (int i=0; i<delimiterBegin.length(); ++i)
        regexp+=QString("\\") + delimiterBegin.at(i);
    regexp+="(.*)";
    for (int i=0; i<delimiterEnd.length(); ++i)
        regexp+= QString("\\") + delimiterEnd.at(i);


    QRegExp rx(regexp);
    rx.setMinimal(true);
    QString str( setVariablesValue(string, m_data->workingReportCopy->variables()) );

    int pos = 0;
    while ((pos = rx.indexIn(str, pos)) != -1) {
        int length = rx.matchedLength();

        QString expression = rx.cap(1);
        QString evaluationResult = _processString(expression, item);

        str.replace(pos, length, evaluationResult);
        pos += evaluationResult.length();
    }

    return str;
}


QString RendererProcessor::processString(const QString & string, const BaseItemInterface *item)
{
    return _processString( setVariablesValue(string, m_data->workingReportCopy->variables()), item);
}


QString RendererProcessor::_processString(const QString & string, const CuteReport::BaseItemInterface * item)
{
    QString expression = preprocessEvaluateString(string, item);
    QString evaluationResult;

    if (m_data->scriptEngine->canEvaluate(expression)) {
        QScriptValue evaluateValue = m_data->scriptEngine->evaluate(expression).toString();
        if (m_data->scriptEngine->hasUncaughtException()) {
            QString _message = QString("script error at line %1 \n%2")
                               .arg(m_data->scriptEngine->uncaughtExceptionLineNumber())
                               .arg(m_data->scriptEngine->uncaughtException().toString());
            emit log(LogError, QString("script evaluation error for item \'%1\'").arg(item->objectName()),
                     QString("script evaluation error for item \'%1\': %2").arg(item->objectName()).arg(_message));
            evaluationResult = "\'error\'";
        } else {
            evaluationResult = evaluateValue.toString();
        }
    } else
        evaluationResult = "\'can't evaluate\'";

    return evaluationResult;
}


RendererItemInterface *RendererProcessor::rendererItemInterface() const
{
    return m_rendererItemInterface;
}


void RendererProcessor::sendLog(LogLevel level, const QString &shortMessage, const QString &fullMessage)
{
    emit log(level, shortMessage, fullMessage);
}


AggregateFunctions *RendererProcessor::aggregateFunctions() const
{
    return m_aggregateFunctions;
}


void RendererProcessor::registerEvaluationString(const QString & string, const QString & delimiterBegin, const QString & delimiterEnd, CuteReport::BaseItemInterface *item)
{
    // escaping all delimiter characters for prevent problems
    QString regexp;
    for (int i=0; i<delimiterBegin.length(); ++i)
        regexp+=QString("\\") + delimiterBegin.at(i);
    regexp+="(.*)";
    for (int i=0; i<delimiterEnd.length(); ++i)
        regexp+= QString("\\") + delimiterEnd.at(i);


    QRegExp rx(regexp);
    rx.setMinimal(true);

    int pos = 0;
    while ((pos = rx.indexIn(string, pos)) != -1) {
        int length = rx.matchedLength();

        QString expression = rx.cap(1);
        registerEvaluationString(expression, item);

        pos += length;
    }
}


void RendererProcessor::registerEvaluationString(const QString & string, BaseItemInterface *item)
{
    if (string.isEmpty())
        return;

    QString str(string);
    /// replacing dataset value strings like data."field" to correct syntax data.value(field)

    QRegExp rx("(\\w+)\\.\\\"(.*)\\\"");
    rx.setMinimal(true);

    int pos = 0;
    while ((pos = rx.indexIn(str, pos)) != -1) {
        int length = rx.matchedLength();

        QString result = QString("%1.getValue(\"%2\")").arg(rx.cap(1)).arg(rx.cap(2));

        str.replace(pos, length, result);
        pos += result.length();
    }

    const BandInterface * band = getBandForItem(item);
    QStringList errors;
    QStringList moduleErrors;

    bool result = true;
    result &= m_preparser->initialItemScriptPreprocess(str, item, &moduleErrors);
    errors << moduleErrors;
    result &= m_aggregateFunctions->initialItemScriptPreprocess(str, band ? band->objectName() : QString(), m_data->workingReportCopy, &moduleErrors);
    errors << moduleErrors;

    foreach (const QString & error, errors) {
        emit log(LogError, QString("Script preprocess error for report:\'%1\' item:\'%2\'")
                 .arg(m_data->workingReportCopy->objectName())
                 .arg(item ? item->objectName() : "Undefined"), error);
    }
}


QString RendererProcessor::preprocessEvaluateString(const QString &str, const BaseItemInterface *item)
{
    QString resultStr = str;

    const BandInterface * band = getBandForItem(item);
    m_preparser->itemScriptPreprocess(resultStr, item, 0);
    m_aggregateFunctions->itemScriptPreprocess(resultStr, band ? band->objectName() : QString(), m_data->workingReportCopy, 0);

    return resultStr;
}


bool RendererProcessor::preprocessScript(QString &str)
{
    QStringList errors;
    QStringList moduleErrors;
    bool result = true;
    result &= m_preparser->mainScriptPreprocess(str, &moduleErrors);
    errors << moduleErrors;
    result &= m_aggregateFunctions->mainScriptPreprocess(str, m_data->workingReportCopy, &moduleErrors);
    errors << moduleErrors;

    foreach (const QString & error, errors) {
        emit log(LogError, QString("Script preprocess error for report \'%1\'").arg(m_data->workingReportCopy->objectName()), error);
    }

    return result;
}


QScriptValue RendererProcessor::scriptPrint( QScriptContext *context, QScriptEngine *engine)
{
    ScriptEngine * eng = reinterpret_cast<ScriptEngine *>(engine);
    if (context->argumentCount() < 1)
        return QString();
    QString str(context->argument(0).toString());
    for (int i=1; i<context->argumentCount();++i)
        str = str.arg(context->argument(i).toString());
    eng->processor()->sendLog(CuteReport::LogInfo, "Print", str );
    return engine->undefinedValue();
}


SUIT_END_NAMESPACE
