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

#include "entityinfo.h"

PropertyInfo::PropertyInfo()
{
    d = new PropertyInfoPrivate(QString(), QString(), QVariant::Invalid, QVariant(), true, QString());
    d->ref.ref();
}

PropertyInfo::PropertyInfo(const PropertyInfo &other)
{
    d = other.d;
    d->ref.ref();
}

PropertyInfo::PropertyInfo(QString rxParse, QString rxEdit, QVariant::Type type, QVariant dv, bool persists, QString convertFmt)
{
    d= new PropertyInfoPrivate(rxParse, rxEdit, type, dv, persists, convertFmt);
    d->ref.ref();
}

PropertyInfo::~PropertyInfo()
{
    if(!d->ref.deref()) delete d;
}

PropertyInfo &PropertyInfo::operator =(const PropertyInfo &other)
{
    qAtomicAssign(d, other.d);
    return *this;
}

bool PropertyInfo::operator ==(const PropertyInfo &other) const
{
    return (d == other.d || *d == *other.d);
}

QString PropertyInfo::rxParse() const
{
    return d->rp;
}

void PropertyInfo::setRxParse(const QString &text)
{
    detach();
    d->rp= text;
}

QString PropertyInfo::rxEdit() const
{
    return d->rv;
}

void PropertyInfo::setRxEdit(const QString &text)
{
    detach();
    d->rv= text;
}

QString PropertyInfo::convertFormat() const
{
    return d->fmt;
}

void PropertyInfo::setConvertFormat(const QString &text)
{
    detach();
    d->fmt= text;
}

QVariant::Type PropertyInfo::dataType() const
{
    return d->typ;
}

void PropertyInfo::setDefaultValue(const QVariant value)
{
    detach();
    d->dv= value;
}

QVariant PropertyInfo::defaultValue() const
{
    return d->dv;
}

void PropertyInfo::setDataType(const QVariant::Type dataType)
{
    detach();
    d->typ= dataType;
}

bool PropertyInfo::persist()
{
    return d->ps;
}

void PropertyInfo::setPersist(bool persist)
{
    detach();
    d->ps= persist;
}

void PropertyInfo::detach()
{
    qAtomicDetach(d);
    if(d->ref.load()==0) d->ref.ref();
}


EntityInfo::EntityInfo()
{
}

EntityInfo::EntityInfo(const EntityInfo &other)
{
    aName2property= other.aName2property;
    aInfo= other.aInfo;

}

EntityInfo::~EntityInfo()
{
    aName2property= QHash<QString,int>();
    aInfo= QHash<int,PropertyInfo>();
}

bool EntityInfo::isEmpty() const
{
    return aName2property.isEmpty();
}

int EntityInfo::propertyIdx(QString propertyName) const
{
    return aName2property.value(propertyName,-1);
}

QList<int> EntityInfo::propertiesIdx() const
{
    return aName2property.values();
}

QString EntityInfo::propertyName(int property) const
{
    return aName2property.key(property,"");
}

bool EntityInfo::syntaxCheck(QString propertyName, QString value) const
{
    int p= propertyIdx(propertyName);
    if(p<0)return false;
    if(!QRegExp(aInfo.value(p).rxParse()).exactMatch(value)) return false;
    return true;
}

bool EntityInfo::syntaxCheck(int property, QString value) const
{
    if(!aInfo.keys().contains(property))return false;
    if(!QRegExp(aInfo.value(property).rxParse()).exactMatch(value)) return false;
    return true;
}

PropertyInfo EntityInfo::propertyInfo(int propertyIdx) const
{
    return aInfo.value(propertyIdx, PropertyInfo());
}

QString EntityInfo::toString(EntityInstance ent) const
{
    Q_UNUSED(ent)

    return QString();
}

