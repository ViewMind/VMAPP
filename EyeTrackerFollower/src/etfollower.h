#ifndef ETFOLLOWER_H
#define ETFOLLOWER_H

#include <QMainWindow>
#include <QApplication>
#include <QDesktopWidget>
#include <QIntValidator>
#include <QGraphicsScene>
#include <QGraphicsEllipseItem>
#include "../../EyeExperimenter/src/EyeTrackerInterface/GazePoint/opengazeinterface.h"
#include "../../CommonClasses/LogInterface/loginterface.h"

namespace Ui {
class ETFollower;
}

class ETFollower : public QMainWindow
{
    Q_OBJECT

public:
    explicit ETFollower(QWidget *parent = 0);
    ~ETFollower();

private slots:
    void on_pbGO_clicked();
    void onNewMessage(const QString &html);
    void onNewDataAvailable(EyeTrackerData data);
    void onEyeTrackerControl(quint8 code);

protected:
    void resizeEvent(QResizeEvent *e);

private:
    Ui::ETFollower *ui;
    OpenGazeInterface *ogi;
    LogInterface logger;
    LogInterface right;
    LogInterface left;

    // Display circles.
    qreal radious;
    QGraphicsEllipseItem *leye;
    QGraphicsEllipseItem *reye;

    // Control and display variables
    qreal lastTimeStamp;
    quint64 freqGlitchCounter;
    qreal lineCounter;
};

#endif // ETFOLLOWER_H
