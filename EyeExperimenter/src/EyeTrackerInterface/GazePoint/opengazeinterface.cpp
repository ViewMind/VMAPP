#include "opengazeinterface.h"

OpenGazeInterface::OpenGazeInterface(QObject *parent, qreal width, qreal height):EyeTrackerInterface(parent,width,height)
{
    socket = new QTcpSocket(this);

    // No eye data set.
    eyeDataToUse = -1;

    // Doing the connections
    connect(socket,SIGNAL(connected()),this,SLOT(on_connected()));
    connect(socket,SIGNAL(error(QAbstractSocket::SocketError)),this,SLOT(on_socketError(QAbstractSocket::SocketError)));
    connect(socket,SIGNAL(readyRead()),this,SLOT(on_readyRead()));
    connect(socket,SIGNAL(disconnected()),this,SLOT(on_disconnected()));

    // The event detecter
    eventDetecter = new EventDetection();
    eventDetecter->configure(0,0,width,height,"Click to end calibration");

    // Doing a connection
    connect(eventDetecter,SIGNAL(buttonPressed()),this,SLOT(on_calibrationAborted()));

    shouldDisconnect = false;
    isConnecting = false;
    buffer = "";

    // Calibration data
}

void OpenGazeInterface::on_calibrationAborted(){
    eventDetecter->hide();
    logger.appendWarning("OpenGaze ET: Calibration aborted");
    emit(eyeTrackeControl(ET_CODE_CALIBRATION_ABORTED));
}

//***************************************** Socket related functions.******************************************************
void OpenGazeInterface::on_connected(){
    logger.appendSuccess("CONNECTED to OpenGaze EyeTracker Server");
    emit(eyeTrackeControl(ET_CODE_CONNECTION_SUCCESS));

    // Configuring the ET
    QStringList cmds2Enable;
    cmds2Enable << GPC_ENABLE_SEND_POG_LEFT << GPC_ENABLE_SEND_POG_RIGHT << GPC_ENABLE_SEND_PUPIL_LEFT << GPC_ENABLE_SEND_PUPIL_RIGHT << GPC_ENABLE_SEND_TIME;
    for (qint32 i = 0; i < cmds2Enable.size(); i++){
        OpenGazeCommand cmd;
        cmd.setEnableCommand(cmds2Enable.at(i),true);
        socket->write(cmd.prepareCommandToSend());
    }
}

void OpenGazeInterface::on_socketError(QAbstractSocket::SocketError error){
    QMetaEnum metaEnum = QMetaEnum::fromType<QAbstractSocket::SocketError>();
    logger.appendError(QString("SOCKET ERROR: ") +  metaEnum.valueToKey(error));
    if (isConnecting) {
        isConnecting = false;
        logger.appendError("Calibration failed due to last error");
        emit(eyeTrackeControl(ET_CODE_CALIBRATION_FAILED));
    }
}

void OpenGazeInterface::on_readyRead(){
    QByteArray bytes = socket->readAll();

    for (qint32 i = 0; i < bytes.size(); i++){
        buffer = buffer + bytes.at(i);
        //qWarning() << buffer;
        if (buffer.endsWith("/>")){
            OpenGazeCommand cmd;
            QString err = cmd.fromString(buffer);
            if (err.isEmpty()){
                processReceivedCommand(cmd);
            }
            else{
                logger.appendError("Buffering error: " + err);
            }
            buffer = "";
        }
    }
}

void OpenGazeInterface::on_disconnected(){
    logger.appendWarning("DISCONNECTED from OpenGaze EyeTracker Server");
    if (!shouldDisconnect) emit(eyeTrackeControl(ET_CODE_DISCONNECTED_FROM_ET));
}

//*************************** Processing commands received by the ET ************************************************
void OpenGazeInterface::processReceivedCommand(const OpenGazeCommand &cmd){
    /// FOR DEBUGGING Will print the command and all its fields to the console.
    //cmd.prettyPrint();

    if (cmd.getCommandType() == GPCT_REC){
        EyeTrackerData data;
        if ((cmd.getField(GPF_LPOGV) == "1") && canUseLeft()){
            data.xLeft = qRound(cmd.getField(GPF_LPOGX).toDouble()*screenWidth);
            data.yLeft = qRound(cmd.getField(GPF_LPOGY).toDouble()*screenHeight);
        }
        if ((cmd.getField(GPF_RPOGV) == "1") && canUseRight()){
            data.xRight = qRound(cmd.getField(GPF_RPOGX).toDouble()*screenWidth);
            data.yRight = qRound(cmd.getField(GPF_RPOGY).toDouble()*screenHeight);
        }
        if (cmd.getField(GPF_LPV) == "1"){
            qreal pupil = cmd.getField(GPF_LPD).toDouble();
            qreal scale = cmd.getField(GPF_LPS).toDouble();
            pupil = pupil/scale;
            data.pdLeft = pupil;
        }
        if (cmd.getField(GPF_RPV) == "1"){
            qreal pupil = cmd.getField(GPF_RPD).toDouble();
            qreal scale = cmd.getField(GPF_RPS).toDouble();
            pupil = pupil/scale;
            data.pdRight = pupil;
        }
        data.time = cmd.getField(GPF_TIME).toDouble()*1000000; // (Transforming seconds into microseconds);
        emit(newDataAvailable(data));
    }
    else if (cmd.getCommandType() == GPCT_CAL){
        if (cmd.getID() == GPC_CALIB_RESULT){
            // This means that the calibration is done, checking on the results.
            qreal leftValid = 0;
            qreal rightValid = 0;
            qreal total = 0;
            bool sendOk = true;

            for (qint32 i = 1; i <= 9; i++){
                QString lv = cmd.getField("LV"+QString::number(i));
                QString rv = cmd.getField("RV"+QString::number(i));

                if (lv.isEmpty() || rv.isEmpty()) break;
                total++;
                if (lv == "1") leftValid++;
                if (rv == "1") rightValid++;
            }

            if ((leftValid/total) >= CALIBRATION_THRESHOLD){
                if ((rightValid/total) >= CALIBRATION_THRESHOLD){
                    //Use both.
                    eyeDataToUse = USE_BOTH_EYES;
                }
                else {
                    logger.appendWarning("Gazepoint ET: Ignoring data for right eye due to poor calbration results");
                    eyeDataToUse = USE_LEFT_EYE;
                }
            }
            else if ((rightValid/total) >= CALIBRATION_THRESHOLD){
                logger.appendWarning("Gazepoint ET: Ignoring data for left eye due to poor calbration results");
                eyeDataToUse = USE_RIGHT_EYE;
            }
            else{
                // Could not calibrate
                logger.appendError("Gazepoint ET: Calbration failed due to poor calibration results for both eyes");
                sendOk = false;
                emit(eyeTrackeControl(ET_CODE_CALIBRATION_FAILED));
            }

            // Something when right.
            eventDetecter->hide();
            OpenGazeCommand closeWindow;
            closeWindow.setEnableCommand(GPC_CALIBRATE_SHOW,false);
            socket->write(closeWindow.prepareCommandToSend());
            if (sendOk) emit(eyeTrackeControl(ET_CODE_CALIBRATION_DONE));
        }
    }
}
//*************************** Eye Interface Functions ************************************************
void OpenGazeInterface::calibrate(EyeTrackerCalibrationParameters params){
    Q_UNUSED(params);
    OpenGazeCommand cmd;
    cmd.setEnableCommand(GPC_CALIBRATE_SHOW,true);
    socket->write(cmd.prepareCommandToSend());
    cmd.setEnableCommand(GPC_CALIBRATE_START,true);
    socket->write(cmd.prepareCommandToSend());
    eventDetecter->showMaximized();
}

void OpenGazeInterface::connectToEyeTracker(){
    socket->connectToHost(TCP_SERVER_ADDRESS,TCP_SERVER_PORT);
    isConnecting = true;
}

void OpenGazeInterface::enableUpdating(bool enable){
    OpenGazeCommand cmd;
    cmd.setEnableCommand(GPC_ENABLE_SEND_DATA,enable);
    socket->write(cmd.prepareCommandToSend());
}

void OpenGazeInterface::disconnectFromEyeTracker(){
    shouldDisconnect = true;
    socket->disconnectFromHost();
}

OpenGazeInterface::~OpenGazeInterface(){
    delete eventDetecter;
    delete socket;
}
