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

#include <QtWidgets>

#include "direntrynavibar.h"
#include "direntrylistview.h"
#include "visualkeyboardnavigator/visualkeyboardnavigator.h"
#include "pathchangewatcher.h"
#include "bookmarksdialog.h"
#include "settingsstore/settingsstore.h"

DirEntryNaviBar::DirEntryNaviBar(DirEntryListView *dirEntryView, QWidget *parent) :
    ISettings(parent),aDirEntryView(dirEntryView)
{
    setObjectName(tr("NavigationBar"));
    aPathEdit= new QLineEdit();
    aPathEdit->setPlaceholderText(tr("Enter path to walk..."));
    aPathEdit->setWhatsThis(tr("Enter path to walk..."));

    aButtonBackward= new QToolButton();
    aButtonBackward->setAutoRaise(true);
    aButtonBackward->setIcon(QIcon("://img/arrow-180.png"));
    aButtonBackward->setToolTip(tr("backward"));
    aButtonForward= new QToolButton();
    aButtonForward->setAutoRaise(true);
    aButtonForward->setIcon(QIcon("://img/arrow.png"));
    aButtonForward->setToolTip(tr("forward"));
    aButtonUp= new QToolButton();
    aButtonUp->setAutoRaise(true);
    aButtonUp->setIcon(QIcon("://img/arrow-090.png"));
    aButtonUp->setToolTip(tr("level up"));
    aButtonRefresh= new QToolButton();
    aButtonRefresh->setAutoRaise(true);
    aButtonRefresh->setIcon(QIcon("://img/arrow-circle-315.png"));
    aButtonRefresh->setToolTip(tr("refresh"));
    aButtonBookmarkAdd= new QToolButton();
    aButtonBookmarkAdd->setAutoRaise(true);
    aButtonBookmarkAdd->setIcon(QIcon("://img/star-small-empty.png"));
    aButtonBookmarkAdd->setToolTip(tr("bookmark"));
    aButtonBookmarkAdd->setWhatsThis(tr("Add/Remove bookmark"));

    aButtonTools= new QToolButton();
    aButtonTools->setAutoRaise(true);
    aButtonTools->setIcon(QIcon("://img/edit-alignment-justify.png"));
    aButtonTools->setToolTip(tr("tools"));

    aActHistoryView= new QAction(QIcon("://img/clock-history.png"),tr("&1 History"),this);
    aActBookmarksView= new QAction(QIcon("://img/star.png"),tr("&2 Bookmaks"),this);
    aActAbout= new QAction(tr("&3 About 'Path Walker'"),this);

    QMenu *toolMenu= new QMenu(tr("tools"),aButtonTools);
    toolMenu->addAction(aActHistoryView);
    toolMenu->addAction(aActBookmarksView);
    toolMenu->addAction(aActAbout);

    aButtonTools->setMenu(toolMenu);
    aButtonTools->setPopupMode(QToolButton::InstantPopup);

    aPathWatcher= new PathChangeWatcher();

    doInitLayout();
    doInitConnections();

    setFocusProxy(aPathEdit);

    VisualKeyboardNavigator::registerTag(Qt::Key_1, new VisualTag(tr("1"), window()), aPathEdit);
}

bool DirEntryNaviBar::settingsSave()
{
    QSettings cfg(SettingsStore::cfgPath(),QSettings::IniFormat);
    cfg.setValue(objectName()+"/Bookmarks",aBookmarks);
    return true;
}

bool DirEntryNaviBar::settingsRestore()
{
    QSettings cfg(SettingsStore::cfgPath(),QSettings::IniFormat);
    aBookmarks= cfg.value(objectName()+"/Bookmarks").value<QStringList>();
    return true;
}

QString DirEntryNaviBar::path() const
{
    return aPathEdit->text();
}

void DirEntryNaviBar::pathSet(const QString &path)
{
    aPathEdit->setText(path);
    aPathWatcher->watch(path);
    aPathEdit->setStyleSheet("QLineEdit{background:white;"
                             " border: 1px solid lightgrey;"
                             " border-right: none;"
                             "}"
                             );
    aButtonBookmarkAdd->setIcon(aBookmarks.contains(aPathEdit->text(),Qt::CaseInsensitive)
                                ? QIcon("://img/star-small.png")
                                : QIcon("://img/star-small-empty.png"));
}

void DirEntryNaviBar::toolsMenuPopup()
{
    aButtonTools->click();
}

void DirEntryNaviBar::onActHistoryView()
{

}

void DirEntryNaviBar::onActBookmarksView()
{
    BookmarksDialog *dlg= new BookmarksDialog(aBookmarks, this);
    connect(dlg, &BookmarksDialog::activated, this, &DirEntryNaviBar::pathSet);
    connect(dlg, &BookmarksDialog::activated, this, &DirEntryNaviBar::pathChangedByUser);
    dlg->exec();
    QStringList res= dlg->bookmarks();
    if(res.count()!=aBookmarks.count()){
        aBookmarks= res;
        aButtonBookmarkAdd->setIcon(aBookmarks.contains(aPathEdit->text(),Qt::CaseInsensitive)
                                    ? QIcon("://img/star-small.png")
                                    : QIcon("://img/star-small-empty.png"));
    }
    delete dlg;
}

void DirEntryNaviBar::onActAbout()
{

}

void DirEntryNaviBar::onPathEditReturnPressed()
{
    emit pathChangedByUser(aPathEdit->text());
}

void DirEntryNaviBar::onButtonBookmarkToggle()
{
    if(!aBookmarks.contains(aPathEdit->text(),Qt::CaseInsensitive)){
        aBookmarks.append(aPathEdit->text());
        aButtonBookmarkAdd->setIcon(QIcon("://img/star-small.png"));
    }else{
        int ret= QMessageBox::question(this, tr("Bookmark"), tr("Delete bookmark for path '%1'?").arg(aPathEdit->text()),QMessageBox::Yes, QMessageBox::No);
        if(ret==QMessageBox::Yes){
            aBookmarks.removeAll(aPathEdit->text());
            aButtonBookmarkAdd->setIcon(QIcon("://img/star-small-empty.png"));
        }
    }
}

void DirEntryNaviBar::onDirModified(QString path)
{
    if(path != aPathEdit->text()) return;
    aPathEdit->setStyleSheet("QLineEdit{background:lightgreen;"
                             " border: 1px solid lightgrey;"
                             " border-right: none;"
                             "}"
                             );
    QApplication::alert(window());
}

void DirEntryNaviBar::onButtonBackward()
{
    aDirEntryView->historyBack();
    aButtonBackward->setEnabled(aDirEntryView->historyBackwardPossible());
    aButtonForward->setEnabled(aDirEntryView->historyForwardPossible());
}

void DirEntryNaviBar::onButtonForkward()
{
    aDirEntryView->historyForward();
    aButtonBackward->setEnabled(aDirEntryView->historyBackwardPossible());
    aButtonForward->setEnabled(aDirEntryView->historyForwardPossible());
}

void DirEntryNaviBar::onButtonUp()
{
    aDirEntryView->cdUp();
}

void DirEntryNaviBar::showEvent(QShowEvent *event)
{
    Q_UNUSED(event)
    aPathEdit->setFixedHeight(aButtonBookmarkAdd->height());
}

void DirEntryNaviBar::doInitLayout()
{
    aPathEdit->setStyleSheet("QLineEdit{background:white;"
                             " border: 1px solid lightgrey;"
                             " border-right: none;"
                             "}"
                             );
    aButtonBookmarkAdd->setStyleSheet("QToolButton{background:white;"
                                      " border: 1px solid lightgrey;"
                                      " border-left: none;"
                                      "} "
                                      "QToolButton:focus{background:lightblue;"
                                      " border: 1px solid lightgrey;"
                                      " border-left: none;"
                                      "}");
    QString style= "QToolButton:focus{background:lightblue;}";
    aButtonBackward->setStyleSheet(style);
    aButtonForward->setStyleSheet(style);
    aButtonUp->setStyleSheet(style);
    aButtonRefresh->setStyleSheet(style);
    aButtonTools->setStyleSheet(style);

    QHBoxLayout *lt= new QHBoxLayout();
    lt->addWidget(aButtonBackward);
    lt->addWidget(aButtonForward);
    lt->addWidget(aButtonUp);
    lt->addWidget(aButtonRefresh);
    lt->addWidget(aPathEdit);
    lt->addWidget(aButtonBookmarkAdd);
    lt->addSpacing(3);
    lt->addWidget(aButtonTools);
    lt->setSpacing(0);
    lt->setMargin(0);
    setLayout(lt);
}

void DirEntryNaviBar::doInitConnections()
{
    connect(aActHistoryView, &QAction::triggered, this, &DirEntryNaviBar::historyDialogRequested);
    connect(aActBookmarksView, &QAction::triggered, this, &DirEntryNaviBar::onActBookmarksView);
    connect(aActBookmarksView, &QAction::triggered, this, &DirEntryNaviBar::bookmarksDialogRequested);
    connect(aActAbout, &QAction::triggered, this, &DirEntryNaviBar::aboutRequested);
    connect(this, &DirEntryNaviBar::pathChangedByUser, aDirEntryView, &DirEntryListView::setPath);

    connect(aButtonBackward, &QToolButton::clicked, this, &DirEntryNaviBar::onButtonBackward);
    connect(aButtonForward, &QToolButton::clicked, this, &DirEntryNaviBar::onButtonForkward);
    connect(aButtonUp, &QToolButton::clicked, this, &DirEntryNaviBar::onButtonUp);
    connect(aButtonRefresh, &QToolButton::clicked, this, &DirEntryNaviBar::onPathEditReturnPressed);
    connect(aButtonBookmarkAdd, &QToolButton::clicked, this, &DirEntryNaviBar::onButtonBookmarkToggle);
    connect(aPathEdit, &QLineEdit::returnPressed, this, &DirEntryNaviBar::onPathEditReturnPressed);
    connect(aPathWatcher, &PathChangeWatcher::modified, this, &DirEntryNaviBar::onDirModified);
}
