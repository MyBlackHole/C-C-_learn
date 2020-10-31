/*
 * This file is part of Visual Keyboard Navi software
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

#ifndef VISUALTAG_H
#define VISUALTAG_H

#include <QLabel>

class VisualTag : public QLabel
{
    Q_OBJECT
public:
    explicit VisualTag(QWidget *parent = 0);
    explicit VisualTag(const QString & text, QWidget * parent = 0, Qt::WindowFlags f = 0);

signals:

public slots:

private:
    void doInit();

};

#endif // VISUALTAG_H
