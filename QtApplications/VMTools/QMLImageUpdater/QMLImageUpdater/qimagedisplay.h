#ifndef QIMAGEDISPLAY_H
#define QIMAGEDISPLAY_H

#include <QQuickPaintedItem>
#include <QImage>
#include <QPainter>
#include <QDebug>
#include <QKeyEvent>

class QImageDisplay : public QQuickPaintedItem
{
    Q_OBJECT
    //Q_PROPERTY(QImage image READ image WRITE setImage NOTIFY imageChanged)
    Q_PROPERTY(QImage image WRITE setImage)
public:
    QImageDisplay(QQuickItem *parent = nullptr);

    // SET - GET that works as QML Property.
    //QImage image() const;
    void setImage(const QImage &image);

    // Reimplemented function
    void paint(QPainter *painter);

signals:
    //void imageChanged();

private:
    void keyPressEvent(QKeyEvent *event);

private:
    QImage displayImage;

};

#endif // QIMAGEDISPLAY_H
