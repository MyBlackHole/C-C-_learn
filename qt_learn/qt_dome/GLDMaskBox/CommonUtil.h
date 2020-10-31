#ifndef COMMONUTIL_H
#define COMMONUTIL_H

#include <QLineF>
#include <QRectF>
#include <QColor>
#include <QString>
#include <QVector>

#include <qmath.h>


/*!
    * @brief  calcCrossLine2Rect
    *   ����ֱ������εĽ���
    * @author:  min
    * @param [in] QLineF line              ֱ��
    * @param [in] QRectF rc                ����
    * @param [out] QPointF & ptCross1      ����1
    * @param [out] QPointF & ptCross2      ����2
    * @return     bool �Ƿ��ཻ(ֱ���ھ����ڲ����ཻ)
*/
static bool calcCrossLine2Rect(const QLineF line, const QRectF rc, QPointF & ptCross1, QPointF & ptCross2)
{
    bool bRet = false;

    do
    {
        // ���㶼�ھ�����
        if (rc.toRect().contains(line.p1().toPoint(), true) &&
            rc.toRect().contains(line.p2().toPoint(), true))
        {
            break;
        }

        QVector<QPointF> pts;
        QVector<QLineF> lines;
        QLineF lineTop(rc.topLeft(), rc.topRight());
        QLineF lineBottom(rc.bottomLeft(), rc.bottomRight());
        QLineF lineLeft(rc.topLeft(), rc.bottomLeft());
        QLineF lineRight(rc.topRight(), rc.bottomRight());
        lines << lineTop << lineBottom << lineLeft << lineRight;

        QPointF ptTmp(0, 0);
        for (int i = 0; i < lines.size(); ++i)
        {
            if (line.intersect(lines[i], &ptTmp) == QLineF::BoundedIntersection)
            {
                if (i == 0)
                {
                    ptTmp += QPoint(0, 1);
                }
                else if (i == 1)
                {
                    ptTmp += QPoint(0, -1);
                }
                else if (i == 2)
                {
                    ptTmp += QPoint(1, 0);
                }
                else if (i == 3)
                {
                    ptTmp += QPoint(-1, 0);
                }

                pts << ptTmp;
            }
        }

        if (pts.size() <= 0)
        {
            break;
        }
        else if (pts.size() < 2)
        {
            ptCross1 = pts[0];
            bRet = true;
            break;
        }
        else if (pts.size() >= 2)
        {
            ptCross1 = pts[0];
            ptCross2 = pts[1];
            bRet = true;
            break;
        }

    }while(0);

    return bRet;
}


/*!
    * @brief  calcRadiusAndAngleOfPt2d
    *   ����Ŀ������뾶�ͽǶȣ���Բ��
    * @author:  min
    * @param [in] QPointF ptTarget    Ŀ���
    * @param [out] double &dRadius    �뾶
    * @param [out] double &dAngle     �Ƕ�
    * @param [in] QPointF ptOrigin    ԭ��
    * @return     void
*/
static void calcRadiusAndAngleOfPt2d(
    const QPointF ptTarget,
    double &dRadius,
    double &dAngle,
    const QPointF ptOrigin = QPointF(0.0,0.0))
{
    double dx = ptTarget.x() - ptOrigin.x();
    double dy = ptTarget.y() - ptOrigin.y();
    dAngle = 0;
    dRadius = sqrt(dx * dx + dy * dy);

    dAngle = qAcos(dx / dRadius) * 180 / M_PI;

    if(ptTarget.y() < ptOrigin.y())
    {
        dAngle = 360 - dAngle;
    }
}

/*!
    * @brief  calcPtOfAngle2d
    *   ͨ���뾶�ͽǶȼ���Ŀ���(��Բ)
    * @author:  min
    * @param [in] double dXRadius   X��뾶
    * @param [in] double dYRadius   Y��뾶
    * @param [in] double dAngle     �Ƕ�
    * @param [in] QPointF ptOrigin  ԭ��
    * @return     QPointF           ����Ŀ���
*/
static QPointF calcPtOfAngle2d(
        const double dXRadius,
        const double dYRadius,
        const double dAngle,
        const QPointF ptOrigin = QPointF(0.0,0.0))
{
    QPointF pt;

    pt.setX(ptOrigin.x() + dXRadius * qCos(dAngle * M_PI / 180.0));
    pt.setY(ptOrigin.y() + dYRadius * qSin(dAngle * M_PI / 180.0));

    return pt;
}

#endif // COMMONUTIL_H
