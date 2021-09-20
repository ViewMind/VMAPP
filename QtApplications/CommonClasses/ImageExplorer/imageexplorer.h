#ifndef IMAGEEXPLORER_H
#define IMAGEEXPLORER_H

#include <QWidget>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QGraphicsView>
#include <QPushButton>
#include <QToolButton>
#include <QFileInfo>
#include <QImage>
#include <QPixmap>
#include <QGraphicsPixmapItem>
#include <QDebug>
#include <QWheelEvent>
#include <QKeyEvent>
#include <QLabel>
#include <QtMath>
#include <QElapsedTimer>

class ImageExplorer: public QWidget
{
    Q_OBJECT

public:

    static const QString FIT_NOFIT;
    static const QString FIT_FIT_WIDTH;
    static const QString FIT_FIT_HEIGHT;

    ImageExplorer();

    bool setImageFile(const QString &image);
    void setZoomType(const QString &fit);

private slots:
    void on_FitTypeChanged();

protected:
    void resizeEvent(QResizeEvent *event);
    void wheelEvent(QWheelEvent *event);
    void keyPressEvent(QKeyEvent *event);
    void keyReleaseEvent(QKeyEvent *event);

private:

    QGraphicsView *canvas;
    QPushButton *pbFit;
    QPixmap picture;
    qreal wOriginal, hOriginal;
    bool havePixmap;
    bool zoomEnabled;
    qreal zoomScale;
    QLabel *zoomValue;

    // The percente value for each zoom step for each zoom step.
    const qreal ZOOM_STEP = 2;
    const qreal MAX_ZOOM = 200;
    const qreal MIN_ZOOM = 10;

};

#endif // IMAGEEXPLORER_H
