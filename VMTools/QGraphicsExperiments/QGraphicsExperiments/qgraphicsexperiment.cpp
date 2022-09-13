#include "qgraphicsexperiment.h"
#include "ui_qgraphicsexperiment.h"

QGraphicsExperiment::QGraphicsExperiment(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::QGraphicsExperiment)
{
    ui->setupUi(this);
    canvas = new QGraphicsScene(ui->gvView);
    canvas->setSceneRect(0,0,1920, 1080);
    ui->gvView->setScene(canvas);
    RunTests();
}

QGraphicsExperiment::~QGraphicsExperiment()
{
    delete ui;
}

void QGraphicsExperiment::RunTests(){

    qreal center_x = canvas->sceneRect().width()/2;
    qreal center_y = canvas->sceneRect().height()/2;

    QGraphicsArrow arrow;
    qreal x2, y2;

    qreal delta = 300;

    x2 = center_x + delta;
    y2 = center_y + delta;
    arrow.setArrowColor(Qt::red);
    arrow.render(canvas,center_x,center_y,x2,y2);

    qreal D = QGraphicsArrow::DistanceBetweenTwoPoints(QPointF(center_x,center_y),QPointF(x2,y2));
    qDebug() << "D is" << D;
    QPointF p = QGraphicsArrow::PointAlongRectThatCreatesAShorterLengthLine(center_x,center_y,x2,y2,D*0.8);

    arrow.setArrowColor(QColor(Qt::blue));
    arrow.setArrowWidth(5);
    arrow.render(canvas,center_x,center_y,p.x(),p.y());

    x2 = center_x - delta;
    y2 = center_y - delta;
    arrow.setArrowColor(Qt::green);
    arrow.render(canvas,center_x,center_y,x2,y2);

    x2 = center_x + delta;
    y2 = center_y - delta;
    arrow.setArrowColor(Qt::blue);
    arrow.render(canvas,center_x,center_y,x2,y2);

    x2 = center_x - delta;
    y2 = center_y + delta;
    arrow.setArrowColor(Qt::cyan);
    arrow.render(canvas,center_x,center_y,x2,y2);

    x2 = center_x;
    y2 = center_y + delta;
    arrow.setArrowColor(Qt::magenta);
    arrow.render(canvas,center_x,center_y,x2,y2);

    x2 = center_x;
    y2 = center_y - delta;
    arrow.setArrowColor(Qt::darkYellow);
    arrow.render(canvas,center_x,center_y,x2,y2);


    x2 = center_x - delta;
    y2 = center_y;
    arrow.setArrowColor(Qt::darkBlue);
    arrow.render(canvas,center_x,center_y,x2,y2);

    x2 = center_x + delta;
    y2 = center_y;
    arrow.setArrowColor(Qt::darkGreen);
    arrow.render(canvas,center_x,center_y,x2,y2);


}


