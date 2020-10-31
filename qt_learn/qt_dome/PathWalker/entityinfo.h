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

#ifndef ENTITYINFO_H
#define ENTITYINFO_H

#include <QHash>
#include <QVariant>

#include "entityinstance.h"

class QString;

class PropertyInfoPrivate
{
public:
    PropertyInfoPrivate()
    {}

    PropertyInfoPrivate(const PropertyInfoPrivate & other)
        : rp(other.rp),rv(other.rv),fmt(other.fmt),typ(other.typ),dv(other.dv),ps(other.ps)
    {}

    PropertyInfoPrivate(QString rxParse, QString rxEdit, QVariant::Type type, QVariant defaultValue, bool persists, QString convertFmt)
        :rp(rxParse),rv(rxEdit),fmt(convertFmt),typ(type),dv(defaultValue),ps(persists)
    {}

    bool operator==(const PropertyInfoPrivate & other) const
    {
        return (rp == other.rp && rv == other.rv && fmt == other.fmt && typ == other.typ && dv == other.dv && ps == other.ps);
    }

    QAtomicInt ref;
    QString rp; // regexp for parsed value check
    QString rv; // regexp for edited value check
    QString fmt;    // format for to string convert if applicable
    QVariant::Type typ;
    QVariant dv; // default value
    bool ps; // is property persistant
};
Q_DECLARE_METATYPE(PropertyInfoPrivate)

class PropertyInfo{
public:

    PropertyInfo();
    PropertyInfo(const PropertyInfo & other);
    PropertyInfo(QString rxParse, QString rxEdit, QVariant::Type type= QVariant::String, QVariant dv= QVariant(), bool persists= true, QString convertFmt= QString());
    ~PropertyInfo();
    PropertyInfo& operator=(const PropertyInfo & other);
    bool operator==(const PropertyInfo & other) const;
    inline bool operator!=(const PropertyInfo & other) const { return !operator==(other); }
    QString rxParse() const;
    void setRxParse(const QString & text);
    QString rxEdit() const;
    void setRxEdit(const QString &text);
    QString convertFormat() const;
    void setConvertFormat(const QString & text);
    QVariant::Type dataType() const;
    void setDefaultValue(const QVariant value);
    QVariant defaultValue() const;
    void setDataType(const QVariant::Type dataType);
    bool persist();
    void setPersist(bool persist);

private:
    void detach();
    PropertyInfoPrivate *d;
};
Q_DECLARE_METATYPE(PropertyInfo)

class EntityInfo{
    Q_ENUMS(EntityModify::State)

public:
    EntityInfo();
    EntityInfo(const EntityInfo & other);
    virtual ~EntityInfo();
    bool isEmpty() const;
    int propertyIdx(QString propertyName) const;
    QList<int> propertiesIdx() const;
    QString propertyName(int propertyIdx) const;
    bool syntaxCheck(QString propertyName, QString value) const;
    bool syntaxCheck(int propertyIdx, QString value) const;
    PropertyInfo propertyInfo(int propertyIdx) const;
    virtual QString toString(EntityInstance ent) const;

    //protected:
    QHash<QString,int> aName2property;
    QHash<int,PropertyInfo> aInfo;
};
Q_DECLARE_METATYPE(EntityInfo)

#endif // ENTITYINFO_H
