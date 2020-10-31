/*
 * This file is part of Visual Keyboard Navi software
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

#include "visualtaginfo.h"

VisualTagInfo::VisualTagInfo()
{
    d = new VisualTagInfoPrivate(Qt::Key_unknown,0,0);
    d->ref.ref();
}

VisualTagInfo::~VisualTagInfo()
{
    if(!d->ref.deref()) delete d;
}

VisualTagInfo &VisualTagInfo::operator=(const VisualTagInfo &other)
{
    qAtomicAssign(d, other.d);
    return *this;
}

bool VisualTagInfo::operator==(const VisualTagInfo &other) const
{
    return (d == other.d || *d == *other.d);
}

Qt::Key VisualTagInfo::key()
{
    return d->aKey;
}

QWidget *VisualTagInfo::tag()
{
    return d->aTag;
}

QWidget *VisualTagInfo::target()
{
    return d->aTarget;
}

VisualTagInfo::VisualTagInfo(const VisualTagInfo &other)
{
    d = other.d;
    d->ref.ref();
}

VisualTagInfo::VisualTagInfo(Qt::Key key, QWidget *tag, QWidget *target)
{
    d= new VisualTagInfoPrivate(key, tag, target);
    d->ref.ref();
}
