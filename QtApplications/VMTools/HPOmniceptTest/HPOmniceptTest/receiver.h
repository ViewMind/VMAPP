#ifndef RECEIVER_H
#define RECEIVER_H

#include <QObject>
#include <QDebug>
#include <iostream>
#include "../../../CommonClasses/EyeTrackingInterface/HPReverb/hpomniceptprovider.h"


class Receiver : public QObject
{
    Q_OBJECT
public:

    struct Averager {
        qreal accumulator = 0;
        qreal count = 0;
        void accumulate(qreal v){accumulator = accumulator + v; count++;}
        qreal getAverage() const { return accumulator/count; }
    };

    explicit Receiver(QObject *parent = nullptr);

    bool done;

signals:

public slots:
    void newEyeData(QVariantMap eyedata);

private:
    Averager period;
    qreal lastTimeStamp;



};

#endif // RECEIVER_H
