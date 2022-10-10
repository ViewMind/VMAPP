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
    Q_PROPERTY(QImage image READ getImage WRITE setImage)

public:
    QImageDisplay(QQuickItem *parent = nullptr);

    // SET - GET that works as QML Property.
    QImage getImage() const;
    void setImage(const QImage &image);

    // Reimplemented function
    void paint(QPainter *painter) override;

signals:
    //void imageChanged();

private:
    void keyPressEvent(QKeyEvent *event) override;

private:
    QImage displayImage;

};

#endif // QIMAGEDISPLAY_H
