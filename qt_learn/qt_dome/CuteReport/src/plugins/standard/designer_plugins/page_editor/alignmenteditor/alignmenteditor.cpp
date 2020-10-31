/***************************************************************************
 *   This file is part of the CuteReport project                           *
 *   Copyright (C) 2012-2014 by Alexander Mikhalov                         *
 *   alexander.mikhalov@gmail.com                                          *
 *                                                                         *
 **                   GNU General Public License Usage                    **
 *                                                                         *
 *   This library is free software: you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation, either version 3 of the License, or     *
 *   (at your option) any later version.                                   *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program.  If not, see <http://www.gnu.org/licenses/>. *
 *                                                                         *
 **                  GNU Lesser General Public License                    **
 *                                                                         *
 *   This library is free software: you can redistribute it and/or modify  *
 *   it under the terms of the GNU Lesser General Public License as        *
 *   published by the Free Software Foundation, either version 3 of the    *
 *   License, or (at your option) any later version.                       *
 *   You should have received a copy of the GNU Lesser General Public      *
 *   License along with this library.                                      *
 *   If not, see <http://www.gnu.org/licenses/>.                           *
 *                                                                         *
 *   This library is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 ***************************************************************************/
#include "alignmenteditor.h"
#include "ui_alignmenteditor.h"

#include  <QMetaProperty>
#include <QDebug>

AlignmentEditor::AlignmentEditor(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::AlignmentEditor)
  ,m_doProcessControls(true)
{
    ui->setupUi(this);

    connect(ui->alignLeft, SIGNAL(released()), this, SLOT(buttonPressed()));
    connect(ui->alignRight, SIGNAL(released()), this, SLOT(buttonPressed()));
    connect(ui->alignCenter, SIGNAL(released()), this, SLOT(buttonPressed()));
    connect(ui->alignJustify, SIGNAL(released()), this, SLOT(buttonPressed()));
    connect(ui->alignTop, SIGNAL(released()), this, SLOT(buttonPressed()));
    connect(ui->alignBottom, SIGNAL(released()), this, SLOT(buttonPressed()));
    connect(ui->alignMiddle, SIGNAL(released()), this, SLOT(buttonPressed()));
    connect(ui->wordWrap, SIGNAL(released()), this, SLOT(buttonPressed()));
}


AlignmentEditor::~AlignmentEditor()
{
    delete ui;
}


void AlignmentEditor::setObject(QObject *object)
{
    if (m_object == object)
        return;

    if (m_object)
        disconnect(m_object, 0, this, 0);

    m_object = object;
    m_objectList.clear();
}


void AlignmentEditor::setObjectList(const QObjectList &objects)
{
    setObject(0);
    m_objectList = objects;
}


void AlignmentEditor::setAlignPropertyName(const QString &propertyName)
{
    m_alignPropertyName = propertyName;
}


void AlignmentEditor::update()
{
    if (m_object) {

        QVariant valueVar = m_object ? m_object->property(m_alignPropertyName.toLatin1()) : QVariant();
        int value = (valueVar.isValid() && valueVar.canConvert<int>()) ? valueVar.value<int>() : 0;
        setAlignment(value);

        int indexFont = m_object->metaObject()->indexOfProperty(m_alignPropertyName.toLatin1());
        if (m_object->metaObject()->property(indexFont).hasNotifySignal()) {
            QMetaMethod signal = m_object->metaObject()->property(indexFont).notifySignal();
            QMetaMethod slot = this->metaObject()->method(this->metaObject()->indexOfSlot("alignmentFromObjectProperty()"));
            connect(m_object, signal, this, slot, Qt::UniqueConnection);
        }

    }  else if (m_objectList.count()) {

        bool alignFound = false;
        int value;

        foreach(QObject * object, m_objectList) {
            QVariant alignValue = object->property(m_alignPropertyName.toLatin1());
            if (alignValue.isValid() && alignValue.canConvert<int>()) {
                int a = alignValue.value<int>();
                if (!alignFound) {
                    value = a;
                    alignFound = true;
                } else {
                    if ((a & Qt::AlignLeft) != (value & Qt::AlignLeft))
                        value &= ~(Qt::AlignLeft);
                    if ((a & Qt::AlignRight) != (value & Qt::AlignRight))
                        value &= ~(Qt::AlignRight);
                    if ((a & Qt::AlignHCenter) != (value & Qt::AlignHCenter))
                        value &= ~(Qt::AlignHCenter);
                    if ((a & Qt::AlignJustify) != (value & Qt::AlignJustify))
                        value &= ~(Qt::AlignJustify);
                    if ((a & Qt::AlignTop) != (value & Qt::AlignTop))
                        value &= ~(Qt::AlignTop);
                    if ((a & Qt::AlignBottom) != (value & Qt::AlignBottom))
                        value &= ~(Qt::AlignBottom);
                    if ((a & Qt::AlignVCenter) != (value & Qt::AlignVCenter))
                        value &= ~(Qt::AlignVCenter);
                    if ((a & Qt::TextWordWrap) != (value & Qt::TextWordWrap))
                        value &= ~(Qt::TextWordWrap);
                }
            }
        }

        setAlignment(value);

    } else {
        clear();
    }
}


void AlignmentEditor::clear()
{
    if (m_object)
        disconnect(m_object, 0, this, 0);
    m_object = 0;
    m_objectList.clear();
    m_alignPropertyName = QString();
    setAlignment(0);
}


void AlignmentEditor::setAlignment(int alignment)
{
    m_doProcessControls = false;
    ui->alignLeft->setChecked(alignment & Qt::AlignLeft);
    ui->alignRight->setChecked(alignment & Qt::AlignRight);
    ui->alignCenter->setChecked(alignment & Qt::AlignHCenter);
    ui->alignJustify->setChecked(alignment & Qt::AlignJustify);
    ui->alignTop->setChecked(alignment & Qt::AlignTop);
    ui->alignBottom->setChecked(alignment & Qt::AlignBottom);
    ui->alignMiddle->setChecked(alignment & Qt::AlignVCenter);
    ui->wordWrap->setChecked(alignment & Qt::TextWordWrap);
    m_doProcessControls = true;
}


int AlignmentEditor::alignment()
{
//    QVariant valueVar = m_object ? m_object->property(m_alignPropertyName.toLatin1()) : QVariant();
//    int value = valueVar.value<int>();
    int value = 0;
    if (ui->alignLeft->isChecked()) value |= Qt::AlignLeft; //else value &= (~Qt::AlignLeft);
    if (ui->alignRight->isChecked()) value |= Qt::AlignRight; //else value &= (~Qt::AlignRight);
    if (ui->alignCenter->isChecked()) value |= Qt::AlignHCenter; //else value &= (~Qt::AlignHCenter);
    if (ui->alignJustify->isChecked()) value |= Qt::AlignJustify; //else value &= (~Qt::AlignJustify);
    if (ui->alignTop->isChecked()) value |= Qt::AlignTop; //else value &= (~Qt::AlignTop);
    if (ui->alignBottom->isChecked()) value |= Qt::AlignBottom; //else value &= (~Qt::AlignBottom);
    if (ui->alignMiddle->isChecked()) value |= Qt::AlignVCenter; //else value &= (~Qt::AlignVCenter);
    if (ui->wordWrap->isChecked()) value |= Qt::TextWordWrap; //else value &= (~Qt::TextWordWrap);

    return value;
}


void AlignmentEditor::alignmentToObjectProperty()
{
    if ((!m_object && m_objectList.isEmpty()) || !m_doProcessControls)
        return;

    int a = alignment();

    if (m_object)
        m_object->setProperty(m_alignPropertyName.toLatin1(), a);
    if (!m_objectList.isEmpty())
        foreach (QObject * object, m_objectList)
            object->setProperty(m_alignPropertyName.toLatin1(), a);
}


void AlignmentEditor::alignmentFromObjectProperty()
{
    if (!m_object)
        return;

    QVariant valueVar = m_object ? m_object->property(m_alignPropertyName.toLatin1()) : QVariant();
    int value = valueVar.value<int>();
    setAlignment(value);
}


void AlignmentEditor::buttonPressed()
{
    QToolButton * button = dynamic_cast<QToolButton*>(sender());
    if (!button)
        return;

    if (button == ui->alignLeft || button == ui->alignRight || button == ui->alignCenter || button == ui->alignJustify) {
        if (button != ui->alignLeft) ui->alignLeft->setChecked(false);
        if (button != ui->alignRight) ui->alignRight->setChecked(false);
        if (button != ui->alignCenter) ui->alignCenter->setChecked(false);
        if (button != ui->alignJustify) ui->alignJustify->setChecked(false);
        button->setChecked(true);
    }  else if (button == ui->alignTop || button == ui->alignBottom || button == ui->alignMiddle) {
        if (button != ui->alignTop) ui->alignTop->setChecked(false);
        if (button != ui->alignBottom) ui->alignBottom->setChecked(false);
        if (button != ui->alignMiddle) ui->alignMiddle->setChecked(false);
        button->setChecked(true);
    } /*else if (button == ui->wordWrap)
        button->setChecked(!button->isChecked());*/

    alignmentToObjectProperty();
}
