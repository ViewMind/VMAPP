#include "qgraphicsarrow.h"

QGraphicsArrow::QGraphicsArrow()
{

    arrowColor = Qt::cyan;
    arrowBodywidth = 10;
    arrowHeadHeightToBodyWidthRatio = 2;
    arrowHeadLengthToHeightRatio = 2;

}

void QGraphicsArrow::setArrowHeadRatios(qreal hToBodyWidth, qreal ltoHR){
    arrowHeadHeightToBodyWidthRatio = hToBodyWidth;
    arrowHeadLengthToHeightRatio = ltoHR;
}

void QGraphicsArrow::setArrowColor(const QColor &color){
    arrowColor = color;
}

void QGraphicsArrow::setArrowWidth(const qreal &width){
    arrowBodywidth = width;
}

void QGraphicsArrow::render(QGraphicsScene *scene, qreal x1, qreal y1, qreal x2, qreal y2){

    // For Debug.
    //scene->addLine(x1,y1,x2,y2,QPen(Qt::black,1,Qt::SolidLine,Qt::RoundCap,Qt::BevelJoin));

    qreal arrowHeadHeight = arrowBodywidth*arrowHeadHeightToBodyWidthRatio;
    qreal arrowHeadLength = arrowHeadHeight*arrowHeadLengthToHeightRatio;

    // First we figure out the quadrant, as if the starting point is the 0,0. We need to subsctract the arrow head length from the length of the segment.
    qreal B  = x2-x1;
    qreal A  = y2-y1;
    qreal Dx, Dy;

    if (qAbs(B) < COMPARISON_TOLERANCE){
        // This is a vertical line.
        if (A > 0){
            // Down Arrow.
            Dy = -arrowHeadLength;
            Dx = 0;
        }
        else {
            // Up arrow.
            Dy = arrowHeadLength;
            Dx = 0;
        }
    }
    else {
        // The minus is added because the Delta is the EXACT opposite of the of the resulting sign.
        Dx = arrowHeadLength*qAbs(B)/qSqrt(A*A + B*B);
        Dy = A*Dx/B;

        qDebug() << Dx << Dy;

        if (B > 0){ // Only when the x2 is larger than x1 are the signs inverted.
            Dx = -Dx;
            Dy = -Dy;
        }

    }

    scene->addLine(x1,y1,x2+Dx,y2+Dy,QPen(arrowColor,arrowBodywidth,Qt::SolidLine,Qt::RoundCap,Qt::BevelJoin));


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

}
