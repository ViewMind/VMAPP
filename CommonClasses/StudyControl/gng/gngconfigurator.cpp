#include "gngconfigurator.h"

GNGConfigurator::GNGConfigurator(){
    this->studyDescriptionFile = ":/CommonClasses/StudyControl/gng/descriptions/go_no_go.dat";
}

bool GNGConfigurator::studySpecificConfiguration(const QVariantMap &studyConfig){

    Q_UNUSED(studyConfig)

    qint32 numberOfTrials = -1; // This will use all of them.
    if (this->shortStudies){
        numberOfTrials = N_TRIALS_IN_SHORT_STUDIES;
    }

    // Now we parse the trial. If it worked the configuration is loaded with the GoNoGo trials.
    if (!parseStudyDescription(numberOfTrials)) return false;

    // Now that the description was parsed, we can set all the other configuration constants.
    configuration[RRS::StudyConfigurationFields::STUDY] = VMDC::Study::GONOGO;
    configuration[RRS::StudyConfigurationFields::IS_STUDY_3D] = false;

    return true;
}

bool GNGConfigurator::parseStudyDescription(int numberOfTrials){

    QStringList trialTypeList;
    trialTypeList << "R<-" << "G<-" << "R->" << "G->";


    QStringList lines = this->studyDescriptionContent.split('\n',Qt::SkipEmptyParts);
    if (lines.isEmpty()){
        error = "No lines in experiment description";
        return false;
    }

    // The first line should be the version string.
    versionString = lines.first();
    versionString = versionString.trimmed();

    // Making sure the name is unique.
    QSet<QString> uniqueIDs;
    QVariantList trials;

    for (qint32 i = 1; i < lines.size(); i++){
        QStringList tokens = lines.at(i).split(" ",Qt::SkipEmptyParts);
        if (tokens.size() != 2){
            error = "Invalid number of tokens in line " + lines.at(i) + " it should only have 2 parts";
            return false;
        }

        QString name = tokens.first().trimmed();
        QString trialDesc = tokens.last().trimmed();

        if (uniqueIDs.contains(name)){
            error = "Repeated trial ID " + name;
            return false;
        }

        QVariantMap trial;

        qint32 trialTypeID = trialTypeList.indexOf(trialDesc);
        if (trialTypeID == -1){
            error = "Unrecognized trial type " + trialDesc + " in line " + lines.at(i);
            return false;
        }

        switch (trialTypeID){
        case 0:
            trial[RRS::GNG::COLOR_CODE] = "R";
            trial[RRS::GNG::RIGHT]      = false;
            break;
        case 1:
            trial[RRS::GNG::COLOR_CODE] = "G";
            trial[RRS::GNG::RIGHT]      = false;
            break;
        case 2:
            trial[RRS::GNG::COLOR_CODE] = "R";
            trial[RRS::GNG::RIGHT]      = true;
            break;
        case 3:
            trial[RRS::GNG::COLOR_CODE] = "G";
            trial[RRS::GNG::RIGHT]      = true;
            break;
        }

        trials << trial;
        if (trials.size() == numberOfTrials) break;
    }

    this->configuration[RRS::StudyConfigurationFields::GNG_ARROWS] = trials;

    return true;


}
