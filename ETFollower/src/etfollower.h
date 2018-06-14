#ifndef ETFOLLOWER_H
#define ETFOLLOWER_H

#include <QMainWindow>
#include "followscreen.h"

#include "../../EyeExperimenter/src/EyeTrackerInterface/Mouse/mouseinterface.h"
#include "../../EyeExperimenter/src/EyeTrackerInterface/GazePoint/opengazeinterface.h"

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
    // Buttons
    void on_pbCalibrate_clicked();
    void on_pbFollow_clicked();

    // ET Control signals
    void onETControlChanged(quint8 code);

private:
    Ui::ETFollower *ui;
    EyeTrackerInterface *iface;
    FollowScreen fscreen;
};

#endif // ETFOLLOWER_H
