#ifndef _BASELINE_H
#define _BASELINE_H

#include <vector>
#include <QMouseEvent>

#include <qwidget.h>

#include "ITool.h"

class BaseLine : public ITool
{  
public:
    BaseLine(QImage* image, QColor* color, QWidget* parent);

    void mousePressEvent(QMouseEvent* event);
    void mouseMoveEvent(QMouseEvent* event);
    void mouseReleaseEvent(QMouseEvent* event);
    void updateImage(QImage* image);
    void updateColor(QColor* color);

    void setPoints(std::vector<QPoint> points);

protected:
    void drawPoint(int x, int y, bool focus);
    void removePoint(int x, int y);
    virtual void drawLine(QColor color);

protected:
    QImage* m_image;
    QImage* m_unselected_icon;
    QImage* m_selected_icon;
    QColor* m_color;
    QWidget* m_parent;
    std::vector<QPoint> m_points;
    int m_range;
    int m_selected;
};

#endif
