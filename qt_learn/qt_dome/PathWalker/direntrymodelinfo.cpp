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

#include <QColor>
#include <QBrush>
#include <QDateTime>
#include <QLocale>

#include <QtDebug>

#include "direntrymodelinfo.h"
#include "direntry.h"

DirEntryModelInfo::DirEntryModelInfo(const EntityInfo &info)
    :EntityListModelInfo(info)
{
    aColProp.append(DirEntryProperty::Mark);
    aColProp.append(DirEntryProperty::Name);
    aColProp.append(DirEntryProperty::Size);
    aColProp.append(DirEntryProperty::LastModified);
    aColProp.append(DirEntryProperty::isDir);

    aColFlag<<ColumnFlags(Qt::NoItemFlags,Qt::ItemIsEnabled|Qt::ItemIsUserCheckable)
           <<ColumnFlags(Qt::NoItemFlags,Qt::ItemIsEnabled|Qt::ItemIsEditable)
          <<ColumnFlags(Qt::ItemIsEditable,Qt::ItemIsEnabled)
         <<ColumnFlags(Qt::ItemIsEditable,Qt::ItemIsEnabled)
        <<ColumnFlags(Qt::ItemIsEditable,Qt::ItemIsEnabled);

    aColRole.append(RoleSet()<<Qt::BackgroundRole<<Qt::ToolTipRole<<Qt::CheckStateRole);
    aColRole.append(RoleSet()<<Qt::BackgroundRole<<Qt::ToolTipRole<<Qt::DisplayRole<<Qt::EditRole<<Qt::DecorationRole);
    aColRole.append(RoleSet()<<Qt::BackgroundRole<<Qt::ToolTipRole<<Qt::DisplayRole<<Qt::EditRole);
    aColRole.append(RoleSet()<<Qt::BackgroundRole<<Qt::ToolTipRole<<Qt::DisplayRole<<Qt::EditRole);
    aColRole.append(RoleSet()<<Qt::BackgroundRole<<Qt::ToolTipRole<<Qt::DisplayRole<<Qt::EditRole<<Qt::CheckStateRole);

    if(aRolesMap.isEmpty()){
        RolesMap roles;
        roles.insert((int)Qt::DecorationRole, (int)DirEntryProperty::icon);
        aRolesMap.insert((int)DirEntryProperty::Name, roles);
    }
}

QVariant DirEntryModelInfo::data(const EntityInstance &e, int column, int role) const
{
    if(!aColRole[column].contains(role)) return QVariant();

    if(role==Qt::ToolTipRole){
        if(aColProp[column]==EntityProperty::Modify){
            if(!e.isSet(aColProp[column])) return QVariant();
            return e.get(aColProp[column]).typeName()
                    +QString::number((int)e.get(aColProp[column]).value<int>());
        }
        return e.get(aColProp[column]).toString().prepend("<p>").append("</p>");
    }
    if(role==Qt::EditRole){
        return e.get(aColProp[column]);
    }
    if(role==Qt::DisplayRole){
        if(aColProp[column]==DirEntryProperty::isDir) return QVariant();
        if(aColProp[column]==DirEntryProperty::LastModified) return e.get(DirEntryProperty::LastModified).value<QDateTime>().toString("dd.MM.yyyy hh:mm:ss");
        if(aColProp[column]==DirEntryProperty::Size){
            if(e.get(DirEntryProperty::isDir).value<bool>()) return QVariant();
            qint64 sz= e.get(DirEntryProperty::Size).value<qint64>();
            QString ret= QString::number(sz);
            QString sfx= " b";
            if(sz>=1000000000000){
                ret= ret.left(ret.length()-10);
                sfx= " Tb";
            }else if(sz>=1000000000){
                ret= ret.left(ret.length()-7);
                sfx= " Gb";
            }else if(sz>=1000000){
                ret= ret.left(ret.length()-4);
                sfx= " Mb";
            }else if(sz>=1000){
                ret= ret.left(ret.length()-1);
                sfx= " Kb";
            }
            return (sfx!=" b"? ret.insert(ret.length()-2,QLocale::system().decimalPoint()):ret)+sfx;
        }
        return e.get(aColProp[column]).toString();
    }

    if(role==Qt::CheckStateRole){
        if(aColRole[column].contains(Qt::CheckStateRole)){
            if(aColProp[column]==DirEntryProperty::isDir){
                return e.get(aColProp[column]).value<bool>()?Qt::Checked:Qt::Unchecked;
            }
            return (e.isSet(aColProp[column])
                    ?e.get(aColProp[column]):Qt::Unchecked);
        }
    }
    if(role==Qt::BackgroundRole){
        if(e.get(DirEntryProperty::Mark).value<Qt::CheckState>()==Qt::Checked) return QBrush(QColor(0xff,0xff,0x00,0x4f));
    }

    if(role==Qt::DecorationRole){
        if(aColProp[column]==EntityProperty::Modify){
            if(!e.isSet(aColProp[column])) return QVariant();
            int v= e.get(aColProp[column]).value<int>();
            if(v==EntityModify::Saved) return QVariant();
            if(v==EntityModify::New) return QPixmap(":/img/new.png");
            if(v==EntityModify::Edited) return QPixmap(":/img/pencil-small.png");
        }
    }
    return value(e, aColProp[column], role);
}

QVariant DirEntryModelInfo::headerData(int section, Qt::Orientation orientation, int role) const
{
    if(orientation==Qt::Horizontal){
        if(role==Qt::DecorationRole){
            if(aColProp[section]==EntityProperty::Mark) return QPixmap(":/img/ui-check-boxes.png");
            if(aColProp[section]==EntityProperty::Modify) return QPixmap(":/img/database-small.png");
        }else if(role==Qt::ToolTipRole){
            if(aColProp[section]==EntityProperty::Mark) return QString(QObject().tr("mark"));
            return aEntInfo.propertyName(aColProp[section]);
        }else if(role==Qt::DisplayRole || role==Qt::EditRole){
            return aEntInfo.propertyName(aColProp[section]);
        }
    }
    return QVariant();
}
