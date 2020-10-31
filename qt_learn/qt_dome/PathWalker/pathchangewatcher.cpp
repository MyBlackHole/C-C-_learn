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

#include <QDir>
#include <QtDebug>
#include <QDirIterator>
#include <QFileIconProvider>

#include "pathchangewatcher.h"

void PathChangeWatcherWorker::watch(QMutex *aCancel, QString path)
{
    emit stateChanged(PathChangeWatcherState::Busy);
    QDir d(path);
    if(!d.exists()){
        emit done();
        return;
    }

    QDateTime lastModified= QFileInfo(path).lastModified();
    while(1){
        if(!aCancel->tryLock()){
            aCancel->unlock();
            emit done();
            break;
        }
        aCancel->unlock();

        if(lastModified == QFileInfo(path).lastModified()){
            QThread::currentThread()->sleep(1);
            continue;
        }
        lastModified= QFileInfo(path).lastModified();
        emit modified(path);
    }
    emit stateChanged(PathChangeWatcherState::Idle);
    emit done();
}


PathChangeWatcher::PathChangeWatcher(QObject *parent)
    : QObject(parent)
{
    PathChangeWatcherWorker *aWorker= new PathChangeWatcherWorker;
    aWorker->moveToThread(&aThread);
    connect(this, &PathChangeWatcher::invokeWorkerWatch, aWorker, &PathChangeWatcherWorker::watch);
    connect(aWorker, &PathChangeWatcherWorker::modified, this, &PathChangeWatcher::modified);
    connect(aWorker, &PathChangeWatcherWorker::stateChanged, this, &PathChangeWatcher::onWorkerStateChanged);
    connect(aWorker, &PathChangeWatcherWorker::done, this, &PathChangeWatcher::onWorkerFinished);
    aThread.start();
}

PathChangeWatcher::~PathChangeWatcher()
{
    aThread.quit();
    aThread.wait();
    aWorker->deleteLater();
}

void PathChangeWatcher::cancel()
{
    QMutexLocker lck(&aMtx);
    if(aWorkerState == PathChangeWatcherState::Busy) aWorkerCancel.tryLock();
}

void PathChangeWatcher::watch(QString path)
{
    QMutexLocker lck(&aMtx);
    if(aWorkerState == PathChangeWatcherState::Busy){
        aQueuePath= path;
        aWorkerCancel.tryLock();
    }else{
        emit invokeWorkerWatch(&aWorkerCancel, path);
    }
}

void PathChangeWatcher::onWorkerFinished()
{
    QMutexLocker lck(&aMtx);
    if(!aQueuePath.isEmpty()){
        QString p(aQueuePath);
        aQueuePath= QString();
        emit invokeWorkerWatch(&aWorkerCancel, p);
    }else{
        aWorkerState= PathChangeWatcherState::Idle;
    }
}

void PathChangeWatcher::onWorkerStateChanged(int value)
{
    QMutexLocker lck(&aMtx);
    aWorkerState= value;
}
