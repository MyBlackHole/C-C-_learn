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

#ifndef VISUALTAGINFO_H
#define VISUALTAGINFO_H

#include <QPointer>
#include <QAtomicInt>
#include <QWidget>

class VisualTagInfoPrivate
{
public:
    VisualTagInfoPrivate()
    {}

    VisualTagInfoPrivate(const VisualTagInfoPrivate & other)
        : aKey(other.aKey),aTag(other.aTag),aTarget(other.aTarget)
    {}

    VisualTagInfoPrivate(Qt::Key key, QWidget* tag, QWidget* target)
        :aKey(key),aTag(tag),aTarget(target)
    {
    }

    bool operator==(const VisualTagInfoPrivate & other) const
    {
        return (aKey == other.aKey && aTag == other.aTag && aTarget == other.aTarget);
    }

    QAtomicInt ref;
    Qt::Key aKey;
    QPointer<QWidget> aTag;
    QPointer<QWidget> aTarget;
};

class VisualTagInfo
{
public:
    VisualTagInfo();
    VisualTagInfo(const VisualTagInfo & other);
    VisualTagInfo(Qt::Key key, QWidget *tag, QWidget *target);
    virtual ~VisualTagInfo();
    VisualTagInfo& operator=(const VisualTagInfo & other);
    bool operator==(const VisualTagInfo & other) const;
    inline bool operator!=(const VisualTagInfo & other) const { return !operator==(other); }

    Qt::Key key();
    QWidget* tag();
    QWidget* target();

private:
    void detach();
    VisualTagInfoPrivate *d;
};

#endif // VISUALTAGINFO_H
