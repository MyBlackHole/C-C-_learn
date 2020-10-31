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

#include <QtGui>
#include <QToolButton>
#include <QLabel>
#include <QMovie>
#include <QHBoxLayout>

#include "busydialog.h"

BusyDialog::BusyDialog(const QString &text, QWidget *parent) :
    QFrame(parent)
{
    setObjectName("busy");

    setAutoFillBackground(true);

    aCancel= new QToolButton;
    aCancel->setIcon(QIcon(":img/cross-button.png"));
    aCancel->setAutoRaise(true);
    aCancel->setToolTip(tr("stop"));

    QLabel *label= new QLabel;
    aSpinner = new QMovie(":img/busy-small.gif");
    label->setMovie(aSpinner);

    aMessage= new QLabel(text);

    QHBoxLayout *lt= new QHBoxLayout;
    lt->addWidget(aCancel);
    lt->addWidget(label);
    lt->addWidget(aMessage,1);
    lt->addSpacing(5);
    lt->setMargin(2);
    setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);

    setLayout(lt);

    connect(aCancel, &QToolButton::clicked,this, &BusyDialog::onCancelClicked);
    connect(aCancel, &QToolButton::clicked,this, &BusyDialog::canceled);

    setStyleSheet("QFrame#busy{border-width: 1px; "
                  "border-style: solid; "
                  "border-color: darkgray;"
                  "border-bottom-right-radius: 7px; "
                  "background-color: white;}");

    setFocusProxy(aCancel);
}

void BusyDialog::messageSet(const QString &text)
{
    aMessage->setText(text);
}


void BusyDialog::start(bool unbreakable)
{
    if(unbreakable) aCancel->hide();
    else aCancel->show();
    aCancel->setEnabled(true);
    aSpinner->start();
    resize(sizeHint());
}

void BusyDialog::stop()
{
    aSpinner->stop();
}

void BusyDialog::onCancelClicked()
{
    aCancel->setEnabled(false);
}
