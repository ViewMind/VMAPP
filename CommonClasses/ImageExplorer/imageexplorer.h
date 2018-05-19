#ifndef IMAGEEXPLORER_H
#define IMAGEEXPLORER_H

#include <QWidget>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QGraphicsView>
#include <QPushButton>
#include <QFileInfo>
#include <QImage>
#include <QPixmap>
#include <QGraphicsPixmapItem>

class ImageExplorer: public QWidget
{
    Q_OBJECT

public:
    ImageExplorer();

    bool setImageFile(const QString &image);

private slots:
    void on_ZoomIn();
    void on_ZoomOut();

protected:
    void resizeEvent(QResizeEvent *event);

private:
    QGraphicsView *canvas;
    QGraphicsPixmapItem *picture;

};

#endif // IMAGEEXPLORER_H
