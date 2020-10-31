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
#include <QStringListModel>
#include <QItemSelectionModel>

#include "historydialog.h"

HistoryDialog::HistoryDialog(DirPathHistory *history, QWidget *parent) :
    QDialog(parent),aHistory(history)
{
    setWindowTitle(tr("History"));
    setWindowIcon(QIcon("://img/clock-history.png"));

    aModel= new QStringListModel(aHistory->paths());

    aView= new QListView();
    aView->setEditTriggers(QAbstractItemView::NoEditTriggers);
    aView->setSelectionBehavior(QAbstractItemView::SelectRows);
    aView->setSelectionMode(QAbstractItemView::ExtendedSelection);
    aView->setModel(aModel);
    aView->setCurrentIndex(aModel->index(aHistory->index()));
    aView->selectionModel()->select(aView->currentIndex(),QItemSelectionModel::Select);
    aView->scrollTo(aView->currentIndex());
    connect(aView, &QListView::activated, this, &HistoryDialog::onItemActivated);

    QPushButton *buttonClose= new QPushButton(tr("Close"));
    buttonClose->setDefault(true);
    connect(buttonClose, &QPushButton::clicked, this, &QDialog::accept);

    QHBoxLayout *ctrlt= new QHBoxLayout();
    ctrlt->addStretch(1);
    ctrlt->addWidget(buttonClose);
    ctrlt->addStretch(1);

    QVBoxLayout *lt= new QVBoxLayout();
    lt->addWidget(aView,1);
    lt->addLayout(ctrlt);
    lt->setMargin(3);

    setLayout(lt);

    QAction *act= new QAction(tr("Cancel"), this);
    act->setShortcut(QKeySequence(Qt::Key_Escape));
    connect(act, &QAction::triggered, this, &QDialog::reject);
    addAction(act);
}

void HistoryDialog::onItemActivated(const QModelIndex &index)
{
    aHistory->indexSet(index.row());
    emit activated(aModel->data(index,Qt::EditRole).value<QString>());
}

void HistoryDialog::keyPressEvent(QKeyEvent *event)
{
    if(event->key()==Qt::Key_Delete){
        QModelIndexList idxs= aView->selectionModel()->selectedRows();
        int ret= QMessageBox::question(this, tr("Delete")
                                       ,tr("Delete %1 paths?").arg(idxs.count())
                                       ,QMessageBox::Yes,QMessageBox::No);
        if(ret!=QMessageBox::Yes || idxs.isEmpty()) return;
        int row= idxs.at(0).row();
        row= row>0?--row:0;
        QStringList rem;
        foreach (QModelIndex idx, idxs){
            rem.append(aModel->data(idx,Qt::EditRole).value<QString>());
        }
        QStringList b= aModel->stringList();
        foreach (QString s, rem){
            b.removeAll(s);
        }
        aModel->setStringList(b);
        aView->setCurrentIndex(aModel->index(row));
        aView->selectionModel()->select(aView->currentIndex(),QItemSelectionModel::Select);
        aHistory->pathsSet(b);
        aHistory->indexSet(b.isEmpty()?-1:row<b.count()?row:b.count()-1);
    }
}
