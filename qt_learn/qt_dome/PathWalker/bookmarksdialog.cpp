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

#include <QListView>
#include <QKeyEvent>
#include <QVBoxLayout>
#include <QMessageBox>
#include <QPushButton>
#include <QAction>

#include "bookmarksdialog.h"

BookmarksDialog::BookmarksDialog(QStringList bookmarks, QWidget *parent) :
    QDialog(parent)
{
    setWindowTitle(tr("Bookmarks"));
    setWindowIcon(QIcon("://img/star.png"));

    aBookmarks= new QStringListModel(bookmarks);

    aEditor= new QListView();
    aEditor->setEditTriggers(QAbstractItemView::NoEditTriggers);
    aEditor->setSelectionBehavior(QAbstractItemView::SelectRows);
    aEditor->setSelectionMode(QAbstractItemView::ExtendedSelection);
    aEditor->setModel(aBookmarks);
    connect(aEditor, &QListView::activated, this, &BookmarksDialog::onItemActivated);

    QPushButton *buttonClose= new QPushButton(tr("Close"));
    buttonClose->setDefault(true);
    connect(buttonClose, &QPushButton::clicked, this, &QDialog::accept);

    QHBoxLayout *ctrlt= new QHBoxLayout();
    ctrlt->addStretch(1);
    ctrlt->addWidget(buttonClose);
    ctrlt->addStretch(1);

    QVBoxLayout *lt= new QVBoxLayout();
    lt->addWidget(aEditor,1);
    lt->addLayout(ctrlt);
    lt->setMargin(3);

    setLayout(lt);

    QAction *act= new QAction(tr("Cancel"), this);
    act->setShortcut(QKeySequence(Qt::Key_Escape));
    connect(act, &QAction::triggered, this, &QDialog::reject);
    addAction(act);
}

QStringList BookmarksDialog::bookmarks()
{
    return aBookmarks->stringList();
}

void BookmarksDialog::onItemActivated(const QModelIndex &index)
{
    emit activated(aBookmarks->data(index,Qt::EditRole).value<QString>());
}

void BookmarksDialog::keyPressEvent(QKeyEvent *event)
{
    if(event->key()==Qt::Key_Delete){
        QModelIndexList idxs= aEditor->selectionModel()->selectedRows();
        int ret= QMessageBox::question(this, tr("Delete")
                                       ,tr("Delete %1 bokkmarks?").arg(idxs.count())
                                       ,QMessageBox::Yes,QMessageBox::No);
        if(ret==QMessageBox::Yes){
            QStringList rem;
            foreach (QModelIndex idx, idxs){
                rem.append(aBookmarks->data(idx,Qt::EditRole).value<QString>());
            }
            QStringList b= aBookmarks->stringList();
            foreach (QString s, rem){
                b.removeAll(s);
            }
            aBookmarks->setStringList(b);
        }
    }
}
