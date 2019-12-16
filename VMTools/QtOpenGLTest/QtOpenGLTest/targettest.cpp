#include "targettest.h"

TargetTest::TargetTest()
{
    canvas = nullptr;
    leftEye = nullptr;
    rightEye = nullptr;
    openGLTexture = nullptr;
}

bool TargetTest::initialize(qint32 screenw, qint32 screenh){

    canvas = new QGraphicsScene(0,0,screenw,screenh);
    canvas->setBackgroundBrush(QBrush(Qt::gray));

    qreal R = K_LARGE_D*screenw/2;
    r = K_SMALL_D*screenw/2;

    qreal horizontal_target_space =  (screenw -4*2*R)/2;
    qreal vertical_target_space   =  (screenh -4*2*R)/2;
    qreal horizontal_margin       =  R;
    qreal vertical_margin         =  R;
    qreal offset = (R-r);

    QList<QPointF> largeTargetUpperRight;
    largeTargetUpperRight << QPointF(horizontal_margin                                  ,vertical_margin)
                          << QPointF(horizontal_margin + 2*R + horizontal_target_space  ,vertical_margin)
                          << QPointF(horizontal_margin + 4*R + 2*horizontal_target_space,vertical_margin)
                          << QPointF(horizontal_margin                                  ,vertical_margin + 2*R + vertical_target_space)
                          << QPointF(horizontal_margin + 2*R + horizontal_target_space  ,vertical_margin + 2*R + vertical_target_space)
                          << QPointF(horizontal_margin + 4*R + 2*horizontal_target_space,vertical_margin + 2*R + vertical_target_space)
                          << QPointF(horizontal_margin                                  ,vertical_margin + 4*R + 2*vertical_target_space)
                          << QPointF(horizontal_margin + 2*R + horizontal_target_space  ,vertical_margin + 4*R + 2*vertical_target_space)
                          << QPointF(horizontal_margin + 4*R + 2*horizontal_target_space,vertical_margin + 4*R + 2*vertical_target_space);

    for (qint32 i = 0; i < largeTargetUpperRight.size(); i++){
        QGraphicsEllipseItem *circle = canvas->addEllipse(0,0,2*R,2*R,QPen(Qt::black),QBrush(Qt::darkBlue));
        QGraphicsEllipseItem *innerCircle = canvas->addEllipse(0,0,2*r,2*r,QPen(Qt::black),QBrush(Qt::yellow));
        qreal x = largeTargetUpperRight.at(i).x();
        qreal y = largeTargetUpperRight.at(i).y();
        circle->setPos(x,y);
        innerCircle->setPos(x+offset,y+offset);
    }

    // Initializing the
    leftEye = canvas->addEllipse(0,0,2*r,2*r,QPen(),QBrush(QColor(0,0,255,100)));
    rightEye = canvas->addEllipse(0,0,2*r,2*r,QPen(),QBrush(QColor(0,255,0,100)));

    return true;
}

GLuint TargetTest::getTextureGLID() const{
    if (openGLTexture != nullptr) return openGLTexture->textureId();
    else return 0;
}

void TargetTest::renderCurrentPosition(qint32 rx, qint32 ry, qint32 lx, qint32 ly){
    if (!canvas) return;

    leftEye->setPos(lx-r,ly-r);
    rightEye->setPos(rx-r,ry-r);

    //qDebug() << "X is" << rx << "Y is " << ry << canvas->sceneRect();

    QImage image(static_cast<int>(canvas->width()),static_cast<int>(canvas->height()),QImage::Format_RGB888);
    QPainter painter( &image );
    canvas->render( &painter );

    if (openGLTexture == nullptr){
        openGLTexture = new QOpenGLTexture(image.mirrored());
        openGLTexture->setMinificationFilter(QOpenGLTexture::Linear);
        openGLTexture->setMagnificationFilter(QOpenGLTexture::Linear);
    }
    else{
        openGLTexture->destroy();
        openGLTexture->create();
        openGLTexture->setSize(image.size().width()*image.size().height());
        openGLTexture->setData(image.mirrored(),QOpenGLTexture::DontGenerateMipMaps);
//        delete openGLTexture;
//        openGLTexture = new QOpenGLTexture(image.mirrored());
//        openGLTexture->setMinificationFilter(QOpenGLTexture::Linear);
//        openGLTexture->setMagnificationFilter(QOpenGLTexture::Linear);
    }
}

TargetTest::~TargetTest(){
    finalize();
}

void TargetTest::finalize(){
    if (canvas){
        delete canvas;
        canvas = nullptr;
    }
    leftEye = nullptr;
    rightEye = nullptr;
    if (openGLTexture) delete openGLTexture;
}

