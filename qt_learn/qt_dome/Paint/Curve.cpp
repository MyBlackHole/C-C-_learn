#include "BaseLine.h"
#include "Curve.h"
#include <qpainter.h>
#include <vector>
#include <iostream>
using namespace std;

Curve::Curve(QImage* image, QColor* color, QWidget* parent)
    : BaseLine(image, color, parent)
{
    this->drawLine(*color);
}

void Curve::drawLine(QColor color)
{
   QPainter painter(m_image);
   painter.setPen(color);

    vector<QPointF> floatPoints;
    floatPoints.resize(m_points.size());

    for (unsigned int i = 0; i <= m_points.size()-1; ++i)
    {
        floatPoints[i] = QPointF(m_points[i]);
    }
   
   for (float t = 0.0; t <= 1.0; t += 0.0005)
   {
        vector<QPointF> da = findPoint(floatPoints, t);
        painter.drawPoint(da[0]);
   } 
}

vector<QPointF> Curve::findPoint(vector<QPointF> points, double t)
{
    vector<QPointF> returnPoints(points.size() - 1);

    for (unsigned int i = 0; i <= points.size() - 2; ++i)
    {
        returnPoints[i] = this->findMidpoint(&points[i], &points[i+1], t);
    }
    
    if (returnPoints.size() != 1)
    {
        returnPoints = this->findPoint(returnPoints, t);
    }

    return returnPoints;
}

QPointF Curve::findMidpoint(QPointF* a, QPointF* b, double t)
{
    QPointF r;
    r.setX(a->x() + (b->x() - a->x()) * t);
    r.setY(a->y() + (b->y() - a->y()) * t);
    return r;
}
