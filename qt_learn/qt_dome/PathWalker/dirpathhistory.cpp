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

#include "dirpathhistory.h"

DirPathHistory::DirPathHistory(QObject *parent) :
    QObject(parent),aIdx(-1)
{
}

int DirPathHistory::index() const
{
    return aIdx;
}

void DirPathHistory::indexSet(int index)
{
    aIdx= index;
}

QStringList DirPathHistory::paths() const
{
    return aPaths;
}

void DirPathHistory::pathsSet(const QStringList &paths)
{
    aPaths= paths;
}

QString DirPathHistory::path() const
{
    return pathAt(aIdx);
}

QString DirPathHistory::pathAt(int index) const
{
    if(index<0 || index>=aPaths.length()) return QString();
    return aPaths.at(index);
}

bool DirPathHistory::forwardPossible()
{
    return aIdx<0 || aIdx+1>=aPaths.length()? false : true;
}

bool DirPathHistory::backwardPossible()
{
    return aIdx-1<0? false : true;
}

void DirPathHistory::forward()
{
    if(aIdx<0 || aIdx+1>=aPaths.length()) return;
    ++aIdx;
    emit currentChanged(aPaths.at(aIdx));
}

void DirPathHistory::backward()
{
    if(aIdx-1<0) return;
    --aIdx;
    emit currentChanged(aPaths.at(aIdx));
}

void DirPathHistory::append(const QString &path)
{
    if(aIdx<0){
        ++aIdx;
        aPaths.append(path);
        return;
    }
    if(aPaths.at(aIdx)==path) return;
    if(aIdx+1<aPaths.length() && aPaths.at(aIdx+1)==path){
        ++aIdx;
        return;
    }
    if(aIdx+1<aPaths.length()) aPaths= aPaths.mid(0,aIdx+1);
    if(aPaths.at(aIdx)!=path){
        ++aIdx;
        aPaths.append(path);
    }
}
