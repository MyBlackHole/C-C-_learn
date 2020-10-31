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

#include "task.h"

// Global static instance used to ensure a single instance of the class.
EntityInfo Task::aInfo= EntityInfo();

// This function is called to create an instance of the class.
// Calling the constructor publicly is not allowed. The constructor
// is private and is only called by this Instance function.

const EntityInfo &Task::info()
{
    if (aInfo.isEmpty()){
        aInfo.aName2property.insert(QObject::tr("Type"),TaskProperty::Type);
        aInfo.aName2property.insert(QObject::tr("Source"),TaskProperty::Source);
        aInfo.aName2property.insert(QObject::tr("Destination"),TaskProperty::Destination);

        aInfo.aInfo.insert(TaskProperty::Type, PropertyInfo("\\d{12}", "\\d{12}", QVariant::Int));
        aInfo.aInfo.insert(TaskProperty::Source, PropertyInfo(".*", ".*", QVariant::String));
        aInfo.aInfo.insert(TaskProperty::Destination, PropertyInfo(".*", ".*", QVariant::String));
    }

    return aInfo;
}
