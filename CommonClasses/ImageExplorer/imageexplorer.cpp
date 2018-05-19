#include "imageexplorer.h"

ImageExplorer::ImageExplorer(): QWidget(0)
{

    // UI Setup
    QPushButton *pbZoomIn = new QPushButton("+",this);
    QPushButton *pbZoomOut = new QPushButton("-",this);
    canvas = new QGraphicsView(this);
    QHBoxLayout *buttons = new QHBoxLayout();
    buttons->addWidget(pbZoomIn);
    buttons->addStretch();
    buttons->addWidget(pbZoomOut);
    QVBoxLayout *vlayout = new QVBoxLayout();
    vlayout->addLayout(buttons);
    vlayout->addWidget(canvas);
    canvas->setScene(new QGraphicsScene(0,0,500,1000));
    canvas->setRenderHint(QPainter::Antialiasing);

    // The layout

    this->setLayout(vlayout);

    // Connections
    connect(pbZoomIn,&QPushButton::clicked,this,&ImageExplorer::on_ZoomIn);
    connect(pbZoomOut,&QPushButton::clicked,this,&ImageExplorer::on_ZoomOut);

    this->setWindowTitle("ImageExplorer");
    picture = nullptr;

}

bool ImageExplorer::setImageFile(const QString &image){

    QFileInfo file(image);
    if (!file.exists()) return false;

    QImage qimage(image);
    if (qimage.isNull()) return false;

    QPixmap item = QPixmap::fromImage(qimage);

    this->setWindowTitle(file.completeBaseName() + "." + file.completeSuffix());
    canvas->scene()->clear();
    picture = canvas->scene()->addPixmap(item);
    picture->setPos(0,0);
    canvas->scene()->setSceneRect(0,0,picture->boundingRect().width(),picture->boundingRect().height());

    return true;

}

void ImageExplorer::on_ZoomIn(){

}

void ImageExplorer::on_ZoomOut(){

}

void ImageExplorer::resizeEvent(QResizeEvent *event){
    Q_UNUSED(event);
}
