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

#include <QPushButton>
#include <QLabel>
#include <QGridLayout>
#include <QApplication>

#include "aboutpathwalker.h"

AboutPathWalker::AboutPathWalker(QWidget *parent) :
    QDialog(parent)
{
    QLabel *ico= new QLabel;
    ico->setPixmap(QPixmap("://img/folders.png"));
    QLabel *title= new QLabel("<b>Path Walker</b> <a href=\"https://code.google.com/p/path-walker/\"><img src=\":/img/shortcut.png\"></a>");
    title->setTextFormat(Qt::RichText);
    title->setOpenExternalLinks(true);
    title->setTextInteractionFlags(Qt::LinksAccessibleByMouse);
    QLabel *author= new QLabel("Author: <b>Denis Kvita</b> (<a href=\"mailto:dkvita@gmail.com\">dkvita@gmail.com</a>)");
    author->setTextFormat(Qt::RichText);
    author->setOpenExternalLinks(true);
    author->setTextInteractionFlags(Qt::LinksAccessibleByMouse);

    QLabel *fugue= new QLabel("Some Icons are Copyright© <a href=\"http://p.yusukekamiyamane.com\">Yusuke Kamiyamane</a>.");
    fugue->setTextFormat(Qt::RichText);
    fugue->setOpenExternalLinks(true);
    fugue->setTextInteractionFlags(Qt::LinksAccessibleByMouse);

    QLabel *qt= new QLabel("Powered by <a href=\"http://qt.digia.com/\">Qt 5.4.0</a>.");
    qt->setTextFormat(Qt::RichText);
    qt->setOpenExternalLinks(true);
    qt->setTextInteractionFlags(Qt::LinksAccessibleByMouse);

    QPushButton *ok= new QPushButton("Close");
    connect(ok, &QPushButton::clicked, this, &QDialog::accept);

    QHBoxLayout *clt= new QHBoxLayout;
    clt->addStretch(1);
    clt->addWidget(ok);
    clt->addStretch(1);

    QGridLayout *alt= new QGridLayout;
    alt->addWidget(ico, 0,0);
    alt->addWidget(title, 0,1);
    alt->addWidget(author, 1,0, 1,2);
    alt->addWidget(fugue, 2,0, 1,2);
    alt->addWidget(qt, 3,0, 1,2);
    alt->addLayout(clt, 4,0, 1,2);
    alt->setColumnStretch(1,1);

    setLayout(alt);
    resize(350,150);
}
