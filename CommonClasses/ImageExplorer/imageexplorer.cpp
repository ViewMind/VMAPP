#include "imageexplorer.h"

const QString ImageExplorer::FIT_NOFIT =  "Control + MouseWheel to Zoom In and Out";
const QString ImageExplorer::FIT_FIT_WIDTH  = "Fit Width";
const QString ImageExplorer::FIT_FIT_HEIGHT = "Fit Height";


ImageExplorer::ImageExplorer(): QWidget(0)
{

    // UI Setup
    zoomValue = new QLabel(FIT_FIT_WIDTH,this);
    pbFit = new QPushButton(FIT_FIT_WIDTH,this);
    canvas = new QGraphicsView(this);
    QHBoxLayout *buttons = new QHBoxLayout();
    buttons->addWidget(pbFit);
    buttons->addStretch();
    buttons->addWidget(zoomValue);
    QVBoxLayout *vlayout = new QVBoxLayout();
    vlayout->addLayout(buttons);
    vlayout->addWidget(canvas);
    canvas->setScene(new QGraphicsScene(0,0,500,1000));
    canvas->setRenderHints(QPainter::Antialiasing | QPainter::SmoothPixmapTransform | QPainter::HighQualityAntialiasing);

    // The layout

    this->setLayout(vlayout);

    // Connections;
    connect(pbFit,&QPushButton::clicked,this,&ImageExplorer::on_FitTypeChanged);

    this->setWindowTitle("ImageExplorer");
    havePixmap = false;
    zoomEnabled = false;

}

void ImageExplorer::setZoomType(const QString &fit){
    if (fit == FIT_FIT_WIDTH) pbFit->setText(FIT_FIT_WIDTH);
    else if (fit == FIT_FIT_HEIGHT) pbFit->setText(FIT_FIT_HEIGHT);
    else pbFit->setText(FIT_NOFIT);
    resizeEvent(nullptr);
}

void ImageExplorer::on_FitTypeChanged(){
    if (pbFit->text() == FIT_FIT_WIDTH){
        pbFit->setText(FIT_FIT_HEIGHT);
        zoomValue->setText(FIT_FIT_HEIGHT);
    }
    else if (pbFit->text() == FIT_FIT_HEIGHT){
        pbFit->setText(FIT_NOFIT);
        zoomValue->setText(QString::number(zoomScale) + "%");
    }
    else{
        pbFit->setText(FIT_FIT_WIDTH);
        zoomValue->setText(FIT_FIT_WIDTH);
    }
    resizeEvent(nullptr);
}

bool ImageExplorer::setImageFile(const QString &image){

    QFileInfo file(image);
    if (!file.exists()) return false;

    QImage qimage(image);
    if (qimage.isNull()) return false;

    // By default the image is fit to the width of the window.
    pbFit->setText(FIT_FIT_WIDTH);

    picture = QPixmap::fromImage(qimage);
    wOriginal = qimage.width();
    hOriginal = qimage.height();
    havePixmap = true;
    zoomScale = 100;

    this->setWindowTitle(file.completeBaseName() + "." + file.completeSuffix());
    resizeEvent(nullptr);
    return true;

}


void ImageExplorer::wheelEvent(QWheelEvent *event){

    if (!zoomEnabled){
        QWidget::wheelEvent(event);
        return;
    }
    if (event->angleDelta().y() < 0){
        if (zoomScale > MIN_ZOOM){
            zoomScale = zoomScale - ZOOM_STEP;
            zoomValue->setText(QString::number(zoomScale) + "%");
            resizeEvent(nullptr);
        }
    }
    else{
        if (zoomScale < MAX_ZOOM){
            zoomScale = zoomScale + ZOOM_STEP;
            zoomValue->setText(QString::number(zoomScale) + "%");
            resizeEvent(nullptr);
        }
    }
}

void ImageExplorer::keyPressEvent(QKeyEvent *event){
    if (event->key() == Qt::Key_Control){
        if (pbFit->text() == FIT_NOFIT) zoomEnabled = true;
        else zoomEnabled = false;
    }
}

void ImageExplorer::keyReleaseEvent(QKeyEvent *event){
    Q_UNUSED(event);
    zoomEnabled = false;
}

void ImageExplorer::resizeEvent(QResizeEvent *event){
    Q_UNUSED(event);
    if (havePixmap){
        if (pbFit->text() != FIT_NOFIT){
            // The scene rect contains the picture dimensions
            qreal scale = 1;
            qreal w, h;
            if (pbFit->text() == FIT_FIT_WIDTH){
                scale = (qreal)canvas->width()/wOriginal;
                w = canvas->width();
                h = hOriginal*scale;
            }
            else if (pbFit->text() == FIT_FIT_HEIGHT){
                scale = (qreal)canvas->height()/hOriginal;
                w = wOriginal*scale;
                h = canvas->height();
            }
            canvas->scene()->clear();
            QPixmap spix = picture.scaled(w,h,Qt::KeepAspectRatio,Qt::SmoothTransformation);
            QGraphicsPixmapItem * pic = canvas->scene()->addPixmap(spix);
            pic->setPos(0,0);
            canvas->scene()->setSceneRect(0,0,w,h);
        }
        else{
            // Should apply the zoom
            qreal scale = zoomScale/100.0;
            qreal w, h;
            w = wOriginal*scale;
            h = wOriginal*scale;
            canvas->scene()->clear();
            QPixmap spix = picture.scaled(w,h,Qt::KeepAspectRatio,Qt::SmoothTransformation);
            QGraphicsPixmapItem * pic = canvas->scene()->addPixmap(spix);
            pic->setPos(0,0);
            w = qMax((qreal)canvas->width(),w);
            h = qMax((qreal)canvas->height(),h);
            canvas->scene()->setSceneRect(0,0,w,h);
        }
    }
}
