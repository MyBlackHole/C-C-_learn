#ifndef _ITOOL_H
#define _ITOOL_H

#include <qwidget.h>

class ITool
{      
public:
    virtual void mousePressEvent(QMouseEvent* event) = 0;
    virtual void mouseMoveEvent(QMouseEvent* event) = 0;
    virtual void mouseReleaseEvent(QMouseEvent* event) = 0;
    virtual void updateImage(QImage* image) = 0;
    virtual void updateColor(QColor* color) = 0;
};
#endif
