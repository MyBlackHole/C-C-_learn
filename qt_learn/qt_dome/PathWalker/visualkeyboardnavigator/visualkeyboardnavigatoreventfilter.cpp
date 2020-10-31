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

#include <QApplication>
#include <QFrame>
#include <QPropertyAnimation>
#include <QKeyEvent>

#include <QWhatsThis>

#include "visualkeyboardnavigatoreventfilter.h"
#include "visualkeyboardnavigator.h"

VisualKeyboardNavigatorEventFilter::VisualKeyboardNavigatorEventFilter(QObject *parent) :
    QObject(parent)
{
    isActive= false;

    aMsk= new QFrame(0);
    aMsk->setLineWidth(2);
    aMsk->setFrameStyle(QFrame::Box);
    aMsk->setStyleSheet("QFrame{color: darkblue;}");
    aMsk->hide();

    aMskAnim= new QPropertyAnimation(aMsk,"geometry");
    aMskAnim->setEasingCurve(QEasingCurve::OutExpo);
    aMskAnim->setDuration(200);
    connect(aMskAnim,&QPropertyAnimation::finished,this, &VisualKeyboardNavigatorEventFilter::onAnimationFinished);

    QGuiApplication::instance()->installEventFilter(this);
}

void VisualKeyboardNavigatorEventFilter::onAnimationFinished()
{
    aMsk->hide();
}

void VisualKeyboardNavigatorEventFilter::onApplicationFocusChanged(QWidget *old, QWidget *now)
{
    Q_UNUSED(old)
    foreach(VisualTagInfo ti, VisualKeyboardNavigator::info()){
        if(now==0 || ti.target()->window()!=now->window()) ti.tag()->hide();
    }
}

bool VisualKeyboardNavigatorEventFilter::eventFilter(QObject *obj, QEvent *event)
{
    if (event->type() == QEvent::MouseButtonPress){
        if(isActive){
            foreach(VisualTagInfo ti, VisualKeyboardNavigator::info()) ti.tag()->hide();
            isActive= false;
        }
    }
    if (event->type() != QEvent::KeyPress) return QObject::eventFilter(obj, event);

    QKeyEvent *keyEvent = static_cast<QKeyEvent *>(event);
    int key= keyEvent->key();
    if(key==Qt::Key_F1){
        if(QApplication::focusWidget()!=0){
            //QApplication::focusWidget()->toolTip();
            //            QWhatsThis::enterWhatsThisMode();
            QWhatsThis::showText(QApplication::focusWidget()->mapToGlobal(QPoint(0,QApplication::focusWidget()->height()))
                                 ,QApplication::focusWidget()->whatsThis()
                                 ,QApplication::focusWidget());
            return true;
        }
    }
    if(keyEvent->modifiers()==Qt::ControlModifier && key==Qt::Key_Tab){
        foreach(VisualTagInfo ti, VisualKeyboardNavigator::info()){
            if(!ti.target()->isVisible()) continue;
            QWidget *w= ti.tag();
            if(w->window()!= QApplication::activeWindow()) continue;
            w->move(w->parentWidget()->mapFromGlobal(ti.target()->mapToGlobal(QPoint(0,0))));
            w->show();
            w->raise();
        }
        isActive= true;
        return true;
    }
    if(isActive){
        foreach(VisualTagInfo ti, VisualKeyboardNavigator::info()){
            QWidget *tag= ti.tag();
            tag->hide();
            if(key!=ti.key()
                    || !ti.target()->isVisible()
                    || tag->window()!= QApplication::activeWindow()) continue;
            QWidget *target= ti.target();
            target->setFocus();
            aMsk->setParent(QApplication::activeWindow());
            QRect mskG= QApplication::activeWindow()->geometry();
            mskG.moveTo(0,0);
            aMsk->setGeometry(mskG);
            aMsk->raise();
            aMsk->show();

            aMskAnim->setStartValue(aMsk->geometry());
            QRect newG= target->geometry();
            newG.moveTo(target->mapTo(target->window(), QPoint(0,0)));
            aMskAnim->setEndValue(newG);
            aMskAnim->start();
        }
        isActive= false;
        return true;
    }
    return QObject::eventFilter(obj, event);
}
