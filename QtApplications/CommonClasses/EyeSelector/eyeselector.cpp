#include "eyeselector.h"

EyeSelector::EyeSelector()
{
    // Default behaviour.
    eyeSelectorBehaviour = EBS_CONTINOUS;
}

void EyeSelector::setTargetCountForSelection(qint32 tcs){
    targetCountForSelection = tcs;
}

void EyeSelector::setTargetBoxes(QList<QRectF> tboxes){
    targetBoxes = tboxes;
}

void EyeSelector::setBehaviour(EyeSelectorBehaviour esb){
    eyeSelectorBehaviour = esb;
}

qint32 EyeSelector::isSelectionMade(QPoint point){
    return isSelectionMade(point.x(),point.y());
}

qint32 EyeSelector::isSelectionMade(QPointF pointf){
    return isSelectionMade(pointf.x(),pointf.y());
}

qint32 EyeSelector::isSelectionMade(qreal x, qreal y){
    qint32 whichTarget = -1;
    //qDebug() << "EyeSelector (" << x << y << ") in any of " << targetBoxes;
    for (qint32 i = 0; i < targetBoxes.size(); i++){
        if (targetBoxes.at(i).contains(x,y)){
            whichTarget = i;
            break;
        }
    }

    if (whichTarget == -1){
        if (eyeSelectorBehaviour == EBS_CONTINOUS){
            reset();
        }
    }
    else{
        if (whichTarget == currentSelection){
            pointCounter--;
            if (pointCounter == 0){
                qint32 ans = currentSelection;
                reset();
                return ans;
            }
        }
        else {
            reset();
            currentSelection = whichTarget;
        }
    }

    return -1;

}

void EyeSelector::reset(){
    pointCounter = targetCountForSelection;
    currentSelection = -1;
}

