#ifndef LINEMATH_H
#define LINEMATH_H

#include <QPointF>
#include <QPoint>
#include <QList>
#include <QtMath>

/**
 * @brief The LineMath class
 * Collection of math function for a line in a 2D plane.
 */

class LineMath
{
public:
    LineMath();

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
    constexpr static const qreal COMPARISON_TOLERANCE = 0.001;

};

#endif // LINEMATH_H
