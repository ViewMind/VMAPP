#include "edpfielding.h"

EDPFielding::EDPFielding(ConfigurationManager *c):EDPBase(c)
{
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

    QString id;    
    QString imageNumber;

    // Variables used to count the number of trials
    numberOfTrials = 0;
    QString lastID = "";

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
    qreal minX = targetX - (qreal)(RECT_WIDTH)/2 - dW;
    qreal minY = targetY - (qreal)(RECT_HEIGHT)/2 - dH;
    qreal maxX = targetX + (qreal)(RECT_WIDTH)/2 + dW;
    qreal maxY = targetY + (qreal)(RECT_HEIGHT)/2 + dH;

    eyeFixations.left.append(fL);
    eyeFixations.right.append(fR);
    QStringList id;
    id << imgID << trialID;
    eyeFixations.trialID.append(id);

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
