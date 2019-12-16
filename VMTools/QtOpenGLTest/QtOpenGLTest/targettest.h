#ifndef TARGETTEST_H
#define TARGETTEST_H


#include <QPainter>
#include <QGraphicsScene>
#include <QGraphicsEllipseItem>
#include <QOpenGLTexture>
#include <QImage>
#include <QDebug>

class TargetTest
{
public:
    TargetTest();
    ~TargetTest();

    bool initialize(qint32 screenw, qint32 screenh);
    void finalize();
    void setMaxWidthAndHeight(qint32 W, qint32 H);
    void renderCurrentPosition(qint32 rx, qint32 ry, qint32 lx, qint32 ly);
    GLuint getTextureGLID() const;


private:

    const qreal K_LARGE_D = 0.1;
    const qreal K_SMALL_D = 0.02;

    QGraphicsScene *canvas;
    QGraphicsEllipseItem *leftEye;
    QGraphicsEllipseItem *rightEye;
    QOpenGLTexture *openGLTexture;
    qreal r;
};

#endif // TARGETTEST_H
