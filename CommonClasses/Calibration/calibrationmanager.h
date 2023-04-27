#ifndef CALIBRATIONMANAGER_H
#define CALIBRATIONMANAGER_H

#include <QObject>
#include <QFile>

#include "eyecorrectioncoefficients.h"
#include "calibrationtargets.h"
#include "calibrationvalidation.h"
#include "../LogInterface/staticthreadlogger.h"

class CalibrationManager : public QObject
{
    Q_OBJECT

public:

    // Calibration done codes.
    static const qint32 CALIBRATION_SUCCESSFUL        = 0;
    static const qint32 CALIBRATION_FAILED            = 1;
    static const qint32 CALIBRATION_FILE_LOAD_FAILED  = 2;

    explicit CalibrationManager(QObject *parent = nullptr);

    void startCalibration(qint32 width,  qint32 height, //   The dimentions of the screen. Required by the 2D Calibration.And to draw the validation graph.
            bool mode3D, const QVariantMap &calib_valid_params, // The calibration validation parameters.
            const QString coefficient_file_name // The name of the file where we should store the resulting coefficients. If the file exists this is loaded from there.
    );

    void addEyeDataToCalibrationPoint(float xl, float xr, float yl, float yr, float zl, float zr);

    QString getRecommendedEye() const;

    QVariantMap getCalibrationAttemptData() const;

    QVariantMap getCalibrationConfigurationParameters() const;

    QString getCalibrationValidationReport() const;

    void process3DCalibrationEyeTrackingData(const RenderServerPacket &calibrationData);

    RenderServerPacket getRenderServerPacket() const;

    bool isCalibrationCompleted() const;

    bool requires2DCalibrationDataPointSamples() const;

    void resetCalibrationCompleted();

    EyeTrackerData correct2DData(EyeTrackerData input);

    // For Debugging ONLY.
    void debugSaveCalibrationValidationData(const QString &filename);

signals:
    void newPacketAvailable();
    void calibrationDone(qint32 code);

private slots:
    void onWaitToDataGather();
    void onNewAnimationFrame();
    void onCalibrationPointStatus(qint32 whichCalibrationPoint, bool isMoving);

private:

    ////////////////////////////// GENERAL Calibration Variables /////////////////

    // Holds the number of ms before start gathering data.
    qint32 calibration_wait_time;

    // Holds the number of ms when we gather data.
    qint32 calibration_gather_time;

    // Index of the list of collectedCalibrationDataPoints.
    qint32 currentCalibrationPointIndex;

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

    ////////////////////////////// 2D Calibration VARIABLES //////////////////////
    CalibrationTargets calibration2DTargetControl;

    // Timer to indicate when to start gathering data
    QTimer waitToGatherTimer;

    // Flag that indicates if we are in the wait time of calibration or the gather time.
    bool isDataGatheringEnabled;

    // Flag to indicate the whether to send or not the animation frame packet.
    bool enableSendAnimationFrames;

    ////////////////////////////// 3D Calibration VARIABLES //////////////////////

    // The target vectors used to compute the calibration. Before normalization.
    QList<QVector3D> nonNormalizedTargetVectors;

    // The validation radious. This is used to know if the predicted corrections for the raw data are close enough to its corresponding target vector.
    qreal validationRadious;

    ////////////////////////////// GENERAL Functions /////////////////
    void finalizeCalibrationProcess(qint32 code, bool sendCalibrationCoefficientsToServer = false);
    void sendCalibrationCoefficientPacket();

    ////////////////////////////// DEBUG LOAD FUNCTIONS /////////////////
    bool debugLoadFixed3DCalibrationParameters();
    bool debugLoadFixed2DCalibrationParameters();

};

#endif // CALIBRATIONMANAGER_H
