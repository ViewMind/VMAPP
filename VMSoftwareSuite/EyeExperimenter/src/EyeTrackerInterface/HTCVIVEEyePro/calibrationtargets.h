#ifndef TARGETTEST_H
#define TARGETTEST_H


#include <QPainter>
#include <QGraphicsScene>
#include <QGraphicsEllipseItem>
#include <QImage>
#include <QDebug>


class CalibrationTargets
{
public:
    CalibrationTargets();
    ~CalibrationTargets();

    void initialize(qint32 screenw, qint32 screenh);

    QImage setSingleTarget(qint32 x, qint32 y);
    QImage getClearScreen();

    void setTargetTest();
    void saveCanvasToTestImageFile();

    QImage renderCurrentPosition(qint32 rx, qint32 ry, qint32 lx, qint32 ly);

private:

    const qreal K_LARGE_D = 0.1;
    const qreal K_SMALL_D = 0.02;

    QGraphicsScene *canvas;
    QGraphicsEllipseItem *leftEye;
    QGraphicsEllipseItem *rightEye;
    qreal r;

};

#endif // TARGETTEST_H
