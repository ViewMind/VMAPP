#ifndef DATAPLOTTER_H
#define DATAPLOTTER_H

#include <QGraphicsScene>
#include <QGraphicsEllipseItem>
#include <QPainter>
#include <QDesktopWidget>
#include <QApplication>

#include "dataextractor.h"

class DataPlotter
{
public:
    DataPlotter();
    void plotPoints(const QString &outputFileName, const DataExtractor::DataPoints &datapoints);
};

#endif // DATAPLOTTER_H
