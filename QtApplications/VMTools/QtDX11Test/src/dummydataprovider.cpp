#include "dummydataprovider.h"

DummyDataProvider::DummyDataProvider()
{
    connect(&sampleTimer,&QTimer::timeout,this,&DummyDataProvider::on_timeOut);
}

void DummyDataProvider::start(){
    sampleTimer.setInterval(static_cast<int>(1000.0/120.0));
    //sampleTimer.setInterval(500);
    sampleTimer.start();
}

void DummyDataProvider::stop(){
    sampleTimer.stop();
}

void DummyDataProvider::on_timeOut(){
    QScreen *screen0 = QApplication::screens().at(0);
    // Position on first screen.
    QPoint pos0 = QCursor::pos(screen0);
    if ((pos0.x() >= 0) && (pos0.y() >= 0)){
        emit(newPositionData(pos0.x(),pos0.y(),pos0.x(),pos0.y()));
    }
}
