#ifndef EXPERIMENTDATAPAINTER_H
#define EXPERIMENTDATAPAINTER_H

#include "../common.h"

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

};

#endif // EXPERIMENTDATAPAINTER_H
