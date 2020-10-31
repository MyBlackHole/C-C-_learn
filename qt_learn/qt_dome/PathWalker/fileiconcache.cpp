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

#include <QtCore>

#include "fileiconcache.h"

// Global static pointer used to ensure a single instance of the class.
IconCache FileIconCache::aCache= IconCache();
QMutex* FileIconCache::mtx= new QMutex();

QIcon FileIconCache::icon(const QString &sfx)
{
    QMutexLocker lock(mtx);
    return aCache.value(sfx, QIcon());
}

bool FileIconCache::contains(const QString &sfx)
{
    QMutexLocker lock(mtx);
    return aCache.contains(sfx);
}

void FileIconCache::iconAdd(const QString &sfx, const QIcon ico)
{
    QMutexLocker lock(mtx);
    aCache.insert(sfx, ico);
}
