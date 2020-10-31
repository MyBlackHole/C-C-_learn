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

#include "asyncentitylistfilter.h"

#include <QtDebug>

void EntityListFilter::filter(QMutex *aCancel, EntityInstanceList list, EntityInfo info, EntityCompareConditions filter)
{
    emit stateChanged(EntityListFilterState::Busy);
    EntityInstanceList matched;
    EntityInstanceList skipped;
    for(int i=0; i<list.count(); i++){
        if(!aCancel->tryLock()){
            emit done(EntityInstanceList(), EntityInstanceList());
            return;
        }
        aCancel->unlock();
        if(EntityCompare::isMatch(list.at(i), info, filter)) matched.append(list.at(i));
        else skipped.append(list.at(i));
    }
    emit stateChanged(EntityListFilterState::Idle);
    emit done(matched, skipped);
}

AsyncEntityListFilter::AsyncEntityListFilter(QObject *parent)
    : QObject(parent)
{
    EntityListFilter *aWorker= new EntityListFilter;
    aWorker->moveToThread(&aThread);
    connect(this, &AsyncEntityListFilter::invokeWorkerFilter, aWorker, &EntityListFilter::filter);
    connect(aWorker, &EntityListFilter::done, this, &AsyncEntityListFilter::onWorkerFinished);
    connect(aWorker, &EntityListFilter::stateChanged, this, &AsyncEntityListFilter::onWorkerStateChanged);
    aThread.start();
}

AsyncEntityListFilter::~AsyncEntityListFilter()
{
    aThread.quit();
    aThread.wait();
    aWorker->deleteLater();
}

void AsyncEntityListFilter::cancel()
{
    QMutexLocker lck(&aMtx);
    if(aWorkerState == EntityListFilterState::Busy) aWorkerCancel.tryLock();
}

void AsyncEntityListFilter::filter(EntityInstanceList list, EntityInfo info, EntityCompareConditions filter)
{
    QMutexLocker lck(&aMtx);
    if(aWorkerState == EntityListFilterState::Busy){
        aQueueList= list;
        aQueueInfo= info;
        aQueueFilter= filter;
        aWorkerCancel.lock();
    }else{
        emit invokeWorkerFilter(&aWorkerCancel, list, info, filter);
    }
}

void AsyncEntityListFilter::onWorkerFinished(EntityInstanceList matched, EntityInstanceList skipped)
{
    QMutexLocker lck(&aMtx);
    if(aWorkerState != EntityListFilterState::Idle) aWorkerCancel.unlock();
    if(!aQueueList.isEmpty()){
        EntityInstanceList l(aQueueList);
        EntityInfo i(aQueueInfo);
        EntityCompareConditions f(aQueueFilter);

        aQueueList= EntityInstanceList();
        aQueueInfo= EntityInfo();
        aQueueFilter= EntityCompareConditions();
        emit invokeWorkerFilter(&aWorkerCancel, l, i, f);
    }else{
        emit done(matched, skipped, (aWorkerState != EntityListFilterState::Idle));
        aWorkerState= EntityListFilterState::Idle;
    }
}

void AsyncEntityListFilter::onWorkerStateChanged(int value)
{
    QMutexLocker lck(&aMtx);
    aWorkerState= value;
}
