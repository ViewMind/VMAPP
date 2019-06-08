#include "uiconfigmap.h"

UIConfigMap::UIConfigMap()
{

}

bool UIConfigMap::setConfigurationString(const QString &configStr){

    if (configStr.size() != UI_MAP_EXPECTED_SIZE){
        error = "Configuration string " + configStr + " is supposed to be " + QString::number(UI_MAP_EXPECTED_SIZE) + " characters long";
        return false;
    }

    if ((configStr.at(UIMAP_PATIENT_SUBJECT) == "P") || (configStr.at(UIMAP_PATIENT_SUBJECT) == "S")) options[UIMAP_PATIENT_SUBJECT] = configStr.at(UIMAP_PATIENT_SUBJECT);
    else{
        error = "Configuration for PatientSubject Structure (0) shouldb be P or S, but found " + QString(configStr.at(UIMAP_PATIENT_SUBJECT));
        return false;
    }

    if ((configStr.at(UIMAP_INDEX_BEHAVIORAL) == "I") || (configStr.at(UIMAP_INDEX_BEHAVIORAL) == "B")) options[UIMAP_INDEX_BEHAVIORAL] = configStr.at(UIMAP_INDEX_BEHAVIORAL);
    else{
        error = "Configuration for PatientSubject Structure (1) shouldb be I or B, but found " + QString(configStr.at(UIMAP_PATIENT_SUBJECT));
        return false;
    }

    if ((configStr.at(UIMAP_BLOCK_BIND_OPTIONS) == "N") || (configStr.at(UIMAP_BLOCK_BIND_OPTIONS) == "Y")) options[UIMAP_BLOCK_BIND_OPTIONS] = configStr.at(UIMAP_BLOCK_BIND_OPTIONS);
    else{
        error = "Configuration for PatientSubject Structure (2) shouldb be N or Y, but found " + QString(configStr.at(UIMAP_PATIENT_SUBJECT));
        return false;
    }

    return true;

}
