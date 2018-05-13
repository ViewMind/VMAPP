#include "edpreading.h"

EDPReading::EDPReading(ConfigurationManager *c):EDPBase(c)
{
    manager = new ReadingManager();
    manager->init(c);
}

bool EDPReading::doEyeDataProcessing(const QString &data){

    if (!initializeReadingDataMatrix()){
        return false;
    }

    QStringList lines = data.split("\n");

    // This will have all the data from a single image.
    DataMatrix imageData;
    QString lastID = "";

    qint64 filterValue = config->getInt(CONFIG_DAT_TIME_FILTER_THRESHOLD);
    warnings = "";
    filteredLinesList.clear();
    bool dontCheckNext = false;

    qint32 updateCounter = 0;

    for (qint32 i = 0; i < lines.size(); i++){

        QString line = lines.at(i);

        QStringList tokens = line.split(' ',QString::SkipEmptyParts);

        QString id = tokens.first().trimmed();

        if (id != lastID){

            if ( (imageData.size() < READING_WARNING_NUM_DATAPOINTS) && !dontCheckNext && !lastID.isEmpty()){
                warnings = warnings + "Trial ID: " + lastID +  " had less than " + QString::number(READING_WARNING_NUM_DATAPOINTS) + " datapoints <br>";
            }
            dontCheckNext = false;

            if (!imageData.isEmpty()){

                // Adding data to matrix
                //qWarning() << "Adding data to matrix of size" << imageData.size() << "for ID" << lastID;
                appendDataToReadingMatrix(imageData,lastID);
                //qWarning() << "DONE";

                // Updating progress
                updateCounter++;
                emit (updateProgress((updateCounter+1)*100/numberToProcess));

                // Clearing the image data
                //qWarning() << "CLEARING";
                imageData.clear();
            }

            lastID = id;
            if (line.contains("->")){
                // Updating progress
                updateCounter++;
                emit (updateProgress((updateCounter+1)*100/numberToProcess));
                // The next time the check for the number of lines will give zero due to this being a question and it has no data.
                dontCheckNext = true;
                continue;
            }
        }

        // If the code got here, this is just another row to process.
        DataRow row;
        for (qint32 i = 1; i < tokens.size(); i++){
            row << tokens.at(i).toDouble();
        }
        if (!imageData.isEmpty()){
            if (imageData.last().size() != row.size()){
                error = "Something is wrong with the data format. Different size rows in the data matrix from line: " + line;
                return false;
            }
        }

        // Applying the filter
        if ((row.at(READ_TI) < filterValue) || (filterValue <= 0)){
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
        appendDataToReadingMatrix(imageData,lastID);
        //qWarning() << "DONE";

        // Updating progress
        updateCounter++;
        emit (updateProgress((updateCounter+1)*100/numberToProcess));
    }

    return true;

}

bool EDPReading::initializeReadingDataMatrix(){

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
           << "sujnum"
           << "oracion"
           << "trial"
           << "fixn"
           << "screenpos"
           << "wn"
           << "pos"
           << "dur"
           << "longoracion"
           << "ojoDI"
           << "pupila";

    writer << header.join(",") << "\n";
    file.close();

    return true;

}

bool EDPReading::appendDataToReadingMatrix(const DataMatrix &data, const QString &imgID){

    // Calculating the fixations for each eye.;
    Fixations fL = mwa.computeFixations(data,READ_XL,READ_YL,READ_TI);

    //qWarning() << "ABORTING";
    //abort();

    Fixations fR = mwa.computeFixations(data,READ_XR,READ_YR,READ_TI);

    // The image with the fixations is generated.
    ReadingManager *m = (ReadingManager *)manager;
    if (!skipImageGeneration){
        if (m->drawPhrase(imgID)){
            drawFixationOnImage(imgID,fL,fR);
        }
    }

    QFile file(outputFile);
    if (!file.open(QFile::Append)){
        error = "Could not open " + outputFile + " for appending";
        return false;
    }

    QTextStream writer(&file);

    // The exact same processing is done for left and right.
    for (qint32 i = 0; i < fL.size(); i++){

        // Average position of th word.
        qreal wn = averageColumnOfMatrix(data,
                                         READ_WL,
                                         fL.at(i).indexFixationStart,
                                         fL.at(i).indexFixationEnd);

        // Average position of the character
        qreal pos = averageColumnOfMatrix(data,
                                          READ_CL,
                                          fL.at(i).indexFixationStart,
                                          fL.at(i).indexFixationEnd);

        // Average pupil size
        qreal pup = averageColumnOfMatrix(data,
                                          READ_PL,
                                          fL.at(i).indexFixationStart,
                                          fL.at(i).indexFixationEnd);

        writer << subjectIdentifier << "," << "1," << imgID << "," << imgID << "," << i+1 << ","
            << fL.at(i).x << "," << wn << "," << pos << "," << fL.at(i).duration << ","
               // The length of the sentece is alwasy the same so the value of the first row is taken, only.
            << data.first().at(READ_SL) << ",0"
            << "," << pup;
        writer << "\n";
    }


    for (qint32 i = 0; i < fR.size(); i++){

        // Average position of th word.
        qreal wn = averageColumnOfMatrix(data,
                                         READ_WR,
                                         fR.at(i).indexFixationStart,
                                         fR.at(i).indexFixationEnd);

        // Average position of the character
        qreal pos = averageColumnOfMatrix(data,
                                          READ_CR,
                                          fR.at(i).indexFixationStart,
                                          fR.at(i).indexFixationEnd);

        // Average pupil size
        qreal pup = averageColumnOfMatrix(data,
                                          READ_PR,
                                          fR.at(i).indexFixationStart,
                                          fR.at(i).indexFixationEnd);


        writer << subjectIdentifier << "," << "1," << imgID << "," << imgID << "," << i+1 << ","
            << fR.at(i).x << "," << wn << "," << pos << "," << fR.at(i).duration << ","
               // The length of the sentece is alwasy the same so the value of the first row is taken, only.
            << data.first().at(READ_SL) << ",1"
            << "," << pup;
        writer << "\n";
    }

    file.close();
    return true;
}

