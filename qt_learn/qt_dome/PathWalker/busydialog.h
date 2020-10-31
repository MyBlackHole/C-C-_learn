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

#ifndef BUSYDIALOG_H
#define BUSYDIALOG_H

#include <QFrame>

class QLabel;
class QToolButton;
class QMovie;

class BusyDialog : public QFrame
{
	Q_OBJECT
public:
    explicit BusyDialog(const QString &text, QWidget *parent = 0);
    void messageSet(const QString &text);

signals:
    void canceled();

public slots:
	void start(bool unbreakable= false);
	void stop();

protected slots:
    void onCancelClicked();

private:
    QLabel *aMessage;
    QToolButton *aCancel;
    QMovie *aSpinner;
};

#endif // BUSYDIALOG_H
