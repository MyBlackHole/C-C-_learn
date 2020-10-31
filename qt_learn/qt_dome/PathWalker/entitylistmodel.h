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

#ifndef ENTITYLISTMODEL_H
#define ENTITYLISTMODEL_H

#include <QAbstractTableModel>
#include <QList>
#include <QTimer>
#include <QtConcurrent/QtConcurrent>
#include <functional>

#include "entityinstance.h"
#include "entitycompare.h"
#include "entitylistmodelinfo.h"

class AsyncEntityListArranger;
class AsyncEntityListFilter;

class EntityListModel : public QAbstractTableModel
{
    Q_OBJECT

public:
    explicit EntityListModel(EntityListModelInfo *mInfo, QObject *parent = 0);
    virtual int columnCount(const QModelIndex &parent = QModelIndex()) const;
    virtual int rowCount(const QModelIndex & parent = QModelIndex()) const;
    virtual QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;
    virtual Qt::ItemFlags flags(const QModelIndex & index ) const;
    virtual bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole);
    bool setProperty(int row, int property, const QVariant &value, int role = Qt::EditRole);
    virtual QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const;
    virtual bool setHeaderData(int section, Qt::Orientation orientation, const QVariant &value, int role = Qt::EditRole);
    int filteredRowCount() const;
    EntityInstance at(int row) const;
    EntityInstance takeAt(int row);
    PropertyValueFilter filter() const;
    bool isMatchFilter(const EntityInstance &e) const;
    EntityInstance getByPropertyFirst(int property, QVariant value) const;
    bool setSort(int column, def::SortOrder order);
    bool isEntityMarked(int row);
    bool isEntityNew(int row);
    bool isEntityModified(int row);
    int createNew();
    int columnForProperty(int p) const;
    int propertyForColumn(int c) const;
    bool hasModified() const;
    bool hasMarked() const;
    void sortOrderDefaultSet(PropertyPrecedeOrderList sortOrder);

signals:
    void modifiedByUser();
    void modified();
    void docAssignChanged(int row);
    void busy();
    void ready();

public slots:
    void insert(int offset, EntityInstance entity);
    int insertInSortOrder(EntityInstance entity); // return row for inserted entity
    int append(EntityInstance entity); // return row for appended entity
    void append(const QList<EntityInstance> &entities);
    void replace(int offset, EntityInstance entity);
    bool replaceByProperty(int property, EntityInstance entity);
    bool replaceByPropertyOrAppend(int property, EntityInstance entity);
    bool replaceByExample(EntityInstance example, EntityInstance entity);
    void replaceByPropertyOrInsertInSortOrder(int property, EntityInstance entity);
    void replaceByExampleOrInsertInSortOrder(EntityInstance example, EntityInstance entity);
    void removeEntityByProperty(int property, QVariant value);
    void removeEntityByExample(EntityInstance example);
    virtual bool removeRow(int row, const QModelIndex &parent= QModelIndex());
    virtual bool removeRows(int row, int count, const QModelIndex &parent= QModelIndex());
    bool hasSort();
    void sort();
    void sortCancel();
    void valueFilterSet(const PropertyValueFilter &filter);
    void valueFilterClear();
    void valueFilterApply();
    void filterSet(const EntityCompareConditions &filter);
    void filterClear();
    void filterApply();
    void filterCancel();
    void clear();
    void onUpdateTimer();
    int autoIdGenerateNext();
    void onSortTimer();
    void onSort(int column, def::SortOrder order);
    void rowsFixedTopSet(EntityInstanceList &entities);

protected slots:
    void onAsyncSortFinished(EntityInstanceList sortedRows, bool canceled);
    void onAsyncFilterFinished(EntityInstanceList matchedRows, EntityInstanceList skippedRows, bool canceled);

protected:
    QList<EntityInstance>& rowsListForIdx(const QModelIndex &idx) const;
    QList<EntityInstance>& rowsListForRow(const int row) const;
    int rowInRows(int row) const;
    PropertyPrecedeOrderList aSortOrder;
    PropertyPrecedeOrderList aSortOrderDefault;
    PropertyValueFilter aValueFilter;
    EntityCompareConditions aFilter;
    QList<EntityInstance> aRowsFixedTop;
    QList<EntityInstance> aRowsFixedBottom;
    QList<EntityInstance> aRows;
    QList<EntityInstance> aFilteredRows;
    int aIdGen;
    QTimer *aUpdateTimer;
    AsyncEntityListArranger *aArranger;
    AsyncEntityListFilter *aFilterer;
    QTimer *aSortTimer;
    EntityListModelInfo *aInfo;
};

#endif // ENTITYLISTMODEL_H
