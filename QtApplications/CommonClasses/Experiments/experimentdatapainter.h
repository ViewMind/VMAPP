#ifndef EXPERIMENTDATAPAINTER_H
#define EXPERIMENTDATAPAINTER_H

#include <QGraphicsScene>
#include <QGraphicsEllipseItem>
#include <QPainter>
#include <QApplication>
#include "../ConfigurationManager/configurationmanager.h"
#include "../eyetracker_defines.h"

// Number of actual trials to have in demo mode
#define   NUMBER_OF_TRIALS_IN_DEMO_MODE                 3

class ExperimentDataPainter
{
public:
    ExperimentDataPainter();
    virtual ~ExperimentDataPainter();

    // Basic functions to reimplement.
    virtual bool parseExpConfiguration(const QString &contents){ Q_UNUSED(contents) return false;}
    virtual void init(qreal display_resolution_width, qreal display_resolution_height);
    virtual void configure(const QVariantMap &configuration);
    virtual qint32 size() const {return 0;}
    virtual qreal sizeToProcess() const {return 0;}

    // For debugging
    void setShortStudy();

    // For on-screen gaze tracking.
    void updateGazePoints(qreal xr, qreal xl, qreal yr, qreal yl);
    void redrawGazePoints();

    // Trims the number of trials to number in demo mode.
    virtual void enableDemoMode(){}

    QPixmap getImage() const;
    QImage getQImage() const;
    QString getError() const {return error;}

    QGraphicsScene * getCanvas() {return canvas;}

    QString getVersion() const { return versionString; }

    QList<QStringList> getExpectedIDs() const { return expectedIDs; }

protected:

    void updateGazePosition();
    void clearCanvas();

    qreal ScreenResolutionWidth;
    qreal ScreenResolutionHeight;

    // Enable on-screen gaze tracking
    bool gazeUpdateEnabled; // This flag needs to be used to ensure that NO update is done when the ellipse items don´t exist;
    qreal gazeXr,gazeXl,gazeYr,gazeYl;
    QGraphicsEllipseItem *leftEyeTracker;
    QGraphicsEllipseItem *rightEyeTracker;
    qreal R;

    // The expected ids for the experiment
    QList<QStringList> expectedIDs;

    //ConfigurationManager *config;
    QGraphicsScene *canvas;
    QString error;
    QString versionString;

};

#endif // EXPERIMENTDATAPAINTER_H
