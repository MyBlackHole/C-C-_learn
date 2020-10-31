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

#ifndef ASYNCENTITYLISTARRANGER_H
#define ASYNCENTITYLISTARRANGER_H

#include <QThread>
#include <QMutex>

#include "entityinstance.h"
#include "entityinfo.h"

namespace EntityListArrangerState {
enum State{
    Idle=0
    ,Busy
};
}

class EntityListArranger : public QObject
{
    Q_OBJECT

signals:
    void done(EntityInstanceList rows);
    void stateChanged(int value);

public slots:
    void arrange(QMutex *aCancel, EntityInstanceList list, PropertyPrecedeOrderList sortOrder, const EntityInfo &inf);

};

class AsyncEntityListArranger : public QObject
{
    Q_OBJECT
    QThread aThread;
    EntityListArranger *aWorker;

public:
    AsyncEntityListArranger(QObject *parent=0);
    ~AsyncEntityListArranger();

signals:
    void invokeWorkerArrange(QMutex *aCancel, EntityInstanceList list, PropertyPrecedeOrderList sortOrder, const EntityInfo &inf);
    void done(EntityInstanceList rows, bool canceled);

public slots:
    void cancel();
    void arrange(EntityInstanceList list, PropertyPrecedeOrderList sortOrder, const EntityInfo &inf);

protected slots:
    void onWorkerFinished(EntityInstanceList list);
    void onWorkerStateChanged(int value);

private:
    EntityInstanceList aQueueList;
    PropertyPrecedeOrderList aQueueSortOrder;
    EntityInfo aEntityInfo;

    int aWorkerState;
    QMutex aWorkerCancel;

    QMutex aMtx;
};

#endif // ASYNCENTITYLISTARRANGER_H
