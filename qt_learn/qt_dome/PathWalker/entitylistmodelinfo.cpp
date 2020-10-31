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

#include <QModelIndex>

#include "entitylistmodelinfo.h"

EntityListModelInfo::EntityListModelInfo(const EntityInfo &info):
    aEntInfo(info)
{
}

EntityListModelInfo::EntityListModelInfo(const EntityListModelInfo &other):
    aEntInfo(other.aEntInfo)
  ,aColProp(other.aColProp)
  ,aColFlag(other.aColFlag)
  ,aColRole(other.aColRole)
  ,aRolesMap(other.aRolesMap)
{
}

EntityListModelInfo &EntityListModelInfo::operator=(const EntityListModelInfo &other)
{
    aEntInfo= other.aEntInfo;
    aColProp=other.aColProp;
    aColFlag=other.aColFlag;
    aRolesMap=other.aRolesMap;
    aColRole=other.aColRole;
    return *this;
}

EntityListModelInfo::~EntityListModelInfo()
{
    aEntInfo= EntityInfo();
    aColProp= PropertyList();
    aColFlag= QList<ColumnFlags>();
    aColRole= QList<RoleSet>();
    aRolesMap= QHash<int, RolesMap>();
}

Qt::ItemFlags EntityListModelInfo::flags(int column, Qt::ItemFlags f) const
{
    return (f&~aColFlag.at(column).flagsOff)|aColFlag.at(column).flagsOn;
}

int EntityListModelInfo::columnForProperty(int p) const
{
    return aColProp.contains(p)? aColProp.indexOf(p): -1;
}

int EntityListModelInfo::propertyForColumn(int c) const
{
    return aColProp.count()<=c? -1: aColProp.at(c);
}

QVariant EntityListModelInfo::value(const EntityInstance &e, int p, int role) const
{
    if(!aRolesMap.contains(p) || !aRolesMap.value(p).contains(role)) return e.get(p);
    return e.get(aRolesMap.value(p).value(role));
}
