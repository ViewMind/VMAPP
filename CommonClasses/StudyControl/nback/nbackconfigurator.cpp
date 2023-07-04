#include "nbackconfigurator.h"

NBackConfigurator::NBackConfigurator(): StudyConfigurator() {
    this->studyDescriptionFile = ":/CommonClasses/StudyControl/nback/descriptions/nback.dat";    
}

bool NBackConfigurator::studySpecificConfiguration(const QVariantMap &studyConfig){

    qint32 numberOfTrials = -1; // This will use all of them.
    if (this->shortStudies){
        numberOfTrials = N_TRIALS_IN_SHORT_STUDIES;
    }
    else {
        if (this->studyType == VMDC::Study::NBACK){
            numberOfTrials = REDUCED_TRIAL_SET;
        }
    }

    // Now we parse the trial. If it worked the configuration is loaded with the NBack trials.
    if (!parseStudyDescription(numberOfTrials)) return false;

    // Now that the description was parsed, we can set all the other configuration constants.
    configuration[RRS::StudyConfigurationFields::PAUSE_MESSAGE] = studyConfig[VMDC::StudyParameter::WAIT_MESSAGE];
    configuration[RRS::StudyConfigurationFields::IS_STUDY_3D] = false;

    if (this->studyType == VMDC::Study::NBACKVS){

        if (!studyConfig.value(VMDC::StudyParameter::NBACK_LIGHT_ALL).toBool()){
            configuration[RRS::StudyConfigurationFields::NBACK_LIGHTUP] = RRS::NBack::NBackLightUp::CORRECT;
        }
        else {
            configuration[RRS::StudyConfigurationFields::NBACK_LIGHTUP] = RRS::NBack::NBackLightUp::ALL;
        }

        configuration[RRS::StudyConfigurationFields::STUDY]                = VMDC::Study::NBACKVS;
        configuration[RRS::StudyConfigurationFields::MIN_HOLD_TIME]        = NBACKVS_MIN_HOLD_TIME;
        configuration[RRS::StudyConfigurationFields::MAX_HOLD_TIME]        = NBACKVS_MAX_HOLD_TIME;
        configuration[RRS::StudyConfigurationFields::STEP_HOLD_TIME]       = NBACKVS_STEP_HOLD_TIME;
        configuration[RRS::StudyConfigurationFields::START_HOLD_TIME]      = NBACKVS_START_HOLD_TIME;
        configuration[RRS::StudyConfigurationFields::NUMBER_OF_TARGETS]    = studyConfig.value(VMDC::StudyParameter::NUMBER_TARGETS).toInt();
        configuration[RRS::StudyConfigurationFields::N_TRIALS_STEP_CHANGE] = NBACKVS_NTRIAL_FOR_STEP_CHANGE;
        configuration[RRS::StudyConfigurationFields::TIMEOUT_TRAN]         = NBACKVS_TRANSITION_TIME;
        configuration[RRS::StudyConfigurationFields::TIMEOUT]              = NBACKVS_RETRIEVAL_TIMEOUT;        

    }
    else {
        qint32 hold_time = studyConfig.value(VMDC::StudyParameter::NBACK_HOLD_TIME).toInt();
        configuration[RRS::StudyConfigurationFields::STUDY]                = VMDC::Study::NBACK;
        configuration[RRS::StudyConfigurationFields::MIN_HOLD_TIME]        = hold_time;
        configuration[RRS::StudyConfigurationFields::MAX_HOLD_TIME]        = hold_time;
        configuration[RRS::StudyConfigurationFields::STEP_HOLD_TIME]       = 0;
        configuration[RRS::StudyConfigurationFields::START_HOLD_TIME]      = hold_time;
        configuration[RRS::StudyConfigurationFields::NUMBER_OF_TARGETS]    = studyConfig.value(VMDC::StudyParameter::NUMBER_TARGETS).toInt();
        configuration[RRS::StudyConfigurationFields::N_TRIALS_STEP_CHANGE] = 0;
        configuration[RRS::StudyConfigurationFields::TIMEOUT_TRAN]         = studyConfig.value(VMDC::StudyParameter::NBACK_TRANSITION).toInt();
        configuration[RRS::StudyConfigurationFields::TIMEOUT]              = studyConfig.value(VMDC::StudyParameter::NBACK_TIMEOUT).toInt();
        configuration[RRS::StudyConfigurationFields::NBACK_LIGHTUP]        = RRS::NBack::NBackLightUp::NONE;
    }

    return true;
}


bool NBackConfigurator::parseStudyDescription(int numberOfTrials){
    QStringList lines = studyDescriptionContent.split('\n',Qt::KeepEmptyParts);

    // Needed to check for unique ids.
    QSet<QString> uniqueIDs;

    // Checking the size of the first line to see if it is a version string.
    qint32 startI = 0;
    QString possibleHeader = lines.first();
    //qDebug() << "Possible header is" << possibleHeader << " out of " << lines.size() << "lines";
    QStringList headerParts = possibleHeader.split(" ",Qt::SkipEmptyParts);
    if (headerParts.size() == 1){
        // Version string pesent
        startI = 1;
        versionString = headerParts.first().trimmed();
    }

    qint32 sizeCheck = 0;
    if (versionString == "v2"){
        sizeCheck = MAX_SEQUENCE_LENGTH+1;
    }
    else{
        error = "Unknown fielding description version: " + versionString;
        return false;
    }

    QVariantList nback_trials;

    for (qint32 i = startI; i < lines.size(); i++){

        if (lines.at(i).isEmpty()) continue;

        // Each NBack Trial is defined by an ID and a sequence of Ints.
        QString id;
        QVariantList sequence;

        QStringList tokens = lines.at(i).split(' ',Qt::SkipEmptyParts);

        if (tokens.size() != sizeCheck){
            error = "Invalid line: " + lines.at(i) + " should only have " + QString::number(sizeCheck) + " items separated by space, the id and "
                    + QString::number(MAX_SEQUENCE_LENGTH) +  " numbers. Version: " + versionString;
            return false;
        }

        id = tokens.first();

        // If the id was defined before, its an error. Otherwise we add it to the ids found list.
        if (uniqueIDs.contains(id)){
            error = "Cannot use the same id twice: " + id + " has already been found";
            return false;
        }
        uniqueIDs << id;

        for (qint32 j =1; j < tokens.size(); j++){
            bool ok;
            qint32 value = tokens.at(j).toInt(&ok);
            if (!ok){
                error = "In line: " + lines.at(i) + ", " + tokens.at(j) + " is not a valid integer";
                return false;
            }
            if ((value < 0) || (value > 5)){
                error = "In line: " + lines.at(i) + ", " + tokens.at(j) + " is not an integer between 0 and 5";
                return false;
            }
            sequence << value;
        }

        // We now create a new NBack Trial.
        QVariantMap trial;
        trial[RRS::NBack::NBackTrial::ID] = id;
        trial[RRS::NBack::NBackTrial::SEQ] = sequence;

        // We add the trial to the trial list.
        nback_trials << trial;
        if (nback_trials.size() == numberOfTrials) break;
    }

    // When we are done we add the trial list to the configuration.
    configuration[RRS::StudyConfigurationFields::NBACK_TRIALS] = nback_trials;
    return true;

}
