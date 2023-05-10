#ifndef CALIBRATIONHISTORY_H
#define CALIBRATIONHISTORY_H

#include "../RawDataContainer/VMDC.h"
#include <QVariantMap>
#include <QDateTime>

class CalibrationHistory
{
public:
    CalibrationHistory();

    // Resets the configuration and the attempt list. Sets the flag for awaiting for configuration.
    void reset();

    // Sets teh configuration if the awaitingForConfiguration flag is set. Then clears it. And geraates a calibration_history_id.
    void setConfiguration(const QVariantMap &config);

    // Adds a calibration attempt to the list. No Checks done.
    void addCalibrationAttempt(const QVariantMap &attempt);

    // Generates the map structure for calibration history to store in a study file.
    QVariantMap getHistory() const;

    // Get full map of the last attempt. This is required to display calibration data.
    QVariantMap getMapOfLastAttempt() const;

private:

    QVariantMap calibrationConfiguration;
    QVariantList calibrationAttempts;
    bool awaitingForConfiguration;


};

#endif // CALIBRATIONHISTORY_H
