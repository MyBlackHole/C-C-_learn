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

#include "entitylistmodel.h"
#include "asyncentitylistarranger.h"
#include "asyncentitylistfilter.h"

EntityListModel::EntityListModel(EntityListModelInfo *mInfo, QObject *parent) :
    QAbstractTableModel(parent),aIdGen(0), aInfo(mInfo)
{
    aSortTimer= new QTimer();
    aSortTimer->setInterval(5); // delay before applying sorting
    connect(aSortTimer,SIGNAL(timeout()),this,SLOT(onSortTimer()));

    aUpdateTimer= new QTimer();
    aUpdateTimer->setInterval(50); // delay before applying changes
    connect(aUpdateTimer,SIGNAL(timeout()),this,SLOT(onUpdateTimer()));

    aArranger= new AsyncEntityListArranger;
    connect(aArranger, &AsyncEntityListArranger::done, this, &EntityListModel::onAsyncSortFinished);

    aFilterer= new AsyncEntityListFilter;
    connect(aFilterer, &AsyncEntityListFilter::done, this, &EntityListModel::onAsyncFilterFinished);
}

int EntityListModel::columnCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent)
    return aInfo->aColProp.count();
}

int EntityListModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent)
    return aRows.count()+aRowsFixedTop.count()+aRowsFixedBottom.count();
}

QVariant EntityListModel::data(const QModelIndex &index, int role) const
{
    if(index.row()<aRowsFixedTop.count() && aInfo->aColProp[index.column()]==EntityProperty::Mark) return QVariant();
    return aInfo->data(rowsListForIdx(index).at(rowInRows(index.row())),index.column(),role);
}

Qt::ItemFlags EntityListModel::flags(const QModelIndex &index) const
{
    if(index.row()<aRowsFixedTop.count() || index.column()>=aInfo->aColFlag.count()) return QAbstractTableModel::flags(index);
    return aInfo->flags(index.column(),QAbstractTableModel::flags(index));
}

bool EntityListModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    if(role == Qt::EditRole){
        if(aInfo->aColProp[index.column()] == EntityProperty::AutoId){
            rowsListForIdx(index)[rowInRows(index.row())].set(EntityProperty::AutoId,value);
            emit dataChanged(index,index);
            emit modifiedByUser();
            return true;
        }
        if(aInfo->aColProp[index.column()]>EntityProperty::Id){
            if(rowsListForIdx(index).at(rowInRows(index.row())).get(aInfo->aColProp[index.column()])==value) return false;
            int v= rowsListForIdx(index).at(rowInRows(index.row())).get(EntityProperty::Modify).value<int>();
            rowsListForIdx(index)[rowInRows(index.row())].set(aInfo->aColProp[index.column()],value);
            if(v!=EntityModify::New) rowsListForIdx(index)[rowInRows(index.row())].set(EntityProperty::Modify,(int)EntityModify::Edited);
            emit dataChanged(this->index(index.row(),index.column())
                             ,this->index(index.row(),EntityProperty::Modify));
            emit modifiedByUser();
            return true;
        }
    }
    if(role == Qt::CheckStateRole){
        if(aInfo->aColProp[index.column()]==EntityProperty::Mark){
            Qt::CheckState m= (Qt::CheckState)value.toInt();
            this->setProperty(index.row(), aInfo->aColProp[index.column()]
                    ,QVariant::fromValue(m));
        }
        emit modifiedByUser();
        return true;
    }
    return false;
}

bool EntityListModel::setProperty(int row, int property, const QVariant &value, int role)
{
    Q_UNUSED(role)
    rowsListForRow(row)[rowInRows(row)].set(property,value);
    emit dataChanged(index(row,0),index(row,aInfo->aColProp.count()-1));
    return true;
}

QVariant EntityListModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if(orientation==Qt::Horizontal){
        if(role==Qt::UserRole+1){ // SortOrder
            for(int i=0;i<aSortOrder.count(); ++i)
                if(aSortOrder.at(i).first==aInfo->aColProp[section]){
                    return aSortOrder.at(i).second;
                }
            return def::Unsorted;
        }else if(role==Qt::UserRole+2){ // SortIndex
            for(int i=0;i<aSortOrder.count(); ++i)
                if(aSortOrder.at(i).first==aInfo->aColProp[section]){
                    return i+1;
                }
        }
    }
    return aInfo->headerData(section, orientation, role);
}

bool EntityListModel::setHeaderData(int section, Qt::Orientation orientation, const QVariant &value, int role)
{
    if(orientation==Qt::Horizontal){
        if(role==Qt::UserRole+1){ // SortOrder
            onSort(section,(def::SortOrder)(value.toInt()));
            emit headerDataChanged(Qt::Horizontal,0,columnCount()-1);
            return true;
        }
    }
    return EntityListModel::setHeaderData(section,orientation,value,role);
}

int EntityListModel::filteredRowCount() const
{
    return aFilteredRows.count();
}

EntityInstance EntityListModel::at(int row) const
{
    return rowsListForRow(row).at(rowInRows(row));
}

EntityInstance EntityListModel::takeAt(int row)
{
    if(row<0 && row>=rowCount()) return EntityInstance();
    beginRemoveRows(QModelIndex(),row,row);
    EntityInstance e= rowsListForRow(row).takeAt(rowInRows(row));
    endRemoveRows();
    return e;
}

PropertyValueFilter EntityListModel::filter() const
{
    return aValueFilter;
}

bool EntityListModel::isMatchFilter(const EntityInstance &e) const
{
    foreach(int p, aValueFilter.keys())
        if(!e.isSet(p) || !aValueFilter.value(p).contains(e.get(p))) return false;
    return true;
}

EntityInstance EntityListModel::getByPropertyFirst(int property, QVariant value) const
{
    for(int i=0; i<aRows.count(); i++){
        if(aRows.at(i).get(property) == value){
            return aRows.at(i);
        }
    }
    return EntityInstance();
}

bool EntityListModel::setSort(int column, def::SortOrder order)
{
    PropertyPrecedeOrder cso= qMakePair(aInfo->aColProp[column],def::Unsorted);
    int csi= aSortOrder.count();
    for(int i=0;i<aSortOrder.count(); ++i)
        if(aSortOrder.at(i).first==aInfo->aColProp[column]){
            cso= aSortOrder.takeAt(i);
            csi= i;
            break;
        }
    if(order!=def::Unsorted){
        aSortOrder.insert(csi>aSortOrder.count()?aSortOrder.count():csi,qMakePair(aInfo->aColProp[column],order));
    }
    return cso.second!=order? true: false;
}

bool EntityListModel::isEntityMarked(int row)
{
    if(!rowsListForRow(row).at(rowInRows(row)).isSet(EntityProperty::Mark)) return false;
    return (rowsListForRow(row).at(rowInRows(row)).get(EntityProperty::Mark).value<Qt::CheckState>()==Qt::Checked);
}

bool EntityListModel::isEntityNew(int row)
{
    if(!rowsListForRow(row).at(rowInRows(row)).isSet(EntityProperty::Modify)) return false;
    return (rowsListForRow(row).at(rowInRows(row)).get(EntityProperty::Modify).value<EntityModify::State>())==EntityModify::New;
}

bool EntityListModel::isEntityModified(int row)
{
    if(!rowsListForRow(row).at(rowInRows(row)).isSet(EntityProperty::Modify)) return false;
    return (rowsListForRow(row).at(rowInRows(row)).get(EntityProperty::Modify).value<EntityModify::State>())==EntityModify::Edited;
}

int EntityListModel::createNew()
{
    aIdGen++;
    append(EntityInstance()
           .set(EntityProperty::Modify,(int)EntityModify::New)
           .set(EntityProperty::Id,QVariant::fromValue(aIdGen))
           .set(EntityProperty::AutoId,QVariant::fromValue(aIdGen)));
    emit modifiedByUser();
    return aIdGen;
}

int EntityListModel::columnForProperty(int p) const
{
    return aInfo->aColProp.contains(p)? aInfo->aColProp.indexOf(p): -1;
}

int EntityListModel::propertyForColumn(int c) const
{
    return aInfo->aColProp.count()<=c? -1: aInfo->aColProp.at(c);
}

bool EntityListModel::hasModified() const
{
    foreach(EntityInstance e, aRows)
        if(e.isSet(EntityProperty::Modify)
                && e.get(EntityProperty::Modify).value<int>()!=EntityModify::Saved) return true;
    return false;
}

bool EntityListModel::hasMarked() const
{
    foreach(EntityInstance e, aRows)
        if(e.isSet(EntityProperty::Mark)
                && e.get(EntityProperty::Mark).value<Qt::CheckState>()==Qt::Checked) return true;
    return false;
}

void EntityListModel::sortOrderDefaultSet(PropertyPrecedeOrderList sortOrder)
{
    aSortOrderDefault= sortOrder;
}

void EntityListModel::insert(int offset, EntityInstance entity)
{
    // TODO: decide insert or not into the fixed rows lists
    emit beginInsertRows(QModelIndex(),offset,offset);
    rowsListForRow(offset).insert(rowInRows(offset),entity);
    emit endInsertRows();
}

int EntityListModel::insertInSortOrder(EntityInstance entity)
{
    for(int i=0; i<aRows.count(); i++){
        if(EntityInstanceCompare::precede(entity,aRows.at(i),aSortOrder, aInfo->aEntInfo)){
            beginInsertRows(QModelIndex(),i,i);
            aRows.insert(i,entity);
            endInsertRows();
            return i;
        }
    }
    return append(entity);
}

int EntityListModel::append(EntityInstance entity)
{
    if(!entity.isSet(EntityProperty::AutoId)) entity.set(EntityProperty::AutoId, autoIdGenerateNext());
    int idx= aRows.count();
    aRows.append(entity);
    if(!aUpdateTimer->isActive()) aUpdateTimer->start();
    return idx;
}

void EntityListModel::append(const QList<EntityInstance> &entities)
{
    beginInsertRows(QModelIndex(),(aRows.count()>0?aRows.count()-1:0),(aRows.count()+entities.count()>0?aRows.count()+entities.count()-1:0));
    aRows.append(entities);
    endInsertRows();
}

void EntityListModel::replace(int offset, EntityInstance entity)
{
    // TODO: decide replace or not in the fixed rows lists
    if(offset >= 0 && offset < aRows.count()){
        rowsListForRow(offset).replace(rowInRows(offset),entity);
        emit dataChanged(index(offset,0), index(offset,columnCount()>0?columnCount()-1:0));
    }
}

bool EntityListModel::replaceByProperty(int property, EntityInstance entity)
{
    for(int i=0; i<aRows.count(); i++){
        if(aRows.at(i).get(property) == entity.get(property)){
            aRows.replace(i,entity);
            emit dataChanged(index(i,0), index(i,columnCount()>0?columnCount()-1:0));
            return true;
        }
    }
    return false;
}

bool EntityListModel::replaceByPropertyOrAppend(int property, EntityInstance entity)
{
    for(int i=0; i<aRows.count(); i++){
        if(aRows.at(i).get(property) == entity.get(property)){
            aRows.replace(i,entity);
            emit dataChanged(index(i,0), index(i,columnCount()>0?columnCount()-1:0));
            return true;
        }
    }
    aRows.append(entity);
    if(!aUpdateTimer->isActive()) aUpdateTimer->start();
    return true;
}

bool EntityListModel::replaceByExample(EntityInstance example, EntityInstance entity)
{
    int i=0;
    while(i<aRows.count()){
        bool needReplace= true;
        foreach(int property, example.properties()){
            if(aRows.at(i).get(property) != example.get(property)){
                needReplace= false;
            }
        }
        if(needReplace){
            replace(i,entity);
            return true;
        }
        i++;
    }
    return false;
}

void EntityListModel::replaceByPropertyOrInsertInSortOrder(int property, EntityInstance entity)
{
    if(replaceByProperty(property, entity)) return;
    insertInSortOrder(entity);
}

void EntityListModel::replaceByExampleOrInsertInSortOrder(EntityInstance example, EntityInstance entity)
{
    if(replaceByExample(example,entity)) return;
    insertInSortOrder(entity);
}

void EntityListModel::removeEntityByProperty(int property, QVariant value)
{
    int i=0;
    while(i<aRows.count()){
        if(aRows.at(i).get(property) == value){
            removeRow(i);
            continue;
        }
        i++;
    }
    return;
}

void EntityListModel::removeEntityByExample(EntityInstance example)
{
    int i=0;
    while(i<aRows.count()){
        bool needRemove= true;
        foreach(int property, example.properties()){
            if(aRows.at(i).get(property) != example.get(property)){
                needRemove= false;
            }
        }
        if(needRemove){
            removeRow(i);
            continue;
        }
        i++;
    }
    return;
}

bool EntityListModel::removeRow(int row, const QModelIndex &parent)
{
    Q_UNUSED(parent)
    if(row<0 && row>=rowCount()) return false;
    beginRemoveRows(parent,row,row);
    rowsListForRow(row).removeAt(rowInRows(row));
    endRemoveRows();
    return true;
}

bool EntityListModel::removeRows(int row, int count, const QModelIndex &parent)
{
    Q_UNUSED(parent)
    if(count<=0 || row<0 || row>=rowCount()) return false;
    int cnt= row+count<rowCount()?count:rowCount()-row;
    beginRemoveRows(parent,row,row+cnt-1);
    while(cnt-->0) rowsListForRow(row).removeAt(rowInRows(row));;
    endRemoveRows();
    return true;
}

bool EntityListModel::hasSort()
{
    return aSortOrder.count()<=0?false:true;
}

void EntityListModel::sort()
{
    if(aRows.isEmpty()
            || (aSortOrder.isEmpty()
                && aSortOrderDefault.isEmpty()) ){
        return;
    }
    emit busy();
    aArranger->arrange(aRows, (aSortOrder.isEmpty()?aSortOrderDefault:aSortOrder), aInfo->aEntInfo);
}

void EntityListModel::sortCancel()
{
    aArranger->cancel();
}

void EntityListModel::valueFilterSet(const PropertyValueFilter &filter)
{
    valueFilterClear();
    aValueFilter= filter;
}

void EntityListModel::valueFilterClear()
{
    if(!aFilteredRows.isEmpty()){
        aRows.append(aFilteredRows);
    }
    aFilteredRows.clear();
    aValueFilter.clear();
    sort();
}

void EntityListModel::valueFilterApply()
{
    emit layoutAboutToBeChanged();
    if(aValueFilter.isEmpty()){
        foreach(EntityInstance e, aFilteredRows) insertInSortOrder(e);
    }else{
        foreach(EntityInstance e, aRows)
            foreach(int p, aValueFilter.keys()){
                if(!e.isSet(p) || !aValueFilter.value(p).contains(e.get(p))){
                    aFilteredRows.append(e);
                    aRows.removeOne(e);
                    break;
                }
            }
    }
    emit layoutChanged();
}

void EntityListModel::filterSet(const EntityCompareConditions &filter)
{
    aFilter= filter;
}

void EntityListModel::filterClear()
{
    emit layoutAboutToBeChanged();
    if(!aFilteredRows.isEmpty()){
        aRows.append(aFilteredRows);
    }
    aFilteredRows.clear();
    aFilter.clear();
    emit layoutChanged();
    emit ready();
    aSortTimer->start();
}

void EntityListModel::filterApply()
{
    if(!aFilteredRows.isEmpty()){
        aRows.append(aFilteredRows);
    }
    if(aRows.isEmpty()) return;
    emit busy();
    aFilterer->filter(aRows, aInfo->aEntInfo, aFilter);
}

void EntityListModel::filterCancel()
{
    aFilterer->cancel();
}

void EntityListModel::clear()
{
    aArranger->cancel();
    aFilterer->cancel();

    beginRemoveRows(QModelIndex(),0,(rowCount()>0?rowCount()-1:0));
    aRows.clear();
    aRowsFixedTop.clear();
    aRowsFixedBottom.clear();
    aFilteredRows.clear();
    aIdGen=0;
    endRemoveRows();
}

void EntityListModel::onUpdateTimer()
{
    aUpdateTimer->stop();
    emit layoutAboutToBeChanged();
    emit layoutChanged();
    emit modified();
}

int EntityListModel::autoIdGenerateNext()
{
    return ++aIdGen;
}

void EntityListModel::onSortTimer()
{
    aSortTimer->stop();
    sort();
}

void EntityListModel::onSort(int column, def::SortOrder order)
{
    if(!setSort(column, order)){
        return;
    }
    aSortTimer->start();
}

void EntityListModel::rowsFixedTopSet(EntityInstanceList &entities)
{
    if(aRowsFixedTop.count()>0){
        beginRemoveRows(QModelIndex(),0,aRowsFixedTop.count()-1);
        aRowsFixedTop.clear();
        endRemoveRows();
    }
    beginInsertRows(QModelIndex(),0,(entities.count()>0?entities.count()-1:0));
    aRowsFixedTop= entities;
    emit endInsertRows();
}

void EntityListModel::onAsyncSortFinished(EntityInstanceList sortedRows, bool canceled)
{
    if(!canceled){
        emit layoutAboutToBeChanged();
        aRows= sortedRows;
        emit layoutChanged();
        emit dataChanged(index(0,0),index((aRows.count()>0?aRows.count()-1:0),(columnCount()>0?columnCount()-1:0)));
    }
    emit ready();
}

void EntityListModel::onAsyncFilterFinished(EntityInstanceList matchedRows, EntityInstanceList skippedRows, bool canceled)
{
    if(!canceled){
        if(skippedRows.count()>0) beginRemoveRows(QModelIndex(),matchedRows.count(),matchedRows.count()+skippedRows.count()-1);
        aRows= matchedRows;
        aFilteredRows= skippedRows;
        if(skippedRows.count()>0) endRemoveRows();
        emit layoutAboutToBeChanged();
        emit layoutChanged();
    }
    emit ready();
    aSortTimer->start();
}

QList<EntityInstance> &EntityListModel::rowsListForIdx(const QModelIndex &idx) const
{
    return rowsListForRow(idx.row());
}

QList<EntityInstance> &EntityListModel::rowsListForRow(const int row) const
{
    if(row<aRowsFixedTop.count()){
        return (QList<EntityInstance> &)aRowsFixedTop;
    }else if(row<aRows.count()+aRowsFixedTop.count()){
        return (QList<EntityInstance> &)aRows;
    }else return (QList<EntityInstance> &)aRowsFixedBottom;
}

int EntityListModel::rowInRows(int row) const
{
    if(row<aRowsFixedTop.count()){
        return row;
    }else if(row<aRows.count()+aRowsFixedTop.count()){
        return row-aRowsFixedTop.count();
    }else return row-aRowsFixedTop.count()-aRows.count();
}
