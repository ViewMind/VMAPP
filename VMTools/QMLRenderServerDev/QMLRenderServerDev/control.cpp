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

void Control::startRenderingStudy() {

   QSize size = renderServer.getRenderResolution();

#ifdef NBACK
   QFile file("C:/Users/ViewMind/Documents/VMAPP/CommonClasses/Experiments/nbackfamiliy/descriptions/fielding.dat");
#endif

#ifdef BINDING
   QFile file("C:/Users/ViewMind/Documents/VMAPP/CommonClasses/Experiments/binding/descriptions/bc_3.dat");
#endif

#ifdef GONOGO
   QFile file("C:/Users/ViewMind/Documents/VMAPP/CommonClasses/Experiments/gonogo/descriptions/go_no_go.dat");
#endif

   file.open(QFile::ReadOnly);
   QTextStream reader(&file);
   QString content = reader.readAll();
   file.close();

   QVariantMap cnf;
   cnf[NBackManager::CONFIG_IS_VR_BEING_USED] = true;
   cnf[NBackManager::CONFIG_IS_VS] = true;
   cnf[NBackManager::CONFIG_PAUSE_TEXT_LANG] = NBackManager::LANG_EN;

   cnf[BindingManager::CONFIG_IS_BC] = true;
   cnf[BindingManager::CONFIG_N_TARGETS] = 3;


#ifdef GONOGO
   gonogo.configure(cnf);
   gonogo.init(size.width(),size.height());
   if (!gonogo.parseExpConfiguration(content)){
       qDebug() << "Failed in parsing the study description";
       return;
   }
   else {
       qDebug() << "Study description parsed";
   }
#endif

#ifdef BINDING
   binding.configure(cnf);
   binding.init(size.width(),size.height());
   if (!binding.parseExpConfiguration(content)){
       qDebug() << "Failed in parsing the study description";
       return;
   }
   else {
       qDebug() << "Study description parsed";
   }
#endif


#ifdef NBACK
   nback.configure(cnf);
   nback.init(size.width(),size.height());
   if (!nback.parseExpConfiguration(content)){
       qDebug() << "Failed in parsing the study description";
       return;
   }
   else {
       qDebug() << "Study description parsed";
   }
#endif

   expScreen = 0;

#ifdef NBACK
   nback.renderStudyExplanationScreen(0);
   updateImage = nback.getImage();
#endif

#ifdef BINDING
   binding.renderStudyExplanationScreen(0);
   updateImage = binding.getImage();
#endif

#ifdef GONOGO
   gonogo.renderStudyExplanationScreen(0);
   updateImage = gonogo.getImage().render();
#endif

   //renderServer.sendEnable2DRenderPacket(true);
   renderServer.sendPacket(updateImage.render());

   //fastTimer.start(30);

}

void Control::nextStudyExplanation() {
    expScreen++;

#ifdef NBACK
    expScreen = (expScreen % nback.getNumberOfStudyExplanationScreens());
    nback.renderStudyExplanationScreen(expScreen);
    updateImage = nback.getImage();
#endif

#ifdef BINDING
    expScreen = (expScreen % binding.getNumberOfStudyExplanationScreens());
    binding.renderStudyExplanationScreen(expScreen);
    updateImage = binding.getImage();
#endif

#ifdef GONOGO
    expScreen = (expScreen % gonogo.getNumberOfStudyExplanationScreens());
    gonogo.renderStudyExplanationScreen(expScreen);
    updateImage = gonogo.getImage();
#endif

    renderServer.sendPacket(updateImage.render());

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

void Control::forceConnect(){
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

    // This is the path to the executable, so we can now start the server.
    renderServer.startRenderServer(location,mainWindowID);
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
    packet.setPayloadField(PacketLogLocation::APP_NAME,"RenderServer");
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
        simulatedStudyNewPacket(packet);
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
    RenderServerTextItem *text = bg.addText("Hello World",font);
    messageID = text->getItemID();
    text->setBrush(QBrush(QColor("#2A3990")));
    text->setAlignment(QString(text->ALIGN_CENTER));
    QRectF br = text->boundingRect();
    QRectF imgbr = img->boundingRect();

    //qDebug() << "Image bounding rect" << imgbr;

    qreal x = (screen.width() - br.width())/2;
    qreal y = imgbr.top() + imgbr.height() + 50;
    text->setPos(x,y);

    QList<QPointF> circleAnimationCenters;

    circleAnimationCenters << QPointF(0.5*screen.width(),0.5*screen.height());
    circleAnimationCenters << QPointF(0.25*screen.width(),0.25*screen.height());
    circleAnimationCenters << QPointF(0.75*screen.width(),0.25*screen.height());
    circleAnimationCenters << QPointF(0.5*screen.width(),0.5*screen.height());
    circleAnimationCenters << QPointF(0.25*screen.width(),0.75*screen.height());
    circleAnimationCenters << QPointF(0.75*screen.width(),0.75*screen.height());

    qreal smallR = 0.01*screen.width();
    qreal bigR = 0.02*screen.width();

    QColor color(Qt::red);
    color.setAlpha(127);
    RenderServerCircleItem * circle = bg.addEllipse(0,0,2*bigR,2*bigR,QPen(),QBrush(color));
    movingCircleID = circle->getItemID();

    // We now set up the animation.
    animator.setScene(bg);

    if (!animator.addVariableStop(movingCircleID,RenderControlPacketFields::RADIOUS,smallR,800,false)){
        qDebug() << "Failed to adding big R stop" << animator.getError();
        return;
    }
    if (!animator.addVariableStop(movingCircleID,RenderControlPacketFields::RADIOUS,bigR,800,false)){
        qDebug() << "Failed to adding big R stop" << animator.getError();
        return;
    }

    qint32 delay = 3000;
    qint32 anim  = 3000;

    for (qint32 i = 0; i < circleAnimationCenters.size(); i++){
        QPointF p = circleAnimationCenters.at(i);
        QString xname = RenderControlPacketFields::X + ".0";
        QString yname = RenderControlPacketFields::Y + ".0";
        if (!animator.addVariableStop(movingCircleID,xname,p.x(),anim,true,delay)){
            qDebug() << "Could not add variable" << movingCircleID << xname << ". Reason:" << animator.getError();
            return;
        }
        if (!animator.addVariableStop(movingCircleID,yname,p.y(),anim,false,delay)){
            qDebug() << "Could not add variable" << movingCircleID << yname << ". Reason:" << animator.getError();
            return;
        }
    }

    updateImage.copyFrom(bg);

    //this->enablePacketLog(true);
    renderServer.sendEnable2DRenderPacket(true);

    //animator.startAnimation();
    //mtimer.start();

}


void Control::packetBurst() {

    for (qint32 i = 0; i < 20; i++){
        RenderServerPacket p;
        p.setPacketType(RenderServerPacketType::TYPE_LOG_MESSAGE);
        p.setPayloadField(PacketNumberedLog::NUMBER,i);
        p.setPayloadField(PacketNumberedLog::MESSAGE,"This is log number " + QString::number(i));
        renderServer.sendPacket(p);
    }

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

void Control::simulatedStudyNewPacket(RenderServerPacket p){

    if (p.getType() == RenderServerPacketType::TYPE_STUDY_DESCRIPTION){
        // We log the description.
        QString description = p.getPayloadField(PacketStudyDescription::STUDY_DESC).toString();        

        // We need to decode de description.
        QJsonParseError json_error;
        QJsonDocument doc = QJsonDocument::fromJson(description.toLatin1(),&json_error);
        if (doc.isNull()){
            StaticThreadLogger::error("Control::simulatedStudyNewPacket","Failed to decode JSON String for description Reason: " + json_error.errorString());
            return;
        }

        // If parsing went well we need the two obligatory fields.
        QVariantMap map = doc.object().toVariantMap();
        p.setPayloadField(PacketStudyDescription::STUDY_DESC,map);

        StaticThreadLogger::log("Control::simulatedStudyNewPacket","Received new study description. Packet is\n" + Debug::QVariantMapToString(p.getPayload()));
        //StaticThreadLogger::log("Control::simulatedStudyNewPacket","DESCRIPTION:\n" + Debug::QVariantMapToString(map));

        studyData.insert(p.getPayload());

        //Debug::prettpPrintQVariantMap(studyData);

    }
    else if (p.getType() == RenderServerPacketType::TYPE_3DSTUDY_CONTROL){
        // It should have messages.
        StaticThreadLogger::log("Control::simulatedStudyNewPacket","Received study control packet\n" + Debug::QVariantMapToString(p.getPayload()));
    }
    else if (p.getType() == RenderServerPacketType::TYPE_STUDY_DATA){
        StaticThreadLogger::log("Control::simulatedStudyNewPacket","End of Study, Study DAta" + Debug::QVariantMapToString(p.getPayload()));
        studyData.insert(p.getPayload());
        requestUpdateTimer.stop();

        qDebug() << "Saving";
        //Debug::prettpPrintQVariantMap(studyData);

        QString study_string = Debug::QVariantMapToString(studyData);
        QFile file("study_data.json");
        file.open(QFile::WriteOnly);
        QTextStream writer(&file);
        writer << study_string;
        file.close();


    }
    else {
        StaticThreadLogger::warning("Control::simulatedStudyNewPacket","Received packet of unknown type: " + p.getType());
    }

}

void Control::sendStudy3DControl(qint32 selected_option, bool checkedState){

    if (selected_option == 0){ // Request packet description.
        studyData.clear();
        RenderServerPacket cmd;
        cmd.setPacketType(RenderServerPacketType::TYPE_3DSTUDY_CONTROL);
        cmd.setPayloadField(Packet3DStudyControl::COMMAND,Study3DControlCommands::CMD_REQUEST_STUDY_DESCRIPTION);
        cmd.setPayloadField(Packet3DStudyControl::COMMAND_ARG,Study3DNames::GO_NO_GO_SPHERE);
        cmd.setPayloadField(Packet3DStudyControl::SHORT_STUDIES,checkedState);
        renderServer.sendPacket(cmd); // Now we should receive a study description.
    }
    else if (selected_option == 1){
        RenderServerPacket cmd;
        cmd.setPacketType(RenderServerPacketType::TYPE_3DSTUDY_CONTROL);
        cmd.setPayloadField(Packet3DStudyControl::COMMAND,Study3DControlCommands::CMD_START_HAND_CALIBRATION);
        renderServer.sendPacket(cmd); // Now we should receive a study description.
    }
    else if (selected_option == 2){
        RenderServerPacket cmd;
        cmd.setPacketType(RenderServerPacketType::TYPE_3DSTUDY_CONTROL);
        cmd.setPayloadField(Packet3DStudyControl::COMMAND,Study3DControlCommands::CMD_NEXT_EXPLANATION);
        renderServer.sendPacket(cmd); // Now we should receive a study description.
    }
    else if (selected_option == 3){
        RenderServerPacket cmd;
        cmd.setPacketType(RenderServerPacketType::TYPE_3DSTUDY_CONTROL);
        cmd.setPayloadField(Packet3DStudyControl::COMMAND,Study3DControlCommands::CMD_PREVIOUS_EXPLANATION);
        renderServer.sendPacket(cmd); // Now we should receive a study description.
    }
    else if (selected_option == 4){
        RenderServerPacket cmd;
        cmd.setPacketType(RenderServerPacketType::TYPE_3DSTUDY_CONTROL);
        cmd.setPayloadField(Packet3DStudyControl::COMMAND,Study3DControlCommands::CMD_START_EXAMPLES);
        renderServer.sendPacket(cmd); // Now we should receive a study description.
    }
    else if (selected_option == 5){
        RenderServerPacket cmd;
        cmd.setPacketType(RenderServerPacketType::TYPE_3DSTUDY_CONTROL);
        cmd.setPayloadField(Packet3DStudyControl::COMMAND,Study3DControlCommands::CMD_NEXT_EXAMPLES);
        renderServer.sendPacket(cmd); // Now we should receive a study description.
    }
    else if (selected_option == 6){
        RenderServerPacket cmd;
        mtimer.start();
        cmd.setPacketType(RenderServerPacketType::TYPE_3DSTUDY_CONTROL);
        cmd.setPayloadField(Packet3DStudyControl::COMMAND,Study3DControlCommands::CMD_STUDY_START);
        cmd.setPayloadField(Packet3DStudyControl::TIMESTAMP,mtimer.elapsed());
        renderServer.sendPacket(cmd); // Now we should receive a study description.
        requestUpdateTimer.start(25); // 40 FPS is about every 25 ms.        
    }
    else if (selected_option == 7){
        RenderServerPacket cmd;
        cmd.setPacketType(RenderServerPacketType::TYPE_3DSTUDY_CONTROL);
        cmd.setPayloadField(Packet3DStudyControl::COMMAND,Study3DControlCommands::CMD_END_HAND_CALIBRATION);
        renderServer.sendPacket(cmd);
    }

}
