#include "viveeyepoller.h"

//QVariantMap HPOmniceptProvider::newEyeData = QVariantMap();

ViveSR::anipal::Eye::EyeData VIVEEyePoller::newEyeData = ViveSR::anipal::Eye::EyeData();
std::mutex VIVEEyePoller::mtx;
bool VIVEEyePoller::isNewDataAvailable = false;

VIVEEyePoller::VIVEEyePoller()
{
    keepGoing = false;
    wasInitialized = false;
    compareTolerance = 0.001f;
}

QString VIVEEyePoller::getError() const{
    return error;
}

bool VIVEEyePoller::isRunning() const {
    return keepGoing;
}

bool VIVEEyePoller::initalizeEyeTracking(){

    if (wasInitialized) return true;

    int error_code = ViveSR::anipal::Initial(ViveSR::anipal::Eye::ANIPAL_TYPE_EYE, nullptr);
    if (error_code == ViveSR::Error::WORK) {
        wasInitialized = true;
    }
    else if (error_code == ViveSR::Error::RUNTIME_NOT_FOUND) {
        error = "Eye Tracking Runtime Not Found";
        return false;
    }
    else {
        error = "Failed to initialize Eye engine. please refer the code " + QString::number(error_code) + " of ViveSR::Error";
        return false;
    }

    ViveSR::anipal::Eye::RegisterEyeDataCallback(VIVEEyePoller::EyeCallback);

    return true;

}


void VIVEEyePoller::run(){

    //ViveSR::anipal::Eye::EyeData eye_data;

    keepGoing = true;

    timestampTimer.start();

    QVariantMap eye;

    eye[HTCVIVE::Eye::X] = 0;
    eye[HTCVIVE::Eye::Y] = 0;
    eye[HTCVIVE::Eye::Z] = 0;

    previousSample[HTCVIVE::LeftEye] = eye;
    previousSample[HTCVIVE::RightEye] = eye;

    //qint64 previous = 0;


    while (keepGoing) {
        //int result = ViveSR::anipal::Eye::GetEyeData(&eye_data);

        // Check if the call back made new data available.
        ViveSR::anipal::Eye::EyeData result;
        bool newDataReady = false;
        mtx.lock();
        if (isNewDataAvailable){
            newDataReady = true;
            result = newEyeData;
            isNewDataAvailable = false;
        }
        mtx.unlock();

        if (newDataReady) {

            ViveSR::anipal::Eye::SingleEyeData le = result.verbose_data.left;
            ViveSR::anipal::Eye::SingleEyeData re = result.verbose_data.right;
            //qint32 timestamp = result.timestamp;
            // The use of this timer prevents the posibility of an overflow in the time stamp.
            qint64 timestamp = timestampTimer.elapsed();

            float xl,xr,yl,yr,zl,zr,pl,pr;
            xl = le.gaze_direction_normalized.x;
            yl = le.gaze_direction_normalized.y;
            zl = le.gaze_direction_normalized.z;
            pl = le.pupil_diameter_mm;

            xr = re.gaze_direction_normalized.x;
            yr = re.gaze_direction_normalized.y;
            zr = re.gaze_direction_normalized.z;
            pr = re.pupil_diameter_mm;

            // Ensuring that the sample is different. If it's not, then we return.
            // if (isSampleSameAsPrevious(HTCVIVE::LeftEye,xl,yl,zl) && isSampleSameAsPrevious(HTCVIVE::LeftEye,xr,yr,zr)) continue;

            QVariantMap l;
            QVariantMap r;
            l[HTCVIVE::Eye::X] = xl;
            l[HTCVIVE::Eye::Y] = yl;
            l[HTCVIVE::Eye::Z] = zl;
            l[HTCVIVE::Eye::Pupil] = pl;

            r[HTCVIVE::Eye::X] = xr;
            r[HTCVIVE::Eye::Y] = yr;
            r[HTCVIVE::Eye::Z] = zr;
            r[HTCVIVE::Eye::Pupil] = pr;

            //qint64 sampleRate = (timestampTimer.elapsed() - previous);
            //previous = timestampTimer.elapsed();
            //qint32 sampleRate = (timestamp - previousSample.value(HTCVIVE::Timestamp).toInt());
            //qDebug() << "Sample Rate in MS" << sampleRate;

            previousSample[HTCVIVE::LeftEye] = l;
            previousSample[HTCVIVE::RightEye] = r;
            previousSample[HTCVIVE::Timestamp] = timestamp;

            emit VIVEEyePoller::eyeDataAvailable(previousSample);

        }
    }
}

void VIVEEyePoller::stop(){
    keepGoing = false;
    //qDebug() << "VIVE EYE Poller Was Disabled";
}

bool VIVEEyePoller::isSampleSameAsPrevious(const QString &whichEye, float x, float y, float z){

    QVariantMap eye = previousSample.value(whichEye).toMap();

    float px = eye.value(HTCVIVE::Eye::X).toFloat();
    float py = eye.value(HTCVIVE::Eye::X).toFloat();
    float pz = eye.value(HTCVIVE::Eye::X).toFloat();

    return (qAbs(px - x) < compareTolerance) && (qAbs(py - y) < compareTolerance) && (qAbs(pz - z) < compareTolerance);

}

void VIVEEyePoller::EyeCallback(const ViveSR::anipal::Eye::EyeData &eye_data){

    // Wait for the buffer to be ready.
    mtx.lock();

    // Storing the new informationn
    newEyeData = eye_data;
    isNewDataAvailable = true;
    //qDebug() << "TS" << eye_data.timestamp;

    // Realeasing the protection
    mtx.unlock();
}





