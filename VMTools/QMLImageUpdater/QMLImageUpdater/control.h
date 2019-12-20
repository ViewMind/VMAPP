#ifndef CONTROL_H
#define CONTROL_H

#include <QObject>
#include <QGraphicsScene>
#include <QGraphicsTextItem>
#include <QPainter>
#include <QDebug>

class Control : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QImage image READ image NOTIFY newImageAvailable)
public:
    explicit Control(QObject *parent = nullptr);

    QImage image() const;
    //void setImage(const QImage &image);
    Q_INVOKABLE void goToNextImage();

signals:
    void newImageAvailable();

public slots:

private:
    QGraphicsScene *canvas;
    QImage currentImage;
    qint32 counter;

};

#endif // CONTROL_H
