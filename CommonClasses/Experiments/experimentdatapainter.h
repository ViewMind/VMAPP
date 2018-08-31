#ifndef EXPERIMENTDATAPAINTER_H
#define EXPERIMENTDATAPAINTER_H

#include <QGraphicsScene>
#include <QPainter>
#include <QDesktopWidget>
#include <QApplication>
#include "../common.h"
#include "../ConfigurationManager/configurationmanager.h"

// Number of actual trials to have in demo mode
#define   NUMBER_OF_TRIALS_IN_DEMO_MODE                 3

class ExperimentDataPainter
{
public:
    ExperimentDataPainter();
    ~ExperimentDataPainter();

    // Basic functions to reimplement.
    virtual bool parseExpConfiguration(const QString &contents){ Q_UNUSED(contents) return false;}
    virtual void init(ConfigurationManager *c);
    virtual qint32 size() const {return 0;}
    virtual qreal sizeToProcess() const {return 0;}

    QPixmap getImage() const;
    QString getError() const {return error;}

    QGraphicsScene * getCanvas() {return canvas;}

    QString getVersion() const { return versionString; }

protected:

    qreal ScreenResolutionWidth;
    qreal ScreenResolutionHeight;

    ConfigurationManager *config;
    QGraphicsScene *canvas;
    QString error;
    QString versionString;

    // Trims the number of trials to number in demo mode.
    virtual void enableDemoMode(){}

};

#endif // EXPERIMENTDATAPAINTER_H
