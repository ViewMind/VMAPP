#include "dotfollowconfigurator.h"

DotFollowConfigurator::DotFollowConfigurator() {
    this->studyDescriptionFile = ":/CommonClasses/StudyControl/dotfollow/descriptions/dotfollow.dat";
}

bool DotFollowConfigurator::studySpecificConfiguration(const QVariantMap &studyConfig){

    Q_UNUSED(studyConfig)

    // The trials in this study are the dot movements. However they won't be treated
    // as trials as the image is seldom static.
    qint32 numberOfMovements = -1; // This will use all of them.
    if (this->shortStudies){
        numberOfMovements = N_TRIALS_IN_SHORT_STUDIES;
    }

    // We create the configuration that we will send to the RRS
    if (!parseStudyDescription(numberOfMovements)) return false;

    // Now that the description was parsed, we can set all the other configuration constants.
    configuration[RRS::StudyConfigurationFields::STUDY]          = VMDC::Study::MOVING_DOT;
    configuration[RRS::StudyConfigurationFields::IS_STUDY_3D]    = false;
    configuration[RRS::StudyConfigurationFields::MIN_HOLD_TIME]  = DF_MIN_HOLD_TIME;
    configuration[RRS::StudyConfigurationFields::MAX_HOLD_TIME]  = DF_MAX_HOLD_TIME;
    configuration[RRS::StudyConfigurationFields::MOVE_HOLD_TIME] = DF_MOVE_END_HOLD_TIME;
    configuration[RRS::StudyConfigurationFields::MOVE_TIME]      = DF_MOVE_TIME;

    return true;

}


bool DotFollowConfigurator::parseStudyDescription(int numberOfMovements){

    QString MT_MOVE     = "MV";
    QString MT_GO       = "IN";
    QString MT_HOLD     = "HL";
    QString MT_CLWISE   = "CW";
    QString MT_COUTCL   = "CC";
    QString MT_BKG_ON   = "BL";
    QString MT_BKG_OFF  = "BO";
    QString MT_RIGHT_ON = "RO";
    QString MT_LEFT_ON  = "LO";

    QStringList movements;
    movements << MT_CLWISE << MT_COUTCL << MT_GO << MT_HOLD << MT_MOVE << MT_BKG_OFF << MT_BKG_ON << MT_RIGHT_ON << MT_LEFT_ON;

    QStringList lines = this->studyDescriptionContent.split('\n',Qt::SkipEmptyParts);
    if (lines.isEmpty()){
        error = "No lines in experiment description";
        return false;
    }

    // The first line should be the version string.
    versionString = lines.first();
    versionString = versionString.trimmed();

    QVariantList moves;

    for (qint32 i = 1; i < lines.size(); i++){

        QStringList tokens = lines.at(i).split(" ",Qt::SkipEmptyParts);

        if (tokens.size() < 2){
            error = "Invalid number of tokens in line " + lines.at(i) + " it should have at least 2 parts";
            return false;
        }

        // We parse the origin point.
        bool ok         = true;
        int origin      = tokens.at(0).toInt(&ok);
        int destination = -1;
        int hold_time   = 0;

        if (!ok){
            error = "Origin point in line '" + lines.at(i) + "' is invalid";
            return false;
        }
        if ((origin < 0) && (origin > 8)){
            error = "Origin point in line '" + lines.at(i) + "' is invalid. Should be between 0 and 9";
            return false;
        }

        // Now the move type.
        QString move_type = tokens.at(1);

        if (move_type == MT_MOVE){

            // We need the destination.
            if (tokens.size() < 3){
                error = "Origin point in line '" + lines.at(i) + "' is invalid. There should be at least 3 parts of MV type move";
                return false;
            }

            destination = tokens.at(2).toInt(&ok);
            if (!ok){
                error = "Destination point in line '" + lines.at(i) + "' is invalid";
                return false;
            }
            if ((origin < 0) && (origin > 8)){
                error = "Destination point in line '" + lines.at(i) + "' is invalid. Should be between 0 and 9";
                return false;
            }
        }
        else if ((move_type == MT_HOLD) ||
                 (move_type == MT_BKG_OFF) || (move_type == MT_BKG_ON) ||
                 (move_type == MT_RIGHT_ON) || (move_type == MT_LEFT_ON) ){

            // We need the destination.
            if (tokens.size() < 3){
                error = "Origin point in line '" + lines.at(i) + "' is invalid. There should be at least 3 parts of " + move_type + " type move";
                return false;
            }

            hold_time = tokens.at(2).toInt(&ok);
            if (!ok){
                error = "Hold time in line '" + lines.at(i) + "' is invalid";
                return false;
            }

        }
        else {

            // We just need to check for validity.
            if (!movements.contains(move_type)){
                error = "Invalid movement type on line '" + lines.at(i) + "'";
                return false;
            }

        }


        // All is good we make the dot follow object.
        QVariantMap dfObject;
        dfObject[RRS::DOT_FOLLOW::END_POS]   = destination;
        dfObject[RRS::DOT_FOLLOW::HOLD_TIME] = hold_time;
        dfObject[RRS::DOT_FOLLOW::MOVE_TYPE] = move_type;
        dfObject[RRS::DOT_FOLLOW::START_POS] = origin;


        moves << dfObject;
        if (numberOfMovements > 0){
            if (moves.size() >= numberOfMovements) break;
        }


    }

    this->configuration[RRS::StudyConfigurationFields::DOT_FOLLOW] = moves;
    return true;

}
