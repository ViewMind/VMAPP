#include "receiver.h"

Receiver::Receiver(QObject *parent) : QObject(parent)
{
    lastTimeStamp = 0;
    done = false;
}


void Receiver::newEyeData(QVariantMap eyedata){

    if (done) return;
    qreal timestamp = eyedata.value(HPProvider::Timestamp).toReal();

    qreal xr        = eyedata.value(HPProvider::RightEye).toMap().value(HPProvider::Eye::X).toReal();
    qreal yr        = eyedata.value(HPProvider::RightEye).toMap().value(HPProvider::Eye::Y).toReal();
    qreal zr        = eyedata.value(HPProvider::RightEye).toMap().value(HPProvider::Eye::Z).toReal();
    qreal pr        = eyedata.value(HPProvider::RightEye).toMap().value(HPProvider::Eye::Pupil).toReal();

    qreal xl        = eyedata.value(HPProvider::LeftEye).toMap().value(HPProvider::Eye::X).toReal();
    qreal yl        = eyedata.value(HPProvider::LeftEye).toMap().value(HPProvider::Eye::Y).toReal();
    qreal zl        = eyedata.value(HPProvider::LeftEye).toMap().value(HPProvider::Eye::Z).toReal();
    qreal pl        = eyedata.value(HPProvider::LeftEye).toMap().value(HPProvider::Eye::Pupil).toReal();

    qreal diff = timestamp - lastTimeStamp;
    QString printDiff = QString::number(diff);
    if (lastTimeStamp > 0){
       printDiff = printDiff + "*";
       period.accumulate(diff);
    }

    std::cout << "@" << timestamp << " (" << printDiff.toStdString() << ") "
              << ". R: (" << xr << "," << yr << "," << zr << "), " << pr
              << ". L: (" << xl << "," << yl << "," << zl << "), " << pl << std::endl;


    if (period.count > 1000){
        std::cout << "Average period: " << period.getAverage() << "ms. AVG Freq: " << 1000.0/period.getAverage() << std::endl;
        done = true;
    }

    lastTimeStamp = timestamp;
}
