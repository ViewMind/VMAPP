#include "gngspheresconfigurator.h"

GNGSpheresConfigurator::GNGSpheresConfigurator(): StudyConfigurator() {
    this->studyDescriptionFile = ":/CommonClasses/StudyControl/gng3D/descriptions/gonogo_spheres.dat";
}

bool GNGSpheresConfigurator::studySpecificConfiguration(const QVariantMap &studyConfig){

    qreal min_speed     = studyConfig.value(VMDC::StudyParameter::MIN_SPEED).toReal();
    qreal max_speed     = studyConfig.value(VMDC::StudyParameter::MAX_SPEED).toReal();
    qreal initial_speed = studyConfig.value(VMDC::StudyParameter::INITIAL_SPEED).toReal();

    qint32 numberOfTrials = -1; // This will use all of them.
    if (this->shortStudies){
        numberOfTrials = N_TRIALS_IN_SHORT_STUDIES;
    }
    else if (min_speed == max_speed){
        numberOfTrials = N_TRIALS_IN_NON_VS_STUDIES;
    }

    // Now we parse the trial. If it worked the configuration is loaded with the NBack trials.
    if (!parseStudyDescription(numberOfTrials)) return false;

    //qDebug() << "Number of trials for GNG3D" << numberOfTrials << "Speed Setting: "  << min_speed <<  initial_speed << max_speed;

    if (min_speed == max_speed) configuration[RRS::StudyConfigurationFields::STUDY] = VMDC::Study::SPHERES;
    else configuration[RRS::StudyConfigurationFields::STUDY] = VMDC::Study::SPHERES_VS;

    QString handToUse = studyConfig.value(VMDC::StudyParameter::HAND_TO_USE).toString();

    if (handToUse == VMDC::Hand::BOTH){
        configuration[RRS::StudyConfigurationFields::HAND_L_ENABLED] = true;
        configuration[RRS::StudyConfigurationFields::HAND_R_ENABLED] = true;
    }
    else if (handToUse == VMDC::Hand::LEFT){
        configuration[RRS::StudyConfigurationFields::HAND_L_ENABLED] = true;
        configuration[RRS::StudyConfigurationFields::HAND_R_ENABLED] = false;
    }
    else if (handToUse == VMDC::Hand::RIGHT){
        configuration[RRS::StudyConfigurationFields::HAND_L_ENABLED] = false;
        configuration[RRS::StudyConfigurationFields::HAND_R_ENABLED] = true;
    }
    else {
        error = "Invalid hand configuration: " + handToUse;
        return false;
    }

    if (min_speed > max_speed){
        error = "Specified minimum speed (" + QString::number(min_speed) + ") is lower than the specified maxiumum speed (" + QString::number(max_speed) + ")";
        return false;
    }
    if (initial_speed < min_speed){
        error = "Specified minimum speed (" + QString::number(min_speed) + ") is higher than the specified initial speed (" + QString::number(initial_speed) + ")";
        return false;

    }
    if (initial_speed > max_speed){
        error = "Specified maximum speed (" + QString::number(max_speed) + ") is lower than the specified initial speed (" + QString::number(initial_speed) + ")";
        return false;
    }

    configuration[RRS::StudyConfigurationFields::MIN_SPEED]     = min_speed;
    configuration[RRS::StudyConfigurationFields::MAX_SPEED]     = max_speed;
    configuration[RRS::StudyConfigurationFields::INITIAL_SPEED] = initial_speed;
    configuration[RRS::StudyConfigurationFields::IS_STUDY_3D]   = true;

    return true;

}

bool GNGSpheresConfigurator::parseStudyDescription(int numberOfTrials){
    QJsonParseError json_error;
    QJsonDocument doc = QJsonDocument::fromJson(studyDescriptionContent.toUtf8(),&json_error);
    if (doc.isNull()){
        error = "Error parsing the JSON file description: " + json_error.errorString();
        return false;
    }
    QVariantMap data = doc.object().toVariantMap();

    if (!data.contains(JFIELD_SPHERES)){
        error = "JSON Description does not contain 'spheres' key";
        return false;
    }

    QVariantList spheres;

    QVariantList list = data.value(JFIELD_SPHERES).toList();

    for (qint32 i = 0; i < list.size(); i++){

        QVariantMap sphere = list.at(i).toMap();
        QString color_code = sphere.value(JFIELD_COLOR).toString();
        qint32  origin     = sphere.value(JFIELD_ORIGIN).toInt();

        if ((color_code != "R") && (color_code != "G")){
            error = "Description contains unexpected color code: '" + color_code + "'";
            return false;
        }
        if ((origin < 0) || (origin >= N_ORIGINS)){
            error = "Description contains invalid origin value of: '" + QString::number(origin) + "'";
            return false;
        }

        QVariantMap map;
        map[RRS::GNGSpheres::Sphere::ID] = i;
        map[RRS::GNGSpheres::Sphere::COLOR] = color_code;
        map[RRS::GNGSpheres::Sphere::ORIGIN] = origin;

        spheres << map;

        if (spheres.size() == numberOfTrials){
            break;
        }

    }

    configuration[RRS::StudyConfigurationFields::GNG3D_SPHERES] = spheres;
    return true;

}
