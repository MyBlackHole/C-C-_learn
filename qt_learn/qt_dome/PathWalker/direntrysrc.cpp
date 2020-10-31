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
#include <QStandardPaths>

#include "direntrysrc.h"
#include "direntry.h"
#include "fileiconcache.h"

DirEntryFetcher::DirEntryFetcher(QMutex *cancelMutex, QObject *parent)
    :QObject(parent), aCancel(cancelMutex)
{
}

void DirEntryFetcher::fetch(QString path)
{
    emit stateChanged(DirEntryFetcherState::Busy);
    QFileIconProvider fip;
    QIcon icon;
    QString sfx=":";
    if(FileIconCache::contains(sfx)) icon= FileIconCache::icon(sfx);
    else{
        icon= fip.icon(QFileInfo("./"));
        FileIconCache::iconAdd(sfx,icon);
    }
    EntityInstance e;
    e.set(DirEntryProperty::Name,"..");
    e.set(DirEntryProperty::isDir, true);
    e.set(DirEntryProperty::icon, icon);
    QDir d(path);
    if(!d.exists()){
        emit verified(false, e);
        emit stateChanged(DirEntryFetcherState::Idle);
        // TODO: emit done???
        return;
    }

    emit verified(true, e.set(DirEntryProperty::LastModified,QFileInfo(path).lastModified()));

    if(!aCancel->tryLock()){
        emit stateChanged(DirEntryFetcherState::Idle);
        emit canceled();
        emit finished();
        return;
    }
    aCancel->unlock();

    if(fetchEntries(path, QDir::Drives|QDir::Dirs|QDir::NoDot|QDir::NoDotDot)){
        if(!fetchEntries(path, QDir::Files)) emit canceled();
    }else emit canceled();
    emit stateChanged(DirEntryFetcherState::Idle);
    emit finished();
}

bool DirEntryFetcher::fetchEntries(QString path, QDir::Filters dirFilters)
{
    QString icoCachePath= QStandardPaths::writableLocation(QStandardPaths::DataLocation)+"/icocache/";
    if(!QDir(icoCachePath).exists()) QDir().mkpath(icoCachePath);
    QFileIconProvider fip;
    QDirIterator di(path,dirFilters);
    while(di.hasNext()){
        if(!aCancel->tryLock()){
            return false;
        }
        aCancel->unlock();
        di.next();
        QFileInfo fi= di.fileInfo();

        QIcon icon;
        QString sfx=":";
        if(fi.isDir()){
            if(FileIconCache::contains(sfx)) icon= FileIconCache::icon(sfx);
            else{
                icon= fip.icon(QFileInfo("./"));
                FileIconCache::iconAdd(sfx,icon);
            }
        }else{
            QStringList sfxs= fi.completeSuffix().toLower().split('.', QString::SkipEmptyParts);
            sfx= sfxs.isEmpty()?"":sfxs.last();
            if(FileIconCache::contains(sfx)) icon= FileIconCache::icon(sfx);
            else{
                QFile df(icoCachePath+"x."+sfx);
                df.open(QIODevice::Append);
                df.close();
                icon= fip.icon(QFileInfo(df.fileName()));
                FileIconCache::iconAdd(sfx,icon);
            }
        }
        EntityInstance e;
        e.set(DirEntryProperty::Name, fi.fileName());
        if(!fi.isDir()) e.set(DirEntryProperty::Size, fi.size());
        else if(fi.fileName()=="..") e.set(DirEntryProperty::isSpecial,true);
        e.set(DirEntryProperty::LastModified, fi.lastModified());
        e.set(DirEntryProperty::isDir, fi.isDir());
        e.set(DirEntryProperty::icon, icon);
        emit dataReady(e);
    }
    return true;
}

DirEntrySrc::DirEntrySrc(QObject *parent)
    : QObject(parent)
{
    DirEntryFetcher *aWorker= new DirEntryFetcher(&aWorkerCancel);
    aWorker->moveToThread(&aThread);

    connect(this, &DirEntrySrc::invokeWorkerFetch, aWorker, &DirEntryFetcher::fetch);

    connect(aWorker, &DirEntryFetcher::dataReady, this, &DirEntrySrc::dataReady);
    connect(aWorker, &DirEntryFetcher::verified, this, &DirEntrySrc::verified);
    connect(aWorker, &DirEntryFetcher::finished, this, &DirEntrySrc::onWorkerFinished);
    connect(aWorker, &DirEntryFetcher::stateChanged, this, &DirEntrySrc::onWorkerStateChanged);

    aThread.start();
}

DirEntrySrc::~DirEntrySrc()
{
    aThread.quit();
    aThread.wait();
    aWorker->deleteLater();
}

QString DirEntrySrc::path() const
{
    return aPath;
}

void DirEntrySrc::cancel()
{
    QMutexLocker lck(&aMtx);
    if(aWorkerState != DirEntryFetcherState::Busy) return;
    aWorkerCancel.tryLock();
}

bool DirEntrySrc::fetch(const QString &path)
{
    QMutexLocker lck(&aMtx);
    aPathQueue= path;
    if(aWorkerState == DirEntryFetcherState::Busy){
        aWorkerCancel.lock();
    }else{
        aPath= path;
        emit invokeWorkerFetch(aPath);
    }
    return true;
}

void DirEntrySrc::onWorkerFinished()
{
    if(!aWorkerCancel.tryLock()) emit canceled();
    aWorkerCancel.unlock();
    if(aPath != aPathQueue){
        aPath= aPathQueue;
        emit invokeWorkerFetch(aPath);
    }else emit finished();
}

void DirEntrySrc::onWorkerStateChanged(int value)
{
    QMutexLocker lck(&aMtx);
    aWorkerState= value;
}
