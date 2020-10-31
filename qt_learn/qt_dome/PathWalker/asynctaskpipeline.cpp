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

#include "asynctaskpipeline.h"

void TaskWorker::work(QMutex *aCancel, EntityInstance task, EntityInstance result)
{
    Q_UNUSED(aCancel);
    emit stateChanged(TaskWorkerState::Busy);
    if(task.get(TaskProperty::Type).value<int>() == TaskType::Rename){
        ;
    }else if(task.get(TaskProperty::Type).value<int>() == TaskType::Copy){
        ;
    }else if(task.get(TaskProperty::Type).value<int>() == TaskType::Move){
        ;
    }else if(task.get(TaskProperty::Type).value<int>() == TaskType::Delete){
        ;
    }
    emit stateChanged(TaskWorkerState::Idle);
}

AsyncTaskPipeline::AsyncTaskPipeline(QObject *parent) :
    QObject(parent)
{
}

AsyncTaskPipeline::~AsyncTaskPipeline()
{
    aThread.quit();
    aThread.wait();
    aWorker->deleteLater();
}

void AsyncTaskPipeline::cancel()
{

}

void AsyncTaskPipeline::schedule(EntityInstance task)
{

}

void AsyncTaskPipeline::onWorkerDone(EntityInstance task)
{

}

void AsyncTaskPipeline::onWorkerStateChanged(int value)
{
    Q_UNUSED(value);
}
