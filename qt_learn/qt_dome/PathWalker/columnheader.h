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

#ifndef COLUMNHEADER_H
#define COLUMNHEADER_H

#include <QtWidgets/QHeaderView>
#include <QtWidgets/QMenu>
#include <QMap>
#include <QList>
#include "def.h"

class ColumnHeader : public QHeaderView
{
    Q_OBJECT
public:
    explicit ColumnHeader(QWidget *parent = 0);
    QList<QAction *> actions() const;
    bool restoreState(const QByteArray &state);
    QByteArray saveState() const;

public slots:
    void onSectionClicked(int logicalIdx);
    void onCustomContextMenuRequested(const QPoint &pos);
    void onSectionCountChanged(int oldCount, int newCount);
    void onSectionShow(bool isShow);
    void clearSorting();

protected:
    virtual void paintSection(QPainter * painter, const QRect & rect, int logicalIndex)const;

private:
    QMenu *ctxMenu;
};

#endif // COLUMNHEADER_H
