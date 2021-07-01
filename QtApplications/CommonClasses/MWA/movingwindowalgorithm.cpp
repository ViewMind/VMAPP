#include "movingwindowalgorithm.h"

const qreal MovingWindowAlgorithm::PUPIL_ZERO_TOL = 1e-6;

MovingWindowAlgorithm::MovingWindowAlgorithm()
{
    minimalFixations = false;
}

void FixationList::displayFixList() const{
    for (qint32 i = 0; i < trialID.size(); i++){
        qWarning() << i << ":" << trialID.at(i);
    }
    for (qint32 i = 0; i < left.size(); i++){
        qWarning() << i << ":" << left.at(i).size();
    }
    for (qint32 i = 0; i < right.size(); i++){
        qWarning() << i << ":" << right.at(i).size();
    }
}

void FixationList::fillFixationsLists(){
    left.clear(); right.clear();
    for (qint32 i = 0; i < trialID.size(); i++){
        left << Fixations();
        right << Fixations();
    }
}

bool FixationList::checkCoherenceInListSizes() const{
    return (trialID.size() == left.size()) && (left.size() == right.size());
}

qint32 FixationList::indexOfTrial(const QStringList &toFind) const{
    for (qint32 i = 0; i < trialID.size(); i++){
        if (toFind.size() == trialID.at(i).size()){
            bool isSame = true;
            for (qint32 j = 0; j < toFind.size(); j++){
                if (toFind.at(j) != trialID.at(i).at(j)){
                    isSame = false;
                    break;
                }
            }
            if (isSame) return i;
        }
    }
    return -1;
}

Fixations MovingWindowAlgorithm::computeFixations(const DataMatrix &data, qint32 xI, qint32 yI, qint32 tI){

    Fixations fixations;

    qint32 startIndex = 0;
    qint32 endIndex;
    bool windowExpanded = false;
    MinMax mmX,mmY;

#ifdef ENABLE_MWA_DEBUG
    QString logstr;
    logstr = "";
#endif

    while (true) {

        endIndex = startIndex + parameters.getStartWindowSize()-1;
        windowExpanded = false;

        // If the end index is outside the bounds the remaining data can be discarded as a fixation there
        // would be shorter than the minimum fixation. So the algorithm ends here.
        if (endIndex >= data.size()) break;

        // Getting the start window dispersion limits
        mmX = findDispLimits(data,xI,startIndex,endIndex);
        mmY = findDispLimits(data,yI,startIndex,endIndex);

#ifdef ENABLE_MWA_DEBUG
        logstr = logstr + "SET: ";
        for (qint32 i = startIndex; i <= endIndex; i++){
            logstr = logstr + "@" + QString::number(data.at(i).at(tI))
                    + "(" + QString::number(data.at(i).at(xI)) + "," + QString::number(data.at(i).at(yI)) + ") ";
        }
        logstr = logstr + "XB: " + mmX.toString() + ". YB: " + mmY.toString() + "\n";
#endif

        while ((mmX.diff() + mmY.diff()) <= parameters.maxDispersion){

            // Increasing the window size by 1
            endIndex++;
            windowExpanded = true;

            if (endIndex >= data.size()){
                // This was the final fixation.
                fixations << calculateFixationPoint(data,xI,yI,startIndex,endIndex-1,tI);
#ifdef ENABLE_MWA_DEBUG
                logstr = logstr + "LAST FIX FOUND: " + fixations.last().toString();
#endif
                return fixations;
            }

            // A new data point is added as part of the fixation.
            mmX.max = qMax(mmX.max,data.at(endIndex).at(xI));
            mmY.max = qMax(mmY.max,data.at(endIndex).at(yI));
            mmX.min = qMin(mmX.min,data.at(endIndex).at(xI));
            mmY.min = qMin(mmY.min,data.at(endIndex).at(yI));

#ifdef ENABLE_MWA_DEBUG
            logstr = logstr + "SET: ";
            for (qint32 i = startIndex; i <= endIndex; i++){
                logstr = logstr + "@" + QString::number(data.at(i).at(tI))
                        + "(" + QString::number(data.at(i).at(xI)) + "," + QString::number(data.at(i).at(yI)) + ") ";
            }
            logstr = logstr + "XB: " + mmX.toString() + ". YB: " + mmY.toString() + "\n";
#endif

        }

        // If the code, got here, it means the new point is NOT part of the fixation.
        if (windowExpanded){
            // This means that the current window IS a fixation.
            fixations << calculateFixationPoint(data,xI,yI,startIndex,endIndex-1,tI);
#ifdef ENABLE_MWA_DEBUG
            logstr = logstr + "FIX FOUND: " + fixations.last().toString() + "\n";
#endif
            startIndex = endIndex;
        }
        else{
            // The current minumum window is NOT a fixation. We simply move one to the right.
#ifdef ENABLE_MWA_DEBUG
            logstr = logstr + "-----------\n";
#endif
            startIndex++;
        }

    }

#ifdef ENABLE_MWA_DEBUG
    logger.appendStandard(logstr);
#endif

    return fixations;

}

void MovingWindowAlgorithm::setMinimalFixations(bool enable){
    minimalFixations = enable;
}

Fixation MovingWindowAlgorithm::calculateFixationsOnline(qreal x, qreal y, qreal timeStamp, qreal pupil, qreal schar, qreal word){
    Fixation fixation;
    fixation.indexFixationEnd = -1;
    fixation.indexFixationStart = -1;

    // Creating the data point and adding it to the list.
    DataPoint p; p.x = x; p.y = y; p.timestamp = timeStamp; p.pupil = pupil; p.schar = schar; p.word = word;
    onlinePointsForFixation << p;

    // If there are not enough points to consider a fixation, then there is no fixation.
    if (onlinePointsForFixation.size() < parameters.getStartWindowSize()) return fixation;

    if (onlinePointsForFixation.size() == parameters.getStartWindowSize()){
        // This is the case where the dispersion is first computed.
        onlineFindDispLimits();
#ifdef ENABLE_MWA_DEBUG
        QString logstr;
        logstr = "SET: ";
        for (qint32 i = 0; i < onlinePointsForFixation.size(); i++){
            logstr = logstr + onlinePointsForFixation.at(i).toString() + " ";
        }
        logstr = logstr + "XB: " + onlineMMMX.toString() + ". YB: " + onlineMMMY.toString() + "\n";
#endif
        if ((onlineMMMX.diff() + onlineMMMY.diff()) > parameters.maxDispersion){
            // This means that this set of data cannot form a fixation, so the first value is discarded.
            onlinePointsForFixation.removeFirst();
#ifdef ENABLE_MWA_DEBUG
            logstr = logstr + "-----------";
#endif
        }
#ifdef ENABLE_MWA_DEBUG
        logger.appendStandard(logstr);
#endif
        return fixation;
    }

    // If the code got here the number of online points for fixations is greater than the minimum to form a fixation.
    // So first we update the dispersion structure.
    onlineMMMX.max = qMax(onlineMMMX.max,x);
    onlineMMMY.max = qMax(onlineMMMY.max,y);
    onlineMMMX.min = qMin(onlineMMMX.min,x);
    onlineMMMY.min = qMin(onlineMMMY.min,y);

#ifdef ENABLE_MWA_DEBUG
    QString logstr;
    logstr = "SET: ";
    for (qint32 i = 0; i < onlinePointsForFixation.size(); i++){
        logstr = logstr + onlinePointsForFixation.at(i).toString() + " ";
    }
    logstr = logstr + "XB: " + onlineMMMX.toString() + ". YB: " + onlineMMMY.toString() + "\n";
#endif

    // Computing the minimalFixations condition.
    bool minimalFixationCondition = minimalFixations && (onlinePointsForFixation.size() >= parameters.getStartWindowSize() + POINTS_OVER_MIN_TO_HOLD);

    // We now check if the maximum dispersion allowed was breached. At this point a fixation is formed. It's just a matter of how long it is.
    // For that reason if minimalFixations was enabled, the fixation is formed and we move on.
    if (((onlineMMMX.diff() + onlineMMMY.diff()) > parameters.maxDispersion) || (minimalFixationCondition)){
        // If it was then that means that the last point should removed. and a fixation is formed.
        onlinePointsForFixation.removeLast();
        fixation = onlineCalcuationOfFixationPoint();
#ifdef ENABLE_MWA_DEBUG
        logstr = logstr + "FIX FOUND: " + fixation.toString();
#endif
        onlinePointsForFixation.clear();
        // The last point might be the start of a new fixation.
        onlinePointsForFixation << p;
    }

#ifdef ENABLE_MWA_DEBUG
    logger.appendStandard(logstr);
#endif
    return fixation;
}

Fixation MovingWindowAlgorithm::finalizeOnlineFixationCalculation(){
    // Computing the final fixation if there was one.
    Fixation fixation;
    fixation.indexFixationEnd = -1;
    fixation.indexFixationStart = -1;

    if (parameters.getStartWindowSize() <= 0) {
        qDebug() << "Online fixation algorithm with zero starting window size";
        return fixation;
    }

    if (onlinePointsForFixation.size() >= parameters.getStartWindowSize()){
        fixation = onlineCalcuationOfFixationPoint();
#ifdef ENABLE_MWA_DEBUG
        logger.appendStandard("FIX FOUND: " + fixation.toString());
#endif
    }

    onlinePointsForFixation.clear();
    return fixation;
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
    f.fixStart = data.at(startI).at(tI);
    f.fixEnd   = data.at(endI).at(tI);
    f.duration = f.fixEnd - f.fixStart;
    //    qDebug() << "CENTROID" << f.x << f.y << "." << "DURATION" << f.duration << "Number of data points used: " << (endI-startI+1) << "Minimum number of data points" << parameters.getStartWindowSize();
    //    if ((endI-startI+1) == parameters.getStartWindowSize()){
    //        QList<qreal> templist;
    //        for (qint32 i = startI; i <= endI; i++){
    //            templist << data.at(i).at(tI);
    //        }
    //        qDebug() << "TIME POINTS USED IN FIXATION" << templist;
    //    }

    // Finally the indexes are stored.
    f.indexFixationStart = startI;
    f.indexFixationEnd = endI;

    return f;

}

Fixation MovingWindowAlgorithm::onlineCalcuationOfFixationPoint(){
    Fixation f;
    f.x = 0;
    f.y = 0;
    f.pupil = 0;
    f.pupilZeroCount = 0;
    f.sentence_char = 0;
    f.sentence_word = 0;
    for (qint32 i = 0; i < onlinePointsForFixation.size(); i++){
        f.x = f.x + onlinePointsForFixation.at(i).x;
        f.y = f.y + onlinePointsForFixation.at(i).y;
        f.pupil = f.pupil + onlinePointsForFixation.at(i).pupil;
        f.sentence_char = f.sentence_char + onlinePointsForFixation.at(i).schar;
        f.sentence_word = f.sentence_word + onlinePointsForFixation.at(i).word;
        if (qAbs(f.pupil) < PUPIL_ZERO_TOL){
            f.pupilZeroCount++;
        }
    }
    qreal size = onlinePointsForFixation.size();
    f.x = f.x/size;
    f.y = f.y/size;
    f.pupil = f.pupil/size;
    f.fixStart = onlinePointsForFixation.first().timestamp;
    f.fixEnd   = onlinePointsForFixation.last().timestamp;
    f.duration = f.fixEnd - f.fixStart;
    f.time = (f.fixStart + f.fixEnd)/2;
    f.indexFixationEnd = 0;
    f.indexFixationStart = 0;
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
    // OLD CODE. This created errors in computing SOME fixations. Leaving it just in case we need the "error"
    // mm.min = data.at(endI).at(col);
    mm.min = data.at(startI).at(col);

    for (qint32 i = startI+1; i <= endI; i++){
        mm.max = qMax(mm.max,data.at(i).at(col));
        mm.min = qMin(mm.min,data.at(i).at(col));
    }

    return mm;

}

void MovingWindowAlgorithm::onlineFindDispLimits(){

    onlineMMMX.max = 0;
    onlineMMMX.min = 0;
    onlineMMMY.max = 0;
    onlineMMMY.min = 0;

    if (parameters.getStartWindowSize() <= 0) {
        qDebug() << "Online fixation algorithm with zero starting window size";
        return;
    }

    // This check should not be necessary, but done just in case.
    if (onlinePointsForFixation.size() < parameters.getStartWindowSize()) return;

    onlineMMMX.max = onlinePointsForFixation.first().x;
    onlineMMMX.min = onlinePointsForFixation.last().x;
    onlineMMMY.max = onlinePointsForFixation.first().y;
    onlineMMMY.min = onlinePointsForFixation.last().y;

    for (qint32 i = 0; i < onlinePointsForFixation.size(); i++){
        onlineMMMX.max = qMax(onlineMMMX.max,onlinePointsForFixation.at(i).x);
        onlineMMMX.min = qMin(onlineMMMX.min,onlinePointsForFixation.at(i).x);
        onlineMMMY.max = qMax(onlineMMMY.max,onlinePointsForFixation.at(i).y);
        onlineMMMY.min = qMin(onlineMMMY.min,onlinePointsForFixation.at(i).y);
    }

}
