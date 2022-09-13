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

    // The arrow start circle will be drawn with any multiplier that is larger than zero.
    // This will cause the start circle diameter be equal to the multiplier times the arrow body width.
    void setArrowStartCircleParameters(qreal widthMultiplier, const QColor &color);

    void setArrowColor(const QColor &color);
    void setArrowWidth(const qreal &width);

    void render(QGraphicsScene *scene, qreal x1, qreal y1, qreal x2, qreal y2, qreal z = 0);

    /**
     * @brief PointAlongRectThatCreatesAShorterLengthLine.
     * Given the rect defined by points Po and Pf, named R, the function will compute a Point P
     * such that the P belongs ot the rect, the distance between Po and P is L where
     * The resulting line line will coincide with R from Po to P.
     * @param xo - x coordinate of P0 or the initial point
     * @param yo - y coordinate of P0 or the initial point
     * @param xf - x coordinate of Pf or the end point.
     * @param yf - y coordinate of Pf or the end point.
     * @param L  - How long we want the line to actually be by changing Pf for P.
     * @return
     */
    static QPointF PointAlongRectThatCreatesAShorterLengthLine(qreal xo, qreal yo, qreal xf, qreal yf, qreal L);

    // This returns the Delta X and the Delta Y which must be summed to the xf and yf coordinates in order to increase
    // Or decrease the length of the segment defined by Po and Pf.
    static QList<qreal> PointDeltaToChangeSegmentLength(qreal xo, qreal yo, qreal xf, qreal yf, qreal DeltaSegment);

    // The euclidic distance between two points.
    static qreal DistanceBetweenTwoPoints(const QPointF &p0, const QPointF &p1);

    // Returns a two value list with the m and b respectively of the rect that passees through p0 and p1.
    // If a verticla line is specified an empty list is returned.
    static QList<qreal> RectParameters(const QPointF &p0, const QPointF &p1);

private:

    QColor arrowColor;
    QColor arrowBeginMarkerColor;
    qreal arrowBeginMarkerWidthMultiplier;
    qreal arrowBodywidth;
    qreal arrowHeadHeightToBodyWidthRatio;
    qreal arrowHeadLengthToHeightRatio;

    constexpr static const qreal COMPARISON_TOLERANCE = 0.001;

};

#endif // QGRAPHICSARROW_H
