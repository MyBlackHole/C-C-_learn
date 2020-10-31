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
#ifndef FONTEDITOR_H
#define FONTEDITOR_H

#include <QWidget>
#include <QFontDatabase>
#include <QPointer>
#include <QFont>

namespace Ui {
class FontEditor;
}

class FontEditor : public QWidget
{
    Q_OBJECT

public:
    explicit FontEditor(QWidget *parent = 0);
    ~FontEditor();

    void setObject(QObject * object);
    void setObjectList(const QObjectList & objects);
    void setFontPropertyName(const QString & fontPropertyName);
    void setColorPropertyName(const QString & colorPropertyName);
    void update();
    void clear();
    QString fontPropertyName();

    void setFont(const QFont &font);
    void setFont(const QString & familyName, qreal size, bool bold, bool italic, bool strikeout, bool underline);
    QFont font();

private slots:
    void fontToObjectProperty();
    void fontFromObjectProperty();
    void showFontDialog();
    void showFontColorDialog();

private:
    Ui::FontEditor *ui;
    QFontDatabase m_fontDatabase;
    QPointer<QObject> m_object;
    QObjectList m_objectList;
    QString m_fontPropertyName;
    QString m_colorPropertyName;
    //QFont m_font;
    bool m_doProcessControls;
};

#endif // FONTEDITOR_H
