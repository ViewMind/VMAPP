#include "drawingconstantscalculator.h"

DrawingConstantsCalculator::DrawingConstantsCalculator()
{
    targetH = 1;
    targetW = 1;
    computeK();
}

void DrawingConstantsCalculator::setTargetResolution(qreal w, qreal h){
    targetH = h;
    targetW = w;
    computeK();
}

qreal DrawingConstantsCalculator::getVerticalRatio(qreal ref_ratio){
    return ref_ratio*sqrtK;
}

qreal DrawingConstantsCalculator::getHorizontalRatio(qreal ref_ratio){
    return ref_ratio/sqrtK;
}

void DrawingConstantsCalculator::computeK(){
    K = (ReferenceResolution::height*targetW)/(ReferenceResolution::width*targetH);
    sqrtK = qSqrt(K);
}
