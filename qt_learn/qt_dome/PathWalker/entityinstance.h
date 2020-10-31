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

#ifndef ENTITYINSTANCE_H
#define ENTITYINSTANCE_H

#include <QtGlobal>
#include <QMetaType>
#include <QList>
#include <QPair>
#include <QHash>
#include <QVariant>
#include <QAtomicInt>

#include "def.h"

class QStringList;
class EntityInfo;

Q_DECLARE_METATYPE(Qt::CheckState)

typedef QPair<int,def::SortOrder> PropertyPrecedeOrder;
typedef QList<PropertyPrecedeOrder> PropertyPrecedeOrderList;
typedef QHash<int,QVariant> VarRecord;
typedef QList<int> PropertyList;
typedef QHash<int,QVariantList> PropertyValueFilter;
Q_DECLARE_METATYPE(PropertyPrecedeOrder)
Q_DECLARE_METATYPE(PropertyPrecedeOrderList)
Q_DECLARE_METATYPE(VarRecord)
Q_DECLARE_METATYPE(PropertyList)
Q_DECLARE_METATYPE(PropertyValueFilter)

namespace EntityProperty{
enum Properties{
    Invalid=-1
    ,Mark= 0
    ,Modify= 1
    ,Syntax= 2
    ,Semantic= 3
    ,Dict= 4
    ,AutoId= 5
    ,Id= 6
};
}
Q_DECLARE_METATYPE(EntityProperty::Properties)

namespace EntityModify{
enum State{
    Invalid= -1
    ,Saved= 0
    ,New= 1
    ,Edited= 2
};
}
Q_DECLARE_METATYPE(EntityModify::State)

class EntityInstancePrivate
{
public:
    EntityInstancePrivate()
    {}

    EntityInstancePrivate(const VarRecord & record)
        : rcd(record)
    {}

    EntityInstancePrivate(const EntityInstancePrivate & other)
        : rcd(other.rcd)
    {}

    bool operator==(const EntityInstancePrivate & other) const
    {
        return (rcd == other.rcd);
    }

    QAtomicInt ref;
    VarRecord rcd;
};
Q_DECLARE_METATYPE(EntityInstancePrivate)

class EntityInstance
{
public:
    EntityInstance();
    EntityInstance(const EntityInstance & other);
    EntityInstance& operator=(const EntityInstance & other);
    bool operator==(const EntityInstance & other) const;
    inline bool operator!=(const EntityInstance & other) const { return !operator==(other); }
    ~EntityInstance();
    EntityInstance set(int property, QVariant value);
    QVariant get(int property) const;
    EntityInstance rem(int property);

    bool isSet(int property) const;
    int propertiesCount() const;
    QList<int> properties() const;

private:
    void detach();
    EntityInstancePrivate *d;
};
Q_DECLARE_METATYPE(EntityInstance)

typedef QList<EntityInstance> EntityInstanceList;
Q_DECLARE_METATYPE(EntityInstanceList)


QDebug operator<<(QDebug dbg, const EntityInstance &instance);

class EntityInstanceCompare
{
public:
    static bool precede(const EntityInstance & e1, const EntityInstance & e2, const PropertyPrecedeOrderList & s, const EntityInfo &inf);
    static QList<EntityInstance> sort(QList<EntityInstance> l, const PropertyPrecedeOrderList & s, const EntityInfo &inf);

private:
    EntityInstanceCompare(){}
    EntityInstanceCompare(EntityInstanceCompare const&){}   // copy constructor is private
    EntityInstanceCompare& operator=(EntityInstanceCompare const&){return *this;}   // assignment operator is private
};

bool lessThan(const QVariant &v1, const QVariant &v2);
bool greaterThan(const QVariant &v1, const QVariant &v2);

#endif // ENTITYINSTANCE_H
