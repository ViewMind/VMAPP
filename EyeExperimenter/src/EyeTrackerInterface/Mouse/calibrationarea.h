#ifndef CALIBRATIONAREA_H
#define CALIBRATIONAREA_H

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

class CalibrationArea : public QDialog
{
    Q_OBJECT
public:
    explicit CalibrationArea(QWidget *parent = 0);

    // Advances to the next calibration point. Returns true ONLY when the calibration is done.
    bool isInCalibrationPoint(int x, int y);
    void setAutoCalibration(bool autoc) {autoCalibrate = autoc;}

signals:
    void calibrationCanceled();

protected:
    void keyPressEvent(QKeyEvent *event);
    void showEvent(QShowEvent *event);

private:
    QGraphicsEllipseItem *calibrationTarget;
    QGraphicsView *gview;
    QList<QPoint> calibrationPoints;
    qint32 currentPoint;
    qreal R;
    bool autoCalibrate;

    void drawCalibrationPoint();

};

#endif // CALIBRATIONAREA_H
