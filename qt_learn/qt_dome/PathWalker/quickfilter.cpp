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

#include <QLineEdit>
#include <QHBoxLayout>
#include <QKeyEvent>
#include <QPoint>
#include <QCompleter>
#include <QTimer>
#include <QStringListModel>
#include <QSettings>
#include <QApplication>
#include <QtDebug>

#include "quickfilter.h"
#include "settingsstore/settingsstore.h"

QuickFilter::QuickFilter(QWidget *parent) :
    ISettings(parent), aCompleter(0)
{
    setObjectName("QuickSearch");
    aPatt= new QLineEdit();
    aPatt->setPlaceholderText(tr("Quick filter"));
    setFocusProxy(aPatt);
    aPatt->installEventFilter(this);

    setAutoFillBackground(true);
    setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);

    QStringListModel *mdl= new QStringListModel();
    mdl->setStringList(QStringList()<<"*"<<"*.txt"<<"*.log");

    aCompleter= new QCompleter();
    aPatt->setCompleter(aCompleter);
    aCompleter->setModel(mdl);
    aPatt->setCompleter(aCompleter);

    aNotifyTimer= new QTimer();
    aNotifyTimer->setSingleShot(true);
    aNotifyTimer->setInterval(1500);

    doInitLayout();
    doInitConnection();
}

bool QuickFilter::settingsSave()
{
    QSettings cfg(SettingsStore::cfgPath(),QSettings::IniFormat);
    cfg.setValue(objectName()+"/lastPattern",aPatt->text());
    QStringList cLst= completerModel()->stringList();
    cLst.removeDuplicates();
    while(cLst.count()>=8) cLst.removeLast();
    cfg.setValue(objectName()+"/completionList",cLst);
    return true;
}

bool QuickFilter::settingsRestore()
{
    QSettings cfg(SettingsStore::cfgPath(),QSettings::IniFormat);
    qobject_cast<QStringListModel*>(aCompleter->model())->setStringList(cfg.value(objectName()+"/completionList").toStringList());
    return true;
}

QString QuickFilter::pattern()
{
    return aPatt->text();
}

bool QuickFilter::eventFilter(QObject *watched, QEvent *event)
{
    if(event->type()==QEvent::KeyPress){
        QKeyEvent *k= static_cast<QKeyEvent*>(event);
        if(watched==aPatt){
            if(k->key()==Qt::Key_Return){
                aNotifyTimer->stop();
                if(!aPatt->text().isEmpty() && !completerModel()->stringList().contains(aPatt->text(),Qt::CaseInsensitive)){
                    completerModel()->insertRow(0);
                    completerModel()->setData(completerModel()->index(0,0),QVariant::fromValue(aPatt->text()));
                    if(completerModel()->rowCount()>7) completerModel()->removeRows(7,completerModel()->rowCount()-8);
                } aPatt->setStyleSheet("");

                if(aLastAppliedPatt!=aPatt->text()){
                    aLastAppliedPatt= aPatt->text();
                    emit searching(aPatt->text());
                }
                return true;
            }
            if(k->key()==Qt::Key_Escape){
                aNotifyTimer->stop();
                aPatt->setText("");
                emit searching("");
                hide();
                return true;
            }
            if(k->modifiers()==Qt::ControlModifier && k->key()==Qt::Key_Space){
                aCompleter->complete();
                return true;
            }
            if(k->key()==Qt::Key_Tab)
            {
                keyPressEvent(k);
                return true;
            }
        }
    }
    return false;
}

QStringListModel *QuickFilter::completerModel()
{
    return qobject_cast<QStringListModel*>(aCompleter->model());
}

void QuickFilter::patternAppend(const QString &text)
{
    aPatt->setText(aPatt->text()+text);
    aNotifyTimer->start();
}

void QuickFilter::patternClear()
{
    aPatt->clear();
    aNotifyTimer->start();
}

void QuickFilter::onNotifyTimer()
{
    aNotifyTimer->stop();
    if(!aPatt->text().isEmpty() && !completerModel()->stringList().contains(aPatt->text(),Qt::CaseInsensitive)){
        aPatt->setStyleSheet("QLineEdit{background: lightgreen;}");
    }else aPatt->setStyleSheet("");

    if(aLastAppliedPatt==aPatt->text()) return;
    aLastAppliedPatt= aPatt->text();
    emit searching(aPatt->text());
}

void QuickFilter::cancel()
{
    hide();
    aPatt->clear();
    onNotifyTimer();
}

void QuickFilter::doInitLayout()
{
    QHBoxLayout *lt= new QHBoxLayout();
    lt->addWidget(aPatt,1);
    lt->setMargin(0);
    setLayout(lt);
}

void QuickFilter::doInitConnection()
{
    connect(aPatt, &QLineEdit::textEdited, this, &QuickFilter::onNotifyTimer);
    connect(aCompleter, SIGNAL(activated(QString)), this, SLOT(onNotifyTimer()));
    connect(aNotifyTimer, &QTimer::timeout, this, &QuickFilter::onNotifyTimer);
}
