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

#ifndef STYLEPROXYMENU_H
#define STYLEPROXYMENU_H

#include <QProxyStyle>

class StyleProxyMenu : public QProxyStyle
{
    Q_OBJECT
public:
    virtual void drawControl(ControlElement element, const QStyleOption * option, QPainter * painter, const QWidget * widget = 0) const;

signals:

public slots:

protected:
    QColor lightShade() const {
        return QColor(255, 255, 255, 90);
    }
    QColor darkShade() const {
        return QColor(0, 0, 0, 60);
    }

    QColor topShadow() const {
        return QColor(0, 0, 0, 18);
    }

    QColor innerContrastLine() const {
        return QColor(255, 255, 255, 30);
    }

    // On mac we want a standard blue color used when the system palette is used
    bool isMacSystemPalette(const QPalette &pal) const {
        Q_UNUSED(pal);
#if defined(Q_OS_MACX)
        const QPalette *themePalette = QGuiApplicationPrivate::platformTheme()->palette();
        if (themePalette && themePalette->color(QPalette::Normal, QPalette::Highlight) ==
                pal.color(QPalette::Normal, QPalette::Highlight) &&
                themePalette->color(QPalette::Normal, QPalette::HighlightedText) ==
                pal.color(QPalette::Normal, QPalette::HighlightedText))
            return true;
#endif
        return false;
    }

    QColor highlight(const QPalette &pal) const {
        if (isMacSystemPalette(pal))
            return QColor(60, 140, 230);
        return pal.color(QPalette::Highlight);
    }

    QColor highlightedText(const QPalette &pal) const {
        if (isMacSystemPalette(pal))
            return Qt::white;
        return pal.color(QPalette::HighlightedText);
    }

    QColor outline(const QPalette &pal) const {
        if (!pal.window().texture().isNull())
            return QColor(0, 0, 0, 160);
        return pal.background().color().darker(140);
    }

    QColor highlightedOutline(const QPalette &pal) const {
        QColor highlightedOutline = highlight(pal).darker(125);
        if (highlightedOutline.value() > 160)
            highlightedOutline.setHsl(highlightedOutline.hue(), highlightedOutline.saturation(), 160);
        return highlightedOutline;
    }

    QColor tabFrameColor(const QPalette &pal) const {
        if (!pal.button().texture().isNull())
            return QColor(255, 255, 255, 8);
        return buttonColor(pal).lighter(104);
    }

    QColor buttonColor(const QPalette &pal) const {
        QColor buttonColor = pal.button().color();
        int val = qGray(buttonColor.rgb());
        buttonColor = buttonColor.lighter(100 + qMax(1, (180 - val)/6));
        buttonColor.setHsv(buttonColor.hue(), buttonColor.saturation() * 0.75, buttonColor.value());
        return buttonColor;
    }

    enum {
        menuItemHMargin      =  3, // menu item hor text margin
        menuArrowHMargin     =  6, // menu arrow horizontal margin
        menuRightBorder      = 15, // right border on menus
        menuCheckMarkWidth   = 12  // checkmarks width on menus
    };

    static QColor mergedColors(const QColor &colorA, const QColor &colorB, int factor = 50)
    {
        const int maxFactor = 100;
        QColor tmp = colorA;
        tmp.setRed((tmp.red() * factor) / maxFactor + (colorB.red() * (maxFactor - factor)) / maxFactor);
        tmp.setGreen((tmp.green() * factor) / maxFactor + (colorB.green() * (maxFactor - factor)) / maxFactor);
        tmp.setBlue((tmp.blue() * factor) / maxFactor + (colorB.blue() * (maxFactor - factor)) / maxFactor);
        return tmp;
    }
};

#endif // STYLEPROXYMENU_H
