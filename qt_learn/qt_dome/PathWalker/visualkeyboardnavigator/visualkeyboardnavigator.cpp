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

#include <QtCore>
#include <QApplication>

#include "visualkeyboardnavigator.h"


// Global static pointer used to ensure a single instance of the class.
QList<VisualTagInfo> VisualKeyboardNavigator::aInfo= QList<VisualTagInfo>();
VisualKeyboardNavigatorEventFilter* VisualKeyboardNavigator::aNavi= 0;

// This function is called to create an instance of the class.
// Calling the constructor publicly is not allowed. The constructor
// is private and is only called by this Instance function.

const QList<VisualTagInfo> &VisualKeyboardNavigator::info()
{
    return aInfo;
}

void VisualKeyboardNavigator::registerTag(Qt::Key key, QWidget *tag, QWidget *target)
{
    if(aNavi==0){
        VisualKeyboardNavigator::aNavi= new VisualKeyboardNavigatorEventFilter();
        QObject::connect(qApp, &QApplication::focusChanged, aNavi, &VisualKeyboardNavigatorEventFilter::onApplicationFocusChanged);
    }

    aInfo.append(VisualTagInfo(key,tag,target));
}

void VisualKeyboardNavigator::unregisterTag(Qt::Key key, QWidget *tag, QWidget *target)
{
    aInfo.removeAll(VisualTagInfo(key,tag,target));
}
