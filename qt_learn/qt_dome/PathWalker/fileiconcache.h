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

#ifndef FILEICONCACHE_H
#define FILEICONCACHE_H

#include <QHash>
#include <QIcon>
#include <QMutex>

typedef QHash<QString,QIcon> IconCache;

class FileIconCache
{
public:
    static QIcon icon(const QString& sfx); // get icon for file suffix 'sfx'
    static bool contains(const QString& sfx);
    static void iconAdd(const QString& sfx, const QIcon ico);

private:
    FileIconCache(){} // Private so that it can  not be called
    FileIconCache(FileIconCache const&){}   // copy constructor is private
    FileIconCache& operator=(FileIconCache const&){return *this;}  // assignment operator is private
    static IconCache aCache;
    static QMutex* mtx;
};

#endif // FILEICONCACHE_H
