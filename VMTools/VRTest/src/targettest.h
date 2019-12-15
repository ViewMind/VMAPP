#ifndef TARGETTEST_H
#define TARGETTEST_H


#include <QPainter>
#include <QGraphicsScene>
#include <QGraphicsEllipseItem>
#include <QOpenGLFramebufferObject>
#include <QOpenGLPaintDevice>
#include <QOffscreenSurface>
#include <QDebug>

class TargetTest
{
public:
    TargetTest();
    ~TargetTest();

    void initialize(quint32 screenw, quint32 screenh);
    void finalize();
    void renderCurrentPosition(qint32 rx, qint32 ry, qint32 lx, qint32 ly);
    QSize getRecommendedRenderSize() { return renderSize; }
    ///QOpenGLFramebufferObject * getFBO() { return fbo;}


private:

    const qreal K_LARGE_D = 0.1;
    const qreal K_SMALL_D = 0.02;

    QGraphicsScene *canvas;    
//    QOpenGLFramebufferObject *fbo;
//    QOpenGLContext *openGLContext;
//    QOffscreenSurface *offscreenSurface;
    QGraphicsEllipseItem *leftEye;
    QGraphicsEllipseItem *rightEye;
    QSize renderSize;
    qreal r;
};

#endif // TARGETTEST_H
