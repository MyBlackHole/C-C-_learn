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

#ifndef PATHCHANGEWATCHER_H
#define PATHCHANGEWATCHER_H

#include <QThread>
#include <QMutex>
#include <QDateTime>

namespace PathChangeWatcherState {
enum State{
    Idle=0
    ,Busy
};
}

class PathChangeWatcherWorker : public QObject
{
    Q_OBJECT

signals:
    void done();
    void modified(QString path);
    void stateChanged(int value);

public slots:
    void watch(QMutex *aCancel, QString path);
};


class PathChangeWatcher : public QObject
{
    Q_OBJECT
    QThread aThread;
    PathChangeWatcherWorker *aWorker;

public:
    PathChangeWatcher(QObject *parent=0);
    ~PathChangeWatcher();

signals:
    void modified(QString path);
    void invokeWorkerWatch(QMutex *aCancel, QString path);

public slots:
    void cancel();
    void watch(QString path);

protected slots:
    void onWorkerFinished();
    void onWorkerStateChanged(int value);

private:
    QString aQueuePath;

    int aWorkerState;
    QMutex aWorkerCancel;

    QMutex aMtx;
};

#endif // PATHCHANGEWATCHER_H
