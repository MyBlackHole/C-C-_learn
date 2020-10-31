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

#ifndef ASYNCENTITYLISTFILTER_H
#define ASYNCENTITYLISTFILTER_H

#include <QThread>
#include <QMutex>

#include "entityinstance.h"
#include "entitycompare.h"

namespace EntityListFilterState {
enum State{
    Idle=0
    ,Busy
};
}

class EntityListFilter : public QObject
{
    Q_OBJECT

signals:
    void done(EntityInstanceList matched, EntityInstanceList skipped);
    void stateChanged(int value);

public slots:
    void filter(QMutex *aCancel, EntityInstanceList list, EntityInfo info, EntityCompareConditions filter);
};

class AsyncEntityListFilter : public QObject
{
    Q_OBJECT
    QThread aThread;
    EntityListFilter *aWorker;

public:
    AsyncEntityListFilter(QObject *parent=0);
    ~AsyncEntityListFilter();

signals:
    void invokeWorkerFilter(QMutex *aCancel, EntityInstanceList list, EntityInfo info, EntityCompareConditions filter);
    void done(EntityInstanceList matched, EntityInstanceList skipped, bool canceled);

public slots:
    void cancel();
    void filter(EntityInstanceList list, EntityInfo info, EntityCompareConditions filter);

protected slots:
    void onWorkerFinished(EntityInstanceList matched, EntityInstanceList skipped);
    void onWorkerStateChanged(int value);

private:
    EntityInstanceList aQueueList;
    EntityInfo aQueueInfo;
    EntityCompareConditions aQueueFilter;

    int aWorkerState;
    QMutex aWorkerCancel;

    QMutex aMtx;
};

#endif // ASYNCENTITYLISTFILTER_H
