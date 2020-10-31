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

#include <QtDebug>

#include "asyncentitylistarranger.h"

void EntityListArranger::arrange(QMutex *aCancel, EntityInstanceList list, PropertyPrecedeOrderList sortOrder, const EntityInfo &inf)
{
    emit stateChanged(EntityListArrangerState::Busy);
    for(int i=0; i<list.count(); i++){
        for(int j=i+1; j<list.count(); j++){
            if(!aCancel->tryLock()){
                emit done(EntityInstanceList());
                return;
            }
            aCancel->unlock();
            if(EntityInstanceCompare::precede(list.at(j), list.at(i),sortOrder, inf)){
                list.insert(i,list.takeAt(j));
            }
        }
    }
    emit stateChanged(EntityListArrangerState::Idle);
    emit done(list);
}


AsyncEntityListArranger::AsyncEntityListArranger(QObject *parent)
    : QObject(parent)
{
    EntityListArranger *aWorker= new EntityListArranger;
    aWorker->moveToThread(&aThread);
    connect(this, &AsyncEntityListArranger::invokeWorkerArrange, aWorker, &EntityListArranger::arrange);
    connect(aWorker, &EntityListArranger::done, this, &AsyncEntityListArranger::onWorkerFinished);
    connect(aWorker, &EntityListArranger::stateChanged, this, &AsyncEntityListArranger::onWorkerStateChanged);
    aThread.start();
}

AsyncEntityListArranger::~AsyncEntityListArranger()
{
    aThread.quit();
    aThread.wait();
    aWorker->deleteLater();
}

void AsyncEntityListArranger::cancel()
{
    QMutexLocker lck(&aMtx);
    if(aWorkerState == EntityListArrangerState::Busy) aWorkerCancel.tryLock();
}

void AsyncEntityListArranger::arrange(EntityInstanceList list, PropertyPrecedeOrderList sortOrder, const EntityInfo &inf)
{
    QMutexLocker lck(&aMtx);
    if(aWorkerState == EntityListArrangerState::Busy){
        aQueueList= list;
        aQueueSortOrder= sortOrder;
        aEntityInfo= inf;
        aWorkerCancel.tryLock();
    }else{
        emit invokeWorkerArrange(&aWorkerCancel, list, sortOrder, inf);
    }
}

void AsyncEntityListArranger::onWorkerFinished(EntityInstanceList list)
{
    QMutexLocker lck(&aMtx);
    if(aWorkerState != EntityListArrangerState::Idle) aWorkerCancel.unlock();
    if(!aQueueList.isEmpty()){
        EntityInstanceList l(aQueueList);
        PropertyPrecedeOrderList s(aQueueSortOrder);
        EntityInfo i(aEntityInfo);

        aQueueList= EntityInstanceList();
        aQueueSortOrder= PropertyPrecedeOrderList();
        emit invokeWorkerArrange(&aWorkerCancel, l, s, i);
    }else{
        emit done(list, (aWorkerState != EntityListArrangerState::Idle));
        aWorkerState= EntityListArrangerState::Idle;
    }
}

void AsyncEntityListArranger::onWorkerStateChanged(int value)
{
    QMutexLocker lck(&aMtx);
    aWorkerState= value;
}
