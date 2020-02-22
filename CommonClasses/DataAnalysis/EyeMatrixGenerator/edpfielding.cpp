#include "edpfielding.h"

EDPFielding::EDPFielding(ConfigurationManager *c):EDPBase(c)
{
}

bool EDPFielding::doEyeDataProcessing(const QString &data){

    // This just creates the headeer.
    initializeFieldingDataMatrix();

    QStringList lines = data.split("\n");

    // The header format is HEADERNAME size X Y, so 2 and 3 have the center coordinates.
    centerX = config->getReal(CONFIG_RESOLUTION_WIDTH)/2;
    centerY = config->getReal(CONFIG_RESOLUTION_WIDTH)/2;

    centerMinX = centerX - static_cast<qreal>(RECT_WIDTH/fieldingKx)/2 - dW;
    centerMinY = centerY - static_cast<qreal>(RECT_HEIGHT/fieldingKy)/2 - dH;
    centerMaxX = centerX + static_cast<qreal>(RECT_WIDTH/fieldingKy)/2 + dW;
    centerMaxY = centerY + static_cast<qreal>(RECT_HEIGHT/fieldingKy)/2 + dH;

    // This will have all the data from a single image.
    DataMatrix imageData;

    QString id;
    QString imageNumber;

    // Variables used to count the number of trials
    numberOfTrials = 0;
    QString lastID = "";

    // X and Y margin for look detection
    qreal k = fieldingMargin/200.0; // This is divided by a 100 to get it to number between 0 and 1 and divided by two to get half of that.
    fieldingKx = config->getReal(CONFIG_FIELDING_XPX_2_MM);
    fieldingKy = config->getReal(CONFIG_FIELDING_YPX_2_MM);
    dH = static_cast<qreal>(RECT_HEIGHT/fieldingKx)*k;
    dW = static_cast<qreal>(RECT_WIDTH/fieldingKy)*k;

    //qDebug() << "dH and dW" << dH << dW;

    qreal targetX, targetY;

    for(int i = 0; i < lines.size(); i++){

        QString line = lines.at(i);

        //qWarning() << "Reading line" << line;

        QStringList tokens = line.split(' ',QString::SkipEmptyParts);

        // The data is contained between lines that have four tokens (id imageNumber x_target y_target)
        if ( (tokens.size() == 4) ){

            // Saving stored data
            if (!imageData.isEmpty()){

                // Adding data to matrix
                //qWarning() << "Adding data to matrix of size" << imageData.size() << "for ID" << lastID;
                appendDataToFieldingMatrix(imageData,id,imageNumber,targetX,targetY);

                // Clearing the image data
                //qWarning() << "CLEARING";
                imageData.clear();
            }

            // Saving new id and trial and center of the circle.
            id = tokens.first().trimmed();
            imageNumber = tokens.at(1).trimmed();
            QString number = tokens.at(2).trimmed();
            targetX = number.toDouble();
            number = tokens.at(3).trimmed();
            targetY = number.toDouble();

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
        appendDataToFieldingMatrix(imageData,id,imageNumber,targetX,targetY);

        // Clearing the image data
        //qWarning() << "CLEARING";
        imageData.clear();
    }

    // Up until this point the CSV matrix is only a list of QStringList, so now the response time is computed per eye, stored in the data structures and finally the CSV file is written to disk
    //qDebug() << "PROCESSING LEFT";
    computeResponseTimes(&lResponseTimeStruct);
    //qDebug() << "PROCESSING RIGHT";
    computeResponseTimes(&rResponseTimeStruct);
    //qDebug() << "STORING LEFT";
    fillResponseTimes(&ldata,&lResponseTimeStruct);
    //qDebug() << "STORING RIGHT";
    fillResponseTimes(&rdata,&rResponseTimeStruct);
    return finalizeFieldingDataMatrix();

}

void EDPFielding::initializeFieldingDataMatrix(){

    csvHeader << "suj"       //0
              << "idtrial"      //1
              << "imgnum"       //2
              << "target_hit"   //3
              << "dur"          //4
              << "ojoDI"        //5
              << "latSac"       //6
              << "amp_sacada"   //7
              << "resp_time";   //8

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

    return true;
}



void EDPFielding::computeResponseTimes(ResponseTimeStruct *responseTimeStruct){

    QStringList trialIDs = responseTimeStruct->keys();

    for (qint32 i = 0; i < trialIDs.size(); i++){

        FieldingTrialTimes ftt = responseTimeStruct->value(trialIDs.at(i));

        MinMaxTimeStamp img5, img6, img7;
        img5.reset(); img6.reset(); img7.reset();

        if (ftt.contains("5")) img5 = ftt.value("5");
        if (ftt.contains("6")) img6 = ftt.value("6");
        if (ftt.contains("7")) img7 = ftt.value("7");

        // Computing image five.
        if ((img5.crossTime > 0) && (img5.firstHit > 0) && (img5.crossTime < img5.firstHit)) img5.timeToStore = img5.firstHit - img5.crossTime;

        // Computing image 6.
        if ( (img6.firstHit > 0) && (img5.lastHit > 0) && (img6.firstHit > img5.lastHit)) img6.timeToStore = img6.firstHit - img5.lastHit;

        // Computing image 7
        if ( (img7.firstHit > 0) && (img6.lastHit > 0) && (img7.firstHit > img6.lastHit)) img7.timeToStore = img7.firstHit - img6.lastHit;

        // Restoring the values with the new computed values.
        ftt["5"] = img5;
        ftt["6"] = img6;
        ftt["7"] = img7;

        //qDebug() << trialIDs.at(i) << "5: " + img5.toString() << "6: " + img6.toString() << "7: " + img7.toString();

        responseTimeStruct->insert(trialIDs.at(i),ftt);



    }

}

//#define   CSV_TRIALID                                       3
//#define   CSV_IMGNUM                                    2
//#define   CSV_RESP_TIME                                 8

void EDPFielding::fillResponseTimes(QList<QStringList> *pdata, ResponseTimeStruct *respTimeStruct){
    for (qint32 i = 0; i < pdata->size(); i++){
        QString trialID = pdata->at(i).at(CSV_TRIALID);
        QString imgNum  = pdata->at(i).at(CSV_IMGNUM);
        MinMaxTimeStamp mmts = respTimeStruct->value(trialID).value(imgNum);
        //qDebug() << trialID << imgNum << mmts.toString();
        if (mmts.timeToStore > 0){
            QStringList row = pdata->at(i);
            row[CSV_RESP_TIME] = QString::number(mmts.timeToStore);
            pdata->replace(i,row);
        }
    }
}

void EDPFielding::appendDataToFieldingMatrix(const DataMatrix &data,
                                             const QString &trialID,
                                             const QString &imgID,
                                             const qreal &targetX,
                                             const qreal &targetY){

    qreal freqCheck = calculateSamplingFrequency(data,FIELDING_TI);
    qreal freq = config->getReal(CONFIG_SAMPLE_FREQUENCY);
    if (qAbs(freqCheck - freq) > 0.1*freq){
        samplingFrequencyCheck << "Fielding Frequency Check failed at: " + imgID
                                  + ". Expected Frequency is " + QString::number(freq)
                                  + ". Measured: " + QString::number(freqCheck);
    }

    // Calculating the fixations for each eye.;
    Fixations fL = mwa.computeFixations(data,FIELDING_XL,FIELDING_YL,FIELDING_TI);
    Fixations fR = mwa.computeFixations(data,FIELDING_XR,FIELDING_YR,FIELDING_TI);

    // Calculating value ranges to check if the subject "looked at" the target.
    qreal minX = targetX - static_cast<qreal>(RECT_WIDTH/fieldingKx)/2 - dW;
    qreal minY = targetY - static_cast<qreal>(RECT_HEIGHT/fieldingKy)/2 - dH;
    qreal maxX = targetX + static_cast<qreal>(RECT_WIDTH/fieldingKy)/2 + dW;
    qreal maxY = targetY + static_cast<qreal>(RECT_HEIGHT/fieldingKy)/2 + dH;


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

    // Indicates if fixation is within target.
    int isIn;

    SacadeAmplitudeCalculator sac;
    sac.reset();

    MinMaxTimeStamp mmts;
    mmts.reset();

    QHash<QString,MinMaxTimeStamp> responseTime;

    for (qint32 i = 0; i < fL.size(); i++){

        if (fL.at(i).isIn(minX,maxX,minY,maxY)) isIn = 1;
        else isIn = 0;

        QStringList left;
        left << subjectIdentifier
             << trialID
             << imgID
             << QString::number(isIn)
             << QString::number(fL.at(i).duration)
             << "0"
             << QString::number(sacLatL)
             << QString::number(sac.calculateSacadeAmplitude(fL.at(i).x,fL.at(i).y,monitorGeometry))
             << "N/A";
        ldata << left;

        // For efficiency, I do not check if imgID is 5, 6 or 7. Beucase the extra data is very small.
        // qDebug() << trialID << imgID << isIn;
        if (isIn == 1){
            if (mmts.firstHit < 0){
                mmts.firstHit = fL.at(i).fixStart;
                mmts.lastHit = fL.at(i).fixStart;
            }
            else mmts.lastHit = fL.at(i).fixStart;
        }
        else{
            // Might be a cross
            if ((imgID == "5") && (mmts.crossTime < 0)){
                if (fL.at(i).isIn(centerMinX,centerMaxX,centerMinY,centerMaxY)){
                    mmts.crossTime = fL.at(i).fixStart;
                }
            }
        }
    }

    //qDebug() << "SAVING LEFT"  << trialID << imgID << "->" << mmts.toString();

    // Saving the structure to calculate the response times.
    responseTime = lResponseTimeStruct.value(trialID);
    responseTime[imgID] = mmts;
    lResponseTimeStruct[trialID] = responseTime;

    sac.reset();
    mmts.reset();

    for (qint32 i = 0; i < fR.size(); i++){

        if (fR.at(i).isIn(minX,maxX,minY,maxY)) isIn = 1;
        else isIn = 0;

        QStringList right;
        right << subjectIdentifier
              << trialID
              << imgID
              << QString::number(isIn)
              << QString::number(fR.at(i).duration)
              << "1"
              << QString::number(sacLatR)
              << QString::number(sac.calculateSacadeAmplitude(fR.at(i).x,fR.at(i).y,monitorGeometry))
              << "N/A";
        rdata << right;

        // For efficiency, I do not check if imgID is 5, 6 or 7. Beucase the extra data is very small.
        if (isIn == 1){
            if (mmts.firstHit < 0){
                mmts.firstHit = fR.at(i).fixStart;
                mmts.lastHit = fR.at(i).fixStart;
            }
            else mmts.lastHit = fR.at(i).fixStart;
        }
        else{
            // Might be a cross
            if ((imgID == "5") && (mmts.crossTime < 0)){
                if (fR.at(i).isIn(centerMinX,centerMaxX,centerMinY,centerMaxY)){
                    mmts.crossTime = fR.at(i).fixStart;
                }
            }
        }

    }

    //qDebug() << "SAVING RIGHT"  << trialID << imgID << "->" << mmts.toString();

    // Saving the structure to calculate the response times.
    responseTime = rResponseTimeStruct.value(trialID);
    responseTime[imgID] = mmts;
    rResponseTimeStruct[trialID] = responseTime;

}
