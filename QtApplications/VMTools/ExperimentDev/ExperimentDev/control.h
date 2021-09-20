#ifndef CONTROL_H
#define CONTROL_H

#include <QObject>
#include <QImage>
#include "../../../CommonClasses/EyeTrackingInterface/calibrationtargets.h"
#include "../../../CommonClasses/EyeTrackingInterface/Mouse/mouseinterface.h"
#include "../../../CommonClasses/EyeTrackingInterface/HPReverb/hpomniceptinterface.h"
#include "../../../CommonClasses/EyeTrackingInterface/HTCVIVEEyePro/htcviveeyeproeyetrackinginterface.h"
#include "../../../CommonClasses/OpenVRControlObject/openvrcontrolobject.h"

class Control : public QObject
{
    Q_OBJECT

    Q_PROPERTY(QImage image READ image NOTIFY newImageAvailable)

public:
    explicit Control(QObject *parent = nullptr);

    static const quint8 ET_MOUSE     = 0;
    static const quint8 ET_HP_REBERV = 1;
    static const quint8 ET_HTC_VIVE  = 2;

    /**
     * @brief When this function is called, the light up target test is used as the source of images.
     * If useVR is false, then the w and h passed as a parameter is used.
     */
    Q_INVOKABLE void testEyeTracking();

    /**
     * @brief setEyeTracker: Setting the eye tracker to be used.
     * @param Define between mouse an an actual EyeTracker.
     */
    Q_INVOKABLE void setEyeTracker(quint8 eye_tracker);

    /**
     * @brief setRenderArea: Sets the size of the QML display area. Used for mouse mostly.
     * @param w width
     * @param h height
     */
    Q_INVOKABLE void setRenderArea(qint32 w, qint32 h);

    /**
     * @brief image, Returns the image to be shown in a QImageDisplay in QML
     * @return the internal variable displayImage
     */
    QImage image() const;

    /**
     * @brief Starts the calibration of the currently selected eyetracker.
     */
    Q_INVOKABLE void startCalibration();


    Q_INVOKABLE void switchEyeTrackerEnableState();

signals:
    void newImageAvailable();

public slots:
    void onRequestUpdate();
    void onEyeTrackerControl(quint8);

private slots:
    void onNewEyeTrackingData(EyeTrackerData data);

private:

    typedef enum { RENDERING_NONE, RENDERING_TARGET_TEST, RENDERING_EXPERIMENT, RENDER_WAIT_SCREEN, RENDERING_CALIBRATION } RenderState;
    RenderState renderState;

    qreal VRSCALING = 1.0;

    // Open VR.
    OpenVRControlObject *openvrco;

    // Retruned by image();
    QImage displayImage;

    // Calibration and test draw class.
    CalibrationTargets ct;

    // Render area. The size of the display area where the genenrated image will be shown.
    qint32 renderAreaWidth;
    qint32 renderAreaHeight;

    // EyeTracker to be used.
    quint8 selectedEyeTracker;
    EyeTrackerInterface *eyetracker;

    /**
     * @brief Generates an image depending on what we are rendering. The image should be assigned to displayImage;
     * @return
     */
    QImage generateHMDImage();

    /**
     * @brief getRenderingSize
     * @return The size of the rendering area dependant on the selected eyetracker.
     */
    QSize getRenderingSize();



};

#endif // CONTROL_H
