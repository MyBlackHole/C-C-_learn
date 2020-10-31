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

#include <QRegExp>
#include <QDebug>

#include "entitycompare.h"

EntityCompareCondition::EntityCompareCondition()
{
    d = new EntityCompareConditionPrivate(EntityProperty::Invalid, EntityComparePredicate::Invalid, QVariantList());
    d->ref.ref();
}

EntityCompareCondition::EntityCompareCondition(const EntityCompareCondition &other)
{
    d = other.d;
    d->ref.ref();
}

EntityCompareCondition::EntityCompareCondition(int property, EntityComparePredicate::Predicates predicate, QVariantList value)
{
    d= new EntityCompareConditionPrivate(property, predicate, value);
    d->ref.ref();
}

EntityCompareCondition::~EntityCompareCondition()
{
    if(!d->ref.deref()) delete d;
}


EntityCompareCondition &EntityCompareCondition::operator =(const EntityCompareCondition &other)
{
    qAtomicAssign(d, other.d);
    return *this;
}

bool EntityCompareCondition::operator ==(const EntityCompareCondition &other) const
{
    return (d == other.d || *d == *other.d);
}

int EntityCompareCondition::property() const
{
    return d->pro;
}

QVariantList EntityCompareCondition::values() const
{
    return d->val;
}

EntityCompareCondition EntityCompareCondition::setValues(QVariantList values)
{
    detach();
    d->val= values;
    return *this;
}

bool EntityCompareCondition::isValid() const
{
    return (d->pro!=EntityProperty::Invalid && d->pre!=EntityComparePredicate::Invalid);// && d->val.canConvert());
}

// check if 'EntityInstance' meet the condition
bool EntityCompareCondition::check(const EntityInstance &e , const EntityInfo &inf) const
{
    Q_UNUSED(inf);
    foreach(QVariant v, d->val){
        switch(d->pre){
        case EntityComparePredicate::Invalid:
            break;
        case EntityComparePredicate::Set:
            if(e.isSet(d->pro)) return true;
            break;
        case EntityComparePredicate::NotSet:
            if(!e.isSet(d->pro)) return true;
            break;
        case EntityComparePredicate::Equal:
            if(e.get(d->pro)==v) return true;
            break;
        case EntityComparePredicate::NotEqual:
            if(e.get(d->pro)!=v) return true;
            break;
        case EntityComparePredicate::Contains:
            if(e.get(d->pro).toString().contains(v.toString())) return true;
            break;
        case EntityComparePredicate::NotContains:
            if(!e.get(d->pro).toString().contains(v.toString())) return true;
            break;
        case EntityComparePredicate::Greater:
            if((e.get(d->pro))>v) return true;
            break;
        case EntityComparePredicate::NotGreater:
            if(e.get(d->pro)==v) return true;
            if(e.get(d->pro)<v) return true;
            break;
        case EntityComparePredicate::Less:
            if(e.get(d->pro)<v) return true;
            break;
        case EntityComparePredicate::NotLess:
            if(e.get(d->pro)==v) return true;
            if(e.get(d->pro)>v) return true;
            break;
        case EntityComparePredicate::StartsWith:
            if(e.get(d->pro).toString().startsWith(v.toString())) return true;
            break;
        case EntityComparePredicate::EndsWith:
            if(e.get(d->pro).toString().endsWith(v.toString())) return true;
            break;
        case EntityComparePredicate::NotStartsWith:
            if(!e.get(d->pro).toString().startsWith(v.toString())) return true;
            break;
        case EntityComparePredicate::NotEndsWith:
            if(!e.get(d->pro).toString().endsWith(v.toString())) return true;
            break;
        case EntityComparePredicate::RegExp:
            QRegExp rx(v.toString());
            rx.setPatternSyntax(QRegExp::Wildcard);
            rx.setCaseSensitivity(Qt::CaseInsensitive);
            if(rx.exactMatch(e.get(d->pro).toString())) return true;
            break;
        }
    }
    return false;
}

void EntityCompareCondition::detach()
{
    qAtomicDetach(d);
    if(d->ref.load()==0) d->ref.ref();
}

bool EntityCompare::isMatch(const EntityInstance &e, const EntityInfo &inf, const EntityCompareConditions &cs)
{
    foreach(EntityCompareCondition c, cs){
        if(!c.check(e, inf)) return false;
    }
    return true;
}


QDebug operator<<(QDebug dbg, const EntityCompareCondition &d)
{
    dbg.nospace()<<"EntityCompareCondition("<<d.property()<<":"<<d.values()<<")";
    return dbg.space();
}
