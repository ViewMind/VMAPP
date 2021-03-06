#include "edpnbackrt.h"

EDPNBackRT::EDPNBackRT(ConfigurationManager *c):EDPBase(c)
{
    // Setting the target search logic to be appropiate for RT
    targetHitSearcher.setTargetHitLogic(TargetHitSearcher::THL_RT);
}

bool EDPNBackRT::doEyeDataProcessing(const QString &data){

    // This just creates the headeer.
    initializeFieldingDataMatrix();

    // Parsing the fielding experiment.
    if (!parser.parseFieldingExperiment(eyeFixations.experimentDescription,
                                        config->getReal(CONFIG_RESOLUTION_WIDTH),config->getReal(CONFIG_RESOLUTION_HEIGHT),
                                        config->getReal(CONFIG_FIELDING_XPX_2_MM),config->getReal(CONFIG_FIELDING_YPX_2_MM))){
        error = "Error parsing n back rt experiment: " + parser.getError();
        return false;
    }

    QStringList lines = data.split("\n");
    QList<QRectF> tBoxes = parser.getHitTargetBoxes();
    qreal hitW = tBoxes.first().width();
    qreal hitH = tBoxes.first().height();

    // X and Y margin for look detection
    // The center is the actual center of the screen.
    centerX = config->getReal(CONFIG_RESOLUTION_WIDTH)/2;
    centerY = config->getReal(CONFIG_RESOLUTION_HEIGHT)/2;

    centerMinX = centerX - hitW/2.0;
    centerMinY = centerY - hitH/2.0;
    centerMaxX = centerX + hitW/2.0;
    centerMaxY = centerY + hitH/2.0;

    targetHitSearcher.setTargetBoxes(tBoxes);

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

        // The data is contained between lines that have four tokens (id imageNumber)
        if ( (tokens.size() == 2) ){

            //qDebug() << "This row is a header";

            // Saving stored data
            if (!imageData.isEmpty()){

                // Adding data to matrix
                //qDebug() << "Adding data to matrix of size" << imageData.size() << "for ID" << lastID;

                appendDataToFieldingMatrix(imageData,id,imageNumber,trialSequence);

                // Clearing the image data
                //qDebug() << "CLEARING";
                imageData.clear();
            }

            // Saving new id and trial and center of the circle.
            id = tokens.first().trimmed();
            imageNumber = tokens.at(1).trimmed();
            trialSequence = parser.getSequenceForTrial(id);

            // The new description of the Fielding experiment uses 6 targets, because NBACKVS can have up to six targets.
            // However for NBACK RT we only need the first 3.
            if ((trialSequence.size() == NBACK_EXP_DESCRIPTION_NUM_OF_TARGETS)){
                while (trialSequence.size() > NBACK_RT_NUM_OF_TARGETS){
                    trialSequence.pop_back();
                }
            }

            if (trialSequence.size() != 3){
                error = "Error trying to get sequence for N Back RT trial : " + id +  ". Got a sequence of size " + QString::number(trialSequence.size());
                return false;
            }

            if (lastID != id){
                numberOfTrials++;
                lastID = id;
            }

            // There is nothing more to do.
            continue;
        }

        //qDebug() << "Adding another row of image data" << tokens;

        // If the program got here, this is another row for image data.
        DataRow row;
        for (qint32 i = 0; i < tokens.size(); i++){
            row << tokens.at(i).toDouble();
        }
        //qDebug() << "Finished adding the last row";
        if (!imageData.isEmpty()){
            if (imageData.last().size() != row.size()){
                error = "Something is wrong with the data format. Different size rows in the data matrix from line: " + line;
                //qDebug() << "Returning false";
                return false;
            }
        }
        imageData << row;

        //qDebug() << "Finished adding the row of image data";
    }

    //qDebug() << "Finished all lines";

    // Saving stored data of the last image.
    if (!imageData.isEmpty()){

        // Adding data to matrix
        //qDebug() << "Adding data to matrix of size" << imageData.size() << "for ID" << lastID;
        appendDataToFieldingMatrix(imageData,id,imageNumber,trialSequence);

        // Clearing the image data
        //qDebug() << "CLEARING";
        imageData.clear();
    }

    //qDebug() << "Returning after finalizing";

    return finalizeFieldingDataMatrix();

}

void EDPNBackRT::initializeFieldingDataMatrix(){

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

bool EDPNBackRT::finalizeFieldingDataMatrix(){
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


void EDPNBackRT::appendDataToFieldingMatrix(const DataMatrix &data,
                                            const QString &trialID,
                                            const QString &imgID,
                                            const QList<qint32> &trialSequence){

    qreal freqCheck = calculateSamplingFrequency(data,FIELDING_TI);
    qreal freq = config->getReal(CONFIG_SAMPLE_FREQUENCY);
    if (qAbs(freqCheck - freq) > 0.1*freq){
        samplingFrequencyCheck << "NBack RT Frequency Check failed at: " + imgID
                                  + ". Expected Frequency is " + QString::number(freq)
                                  + ". Measured: " + QString::number(freqCheck);
    }

    // Calculating the fixations for each eye.;
#ifdef ENABLE_MWA_DEBUG
    mwa.setLog("mwa_offline.log");
    mwa.logMessage("TRIAL " + trialID + " - " + imgID + ". LEFT FIXATIONS");
#endif
    Fixations fL = mwa.computeFixations(data,FIELDING_XL,FIELDING_YL,FIELDING_TI);
#ifdef ENABLE_MWA_DEBUG
    mwa.logMessage("TRIAL " + trialID + " - " + imgID + ". RIGHT FIXATIONS");
#endif
    Fixations fR = mwa.computeFixations(data,FIELDING_XR,FIELDING_YR,FIELDING_TI);

    // If enabled raw data file is created.
    if (config->containsKeyword(CONFIG_SAVE_RAW_DATA_CSV)){
        if (config->getBool(CONFIG_SAVE_RAW_DATA_CSV)){
            appendToRawDataCSVFile(data,trialID,imgID,FIELDING_TI,FIELDING_YR,FIELDING_XL,FIELDING_XR,FIELDING_YL);
        }
    }

    //    ///////////////////////////// THIS CODE IS FOR DEBUGGING THE ONLINE FIXATION ALGORITHM
    //    // Computing fixation online and comparing the results.
    //    Fixations fL2, fR2;
    //    Fixation workF;
    //#ifdef ENABLE_MWA_DEBUG
    //    mwa.setLog("mwa_online.log");
    //    mwa.logMessage("TRIAL " + trialID + " - " + imgID + ". LEFT FIXATIONS");
    //#endif
    //    mwa.finalizeOnlineFixationCalculation(); // Making sure that everything is reset.
    //    for (qint32 i = 0; i  < data.size(); i++){
    //        workF = mwa.calculateFixationsOnline(data.at(i).at(FIELDING_XL),data.at(i).at(FIELDING_YL),data.at(i).at(FIELDING_TI));
    //        if (workF.isValid()) fL2 << workF;
    //    }
    //    workF = mwa.finalizeOnlineFixationCalculation();
    //    if (workF.isValid()) fL2 << workF;

    //#ifdef ENABLE_MWA_DEBUG
    //    mwa.logMessage("TRIAL " + trialID + " - " + imgID + ". RIGHT FIXATIONS");
    //#endif
    //    for (qint32 i = 0; i  < data.size(); i++){
    //        workF = mwa.calculateFixationsOnline(data.at(i).at(FIELDING_XR),data.at(i).at(FIELDING_YR),data.at(i).at(FIELDING_TI));
    //        if (workF.isValid()) fR2 << workF;
    //    }
    //    workF = mwa.finalizeOnlineFixationCalculation();
    //    if (workF.isValid()) fR2 << workF;

    //    qDebug() <<  "TRIAL ID" << trialID << "IMG" << imgID;
    //    qint32 size = qMax(fL.size(),fL2.size());
    //    if (fL.size() != fL2.size()){
    //        qDebug() << "FL SIZE DIFFERENCE. OFFLINE" << fL.size() << "ONLINE" << fL2.size();
    //        for (qint32 i = 0; i < size; i++){
    //            QString s = QString::number(i) + ". ";
    //            if (i < fL.size()){
    //                s = s + "OFFLINE: " + fL.at(i).toString();
    //            }
    //            else{
    //                s = s + "OFFLINE: N/A";
    //            }
    //            s  = s + ". ";
    //            if (i < fL2.size()){
    //                s = s + "ONLINE: " + fL2.at(i).toString();
    //            }
    //            else{
    //                s = s + "ONLINE: N/A";
    //            }
    //            qDebug().noquote() << s;
    //        }
    //    }
    //    else{
    //        // Same amount.
    //        for (qint32 i = 0; i < size; i++){
    //            if (!fL.at(i).isSame(fL2.at(i))){
    //                qDebug().noquote() << "DIFF AT" << i << "OFFLINE" << fL.at(i).toString() << "ONLINE" << fL2.at(i).toString();
    //            }
    //        }
    //    }
    //    ///////////////////////////// THIS CODE IS FOR DEBUGGING THE ONLINE FIXATION ALGORITHM


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

    // Computing response time only on image type 4.
    QString responseTime = "N/A";
    if (imgID == "4"){
        responseTime = QString::number(data.last().at(FIELDING_TI) - data.first().at(FIELDING_TI));
    }

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

}


