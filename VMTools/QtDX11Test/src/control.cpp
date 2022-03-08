#include "control.h"

Control::Control(QObject *parent) : QObject(parent)
{
    d3dManager = nullptr;
    connect(&mouseTracker,&DummyDataProvider::newPositionData,this,&Control::newPositionData);
}

void Control::startTest(){

    d3dManager = new D3DManager();
    if (!d3dManager->Initialize()){
        qDebug() << "D3D Intialization errors" << d3dManager->getError();
    }
    unsigned int W,H;
    d3dManager->getScreenDimensions(W,H);
    targetTest.initialize(static_cast<int>(W),static_cast<int>(H));
    qDebug() << W << H;
    if (!d3dManager->LoadQImage(targetTest.renderCurrentPosition(0,0,0,0))){
        qDebug() << "Error Loading QImage: " << d3dManager->getError();
    }
    d3dManager->RenderImage();

    mouseTracker.start();

}

void Control::newPositionData(int rx, int ry, int lx, int ly){
    if (!d3dManager->LoadQImage(targetTest.renderCurrentPosition(rx,ry,lx,ly))){
        qDebug() << "Error Loading QImage: " << d3dManager->getError();
    }
    d3dManager->RenderImage();
}

void Control::stopTest(){

    mouseTracker.stop();

    if (d3dManager){
        d3dManager->Shutdown();
        delete d3dManager;
        d3dManager = nullptr;
    }
    targetTest.finalize();
}

Control::~Control(){
    if (d3dManager){
        d3dManager->Shutdown();
        delete d3dManager;
        d3dManager = nullptr;
    }
}

