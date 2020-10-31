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

#include <QPainter>
#include <QStyleOption>
#include <QFont>
#include <QColor>
#include <QComboBox>

#include "styleproxymenu.h"

// from windows style
static const int windowsItemFrame        =  2; // menu item frame width
static const int windowsItemHMargin      =  3; // menu item hor text margin
static const int windowsItemVMargin      =  8; // menu item ver text margin
static const int windowsRightBorder      = 15; // right border on windows

//static const int groupBoxBottomMargin    =  0;  // space below the groupbox
//static const int groupBoxTopMargin       =  3;

void StyleProxyMenu::drawControl(QStyle::ControlElement element, const QStyleOption *option, QPainter *painter, const QWidget *widget) const
{
    // paint funny menu item
    if(element==CE_MenuItem){
        //QRect rect = option->rect;
        //QColor outlineC = outline(option->palette);
        QColor highlightedOutlineC = highlightedOutline(option->palette);
        QColor shadow = darkShade();
        painter->save();
        // Draws one item in a popup menu.
        if (const QStyleOptionMenuItem *menuItem = qstyleoption_cast<const QStyleOptionMenuItem *>(option)) {
            QColor highlightOutline = highlightedOutlineC;
            QColor highlight = option->palette.highlight().color();
            if (menuItem->menuItemType == QStyleOptionMenuItem::Separator) {
                int w = 0;
                if (!menuItem->text.isEmpty()) {
                    painter->setFont(menuItem->font);
                    proxy()->drawItemText(painter, menuItem->rect.adjusted(5, 0, -5, 0), Qt::AlignLeft | Qt::AlignVCenter,
                                          menuItem->palette, menuItem->state & State_Enabled, menuItem->text,
                                          QPalette::Text);
                    w = menuItem->fontMetrics.width(menuItem->text) + 5;
                }
                painter->setPen(shadow.lighter(106));
                bool reverse = menuItem->direction == Qt::RightToLeft;
                painter->drawLine(menuItem->rect.left() + 5 + (reverse ? 0 : w), menuItem->rect.center().y(),
                                  menuItem->rect.right() - 5 - (reverse ? w : 0), menuItem->rect.center().y());
                painter->restore();
                return;
            }
            bool selected = menuItem->state & State_Selected && menuItem->state & State_Enabled;
            if (selected) {
                QRect r = option->rect;
                painter->fillRect(r, highlight);
                painter->setPen(QPen(highlightOutline));
                const QLine lines[4] = {
                    QLine(QPoint(r.left() + 1, r.bottom()), QPoint(r.right() - 1, r.bottom())),
                    QLine(QPoint(r.left() + 1, r.top()), QPoint(r.right() - 1, r.top())),
                    QLine(QPoint(r.left(), r.top()), QPoint(r.left(), r.bottom())),
                    QLine(QPoint(r.right() , r.top()), QPoint(r.right(), r.bottom())),
                };
                painter->drawLines(lines, 4);
            }
            bool checkable = menuItem->checkType != QStyleOptionMenuItem::NotCheckable;
            bool checked = menuItem->checked;
            bool sunken = menuItem->state & State_Sunken;
            bool enabled = menuItem->state & State_Enabled;

            bool ignoreCheckMark = false;
            int checkcol = qMax(menuItem->maxIconWidth, 20);

            if (qobject_cast<const QComboBox*>(widget))
                ignoreCheckMark = true; //ignore the checkmarks provided by the QComboMenuDelegate

            if (!ignoreCheckMark) {
                // Check
                QRect checkRect(option->rect.left() + 7, option->rect.center().y() - 6, 14, 14);
                checkRect = visualRect(menuItem->direction, menuItem->rect, checkRect);
                if (checkable) {
                    if (menuItem->checkType & QStyleOptionMenuItem::Exclusive) {
                        // Radio button
                        if (checked || sunken) {
                            painter->setRenderHint(QPainter::Antialiasing);
                            painter->setPen(Qt::NoPen);

                            QPalette::ColorRole textRole = !enabled ? QPalette::Text:
                                                                      selected ? QPalette::HighlightedText : QPalette::ButtonText;
                            painter->setBrush(option->palette.brush( option->palette.currentColorGroup(), textRole));
                            painter->drawEllipse(checkRect.adjusted(4, 4, -4, -4));
                        }
                    } else {
                        // Check box
                        if (menuItem->icon.isNull()) {
                            QStyleOptionButton box;
                            box.QStyleOption::operator=(*option);
                            box.rect = checkRect;
                            if (checked)
                                box.state |= State_On;
                            proxy()->drawPrimitive(PE_IndicatorCheckBox, &box, painter, widget);
                        }
                    }
                }
            } else { //ignore checkmark
                if (menuItem->icon.isNull())
                    checkcol = 0;
                else
                    checkcol = menuItem->maxIconWidth;
            }

            // Text and icon, ripped from windows style
            bool dis = !(menuItem->state & State_Enabled);
            bool act = menuItem->state & State_Selected;
            const QStyleOption *opt = option;
            const QStyleOptionMenuItem *menuitem = menuItem;

            QPainter *p = painter;
            QRect vCheckRect = visualRect(opt->direction, menuitem->rect,
                                          QRect(menuitem->rect.x() + 4, menuitem->rect.y(),
                                                checkcol, menuitem->rect.height()));
            if (!menuItem->icon.isNull()) {
                QIcon::Mode mode = dis ? QIcon::Disabled : QIcon::Normal;
                if (act && !dis)
                    mode = QIcon::Active;
                QPixmap pixmap;

                int smallIconSize = proxy()->pixelMetric(PM_SmallIconSize, option, widget);
                QSize iconSize(smallIconSize, smallIconSize);
                if (const QComboBox *combo = qobject_cast<const QComboBox*>(widget))
                    iconSize = combo->iconSize();
                if (checked)
                    pixmap = menuItem->icon.pixmap(iconSize, mode, QIcon::On);
                else
                    pixmap = menuItem->icon.pixmap(iconSize, mode);

                int pixw = pixmap.width();
                int pixh = pixmap.height();

                QRect pmr(0, 0, pixw, pixh);
                pmr.moveCenter(vCheckRect.center());
                painter->setPen(menuItem->palette.text().color());
                if (checkable && checked) {
                    QStyleOption opt = *option;
                    if (act) {
                        QColor activeColor = mergedColors(option->palette.background().color(),
                                                          option->palette.highlight().color());
                        opt.palette.setBrush(QPalette::Button, activeColor);
                    }
                    opt.state |= State_Sunken;
                    opt.rect = vCheckRect;
                    proxy()->drawPrimitive(PE_PanelButtonCommand, &opt, painter, widget);
                }
                painter->drawPixmap(pmr.topLeft(), pixmap);
            }
            if (selected) {
                painter->setPen(menuItem->palette.highlightedText().color());
            } else {
                painter->setPen(menuItem->palette.text().color());
            }
            int x, y, w, h;
            menuitem->rect.getRect(&x, &y, &w, &h);
            int tab = menuitem->tabWidth;
            QColor discol;
            if (dis) {
                discol = menuitem->palette.text().color();
                p->setPen(discol);
            }
            int xm = windowsItemFrame + checkcol + windowsItemHMargin + 2;
            int xpos = menuitem->rect.x() + xm;

            QRect textRect(xpos, y + windowsItemVMargin, w - xm - windowsRightBorder - tab + 1, h - 2 * windowsItemVMargin);
            QRect vTextRect = visualRect(opt->direction, menuitem->rect, textRect);
            QString s = menuitem->text;
            if (!s.isEmpty()) {                     // draw text
                p->save();
                int t = s.indexOf(QLatin1Char('\t'));
                int text_flags = Qt::AlignVCenter | Qt::TextShowMnemonic | Qt::TextDontClip | Qt::TextSingleLine;
                if (!styleHint(SH_UnderlineShortcut, menuitem, widget))
                    text_flags |= Qt::TextHideMnemonic;
                text_flags |= Qt::AlignLeft;
                if (t >= 0) {
                    QRect vShortcutRect = visualRect(opt->direction, menuitem->rect,
                                                     QRect(textRect.topRight(), QPoint(menuitem->rect.right(), textRect.bottom())));
                    if (dis && !act && proxy()->styleHint(SH_EtchDisabledText, option, widget)) {
                        p->setPen(menuitem->palette.light().color());
                        p->drawText(vShortcutRect.adjusted(1, 1, 1, 1), text_flags, s.mid(t + 1));
                        p->setPen(discol);
                    }
                    p->drawText(vShortcutRect, text_flags, s.mid(t + 1));
                    s = s.left(t);
                }
                QFont font = menuitem->font;
                // font may not have any "hard" flags set. We override
                // the point size so that when it is resolved against the device, this font will win.
                // This is mainly to handle cases where someone sets the font on the window
                // and then the combo inherits it and passes it onward. At that point the resolve mask
                // is very, very weak. This makes it stonger.
                font.setPointSizeF(QFontInfo(menuItem->font).pointSizeF());

                if (menuitem->menuItemType == QStyleOptionMenuItem::DefaultItem)
                    font.setBold(true);

                p->setFont(font);
                if (dis && !act && proxy()->styleHint(SH_EtchDisabledText, option, widget)) {
                    p->setPen(menuitem->palette.light().color());
                    p->drawText(vTextRect.adjusted(1, 1, 1, 1), text_flags, s.left(t));
                    p->setPen(discol);
                }
                p->drawText(vTextRect, text_flags, s.left(t));
                p->restore();
            }

            // Arrow
            if (menuItem->menuItemType == QStyleOptionMenuItem::SubMenu) {// draw sub menu arrow
                int dim = (menuItem->rect.height() - 4) / 2;
                PrimitiveElement arrow;
                arrow = option->direction == Qt::RightToLeft ? PE_IndicatorArrowLeft : PE_IndicatorArrowRight;
                int xpos = menuItem->rect.left() + menuItem->rect.width() - 3 - dim;
                QRect  vSubMenuRect = visualRect(option->direction, menuItem->rect,
                                                 QRect(xpos, menuItem->rect.top() + menuItem->rect.height() / 2 - dim / 2, dim, dim));
                QStyleOptionMenuItem newMI = *menuItem;
                newMI.rect = vSubMenuRect;
                newMI.state = !enabled ? State_None : State_Enabled;
                if (selected)
                    newMI.palette.setColor(QPalette::Foreground,
                                           newMI.palette.highlightedText().color());
                proxy()->drawPrimitive(arrow, &newMI, painter, widget);
            }
        }
        painter->restore();
        return;
    }
    QProxyStyle::drawControl(element,option,painter,widget);
}
