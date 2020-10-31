#ifndef QCOLORWIDGET_H
#define QCOLORWIDGET_H

#include <QWidget>
#include "../objectinspector_global.h"

class OBJECTINSPECTORSHARED_EXPORT QColorWidget : public QWidget
{
    Q_OBJECT

public:
    explicit QColorWidget(QWidget *parent = 0);
    
    const   QColor&   color()     const;
    const   QColor&   helpColor() const;

protected:
    void    paintEvent(QPaintEvent * );

signals:
    void    colorChanged();

public slots:
    void    setColor(const QColor & Value);
    void    setHelpColor(const QColor & Value);

private:
    QColor  fcolor;
    QColor  fhelpColor;
    QBrush  fmaskBrush;
};

#endif // QCOLORWIDGET_H
