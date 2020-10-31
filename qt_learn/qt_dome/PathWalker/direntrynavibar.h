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

#ifndef DIRENTRYNAVIBAR_H
#define DIRENTRYNAVIBAR_H

#include "settingsstore/isettings.h"

#include <QStringList>
#include <QDateTime>

class QToolButton;
class QAction;
class QLineEdit;
class DirEntryListView;
class PathChangeWatcher;

class DirEntryNaviBar : public ISettings
{
    Q_OBJECT
public:
    explicit DirEntryNaviBar(DirEntryListView *dirEntryView, QWidget *parent = 0);
    bool settingsSave();
    bool settingsRestore();
    QString path() const;

signals:
    void pathChangedByUser(QString path);
    void historyDialogRequested();
    void bookmarkAddDialogRequested();
    void bookmarksDialogRequested();
    void aboutRequested();

public slots:
    void pathSet(const QString &path);
    void toolsMenuPopup();
    void onActHistoryView();
    void onActBookmarksView();
    void onActAbout();

protected slots:
    void onPathEditReturnPressed();
    void onButtonBookmarkToggle();
    void onDirModified(QString path);
    void onButtonBackward();
    void onButtonForkward();
    void onButtonUp();

protected:
    virtual void showEvent(QShowEvent *event);

private:
    DirEntryListView *aDirEntryView;
    QLineEdit* aPathEdit;

    QToolButton *aButtonBackward;
    QToolButton *aButtonForward;
    QToolButton *aButtonUp;
    QToolButton *aButtonRefresh;
    QToolButton *aButtonBookmarkAdd;

    QToolButton *aButtonTools;

    QAction *aActHistoryView;
    QAction *aActBookmarksView;
    QAction *aActAbout;

    QStringList aBookmarks;
    PathChangeWatcher *aPathWatcher;

    void doInitLayout();
    void doInitConnections();
};

#endif // DIRENTRYNAVIBAR_H
