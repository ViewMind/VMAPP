#ifndef HPOMNICEPTPROVIDER_H
#define HPOMNICEPTPROVIDER_H

#include <QObject>
#include <QString>
#include <QDebug>
#include <QMap>
#include <QVariant>
#include "./hp/omnicept/lib-abi-cpp/LicensingModel.h"
#include "./hp/omnicept/lib-abi-cpp/SessionLicense.h"
#include "./hp/omnicept/lib-abi-cpp/HandshakeError.h"
#include "./hp/omnicept/lib-abi-cpp/TransportError.h"
#include "./hp/omnicept/lib-abi-cpp/ProtocolError.h"
#include "./hp/omnicept/lib-abi-cpp/Subscription.h"
#include "./hp/omnicept/lib-abi-cpp/SubscriptionList.h"
#include "./hp/omnicept/lib-abi-cpp/EyeTracking.h"
#include "./hp/omnicept/lib-client-cpp/Client.h"
#include "./hp/omnicept/Glia.h"

#include <QThread>
#include <QElapsedTimer>
#include <mutex>
#include <iostream>


// Convenience namespace for the map containing all relevant raw eyetracking data.
namespace HPProvider {

   static const QString LeftEye   = "Left Eye";
   static const QString RightEye  = "Right Eye";
   static const QString Timestamp = "TimeStamp";

   namespace Eye {

      static const QString X = "x";
      static const QString Y = "y";
      static const QString Z = "z";
      static const QString Pupil = "Pupil";

   }

}

class HPOmniceptProvider: public QThread
{
    Q_OBJECT

public:
    HPOmniceptProvider();

    // QThread function for the actual thread. It will run the client util it is stopped. After than it will destroy the client and connect must be called again.
    void run() override;

    // Creates the HPClient and leaves it ready for running.
    bool connectToHPRuntime();

    // Stops the client.
    void stopProvider();

    // Disconnectsf from client.
    void disconnectFromHPRuntime();

    // Error message generated if any.
    QString getError() const;

signals:
    void eyeDataAvailable(QVariantMap eyedata);

private:
    // Variables required specifically to make the Omnicept EyeTracker work according to examples.
    std::unique_ptr<HP::Omnicept::Client> omniceptClient;
    HP::Omnicept::Client::StateCallback_T stateCallback;
    bool isConnectedToOmnicept;

    // Error message if there was one.
    QString error;

    // Used to compute the time stamp of the data.
    QElapsedTimer baselineTimer;

    // Indicates when the data gathering should stop.
    bool stopFlag;

    // Map that will contain all new raw data variables. Must be static due to the callback necessitating being static.
    static QVariantMap newEyeData;
    // Mutex to ensure atomic access to newEyeData from this thread and the main thread which will contain the newEyeDataAvailable function
    static std::mutex mtx;
    // The function to register as a callback to the HPOmiceptClient. Since it is a callback, it needs to be static.
    static void newEyeDataAvailable(std::shared_ptr<HP::Omnicept::Abi::EyeTracking> data);

    // Used for determining which raw data to keep.
    static const float VALID_CONFIDENCE_VALUE;

};

#endif // HPOMNICEPTPROVIDER_H
