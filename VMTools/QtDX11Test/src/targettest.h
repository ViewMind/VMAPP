#ifndef TARGETTEST_H
#define TARGETTEST_H

#include <QPainter>
#include <QGraphicsScene>
#include <QGraphicsEllipseItem>

class TargetTest
{
public:
    TargetTest();
    ~TargetTest();

    void initialize(qint32 screenw, qint32 screenh);
    void finalize();
    QImage renderCurrentPosition(qint32 rx, qint32 ry, qint32 lx, qint32 ly);


private:

    const qreal K_LARGE_D = 0.1;
    const qreal K_SMALL_D = 0.02;

    QGraphicsScene *canvas;
    QGraphicsEllipseItem *leftEye;
    QGraphicsEllipseItem *rightEye;
    QImage currentRender;
    QPainter *painter;
    qreal r;
};

#endif // TARGETTEST_H
