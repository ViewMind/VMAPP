#include "experimentdatapainter.h"

ExperimentDataPainter::ExperimentDataPainter()
{

}

void ExperimentDataPainter::init(ConfigurationManager *c){
    ScreenResolutionHeight = c->getReal(CONFIG_RESOLUTION_HEIGHT);
    ScreenResolutionWidth = c->getReal(CONFIG_RESOLUTION_WIDTH);
    canvas = new QGraphicsScene(0,0,ScreenResolutionWidth,ScreenResolutionHeight);
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
