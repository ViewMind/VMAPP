#include "edpfielding.h"

EDPFielding::EDPFielding(ConfigurationManager *c):EDPBase(c)
{
    targetHitSearcher.setTargetHitLogic(TargetHitSearcher::THL_MS);
}

bool EDPFielding::doEyeDataProcessing(const QString &data){

    // This just creates the headeer.
    initializeFieldingDataMatrix();

    // Parsing the fielding experiment.
    if (!parser.parseFieldingExperiment(eyeFixations.experimentDescription,config->getReal(CONFIG_RESOLUTION_WIDTH),config->getReal(CONFIG_RESOLUTION_HEIGHT),
                                        config->getReal(CONFIG_FIELDING_XPX_2_MM),config->getReal(CONFIG_FIELDING_YPX_2_MM))){
        error = "Error parsing NBackMS experiment: " + parser.getError();
        return false;
    }


    QStringList lines = data.split("\n");
    QList<QRectF> tBoxes = parser.getHitTargetBoxes();
    qreal hitW = tBoxes.first().width();
    qreal hitH = tBoxes.first().height();

    // The target boxes set for searching for target hits.
    targetHitSearcher.setTargetBoxes(tBoxes);

    // The center is the actual center of the screen.
    centerX = config->getReal(CONFIG_RESOLUTION_WIDTH)/2;
    centerY = config->getReal(CONFIG_RESOLUTION_HEIGHT)/2;

    // Using the width and height of the target hit box to detect the center.
    centerMinX = centerX - hitW/2.0;
    centerMinY = centerY - hitH/2.0;
    centerMaxX = centerX + hitW/2.0;
    centerMaxY = centerY + hitH/2.0;

    // This will have all the data from a single image.
    DataMatrix imageData;

    QString id;
    QString imageNumber;

    // Variables used to count the number of trials
    numberOfTrials = 0;
    QString lastID = "";

    // The trial sequence
    QList<qint32> trialSequence;

    for(int i = 0; i < lines.size(); i++){

        QString line = lines.at(i);

        //qWarning() << "Reading line" << line;

        QStringList tokens = line.split(' ',QString::SkipEmptyParts);

        // The data is contained between lines that have four tokens (id imageNumber x_target y_target). These last two values are ignored in the new parsing.
        if ( (tokens.size() == 4) ){

            // Saving stored data
            if (!imageData.isEmpty()){

                // Adding data to matrix
                //qWarning() << "Adding data to matrix of size" << imageData.size() << "for ID" << lastID;

                //qint32 targetBoxID = parser.getTargetBoxForImageNumber(id,imageNumber.toInt());
                //if ((targetBoxID < 0) || (targetBoxID > 5)){
                //    error = "Error trying to get target box for fielding trial : " + id + " and image number " + imageNumber + ". Got: " + QString::number(targetBoxID);
                //    return false;
                //}
                appendDataToFieldingMatrix(imageData,id,imageNumber,trialSequence);

                // Clearing the image data
                //qWarning() << "CLEARING";
                imageData.clear();
            }

            // Saving new id and trial and center of the circle.
            id = tokens.first().trimmed();
            imageNumber = tokens.at(1).trimmed();
            trialSequence = parser.getSequenceForTrial(id);

            //QString number = tokens.at(2).trimmed();
            //targetX = number.toDouble();
            //number = tokens.at(3).trimmed();
            //targetY = number.toDouble();

            // The new description of the Fielding experiment uses 6 targets, because NBACKVS can have up to six targets.
            // However for NBACK RT we only need the first 3.
            if ((trialSequence.size() == NBACK_EXP_DESCRIPTION_NUM_OF_TARGETS)){
                while (trialSequence.size() > NBACK_MS_NUM_OF_TARGETS){
                    trialSequence.pop_back();
                }
            }

            if (trialSequence.size() != 3){
                error = "Error trying to get sequence for N Back MS trial : " + id +  ". Got a sequence of size " + QString::number(trialSequence.size());
                return false;
            }

            if (lastID != id){
                numberOfTrials++;
                lastID = id;
            }

            // There is nothing more to do.
            continue;
        }

        // If the program got here, this is another row for image data.
        DataRow row;
        for (qint32 i = 0; i < tokens.size(); i++){
            row << tokens.at(i).toDouble();
        }
        if (!imageData.isEmpty()){
            if (imageData.last().size() != row.size()){
                error = "Something is wrong with the data format. Different size rows in the data matrix from line: " + line;
                return false;
            }
        }
        imageData << row;
    }

    // Saving stored data of the last image.
    if (!imageData.isEmpty()){

        // Adding data to matrix
        //qWarning() << "Adding data to matrix of size" << imageData.size() << "for ID" << lastID;
        //qint32 targetBoxID = parser.getTargetBoxForImageNumber(id,imageNumber.toInt());
        //if ((targetBoxID < 0) || (targetBoxID > 5)){
        //    error = "Error trying to get target box for fielding trial : " + id + " and image number " + imageNumber + ". Got: " + QString::number(targetBoxID);
        //    return false;
        //}
        appendDataToFieldingMatrix(imageData,id,imageNumber,trialSequence);

        // Clearing the image data
        //qWarning() << "CLEARING";
        imageData.clear();
    }

//    // Up until this point the CSV matrix is only a list of QStringList, so now the response time is computed per eye, stored in the data structures and finally the CSV file is written to disk
//    //qDebug() << "PROCESSING LEFT";
//    computeResponseTimes(&lResponseTimeStruct);
//    //qDebug() << "PROCESSING RIGHT";
//    computeResponseTimes(&rResponseTimeStruct);
//    //qDebug() << "STORING LEFT";
//    fillResponseTimes(&ldata,&lResponseTimeStruct);
//    //qDebug() << "STORING RIGHT";
//    fillResponseTimes(&rdata,&rResponseTimeStruct);
    return finalizeFieldingDataMatrix();

}

void EDPFielding::initializeFieldingDataMatrix(){

    csvHeader << "suj"          //0
              << "idtrial"      //1
              << "imgnum"       //2
              << "target"       //3
              << "target_hit"   //4
              << "nback"        //5
              << "trial_seq"    //6
              << "seq_comp"     //7
              << "dur"          //8
              << "ojoDI"        //9
              << "latSac"       //10
              << "amp_sacada"   //11
              << "resp_time";   //12

}

bool EDPFielding::finalizeFieldingDataMatrix(){
    QFile file(outputFile);
    if (file.exists()){
        file.remove();
    }

    if (!file.open(QFile::WriteOnly)){
        error = "Could not open " + outputFile + " for writing";
        return false;
    }

    QTextStream writer(&file);

    writer << csvHeader.join(",") << "\n";
    for (qint32 i = 0; i < rdata.size(); i++){
        writer << rdata.at(i).join(",") << "\n";
    }
    for (qint32 i = 0; i < ldata.size(); i++){
        writer << ldata.at(i).join(",") << "\n";
    }

    file.close();

    // If enabled raw data file is created.
    if (config->containsKeyword(CONFIG_SAVE_RAW_DATA_CSV)){
        if (config->getBool(CONFIG_SAVE_RAW_DATA_CSV)){
            return finalizeRawDataFile();
        }
    }

    return true;
}



//void EDPFielding::computeResponseTimes(ResponseTimeStruct *responseTimeStruct){

//    QStringList trialIDs = responseTimeStruct->keys();

//    for (qint32 i = 0; i < trialIDs.size(); i++){

//        FieldingTrialTimes ftt = responseTimeStruct->value(trialIDs.at(i));

//        MinMaxTimeStamp img5, img6, img7;
//        img5.reset(); img6.reset(); img7.reset();

//        if (ftt.contains("5")) img5 = ftt.value("5");
//        if (ftt.contains("6")) img6 = ftt.value("6");
//        if (ftt.contains("7")) img7 = ftt.value("7");

//        // Computing image five.
//        if ((img5.crossTime > 0) && (img5.firstHit > 0) && (img5.crossTime < img5.firstHit)) img5.timeToStore = img5.firstHit - img5.crossTime;

//        // Computing image 6.
//        if ( (img6.firstHit > 0) && (img5.lastHit > 0) && (img6.firstHit > img5.lastHit)) img6.timeToStore = img6.firstHit - img5.lastHit;

//        // Computing image 7
//        if ( (img7.firstHit > 0) && (img6.lastHit > 0) && (img7.firstHit > img6.lastHit)) img7.timeToStore = img7.firstHit - img6.lastHit;

//        // Restoring the values with the new computed values.
//        ftt["5"] = img5;
//        ftt["6"] = img6;
//        ftt["7"] = img7;

//        //qDebug() << trialIDs.at(i) << "5: " + img5.toString() << "6: " + img6.toString() << "7: " + img7.toString();

//        responseTimeStruct->insert(trialIDs.at(i),ftt);

//    }

//}


//void EDPFielding::fillResponseTimes(QList<QStringList> *pdata, ResponseTimeStruct *respTimeStruct){
//    for (qint32 i = 0; i < pdata->size(); i++){
//        QString trialID = pdata->at(i).at(CSV_TRIALID);
//        QString imgNum  = pdata->at(i).at(CSV_IMGNUM);
//        MinMaxTimeStamp mmts = respTimeStruct->value(trialID).value(imgNum);
//        //qDebug() << trialID << imgNum << mmts.toString();
//        if (mmts.timeToStore > 0){
//            QStringList row = pdata->at(i);
//            row[CSV_RESP_TIME] = QString::number(mmts.timeToStore);
//            pdata->replace(i,row);
//        }
//    }
//}

void EDPFielding::appendDataToFieldingMatrix(const DataMatrix &data,
                                             const QString &trialID,
                                             const QString &imgID,
                                             const QList<qint32> &trialSequence){

    qreal freqCheck = calculateSamplingFrequency(data,FIELDING_TI);
    qreal freq = config->getReal(CONFIG_SAMPLE_FREQUENCY);
    if (qAbs(freqCheck - freq) > 0.1*freq){
        samplingFrequencyCheck << "NBack MS Frequency Check failed at: " + imgID
                                  + ". Expected Frequency is " + QString::number(freq)
                                  + ". Measured: " + QString::number(freqCheck);
    }

    // If enabled raw data file is created.
    if (config->containsKeyword(CONFIG_SAVE_RAW_DATA_CSV)){
        if (config->getBool(CONFIG_SAVE_RAW_DATA_CSV)){
            appendToRawDataCSVFile(data,trialID,imgID,FIELDING_TI,FIELDING_XR,FIELDING_XL,FIELDING_YR,FIELDING_YL);
        }
    }

    // Calculating the fixations for each eye.;
    Fixations fL = mwa.computeFixations(data,FIELDING_XL,FIELDING_YL,FIELDING_TI);
    Fixations fR = mwa.computeFixations(data,FIELDING_XR,FIELDING_YR,FIELDING_TI);

    eyeFixations.left.append(fL);
    eyeFixations.right.append(fR);
    QStringList id;
    id << imgID << trialID;
    eyeFixations.trialID.append(id);

    // The sacade latency is considered the first outside the given radious within the center of the image.
    qreal sacLatL = 0;
    qreal sacLatR = 0;
    qreal startTime = data.first().at(FIELDING_TI);
    qreal sqrTol = pixelsInSacadicLatency*pixelsInSacadicLatency;

    // For the Left eye.
    for (qint32 i = 0; i < data.size(); i++){
        qreal sqrD = qPow(data.at(i).at(FIELDING_XL) - centerX,2)+qPow(data.at(i).at(FIELDING_YL) - centerY,2);
        if (sqrD > sqrTol){
            sacLatL = data.at(i).at(FIELDING_TI) - startTime;
            break;
        }
    }

    // For the Right eye.
    for (qint32 i = 0; i < data.size(); i++){
        qreal sqrD = qPow(data.at(i).at(FIELDING_XR) - centerX,2)+qPow(data.at(i).at(FIELDING_YR) - centerY,2);
        if (sqrD > sqrTol){
            sacLatR = data.at(i).at(FIELDING_TI) - startTime;
            break;
        }
    }

    SacadeAmplitudeCalculator sac;
    TargetHitSearcher::TargetHitSearcherReturn targetHitSearcherReturn;
    targetHitSearcher.setNewTrial(trialID,trialSequence);

    QStringList trialSeqStr;
    for (qint32 i = 0; i < trialSequence.size(); i++){
        trialSeqStr << QString::number(trialSequence.at(i));
    }

    sac.reset();
    targetHitSearcher.reset();

    // Computing response time is set to zero to keep output CSV format compatible with NBack RT CSV.
    QString responseTime = "0";

    //qDebug() << "LEFT EYE" << trialID << "Num FIX: " << fL.size();
    for (qint32 i = 0; i < fL.size(); i++){

        // Computing the expected target and the target hit parameter for the image.
        Fixation f = fL.at(i);
        //qDebug() << "CHECKING FIXATION" << i;
        targetHitSearcherReturn = targetHitSearcher.isHit(f.x,f.y,imgID);

        QStringList left;
        left << subjectIdentifier
             << trialID
             << imgID
             << targetHitSearcherReturn.targetHit
             << targetHitSearcherReturn.isIn
             << targetHitSearcherReturn.nback
             << trialSeqStr.join("|")
             << targetHitSearcherReturn.sequenceCompleted
             << QString::number(f.duration)
             << eyeLeftCSV
             << QString::number(sacLatL)
             << QString::number(sac.calculateSacadeAmplitude(f.x,f.y,monitorGeometry))
             << responseTime;
        ldata << left;

    }

    //qDebug() << "SAVING LEFT"  << trialID << imgID << "->" << mmts.toString();

    targetHitSearcher.reset();
    sac.reset();

    //qDebug() << "RIGHT EYE" << trialID << "Num FIX: " << fR.size();
    for (qint32 i = 0; i < fR.size(); i++){
        // Computing the expected target and the target hit parameter for the image.
        Fixation f = fR.at(i);
        //qDebug() << "CHECKING FIXATION" << i;
        targetHitSearcherReturn = targetHitSearcher.isHit(f.x,f.y,imgID);

        QStringList right;
        right << subjectIdentifier
              << trialID
              << imgID
              << targetHitSearcherReturn.targetHit
              << targetHitSearcherReturn.isIn
              << targetHitSearcherReturn.nback
              << trialSeqStr.join("|")
              << targetHitSearcherReturn.sequenceCompleted
              << QString::number(f.duration)
              << eyeRightCSV
              << QString::number(sacLatR)
              << QString::number(sac.calculateSacadeAmplitude(f.x,f.y,monitorGeometry))
              << responseTime;
        rdata << right;

    }

    //qDebug() << "SAVING RIGHT"  << trialID << imgID << "->" << mmts.toString();

/// OLD Version of this function kept for legacy reasons.
//    eyeFixations.left.append(fL);
//    eyeFixations.right.append(fR);
//    QStringList id;
//    id << imgID << trialID;
//    eyeFixations.trialID.append(id);

//    // The sacade latency is considered the first outside the given radious within the center of the image.
//    qreal sacLatL = 0;
//    qreal sacLatR = 0;
//    qreal startTime = data.first().at(FIELDING_TI);
//    qreal sqrTol = pixelsInSacadicLatency*pixelsInSacadicLatency;

//    // For the Left eye.
//    for (qint32 i = 0; i < data.size(); i++){
//        qreal sqrD = qPow(data.at(i).at(FIELDING_XL) - centerX,2)+qPow(data.at(i).at(FIELDING_YL) - centerY,2);
//        if (sqrD > sqrTol){
//            sacLatL = data.at(i).at(FIELDING_TI) - startTime;
//            break;
//        }
//    }

//    // For the Right eye.
//    for (qint32 i = 0; i < data.size(); i++){
//        qreal sqrD = qPow(data.at(i).at(FIELDING_XR) - centerX,2)+qPow(data.at(i).at(FIELDING_YR) - centerY,2);
//        if (sqrD > sqrTol){
//            sacLatR = data.at(i).at(FIELDING_TI) - startTime;
//            break;
//        }
//    }

//    // Indicates if fixation is within target.
//    int isIn;

//    SacadeAmplitudeCalculator sac;
//    sac.reset();

//    MinMaxTimeStamp mmts;
//    mmts.reset();

//    QHash<QString,MinMaxTimeStamp> responseTime;

//    for (qint32 i = 0; i < fL.size(); i++){

//        if (hitTargetBoxes.at(trialSequence).contains(fL.at(i).x,fL.at(i).y)) isIn = 1;
//        else isIn = 0;

////        if (fL.at(i).isIn(minX,maxX,minY,maxY)) isIn = 1;
////        else isIn = 0;

//        QStringList left;
//        left << subjectIdentifier
//             << trialID
//             << imgID
//             << QString::number(isIn)
//             << QString::number(fL.at(i).duration)
//             << eyeLeftCSV
//             << QString::number(sacLatL)
//             << QString::number(sac.calculateSacadeAmplitude(fL.at(i).x,fL.at(i).y,monitorGeometry))
//             << "N/A";
//        ldata << left;

//        // For efficiency, I do not check if imgID is 5, 6 or 7. Beucase the extra data is very small.
//        // qDebug() << trialID << imgID << isIn;
//        if (isIn == 1){
//            if (mmts.firstHit < 0){
//                mmts.firstHit = fL.at(i).fixStart;
//                mmts.lastHit = fL.at(i).fixStart;
//            }
//            else mmts.lastHit = fL.at(i).fixStart;
//        }
//        else{
//            // Might be a cross
//            if ((imgID == "5") && (mmts.crossTime < 0)){
//                if (fL.at(i).isIn(centerMinX,centerMaxX,centerMinY,centerMaxY)){
//                    mmts.crossTime = fL.at(i).fixStart;
//                }
//            }
//        }
//    }

//    //qDebug() << "SAVING LEFT"  << trialID << imgID << "->" << mmts.toString();

//    // Saving the structure to calculate the response times.
//    responseTime = lResponseTimeStruct.value(trialID);
//    responseTime[imgID] = mmts;
//    lResponseTimeStruct[trialID] = responseTime;

//    sac.reset();
//    mmts.reset();

//    for (qint32 i = 0; i < fR.size(); i++){

//        //if (fR.at(i).isIn(minX,maxX,minY,maxY)) isIn = 1;
//        if (hitTargetBoxes.at(trialSequence).contains(fR.at(i).x,fR.at(i).y)) isIn = 1;
//        else isIn = 0;

//        QStringList right;
//        right << subjectIdentifier
//              << trialID
//              << imgID
//              << QString::number(isIn)
//              << QString::number(fR.at(i).duration)
//              << eyeRightCSV
//              << QString::number(sacLatR)
//              << QString::number(sac.calculateSacadeAmplitude(fR.at(i).x,fR.at(i).y,monitorGeometry))
//              << "N/A";
//        rdata << right;

//        // For efficiency, I do not check if imgID is 5, 6 or 7. Beucase the extra data is very small.
//        if (isIn == 1){
//            if (mmts.firstHit < 0){
//                mmts.firstHit = fR.at(i).fixStart;
//                mmts.lastHit = fR.at(i).fixStart;
//            }
//            else mmts.lastHit = fR.at(i).fixStart;
//        }
//        else{
//            // Might be a cross
//            if ((imgID == "5") && (mmts.crossTime < 0)){
//                if (fR.at(i).isIn(centerMinX,centerMaxX,centerMinY,centerMaxY)){
//                    mmts.crossTime = fR.at(i).fixStart;
//                }
//            }
//        }

//    }

//    //qDebug() << "SAVING RIGHT"  << trialID << imgID << "->" << mmts.toString();

//    // Saving the structure to calculate the response times.
//    responseTime = rResponseTimeStruct.value(trialID);
//    responseTime[imgID] = mmts;
//    rResponseTimeStruct[trialID] = responseTime;

}
