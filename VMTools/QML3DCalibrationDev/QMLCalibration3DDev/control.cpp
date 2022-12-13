#include "control.h"

HWND Control::renderHandle = nullptr;

Control::Control(QObject *parent):QObject(parent)
{    

    connect(&renderServer,&RenderServerClient::newPacketArrived,this,&Control::onNewPacketArrived);
    connect(&renderServer,&RenderServerClient::connectionEstablished,this,&Control::onConnectionEstablished);
    connect(&renderServer,&RenderServerClient::newMessage,this,&Control::onNewMessage);
    connect(&renderServer,&RenderServerClient::readyToRender,this,&Control::onReadyToRender);

    connect(&baseUpdateTimer,&QTimer::timeout,this,&Control::onTimeOut);
    connect(&requestUpdateTimer,&QTimer::timeout,this,&Control::onRequestTimerUpdate);

    connect(&animator,&AnimationManager::animationUpdated,this,&Control::onUpdateAnimation);
    connect(&animator,&AnimationManager::reachedAnimationStop,this,&Control::onReachedAnimationStop);

}

void Control::onReadyToRender() {

    // Let's create the image for the backend. For that we need the logo.
    RenderServerPacket p;
    p.setPacketType(RenderServerPacketType::TYPE_IMG_SIZE_REQ);
    p.setPayloadField(PacketImgSizeRequest::HEIGHT,0);
    p.setPayloadField(PacketImgSizeRequest::WIDTH,0);
    p.setPayloadField(PacketImgSizeRequest::NAME,"logo");
    renderServer.sendPacket(p);

}

void Control::enablePacketLog(bool enable){
    RenderServerPacket rsp;
    rsp.setPacketType(RenderServerPacketType::TYPE_2D_DBUG_ENABLE);
    rsp.setPayloadField(RenderControlPacketFields::ENABLE_RENDER_P_DBUG,enable);
    renderServer.sendPacket(rsp);
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

    quint32 DPI = GetDpiForWindow((HWND) mainWindowID);

    float dpi_multiplier = static_cast<float>(DPI)/96.0f;

    target_x = static_cast<qint32>( static_cast<float>(target_x)*dpi_multiplier );
    target_y = static_cast<qint32>( static_cast<float>(target_y)*dpi_multiplier );
    target_w = static_cast<qint32>( static_cast<float>(target_w)*dpi_multiplier );
    target_h = static_cast<qint32>( static_cast<float>(target_h)*dpi_multiplier );

    renderServer.resizeRenderWindow(target_x,target_y,target_w,target_h);
    renderServer.showRenderWindow();

}

void Control::onUpdateAnimation(){
    renderServer.sendPacket(animator.getCurrentFrame());
}

void Control::onReachedAnimationStop(const QString &variable, qint32 animationStopIndex, qint32 animationSignalType){    

    RenderServerItem *item = animator.getItemByID(messageID);
    RenderServerTextItem *text = static_cast<RenderServerTextItem*>(item);

    QString type = RenderServerItem::AnimationSignalType2String(animationSignalType);
    qDebug() << "Animation Stop for variable" << variable << " and stop index of " << animationStopIndex << ". Signal Type" << type << "(" << mtimer.elapsed() << ")";
    mtimer.start();

    text->setText("Got " + variable + "@" + QString::number(animationStopIndex) + ". Type: " + type);
    QRectF brect = text->boundingRect();

    // We need to correct the x so as to center the text.
    QSize screen = renderServer.getRenderResolution();
    qreal x = (screen.width() - brect.width())/2;
    qreal y = text->y();
    text->setPos(x,y);

    renderServer.sendPacket(animator.getCurrentFrame());
}

void Control::onTimeOut(){

    QSize size = renderServer.getRenderResolution();
    QRandomGenerator gen(static_cast<quint32>(QTime::currentTime().msec()));
    qreal x = gen.bounded(size.width());
    qreal y = gen.bounded(size.height());
    qreal R = size.width()*0.01;

    RenderServerScene pic;
    pic.copyFrom(updateImage);
    RenderServerCircleItem * eyepos = pic.addEllipse(x-R,y-R,2*R,2*R,QPen(),QBrush(QColor("#080000")));
    eyepos->setDisplayOnly(true);

    renderServer.sendPacket(pic.render());

}

void Control::onConnectionEstablished(){

    StaticThreadLogger::log("Control::onConnectionEstablished","Connection established");

    RenderServerPacket packet;
    packet.setPacketType(RenderServerPacketType::TYPE_LOG_LOCATION);
    packet.setPayloadField(PacketLogLocation::LOG_LOCATION,StaticThreadLogger::getFullLogFilePath());
    packet.setPayloadField(PacketLogLocation::APP_NAME,"3DCalibTest");
    renderServer.sendPacket(packet);

    onNewMessage("Sent log relocation packet",RenderServerClient::MSG_TYPE_INFO);

    emit Control::requestWindowGeometry();


}

void Control::onNewPacketArrived(){
    RenderServerPacket packet = renderServer.getPacket();

    // We are expecting the packet for setting the backend.
    if (packet.getType() == RenderServerPacketType::TYPE_IMG_SIZE_REQ){
        qreal w = packet.getPayloadField(PacketImgSizeRequest::WIDTH).toReal();
        qreal h = packet.getPayloadField(PacketImgSizeRequest::HEIGHT).toReal();
        if ((w < 1) || (h < 1)){
            qDebug() << "Got BAD image size" << w << h << " for image " <<  packet.getPayloadField(PacketImgSizeRequest::NAME).toString();
        }
        else {
            qDebug() << "Image size is " << w << "x" << h;
            setBackgroundImage(w,h);
        }
    }

    else {
        // This is part of the of the Simulated 3D Control Study.
        qDebug() << "Got Packet of Type" << packet.getType();
    }
}


void Control::setBackgroundImage(qreal w, qreal h){

    RenderServerScene bg;
    bg.setBackgroundBrush(QBrush(Qt::gray));

    QSize screen = renderServer.getRenderResolution();
    bg.setSceneRect(0,0,screen.width(),screen.height());

    qreal tw = 0.7*screen.width();

    RenderServerImageItem *img = bg.addImage("logo",true,w,h,tw);
    img->setPos(0.15*screen.width(),0.3*screen.height());

    QFont font;
    font.setPointSizeF(80);
    font.setWeight(QFont::Normal);
    RenderServerTextItem *text = bg.addText("Ready To Calibrate",font);
    messageID = text->getItemID();
    text->setBrush(QBrush(QColor("#2A3990")));
    text->setAlignment(QString(text->ALIGN_CENTER));
    QRectF br = text->boundingRect();
    QRectF imgbr = img->boundingRect();

    //qDebug() << "Image bounding rect" << imgbr;

    qreal x = (screen.width() - br.width())/2;
    qreal y = imgbr.top() + imgbr.height() + 50;
    text->setPos(x,y);

    updateImage.copyFrom(bg);

    //this->enablePacketLog(true);
    renderServer.sendEnable2DRenderPacket(true);
    renderServer.sendPacket(bg.render());

}


void Control::appClose(){
    renderServer.closeRenderServer();
}

bool Control::checkRenderServerStatus(){
    return renderServer.isRenderServerWorking();
}


void Control::onRequestTimerUpdate() {

    //requestUpdateTimer.stop()

    // Send the packet.
    RenderServerPacket p;
    p.setPacketType(RenderServerPacketType::TYPE_3DSTUDY_CONTROL);
    p.setPayloadField(Packet3DStudyControl::COMMAND,Study3DControlCommands::CMD_EYEPOS_AND_SYNCH);
    QVariantList el, er;
    el << 0.2 << 0.3 << 0.93;
    er << 0.93 << 0.2 << 0.3;
    p.setPayloadField(Packet3DStudyControl::EYE_LEFT,el);
    p.setPayloadField(Packet3DStudyControl::EYE_RIGHT,el);
    p.setPayloadField(Packet3DStudyControl::TIMESTAMP,mtimer.elapsed());
    renderServer.sendPacket(p);

}

