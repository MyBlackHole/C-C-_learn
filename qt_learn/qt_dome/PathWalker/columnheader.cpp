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

#include <QtWidgets/QAction>
#include <QtGui/QPainter>
#include "columnheader.h"

ColumnHeader::ColumnHeader(QWidget *parent) :
    QHeaderView(Qt::Horizontal,parent)
{
    ctxMenu= 0;
    setSectionsClickable(true);
    setTextElideMode(Qt::ElideRight);
    connect(this,SIGNAL(sectionClicked(int)),this,SLOT(onSectionClicked(int)));
    setContextMenuPolicy(Qt::CustomContextMenu);
    connect(this,SIGNAL(customContextMenuRequested(QPoint)),this,SLOT(onCustomContextMenuRequested(QPoint)));
    connect(this,SIGNAL(sectionCountChanged(int,int)), this ,SLOT(onSectionCountChanged(int,int)));
}

QList<QAction *> ColumnHeader::actions() const
{
    if(ctxMenu==0) return QList<QAction*>();
    return ctxMenu->actions();
}

bool ColumnHeader::restoreState(const QByteArray &state)
{
    QByteArray st;
    QDataStream ds(state);
    ds>>st;
    bool ret= QHeaderView::restoreState(st);
    QList<QPair<int,int> > so;
    ds>>so;
    typedef QPair<int,int> csOrder;
    foreach(csOrder co,so){
        model()->setHeaderData(co.first,Qt::Horizontal,co.second,Qt::UserRole+1);
    }

    update();
    return ret;
}

QByteArray ColumnHeader::saveState() const
{
    QByteArray st;
    QDataStream ds(&st,QIODevice::Append);
    ds<<QHeaderView::saveState();
    QMap<int,QPair<int,int> > map;
    for(int c=0; c<model()->columnCount(); c++){
        map.insert(model()->headerData(c,Qt::Horizontal,Qt::UserRole+2).toInt()
                   ,qMakePair<int,int>(c
                                       ,model()->headerData(c,Qt::Horizontal,Qt::UserRole+1).toInt()));
    }
    ds<<map.values();
    return st;
}

void ColumnHeader::onSectionClicked(int logicalIdx)
{
    QVariant csov= model()->headerData(logicalIdx,Qt::Horizontal,Qt::UserRole+1);
    def::SortOrder cso= (def::SortOrder)(csov.isValid()?csov.toInt():def::Unsorted);

    switch(cso){
    case def::Ascending:
        model()->setHeaderData(logicalIdx,Qt::Horizontal,def::Descending, Qt::UserRole+1);
        break;
    case def::Descending:
        model()->setHeaderData(logicalIdx,Qt::Horizontal,def::Unsorted, Qt::UserRole+1);
        break;
    default: model()->setHeaderData(logicalIdx,Qt::Horizontal,def::Ascending, Qt::UserRole+1);
    }
}

void ColumnHeader::onCustomContextMenuRequested(const QPoint &pos)
{
    if(ctxMenu==0) return;
    foreach(QAction *a,ctxMenu->actions()){
        if(a->data().isNull()) continue;
        a->setChecked(!isSectionHidden(a->data().toInt()));
    }

    ctxMenu->popup(viewport()->mapToGlobal(pos));
}

void ColumnHeader::onSectionCountChanged(int oldCount, int newCount)
{
    Q_UNUSED(oldCount)
    Q_UNUSED(newCount)

    if(ctxMenu!=0) delete ctxMenu;
    if(model()==0) return;

    ctxMenu= new QMenu(this);
    ctxMenu->addAction(tr("clear sorting"),this,SLOT(clearSorting()));
    ctxMenu->addSeparator();

    QAction *a;
    for(int i=0; i<count(); i++){
        QString title= model()->headerData(i,orientation()).toString();
        a= new QAction((title.isEmpty()?model()->headerData(i,orientation(),Qt::ToolTipRole).toString():title),this);
        a->setData(i);
        a->setCheckable(true);
        a->setChecked(!isSectionHidden(i));
        connect(a,SIGNAL(toggled(bool)),this,SLOT(onSectionShow(bool)));
        ctxMenu->addAction(a);
    }
}

void ColumnHeader::onSectionShow(bool isShow)
{
    QAction *a= qobject_cast<QAction*>(sender());
    if(a==0) return;
    if(isShow) showSection(a->data().toInt());
    else hideSection(a->data().toInt());
}

void ColumnHeader::clearSorting()
{
    QAbstractItemModel *m= model();
    if(m==0) return;
    for(int i=0; i<count(); i++){
        m->setHeaderData(i,Qt::Horizontal,def::Unsorted,Qt::UserRole+1);
    }
}

void ColumnHeader::paintSection(QPainter *painter, const QRect &rect, int logicalIndex) const
{
    painter->save();
    QHeaderView::paintSection(painter, rect, logicalIndex);
    painter->restore();

    QVariant cso= model()->headerData(logicalIndex,Qt::Horizontal, Qt::UserRole+1);
    QVariant csi= model()->headerData(logicalIndex,Qt::Horizontal, Qt::UserRole+2);

    if(cso.isNull() || (def::SortOrder)(cso.toInt())== def::Unsorted || csi.isNull()) return;

    QPixmap px;
    switch((def::SortOrder)cso.toInt()){
    case def::Ascending:
        px= QPixmap(":img/asc.png");
        break;
    default: px= QPixmap(":img/desc.png");
    }

    QRect pxr;
    pxr.setHeight(px.height());
    pxr.setWidth(px.width());
    pxr.moveRight(rect.right()-2);
    pxr.moveTop(rect.height()-pxr.height()-2);

    painter->drawPixmap(pxr, px);

    painter->save();
    QFont f= painter->font();
    f.setPointSize(6);
    painter->setFont(f);

    QFontMetrics fm(painter->fontMetrics());
    QRect fmr;
    fmr.setWidth(fm.width(QString::number(csi.toInt())));
    fmr.setHeight(fm.height());
    fmr.moveRight(rect.right()-4);
    fmr.moveTop((rect.height()-fmr.height())/2);

    painter->setPen(QColor(0,128,192));
    painter->drawText(fmr, QString::number(csi.toInt()));
    painter->restore();

}
