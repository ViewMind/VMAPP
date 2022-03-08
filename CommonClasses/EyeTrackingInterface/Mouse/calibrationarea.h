#ifndef CALIBRATIONAREA_H
#define CALIBRATIONAREA_H

#include <QDialog>
//#include <QApplication>
#include <QGuiApplication>
#include <QKeyEvent>
#include <QGraphicsScene>
#include <QGraphicsView>
#include <QVBoxLayout>
#include <QGraphicsEllipseItem>
#include <QGraphicsPixmapItem>
#include <QPixmap>
#include <QDebug>
#include <QtMath>

class CalibrationArea : public QDialog
{
    Q_OBJECT
public:
    explicit CalibrationArea(QWidget *parent = nullptr);

    // Advances to the next calibration point. Returns true ONLY when the calibration is done.
    //bool isInCalibrationPoint(int x, int y);
    //void setAutoCalibration(bool autoc) {autoCalibrate = autoc;}

    void setCurrentImage(const QImage &toshow);

signals:
    void calibrationCanceled();

protected:
    void keyPressEvent(QKeyEvent *event) override;
    //void showEvent(QShowEvent *event) override;

private:
    QGraphicsView *gview;
//    QGraphicsEllipseItem *calibrationTarget;
//    QList<QPoint> calibrationPoints;
//    qint32 currentPoint;
//    qreal R;
//    bool autoCalibrate;

    //void drawCalibrationPoint();

};

#endif // CALIBRATIONAREA_H
