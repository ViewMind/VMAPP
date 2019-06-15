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
        error = "Configuration for PatientSubject Structure (1) shouldb be I or B, but found " + QString(configStr.at(UIMAP_INDEX_BEHAVIORAL));
        return false;
    }

    if ((configStr.at(UIMAP_BLOCK_BIND_SIZE) == "N") || (configStr.at(UIMAP_BLOCK_BIND_SIZE) == "Y")) options[UIMAP_BLOCK_BIND_SIZE] = configStr.at(UIMAP_BLOCK_BIND_SIZE);
    else{
        error = "Configuration for PatientSubject Structure (2) shouldb be N or Y, but found " + QString(configStr.at(UIMAP_BLOCK_BIND_SIZE));
        return false;
    }

    if ((configStr.at(UIMAP_BLOCK_BIND_COUNT) == "N") || (configStr.at(UIMAP_BLOCK_BIND_COUNT) == "Y")) options[UIMAP_BLOCK_BIND_COUNT] = configStr.at(UIMAP_BLOCK_BIND_COUNT);
    else{
        error = "Configuration for PatientSubject Structure (3) shouldb be N or Y, but found " + QString(configStr.at(UIMAP_BLOCK_BIND_COUNT));
        return false;
    }

    if ((configStr.at(UIMAP_BIND_DEFAULT_COUNT) == "2") || (configStr.at(UIMAP_BIND_DEFAULT_COUNT) == "3")) options[UIMAP_BIND_DEFAULT_COUNT] = configStr.at(UIMAP_BIND_DEFAULT_COUNT);
    else{
        error = "Configuration for PatientSubject Structure (4) shouldb be 2 or 3, but found " + QString(configStr.at(UIMAP_BIND_DEFAULT_COUNT));
        return false;
    }

    return true;

}
