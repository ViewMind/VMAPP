#ifndef MONITORSCREEN_H
#define MONITORSCREEN_H

#include <QWidget>
#include <QGraphicsView>
#include <QDesktopWidget>
#include <QVBoxLayout>
#include <QApplication>
#include <QGraphicsScene>
#include "../../CommonClasses/common.h"
#include "../../CommonClasses/Experiments/readingmanager.h"
#include "../../CommonClasses/Experiments/fieldingmanager.h"
#include "../../CommonClasses/Experiments/bindingmanager.h"


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

private:
    QGraphicsView *gview;    
    ReadingManager reading;
    BindingManager binding;
    FieldingManager fielding;

    // The graphical items
    QGraphicsPixmapItem *background;
    QGraphicsEllipseItem *leftEyeTracker;
    QGraphicsEllipseItem *rightEyeTracker;
    qreal R;

    // Geomtric constants for scaling and offsetting.
    qreal targetWidth;
    qreal targetHeight;
    qreal scaleX;
    qreal scaleY
    ;

};

#endif // MONITORSCREEN_H
