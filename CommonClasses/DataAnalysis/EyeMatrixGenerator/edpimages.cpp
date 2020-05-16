#include "edpimages.h"

EDPImages::EDPImages(ConfigurationManager *c):EDPBase(c)
{
}

void EDPImages::sumToAnswers(const QString &trialID, const QString &ans){
    if (trialID.contains(STR_TEST)){
        if (trialID.contains(STR_SAME) && ans == "S") answers.testCorrect++;
        else if (trialID.contains(STR_DIFFERENT) && ans == "D") answers.testCorrect++;
        else answers.testWrong++;
    }
    else{
        if (trialID.contains(STR_SAME) && ans == "S") answers.correct++;
        else if (trialID.contains(STR_DIFFERENT) && ans == "D") answers.correct++;
        else answers.wrong++;
    }
}

bool EDPImages::doEyeDataProcessing(const QString &data){

    initializeImageDataMatrix();

    // Parsing the experiment.
    config->addKeyValuePair(CONFIG_DEMO_MODE,false);
    if (!parser.parseBindingExperiment(eyeFixations.experimentDescription,
                                       config,
                                       config->getReal(CONFIG_RESOLUTION_WIDTH),config->getReal(CONFIG_RESOLUTION_HEIGHT),
                                       0)){
        error = "Error parsing binding experiment: " + parser.getError();
        return false;
    }

    eyeFixations.trialID = parser.getExpectedIDs();
    eyeFixations.fillFixationsLists();

    QStringList lines = data.split("\n");

    // This will have all the data from a single image.
    DataMatrix imageData;

    QString id;
    QString isTrial;
    QString response;

    // Quick patch for unnecessary warnings.
    // QHash<QString,qint32> warningCounter;

    qint64 filterValue = config->getInt(CONFIG_DAT_TIME_FILTER_THRESHOLD);
    filteredLinesList.clear();
    warnings = "";

    answers.correct = 0;
    answers.testCorrect = 0;
    answers.testWrong = 0;
    answers.wrong = 0;

    for (qint32 i = 0; i < lines.size(); i++){

        QString line = lines.at(i);

        QStringList tokens = line.split(' ',QString::SkipEmptyParts);

        // The data is contained between lines that either have two tokens (id isTrial) or a -> (response line)
        if ( (tokens.size() == 2) || line.contains("->") ){

            if (line.contains("->")){
                QStringList parts = line.split("->");
                response = parts.last();
                if (response.isEmpty()) response = "N/A";
                sumToAnswers(parts.first(),response);
            }
            else{
                response = "N/A";
            }

            // Saving stored data
            if (!imageData.isEmpty()){

                // Adding data to matrix
                //qWarning() << "Adding data to matrix of size" << imageData.size() << "for ID" << lastID;
                appendDataToImageMatrix(imageData,id,isTrial,response);

                if (imageData.size() < BINDING_WARNING_NUM_DATAPOINTS){
                    warnings = warnings + "Trial ID: " + id + ". ISTrial: " + isTrial + " had less than " + QString::number(BINDING_WARNING_NUM_DATAPOINTS) + " datapoints <br>";
                }

                // Clearing the image data
                //qWarning() << "CLEARING";
                imageData.clear();
            }
            else{
                if (((tokens.size() == 2) && (tokens.last().trimmed() == "1")) || (line.contains("->"))){
                    warnings = warnings + "Trial ID: " + id + ". ISTrial: " + isTrial + " has no data points<br>";
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

    }

    // Adding the score column
    if (csvLines.size() > 2) {
        for (qint32 i = 1; i < csvLines.size(); i++){
            csvLines[i] = csvLines.at(i) + "," + QString::number(answers.correct);
        }
    }

    if (!saveDataToFile()){
        return false;
    }

    return true;
}

void EDPImages::initializeImageDataMatrix(){

    QStringList header;    
    header << "placeholder_id"
           << "age"
           << "suj"
           << "trial_id"
           << "is_trial"
           << "trial_name"
           << "trial_type"
           << "response"
           << "dur"
           << "ojoDI"
           << "blink"
           << "amp_sacada"
           << "pupila"
           << "gaze"
           << "nf"
           << "fixX"
           << "fixY"
           << "targetSide"
           << "tX1"
           << "tY1"
           << "tX2"
           << "tY2"
           << "tX3"
           << "tY3"           
           << "fixStart"
           << "fixEnd"
           << "fixMid"
           << "timeline"
           << "score";

    csvLines << header.join(",");
}

bool EDPImages::saveDataToFile(){

    QFile file(outputFile);
    if (file.exists()){
        file.remove();
    }

    if (!file.open(QFile::WriteOnly)){
        error = "Could not open " + outputFile + " for writing";
        return false;
    }

    QTextStream writer(&file);
    writer << csvLines.join("\n");
    file.close();
    return true;
}

bool EDPImages::appendDataToImageMatrix(const DataMatrix &data,
                                        const QString &trialName,
                                        const QString &isTrial,
                                        const QString &response){

    qreal freqCheck = calculateSamplingFrequency(data,IMAGE_TI);
    qreal freq = config->getReal(CONFIG_SAMPLE_FREQUENCY);
    if (qAbs(freqCheck - freq) > 0.1*freq){
        samplingFrequencyCheck << "Binding Frequency Check failed at: " + trialName + "_" + isTrial
                                  + ". Expected Frequency is " + QString::number(freq)
                                  + ". Measured: " + QString::number(freqCheck);
    }

    qreal slideStart = data.first().at(IMAGE_TI);

    // Calculating the fixations for each eye.;
    Fixations fL = mwa.computeFixations(data,IMAGE_XL,IMAGE_YL,IMAGE_TI);
    Fixations fR = mwa.computeFixations(data,IMAGE_XR,IMAGE_YR,IMAGE_TI);

    // Finding the right trial
    BindingParser::BindingTrial bindingTrial = parser.getTrialByName(trialName);

    // Number of fixations for each Eye.
    QString nfL = QString::number(fL.size());
    QString nfR = QString::number(fR.size());

    // Fixation sum, called Gaze.
    QString gazeL = QString::number(getGaze(fL));
    QString gazeR = QString::number(getGaze(fR));

    // Adding to the result struct
    QStringList id;
    id << trialName << isTrial;
    qint32 index = eyeFixations.indexOfTrial(id);
    if (index == -1){
        error = "Cannot find Trial with id: " + id.join(" ");
        return false;
    }
    eyeFixations.left[index] = fL;
    eyeFixations.right[index] = fR;

    // Deteriminig the trial id and trialtype
    QString trial_id, trial_type;
    QStringList tokens;

    tokens = trialName.split('-',QString::SkipEmptyParts);

    // The first part should be the trial id.
    trial_id = tokens.first();

    // Now checking if type is same or different. This will only work as long as the trial names,
    // contain the information.
    QString targetXYData;

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
        targetXYData = bindingTrial.getCSVXYData(true);
    }
    else{
        trial_type = "N/A";
        targetXYData = bindingTrial.getCSVXYData(false);
    }

    // Used to calculate the value of the sacade.
    SacadeAmplitudeCalculator sac;
    sac.reset();

    for (qint32 i = 0; i < fL.size(); i++){
        QStringList writer;
        writer << "N/A" << ","
               << config->getString(CONFIG_PATIENT_AGE) << ","
               << subjectIdentifier << ","
               << trial_id << ","
               << isTrial << ","
               << trialName << ","
               << trial_type << ","
               << response << ","
               << QString::number(fL.at(i).duration) << ","
               << "0,"
               << QString::number(countZeros(data,IMAGE_PL,fL.at(i).indexFixationStart,fL.at(i).indexFixationEnd)) << ","
               << QString::number(sac.calculateSacadeAmplitude(fL.at(i).x,fL.at(i).y,monitorGeometry)) << ","
               << QString::number(averageColumnOfMatrix(data,IMAGE_PL,fL.at(i).indexFixationStart,fL.at(i).indexFixationEnd)) << ","
               << gazeL << ","
               << nfL << ","
               << QString::number(fL.at(i).x) << ","
               << QString::number(fL.at(i).y) << ","
               << QString::number(parser.getDrawStructure().FlagSideH) << ","
               << targetXYData << ","
               << QString::number(fL.at(i).fixStart,'f',2) << ","
               << QString::number(fL.at(i).fixEnd,'f',2) << ","
               << QString::number((fL.at(i).fixStart + fL.at(i).fixEnd)/2,'f',2) << ","
               << QString::number(fL.at(i).fixStart - slideStart,'f',2);
        csvLines << writer.join("");
    }

    sac.reset();
    for (qint32 i = 0; i < fR.size(); i++){
        QStringList writer;
        writer << "N/A" << ","
               << config->getString(CONFIG_PATIENT_AGE) << ","
               << subjectIdentifier << ","
               << trial_id << ","
               << isTrial << ","
               << trialName << ","
               << trial_type << ","
               << response << ","
               << QString::number(fR.at(i).duration) << ","
               << "1,"
               << QString::number(countZeros(data,IMAGE_PR,fR.at(i).indexFixationStart,fR.at(i).indexFixationEnd)) << ","
               << QString::number(sac.calculateSacadeAmplitude(fR.at(i).x,fR.at(i).y,monitorGeometry)) << ","
               << QString::number(averageColumnOfMatrix(data,IMAGE_PR,fR.at(i).indexFixationStart,fR.at(i).indexFixationEnd)) << ","
               << gazeR << ","
               << nfR << ","
               << QString::number(fR.at(i).x) << ","
               << QString::number(fR.at(i).y) << ","
               << QString::number(parser.getDrawStructure().FlagSideH) << ","
               << targetXYData << ","
               << QString::number(fR.at(i).fixStart,'f',2) << ","
               << QString::number(fR.at(i).fixEnd,'f',2) << ","
               << QString::number((fR.at(i).fixStart + fR.at(i).fixEnd)/2,'f',2) << ","
               << QString::number(fR.at(i).fixStart - slideStart,'f',2);
        csvLines << writer.join("");
    }

    return true;
}
