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

#ifndef ENTITYCOMPARE_H
#define ENTITYCOMPARE_H

#include <QMetaType>

#include "entityinstance.h"
#include "entityinfo.h"

namespace EntityComparePredicate{
enum Predicates{
    Invalid=-1
    ,Set=0
    ,NotSet
    ,Equal
    ,NotEqual
    ,Contains
    ,NotContains
    ,Greater
    ,Less
    ,NotGreater
    ,NotLess
    ,StartsWith
    ,EndsWith
    ,NotStartsWith
    ,NotEndsWith
    ,RegExp
};
}
Q_DECLARE_METATYPE(EntityComparePredicate::Predicates)

class EntityCompareConditionPrivate
{
public:
    EntityCompareConditionPrivate()
        : pro(-1), pre(EntityComparePredicate::Invalid)
    {}

    EntityCompareConditionPrivate(const EntityCompareConditionPrivate & other)
        : pro(other.pro),pre(other.pre),val(other.val)
    {}

    EntityCompareConditionPrivate(int property,EntityComparePredicate::Predicates predicate, QVariantList value)
        :pro(property),pre(predicate),val(value)
    {}

    bool operator==(const EntityCompareConditionPrivate & other) const
    {
        return (pro == other.pro && pre == other.pre && val == other.val);
    }

    QAtomicInt ref;
    int pro;
    EntityComparePredicate::Predicates pre;
    QVariantList val;
};
Q_DECLARE_METATYPE(EntityCompareConditionPrivate)

class EntityCompareCondition
{
public:
    EntityCompareCondition();
    EntityCompareCondition(const EntityCompareCondition &other);
    EntityCompareCondition(int property, EntityComparePredicate::Predicates predicate, QVariantList values);
    virtual ~EntityCompareCondition();
    EntityCompareCondition& operator=(const EntityCompareCondition &other);
    bool operator==(const EntityCompareCondition &other) const;
    inline bool operator!=(const EntityCompareCondition &other) const { return !operator==(other); }

    virtual int property() const;
    virtual QVariantList values() const;
    virtual EntityCompareCondition setValues(QVariantList values);
    virtual bool isValid() const;
    virtual bool check(const EntityInstance &e, const EntityInfo &inf) const;

private:
    void detach();
    EntityCompareConditionPrivate *d;
};
Q_DECLARE_METATYPE(EntityCompareCondition)

QDebug operator<<(QDebug dbg, const EntityCompareCondition& d);

typedef QList<EntityCompareCondition> EntityCompareConditions;
Q_DECLARE_METATYPE(EntityCompareConditions)

class EntityCompare
{
public:
    static bool isMatch(const EntityInstance &e, const EntityInfo &inf, const EntityCompareConditions &cs);

private:
    EntityCompare(){}
    EntityCompare(EntityCompare const&){}             // copy constructor is private
    EntityCompare& operator=(EntityCompare const&){return *this;}  // assignment operator is private
};

#endif // ENTITYCOMPARE_H
