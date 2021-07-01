#include "hpomniceptprovider.h"

const float HPOmniceptProvider::VALID_CONFIDENCE_VALUE = 0.2f; // All values wil be need to be above this value in order to be kept.

QVariantMap HPOmniceptProvider::newEyeData = QVariantMap();

std::mutex HPOmniceptProvider::mtx;

HPOmniceptProvider::HPOmniceptProvider()
{
    isConnectedToOmnicept = false;
    error = "";
    stopFlag = false;
}

QString HPOmniceptProvider::getError() const{
    return error;
}

bool HPOmniceptProvider::connectToHPRuntime(){
    // Creating the empty license.
    HP::Omnicept::Abi::LicensingModel requestedLicense = HP::Omnicept::Abi::LicensingModel::CORE;
    std::unique_ptr<HP::Omnicept::Abi::SessionLicense> sessionLicense = nullptr;
    std::string clientId = "";
    std::string accessKey = "";
    sessionLicense = std::make_unique<HP::Omnicept::Abi::SessionLicense>(clientId, accessKey, requestedLicense, true);

    // This Creates the state callback. Seems to be a function that is called whenver the state is changed?
    stateCallback = [&](const HP::Omnicept::Client::State state)
    {
        if (state == HP::Omnicept::Client::State::RUNNING || state == HP::Omnicept::Client::State::PAUSED)
        {
            isConnectedToOmnicept = true;
        }
        else if (state == HP::Omnicept::Client::State::DISCONNECTED)
        {
            //std::cout << "Omnicept Client Disconnected." << std::endl;
            isConnectedToOmnicept = false;
            return;
        }
    };

    // Start client Build.
    std::string clientName = "OmniceptViewMindClient";
    std::unique_ptr<HP::Omnicept::Glia::AsyncClientBuilder> clientBuilder;

    try
    {
        clientBuilder = HP::Omnicept::Glia::StartBuildClient_Async(clientName, std::move(sessionLicense), stateCallback);
        sessionLicense.reset();
    }
    catch (const std::invalid_argument& e)
    {
        error = "Failed to start asynchronously connecting to Omnicept. invalid_argument: " + QString::fromStdString(e.what());
        return false;
    }

    // Finalizing Client Build
    std::string buildError = "";
    try
    {
        omniceptClient = clientBuilder->getBuildClientResultOrThrow();
        clientBuilder.reset();
    }
    catch (const HP::Omnicept::Abi::HandshakeError& e)
    {
        buildError = "Could not connect to runtime HandshakeError : " + std::string{ e.what() };
    }
    catch (const HP::Omnicept::Abi::TransportError& e)
    {
        buildError = "Could not connect to runtime TransportError : " + std::string{ e.what() };
    }
    catch (const HP::Omnicept::Abi::ProtocolError& e)
    {
        buildError = "Could not connect to runtime ProtocolError : " + std::string{ e.what() };
    }
    catch (const std::invalid_argument& e)
    {
        buildError = "Could not connect to runtime invalid_argument : " + std::string{ e.what() };
    }
    catch (const std::logic_error& e)
    {
        buildError = "Could not connect to runtime logic_error : " + std::string{ e.what() };
    }

    if (buildError != ""){
        error = "Could not build Omnicept Client. Error was: " +  QString::fromStdString(buildError);
        return false;
    }

    // Generating the susbscription list to the information of interest.
    std::shared_ptr<HP::Omnicept::Abi::SubscriptionList> subList = HP::Omnicept::Abi::SubscriptionList::GetSubscriptionListToNone();
    HP::Omnicept::Abi::Subscription eyeTrackingSub = HP::Omnicept::Abi::Subscription::generateSubscriptionForDomainType<HP::Omnicept::Abi::EyeTracking>();
    subList->getSubscriptions().push_back(eyeTrackingSub);

    omniceptClient->setSubscriptions(*subList);
    omniceptClient->startClient();

    // Registering the callback.
    omniceptClient->registerCallback<HP::Omnicept::Abi::EyeTracking>(&HPOmniceptProvider::newEyeDataAvailable);

    // We want the client to remain idle updating becomes enabled.
    //qDebug() << "Pausing Client After Starting";
    omniceptClient->pauseClient();
    return true;
}


void HPOmniceptProvider::stopProvider(){
    stopFlag = true;
}

void HPOmniceptProvider::disconnectFromHPRuntime(){
    omniceptClient->disconnectClient();
    omniceptClient.reset();
    isConnectedToOmnicept = false;
}

void HPOmniceptProvider::run(){

    if (!isConnectedToOmnicept){
        error = "Attempting to run the client withouth having done the connection first";
    }

    baselineTimer.start();
    omniceptClient->startClient();
    stopFlag = false;

    while (!stopFlag){
        mtx.lock();
        if (!newEyeData.empty()){
            newEyeData[HPProvider::Timestamp] = baselineTimer.elapsed();
            emit(eyeDataAvailable(newEyeData));
            newEyeData.clear();
        }
        mtx.unlock();
    }

    omniceptClient->pauseClient();
}

void HPOmniceptProvider::newEyeDataAvailable(std::shared_ptr<HP::Omnicept::Abi::EyeTracking> data){

    // Wait for the buffer to be ready.
    mtx.lock();

    //std::cout << data->toString() << std::endl;
    bool isRightValid = false;
    bool isLeftValid = false;

    QVariantMap l;
    QVariantMap r;

    if (data->rightGazeConfidence < VALID_CONFIDENCE_VALUE){
        r[HPProvider::Eye::Pupil] = 0;
        r[HPProvider::Eye::X]     = 0;
        r[HPProvider::Eye::Y]     = 0;
        r[HPProvider::Eye::Z]     = 0;
    }
    else{
        if (data->rightEyeOpenness > 0.9f){
            r[HPProvider::Eye::Pupil] = data->rightPupilDilation;
        }
        else {
            r[HPProvider::Eye::Pupil] = 0;
        }
        r[HPProvider::Eye::X]     = data->rightGaze.x;
        r[HPProvider::Eye::Y]     = data->rightGaze.y;
        r[HPProvider::Eye::Z]     = data->rightGaze.z;
        isRightValid = true;
    }

    if (data->leftGazeConfidence < VALID_CONFIDENCE_VALUE){
        l[HPProvider::Eye::Pupil] = 0;
        l[HPProvider::Eye::X]     = 0;
        l[HPProvider::Eye::Y]     = 0;
        l[HPProvider::Eye::Z]     = 0;
    }
    else{
        if (data->leftEyeOpenness > 0.9f){
            l[HPProvider::Eye::Pupil] = data->leftPupilDilation;
        }
        else {
            l[HPProvider::Eye::Pupil] = 0;
        }
        l[HPProvider::Eye::X] = data->leftGaze.x;
        l[HPProvider::Eye::Y] = data->leftGaze.y;
        l[HPProvider::Eye::Z] = data->leftGaze.z;
        isLeftValid = true;
    }

    if ((isRightValid || isLeftValid)){
        newEyeData[HPProvider::LeftEye] = l;
        newEyeData[HPProvider::RightEye] = r;
    }
    mtx.unlock();
}
