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

                appendDataToGoNoGoMatrix(imageData,id,answerArray.at(trialType));

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

        appendDataToGoNoGoMatrix(imageData,id,answerArray.at(trialType));

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
    csvHeader << "suj"          //0
              << "idtrial"      //1
              << "target_hit"   //3
              << "dur"          //4
              << "ojoDI"        //5
              << "latSac"       //6
              << "amp_sacada"   //7
              << "resp_time";   //8
}

void EDPGoNoGo::appendDataToGoNoGoMatrix(const DataMatrix &data,
                                         const QString &trialID,
                                         const qint32 &targetBoxID){

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
    addDataToOneEye(data,EYE_L,trialID,targetBoxID,&ldata);
    addDataToOneEye(data,EYE_R,trialID,targetBoxID,&rdata);

}

void EDPGoNoGo::addDataToOneEye(const DataMatrix &data,
                                qint32 eyeID,
                                const QString &trialID,
                                const qint32 &targetBoxID,
                                QList<QStringList> *list){

    qint32 xEyeID, yEyeID;
    QString eyeStringID;
    if (eyeID == EYE_L){
        eyeStringID = eyeLeftCSV;
        xEyeID = GONOGO_XL;
        yEyeID = GONOGO_YL;
    }
    else if (eyeID == EYE_R){
        eyeStringID = eyeRightCSV;
        xEyeID = GONOGO_XR;
        yEyeID = GONOGO_YR;
    }

    qreal startTime = data.first().at(GONOGO_TI);
    qreal sqrTol = pixelsInSacadicLatency*pixelsInSacadicLatency;
    qreal sacLat = 0;

    Fixations fix = mwa.computeFixations(data,xEyeID,yEyeID,GONOGO_TI);

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
        }
    }

    qint32 isIn;

    for (qint32 i = 0; i < fix.size(); i++){

        if (hitTargetBoxes.at(targetBoxID).contains(fix.at(i).x,fix.at(i).y)) isIn = 1;
        else isIn = 0;

        QStringList eyed;
        eyed << subjectIdentifier // suj
             << trialID           // trialID
             << QString::number(isIn) // targetHit
             << QString::number(fix.at(i).duration) // dur
             << eyeStringID //ojoDI
             << QString::number(sacLat) // sacadic latency.
             << QString::number(sac.calculateSacadeAmplitude(fix.at(i).x,fix.at(i).y,monitorGeometry)) // sacade amplitude.
             << respTime; // response time.
        list->append(eyed);

    }
}

