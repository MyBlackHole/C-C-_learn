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

#ifndef QUICKFILTER_H
#define QUICKFILTER_H

#include "settingsstore/isettings.h"

class QLineEdit;
class QAbstractTableModel;
class QCompleter;
class QTimer;
class QStringListModel;

class QuickFilter : public ISettings
{
    Q_OBJECT
public:
    explicit QuickFilter(QWidget *parent = 0);
    virtual bool settingsSave();
    virtual bool settingsRestore();
    QString pattern();
    virtual bool eventFilter(QObject *watched, QEvent *event);

signals:
    void searching(QString patt);

public slots:
    void patternAppend(const QString &text);
    void patternClear();
    void onNotifyTimer();
    void cancel();

protected:
    inline QStringListModel* completerModel();

private:
    QString aLastAppliedPatt;
    QLineEdit *aPatt;
    QCompleter *aCompleter;
    QTimer *aNotifyTimer;

    void doInitLayout();
    void doInitConnection();
};

#endif // QUICKFILTER_H
