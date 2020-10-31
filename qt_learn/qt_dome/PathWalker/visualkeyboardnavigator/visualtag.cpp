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

#include "visualtag.h"

VisualTag::VisualTag(QWidget *parent) :
    QLabel(parent)
{
    doInit();
}

VisualTag::VisualTag(const QString &text, QWidget *parent, Qt::WindowFlags f):
    QLabel(text, parent, f)
{
    doInit();
}

void VisualTag::doInit()
{
    hide();
    setObjectName("navigator");
    setStyleSheet("QLabel#navigator {"
                  "border-width: 1px;"
                  "border-style: solid;"
                  "border-radius: 0px;"
                  "border-bottom-right-radius: 5px;"
                  "border-color: darkblue;"
                  "background: darkblue;"
                  "font-weight: bold;"
                  "color: white;"
                  "}");
    setFixedSize(20,20);
    setAlignment(Qt::AlignCenter);
}
