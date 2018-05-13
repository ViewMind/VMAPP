#include "dataset.h"

DataSet::DataSet()
{
    eyes << CSVData();  // Left data
    eyes << CSVData();  // Right data
}


void DataSet::setLeftAndRightData(const CSVData &all, qint32 eyeCol){

    eyes.first().clear();
    eyes.last().clear();

    // Copying the data separated by Eye.
    for (qint32 i = 0; i < all.size(); i++){

        QList<qint32> cols = all.value(i).keys();

        QHash<qint32,QVariant> rrow;
        QHash<qint32,QVariant> lrow;

        for (qint32 j = 0; j < cols.size(); j++){

            // The Eye column is ignored.
            if (cols.at(j) == eyeCol) continue;

            // The value to copy
            QVariant v = all.at(i).value(cols.at(j));

            if (all.at(i).value(eyeCol).toBool()){
                // Right eye
                rrow[cols.at(j)] = v;
            }
            else{
                // Left eye
                lrow[cols.at(j)] = v;
            }
        }

        if (!rrow.isEmpty()) eyes[1] << rrow;
        if (!lrow.isEmpty()) eyes[0] << lrow;

    }

}

ProcessingResults DataSet::getProcessingResults(qint32 experiment,  int *eyeUsed, int eyeToUse){

    ProcessingResults results;

    // 0 means left, 1 means right and anything else means the one with the most fixations.
    switch (eyeToUse) {
    case EYE_L: *eyeUsed = EYE_L;
        break;
    case EYE_R: *eyeUsed = EYE_R;
        break;
    default:
        if (eyes.first().size() > eyes.last().size()) *eyeUsed = EYE_L;
        else *eyeUsed = EYE_R;
        break;
    }

    switch (experiment){
    case EXP_BINDING_BC:
        results = getBindingResultsFor(*eyeUsed,true);
        break;
    case EXP_BINDING_UC:
        results = getBindingResultsFor(*eyeUsed,false);
        break;
    case EXP_FIELDNG:
        results = getFieldingResultsFor(*eyeUsed);
        break;
    case EXP_READING:
        results = getReadingResultsFor(*eyeUsed);
        break;
    default:
        break;
    }

    return results;

}

ProcessingResults DataSet::getReadingResultsFor(qint32 eyeID) const{

    ProcessingResults info;
    qint32 total = eyes.at(eyeID).size();

    // First step and multiple type fixations.
    qreal first_step = 0;
    qreal mstep   = 0;
    qreal single = 0;

    // Counting variables.
    qint32 maxIndexFound = 0;
    int lastSentence = -1;
    QSet<qint32> indexesFound;
    QSet<qint32> indexesFoundMoreThanOnce;

    for (qint32 r = 0; r < eyes.at(eyeID).size(); r++){

        qint32 sentence = eyes.at(eyeID).at(r).value(CSV_READING_SENTENCE_COL).toInt();
        if (lastSentence != sentence){
            single = single +  indexesFound.size();
            indexesFound.clear();
            indexesFoundMoreThanOnce.clear();
            lastSentence = sentence;
            maxIndexFound = 0;
        }

        // Rouding the real index of the word.
        qint32 wn =  eyes.at(eyeID).at(r).value(CSV_READING_WORD_INDEX_COL).toReal();
        if ( wn >= maxIndexFound) {
            maxIndexFound = wn;
            first_step++;
        }
        else{
            mstep++;
        }

        if (!indexesFoundMoreThanOnce.contains(wn)){
            if (indexesFound.contains(wn)){
                // This means the the index has been found once already.
                indexesFound.remove(wn);
                indexesFoundMoreThanOnce << wn;
            }
            else{
                // First time the index occurs.
                indexesFound << wn;
            }
        }

    }

    // When the for ended it did not get a chance to add the last single fixations, so they are added here.
    single = single +  indexesFound.size();

    // Adding the information as text for the report generation.
    info[STAT_ID_TOTAL_FIXATIONS] = total;
    // This avoid that pesky nan tex).
    if (total == 0) total = 1;
    info[STAT_ID_FIRST_STEP_FIXATIONS] = first_step;
    info[STAT_ID_MULTIPLE_FIXATIONS] = mstep;
    info[STAT_ID_SINGLE_FIXATIONS] = single;

    return info;

}

ProcessingResults DataSet::getBindingResultsFor(qint32 eyeID, bool bound) const{

    // Variables to calculate average pupil size when trial is zero.
    qreal sum = 0;
    qreal counter = 0;

    // Variables for counting same or different response.
    qint32 correct = 0;
    qint32 total = 0;
    qint32 lastTrial = 0;

    for (qint32 r = 0; r < eyes.at(eyeID).size(); r++){
        qint32 trialID = eyes.at(eyeID).at(r).value(CSV_BINDING_TRIALID_COL).toInt();
        bool isTrial = eyes.at(eyeID).at(r).value(CSV_BINDING_ISTRIAL_COL).toBool();
        bool greaterThan50 =  (eyes.at(eyeID).at(r).value(CSV_BINDING_FIXATION_LENGHT_COL) > 50);
        if (trialID != 0){
            if (!isTrial && greaterThan50){
                sum = sum + eyes.at(eyeID).at(r).value(CSV_BINDING_PUPIL_COL).toDouble();
                counter++;
            }
            if (lastTrial != trialID){
                if (eyes.at(eyeID).at(r).value(CSV_BINDING_ISTRIAL_COL).toBool()){
                    QString trialType = eyes.at(eyeID).at(r).value(CSV_BINDING_TRIALTYPE_COL).toString();
                    QString trialResponse = eyes.at(eyeID).at(r).value(CSV_BINDING_TRIALRESPONSE_COL).toString();
                    trialType = trialType.mid(0,1);
                    trialResponse = trialResponse.toLower();

                    if (trialType == trialResponse) correct++;
                    total++;

                    lastTrial = trialID;
                }
            }
        }
    }

    ProcessingResults info;

    if (bound){
        info[STAT_ID_BC_CORRECT]   = correct;
        info[STAT_ID_BC_WRONG]     = total - correct;
        if (eyeID == EYE_R) info[STAT_ID_BC_PUPIL_R]   = sum/counter;
        else info[STAT_ID_BC_PUPIL_L]   = sum/counter;
    }
    else{
        info[STAT_ID_UC_CORRECT] = correct;
        info[STAT_ID_UC_WRONG]   = total - correct;
        if (eyeID == EYE_R) info[STAT_ID_UC_PUPIL_R]   = sum/counter;
        else info[STAT_ID_UC_PUPIL_L]   = sum/counter;
    }

    return info;

}

ProcessingResults DataSet::getFieldingResultsFor(qint32 eyeID) const{

    ProcessingResults results;

    QHash<qint32, QSet<qint32> > hitCounter;
    qreal trialCounter = 0;
    qint32 lastID = -1;

    for (qint32 r = 0; r < eyes.at(eyeID).size(); r++){

        qint32 trialID = eyes.at(eyeID).at(r).value(CSV_FIELDING_TRIALID_COL).toInt();

        if (trialID != lastID){
            trialCounter++;
            lastID = trialID;
        }

        if (eyes.at(eyeID).at(r).value(CSV_FIELDING_TARGET_HIT_COL).toBool()){
            hitCounter[trialID]
                    << eyes.at(eyeID).at(r).value(CSV_FIELDING_IMGNUMBER_COL).toInt();
        }

    }

    qreal hits = 0;
    QList<qint32> trialsHit = hitCounter.keys();
    for (qint32 i = 0; i < trialsHit.size(); i++){
        hits = hits + hitCounter.value(trialsHit.at(i)).size();
    }


    results[STAT_ID_FIELDING_HIT_COUNT] = hits;  // Each trial has 3 possible hits.
    results[STAT_ID_FIELDING_HIT_POSSIBLE] = trialCounter*3;

    return results;

}

//------------------------------------ Filtering Functions ------------------------------------------------

void DataSet::removeFirstNDifferent(qint32 N, qint32 idCol){
    for (qint32 e = 0; e < 2; e++){
        QString last = "";
        qint32 counter = 0;

        while (!eyes.at(e).isEmpty()){
            QString v = eyes.at(e).first().value(idCol).toString();
            if (!last.isEmpty() && (v != last)) {
                counter++;
                if (counter >= N) break;
            }
            last = v;
            eyes[e].removeFirst();
        }
    }
}

void DataSet::roundColumnValues(qint32 idCol){
    for (qint32 e = 0; e < 2; e++){
        for (qint32 i = 0; i < eyes.at(e).size(); i++){
            // Rouding the real index of the word.
            qreal wn_real  = eyes.at(e).at(i).value(idCol).toReal();
            qint32 wn = qFloor(wn_real);
            qreal diff = wn_real - (qreal)(wn);
            if (diff >= 0.5){
                wn++;
            }
            eyes[e][i][idCol] = wn;
        }
    }
}

void DataSet::removeAllWithValueZero(qint32 idCol){

    for (qint32 e = 0; e < 2; e++){
        qint32 row = 0;
        while (row < eyes.at(e).size()){

            if (eyes.at(e).at(row).value(idCol).toReal() == 0){
                eyes[e].removeAt(row);
            }
            else row++;
        }
    }

}

void DataSet::filterRowsWithColOutsideRange(qreal min, qreal max, qint32 idCol){

    for (qint32 e = 0; e < 2; e++){
        qint32 row = 0;
        while (row < eyes.at(e).size()){
            qreal v = eyes.at(e).at(row).value(idCol).toReal();
            if ((v <= min) || (v >= max)){
                eyes[e].removeAt(row);
            }
            else row++;
        }
    }

}
