#include "control.h"

Control::Control(QObject *parent):QObject(parent)
{    

    QFile file("where_to_search.txt");
    QString location = "";
    if (!file.exists()){
        logger.appendError("Cannot find where_to_search.txt");
    }
    else {
        if (file.open(QFile::ReadOnly)){
            QTextStream reader(&file);
            location = reader.readAll();
            location = location.trimmed();
            file.close();
        }
    }

    renderServer.setPathToPortFile(location + "/selected_port");
    connect(&renderServer,&RenderServerClient::newPacketArrived,this,&Control::onNewPacketArrived);
    connect(&renderServer,&RenderServerClient::connectionEstablished,this,&Control::onConnectionEstablished);
    connect(&renderServer,&RenderServerClient::newMessage,this,&Control::onNewMessage);
    connect(&baseUpdateTimer,&QTimer::timeout,this,&Control::onTimeOut);

    debugDataFrameCounter = 0;

}

void Control::onNewMessage(const QString &msg, const quint8 &msgType){
    if (msgType == RenderServerClient::MSG_TYPE_ERROR){
        logger.appendError(msg);
    }
    else if (msgType == RenderServerClient::MSG_TYPE_INFO){
        logger.appendStandard(msg);
    }
    else if (msgType == RenderServerClient::MSG_TYPE_WARNING){
        logger.appendWarning(msg);
    }
}

void Control::connectToRenderServer(){
    renderServer.connectToRenderServer();
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

    logger.appendStandard("Connection established");

    // Sending the resolution information.
    RenderServerPacket packet;

    packet.setPacketType(RenderServerPacketType::TYPE_REMOTE_TARGET_RESOLUTION);
    packet.setPayloadField(PacketResolution::HEIGHT,displayHeight);
    packet.setPayloadField(PacketResolution::WIDTH,displayWidth);

    renderServer.sendPacket(packet);

    //logger.appendStandard("Sent Eye Data");

    baseUpdateTimer.start(30);

}

void Control::onNewPacketArrived(){

    RenderServerPacket packet = renderServer.getPacket();

    if (packet.isPacketOfType(RenderServerPacketType::TYPE_IMAGE)){

        // This is an image packet. For now we just print the frame number.

        QByteArray imageData = QByteArray::fromBase64(packet.getPayloadField(PacketFrame::IMAGE).toString().toLatin1());
        QString res = packet.getPayloadField(PacketFrame::WIDTH).toString() + "x" + packet.getPayloadField(PacketFrame::HEIGHT).toString();

        QString msg = "FRAME NUMBER " + packet.getPayloadField(PacketFrame::FRAME).toString() + ". IMAGE DATA SIZE " + QString::number(imageData.size()) + ". RES: " + res;
        if (debugDataFrameCounter < 500){
            debugDataFrameCounter++;
            logger.appendStandard(msg);
        }

        //LoadImageDataToImage(imageData,parts.first().toInt(),parts.last().toInt());
        displayImage.loadFromData(imageData,"PNG");

        if (!displayImage.isNull()) emit Control::newImageAvailable();
        else logger.appendWarning("Got Null Image");

    }


}

void Control::LoadImageDataToImage(const QByteArray &imgData, qint32 displayWidth, qint32 displayHeight ){

    QElapsedTimer timer;
    timer.start();

    qDebug() << "TARGET RES" << displayWidth << "x" << displayHeight;

    displayImage = QImage(displayWidth,displayHeight,QImage::Format_RGB888);
    qsizetype expected_size = 3*displayWidth*displayHeight;

    if (expected_size != imgData.size()){
        qDebug() << "Expected image size is of " << expected_size << " and obtained size is of " << imgData.size();
        return;
    }

    QRgb value;
    char R = 0;
    char G = 0;
    char B = 0;
    qint32 channel_counter = 0;

    qint32 pixel_counter,x,y;

    pixel_counter = 0;
    bool pixel_complete = false;

    for (qsizetype i = 0; i < expected_size; i++){

        pixel_complete = false;
        if (channel_counter == 0){
            R = imgData.at(i);
            channel_counter++;
        }
        else if (channel_counter == 1){
            G = imgData.at(i);
            channel_counter++;
        }
        else {
            B = imgData.at(i);
            pixel_complete = true;
            channel_counter = 0;
        }

        if (pixel_complete){

            // Breaking down pixel counter to row (y) and column (x)
            y = pixel_counter/displayWidth;
            x = pixel_counter - displayWidth*y;
            // Now Y must be inverted the lowest line is first in the data.
            y = (displayHeight - y) - 1;

            value = qRgb(R,G,B);
            displayImage.setPixel(x,y,value);
            pixel_counter++;
        }


    }

    qDebug() << "Took" << timer.elapsed();

}

QImage Control::image() const {
    return displayImage;
}

void Control::setTargetInDisplayResolution(qint32 width, qint32 height){
    double div = 4;
    displayWidth = static_cast<qint32>(static_cast<double>(width)/div);
    displayHeight = static_cast<qint32>(static_cast<double>(height)/(div));
}


