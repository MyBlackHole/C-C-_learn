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

#ifndef DIRENTRY_H
#define DIRENTRY_H


#include <QMetaType>

#include "entityinfo.h"
#include "entityinstance.h"

class QString;

namespace DirEntryProperty{
enum Properties{
    Invalid= EntityProperty::Invalid
    ,Mark= EntityProperty::Mark
    ,Modify= EntityProperty::Modify
    ,Syntax= EntityProperty::Syntax
    ,Semantic= EntityProperty::Semantic
    ,Dict= EntityProperty::Dict
    ,AutoId= EntityProperty::AutoId
    ,Id= EntityProperty::Id
    ,Name
    ,Size
    ,LastModified
    ,isDir
    ,icon
    ,isSpecial
};
}
Q_DECLARE_METATYPE(DirEntryProperty::Properties)

class DirEntryInfo
{
    Q_ENUMS(DirEntryProperty::Properties)

public:
    static const EntityInfo& info();

private:
    DirEntryInfo(){}  // Private so that it can  not be called
    DirEntryInfo(DirEntryInfo const&){}             // copy constructor is private
    DirEntryInfo& operator=(DirEntryInfo const&){return *this;}  // assignment operator is private
    static EntityInfo aInfo;
};

#endif // DIRENTRY_H
