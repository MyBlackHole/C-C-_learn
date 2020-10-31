#include <vector>
#include <QMouseEvent>

#include <math.h>
#include <qwidget.h>
#include <qpainter.h>

#include "BaseLine.h"

using namespace std;

BaseLine::BaseLine(QImage* image, QColor* color, QWidget* parent)
    : m_image(image)
    , m_color(color)
    , m_parent(parent)
{
    m_range = 5;
    m_selected = -1;
    m_points.resize(4);

    this->m_unselected_icon = new QImage(":/Images/line_unselected.jpg");
    this->m_selected_icon = new QImage(":/Images/line_selected.jpg");
    
    for (int i = 0; i < 4; ++i)
    {
        int p = 50 + 100 * i;
        m_points[i] = QPoint(p, p);
        this->drawPoint(p, p, false);
    }
    
    for (int i = 0; i < 3; ++i)
    {
        this->drawLine(*m_color);
    }
    
    m_parent->repaint();
}

void BaseLine::mousePressEvent(QMouseEvent* event)
{
    if (event->button() == Qt::RightButton)
    {
        this->drawLine(QColor("white"));
        m_points.resize(m_points.size() + 1);
        m_points[m_points.size() - 1] = *(new QPoint(event->x(), event->y()));
        QPoint* p = &m_points[m_points.size() - 1];
        this->drawPoint(p->x(), p->y(), false);
        this->drawLine(*m_color);
    }
    else
    {
        int a = event->x();
        int b = event->y();
        this->m_selected = -1;
        
        for (size_t i = 0; i < m_points.size(); ++i)
        {
            //for every point
            QPoint p = m_points[i];
            if (a > p.x() - m_range && a < p.x() + m_range && b > p.y() - m_range && b < p.y() + m_range)
            {
                this->drawPoint(p.x(), p.y(), true);
                this->m_selected = i;
            }
        }
        
        for (size_t i = 0; i < m_points.size(); ++i)
        {
            if (m_selected != i)
            {
                QPoint p = m_points[i];
                this->drawPoint(p.x(), p.y(), false);
            }
        }
    }
    
}
void BaseLine::mouseMoveEvent(QMouseEvent* event)
{
    if (m_selected == -1)
    {
        return;
    }

    if (event->x() > m_image->width() || event->x() < 0 || event->y() > m_image->height() || event->y() < 0)
    {
        return;
    }
    
    QPoint* p = &m_points[m_selected];
    this->removePoint(p->x(), p->y()); //get rid of the old one...
    
    //get rid of the old lines
    if (m_selected != 0)
    {
        this->drawLine(QColor("white"));
    }

    this->drawLine(QColor("white"));

    p->setX(event->x());
    p->setY(event->y());
    this->drawPoint(p->x(), p->y(), true);
    
    //redraw lines
    if (m_selected != 0)
    {
        this->drawLine(*m_color);
    }

    if (m_selected != 3)
    {
        this->drawLine(*m_color);
    }
}

void BaseLine::mouseReleaseEvent(QMouseEvent* event)
{
    Q_UNUSED(event);

    if (m_selected == -1)
    {
        return;
    }

    QPoint p = m_points[m_selected];
    this->drawPoint(p.x(), p.y(), true);
}

void BaseLine::updateImage(QImage* image)
{
    m_image = image;
}

void BaseLine::drawPoint(int x, int y, bool focus)
{
    QPainter painter(m_image);

    QImage* image;

    if (!focus)
    {
        image = m_unselected_icon;
    }
    else
    {
        image = m_selected_icon;
    }

    int widthOffset = image->width()/2;
    int heightOffset = image->height()/2;
    
    QPoint imageOrigin(x - widthOffset, y - heightOffset);
    painter.drawImage(imageOrigin, *image);
}

void BaseLine::removePoint(int x, int y)
{
    QPainter painter(m_image);

    QImage* image = new QImage(5, 5, QImage::Format_RGB32);
    image->fill(QColor("white").rgba());
    int widthOffset = image->width()/2;
    int heightOffset = image->width()/2;
    
    QPoint imageOrigin(x - widthOffset, y - heightOffset);
    painter.drawImage(imageOrigin, *image);
}

void BaseLine::drawLine(QColor color)
{
    Q_UNUSED(color);
    //default line drawing implementation?
}

void BaseLine::updateColor(QColor* color)
{
    this->m_color = color;
}

void BaseLine::setPoints(vector<QPoint> points)
{
    this->m_points = points;
}
