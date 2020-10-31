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
 *                                                                         *
 *   This library is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 ***************************************************************************/

#include "frameeditor.h"
#include "ui_frameeditor.h"
#include "types.h"

#include  <QMetaProperty>

FrameEditor::FrameEditor(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::FrameEditor)
{
    ui->setupUi(this);

    connect(ui->frameLeft, SIGNAL(released()), this, SLOT(buttonPressed()));
    connect(ui->frameRight, SIGNAL(released()), this, SLOT(buttonPressed()));
    connect(ui->frameTop, SIGNAL(released()), this, SLOT(buttonPressed()));
    connect(ui->frameBottom, SIGNAL(released()), this, SLOT(buttonPressed()));
    connect(ui->frameAll, SIGNAL(released()), this, SLOT(buttonPressed()));
    connect(ui->frameNone, SIGNAL(released()), this, SLOT(buttonPressed()));
}


FrameEditor::~FrameEditor()
{
    delete ui;
}


void FrameEditor::setObject(QObject *object)
{
    if (m_object == object)
        return;

    if (m_object)
        disconnect(m_object, 0, this, 0);

    m_object = object;
    m_objectList.clear();
}


void FrameEditor::setObjectList(const QObjectList &objects)
{
    setObject(0);
    m_objectList = objects;
}


void FrameEditor::setFramePropertyName(const QString &propertyName)
{
    m_framePropertyName = propertyName;
}


void FrameEditor::update()
{

    if (m_object) {
        QVariant valueVar = m_object ? m_object->property(m_framePropertyName.toLatin1()) : QVariant();
        int value = (valueVar.isValid() && valueVar.canConvert<int>()) ? valueVar.value<int>() : 0;
        setFrame(value);

        int indexFont = m_object->metaObject()->indexOfProperty(m_framePropertyName.toLatin1());
        if (m_object->metaObject()->property(indexFont).hasNotifySignal()) {
            QMetaMethod signal = m_object->metaObject()->property(indexFont).notifySignal();
            QMetaMethod slot = this->metaObject()->method(this->metaObject()->indexOfSlot("frameFromObjectProperty()"));
            connect(m_object, signal, this, slot, Qt::UniqueConnection);
        }
    } else if (m_objectList.count()) {

        bool frameFound = false;
        int value;

        foreach(QObject * object, m_objectList) {
            QVariant frameValue = object->property(m_framePropertyName.toLatin1());
            if (frameValue.isValid() && frameValue.canConvert<int>()) {
                int f = frameValue.value<int>();
                if (!frameFound) {
                    value = f;
                    frameFound = true;
                } else {
                    if ((f & CuteReport::DrawLeft) != (value & CuteReport::DrawLeft))
                        value &= ~(CuteReport::DrawLeft);
                    if ((f & CuteReport::DrawRight) != (value & CuteReport::DrawRight))
                        value &= ~(CuteReport::DrawRight);
                    if ((f & CuteReport::DrawTop) != (value & CuteReport::DrawTop))
                        value &= ~(CuteReport::DrawTop);
                    if ((f & CuteReport::DrawBottom) != (value & CuteReport::DrawBottom))
                        value &= ~(CuteReport::DrawBottom);
                }
            }
        }

        setFrame(value);

    } else {
        clear();
    }
}


void FrameEditor::clear()
{
    if (m_object)
        disconnect(m_object, 0, this, 0);
    m_object = 0;
    m_objectList.clear();
    m_framePropertyName = QString();
    setFrame(0);
}


void FrameEditor::setFrame(int frame)
{
    m_doProcessControls = false;
    ui->frameLeft->setChecked(frame & CuteReport::DrawLeft);
    ui->frameRight->setChecked(frame & CuteReport::DrawRight);
    ui->frameTop->setChecked(frame & CuteReport::DrawTop);
    ui->frameBottom->setChecked(frame & CuteReport::DrawBottom);
    m_doProcessControls = true;
}


int FrameEditor::frame()
{
//    QVariant valueVar = m_object ? m_object->property(m_framePropertyName.toLatin1()) : QVariant();
//    int value = valueVar.value<int>();
    int value = 0;
    if (ui->frameLeft->isChecked()) value |= CuteReport::DrawLeft;// else value &= (~CuteReport::DrawLeft);
    if (ui->frameRight->isChecked()) value |= CuteReport::DrawRight;// else value &= (~CuteReport::DrawRight);
    if (ui->frameTop->isChecked()) value |= CuteReport::DrawTop;// else value &= (~CuteReport::DrawTop);
    if (ui->frameBottom->isChecked()) value |= CuteReport::DrawBottom;// else value &= (~CuteReport::DrawBottom);

    return value;
}


void FrameEditor::frameToObjectProperty()
{
    if ((!m_object && m_objectList.isEmpty()) || !m_doProcessControls)
        return;

    int f = frame();

    if (m_object)
        m_object->setProperty(m_framePropertyName.toLatin1(), f);
    if (!m_objectList.isEmpty())
        foreach (QObject * object, m_objectList)
            object->setProperty(m_framePropertyName.toLatin1(), f);
}


void FrameEditor::frameFromObjectProperty()
{
    if (!m_object)
        return;

    QVariant valueVar = m_object ? m_object->property(m_framePropertyName.toLatin1()) : QVariant();
    int value = valueVar.value<int>();
    setFrame(value);
}


void FrameEditor::buttonPressed()
{
    QToolButton * button = dynamic_cast<QToolButton*>(sender());
    if (!button)
        return;

    if (button == ui->frameAll || button == ui->frameNone) {
        ui->frameLeft->setChecked(button == ui->frameAll);
        ui->frameRight->setChecked(button == ui->frameAll);
        ui->frameTop->setChecked(button == ui->frameAll);
        ui->frameBottom->setChecked(button == ui->frameAll);
    }

    frameToObjectProperty();
}
