#ifndef BARGRAPHER_H
#define BARGRAPHER_H

#include <QPainter>
#include <QImage>
#include <QDebug>
#include <QStringList>
#include <QtMath>

class BarGrapher
{
public:
    BarGrapher();

    typedef QList< QList<qreal> > DataSet;

    // Configuration of the Bar Graph
    void configureGraphText(qreal width, qreal height, const QStringList &xItems, const QString &ylabel, const QString &title);

    // Draw it.
    QImage generateGraph(const DataSet &dataSet);

    // Get an error if there was one.
    QString getError() const {return error;}

    // Simply merge images in a grid pattern according to their position in a matrix.
    static QImage mergeImagesInGrid(const  QList< QList<QImage> > & graphs);

private:

    // This is sqrt(2)/2 which corresponds to sin(pi/2)
    static const qreal PROYECTION_FACTOR;
    static const qreal LEFT_MARGIN;
    static const qreal RIGHT_MARGIN;
    static const qreal TOP_MARGIN;

    // Set by the class user
    QStringList itemDescriptions;
    QString yAxisText;
    QString graphTitle;

    qreal W;
    qreal H;

    QString error;


};

#endif // BARGRAPHER_H
