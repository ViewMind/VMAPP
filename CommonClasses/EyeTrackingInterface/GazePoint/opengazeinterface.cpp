#include "opengazeinterface.h"

OpenGazeInterface::OpenGazeInterface(QObject *parent, qreal width, qreal height):EyeTrackerInterface(parent,width,height)
{
    socket = new QTcpSocket(this);

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
    //qWarning() << "CREATING GP3HD";
}

void OpenGazeInterface::on_calibrationAborted(){
    eventDetecter->hide();
    logger.appendWarning("OpenGaze ET: Calibration aborted");
    emit(eyeTrackerControl(ET_CODE_CALIBRATION_ABORTED));
}

QString OpenGazeInterface::getCalibrationValidationReport() const {
    return "No Calibration Report Implemented For Open Gaze Interface";
}

//***************************************** Socket related functions.******************************************************
void OpenGazeInterface::on_connected(){
    logger.appendSuccess("CONNECTED to OpenGaze EyeTracker Server");
    emit(eyeTrackerControl(ET_CODE_CONNECTION_SUCCESS));

    isConnecting = false;

    // Configuring the ET
    QStringList cmds2Enable;
    cmds2Enable << GPC_ENABLE_SEND_POG_LEFT << GPC_ENABLE_SEND_POG_RIGHT << GPC_ENABLE_SEND_PUPILMM << GPC_ENABLE_SEND_TIME;
    for (qint32 i = 0; i < cmds2Enable.size(); i++){
        OpenGazeCommand cmd;
        cmd.setEnableCommand(cmds2Enable.at(i),true);
        socket->write(cmd.prepareCommandToSend());
    }
}

void OpenGazeInterface::on_socketError(QAbstractSocket::SocketError error){
    QMetaEnum metaEnum = QMetaEnum::fromType<QAbstractSocket::SocketError>();
    logger.appendError(QString("OPEN GAZE SOCKET ERROR: ") +  metaEnum.valueToKey(error));
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
    //qWarning() << "SHOULD DISCONNECT" << shouldDisconnect;
    if (!shouldDisconnect) {
        logger.appendError("DISCONNECTED from OpenGaze EyeTracker Server");
        emit(eyeTrackerControl(ET_CODE_DISCONNECTED_FROM_ET));
    }
}

//*************************** Processing commands received by the ET ************************************************
void OpenGazeInterface::processReceivedCommand(const OpenGazeCommand &cmd){
    /// FOR DEBUGGING Will print the command and all its fields to the console.
    //cmd.prettyPrint();

    if (cmd.getCommandType() == GPCT_REC){

        // This is data point from the eytracker
        EyeTrackerData data;

        // Make it MS to ensure that it works with the all data processign capabilities.
        data.timeUnit = EyeTrackerData::TU_MS;
        bool send = false;        

        // Condition for left eye
        if ((cmd.getField(GPF_LPOGV) == "1") && canUseLeft()){
            data.xLeft = qRound(cmd.getField(GPF_LPOGX).toDouble()*screenWidth);
            data.yLeft = qRound(cmd.getField(GPF_LPOGY).toDouble()*screenHeight);
            send = true;
            if (cmd.getField(GPF_LPMMV) == "1"){
                qreal pupil = cmd.getField(GPF_LPMM).toDouble();
                data.pdLeft = pupil;
            }
            else{
                data.pdLeft = 0;
            }
        }
        else{
            data.xLeft = 0;
            data.yLeft = 0;
            data.pdLeft = 0;
        }

        // Condition for right eye
        if ((cmd.getField(GPF_RPOGV) == "1") && canUseRight()){
            data.xRight = qRound(cmd.getField(GPF_RPOGX).toDouble()*screenWidth);
            data.yRight = qRound(cmd.getField(GPF_RPOGY).toDouble()*screenHeight);
            send = true;
            if (cmd.getField(GPF_RPMMV) == "1"){
                qreal pupil = cmd.getField(GPF_RPMM).toDouble();
                data.pdRight = pupil;
            }
            else{
                data.pdRight = 0;
            }
        }
        else {
            data.xRight = 0;
            data.yRight = 0;
            data.pdRight = 0;
        }

        // Send only if there was any valid data.
        if (send){
            data.time = static_cast<qint32>(cmd.getField(GPF_TIME).toDouble()*1000); // (Transforming seconds into ms);
            //qWarning() << "Original time stamp of" << cmd.getField(GPF_TIME).toDouble() << "timedata" << data.time;
            lastData = data;
            emit(newDataAvailable(data));
        }

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


            //qWarning() << "Total" << total  << "LEFT VALID" << leftValid << "RIGHT VALID" << rightValid
            //           << "Can use left: " << canUseLeft() << "Can use right: " << canUseRight() << "Eye To Transmit" << eyeToTransmit;

            if (canUseLeft() && (total != leftValid)){
                logger.appendError("Gazepoint ET: Calbration failed due to poor calibration results for left eye: " + QString::number(leftValid) + " out of " + QString::number(total));
                sendOk = false;
            }

            if (canUseRight() && (total != rightValid)){
                logger.appendError("Gazepoint ET: Calbration failed due to poor calibration results for right eye: " + QString::number(rightValid) + " out of " + QString::number(total));
                sendOk = false;
            }

            // Do clean up and send the correct signal.
            eventDetecter->hide();
            OpenGazeCommand closeWindow;
            closeWindow.setEnableCommand(GPC_CALIBRATE_SHOW,false);
            socket->write(closeWindow.prepareCommandToSend());
            //qWarning() << "SENDING THE CALIBRATION FINISH COMMAND" << sendOk;
            emit(eyeTrackerControl(ET_CODE_CALIBRATION_DONE));

        }
    }
}
//*************************** Eye Interface Functions ************************************************
void OpenGazeInterface::calibrate(EyeTrackerCalibrationParameters params){
    Q_UNUSED(params)
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
