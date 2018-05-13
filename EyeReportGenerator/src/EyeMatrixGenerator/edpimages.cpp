#include "edpimages.h"

EDPImages::EDPImages(ConfigurationManager *c):EDPBase(c)
{
    manager = new BindingManager();
    manager->init(c);
}


bool EDPImages::doEyeDataProcessing(const QString &data){

    if (!initializeImageDataMatrix()){
        return false;
    }

    QStringList lines = data.split("\n");

    // This will have all the data from a single image.
    DataMatrix imageData;
    qint32 updateCounter = 0;

    QString id;
    QString isTrial;
    QString response;

    // Quick patch for unnecessary warnings.
    QHash<QString,qint32> warningCounter;

    qint64 filterValue = config->getInt(CONFIG_DAT_TIME_FILTER_THRESHOLD);
    filteredLinesList.clear();
    warnings = "";

    for (qint32 i = 0; i < lines.size(); i++){

        QString line = lines.at(i);

        QStringList tokens = line.split(' ',QString::SkipEmptyParts);

        // The data is contained between lines that either have two tokens (id isTrial) or a -> (response line)
        if ( (tokens.size() == 2) || line.contains("->") ){

            if (line.contains("->")){
                QStringList parts = line.split("->");
                response = parts.last();
                if (response.isEmpty()) response = "N/A";
            }
            else{
                response = "N/A";
            }

            // Saving stored data
            if (!imageData.isEmpty()){

                // Adding data to matrix
                //qWarning() << "Adding data to matrix of size" << imageData.size() << "for ID" << lastID;
                appendDataToImageMatrix(imageData,id,isTrial,response);

                // Updating progress
                updateCounter++;
                emit (updateProgress(updateCounter*100/numberToProcess));

                if (imageData.size() < BINDING_WARNING_NUM_DATAPOINTS){
                    warnings = warnings + "Trial ID: " + id + ". ISTrial: " + isTrial + " had less than " + QString::number(BINDING_WARNING_NUM_DATAPOINTS) + " datapoints <br>";
                }

                // Clearing the image data
                //qWarning() << "CLEARING";
                imageData.clear();
            }
            else{
                bool printing = false;
                if (((tokens.size() == 2) && (tokens.last().trimmed() == "1")) || (line.contains("->"))){
                    warnings = warnings + "Trial ID: " + id + ". ISTrial: " + isTrial + " has no data points<br>";
                    printing = true;
                }
            }

            // If this was the end of a trial, then the new id and isTrial is saved.
            if (tokens.size() == 2){
                // Saving new id and trial.
                id = tokens.first().trimmed();
                isTrial = tokens.last().trimmed();
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

        // Applying the filter
        if ((row.at(IMAGE_TI) < filterValue) || (filterValue <= 0)){
            imageData << row;
        }
        else{
            filteredLinesList << line;
        }

    }

    // Saving stored data of the last image.
    if (!imageData.isEmpty()){

        // Adding data to matrix
        //qWarning() << "Adding data to matrix of size" << imageData.size() << "for ID" << lastID;
        appendDataToImageMatrix(imageData,id,isTrial,response);

        // Updating progress
        updateCounter++;
        emit (updateProgress(updateCounter*100/numberToProcess));
    }

    return true;


}

bool EDPImages::initializeImageDataMatrix(){
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
           << "trial_id"
           << "is_trial"
           << "trial_name"
           << "trial_type"
           << "response"
           << "dur"
           << "ojoDI"
           << "blink"
           << "amp_sacada"
           << "pupila";

    writer << header.join(",") << "\n";
    file.close();

    return true;
}

bool EDPImages::appendDataToImageMatrix(const DataMatrix &data,
                                        const QString &trialName,
                                        const QString &isTrial,
                                        const QString &response){

    // Calculating the fixations for each eye.;
    Fixations fL = mwa.computeFixations(data,IMAGE_XL,IMAGE_YL,IMAGE_TI);
    Fixations fR = mwa.computeFixations(data,IMAGE_XR,IMAGE_YR,IMAGE_TI);

    BindingManager *m = (BindingManager *)manager;

    if (!skipImageGeneration){
        if (m->drawFlags(trialName,isTrial == "0")){
            // Constructing the image name
            QString imageName = trialName + "_" + isTrial;
            //... and drawing the fixations on the image.
            drawFixationOnImage(imageName,fL,fR);
        }
    }

    QFile file(outputFile);
    if (!file.open(QFile::Append)){
        error = "Could not open " + outputFile + " for appending";
        return false;
    }

    QTextStream writer(&file);

    // Deteriminig the trial id and trialtype
    QString trial_id, trial_type;
    QStringList tokens;

    tokens = trialName.split('-',QString::SkipEmptyParts);

    // The first part should be the trial id.
    trial_id = tokens.first();

    // Now checking if type is same or different. This will only work as long as the directory names,
    // contain the information.
    if (isTrial == "1"){
        if (trialName.contains(STR_DIFFERENT)){
            trial_type = STR_DIFFERENT;
        }
        else if (trialName.contains(STR_SAME)){
            trial_type = STR_SAME;
        }
        else{
            trial_type = "N/A";
        }
    }
    else{
        trial_type = "N/A";
    }

    // The x and y of the last fixation. Used to calculate the value of the sacade.
    qreal lastX = -1;
    qreal lastY = -1;

    for (qint32 i = 0; i < fL.size(); i++){

        qreal sacade = 0;
        if ((lastX > -1) && (lastY > -1)){
            // Calculating the sacade
            qreal xINmm = (lastX - fL.at(i).x)*monitorGeometry.XmmToPxRatio;
            qreal yINmm = (lastY - fL.at(i).y)*monitorGeometry.YmmToPxRatio;
            qreal delta = qSqrt(qPow(xINmm,2) + qPow(yINmm,2));
            sacade = qAtan(delta/monitorGeometry.distanceToMonitorInMilimiters)*180.0/3.141516;
        }

        lastX = fL.at(i).x;
        lastY = fL.at(i).y;

        writer << subjectIdentifier << ","
               << trial_id << ","
               << isTrial << ","
               << trialName << ","
               << trial_type << ","
               << response << ","
               << fL.at(i).duration << ","
               << "0,"
               << countZeros(data,IMAGE_PL,fL.at(i).indexFixationStart,fL.at(i).indexFixationEnd) << ","
               << sacade << ","
               << averageColumnOfMatrix(data,IMAGE_PL,fL.at(i).indexFixationStart,fL.at(i).indexFixationEnd);
        writer << "\n";
    }

    lastX = -1;
    lastY = -1;
    for (qint32 i = 0; i < fR.size(); i++){

        qreal sacade = 0;
        if ((lastX > -1) && (lastY > -1)){
            // Calculating the sacade
            qreal xINmm = (lastX - fR.at(i).x)*monitorGeometry.XmmToPxRatio;
            qreal yINmm = (lastY - fR.at(i).y)*monitorGeometry.YmmToPxRatio;
            qreal delta = qSqrt(qPow(xINmm,2) + qPow(yINmm,2));
            sacade = qAtan(delta/monitorGeometry.distanceToMonitorInMilimiters);
        }

        lastX = fR.at(i).x;
        lastY = fR.at(i).y;

        writer << subjectIdentifier << ","
               << trial_id << ","
               << isTrial << ","
               << trialName << ","
               << trial_type << ","
               << response << ","
               << fR.at(i).duration << ","
               << "1,"
               << countZeros(data,IMAGE_PR,fR.at(i).indexFixationStart,fR.at(i).indexFixationEnd) << ","
               << sacade << ","
               << averageColumnOfMatrix(data,IMAGE_PR,fR.at(i).indexFixationStart,fR.at(i).indexFixationEnd);
        writer << "\n";
    }

    file.close();
    return true;
}
