#ifndef EXPERIMENTDATAPAINTER_H
#define EXPERIMENTDATAPAINTER_H

#include <QGraphicsScene>
#include <QPainter>
#include "../common.h"
#include "../ConfigurationManager/configurationmanager.h"

// Screen resolution is fixed.
#define   SCREEN_W                                      1024
#define   SCREEN_H                                      768

// Number of actual trials to have in demo mode
#define   NUMBER_OF_TRIALS_IN_DEMO_MODE                 3

class ExperimentDataPainter
{
public:
    ExperimentDataPainter();

    // Basic functions to reimplement.
    virtual bool parseExpConfiguration(const QString &contents){ Q_UNUSED(contents) return false;}
    virtual void init(ConfigurationManager *c);
    virtual qint32 size() const {return 0;}
    virtual qreal sizeToProcess() const {return 0;}

    QPixmap getImage() const;
    QString getError() const {return error;}

    QGraphicsScene * getCanvas() {return canvas;}

protected:
    ConfigurationManager *config;
    QGraphicsScene *canvas;
    QString error;

    // Trims the number of trials to number in demo mode.
    virtual void enableDemoMode(){}

};

#endif // EXPERIMENTDATAPAINTER_H
