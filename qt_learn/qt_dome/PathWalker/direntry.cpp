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

#include "direntry.h"

// Global static instance used to ensure a single instance of the class.
EntityInfo DirEntryInfo::aInfo= EntityInfo();

// This function is called to create an instance of the class.
// Calling the constructor publicly is not allowed. The constructor
// is private and is only called by this Instance function.
const EntityInfo &DirEntryInfo::info()
{
    // Only allow one instance of class to be generated.
    if (aInfo.isEmpty()){
        aInfo.aName2property.insert(QObject::tr("Name"),DirEntryProperty::Name);
        aInfo.aName2property.insert(QObject::tr("Size"),DirEntryProperty::Size);
        aInfo.aName2property.insert(QObject::tr("Modified"),DirEntryProperty::LastModified);
        aInfo.aName2property.insert(QObject::tr("dir"),DirEntryProperty::isDir);
        aInfo.aName2property.insert(QObject::tr("icon"),DirEntryProperty::icon);

        aInfo.aInfo.insert(DirEntryProperty::Name, PropertyInfo(".*", ".*", QVariant::String));
        aInfo.aInfo.insert(DirEntryProperty::Size, PropertyInfo("\\d{12}", "\\d{12}", QVariant::Int));
        aInfo.aInfo.insert(DirEntryProperty::LastModified, PropertyInfo("\\d{2}\\.\\d{2}\\.\\d{4} \\d{2}:\\d{2}:\\d{2}"
                                                                        , "\\d{2}\\.\\d{2}\\.\\d{4} \\d{2}:\\d{2}:\\d{2}"
                                                                        , QVariant::DateTime,true,"dd.MM.yyyy hh:mm:ss"));
        aInfo.aInfo.insert(DirEntryProperty::isDir, PropertyInfo("(0|1)", "(0|1)", QVariant::Bool));
        aInfo.aInfo.insert(DirEntryProperty::icon, PropertyInfo(".*", ".*", QVariant::Icon));
        aInfo.aInfo.insert(DirEntryProperty::Mark, PropertyInfo("\\d", "\\d", QVariant::Int, QVariant::fromValue(0)));
        aInfo.aInfo.insert(DirEntryProperty::isSpecial, PropertyInfo("[01]", "[01]", QVariant::Bool, QVariant::fromValue(false)));
    }

    return aInfo;
}
