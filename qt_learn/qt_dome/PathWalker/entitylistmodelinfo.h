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

#ifndef VIEWMODELINFO_H
#define VIEWMODELINFO_H

#include <Qt>
#include <QVariant>
#include <QList>
#include <QHash>
#include <QSet>

#include "entityinfo.h"

typedef QSet<int> RoleSet;
typedef QHash<int,int> RolesMap; //Qt::ItemDataRole -> EntityProperty::Properties
struct ColumnFlags{
    ColumnFlags(Qt::ItemFlags off, Qt::ItemFlags on)
        :flagsOff(off),flagsOn(on){}
    Qt::ItemFlags flagsOff;
    Qt::ItemFlags flagsOn;
};

class EntityListModelInfo
{
public:
    EntityListModelInfo(const EntityInfo &info);
    EntityListModelInfo(const EntityListModelInfo &other);
    EntityListModelInfo& operator=(const EntityListModelInfo &other);
    virtual ~EntityListModelInfo();
    virtual QVariant data(const EntityInstance &e, int column, int role = Qt::DisplayRole) const=0;
    virtual Qt::ItemFlags flags(int column, Qt::ItemFlags f) const;
    virtual QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const=0;
    int columnForProperty(int p) const;
    int propertyForColumn(int c) const;
    QVariant value(const EntityInstance& e, int p, int role) const;

    EntityInfo aEntInfo;
    PropertyList aColProp;
    QList<ColumnFlags> aColFlag; // QPair<clearFlags, setFlags>
    QList<RoleSet> aColRole;
    QHash<int, RolesMap> aRolesMap; // EntityProperty::Properties -> RolesMap
};

#endif // VIEWMODELINFO_H
