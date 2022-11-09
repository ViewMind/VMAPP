#include "linemath.h"

LineMath::LineMath() {}

QPointF LineMath::PointAlongRectThatCreatesAShorterLengthLine(qreal xo, qreal yo, qreal xf, qreal yf, qreal L){

    qreal D = DistanceBetweenTwoPoints(QPointF(xo,yo),QPointF(xf,yf));
    qreal Delta = L - D;

    QList<qreal> deltas = PointDeltaToChangeSegmentLength(xo,yo,xf,yf,Delta);

    return QPointF(xf+deltas.first(),yf+deltas.last());

}

QList<qreal> LineMath::PointDeltaToChangeSegmentLength(qreal xo, qreal yo, qreal xf, qreal yf, qreal DeltaSegment){

    // First we figure out the quadrant, as if the starting point is the 0,0. We need to subsctract the arrow head length from the length of the segment.
    qreal B  = xf-xo;
    qreal A  = yf-yo;
    qreal Dx, Dy;

    // The algorithm belows works such that a negative delta results in a increase in size while a positive one results in a decrease.
    qreal L = -DeltaSegment;

    if (qAbs(B) < COMPARISON_TOLERANCE){
        // This is a vertical line.
        if (A > 0){
            // Down Arrow.
            Dy = -L;
            Dx = 0;
        }
        else {
            // Up arrow.
            Dy = L;
            Dx = 0;
        }
    }
    else {
        Dx = L*qAbs(B)/qSqrt(A*A + B*B);
        Dy = A*Dx/B;

        //qDebug() << Dx << Dy;

        if (B > 0){ // Only when the x2 is larger than x1 are the signs inverted.
            Dx = -Dx;
            Dy = -Dy;
        }

    }

    QList<qreal> ans; ans << Dx << Dy;
    return ans;
}

qreal LineMath::DistanceBetweenTwoPoints(const QPointF &p0, const QPointF &p1){
    qreal dx = p0.x() - p1.x();
    qreal dy = p0.y() - p1.y();
    return qSqrt(dx*dx + dy*dy);
}

QList<qreal> LineMath::RectParameters(const QPointF &p0, const QPointF &p1){

    QList<qreal> params;
    qreal dx = (p0.x() - p1.x());

    if (qAbs(dx) < COMPARISON_TOLERANCE){
        // Vertical line
        return params;
    }

    qreal m = (p0.y() - p1.y())/dx;
    qreal b = p0.y() - m*p0.x();

    params << m << b;
    return params;
}
