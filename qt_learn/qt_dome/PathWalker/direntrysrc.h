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

#ifndef DIRENTRYSRC_H
#define DIRENTRYSRC_H

#include <QThread>
#include <QDateTime>
#include <QDir>
#include <QMutex>

#include "entityinstance.h"

namespace DirEntryFetcherState {
enum State{
    Idle=0
    ,Busy
};
}

class DirEntryFetcher : public QObject
{
    Q_OBJECT

public:
    explicit DirEntryFetcher(QMutex *cancelMutex, QObject *parent=0);

signals:
    void canceled();
    void verified(bool ok, EntityInstance ent);
    void dataReady(EntityInstance ent);
    void finished();
    void stateChanged(int value);

public slots:
    void fetch(QString path);
    bool fetchEntries(QString path, QDir::Filters dirFilters);

private:
    QMutex *aCancel;
};

class DirEntrySrc : public QObject
{
    Q_OBJECT
    QThread aThread;
    DirEntryFetcher *aWorker;

public:
    DirEntrySrc(QObject *parent=0);
    ~DirEntrySrc();
    QString path() const;

signals:
    void canceled();
    void verified(bool ok, EntityInstance ent);
    void dataReady(EntityInstance ent);
    void invokeWorkerFetch(QString path);
    void invokeWorkerCancelSet(int value);
    void finished();

public slots:
    void cancel();
    bool fetch(const QString& path);

protected slots:
    void onWorkerFinished();
    void onWorkerStateChanged(int value);


private:
    QString aPathQueue;
    QString aPath;
    int aWorkerState;
    QMutex aWorkerCancel;

    QMutex aMtx;
};

#endif // DIRENTRYSRC_H
