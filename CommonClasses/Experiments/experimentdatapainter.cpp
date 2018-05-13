#include "experimentdatapainter.h"

ExperimentDataPainter::ExperimentDataPainter()
{

}

void ExperimentDataPainter::init(ConfigurationManager *c){
    canvas = new QGraphicsScene(0,0,SCREEN_W,SCREEN_H);
    config = c;
}

QPixmap ExperimentDataPainter::getImage() const{
    // Drawing the graphics scene onto the painter.
    QPixmap image(canvas->width(),canvas->height());
    QPainter painter(&image);
    painter.setRenderHint(QPainter::Antialiasing);
    canvas->render(&painter);
    return image;
}
