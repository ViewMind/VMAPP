#include "edpgonogo.h"

EDPGoNoGo::EDPGoNoGo(ConfigurationManager *c):EDPBase(c)
{

}


bool EDPGoNoGo::doEyeDataProcessing(const QString &data){

    // This just creates the headeer.
    initializeGoNoGoDataMatrix();

    // Parsing the fielding experiment.
    if (!parser.parseGoNoGoExperiment(eyeFixations.experimentDescription,
                                      config->getReal(CONFIG_RESOLUTION_WIDTH),
                                      config->getReal(CONFIG_RESOLUTION_HEIGHT))){
        error = "Error parsing go no go experiment: " + parser.getError();
        return false;
    }

    // The center is the actual center of the screen.
    centerX = config->getReal(CONFIG_RESOLUTION_WIDTH)/2;
    centerY = config->getReal(CONFIG_RESOLUTION_HEIGHT)/2;

    QStringList lines = data.split("\n");
    hitTargetBoxes = parser.getTargetBoxes();

    // This will have all the data from a single image.
    DataMatrix imageData;

    QString id;
    qint32 trialType;

    QList<qint32> answerArray = parser.getCorrectAnswerArray();

    // Variables used to count the number of trials
    numberOfTrials = 0;
    QString lastID = "";

    // Computing the total experiment time.
    qreal totalExperimentTime = 0;
    qreal experimentStart = -1;
    for(int i = 0; i < lines.size(); i++){
        QString line = lines.at(i);
        QStringList tokens = line.split(' ',QString::SkipEmptyParts);
        if ( (tokens.size() != 2) ){
            // This is  data line.
            if (experimentStart == -1) experimentStart = tokens.at(GONOGO_TI).toDouble();
            else totalExperimentTime = tokens.at(GONOGO_TI).toDouble();
        }
    }
    //qDebug() << "GONOGO Start Time" << experimentStart << " End Time" << totalExperimentTime;
    totalExperimentTime = totalExperimentTime - experimentStart;

    for(int i = 0; i < lines.size(); i++){

        QString line = lines.at(i);

        //qWarning() << "Reading line" << line;

        QStringList tokens = line.split(' ',QString::SkipEmptyParts);

        // The data is contained between lines that have two tokens (id trial_description)
        if ( (tokens.size() == 2) ){

            // Saving stored data
            if (!imageData.isEmpty()){

                if ((trialType >= answerArray.size()) || (trialType < 0)){
                    error = "Error trying to get target box for go no go trial : " + id + " that has a trial type of: " + QString::number(trialType);
                    return false;
                }

                appendDataToGoNoGoMatrix(imageData,id,answerArray.at(trialType),trialType,totalExperimentTime);

                // Clearing the image data
                //qWarning() << "CLEARING";
                imageData.clear();
            }

            // Saving new id and trial and center of the circle.
            id = tokens.first().trimmed();
            bool ok = false;
            trialType = tokens.at(1).trimmed().toInt(&ok);

            if (!ok){
                error = "Error trying to get trial type for " + id + " Trial Type is " + tokens.at(1).trimmed();
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

        if ((trialType >= answerArray.size()) || (trialType < 0)){
            error = "Error trying to get target box for go no go trial : " + id + " that has a trial type of: " + QString::number(trialType);
            return false;
        }

        appendDataToGoNoGoMatrix(imageData,id,answerArray.at(trialType),trialType,totalExperimentTime);

        imageData.clear();
    }

    return finalizeCSVFile();

}


bool EDPGoNoGo::finalizeCSVFile(){
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


void EDPGoNoGo::initializeGoNoGoDataMatrix(){
    csvHeader << "suj"
              << "idtrial"
              << "target_hit"
              << "dur"
              << "ojoDI"
              << "latSac"
              << "amp_sacada"
              << "resp_time"
              << "pupil_size"
              << "num_trial_fix"
              << "gazing"
              << "num_center_fix"
              << "total_study_time"
              << "arrow_type"
              << "trial_type";
}

void EDPGoNoGo::appendDataToGoNoGoMatrix(const DataMatrix &data,
                                         const QString &trialID,
                                         const qint32 &targetBoxID,
                                         const qint32 &trialType,
                                         const qreal &totalExperimentTime){

    qreal freqCheck = calculateSamplingFrequency(data,GONOGO_TI);
    qreal freq = config->getReal(CONFIG_SAMPLE_FREQUENCY);
    if (qAbs(freqCheck - freq) > 0.1*freq){
        samplingFrequencyCheck << "Go No Go Frequency Check failed at: " + trialID
                                  + ". Expected Frequency is " + QString::number(freq)
                                  + ". Measured: " + QString::number(freqCheck);

    }

    QStringList id;
    id << trialID;
    eyeFixations.trialID.append(id);
    addDataToOneEye(data,EYE_L,trialID,targetBoxID,&ldata,trialType,totalExperimentTime);
    addDataToOneEye(data,EYE_R,trialID,targetBoxID,&rdata,trialType,totalExperimentTime);

}

void EDPGoNoGo::addDataToOneEye(const DataMatrix &data,
                                qint32 eyeID,
                                const QString &trialID,
                                const qint32 &targetBoxID,
                                QList<QStringList> *list,
                                const qint32 &trialType,
                                const qreal &totalExperimentTime){

    qint32 xEyeID, yEyeID, pupilID;
    QString eyeStringID;
    if (eyeID == EYE_L){
        eyeStringID = eyeLeftCSV;
        xEyeID = GONOGO_XL;
        yEyeID = GONOGO_YL;
        pupilID = GONOGO_PL;
    }
    else if (eyeID == EYE_R){
        eyeStringID = eyeRightCSV;
        xEyeID = GONOGO_XR;
        yEyeID = GONOGO_YR;
        pupilID = GONOGO_PR;
    }

    qreal startTime = data.first().at(GONOGO_TI);
    qreal sqrTol = pixelsInSacadicLatency*pixelsInSacadicLatency;
    qreal sacLat = 0;

    Fixations fix = mwa.computeFixations(data,xEyeID,yEyeID,GONOGO_TI);

    // Computing the
    //   average pupil size
    //   the gazing.
    //   the number of fixation in the "center" ( using the same criterie for sacadic latency to estimate the number of fixation in the center target )
    QList<qreal> pupilSizes;
    qreal gazing = 0;
    qint32 fixInCenter = 0;
    for (qint32 i = 0; i < fix.size(); i++){
        pupilSizes << averageColumnOfMatrix(data,pupilID,fix.at(i).indexFixationStart,fix.at(i).indexFixationEnd);
        gazing = gazing + fix.at(i).duration;
        qreal sqrD = qPow(fix.at(i).x - centerX,2)+qPow(fix.at(i).y - centerY,2);
        if (sqrD <= sqrTol){
            fixInCenter++;
        }
    }

    if (eyeID == EYE_L){
        eyeFixations.left.append(fix);
    }
    else if (eyeID == EYE_R){
        eyeFixations.right.append(fix);
    }

    // For the Left eye.
    for (qint32 i = 0; i < data.size(); i++){
        qreal sqrD = qPow(data.at(i).at(xEyeID) - centerX,2)+qPow(data.at(i).at(yEyeID) - centerY,2);
        if (sqrD > sqrTol){
            sacLat = data.at(i).at(GONOGO_TI) - startTime;
            break;
        }
    }

    SacadeAmplitudeCalculator sac;
    sac.reset();

    // Response time is computed like the time from the start of the trial to the first fixation that is a hit.
    QString respTime = "N/A";
    for (qint32 i = 0; i < fix.size(); i++){
        if (hitTargetBoxes.at(targetBoxID).contains(fix.at(i).x,fix.at(i).y)){
            qreal fixTime = (fix.at(i).fixStart + fix.at(i).fixEnd)/2;
            respTime = QString::number(fixTime - startTime);
            break;
        }
    }

    qint32 isIn;

    QString arrow_type = "R";
    if ((trialType == GONOGO_TRIAL_TYPE_GLEFT) || (trialType == GONOGO_TRIAL_TYPE_GRIGHT)){
        arrow_type = "G";
    }

    qint32 nonCorrectTargetHitBoxID;
    if (targetBoxID == 0) nonCorrectTargetHitBoxID = 1;
    else nonCorrectTargetHitBoxID = 0;

    for (qint32 i = 0; i < fix.size(); i++){

        // Target hit logic. First we check if fixation is in center
        qreal sqrD = qPow(fix.at(i).x - centerX,2)+qPow(fix.at(i).y - centerY,2);
        if (sqrD <= sqrTol){
            isIn = 0;
        }
        else if (hitTargetBoxes.at(targetBoxID).contains(fix.at(i).x,fix.at(i).y)) isIn = 1;
        else if (hitTargetBoxes.at(nonCorrectTargetHitBoxID).contains(fix.at(i).x,fix.at(i).y)) isIn = -1;
        else if (targetBoxID == 0){
            // The correct target Box is the Left One. So isIN will be zero if x is between the arrow and the left target box
            // which means it's x value should be LOWER than the half of the screen or centerX
            if (fix.at(i).x < centerX) isIn = 0;
            else isIn = -1;
        }
        else { // targetBox is 1.
            // The correct target Box is the Right One. So isIN will be zero if x is between the arrow and the right target box
            // which means it's x value should be LARGER than the half of the screen or centerX
            if (fix.at(i).x > centerX) isIn = 0;
            else isIn = -1;
        }

        QStringList eyed;
        eyed << subjectIdentifier // suj
             << trialID           // trialID
             << QString::number(isIn) // targetHit
             << QString::number(fix.at(i).duration) // dur
             << eyeStringID //ojoDI
             << QString::number(sacLat) // sacadic latency.
             << QString::number(sac.calculateSacadeAmplitude(fix.at(i).x,fix.at(i).y,monitorGeometry)) // sacade amplitude.
             << respTime // response time.
             << QString::number(pupilSizes.at(i))
             << QString::number(fix.size())
             << QString::number(gazing)
             << QString::number(fixInCenter)
             << QString::number(totalExperimentTime)
             << arrow_type
             << QString::number(trialType);
        list->append(eyed);
    }
}

