#include "renderserverclient.h"

////////////////////////////// STATIC FUNCTION FOR CHILD ENUM //////////////////////////

HWND RenderServerClient::renderHandle = nullptr;

BOOL RenderServerClient::EnumChildProc(HWND hwnd, LPARAM lParam){
    Q_UNUSED(lParam)
    renderHandle = hwnd;
    return TRUE; // must return TRUE; If return is FALSE it stops the recursion
}

///////////////////////////////////////////////////////////////////////////////////////

RenderServerClient::RenderServerClient(QObject *parent):QObject(parent)
{
    socket = new QTcpSocket(this);
    connect(socket,&QTcpSocket::connected,this,&RenderServerClient::onConnected);
    connect(socket,&QTcpSocket::stateChanged,this,&RenderServerClient::onStateChanged);
    connect(socket,&QTcpSocket::errorOccurred,this,&RenderServerClient::onErrorOcurred);
    connect(socket,&QTcpSocket::disconnected,this,&RenderServerClient::onDisconnected);
    connect(socket,&QTcpSocket::readyRead,this,&RenderServerClient::onReadyRead);

    connect(&renderServerProcess,&QProcess::errorOccurred,this,&RenderServerClient::onProcessErrorOcurred);
    connect(&renderServerProcess,&QProcess::stateChanged,this,&RenderServerClient::onProcessStateChanged);
    connect(&renderServerProcess,&QProcess::started,this,&RenderServerClient::onProcessStarted);
    connect(&renderServerProcess,&QProcess::finished,this,&RenderServerClient::onProcessFinished);

    connect(&waitTimer,&QTimer::timeout,this,&RenderServerClient::onWaitTimerTimeout);
    connect(&cooldownTimer,&QTimer::timeout,this,&RenderServerClient::onCoolDownTimerTimeout);

    mainWindowID = 0;
    renderHandle = nullptr;

    sentResolutionRequest = false;
    bytesAreBeingSent = false;

    renderWindowHidden = false;

    emittedRenderHandleReady = false;
    isConnectedToRRS = false;

    onClosing = false;

    mtimer.start();

    packetCounter = -1;

}

void RenderServerClient::resizeRenderWindow(qint32 x, qint32 y, qint32 w, qint32 h){
    renderWindowGeometry.setX(x);
    renderWindowGeometry.setY(y);
    renderWindowGeometry.setWidth(w);
    renderWindowGeometry.setHeight(h);
}

void RenderServerClient::hideRenderWindow(){
    MoveWindow(RenderServerClient::renderHandle,renderWindowGeometry.x(),renderWindowGeometry.y(),0,0,TRUE);
}

void RenderServerClient::showRenderWindow() {
    //qDebug() << "Entering Show of RanderWindow with" << renderWindowHidden << renderWindowGeometry;
    if (renderWindowHidden) return;  // Does nothing.
    MoveWindow(RenderServerClient::renderHandle,renderWindowGeometry.x(),renderWindowGeometry.y(),renderWindowGeometry.width(),renderWindowGeometry.height(),TRUE);
}

void RenderServerClient::setRenderWindowHiddenFlag(bool flag){
    renderWindowHidden = flag;
}


void RenderServerClient::startRenderServer(const QString &fullPath, WId mainWinID, bool notaskkill){

    QFileInfo processExecutableInfo(fullPath);
    if (!processExecutableInfo.exists()){
        emit RenderServerClient::newMessage("Cannot find Render Server Executable from path '" + fullPath + "'",MSG_TYPE_ERROR);
        return;
    }

    QStringList arguments;

    if (!notaskkill){

        QString exectutable_name = processExecutableInfo.baseName() + "." + processExecutableInfo.completeSuffix();
        QProcess killprocess;
        killprocess.setProgram("taskkill");

        arguments << "/im";
        arguments << exectutable_name;
        arguments << "/t";
        arguments << "/f";

        killprocess.setArguments(arguments);
        killprocess.start();
        if (!killprocess.waitForFinished()){
            emit RenderServerClient::newMessage("Failed in running task kill on '" + exectutable_name + "'. Code: ",MSG_TYPE_ERROR);
            emit RenderServerClient::newMessage("Task Kill Process standard output: '" + QString(killprocess.readAllStandardOutput()).trimmed() + "'",MSG_TYPE_ERROR);
            emit RenderServerClient::newMessage("Task Kill Process error output: '" + QString(killprocess.readAllStandardError()).trimmed() + "'",MSG_TYPE_ERROR);
        }
        else {
            emit RenderServerClient::newMessage("Successfully ran task kill on '" + exectutable_name + "'. Code: ",MSG_TYPE_INFO);
            emit RenderServerClient::newMessage("Task Kill Process standard output: '" + QString(killprocess.readAllStandardOutput()).trimmed() + "'",MSG_TYPE_INFO);
            emit RenderServerClient::newMessage("Task Kill Process error output: '" + QString(killprocess.readAllStandardError()).trimmed() + "'",MSG_TYPE_INFO);
        }

    }
    else {
        emit RenderServerClient::newMessage("DBUG: Skipping Task Kill of RRS.exe processes",MSG_TYPE_WARNING);
    }


    // WE've tried to kill any pre exisitng RRS processes. From now on it's a matter of checking whethere there is more than one.

    // Setting the working directory.
    renderServerDirectory = processExecutableInfo.absoluteDir().path();
    mainWindowID = mainWinID;
    renderServerProcess.setProgram(fullPath);

    emittedRenderHandleReady = false;

    arguments.clear();
    arguments << "-popupwindow";
    arguments << "-parentHWND";
    arguments << QString::number(mainWindowID);
    renderServerProcess.setArguments(arguments);
    renderServerProcess.setWorkingDirectory(renderServerDirectory);
    renderServerProcess.start();

}

bool RenderServerClient::isRenderServerWorking() const {
    if (mainWindowID == 0) return false;
    if (renderHandle == nullptr) return false;
    if (socket->state() != QTcpSocket::ConnectedState) return false;
    if (renderServerProcess.state() != QProcess::Running) return false;
    return true;
}

bool RenderServerClient::isReadyToRender() const {
    return isConnectedToRRS;
}

void RenderServerClient::connectToRenderServer() {

    // We need to get the prot file first.
    quint16 PORT = 0;

    QString portFile = renderServerDirectory + "/" + PORT_FILE;

    QFile pfile(portFile);
    if (!pfile.exists()) {
        emit RenderServerClient::newMessage("Port file '" + portFile + "' does not exist",MSG_TYPE_ERROR);
        return;
    }

    if (!pfile.open(QFile::ReadOnly)){
        emit RenderServerClient::newMessage("Failed to open port file '" + portFile + "' for reading",MSG_TYPE_ERROR);
        return;
    }

    QTextStream reader(&pfile);
    QString portString = reader.readAll();
    pfile.close();

    bool ok = false;
    PORT = static_cast<quint16> (portString.toUInt(&ok));
    if (!ok){
        emit RenderServerClient::newMessage("Bad content in the port file. Invalid int: '" + portString + "'",MSG_TYPE_ERROR);
        return;
    }

    socket->connectToHost("localhost",PORT);
}

RenderServerPacket RenderServerClient::getPacket() {
    RenderServerPacket p = this->rxPacket;
    this->rxPacket.resetForRX();
    return p;
}

void RenderServerClient::sendPacket(RenderServerPacket packet){


    if (socket->state() != QAbstractSocket::ConnectedState){
        emit RenderServerClient::newMessage("Socket is disconnnected. Clearing message queue",MSG_TYPE_WARNING);
        sendPacketQueue.clear();
        cooldownTimer.stop();
        return;
    }

    this->packetCounter++;
    packet.setCounter(this->packetCounter);

    sendPacketQueue << packet;
    if (!cooldownTimer.isActive()){
        cooldownTimer.start(COOLDOWN_BETWEEN_PACKETS);
    }

}

void RenderServerClient::onCoolDownTimerTimeout() {

    cooldownTimer.stop();

    if (!sendPacketQueue.isEmpty()){
        RenderServerPacket p = sendPacketQueue.takeFirst();
        //qDebug() << "Sending packet " << p.getType() << "at" << mtimer.elapsed() << " Remaining is " << sendPacketQueue.size();
        emit RenderServerClient::newMessage("Sending packet of type '" + p.getType()  + "'. Frame: " + QString::number(p.getCounter()) ,MSG_TYPE_INFO);
        socket->write(p.getByteArrayToSend());
        socket->waitForBytesWritten(3000);
        //qDebug() << "Starting Cooldown timer again";
        cooldownTimer.start(COOLDOWN_BETWEEN_PACKETS);
    }

}

////////////////////////////////////// WAIT TIMER SLOTS ////////////////////////////////////
void RenderServerClient::onWaitTimerTimeout(){

    if (renderHandle == nullptr){
        // We try again until we find it.
        HWND hwnd = (HWND) mainWindowID;
        EnumChildWindows(hwnd, RenderServerClient::EnumChildProc, 0);
        return;
    }

    if (!emittedRenderHandleReady) {
        emittedRenderHandleReady = true;
        emit RenderServerClient::renderWindowHandleReady();
    }


    // If we got the handle, we now connect to the server
    if (socket->state() == QAbstractSocket::UnconnectedState){
        emit RenderServerClient::newMessage("Attempting connection to TCP Server ...",MSG_TYPE_INFO);
        this->connectToRenderServer();
        return;
    }

    if (socket->state() == QAbstractSocket::ConnectedState){
        isConnectedToRRS = true;
        emit RenderServerClient::readyToRender();
    }
    else return;

    waitTimer.stop(); // Once all the conditions have been met the timer can be stopped.

}

////////////////////////////////////// TCP SOCKET SLOTS ////////////////////////////////////

void RenderServerClient::onReadyRead() {

    RenderServerPacket::RXState state =  this->rxPacket.rxBytes(socket->readAll());
    if (state == RenderServerPacket::RX_DONE){
        // We don't want to print frequency update packets.
        this->packetCounter = this->rxPacket.getCounter();
        if (this->rxPacket.getType() != RRS::PacketType::TYPE_FREQ_UPDATE){
            emit RenderServerClient::newMessage("Received packet of type '" + this->rxPacket.getType()  + "'. Frame: " + QString::number(this->packetCounter) ,MSG_TYPE_INFO);
        }
        else {
            if ((this->frequency_update_packet_counter % PRINT_EVERY_N_FUPDATES) == 0){
                // WE print.
                emit RenderServerPacket::newMessage("[FREQUPDATE] " + )
            }
            this->frequency_update_packet_counter++;

        }
        emit RenderServerClient::newPacketArrived();
    }
    else if (state == RenderServerPacket::RX_ERROR){
        emit RenderServerClient::newMessage("Failed in receiving packet. Reason: " + this->rxPacket.getError(),MSG_TYPE_ERROR);
    }

}

void RenderServerClient::onConnected() {
    // waitTimer.stop(); // We can now stop the poll timer.
    emit RenderServerClient::connectionEstablished();
}

void RenderServerClient::onDisconnected(){
    if (onClosing) return;
    emit RenderServerClient::newMessage("Got Disconnected",MSG_TYPE_WARNING);
}

void RenderServerClient::onErrorOcurred(QAbstractSocket::SocketError socketError){
    QMetaEnum metaEnum = QMetaEnum::fromType<QAbstractSocket::SocketError>();
    QString name = metaEnum.valueToKey(socketError);
    name = "ERROR Ocurred: " +  name;
    emit RenderServerClient::newMessage(name,MSG_TYPE_ERROR);
}

void RenderServerClient::onStateChanged(QAbstractSocket::SocketState socketState){
    if (onClosing) return;
    QMetaEnum metaEnum = QMetaEnum::fromType<QAbstractSocket::SocketState>();
    QString name = metaEnum.valueToKey(socketState);
    name = "Socket State Change: " +  name;
    emit RenderServerClient::newMessage(name,MSG_TYPE_WARNING);
}


////////////////////////////////////// PROCESS SLOTS ////////////////////////////////////
void RenderServerClient::onProcessErrorOcurred(QProcess::ProcessError error){
    if (onClosing) return;
    QMetaEnum metaEnum = QMetaEnum::fromType<QProcess::ProcessError>();
    QString name = metaEnum.valueToKey(error);
    name = "Render Server Process Error: " +  name + " from " + renderServerProcess.program();
    emit RenderServerClient::newMessage(name,MSG_TYPE_ERROR);
}

void RenderServerClient::onProcessFinished(int exitCode, QProcess::ExitStatus exitStatus){
    if (onClosing) return;
    if (exitCode != 0){
        QMetaEnum metaEnum = QMetaEnum::fromType<QProcess::ExitStatus>();
        QString name = metaEnum.valueToKey(exitStatus);
        name = "Abnormal Exit Status for Render Server Process: " +  name;
        emit RenderServerClient::newMessage(name,MSG_TYPE_ERROR);
    }
}

void RenderServerClient::onProcessStarted(){
    // Once the process has started we need to wait a "while" and the get the window handle.
    waitTimer.start(POLL_INTERVAL_TO_GET_WINDOW_HANDLE);
}

void RenderServerClient::onProcessStateChanged(QProcess::ProcessState newState){
    if (onClosing) return;
    QMetaEnum metaEnum = QMetaEnum::fromType<QProcess::ProcessState>();
    QString name = metaEnum.valueToKey(newState);
    name = "Render Server Process hast changed state to: " +  name;
    emit RenderServerClient::newMessage(name,MSG_TYPE_WARNING);
}


////////////////////////////////////// Closing Process ////////////////////////////////////
void RenderServerClient::closeRenderServer(){
    onClosing = true;
    socket->disconnectFromHost();
    while (socket->state() != QAbstractSocket::UnconnectedState);
    renderServerProcess.close();
    renderServerProcess.waitForFinished();
}
