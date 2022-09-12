#ifndef QGRAPHICSARROW_H
#define QGRAPHICSARROW_H

/**
 * @brief The QGraphicsArrow class
 * An object of this type will take the two x, y coordinates and draw an arrow from the first to the second coordinate.
 * It will do all the computations to draw the head and then rotate it.
 */

#include <QGraphicsScene>
#include <QPolygonF>
#include <QGraphicsRectItem>
#include <QGraphicsLineItem>
#include <QDebug>

class QGraphicsArrow
{
public:
    QGraphicsArrow();

    void setArrowHeadRatios(qreal hToBodyWidth = 2, qreal ltoHR = 2);
    void setArrowColor(const QColor &color);
    void setArrowWidth(const qreal &width);
    void render(QGraphicsScene *scene, qreal x1, qreal y1, qreal x2, qreal y2);

private:

    QColor arrowColor;
    qreal arrowBodywidth;
    qreal arrowHeadHeightToBodyWidthRatio;
    qreal arrowHeadLengthToHeightRatio;

    const qreal COMPARISON_TOLERANCE = 0.001;

};

#endif // QGRAPHICSARROW_H
