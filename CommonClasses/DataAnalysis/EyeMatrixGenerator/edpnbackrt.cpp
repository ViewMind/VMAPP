#include "edpnbackrt.h"

EDPNBackRT::EDPNBackRT(ConfigurationManager *c):EDPBase(c)
{
}

bool EDPNBackRT::doEyeDataProcessing(const QString &data){

    // This just creates the headeer.
    initializeFieldingDataMatrix();

    // Parsing the fielding experiment.
    if (!parser.parseFieldingExperiment(eyeFixations.experimentDescription)){
        error = "Error parsing n back rt experiment: " + parser.getError();
        return false;
    }

    QStringList lines = data.split("\n");

    // X and Y margin for look detection
    qreal k = fieldingMargin/200.0; // This is divided by a 100 to get it to number between 0 and 1 and divided by two to get half of that.
    fieldingKx = config->getReal(CONFIG_FIELDING_XPX_2_MM);
    fieldingKy = config->getReal(CONFIG_FIELDING_YPX_2_MM);
    dH = static_cast<qreal>(RECT_HEIGHT/fieldingKx)*k;
    dW = static_cast<qreal>(RECT_WIDTH/fieldingKy)*k;

    qreal WScreen = AREA_WIDTH/fieldingKx;
    qreal HScreen = AREA_HEIGHT/fieldingKy;
    qreal generalOffsetX = (config->getReal(CONFIG_RESOLUTION_WIDTH) - WScreen)/2;
    qreal generalOffsetY = (config->getReal(CONFIG_RESOLUTION_HEIGHT) - HScreen)/2;


    // The center is the actual center of the screen.
    centerX = config->getReal(CONFIG_RESOLUTION_WIDTH)/2;
    centerY = config->getReal(CONFIG_RESOLUTION_HEIGHT)/2;

    centerMinX = centerX - static_cast<qreal>(RECT_WIDTH/fieldingKx)/2 - dW;
    centerMinY = centerY - static_cast<qreal>(RECT_HEIGHT/fieldingKy)/2 - dH;
    centerMaxX = centerX + static_cast<qreal>(RECT_WIDTH/fieldingKy)/2 + dW;
    centerMaxY = centerY + static_cast<qreal>(RECT_HEIGHT/fieldingKy)/2 + dH;


    // The tolerance distances are computed as 1/8 of the distance from the center to the closest point to a target square.

    // Computign the target boxes
    qreal targetBoxWidth = RECT_WIDTH/fieldingKx;
    qreal targetBoxHeight = RECT_HEIGHT/fieldingKy;

    targetBoxes.clear();
    targetBoxes << QRectF(generalOffsetX+RECT_0_X/fieldingKx,generalOffsetY+RECT_0_Y/fieldingKy,targetBoxWidth,targetBoxHeight);
    targetBoxes << QRectF(generalOffsetX+RECT_1_X/fieldingKx,generalOffsetY+RECT_1_Y/fieldingKy,targetBoxWidth,targetBoxHeight);
    targetBoxes << QRectF(generalOffsetX+RECT_2_X/fieldingKx,generalOffsetY+RECT_2_Y/fieldingKy,targetBoxWidth,targetBoxHeight);
    targetBoxes << QRectF(generalOffsetX+RECT_3_X/fieldingKx,generalOffsetY+RECT_3_Y/fieldingKy,targetBoxWidth,targetBoxHeight);
    targetBoxes << QRectF(generalOffsetX+RECT_4_X/fieldingKx,generalOffsetY+RECT_4_Y/fieldingKy,targetBoxWidth,targetBoxHeight);
    targetBoxes << QRectF(generalOffsetX+RECT_5_X/fieldingKx,generalOffsetY+RECT_5_Y/fieldingKy,targetBoxWidth,targetBoxHeight);


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

            // Saving stored data
            if (!imageData.isEmpty()){

                // Adding data to matrix
                //qWarning() << "Adding data to matrix of size" << imageData.size() << "for ID" << lastID;

                appendDataToFieldingMatrix(imageData,id,imageNumber,trialSequence);

                // Clearing the image data
                //qWarning() << "CLEARING";
                imageData.clear();
            }

            // Saving new id and trial and center of the circle.
            id = tokens.first().trimmed();
            imageNumber = tokens.at(1).trimmed();
            trialSequence = parser.getSequenceForTrial(id);
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
        appendDataToFieldingMatrix(imageData,id,imageNumber,trialSequence);

        // Clearing the image data
        //qWarning() << "CLEARING";
        imageData.clear();
    }

    return finalizeFieldingDataMatrix();

}

void EDPNBackRT::initializeFieldingDataMatrix(){

    csvHeader << "suj"       //0
              << "idtrial"      //1
              << "imgnum"       //2
              << "target"       //3
              << "target_hit"   //4
              << "dur"          //5
              << "ojoDI"        //6
              << "latSac"       //7
              << "amp_sacada"   //8
              << "resp_time";   //9

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

    return true;
}


void EDPNBackRT::appendDataToFieldingMatrix(const DataMatrix &data,
                                             const QString &trialID,
                                             const QString &imgID,
                                             QList<qint32> trialSequence){

    qreal freqCheck = calculateSamplingFrequency(data,FIELDING_TI);
    qreal freq = config->getReal(CONFIG_SAMPLE_FREQUENCY);
    if (qAbs(freqCheck - freq) > 0.1*freq){
        samplingFrequencyCheck << "NBack RT Frequency Check failed at: " + imgID
                                  + ". Expected Frequency is " + QString::number(freq)
                                  + ". Measured: " + QString::number(freqCheck);
    }

    // Calculating the fixations for each eye.;
    Fixations fL = mwa.computeFixations(data,FIELDING_XL,FIELDING_YL,FIELDING_TI);
    Fixations fR = mwa.computeFixations(data,FIELDING_XR,FIELDING_YR,FIELDING_TI);

    eyeFixations.left.append(fL);
    eyeFixations.right.append(fR);
    QStringList id;
    id << imgID << trialID;
    eyeFixations.trialID.append(id);


//    // Computing maximum and minimum values to check whether a fixation is IN or OUT.
//    qreal minX = targetBoxX.at(trialSequence);
//    qreal minY = targetBoxY.at(trialSequence);
//    qreal maxX = targetBoxX.at(trialSequence) + targetBoxWidth;
//    qreal maxY = targetBoxY.at(trialSequence) + targetBoxHeight;

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
    sac.reset();

    for (qint32 i = 0; i < fL.size(); i++){

        // Computing the expected target and the target hit parameter for the image.
        Fixation f = fL.at(i);
        qint32 imgNum = imgID.toInt();
        QString expected_target;
        QString isIn;

        if ((imgNum >= 1) && (imgNum <= 3)){
            // These are the images that show the red dots.
            qint32 expTarget = trialSequence.at(imgNum-1);
            expected_target = QString::number(expTarget);
            if (targetBoxes.at(expTarget).contains(f.x,f.y)) isIn = "1";
            else isIn = "0";
        }
        else if (imgNum == 4){
            // We need to detect if the fixation sequence, here is correct.
            // The expected target is the allwasy the last in the sequence.
            if (trialSequence.isEmpty()){
                expected_target = "N/A";
                isIn = "0";
            }
            else{
                qint32 expTarget = trialSequence.last();
                expected_target = QString::number(expTarget);
                // Checking if it hits ANY targets.
                isIn = "0";
                for (qint32 i = 0; i < targetBoxes.size(); i++){
                    if (targetBoxes.at(i).contains(f.x,f.y)){
                        // Checking if this is the same as the expected target
                        if (expTarget == i){
                            // We remove only the LAST expected target in the trial sequence.
                            trialSequence.removeLast();
                            isIn = "1";
                        }
                        else{
                            // This means there WAS a HIT but it was NOT the expected target. Sequence is broken, there will be no more hits.
                            trialSequence.clear();
                        }
                        break;
                    }
                }
            }
        }

        QStringList left;
        left << subjectIdentifier
             << trialID
             << imgID
             << expected_target
             << isIn
             << QString::number(f.duration)
             << "0"
             << QString::number(sacLatL)
             << QString::number(sac.calculateSacadeAmplitude(f.x,f.y,monitorGeometry))
             << "N/A";
        ldata << left;

    }

    //qDebug() << "SAVING LEFT"  << trialID << imgID << "->" << mmts.toString();

    sac.reset();

    for (qint32 i = 0; i < fR.size(); i++){
        // Computing the expected target and the target hit parameter for the image.
        Fixation f = fR.at(i);
        qint32 imgNum = imgID.toInt();
        QString expected_target;
        QString isIn;

        if ((imgNum >= 1) && (imgNum <= 3)){
            // These are the images that show the red dots.
            qint32 expTarget = trialSequence.at(imgNum-1);
            expected_target = QString::number(expTarget);
            if (targetBoxes.at(expTarget).contains(f.x,f.y)) isIn = "1";
            else isIn = "0";
        }
        else if (imgNum == 4){
            // We need to detect if the fixation sequence, here is correct.
            // The expected target is the allwasy the last in the sequence.
            if (trialSequence.isEmpty()){
                expected_target = "N/A";
                isIn = "0";
            }
            else{
                qint32 expTarget = trialSequence.last();
                expected_target = QString::number(expTarget);
                // Checking if it hits ANY targets.
                isIn = "0";
                for (qint32 i = 0; i < targetBoxes.size(); i++){
                    if (targetBoxes.at(i).contains(f.x,f.y)){
                        // Checking if this is the same as the expected target
                        if (expTarget == i){
                            // We remove only the LAST expected target in the trial sequence.
                            trialSequence.removeLast();
                            isIn = "1";
                        }
                        else{
                            // This means there WAS a HIT but it was NOT the expected target. Sequence is broken, there will be no more hits.
                            trialSequence.clear();
                        }
                        break;
                    }
                }
            }
        }

        QStringList right;
        right << subjectIdentifier
              << trialID
              << imgID
              << expected_target
              << isIn
              << QString::number(f.duration)
              << "1"
              << QString::number(sacLatR)
              << QString::number(sac.calculateSacadeAmplitude(f.x,f.y,monitorGeometry))
              << "N/A";
        rdata << right;


    }

    //qDebug() << "SAVING RIGHT"  << trialID << imgID << "->" << mmts.toString();

}
