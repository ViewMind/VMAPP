#include "control.h"

HWND Control::renderHandle = nullptr;

Control::Control(QObject *parent):QObject(parent)
{    

    connect(&renderServer,&RenderServerClient::newPacketArrived,this,&Control::onNewPacketArrived);
    connect(&renderServer,&RenderServerClient::connectionEstablished,this,&Control::onConnectionEstablished);
    connect(&renderServer,&RenderServerClient::newMessage,this,&Control::onNewMessage);
    connect(&baseUpdateTimer,&QTimer::timeout,this,&Control::onTimeOut);

}

void Control::onNewMessage(const QString &msg, const quint8 &msgType){
    if (msgType == RenderServerClient::MSG_TYPE_ERROR){
        StaticThreadLogger::error("Control::onNewMessage",msg);
    }
    else if (msgType == RenderServerClient::MSG_TYPE_INFO){
        StaticThreadLogger::log("Control::onNewMessage",msg);
    }
    else if (msgType == RenderServerClient::MSG_TYPE_WARNING){
        StaticThreadLogger::warning("Control::onNewMessage",msg);
    }
}

void Control::setWindowID(WId winID){

    QFile file("where_to_search.txt");
    QString location = "";
    if (!file.exists()){
        StaticThreadLogger::error("Control::setWindowID","Cannot find where_to_search.txt");
    }
    else {
        if (file.open(QFile::ReadOnly)){
            QTextStream reader(&file);
            location = reader.readAll();
            location = location.trimmed();
            file.close();
        }
    }

    mainWindowID = winID;

    // This is the path to the executable, so we can now start the server.
    renderServer.startRenderServer(location,winID);

}

void Control::hideRenderWindow(){
    renderServer.resizeRenderWindow(0,0,0,0);
}

void Control::showRenderWindow(){
    emit Control::requestWindowGeometry();
}

void Control::setRenderWindowGeometry(int target_x, int target_y, int target_w, int target_h){

//    RECT window_rectangle, client_rectangle;
//    GetWindowRect((HWND) mainWindowID, &window_rectangle);
//    GetClientRect((HWND) mainWindowID, &client_rectangle);
    quint32 DPI = GetDpiForWindow((HWND) mainWindowID);

//    qint32 wh = window_rectangle.bottom - window_rectangle.top;
//    qint32 ww = window_rectangle.right - window_rectangle.left;
//    qint32 ch = client_rectangle.bottom - client_rectangle.top;
//    qint32 cw = client_rectangle.right - client_rectangle.left;
//    qint32 dW = ww - cw;
//    qint32 dH = wh - ch;
//    if ((dW < 0) || (dH < 0)) return;

//    float dpi_multiplier = 1;
//    switch(DPI){
//       case 120:
//        dpi_multiplier = 1.25;
//        break;
//    case 144:
//        dpi_multiplier = 1.5;
//        break;
//    case 192:
//        dpi_multiplier = 2.0;
//        break;
//    default:
//        dpi_multiplier = 1.0;
//        break;
//    }

    float dpi_multiplier = static_cast<float>(DPI)/96.0f;

    target_x = static_cast<qint32>( static_cast<float>(target_x)*dpi_multiplier );
    target_y = static_cast<qint32>( static_cast<float>(target_y)*dpi_multiplier );
    target_w = static_cast<qint32>( static_cast<float>(target_w)*dpi_multiplier );
    target_h = static_cast<qint32>( static_cast<float>(target_h)*dpi_multiplier );

//    qDebug() << "Window Rectangle: (" << window_rectangle.left << "," << window_rectangle.top << ")" << ww << "x" << wh;
//    qDebug() << "Client Rectangle: (" << client_rectangle.left << "," << client_rectangle.top << ")" << cw << "x" << ch;
//    qDebug() << "Win - Client. H:" << dH << ". W" << dW << ". Target Coordinates (" << target_x << "," << target_y << ")";
//    qDebug() << "DPI IS NOW" << DPI;

//    qDebug() << "Setting inner window to (" << target_x << "," << target_y << ")" << target_w << "x" << target_h;
//    qDebug() << "============================";

//    renderServer.resizeRenderWindow(x,y,w,h);
    renderServer.resizeRenderWindow(target_x,target_y,target_w,target_h);

}


void Control::onTimeOut(){

    // Regularly send eye tracking data.
    RenderServerPacket packet;
    QVariantMap right, left;
    right[PacketEyeTrackingData::EyeData::X] = 1;
    right[PacketEyeTrackingData::EyeData::Y] = 2;

    left[PacketEyeTrackingData::EyeData::X] = 4;
    left[PacketEyeTrackingData::EyeData::Y] = 5;

    packet.setPacketType(RenderServerPacketType::TYPE_CURRENT_EYE_POSITION);
    packet.setPayloadField(PacketEyeTrackingData::LEFT,left);
    packet.setPayloadField(PacketEyeTrackingData::RIGHT,right);

    renderServer.sendPacket(packet);

}

void Control::onConnectionEstablished(){

    StaticThreadLogger::log("Control::onConnectionEstablished","Connection established");

    RenderServerPacket packet;
    packet.setPacketType(RenderServerPacketType::TYPE_LOG_LOCATION);
    packet.setPayloadField(PacketLogLocation::LOG_LOCATION,StaticThreadLogger::getFullLogFilePath());
    packet.setPayloadField(PacketLogLocation::APP_NAME,"RenderServer");
    renderServer.sendPacket(packet);

    emit Control::requestWindowGeometry();

    // baseUpdateTimer.start(30);

}

void Control::onNewPacketArrived(){

    RenderServerPacket packet = renderServer.getPacket();

    //    if (packet.isPacketOfType(RenderServerPacketType::TYPE_IMAGE)){

    //        // This is an image packet. For now we just print the frame number.

    //        QByteArray imageData = QByteArray::fromBase64(packet.getPayloadField(PacketFrame::IMAGE).toString().toLatin1());
    //        QString res = packet.getPayloadField(PacketFrame::WIDTH).toString() + "x" + packet.getPayloadField(PacketFrame::HEIGHT).toString();

    //        QString msg = "FRAME NUMBER " + packet.getPayloadField(PacketFrame::FRAME).toString() + ". IMAGE DATA SIZE " + QString::number(imageData.size()) + ". RES: " + res;
    //        if (debugDataFrameCounter < 500){
    //            debugDataFrameCounter++;
    //            logger.appendStandard(msg);
    //        }

    //    }


}


//void Control::runUnityRenderServer(){

//    QString filename = "C:/Users/ViewMind/Documents/UnityProjects/RemoteRenderServerComponents/00FinalBuild/DEV.exe";
//    filename = "C:/Users/ViewMind/Documents/VMApp3D/GoNoGoSpheres.exe";

//    QFileInfo info(filename);
//    QString dir = info.absoluteDir().path();

//    qDebug() << "Setting working dir to" << dir;

//    renderServerProcess.setProgram(filename);
//    QStringList arguments;
//    arguments << "-popupwindow";
//    arguments << "-parentHWND";
//    arguments << QString::number(mainWindowID);
//    renderServerProcess.setArguments(arguments);
//    renderServerProcess.start();
//    HWND hwnd = (HWND) mainWindowID;
//    if (!renderServerProcess.waitForStarted()){
//        qDebug() << "Wait for started returned false";
//    }
//    // qDebug() << "PID" << renderServerProcess.processId();

////    PROCESS_INFORMATION procInfo = {0};
////    STARTUPINFO startupInfo = {0};
////    startupInfo.cb = sizeof(startupInfo);
////    startupInfo.dwFlags = STARTF_USESTDHANDLES;
////    startupInfo.hStdInput = GetStdHandle(STD_INPUT_HANDLE);
////    startupInfo.hStdOutput = GetStdHandle(STD_OUTPUT_HANDLE);
////    startupInfo.hStdError = GetStdHandle(STD_ERROR_HANDLE);

////    QString _args = QString(" -popupwindow -parentHWND %1").arg(mainWindowID);

////    HWND hwnd = (HWND) mainWindowID;
////    LPWSTR cmd = (LPWSTR) filename.utf16();
////    LPWSTR args = (LPWSTR) _args.utf16();
////    LPWSTR wdir  = (LPWSTR) dir.utf16();

////    BOOL bProcessStarted = CreateProcess(cmd,args , NULL, NULL, TRUE, CREATE_NO_WINDOW,
////                                         NULL, wdir, &startupInfo, &procInfo);

////    if(!bProcessStarted){
////        qDebug() << "There was an error starting the process";
////    }

////    QTime dieTime= QTime::currentTime().addSecs(3);
////    while (QTime::currentTime() < dieTime);

//    //
//    EnumChildWindows(hwnd, Control::EnumChildProc, 0);
//}

