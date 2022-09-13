#include "qgraphicsarrow.h"

QGraphicsArrow::QGraphicsArrow()
{

    arrowColor = Qt::cyan;
    arrowBodywidth = 10;
    arrowHeadHeightToBodyWidthRatio = 2;
    arrowHeadLengthToHeightRatio = 2;
    arrowBeginMarkerWidthMultiplier = 0;

}

void QGraphicsArrow::setArrowHeadRatios(qreal hToBodyWidth, qreal ltoHR){
    arrowHeadHeightToBodyWidthRatio = hToBodyWidth;
    arrowHeadLengthToHeightRatio = ltoHR;
}

void QGraphicsArrow::setArrowStartCircleParameters(qreal widthMultiplier, const QColor &color){
    arrowBeginMarkerWidthMultiplier = widthMultiplier;
    arrowBeginMarkerColor = color;
}

void QGraphicsArrow::setArrowColor(const QColor &color){
    arrowColor = color;
}

void QGraphicsArrow::setArrowWidth(const qreal &width){
    arrowBodywidth = width;
}

void QGraphicsArrow::render(QGraphicsScene *scene, qreal x1, qreal y1, qreal x2, qreal y2, qreal z){

    // For Debug.
    //scene->addLine(x1,y1,x2,y2,QPen(Qt::black,1,Qt::SolidLine,Qt::RoundCap,Qt::BevelJoin));

    qreal arrowHeadHeight = arrowBodywidth*arrowHeadHeightToBodyWidthRatio;
    qreal arrowHeadLength = arrowHeadHeight*arrowHeadLengthToHeightRatio;

    qreal B  = x2-x1;
    qreal A  = y2-y1;

    QList<qreal> deltas = PointDeltaToChangeSegmentLength(x1,y1,x2,y2,-arrowHeadLength);
    qreal Dx = deltas.first();
    qreal Dy = deltas.last();

    QGraphicsLineItem *line = scene->addLine(x1,y1,x2+Dx,y2+Dy,QPen(arrowColor,arrowBodywidth,Qt::SolidLine,Qt::RoundCap,Qt::BevelJoin));
    line->setZValue(z);


    QPolygonF arrowHeadPoly;
    arrowHeadPoly.append(QPointF(x2,y2));

    if (x1 > x2){
        // Arrow to the left
        arrowHeadPoly.append(QPointF(x2+arrowHeadLength,y2-arrowHeadHeight/2));
        arrowHeadPoly.append(QPointF(x2+arrowHeadLength,y2+arrowHeadHeight/2));
    }
    else if (x1 < x2){
        // Arrow to the right
        arrowHeadPoly.append(QPointF(x2-arrowHeadLength,y2-arrowHeadHeight/2));
        arrowHeadPoly.append(QPointF(x2-arrowHeadLength,y2+arrowHeadHeight/2));
    }
    else if (y1 > y2){
        // Arrow down
        arrowHeadPoly.append(QPointF(x2+arrowHeadHeight/2,y2+arrowHeadLength));
        arrowHeadPoly.append(QPointF(x2-arrowHeadHeight/2,y2+arrowHeadLength));
    }
    else{
        // Arrow up
        arrowHeadPoly.append(QPointF(x2+arrowHeadHeight/2,y2-arrowHeadLength));
        arrowHeadPoly.append(QPointF(x2-arrowHeadHeight/2,y2-arrowHeadLength));
    }

    arrowHeadPoly.append(QPointF(x2,y2));

    QGraphicsPolygonItem *arrowHead = scene->addPolygon(arrowHeadPoly,QPen(arrowColor),QBrush(arrowColor));
    //QGraphicsPolygonItem *arrowHead =  scene->addPolygon(arrowHeadPoly,QPen(Qt::black),QBrush(arrowColor));

    if ((qAbs(B) > COMPARISON_TOLERANCE) && (qAbs(A) > COMPARISON_TOLERANCE)){
        arrowHead->setTransformOriginPoint(x2,y2);
        // Computing the angle
        arrowHead->setRotation(qRadiansToDegrees(qAtan(Dy/Dx)));
    }

    arrowHead->setZValue(z);

    // If enabled we draw a circle at the arrow beginning
    if (arrowBeginMarkerWidthMultiplier > 0){
        qreal R = arrowBodywidth*arrowBeginMarkerWidthMultiplier/2;
        qreal x = x1-R;
        qreal y = y1-R;
        QGraphicsEllipseItem * arrowStart = scene->addEllipse(x,y,2*R,2*R,QPen(arrowBeginMarkerColor),QBrush(arrowBeginMarkerColor));
        arrowStart->setZValue(z);
    }

}


QPointF QGraphicsArrow::PointAlongRectThatCreatesAShorterLengthLine(qreal xo, qreal yo, qreal xf, qreal yf, qreal L){

    qreal D = DistanceBetweenTwoPoints(QPointF(xo,yo),QPointF(xf,yf));
    qreal Delta = L - D;

    QList<qreal> deltas = PointDeltaToChangeSegmentLength(xo,yo,xf,yf,Delta);

    return QPointF(xf+deltas.first(),yf+deltas.last());

}

QList<qreal> QGraphicsArrow::PointDeltaToChangeSegmentLength(qreal xo, qreal yo, qreal xf, qreal yf, qreal DeltaSegment){

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

qreal QGraphicsArrow::DistanceBetweenTwoPoints(const QPointF &p0, const QPointF &p1){
    qreal dx = p0.x() - p1.x();
    qreal dy = p0.y() - p1.y();
    return qSqrt(dx*dx + dy*dy);
}

QList<qreal> QGraphicsArrow::RectParameters(const QPointF &p0, const QPointF &p1){

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
