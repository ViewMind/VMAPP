#include "etfollower.h"
#include "ui_etfollower.h"

ETFollower::ETFollower(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::ETFollower)
{
    ui->setupUi(this);

    ui->pbCalibrate->setEnabled(false);
    ui->pbFollow->setEnabled(false);

    //iface = new MouseInterface();
    QDesktopWidget *desktop = QApplication::desktop();
    QRect screen = desktop->screenGeometry(desktop->screenNumber());
    iface = new OpenGazeInterface(this,screen.width(),screen.height());
    //iface = new MouseInterface();

    // All is good connecting to the mouse
    connect(iface,SIGNAL(newDataAvailable(EyeTrackerData)),&fscreen,SLOT(newPosition(EyeTrackerData)));
    connect(iface,SIGNAL(eyeTrackeControl(quint8)),this,SLOT(onETControlChanged(quint8)));

    iface->connectToEyeTracker();

}

void ETFollower::onETControlChanged(quint8 code){
    qWarning() << "Got ET change code" << code;
    switch(code){
    case EyeTrackerInterface::ET_CODE_CALIBRATION_ABORTED:
        qWarning() << "Calibration aborted";
        break;
    case EyeTrackerInterface::ET_CODE_CALIBRATION_DONE:
        ui->pbFollow->setEnabled(true);
        qWarning() << "Calibration done";
        break;
    case EyeTrackerInterface::ET_CODE_CALIBRATION_FAILED:
        qWarning() << "Calibration failed";
        break;
    case EyeTrackerInterface::ET_CODE_CONNECTION_FAIL:
        qWarning() << "Connection failed";
        break;
    case EyeTrackerInterface::ET_CODE_CONNECTION_SUCCESS:
        ui->pbCalibrate->setEnabled(true);
        break;
    case EyeTrackerInterface::ET_CODE_DISCONNECTED_FROM_ET:
        qWarning() << "Disconnected from ET";
        break;
    }
}

ETFollower::~ETFollower()
{
    delete ui;
}

void ETFollower::on_pbCalibrate_clicked()
{
    EyeTrackerCalibrationParameters calibrationParams;
    calibrationParams.forceCalibration = true;
    calibrationParams.name = "";
    iface->calibrate(calibrationParams);
}

void ETFollower::on_pbFollow_clicked()
{
    fscreen.show();
    iface->enableUpdating(true);
}
