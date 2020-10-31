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

#ifndef VISUALKEYBOARDNAVIGATOR_H
#define VISUALKEYBOARDNAVIGATOR_H

#include <QList>

#include "visualkeyboardnavigatoreventfilter.h"
#include "visualtaginfo.h"
#include "visualtag.h"

class QWidget;

class VisualKeyboardNavigator
{
    friend class VisualKeyboardNavigatorEventFilter;

public:
    static void registerTag(Qt::Key key, QWidget *tag, QWidget *target);
    static void unregisterTag(Qt::Key key, QWidget *tag, QWidget *target);

protected:
    static const QList<VisualTagInfo>& info();

private:
    VisualKeyboardNavigator(){}  // Private so that it can  not be called
    VisualKeyboardNavigator(VisualKeyboardNavigator const&){}             // copy constructor is private
    VisualKeyboardNavigator& operator=(VisualKeyboardNavigator const&){return *this;}  // assignment operator is private
    static QList<VisualTagInfo> aInfo;
    static VisualKeyboardNavigatorEventFilter *aNavi;
};

#endif // VISUALKEYBOARDNAVIGATOR_H
