#include "movingwindowalgorithm.h"

MovingWindowAlgorithm::MovingWindowAlgorithm()
{
}

Fixations MovingWindowAlgorithm::computeFixations(const DataMatrix &data, qint32 xI, qint32 yI, qint32 tI){

    Fixations fixations;

    qint32 startIndex = 0;
    qint32 endIndex;
    bool windowExpanded = false;
    MinMax mmX,mmY;

    while (true) {

        endIndex = startIndex + parameters.getStartWindowSize()-1;
        windowExpanded = false;

        // If the end index is outside the bounds the remaining data can be discarded as a fixation there
        // would be shorter than the minimum fixation. So the algorithm ends here.
        if (endIndex >= data.size()) break;

        // Getting the start window dispersion limits
        mmX = findDispLimits(data,xI,startIndex,endIndex);
        mmY = findDispLimits(data,yI,startIndex,endIndex);

        while ((mmX.diff() + mmY.diff()) <= parameters.maxDispersion){

            // Increasing the window size by 1
            endIndex++;
            windowExpanded = true;

            if (endIndex >= data.size()){
                // This was the final fixation.
                fixations << calculateFixationPoint(data,xI,yI,startIndex,endIndex-1,tI);
                return fixations;
            }

            // A new data point is added as part of the fixation.
            mmX.max = qMax(mmX.max,data.at(endIndex).at(xI));
            mmY.max = qMax(mmY.max,data.at(endIndex).at(yI));
            mmX.min = qMin(mmX.min,data.at(endIndex).at(xI));
            mmY.min = qMin(mmY.min,data.at(endIndex).at(yI));

        }

        // If the code, got here, it means the new point is NOT part of the fixation.
        if (windowExpanded){
            // This means that the current window IS a fixation.
            fixations << calculateFixationPoint(data,xI,yI,startIndex,endIndex-1,tI);
            startIndex = endIndex;
        }
        else{
            // The current minumum window is NOT a fixation. We simply move one to the right.
            startIndex++;
        }

    }

    return fixations;

}

Fixation MovingWindowAlgorithm::calculateFixationPoint(const DataMatrix &data,
                                                       qint32 xI, qint32 yI,
                                                       qint32 startI, qint32 endI,
                                                       qint32 tI){

    Fixation f;
    f.x = 0; f.y  = 0;

    //qWarning() << "CALCULATING THE FIXATION BETWEEN" << startI << "and" << endI << "SAMPLES = " << endI-startI+1;

    // Calculating the centroid of the fixation.
    for (qint32 i = startI; i <= endI; i++){
        //qWarning() << "Row" << i << data.at(i);
        f.x = f.x + data.at(i).at(xI);
        f.y = f.y + data.at(i).at(yI);
    }
    qreal size = endI - startI + 1;
    f.x = f.x/size;
    f.y = f.y/size;

    // Calculating the duration
    f.duration = data.at(endI).at(tI) - data.at(startI).at(tI);
    //qWarning() << "CENTROID" << f.x << f.y << "." << "DURATION" << f.duration;

    // Finally the indexes are stored.
    f.indexFixationStart = startI;
    f.indexFixationEnd = endI;

    return f;

}

MovingWindowAlgorithm::MinMax MovingWindowAlgorithm::findDispLimits(const DataMatrix &data,
                                                                    qint32 col,
                                                                    qint32 startI,
                                                                    qint32 endI) const{

    MinMax mm;
    mm.max = 0;
    mm.min = 0;

    if (data.isEmpty()) return mm;

    mm.max = data.at(startI).at(col);
    mm.min = data.at(endI).at(col);

    for (qint32 i = startI+1; i <= endI; i++){
        mm.max = qMax(mm.max,data.at(i).at(col));
        mm.min = qMin(mm.min,data.at(i).at(col));
    }

    return mm;

}