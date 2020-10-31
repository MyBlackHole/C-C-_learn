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

#ifndef DIRPATHHISTORY_H
#define DIRPATHHISTORY_H

#include <QObject>
#include <QStringList>

class DirPathHistory : public QObject
{
    Q_OBJECT
public:
    explicit DirPathHistory(QObject *parent = 0);
    int index() const;
    void indexSet(int index);
    QStringList paths() const;
    void pathsSet(const QStringList &paths);
    QString path() const;
    QString pathAt(int index) const;
    bool forwardPossible();
    bool backwardPossible();

signals:
    void currentChanged(QString path);

public slots:
    void forward();
    void backward();
    void append(const QString &path);

private:
    QStringList aPaths; // visited dir paths
    // TODO: save/restore current row in dir entries list, ?selection?, scroll position, quick filter
    int aIdx; // index of current path
    // TODO: add MaxLength for history
};

#endif // DIRPATHHISTORY_H
