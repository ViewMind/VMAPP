#include "calibrationhistory.h"

CalibrationHistory::CalibrationHistory()
{
    this->reset();
}

void CalibrationHistory::reset(){
    calibrationAttempts.clear();
    calibrationConfiguration.clear();
    awaitingForConfiguration = true;
}

void CalibrationHistory::setConfiguration(const QVariantMap &config){
    if (!awaitingForConfiguration) return;
    awaitingForConfiguration = false;
    calibrationConfiguration = config;
    // The likelyhood another calibration being created at the exact same microsecond is very small. This a very comfortable way to create a unique UID.
    calibrationConfiguration[VMDC::CalibrationConfigurationFields::CALIBRATION_UID] = QDateTime::currentDateTime().toString("yyyyMMddHHmmsszzz");
}

void CalibrationHistory::addCalibrationAttempt(const QVariantMap &attempt){
    calibrationAttempts << attempt;
}

QVariantMap CalibrationHistory::getHistory() const{
    QVariantMap map;
    map[VMDC::CalibrationFields::CALIBRATION_ATTEMPTS] = calibrationAttempts;
    map[VMDC::CalibrationFields::CONFIG_PARAMS] = calibrationConfiguration;
    return map;
}

QVariantMap CalibrationHistory::getMapOfLastAttempt()  const {

    QVariantMap map;

    if (calibrationAttempts.isEmpty()) return map;

    // We need to get the very last calibration attempt.
    map = calibrationAttempts.last().toMap();

    // And we merge it with the configuration so that no data is missing.

    QStringList keys = calibrationConfiguration.keys();
    for (qint32 i = 0; i < keys.size(); i++){
        map[keys.at(i)] = calibrationConfiguration[keys.at(i)];
    }

    return map;

}

