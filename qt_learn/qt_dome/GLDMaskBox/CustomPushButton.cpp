#include "custompushbutton.h"

#include <QPainter>


CustomPushButton::CustomPushButton(QString imagePath, QWidget *parent)
    : QPushButton(parent)
    , m_imagePath(imagePath)
{
    //设置背景透明
    setAttribute(Qt::WA_TranslucentBackground);

    //加载图像
    pic.load(m_imagePath);

    resize(pic.size());
}

void CustomPushButton::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);

    QPainter painter(this);

    //绘制图像
    painter.drawPixmap(0, 0, pic);
}
