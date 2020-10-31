#include <QMouseEvent>

#include <math.h>
#include <qwidget.h>
#include <qpainter.h>

#include "Line.h"

static double round(double r)
{
    return (r > 0.0) ? floor(r + 0.5) : ceil(r - 0.5);
}

Line::Line(QImage* image, QColor* color, QWidget* parent)
    : BaseLine(image, color, parent)
{
    for (size_t i = 0; i < m_points.size(); ++i)
    {
        this->drawLine(*m_color);
    }
}

void Line::drawLine(QColor color)
{
    for (size_t i = 0; i < m_points.size()-1;  ++i)
    {
        QPoint* p = &m_points[i];
        QPoint* q = &m_points[i + 1];

        for (double t = 0.0; t <= 1.0; t += 0.00001)
        {
            int dx = round(p->x() + (q->x() - p->x()) * t);
            int dy = round(p->y() + (q->y() - p->y()) * t);
            m_image->setPixel(dx, dy, color.rgb());
        }
    }
}
