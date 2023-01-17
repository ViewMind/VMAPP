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

    screenResolutionHeight = 0;
    screenResolutionWidth = 0;

    sentResolutionRequest = false;
    bytesAreBeingSent = false;

    renderWindowHidden = false;

    onClosing = false;

    mtimer.start();

}

void RenderServerClient::resizeRenderWindow(qint32 x, qint32 y, qint32 w, qint32 h){
    renderWindowGeometry.setX(x);
    renderWindowGeometry.setY(y);
    renderWindowGeometry.setWidth(w);
    renderWindowGeometry.setHeight(h);
}

void RenderServerClient::hideRenderWindow(){
    MoveWindow(RenderServerClient::renderHandle,renderWindowGeometry.x(),renderWindowGeometry.y(),0,0,TRUE);
    //ShowWindow(RenderServerClient::renderHandle,SW_HIDE);
    //SetWindowPos(RenderServerClient::renderHandle,HWND_BOTTOM,0,0,0,0,SWP_HIDEWINDOW|SWP_NOMOVE|SWP_NOSIZE);
}

void RenderServerClient::showRenderWindow() {
    //qDebug() << "Entering Show of RanderWindow with" << renderWindowHidden << renderWindowGeometry;
    if (renderWindowHidden) return;  // Does nothing.
    //SetWindowPos(RenderServerClient::renderHandle,HWND_BOTTOM,0,0,0,0,SWP_SHOWWINDOW|SWP_NOMOVE|SWP_NOSIZE);
    //SetWindowPos(RenderServerClient::renderHandle,HWND_TOP,0,0,0,0,SWP_SHOWWINDOW|SWP_NOMOVE|SWP_NOSIZE);
    //SetWindowPos(RenderServerClient::renderHandle,(HWND) mainWindowID,0,0,0,0,0);

    //ShowWindow(RenderServerClient::renderHandle,SW_SHOW);
    MoveWindow(RenderServerClient::renderHandle,renderWindowGeometry.x(),renderWindowGeometry.y(),renderWindowGeometry.width(),renderWindowGeometry.height(),TRUE);
}

void RenderServerClient::setRenderWindowHiddenFlag(bool flag){
    renderWindowHidden = flag;
}


QSize RenderServerClient::getRenderResolution() const{
    QSize size;
    size.setWidth(screenResolutionWidth);
    size.setHeight(screenResolutionHeight);
    return size;
}

qreal RenderServerClient::getHorizontalFieldOfView() const {
    return hFOV;
}

qreal RenderServerClient::getVerticalFieldOfView() const {
    return vFOV;
}

void RenderServerClient::startRenderServer(const QString &fullPath, WId mainWinID){

    QFileInfo processExecutableInfo(fullPath);
    if (!processExecutableInfo.exists()){
        emit RenderServerClient::newMessage("Cannot find Render Server Executable from path '" + fullPath + "'",MSG_TYPE_ERROR);
        return;
    }

    // Setting the working directory.
    renderServerDirectory = processExecutableInfo.absoluteDir().path();
    mainWindowID = mainWinID;
    renderServerProcess.setProgram(fullPath);

    emittedRenderHandleReady = false;

    QStringList arguments;
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
    if (screenResolutionHeight == 0) return false;
    if (screenResolutionWidth == 0) return false;
    return true;
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
        emit RenderServerClient::newMessage("Bad content in the por file. Invalid int: '" + portString + "'",MSG_TYPE_ERROR);
        return;
    }

    socket->connectToHost("localhost",PORT);
}

RenderServerPacket RenderServerClient::getPacket() {
    RenderServerPacket p = this->rxPacket;
    this->rxPacket.resetForRX();
    return p;
}

void RenderServerClient::sendPacket(const RenderServerPacket &packet){

//    if (packet.getType() == RenderServerPacketType::TYPE_2D_RENDER){
//        qDebug() << "SENDING PACKET @ " << mtimer.elapsed();
//        mtimer.start();
//        Debug::prettpPrintQVariantMap(packet.getPayload());
//    }

//    qDebug() << "Sending Packet of Type: " << packet.getType();

    if (DBUGBOOL(Debug::Options::SEND_RENDER_2D_PRINT)){
        if (packet.getType() == RenderServerPacketType::TYPE_2D_RENDER){
            qDebug() << "SENDING PACKET" << packet.getStringSummary() << "@ " << mtimer.elapsed();
            mtimer.start();
        }
    }


    if (socket->state() != QAbstractSocket::ConnectedState){
        emit RenderServerClient::newMessage("Socket is disconnnected. Clearing message queue",MSG_TYPE_WARNING);
        sendPacketQueue.clear();
        cooldownTimer.stop();
        return;
    }

    sendPacketQueue << packet;
    //qDebug() << "==Adding packet. " << packet.getType() << " at " << mtimer.elapsed() << " Packet count is" << sendPacketQueue.size();
    if (!cooldownTimer.isActive()){
        //qDebug() << "==Activating timer at " << mtimer.elapsed();
        cooldownTimer.start(COOLDOWN_BETWEEN_PACKETS);
    }

}

void RenderServerClient::onCoolDownTimerTimeout() {

    cooldownTimer.stop();

    if (!sendPacketQueue.isEmpty()){
        RenderServerPacket p = sendPacketQueue.takeFirst();
        //qDebug() << "Sending packet " << p.getType() << "at" << mtimer.elapsed() << " Remaining is " << sendPacketQueue.size();
        socket->write(p.getByteArrayToSend());
        socket->waitForBytesWritten(3000);
        //qDebug() << "Starting Cooldown timer again";
        cooldownTimer.start(COOLDOWN_BETWEEN_PACKETS);
    }

}

void RenderServerClient::sendEnable2DRenderPacket(bool enable){
    if ((screenResolutionHeight == 0) && (screenResolutionWidth == 0)) return; // No point in sending the packet in this situation.
    RenderServerPacket request;
    request.setPacketType(RenderServerPacketType::TYPE_2D_CONTROL);
    request.setPayloadField(RenderControlPacketFields::WIDTH,screenResolutionWidth);
    request.setPayloadField(RenderControlPacketFields::HEIGHT,screenResolutionHeight);
    request.setPayloadField(RenderControlPacketFields::ENABLE_2D_RENDER,enable);
    this->sendPacket(request);
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
        if ((screenResolutionHeight == 0) && (screenResolutionWidth == 0)){
            // If we got here the render handle is effectively set and we are connected. So The next step is to get the render canvas dimensions.
            if (!sentResolutionRequest){
                RenderServerPacket request;
                request.setPacketType(RenderServerPacketType::TYPE_2D_CONTROL);
                request.setPayloadField(RenderControlPacketFields::WIDTH,0);
                request.setPayloadField(RenderControlPacketFields::HEIGHT,0);
                request.setPayloadField(RenderControlPacketFields::ENABLE_2D_RENDER,false);
                this->sendPacket(request);
                sentResolutionRequest = true;
                emit RenderServerClient::newMessage("Sent request for resolution",MSG_TYPE_INFO);
            }
            else return;
        }
    }
    else return;

    waitTimer.stop(); // Once all the conditions have been met the timer can be stopped.

}

////////////////////////////////////// TCP SOCKET SLOTS ////////////////////////////////////

void RenderServerClient::onReadyRead() {

    RenderServerPacket::RXState state =  this->rxPacket.rxBytes(socket->readAll());
    if (state == RenderServerPacket::RX_DONE){
        //qDebug() << "New Packet Arrived";

        // A new packet arrived. However if the packet if of type control adn we are waiting for resolution data, then the signal is NOT emitted.
        if ((screenResolutionHeight == 0) && (screenResolutionWidth == 0)){
            if (this->rxPacket.getType() == RenderServerPacketType::TYPE_2D_CONTROL){

                screenResolutionWidth  = this->rxPacket.getPayloadField(Render2DControlPacketFields::WIDTH).toInt();
                screenResolutionHeight = this->rxPacket.getPayloadField(Render2DControlPacketFields::HEIGHT).toInt();
                hFOV = this->rxPacket.getPayloadField(Render2DControlPacketFields::HFOV).toReal();
                vFOV = this->rxPacket.getPayloadField(Render2DControlPacketFields::VFOV).toReal();
                QString serverVersion = this->rxPacket.getPayloadField(Render2DControlPacketFields::VERSION).toString();

                //qDebug() << "SET WORKING RESOLUTION TO" << screenResolutionHeight << screenResolutionWidth;

                sentResolutionRequest = false;
                this->rxPacket.resetForRX();

                QString message = "Set the working resolution to " + QString::number(screenResolutionWidth) + "x" + QString::number(screenResolutionHeight);
                message = message + ". RenderServerVersion: " + serverVersion;


                emit RenderServerClient::newMessage(message,MSG_TYPE_INFO);
                if ((screenResolutionHeight != 0) && (screenResolutionWidth != 0)) emit RenderServerClient::readyToRender();
                this->rxPacket.resetForRX();

                return;

            }
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
