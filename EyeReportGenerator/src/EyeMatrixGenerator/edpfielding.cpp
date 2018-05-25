#include "edpfielding.h"

EDPFielding::EDPFielding(ConfigurationManager *c):EDPBase(c)
{
    manager = new FieldingManager();
    manager->init(c);
}

bool EDPFielding::doEyeDataProcessing(const QString &data){

    if (!initializeFieldingDataMatrix()){
        return false;
    }

    QStringList lines = data.split("\n");

    // The header format is HEADERNAME size X Y, so 2 and 3 have the center coordinates.
    centerX = config->getReal(CONFIG_RESOLUTION_WIDTH)/2;
    centerY = config->getReal(CONFIG_RESOLUTION_WIDTH)/2;

    // This will have all the data from a single image.
    DataMatrix imageData;
    qint32 updateCounter = 0;

    QString id;
    QString imageNumber;

    // X and Y margin for look detection
    dH = fieldingMarginInMM/monitorGeometry.YmmToPxRatio;
    dW = fieldingMarginInMM/monitorGeometry.XmmToPxRatio;
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

                // Updating progress
                updateCounter++;
                emit (updateProgress((updateCounter+1)*100/numberToProcess));

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

        // Updating progress
        updateCounter++;
        // The +10 is a simple bug so that the progress bar does not finish in 99%
        emit (updateProgress((updateCounter+10)*100/numberToProcess));

        // Clearing the image data
        //qWarning() << "CLEARING";
        imageData.clear();
    }

    return true;


}

bool EDPFielding::initializeFieldingDataMatrix(){
    QFile file(outputFile);
    if (file.exists()){
        file.remove();
    }

    if (!file.open(QFile::WriteOnly)){
        error = "Could not open " + outputFile + " for writing";
        return false;
    }

    QTextStream writer(&file);

    QStringList header;
    header << "suj"
           << "idtrial"
           << "imgnum"
           << "target_hit"
           << "dur"
           << "ojoDI"
           << "latSac";

    writer << header.join(",") << "\n";
    file.close();

    return true;
}

bool EDPFielding::appendDataToFieldingMatrix(const DataMatrix &data,
                                             const QString &trialID,
                                             const QString &imgID,
                                             const qreal &targetX,
                                             const qreal &targetY){

    // Calculating the fixations for each eye.;
    Fixations fL = mwa.computeFixations(data,FIELDING_XL,FIELDING_YL,FIELDING_TI);
    Fixations fR = mwa.computeFixations(data,FIELDING_XR,FIELDING_YR,FIELDING_TI);

    // Calculating value ranges to check if the subject "looked at" the target.
    qreal minX = targetX - (qreal)(RECT_WIDTH)/2 - dW;
    qreal minY = targetY - (qreal)(RECT_HEIGHT)/2 - dH;
    qreal maxX = targetX + (qreal)(RECT_WIDTH)/2 + dW;
    qreal maxY = targetY + (qreal)(RECT_HEIGHT)/2 + dH;

    //qWarning() << "X Limits: " << minX << maxX << ". Y Limits: " << minY << maxY;
    //qWarning() << trialID << imgID;

    FieldingManager *m = (FieldingManager *)manager;

    if (!skipImageGeneration){

        qint32 imgNumber = imgID.toInt();

        if (imgNumber < 4){
            m->setDrawState(FieldingManager::DS_CROSS_TARGET);
            if (!m->setTargetPositionFromTrialName(trialID,imgNumber-1)){
                error = "Could not find trial with name " + trialID + " for drawing the fixations";
                return false;
            }
        }
        else{
            if (imgNumber == 5){
                m->setDrawState(FieldingManager::DS_3);
            }
            else if (imgNumber == 6){
                m->setDrawState(FieldingManager::DS_2);
            }
            else{
                m->setDrawState(FieldingManager::DS_1);
            }
        }

        // Constructing the image name
        QString imageName = trialID + "_" + imgID;
        //... and drawing the fixations on the image.
        drawFixationOnImage(imageName,fL,fR);
    }


    QFile file(outputFile);
    if (!file.open(QFile::Append)){
        error = "Could not open " + outputFile + " for appending";
        return false;
    }

    QTextStream writer(&file);

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

    for (qint32 i = 0; i < fL.size(); i++){

        if (fL.at(i).isIn(minX,maxX,minY,maxY)) isIn = 1;
        else isIn = 0;

        writer << subjectIdentifier << ","
               << trialID << ","
               << imgID << ","
               << isIn << ","
               << fL.at(i).duration << ","
               << "0,"
               << sacLatL;
        writer << "\n";
    }

    for (qint32 i = 0; i < fR.size(); i++){

        if (fR.at(i).isIn(minX,maxX,minY,maxY)) isIn = 1;
        else isIn = 0;

        writer << subjectIdentifier << ","
               << trialID << ","
               << imgID << ","
               << isIn << ","
               << fR.at(i).duration << ","
               << "1,"
               << sacLatR;
        writer << "\n";
    }

    file.close();
    return true;


}
