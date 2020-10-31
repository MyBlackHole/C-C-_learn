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

#ifndef DIRENTRYLISTVIEW_H
#define DIRENTRYLISTVIEW_H

#include <QWidget>
#include <QTableView>
#include <QListView>
#include <QStringListModel>
#include <QSplitter>
#include <QDateTime>
#include <QMutex>
#include <QMenu>

#include "settingsstore/isettings.h"
#include "direntry.h"
#include "quickfilter.h"
#include "dirpathhistory.h"

#include "entitylistmodel.h"

class DirEntrySrc;
class QLineEdit;
class BusyDialog;
class QLabel;
class DirEntryNaviBar;
class QMimeData;
class QMenu;

#ifdef Q_OS_WIN32
class QWinTaskbarButton;
#endif

class DirEntryListView : public ISettings
{
    Q_OBJECT
public:
    explicit DirEntryListView(QWidget *parent = 0);
    bool settingsSave();
    bool settingsRestore();
    QString path();
    bool historyBackwardPossible();
    bool historyForwardPossible();
    bool cdUpPossible();
    virtual bool eventFilter(QObject *watched, QEvent *event);

signals:
    void busy();
    void ready();
    void activated(QString entryName);

public slots:
    void setPath(const QString &path);
    void setPathSilent(const QString &path);
    void onItemActivated(const QModelIndex &index);
    void cancel();
    void onDirSrcVerified(bool ok, EntityInstance ent);
    void onDirSrcComplete();

    void onSortActionActivated(QAction *a);
    void onContextMenuAjustColumnsSizes();
    void onSearching(const QString &pattern);

    void onDirEntryBusy();
    void onDirEntryReady();

    void cdToCurrent();
    void cdUp();
    void historyBack();
    void historyForward();

    void refresh();

    void historyDialogShow();
    void bokmarksDialogShow();
    void aboutShow();

protected slots:
    void onCanceled();
    void onCustomContextMenuRequested(const QPoint &pos);
    void onCurrentRowChanged(const QModelIndex &current, const QModelIndex &previous);
    void onClipboardCopy();
    void onClipboardMove();
    void onDirEntryModified();

protected:
    virtual void dragEnterEvent(QDragEnterEvent *event);
    virtual void dropEvent(QDropEvent *event);
    virtual void keyPressEvent(QKeyEvent *event);
    bool keyPressEventProcess(QKeyEvent *event);
    void selectedEntriesPrepare(const QString &fsOp);
    void paste(const QPoint &menuPos, const QMimeData *mime);

private:
    QString aPath;
    EntityListModel *aDirEntry;
    DirEntrySrc *aDirEntrySrc;

    QTableView *aDirEntryView;
    QMenu *aContextMenu;

    QuickFilter *aQuickFilter;
    QLabel *aRowCountInfo;
    BusyDialog *aBusy;

    DirPathHistory *aHistory;
    DirEntryNaviBar *aNaviBar;

    QMenu *aCopyMoveMenu;

#ifdef Q_OS_WIN32
    QWinTaskbarButton *aWinTaskBarButton;
#endif
    void doInitLayout();
    void doInitConnections();

};

#endif // DIRENTRYLISTVIEW_H
