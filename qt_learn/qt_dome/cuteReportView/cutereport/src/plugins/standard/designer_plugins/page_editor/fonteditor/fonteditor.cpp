/***************************************************************************
 *   This file is part of the CuteReport project                           *
 *   Copyright (C) 2012-2015 by Alexander Mikhalov                         *
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
#include "fonteditor.h"
#include "ui_fonteditor.h"
#include "pageinterface.h"

#include <QFontDialog>
#include <QColorDialog>

FontEditor::FontEditor(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::FontEditor)
  ,m_doProcessControls(true)
{
    ui->setupUi(this);
    ui->family->addItems(m_fontDatabase.families());
    connect(ui->family, SIGNAL(currentIndexChanged(int)), this, SLOT(fontToObjectProperty()));
    connect(ui->size, SIGNAL(valueChanged(double)), this, SLOT(fontToObjectProperty()));
    connect(ui->bold, SIGNAL(toggled(bool)), this, SLOT(fontToObjectProperty()));
    connect(ui->italic, SIGNAL(toggled(bool)), this, SLOT(fontToObjectProperty()));
    connect(ui->strikeout, SIGNAL(toggled(bool)), this, SLOT(fontToObjectProperty()));
    connect(ui->underline, SIGNAL(toggled(bool)), this, SLOT(fontToObjectProperty()));
    connect(ui->fontDialog, SIGNAL(clicked()), this, SLOT(showFontDialog()));
    connect(ui->fontColor, SIGNAL(clicked()), this, SLOT(showFontColorDialog()));
}


FontEditor::~FontEditor()
{
    delete ui;
}


void FontEditor::setObject(QObject *object)
{
    if (m_object == object)
        return;

    if (m_object)
        disconnect(m_object, 0, this, 0);

    m_object = object;
    m_objectList.clear();
}


void FontEditor::setObjectList(const QObjectList &objects)
{
    setObject(0);
    m_objectList = objects;
}


void FontEditor::setFontPropertyName(const QString &fontPropertyName)
{
    m_fontPropertyName = fontPropertyName;
}


void FontEditor::setColorPropertyName(const QString &colorPropertyName)
{
    m_colorPropertyName = colorPropertyName;
}


void FontEditor::update()
{
    if (m_object) {

        QVariant valueFont = m_object ? m_object->property(m_fontPropertyName.toLatin1()) : QVariant();
        setFont((valueFont.isValid() && valueFont.canConvert<QFont>()) ? valueFont.value<QFont>() : QFont());

        int indexFont = m_object->metaObject()->indexOfProperty(m_fontPropertyName.toLatin1());
        if (m_object->metaObject()->property(indexFont).hasNotifySignal()) {
            QMetaMethod signal = m_object->metaObject()->property(indexFont).notifySignal();
            QMetaMethod slot = this->metaObject()->method(this->metaObject()->indexOfSlot("fontFromObjectProperty()"));
            connect(m_object, signal, this, slot, Qt::UniqueConnection);
        }

        QVariant currentColorVar = m_object->property(m_colorPropertyName.toLatin1());
        ui->fontColor->setEnabled(currentColorVar.canConvert<QColor>());

    } else if (m_objectList.count()) {

        QString family;
        qreal size = -1;
        bool bold = false;
        bool italic = false;
        bool strikeout = false;
        bool underline = false;
        bool fontFound = false;
        bool colorFound = false;
        foreach(QObject * object, m_objectList) {
            QVariant fontValue = object->property(m_fontPropertyName.toLatin1());
            QVariant colorValue = object->property(m_colorPropertyName.toLatin1());
            if (fontValue.isValid() && fontValue.canConvert<QFont>()) {
                QFont f = fontValue.value<QFont>();
                if (!fontFound) {
                    family = f.family();
                    size = f.pointSizeF();
                    bold = f.bold();
                    italic = f.italic();
                    strikeout = f.strikeOut();
                    underline = f.underline();
                    fontFound = true;
                } else {
                    if (f.family() != family) family = QString();
                    if (f.pointSizeF() != size) size = -1;
                    if (f.bold() != bold) bold = false;
                    if (f.italic() != italic) italic = false;
                    if (f.strikeOut() != strikeout) strikeout = false;
                    if (f.underline() != underline) underline = false;
                }
            }
            if (!colorFound && colorValue.isValid() && colorValue.canConvert<QColor>()) {
                colorFound = true;
            }

        }

        setFont(family, size, bold, italic, strikeout, underline);
        ui->fontColor->setEnabled(colorFound);

    } else {
        clear();
    }
}


void FontEditor::clear()
{
    if (m_object)
        disconnect(m_object, 0, this, 0);
    m_object = 0;
    m_objectList.clear();
    m_fontPropertyName = QString();
    m_colorPropertyName = QString();
    m_doProcessControls = false;

    ui->family->setCurrentIndex(-1);
    ui->size->setValue(-1);
    ui->bold->setChecked(false);
    ui->italic->setChecked(false);
    ui->strikeout->setChecked(false);
    ui->underline->setChecked(false);
    m_doProcessControls = true;
}


QString FontEditor::fontPropertyName()
{
    return m_fontPropertyName;
}


void FontEditor::setFont(const QFont & font)
{
    setFont(font.family(), font.pointSizeF(), font.bold(), font.italic(), font.strikeOut(), font.underline());
}


void FontEditor::setFont(const QString & familyName, qreal size, bool bold, bool italic, bool strikeout, bool underline)
{
    m_doProcessControls = false;
    ui->family->setCurrentIndex(familyName.isEmpty() ? -1 : ui->family->findText(familyName));
    ui->size->setValue(size);
    ui->bold->setChecked(bold);
    ui->italic->setChecked(italic);
    ui->strikeout->setChecked(strikeout);
    ui->underline->setChecked(underline);
    m_doProcessControls = true;
}


QFont FontEditor::font()
{
    QFont font;
    if (!m_object && m_objectList.isEmpty())
        return font;
    font.setFamily(ui->family->currentText());
    font.setPointSizeF(ui->size->value());
    font.setBold(ui->bold->isChecked());
    font.setItalic(ui->italic->isChecked());
    font.setStrikeOut(ui->strikeout->isChecked());
    font.setUnderline(ui->underline->isChecked());
    return font;
}


void FontEditor::fontToObjectProperty()
{
    if ((!m_object && m_objectList.isEmpty()) || !m_doProcessControls)
        return;

    QFont f = font();

    if (m_object)
        m_object->setProperty(m_fontPropertyName.toLatin1(), f);
    if (!m_objectList.isEmpty())
        foreach (QObject * object, m_objectList)
            object->setProperty(m_fontPropertyName.toLatin1(), f);
}


void FontEditor::fontFromObjectProperty()
{
    if (!m_object)
        return;

    QVariant value = m_object ? m_object->property(m_fontPropertyName.toLatin1()) : QVariant();
    setFont(value.value<QFont>());
}


void FontEditor::showFontDialog()
{
    bool ok;
    QFont font = QFontDialog::getFont(&ok, this->font(), this);
    if (ok) {
        setFont(font);
        fontToObjectProperty();
    }
}


void FontEditor::showFontColorDialog()
{
    if (!m_object && m_objectList.isEmpty())
        return;

    QVariant currentColorVar;
    if (m_object)
        currentColorVar = m_object->property(m_colorPropertyName.toLatin1());
    else
        currentColorVar = m_objectList.at(0)->property(m_colorPropertyName.toLatin1());

    if (!currentColorVar.canConvert<QColor>())
        return;
    QColor currentColor = currentColorVar.value<QColor>();
    QColor color = QColorDialog::getColor (currentColor);
    if (color.isValid()) {
        if (m_object)
            m_object->setProperty(m_colorPropertyName.toLatin1(), color);
        if (!m_objectList.isEmpty())
            foreach (QObject * object, m_objectList)
                object->setProperty(m_colorPropertyName.toLatin1(), color);
    }

}


