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

#include "entityinstance.h"
#include "entityinfo.h"

EntityInstance::EntityInstance()
{
    d = new EntityInstancePrivate();
    d->ref.ref();
}

EntityInstance::EntityInstance(const EntityInstance &other)
{
    d = other.d;
    d->ref.ref();
}

EntityInstance &EntityInstance::operator =(const EntityInstance &other)
{
    qAtomicAssign(d, other.d);
    return *this;
}


bool EntityInstance::operator ==(const EntityInstance &other) const
{
    return (d == other.d || *d == *other.d);
}

EntityInstance::~EntityInstance()
{
    if(!d->ref.deref()) delete d;
}

EntityInstance EntityInstance::set(int property, QVariant value)
{
    if(d->rcd.contains(property) && d->rcd.value(property)==value) return *this;
    detach();
    d->rcd.insert(property,value);
    return *this;
}

QVariant EntityInstance::get(int property) const
{
    return d->rcd.value(property);
}

EntityInstance EntityInstance::rem(int property)
{
    if(!d->rcd.contains(property)) return *this;
    detach();
    d->rcd.remove(property);
    return *this;
}

bool EntityInstance::isSet(int property) const
{
    return d->rcd.contains(property);
}

int EntityInstance::propertiesCount() const
{
    return d->rcd.keys().count();
}

QList<int> EntityInstance::properties() const
{
    return d->rcd.keys();
}

void EntityInstance::detach()
{
    qAtomicDetach(d);
    if(d->ref.load()==0) d->ref.ref();
}

QDebug operator<<(QDebug dbg, const  EntityInstance &instance)
{
    dbg.nospace()<<"\r\n{";
    foreach(int p, instance.properties()){
        dbg.nospace()<<p<<":"<<instance.get(p)<<";\r\n";
    }
    dbg.nospace()<<"}";
    return dbg.maybeSpace();
}

bool EntityInstanceCompare::precede(const EntityInstance &e1, const EntityInstance &e2, const PropertyPrecedeOrderList &s, const EntityInfo &inf)
{
    QVariant a1,a2;
    int t1,t2;
    foreach(PropertyPrecedeOrder so, s){
        a1= e1.get(so.first);
        a2= e2.get(so.first);
        t1= a1.userType();
        if(t1==QMetaType::UnknownType && inf.propertyInfo(so.first).dataType()!=QMetaType::UnknownType){
            t1= inf.propertyInfo(so.first).dataType();
            a1= inf.propertyInfo(so.first).defaultValue();
        }
        t2= a2.userType();
        if(t2==QMetaType::UnknownType && inf.propertyInfo(so.first).dataType()!=QMetaType::UnknownType){
            t2= inf.propertyInfo(so.first).dataType();
            a2= inf.propertyInfo(so.first).defaultValue();
        }
        if(t1== QMetaType::UnknownType && t2== QMetaType::UnknownType) continue;
        if(so.second == def::Ascending){
            if(t1== QMetaType::UnknownType) return true;
            if(t2== QMetaType::UnknownType) return false;
            if(lessThan(a1,a2)) return true;
            else if(greaterThan(a1,a2)) return false;
        }else if(so.second == def::Descending){
            if(t1== QMetaType::UnknownType) return false;
            if(t2== QMetaType::UnknownType) return true;
            if(greaterThan(a1,a2)) return true;
            else if(lessThan(a1,a2)) return false;
        }
    }
    return false;
}

QList<EntityInstance> EntityInstanceCompare::sort(QList<EntityInstance> l, const PropertyPrecedeOrderList &s, const EntityInfo &inf)
{
    for(int i=0; i<l.count(); i++){
        for(int j=i+1; j<l.count(); j++){
            if(EntityInstanceCompare::precede(l.at(j), l.at(i),s,inf)) l.insert(i,l.takeAt(j));
        }
    }
    return l;
}

bool lessThan(const QVariant &v1, const QVariant &v2)
{
    int t1= v1.userType();
    if(t1!=v2.userType()) return false;

    bool ok=false;
    if(t1==QVariant::Date)
        return( v1.toDate()<v2.toDate() );
    else if(t1==QVariant::DateTime)
        return( v1.toDateTime()<v2.toDateTime() );
    else if(t1==QVariant::Int){
        int vInt1= v1.toInt(&ok);
        if(!ok) return false;
        int vInt2= v2.toInt(&ok);
        if(!ok) return false;
        return vInt1<vInt2;
    }else if(t1==QVariant::UInt){
        uint vUInt1= v1.toUInt(&ok);
        if(!ok) return false;
        uint vUInt2= v2.toUInt(&ok);
        if(!ok) return false;
        return vUInt1<vUInt2;
    }else if(t1==QVariant::Bool){
        bool vBool1= v1.toBool();
        bool vBool2= v2.toBool();
        return vBool1==vBool2?false:(vBool1?true:false);
    }else if(t1==QVariant::LongLong){
        qint64 vInt64a= v1.toLongLong(&ok);
        if(!ok) return false;
        qint64 vInt64b= v2.toLongLong(&ok);
        if(!ok) return false;
        return vInt64a<vInt64b;
    }else if(t1==QVariant::ULongLong){
        quint64 vUInt64a= v1.toULongLong(&ok);
        if(!ok) return false;
        quint64 vUInt64b= v2.toULongLong(&ok);
        if(!ok) return false;
        return vUInt64a<vUInt64b;
    }else if(t1==QVariant::String){
        return v1.toString()<v2.toString();
    }
    return false;
}

bool greaterThan(const QVariant &v1, const QVariant &v2)
{
    if(v1.userType()!=v2.userType()) return false;
    return v1==v2?false:!lessThan(v1,v2);
}
