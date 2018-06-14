#ifndef FOLLOWSCREEN_H
#define FOLLOWSCREEN_H

#include <QDialog>
#include <QDesktopWidget>
#include <QApplication>
#include <QKeyEvent>
#include <QGraphicsScene>
#include <QGraphicsView>
#include <QVBoxLayout>
#include <QGraphicsEllipseItem>
#include <QDebug>
#include <QtMath>
#include "../../EyeExperimenter/src/EyeTrackerInterface/eyetrackerinterface.h"

// Simple class that is shown full screen (single monitor) to just show where the eyes are looking at and the x,y value.

class FollowScreen : public QDialog
{
    Q_OBJECT

public:
    FollowScreen();

public slots:
    void newPosition(EyeTrackerData data);

protected:
    void keyPressEvent(QKeyEvent *e);

private:
    QGraphicsEllipseItem *gaze;
    QGraphicsView *gview;
    QGraphicsTextItem *currentPosition;
    QGraphicsTextItem *updateFreq;
    qreal R;
    qreal lastTime;
};

#endif // FOLLOWSCREEN_H
