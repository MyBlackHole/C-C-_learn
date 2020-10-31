/*
 * This file is part of the PathWalker software
 *
 * Copyright (C) 2014 Denis Kvita <dkvita@gmail.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, see <http://www.gnu.org/licenses/>.
 */

#include <QVBoxLayout>
#include <QSettings>
#include <QApplication>
#include <QtDebug>
#include <QtGui>
#include <QMessageBox>
#include <QLineEdit>
#include <QDesktopServices>
#include <QToolButton>
#include <QClipboard>

#ifdef Q_OS_WIN32
#include <QWinTaskbarButton>
#include <QWinTaskbarProgress>
#endif

#include "direntrylistview.h"
#include "columnheader.h"
#include "direntry.h"
#include "direntrysrc.h"
#include "busydialog.h"

#include "visualkeyboardnavigator/visualkeyboardnavigator.h"
#include "entitycompare.h"
#include "direntrynavibar.h"
#include "settingsstore/settingsstore.h"
#include "direntrymodelinfo.h"
#include "aboutpathwalker.h"
#include "historydialog.h"

DirEntryListView::DirEntryListView(QWidget *parent) :
    ISettings(parent),aContextMenu(0)
{
    setObjectName("DirEntryView");
    setContextMenuPolicy(Qt::CustomContextMenu);
    setAcceptDrops(true);

    if(QMetaType::type("QVariant")==QMetaType::UnknownType) qRegisterMetaType<QVariant>(new QVariant());
    if(QMetaType::type("PropertyPrecedeOrder")==QMetaType::UnknownType) qRegisterMetaType<PropertyPrecedeOrder>(new PropertyPrecedeOrder());
    if(QMetaType::type("PropertyPrecedeOrderList")==QMetaType::UnknownType) qRegisterMetaType<PropertyPrecedeOrderList>(new PropertyPrecedeOrderList());
    if(QMetaType::type("VarRecord")==QMetaType::UnknownType)qRegisterMetaType<VarRecord>(new VarRecord());
    if(QMetaType::type("EntityInstance")==QMetaType::UnknownType)qRegisterMetaType<EntityInstance>(new EntityInstance());
    if(QMetaType::type("EntityInstancePrivate")==QMetaType::UnknownType)qRegisterMetaType<EntityInstancePrivate>(new EntityInstancePrivate());
    if(QMetaType::type("EntityInstanceList")==QMetaType::UnknownType)qRegisterMetaType<EntityInstanceList>(new EntityInstanceList());
    if(QMetaType::type("QList<EntityInstance>")==QMetaType::UnknownType) qRegisterMetaType<QList<EntityInstance> >(new QList<EntityInstance>());
    if(QMetaType::type("EntityInfo")==QMetaType::UnknownType)qRegisterMetaType<EntityInfo>(new EntityInfo());
    if(QMetaType::type("EntityCompareConditions")==QMetaType::UnknownType)qRegisterMetaType<EntityCompareConditions>(new EntityCompareConditions());

    SettingsStore::registerWindow(this);

#ifdef Q_OS_WIN32
    aWinTaskBarButton= new QWinTaskbarButton(this);
#endif
    aHistory= new DirPathHistory();

    aDirEntry= new EntityListModel((EntityListModelInfo*)new DirEntryModelInfo(DirEntryInfo::info()));
    aDirEntry->sortOrderDefaultSet(PropertyPrecedeOrderList()<<qMakePair(DirEntryProperty::isSpecial, def::Ascending));
    aDirEntrySrc= new DirEntrySrc();//DirContentFetcher();
    aDirEntryView= new QTableView();
    aDirEntryView->verticalHeader()->hide();
    aDirEntryView->setShowGrid(false);
    aDirEntryView->setEditTriggers(QAbstractItemView::EditKeyPressed);
    ColumnHeader *h= new ColumnHeader();
    h->setSectionsMovable(true);
    aDirEntryView->setHorizontalHeader(h);
    aDirEntryView->verticalHeader()->setDefaultSectionSize(22);
    aDirEntryView->setSelectionBehavior(QAbstractItemView::SelectRows);
    aDirEntryView->setModel(aDirEntry);
    palette().color(QPalette::Highlight).name();
    aDirEntryView->setStyleSheet("QTableView::item:focus {"
                                 "background-color: "+palette().color(QPalette::Highlight).dark(150).name()+";"
                                                                                                            "}");

    aQuickFilter= new QuickFilter(this);
    aQuickFilter->hide();

    aRowCountInfo= new QLabel();

    aBusy= new BusyDialog(tr("applying..."),this);
    aBusy->hide();

    setFocusProxy(aDirEntryView);

    aNaviBar= new DirEntryNaviBar(this);

    aCopyMoveMenu= new QMenu(aDirEntryView);
    aCopyMoveMenu->addAction(tr("copy"), this, SLOT(onClipboardCopy()));
    aCopyMoveMenu->addAction(tr("move"), this, SLOT(onClipboardMove()));

    doInitLayout();
    doInitConnections();

    setTabOrder(aNaviBar, aDirEntryView);
    setTabOrder(aDirEntryView, aQuickFilter);
    setTabOrder(aQuickFilter, aDirEntryView);
    setTabOrder(aDirEntryView, aNaviBar);

    aDirEntryView->installEventFilter(this);

    VisualKeyboardNavigator::registerTag(Qt::Key_2, new VisualTag(tr("2"), window()), aDirEntryView);
    VisualKeyboardNavigator::registerTag(Qt::Key_3, new VisualTag(tr("3"), window()), aQuickFilter);
    VisualKeyboardNavigator::registerTag(Qt::Key_4, new VisualTag(tr("4"), window()), aBusy);

    aDirEntry->rowsFixedTopSet(EntityInstanceList()<< EntityInstance().set(DirEntryProperty::Name,"..").set(DirEntryProperty::isDir,true));
}

bool DirEntryListView::settingsSave()
{
    QSettings cfg(SettingsStore::cfgPath(),QSettings::IniFormat);
    cfg.setValue(objectName()+"/geometry",saveGeometry());
    ColumnHeader *h= qobject_cast<ColumnHeader*>(aDirEntryView->horizontalHeader());
    if(h!=0) cfg.setValue(objectName()+"/TblHdr",h->saveState());
    cfg.setValue(objectName()+"/lastDirPath",aNaviBar->path());
    cfg.setValue(objectName()+"/History",aHistory->paths());
    cfg.setValue(objectName()+"/HistoryIdx",aHistory->index());
    return true;
}

bool DirEntryListView::settingsRestore()
{
    QSettings cfg(SettingsStore::cfgPath(),QSettings::IniFormat);
    QByteArray b;
    b= cfg.value(objectName()+"/geometry").toByteArray();
    if(!b.isEmpty()) restoreGeometry(b);
    ColumnHeader *h= qobject_cast<ColumnHeader*>(aDirEntryView->horizontalHeader());
    if(h!=0){
        b= cfg.value(objectName()+"/TblHdr").toByteArray();
        if(!b.isEmpty()) h->restoreState(b);
    }
    aHistory->pathsSet(cfg.value(objectName()+"/History").value<QStringList>());
    aHistory->indexSet(cfg.value(objectName()+"/HistoryIdx",-1).value<int>());
    setPathSilent(cfg.value(objectName()+"/lastDirPath",QDir::homePath()).value<QString>());

    return true;
}

QString DirEntryListView::path()
{
    return aPath;
}

bool DirEntryListView::historyBackwardPossible()
{
    return aHistory->backwardPossible();
}

bool DirEntryListView::historyForwardPossible()
{
    return aHistory->forwardPossible();
}

bool DirEntryListView::cdUpPossible()
{
    return true;
}

bool DirEntryListView::eventFilter(QObject *watched, QEvent *event)
{
    if(event->type()==QEvent::KeyPress && watched==aDirEntryView){
        QKeyEvent *k= static_cast<QKeyEvent*>(event);
        if(k->key()==Qt::Key_Return) return false;
        if(k->key()==Qt::Key_Tab
                || k->key()==Qt::Key_Backtab
                || k->key()==Qt::Key_Backspace
                || k->key()==Qt::Key_Escape
                || k->key()==Qt::Key_Space
                || (k->modifiers()==Qt::AltModifier
                    && (k->key()==Qt::Key_Up
                        || k->key()==Qt::Key_Down
                        || k->key()==Qt::Key_Left
                        || k->key()==Qt::Key_Right))){
            keyPressEvent(k);
            return true;
        }

        if(k->modifiers()==Qt::ControlModifier || k->text().isEmpty()) return false;
        if(!aQuickFilter->isVisible()){
            aQuickFilter->patternClear();
        }
        aQuickFilter->show();
        aQuickFilter->setFocus();
        aQuickFilter->patternAppend(k->text());
        return true;
    }
    return ISettings::eventFilter(watched, event);
}

void DirEntryListView::selectedEntriesPrepare(const QString &fsOp)
{
    QList<QUrl> urls;
    QStringList paths;
    int row= -1;
    foreach(QModelIndex idx, aDirEntryView->selectionModel()->selection().indexes()){
        if(row==idx.row()) continue;
        row=idx.row();
        urls.append(QUrl::fromLocalFile(aPath+"/"+aDirEntry->at(idx.row()).get(DirEntryProperty::Name).value<QString>()));
        paths.append(QUrl::fromLocalFile(aPath+"/"+aDirEntry->at(idx.row()).get(DirEntryProperty::Name).value<QString>()).toLocalFile());
    }
    if(urls.count()>0){
        QMimeData *md= new QMimeData();
        md->setUrls(urls);
        md->setText(paths.join("\n"));
        md->setData("application/pw-fsop",QTextCodec::codecForName("utf-8")->fromUnicode(fsOp));
        QApplication::clipboard()->setMimeData(md);
    }
}

void DirEntryListView::paste(const QPoint &menuPos, const QMimeData *mime)
{
    qCritical()<<"op";
    if(mime->hasUrls()){
        QString op="undef";
        if(mime->formats().contains("application/pw-fsop"))
            op= QTextCodec::codecForName("utf-8")->toUnicode(mime->data("application/pw-fsop"));
        else {
            // show question copy or move
            aCopyMoveMenu->setActiveAction(aCopyMoveMenu->actions()[0]);
            aCopyMoveMenu->popup(menuPos);
            aCopyMoveMenu->setFocus();
        }
        qCritical()<<op;
    }
}

void DirEntryListView::setPath(const QString &path)
{
    if(path.trimmed().isEmpty()) return;

    aDirEntryView->setCursor(Qt::BusyCursor);
    aBusy->move(mapTo(this,aDirEntryView->pos()));
    aBusy->raise();
    aBusy->show();
    aBusy->start();
    emit busy();

#ifdef Q_OS_WIN32
    aWinTaskBarButton->setWindow(windowHandle());
    aWinTaskBarButton->progress()->setVisible(true);
    aWinTaskBarButton->progress()->setValue(50);
#endif

    aDirEntry->clear();

    aPath= path;
    aNaviBar->pathSet(aPath);
    aDirEntrySrc->fetch(path);
}

void DirEntryListView::setPathSilent(const QString &path)
{
    if(path.trimmed().isEmpty()) return;

    aDirEntryView->setCursor(Qt::BusyCursor);
    aBusy->move(mapTo(this,aDirEntryView->pos()));
    aBusy->raise();
    aBusy->show();
    aBusy->start();
    emit busy();

#ifdef Q_OS_WIN32
    aWinTaskBarButton->setWindow(windowHandle());
    aWinTaskBarButton->progress()->setVisible(true);
    aWinTaskBarButton->progress()->setValue(50);
#endif

    aDirEntry->clear();

    aPath= path;
    aNaviBar->pathSet(aPath);
    aDirEntrySrc->fetch(path);
}

void DirEntryListView::onItemActivated(const QModelIndex &index)
{
    if(index.isValid()){
        EntityInstance e= aDirEntry->at(index.row());
        QString entry= e.get(DirEntryProperty::Name).value<QString>();
        if(e.get(DirEntryProperty::isDir).value<bool>()){
            if(aQuickFilter->isVisible()){
                aQuickFilter->cancel();
                aDirEntryView->setFocus();
            }
            setPath(QDir::cleanPath(aPath+"/"+entry));
            emit activated(entry);
        }else{
            QFileInfo fi(aPath+"/"+entry);
            QDesktopServices::openUrl(QUrl::fromLocalFile(fi.absoluteFilePath()));
        }
    }
}

void DirEntryListView::cancel()
{
    aDirEntrySrc->cancel();
    aDirEntry->sortCancel();
    aDirEntry->filterCancel();
}

void DirEntryListView::onDirSrcVerified(bool ok, EntityInstance ent)
{
    aDirEntry->rowsFixedTopSet(EntityInstanceList()<<ent);
    if(!ok){
#ifdef Q_OS_WIN32
        aWinTaskBarButton->progress()->hide();
#endif
        emit ready();
        aBusy->stop();
        aBusy->hide();
        aDirEntryView->unsetCursor();
        QMessageBox::warning(this, tr("Warning!"), tr("Can't access dir '%1'!").arg(aDirEntrySrc->path()), QMessageBox::Ok, QMessageBox::NoButton);
        return;
    }
}

void DirEntryListView::onDirSrcComplete()
{
    aHistory->append(aPath);
    if(!aDirEntryView->currentIndex().isValid() && aDirEntry->rowCount()>0){
        aDirEntryView->setCurrentIndex(aDirEntry->index(0,0));
    }
    aBusy->stop();
    aBusy->hide();
    aDirEntryView->unsetCursor();
    aDirEntry->sort();
}

void DirEntryListView::onSortActionActivated(QAction *a)
{
    if(aDirEntryView->currentIndex().column()>= aDirEntry->columnCount()
            || aDirEntryView->currentIndex().column()<0
            || a->data().isNull()
            || !a->data().canConvert<def::SortOrder>()) return;
    def::SortOrder so= a->data().value<def::SortOrder>();
    aDirEntry->onSort(aDirEntryView->currentIndex().column(), so);
}

void DirEntryListView::onContextMenuAjustColumnsSizes()
{
    aDirEntryView->horizontalHeader()->resizeSections(QHeaderView::ResizeToContents);
}

void DirEntryListView::onSearching(const QString &pattern)
{
    if(pattern.isEmpty()){
        aDirEntry->filterClear();
        aDirEntryView->setFocus();
        aDirEntryView->selectRow(0);
        return;
    }
    EntityCompareConditions f;
    QVariantList vls;
    vls<<QVariant::fromValue(pattern+"*");
    f.append(EntityCompareCondition(aDirEntry->propertyForColumn(1)
                                    ,EntityComparePredicate::RegExp
                                    ,vls));
    aDirEntry->filterSet(f);
    aDirEntry->filterApply();
}

void DirEntryListView::onDirEntryBusy()
{
    aDirEntryView->setCursor(Qt::BusyCursor);
    aBusy->move(mapTo(this,aDirEntryView->pos()));
    aBusy->raise();
    aBusy->show();
    aBusy->start();
    emit busy();
}

void DirEntryListView::onDirEntryReady()
{
    aRowCountInfo->setText(QString::number(aDirEntry->rowCount())
                           +(aQuickFilter->isVisible()?"/"+QString::number(aDirEntry->rowCount()+aDirEntry->filteredRowCount()):""));
    aBusy->stop();
    aBusy->hide();
#ifdef Q_OS_WIN32
    aWinTaskBarButton->progress()->hide();
#endif

    aDirEntryView->unsetCursor();
    if(!aQuickFilter->isVisible()) aDirEntryView->setFocus();
    QApplication::alert(window());
}

void DirEntryListView::cdToCurrent()
{
    if(aDirEntryView->currentIndex().isValid()){
        EntityInstance e= aDirEntry->at(aDirEntryView->currentIndex().row());
        if(e.get(DirEntryProperty::isDir).value<bool>()){
            QDir d(aPath);
            if(d.cd(e.get(DirEntryProperty::Name).value<QString>()))
                setPath(d.absolutePath());
        }
    }
}

void DirEntryListView::cdUp()
{
    QString path= QDir::cleanPath(aPath+"/..");
    if(path.length()<aPath.length()) setPath(path);
}

void DirEntryListView::historyBack()
{
    if(aHistory->backwardPossible()) aHistory->backward();
}

void DirEntryListView::historyForward()
{
    if(aHistory->forwardPossible()) aHistory->forward();
}

void DirEntryListView::onCanceled()
{
    EntityInstance e= aDirEntry->getByPropertyFirst(DirEntryProperty::Name,"..");
    if(e.isSet(DirEntryProperty::Name)) return;
    aDirEntry->insert(0,e.set(DirEntryProperty::Name,"..").set(DirEntryProperty::isDir,true));
}

void DirEntryListView::onCustomContextMenuRequested(const QPoint &pos)
{
    if(aContextMenu==0){
        aContextMenu= new QMenu(this);
        QMenu *headerMenu= new QMenu(tr("&1 show/hide"),this);
        ColumnHeader *hdr= (ColumnHeader*)aDirEntryView->horizontalHeader();
        for(int c=0; c< hdr->count(); c++){
            QAction *act= new QAction(aDirEntry->headerData(c,Qt::Horizontal,Qt::DecorationRole).value<QPixmap>()
                                      ,"&"+QString::number(c+1)+" "+(
                                          aDirEntry->headerData(c,Qt::Horizontal,Qt::DisplayRole).value<QString>().isEmpty()
                                          ?aDirEntry->headerData(c,Qt::Horizontal,Qt::ToolTipRole).value<QString>()
                                         :aDirEntry->headerData(c,Qt::Horizontal,Qt::DisplayRole).value<QString>())
                                      ,headerMenu);
            act->setData(c);
            act->setCheckable(true);
            act->setChecked(!hdr->isSectionHidden(c));
            connect(act,&QAction::toggled, hdr, &ColumnHeader::onSectionShow);
            headerMenu->addAction(act);
        }
        QMenu *sortMenu= new QMenu(tr("&2 sort column"),this);
        QAction *act= new QAction("&1 ascending", sortMenu);
        act->setData(QVariant::fromValue(def::Ascending));
        sortMenu->addAction(act);
        act= new QAction("&2 descending", sortMenu);
        act->setData(QVariant::fromValue(def::Descending));
        sortMenu->addAction(act);
        act= new QAction("&3 unsorted", sortMenu);
        act->setData(QVariant::fromValue(def::Unsorted));
        sortMenu->addAction(act);
        aContextMenu->addMenu(headerMenu);
        aContextMenu->addMenu(sortMenu);
        aContextMenu->addAction(tr("&3 adjust size"),this, SLOT(onContextMenuAjustColumnsSizes()));

        connect(aContextMenu, &QMenu::triggered, this, &DirEntryListView::onSortActionActivated);
    }
    // FIXME: fix popup on screen positioning
    aContextMenu->popup(mapToGlobal(pos));//aDirEntryView->viewport()->mapToGlobal(pos));
}

void DirEntryListView::onCurrentRowChanged(const QModelIndex &current, const QModelIndex &previous)
{
    Q_UNUSED(previous)
    aDirEntryView->selectionModel()->select(current,QItemSelectionModel::SelectCurrent|QItemSelectionModel::Rows);
}

void DirEntryListView::onClipboardCopy()
{
    qCritical()<<"user choose copy";
}

void DirEntryListView::onClipboardMove()
{
    qCritical()<<"user choose move";
}

void DirEntryListView::onDirEntryModified()
{
    aRowCountInfo->setText(QString::number(aDirEntry->rowCount())
                           +(aQuickFilter->isVisible()?"/"+QString::number(aDirEntry->rowCount()+aDirEntry->filteredRowCount()):""));
}

void DirEntryListView::dragEnterEvent(QDragEnterEvent *event)
{
    if(event->mimeData()->hasUrls()){
        //        if(event->mimeData()->hasFormat("application/pw-fsop")){
        //            QString op= QTextCodec::codecForName("utf-8")->toUnicode(QApplication::clipboard()->mimeData()->data("application/pw-fsop"));
        //            if(op=="cut") event->setDropAction(Qt::MoveAction);
        //            else if(op=="cut") event->setDropAction(Qt::CopyAction);
        //            else return;
        //        }
        event->acceptProposedAction();
    }
}

void DirEntryListView::dropEvent(QDropEvent *event)
{
    if (event->source() == this && (event->possibleActions() & Qt::MoveAction
                                    || event->possibleActions() & Qt::CopyAction))
        return;
    if (event->proposedAction() == Qt::MoveAction) {
        event->acceptProposedAction();
        //            paste(mapToGlobal(event->pos()), event->mimeData());
        qCritical()<<"drop move";
        // Process the data from the event.
    } else if (event->proposedAction() == Qt::CopyAction) {
        event->acceptProposedAction();
        //            paste(mapToGlobal(event->pos()), event->mimeData());
        qCritical()<<"drop copy";
        // Process the data from the event.
    } else {
        // Ignore the drop.
        return;
    }}

void DirEntryListView::refresh()
{
    setPath(aNaviBar->path());
}

void DirEntryListView::historyDialogShow()
{
    HistoryDialog *dlg= new HistoryDialog(aHistory, this);
    connect(dlg, &HistoryDialog::activated, this, &DirEntryListView::setPath);
    dlg->exec();
    delete dlg;
}

void DirEntryListView::bokmarksDialogShow()
{

}

void DirEntryListView::aboutShow()
{
    AboutPathWalker *dlg= new AboutPathWalker(this);
    dlg->exec();
    delete dlg;
}

void DirEntryListView::keyPressEvent(QKeyEvent *event)
{
    if(keyPressEventProcess(event)) return;
    event->ignore();
    QWidget::keyPressEvent(event);
}

bool DirEntryListView::keyPressEventProcess(QKeyEvent *event)
{
    if(event->modifiers()==Qt::AltModifier){
        if(event->key()==Qt::Key_Up){
            cdUp();
            return true;
        }
        if(event->key()==Qt::Key_Down){
            cdToCurrent();
            return true;
        }
        if(event->key()==Qt::Key_Left){
            historyBack();
            return true;
        }
        if(event->key()==Qt::Key_Right){
            historyForward();
            return true;
        }
    }
    if(event->modifiers()==Qt::ControlModifier){
        if(event->key()==Qt::Key_C){
            selectedEntriesPrepare("copy");
            return true;
        }
        if(event->key()==Qt::Key_X){
            selectedEntriesPrepare("cut");
            return true;
        }
        if(event->key()==Qt::Key_V){
            paste(mapToGlobal(QPoint(aDirEntryView->width()/2,aDirEntryView->height()/2))
                  ,QApplication::clipboard()->mimeData());
            return true;
        }
        if(event->key()==Qt::Key_F){
            if(aQuickFilter->isVisible()){
                aQuickFilter->cancel();
                aDirEntryView->setFocus();
            }else{
                aQuickFilter->show();
                aQuickFilter->setFocus();
            }
            return true;
        }
        if(event->key()==Qt::Key_T){
            aNaviBar->toolsMenuPopup();
            return true;
        }
        if(event->key()==Qt::Key_N){
            QProcess::startDetached(QCoreApplication::applicationFilePath());
            return true;
        }
    }
    if(event->modifiers()==(Qt::ControlModifier|Qt::ShiftModifier)){
        if(event->key()==Qt::Key_C){
            QStringList urls;
            int row= -1;
            foreach(QModelIndex idx, aDirEntryView->selectionModel()->selection().indexes()){
                if(row==idx.row()) continue;
                row=idx.row();
                urls.append(QUrl::fromLocalFile(aPath+"/"+aDirEntry->at(idx.row()).get(DirEntryProperty::Name).value<QString>()).toLocalFile());
            }
            if(urls.count()>0){
                QMimeData *md= new QMimeData();
                md->setText(urls.join('\n'));
                QApplication::clipboard()->setMimeData(md);
            }
            return true;
        }
    }
    if(event->key()==Qt::Key_F5){
        setPath(aPath);
        return true;
    }
    if(event->key()==Qt::Key_Escape){
        if(aBusy->isVisible()) cancel();
        if(aQuickFilter->isVisible()){
            aQuickFilter->cancel();
            aDirEntryView->setFocus();
        }
        return true;
    }
    if(event->key()==Qt::Key_Backspace){
        cdUp();
        return true;
    }

    if(event->key()==Qt::Key_Space){
        int row=-1;
        foreach(QModelIndex idx, aDirEntryView->selectionModel()->selection().indexes()){
            if(row==idx.row()) continue;
            row= idx.row();
            EntityInstance e= aDirEntry->at(idx.row());
            if(e.get(DirEntryProperty::Name).value<QString>()=="..") continue;
            if(idx.isValid()){
                Qt::CheckState m= e.get(DirEntryProperty::Mark).value<Qt::CheckState>();
                aDirEntry->setProperty(idx.row(), DirEntryProperty::Mark
                                       ,QVariant::fromValue(m==Qt::Checked? Qt::Unchecked : Qt::Checked));
            }
            // TODO: notify if data has marked rows: aDirEntryView->setStyleSheet(aDirEntry->hasMarked()?"QTableView{border: 1px solid yellow;}":"");
        }
        return true;
    }
    return false;
}

void DirEntryListView::doInitLayout()
{
    QVBoxLayout *lt= new QVBoxLayout();
    lt->addWidget(aNaviBar);
    lt->addWidget(aDirEntryView,1);
    QHBoxLayout *searchLt= new QHBoxLayout();
    searchLt->setMargin(0);
    searchLt->addWidget(aQuickFilter,1);
    searchLt->addWidget(aRowCountInfo);
    lt->addLayout(searchLt);
    lt->setMargin(3);
    lt->setSpacing(3);
    setLayout(lt);
}

void DirEntryListView::doInitConnections()
{
    connect(aDirEntrySrc, SIGNAL(dataReady(EntityInstance)), aDirEntry, SLOT(append(EntityInstance)));
    connect(aDirEntrySrc, &DirEntrySrc::finished, this, &DirEntryListView::onDirSrcComplete);
    connect(aDirEntrySrc, &DirEntrySrc::finished, this, &DirEntryListView::ready);
    connect(aDirEntrySrc, &DirEntrySrc::canceled, this, &DirEntryListView::onCanceled);
    connect(aDirEntrySrc, &DirEntrySrc::verified, this, &DirEntryListView::onDirSrcVerified);

    connect(aDirEntryView, &QTableView::activated, this, &DirEntryListView::onItemActivated);
    connect(aDirEntryView->selectionModel(), &QItemSelectionModel::currentRowChanged, this, &DirEntryListView::onCurrentRowChanged);

    connect(aDirEntry, &EntityListModel::busy, this, &DirEntryListView::onDirEntryBusy);
    connect(aDirEntry, &EntityListModel::ready, this, &DirEntryListView::onDirEntryReady);
    connect(aDirEntry, &EntityListModel::modified, this, &DirEntryListView::onDirEntryModified);

    connect(aBusy, &BusyDialog::canceled, this, &DirEntryListView::cancel);
    connect(aBusy, &BusyDialog::canceled, aDirEntrySrc, &DirEntrySrc::cancel);
    connect(aBusy, &BusyDialog::canceled, aDirEntry, &EntityListModel::sortCancel);
    connect(aBusy, &BusyDialog::canceled, aDirEntry, &EntityListModel::filterCancel);

    connect(this, &DirEntryListView::customContextMenuRequested, this, &DirEntryListView::onCustomContextMenuRequested);

    connect(aQuickFilter, &QuickFilter::searching, this, &DirEntryListView::onSearching);

    connect(aHistory, &DirPathHistory::currentChanged, this, &DirEntryListView::setPath);

    connect(aNaviBar, &DirEntryNaviBar::historyDialogRequested, this, &DirEntryListView::historyDialogShow);
    connect(aNaviBar, &DirEntryNaviBar::bookmarksDialogRequested, this, &DirEntryListView::bokmarksDialogShow);
    connect(aNaviBar, &DirEntryNaviBar::aboutRequested, this, &DirEntryListView::aboutShow);
}
