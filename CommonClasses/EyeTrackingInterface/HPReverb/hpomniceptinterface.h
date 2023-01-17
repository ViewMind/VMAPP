#ifndef HPOMNICEPTINTERFACE_H
#define HPOMNICEPTINTERFACE_H

#include "hpomniceptprovider.h"
#include "../eyetrackerinterface.h"

#include <QTimer>
#include <QElapsedTimer>

class HPOmniceptInterface: public EyeTrackerInterface
{
public:

    explicit HPOmniceptInterface(QObject *parent = nullptr);

    void connectToEyeTracker() override;

    void enableUpdating(bool enable) override;

    void disconnectFromEyeTracker() override;


private slots:

    void newEyeData(QVariantMap eyedata);

private:

    // Object that will manage all headset data gathering.
    HPOmniceptProvider hpprovider;

    // The sampling frequency of this headset.
    static const float SAMPLING_FREQ;

    // Processing the data as it comes. 2D Only.
    void newEyeData2D(QVariantMap eyedata);

};

#endif // HPOMNICEPTINTERFACE_H
