#ifndef CALIBRATIONMANAGER_H
#define CALIBRATIONMANAGER_H

#include <QObject>
#include <QFile>

#include "eyecorrectioncoefficients.h"
#include "calibrationvalidation.h"
#include "../LogInterface/staticthreadlogger.h"
#include "../RenderServerClient/RenderServerPackets/renderserverpacket.h"
#include "../RenderServerClient/RenderServerPackets/RenderServerStrings.h"

class CalibrationManager : public QObject
{
    Q_OBJECT

public:

    // Calibration done codes.
    static const qint32 CALIBRATION_SUCCESSFUL        = 0;
    static const qint32 CALIBRATION_FAILED            = 1;
    static const qint32 CALIBRATION_FILE_LOAD_FAILED  = 2;

    explicit CalibrationManager(QObject *parent = nullptr);

    void startCalibration(bool mode3D, // To decide on either 2D or 3D calibration.
                          const QVariantMap &calib_valid_params, // The calibration validation parameters.
                          const QString coefficient_file_name // The name of the file where we should store the resulting coefficients. If the file exists this is loaded from there.
    );


    QString getRecommendedEye() const;

    QVariantMap getCalibrationAttemptData() const;

    QVariantMap getCalibrationConfigurationParameters() const;

    QString getCalibrationValidationReport() const;

    void processCalibrationData(const RenderServerPacket &calibrationData);

    RenderServerPacket getRenderServerPacket() const;

    bool isCalibrationCompleted() const;

    void resetCalibrationCompleted();

    QSize getResolution() const;

    // For Debugging ONLY.
    void debugSaveCalibrationValidationData(const QString &filename);

signals:
    void newPacketAvailable();
    void calibrationDone(qint32 code);

private:

    ////////////////////////////// GENERAL Calibration Variables /////////////////

    // Holds the number of ms before start gathering data.
    qint32 calibration_wait_time;

    // Holds the number of ms when we gather data.
    qint32 calibration_gather_time;

    // Number of calibration points
    qint32 numberOfCalibrationPoints;

    // The actual coefficiens for when the calibration is
    EyeCorrectionCoefficients correctionCoefficients;

    // The file name in which to save.
    QString coefficientFileName;

    // Flag that indicates the configured calibration mode.
    bool calibrationMode3D;

    // The packet to send the Remote render server
    RenderServerPacket renderServerPacket;

    // Analyzes the calibration data in order to genrate both the data that is required for the report and to provide the recommend eye and the validation report.
    CalibrationValidation calibrationValidation;

    // This actually stores the data which we use to configure the calibration validation.
    QVariantMap calibrationValidationData;

    // Simple flag to determine whether the calibration has been done at least once.
    bool isCalibrated;

    // The target vectors used to compute the calibration. Before normalization.
    QList<QVector3D> nonNormalizedTargetVectors;

    // This are the 2D coordinates for 2D calibration.
    QList<QPointF> targetPoints2D;

    // The validation radious. This is used to know if the predicted corrections for the raw data are close enough to its corresponding target vector.
    qreal validationRadious;

    // The 2D resolution dimensions that come with the calibration packet. Necessary in order to properly render the result dialog.
    qint32 resolutionWidth;
    qint32 resolutionHeight;

    ////////////////////////////// GENERAL Functions /////////////////
    void finalizeCalibrationProcess(qint32 code);
    void sendCalibrationCoefficientPacket();
    void addEyeDataToCalibrationPoint(float xl, float xr, float yl, float yr, float zl, float zr, qint32 index);

    /**
     * @brief compute2DTargetLocations - Computes target locations using screen percents.
     * @details When using a 3D calibration the target vectors are the non normalized vectors that define
     * the target positon. The UI still requires to know the 2D locations of the targets as if it were a
     * 2D calibration in order to properly plot the dialog with the circles and percents. It does this by
     * using an approximation of the value in X and Y representing the "width" resolution of the screen
     * and the "height"
     */
    void compute2DTargetLocations();

    ////////////////////////////// DEBUG LOAD FUNCTIONS /////////////////
    RenderServerPacket debugLoadFixed3DCalibrationParameters();
    RenderServerPacket debugLoadFixed2DCalibrationParameters();

    const qreal K_LARGE_D = 0.1;
    const qreal VIRTUAL_3D_W = 80/3;
    const qreal VIRTUAL_3D_H = 70/3;

};

#endif // CALIBRATIONMANAGER_H
