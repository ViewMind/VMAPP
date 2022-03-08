#include "targethitsearcher.h"

TargetHitSearcher::TargetHitSearcher()
{
}

void TargetHitSearcher::setTargetHitLogic(TargetHitLogic thl){
    // Securing the target hit logic, so we know which function to call.
    this->thl = thl;
}

void TargetHitSearcher::setNewTrial(const QString &id, const QList<qint32> trialSeq){
    trialID = id;
    trialSequence = trialSeq;
}

void TargetHitSearcher::setTargetBoxes(const QList<QRectF> &tBoxes){
    hitTargetBoxes = tBoxes;
}

void TargetHitSearcher::reset(){
    // Ignored in the RT logic. ONly used for the MS logic.
    allMSTargetHits.clear();
    // This will be ignored in the MS logic. Only used for RT logic.
    expectedTargetIndexInSequence = 2;
}

TargetHitSearcher::TargetHitSearcherReturn TargetHitSearcher::isHit(qreal x, qreal y, const QString &imgID){
    if (thl == TargetHitSearcher::THL_RT){
        return isHitRTVariation(x,y,imgID);
    }
    else{
        return isHitMSVariation(x,y,imgID);
    }
}

TargetHitSearcher::TargetHitSearcherReturn TargetHitSearcher::isHitRTVariation(qreal x, qreal y, const QString &imgID) {
    qint32 imgNum = imgID.toInt();
    TargetHitSearcherReturn ans;
    qint32 target_hit = -1;
    ans.sequenceCompleted = "0";
    ans.isIn = "0";
    ans.nback = "N/A";

    // Finding which target, if any, was hit by the fixation.
    for (qint32 i = 0; i < hitTargetBoxes.size(); i++){
        //if (hitTargetBoxes.at(i).contains(x,y)){
        if (FieldingParser::isHitInTargetBox(hitTargetBoxes,i,x,y)){
            target_hit = i;
            break;
        }
    }

    if ((imgNum >= 1) && (imgNum <= 3)){
        // These are the images that show the red dots.
        qint32 expTarget = trialSequence.at(imgNum-1);
        if (target_hit == expTarget) ans.isIn = "1";
    }
    else if (imgNum == 4){
        // We need to detect if the fixation sequence, here is correct.
        // The expected target is the allwasy the last in the sequence.
        if (expectedTargetIndexInSequence > -1){
            ans.isIn = "0";
            if (trialSequence.at(expectedTargetIndexInSequence) == target_hit){
                ans.nback = QString::number(expectedTargetIndexInSequence+1);
                expectedTargetIndexInSequence--;
                ans.isIn = "1";
                if (expectedTargetIndexInSequence == -1){
                    ans.sequenceCompleted = "1";
                }
            }
        }
    }

    if (target_hit < 0)  ans.targetHit = "N/A";
    else ans.targetHit = QString::number(target_hit);
    return ans;

}


TargetHitSearcher::TargetHitSearcherReturn TargetHitSearcher::isHitMSVariation(qreal x, qreal y, const QString &imgID) {
    qint32 imgNum = imgID.toInt();
    TargetHitSearcherReturn ans;
    qint32 target_hit = -1;
    ans.sequenceCompleted = "0";
    ans.isIn = "0";
    ans.nback = "N/A";

    // Finding which target, if any, was hit by the fixation.
    for (qint32 i = 0; i < hitTargetBoxes.size(); i++){
        //if (hitTargetBoxes.at(i).contains(x,y)){        
        if (FieldingParser::isHitInTargetBox(hitTargetBoxes,i,x,y)){            
            target_hit = i;
            //qDebug() << "Hit Target " << i << "from" << hitTargetBoxes;
            break;
        }
    }

    if ((imgNum >= 1) && (imgNum <= 3)){
        // These are the images that show the red dots.
        qint32 expTarget = trialSequence.at(imgNum-1);
        if (target_hit == expTarget) ans.isIn = "1";
        expectedTargetIndexInSequence = -1;
    }
    else if (imgNum == 5){
        // The target should hit the last target.
        expectedTargetIndexInSequence = 2;
    }
    else if (imgNum == 6){
        // This should hit the second target.
        expectedTargetIndexInSequence = 1;

    }
    else if (imgNum == 7){
        // This should hit the first target.
        expectedTargetIndexInSequence = 0;
    }

    // If we are in the recognition  part of the trial.
    if (expectedTargetIndexInSequence > -1){
        // We check that the target hit corresponds to the correct target hit.
        if (trialSequence.at(expectedTargetIndexInSequence) == target_hit){
            ans.nback = QString::number(expectedTargetIndexInSequence+1);
            ans.isIn = "1";
            if ((imgNum >= 5) && (imgNum <= 7)){
                allMSTargetHits << true;
            }
        }
    }


    if (imgNum == 7){
        if (allMSTargetHits.size() == 3){
            // This means all targets were hit and in ther right sequence so we can consider the sequence complete.
            ans.sequenceCompleted = "1";
        }
    }


    if (target_hit < 0)  ans.targetHit = "N/A";
    else ans.targetHit = QString::number(target_hit);
    return ans;

}
