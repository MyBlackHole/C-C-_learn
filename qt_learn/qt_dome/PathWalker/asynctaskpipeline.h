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

#ifndef ASYNCTASKPIPELINE_H
#define ASYNCTASKPIPELINE_H

#include <QObject>
#include <QThread>
#include <QMutex>

#include "entityinstance.h"
#include "task.h"

namespace TaskWorkerState {
enum State{
    Idle=0
    ,Busy
    ,Wait
};
}

class TaskWorker : public QObject
{
    Q_OBJECT

signals:
    void done(EntityInstance task);
    void stateChanged(int value);

public slots:
    void work(QMutex *aCancel, EntityInstance task, EntityInstance result);
};


class AsyncTaskPipeline : public QObject
{
    Q_OBJECT
    QThread aThread;
    TaskWorker *aWorker;

public:
    explicit AsyncTaskPipeline(QObject *parent = 0);
    ~AsyncTaskPipeline();

signals:
    void invokeWorker(QMutex *aCancel, EntityInstance task);
    void done(EntityInstance task, bool canceled);

public slots:
    void cancel();
    void schedule(EntityInstance task);

protected slots:
    void onWorkerDone(EntityInstance task);
    void onWorkerStateChanged(int value);

private:
    EntityInstanceList aQueueList;

    int aWorkerState;
    QMutex aWorkerCancel;

    QMutex aMtx;
};

#endif // ASYNCTASKPIPELINE_H
