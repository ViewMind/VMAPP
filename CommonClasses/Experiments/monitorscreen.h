#ifndef MONITORSCREEN_H
#define MONITORSCREEN_H

#include <QWidget>
#include <QGraphicsView>
#include <QVBoxLayout>
#include <QApplication>
#include <QGraphicsScene>
#include <QGraphicsPixmapItem>

class MonitorScreen : public QWidget
{
    Q_OBJECT
public:
    explicit MonitorScreen(QWidget *parent = nullptr, const QRect &screen = QRect(), qreal SCREEN_W = 1, qreal SCREEN_H = 1);
    void setBackgroundBrush(const QBrush &brush);

signals:

public slots:
    void updateBackground(const QPixmap &image);
    void updateEyePositions(qint32 rx, qint32 ry, qint32 lx, qint32 ly);
    void addFixations(qint32 rx, qint32 ry, qint32 lx, qint32 ly);
    void addMessages(const QString &message, bool append);

private:
    QGraphicsView *gview;    

    // The graphical items
    QGraphicsPixmapItem *background;
    QGraphicsEllipseItem *leftEyeTracker;
    QGraphicsEllipseItem *rightEyeTracker;
    QList<QGraphicsEllipseItem*> rightEyeFixations;
    QList<QGraphicsEllipseItem*> leftEyeFixations;
    QGraphicsSimpleTextItem* messages;
    qreal R;

    // Geomtric constants for scaling and offsetting.
    qreal targetWidth;
    qreal targetHeight;
    qreal scaleX;
    qreal scaleY;

};

#endif // MONITORSCREEN_H
