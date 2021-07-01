#include "etfollower.h"
#include "ui_etfollower.h"

ETFollower::ETFollower(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::ETFollower)
{
    ui->setupUi(this);

    logger.setGraphicalLogInterface();
    right.setGraphicalLogInterface();
    left.setGraphicalLogInterface();

    QIntValidator *vMin = new QIntValidator();
    QIntValidator *vMax = new QIntValidator();
    vMin->setBottom(0);
    vMax->setBottom(0);
    vMax->setTop(20);
    vMin->setTop(20);
    ui->leMin->setValidator(vMin);
    ui->leMax->setValidator(vMax);

    QIntValidator *vLines = new QIntValidator();
    vLines->setBottom(10);
    vLines->setTop(100000);
    ui->leMaxLines->setValidator(vLines);

    connect(&logger,SIGNAL(newUiMessage(QString)),this,SLOT(onNewMessage(QString)));
    connect(&right,SIGNAL(newUiMessage(QString)),ui->pteRight,SLOT(appendHtml(QString)));
    connect(&left,SIGNAL(newUiMessage(QString)),ui->pteLeft,SLOT(appendHtml(QString)));

    QDesktopWidget * dw = QApplication::desktop();
    logger.appendStandard("SCREENS:");
    int selectedScreen = 0;
    int W, H;
    for (qint32 i = 0; i < dw->screenCount(); i++){
        QRect r = dw->screenGeometry(i);
        QString message = "Screen " + QString::number(i) + ": " + QString::number(r.width()) + "x" + QString::number(r.height());
        if (i == selectedScreen) {
            logger.appendSuccess(message);
            W = r.width();
            H = r.height();
        }
        else logger.appendStandard(message);
    }


    ui->graphicsView->setScene(new QGraphicsScene(0,0,W,H,this));
    ui->graphicsView->scene()->setBackgroundBrush(QBrush(Qt::gray));

    ogi = new OpenGazeInterface(this,W,H);
    connect(ogi,SIGNAL(eyeTrackerControl(quint8)),this,SLOT(onEyeTrackerControl(quint8)));
    connect(ogi,SIGNAL(newDataAvailable(EyeTrackerData)),this,SLOT(onNewDataAvailable(EyeTrackerData)));
}

ETFollower::~ETFollower()
{
    delete ui;
}

void ETFollower::onNewMessage(const QString &html){
    ui->pteLog->appendHtml(html);
}

void ETFollower::resizeEvent(QResizeEvent *e){
    Q_UNUSED(e)
    ui->graphicsView->fitInView(ui->graphicsView->sceneRect(),Qt::KeepAspectRatio);
}

void ETFollower::onNewDataAvailable(EyeTrackerData data){

    qreal freq = 0;
    bool freqGlitch = false;
    qreal timediff = 0;
    if (lastTimeStamp > 0){
        timediff = data.time - lastTimeStamp;
        freqGlitch = ((timediff < ui->leMin->text().toDouble() ) || (timediff > ui->leMax->text().toDouble()));
        lineCounter++;
        // Assuming allways milliseconds.
        freq = 1000.0/timediff;
    }

    if (freqGlitch){
        freqGlitchCounter++;
        left.appendError("@" + QString::number(data.time) + ": " + QString::number(data.xLeft) + ", " + QString::number(data.yLeft));
        right.appendError("@" + QString::number(data.time) + ": " + QString::number(data.xRight) + ", " + QString::number(data.yRight));
        logger.appendError("Freq Glitch: " + QString::number(timediff));
    }
    else{
        left.appendStandard("@" + QString::number(data.time) + ": " + QString::number(data.xLeft) + ", " + QString::number(data.yLeft));
        right.appendStandard("@" + QString::number(data.time) + ": " + QString::number(data.xRight) + ", " + QString::number(data.yRight));
    }

    lastTimeStamp = data.time;

    QString html = "<html><head/><body><p><span style=' font-size:12pt; font-weight:600;'>Freq:</span><span style=' font-size:12pt;'>" + QString::number(freq,'f',2)
            + " Hz. </span><span style=' font-size:12pt; font-weight:600;'>Freq Glitches:</span><span style=' font-size:12pt;'>" + QString::number(freqGlitchCounter)
            + "</span></p></body></html>";
    ui->labStatus->setTextFormat(Qt::RichText);
    ui->labStatus->setText(html);

    if (lineCounter >= ui->leMaxLines->text().toInt()){
        lineCounter = 0;
        ui->pteLeft->clear();
        ui->pteRight->clear();
    }

    // Updating the position of the where we are looking
    reye->setPos(data.xRight-radious,data.yRight-radious);
    leye->setPos(data.xLeft-radious,data.yLeft-radious);

}

void ETFollower::onEyeTrackerControl(quint8 code){
    logger.appendWarning("EYE TRACKER CONTROL CODE: " + QString::number(code));
    if (code == EyeTrackerInterface::ET_CODE_CONNECTION_SUCCESS){
        EyeTrackerCalibrationParameters calibrationParams;
        calibrationParams.forceCalibration = true;
        calibrationParams.name = "";
        ogi->calibrate(EyeTrackerCalibrationParameters());
    }
    else if (code == EyeTrackerInterface::ET_CODE_CALIBRATION_DONE){
        logger.appendSuccess("STARTING ET");

        lastTimeStamp = -1;
        freqGlitchCounter = 0;
        lineCounter = 0;

        radious = ui->graphicsView->scene()->width()*0.007;
        leye = ui->graphicsView->scene()->addEllipse(0,0,2*radious,2*radious,QPen(),QBrush(QColor(0,0,255,100)));
        reye = ui->graphicsView->scene()->addEllipse(0,0,2*radious,2*radious,QPen(),QBrush(QColor(0,255,0,100)));

        ogi->setEyeToTransmit(2);
        ogi->enableUpdating(true);
    }
}

void ETFollower::on_pbGO_clicked()
{
    ogi->connectToEyeTracker();
}
