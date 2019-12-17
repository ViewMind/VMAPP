#include "viveeyepoller.h"

VIVEEyePoller::VIVEEyePoller()
{
    keepGoing = true;
}

void VIVEEyePoller::run(){
    ViveSR::anipal::Eye::EyeData eye_data;
    while (keepGoing) {
        int result = ViveSR::anipal::Eye::GetEyeData(&eye_data);
        if (result == ViveSR::Error::WORK) {
            ViveSR::anipal::Eye::SingleEyeData le = eye_data.verbose_data.left;
            ViveSR::anipal::Eye::SingleEyeData re = eye_data.verbose_data.right;

//            qDebug() << "LEFT validity " << le.eye_data_validata_bit_mask << " nomalized gaze dir "
//                     << le.gaze_direction_normalized.x
//                     << le.gaze_direction_normalized.y
//                     << le.gaze_direction_normalized.z
//                     << "PS: " << le.pupil_diameter_mm
//                     << "RIGHT validity " << re.eye_data_validata_bit_mask << " nomalized gaze dir "
//                     << re.gaze_direction_normalized.x
//                     << re.gaze_direction_normalized.y
//                     << re.gaze_direction_normalized.z
//                     << "PS: " << le.pupil_diameter_mm;

            EyeTrackerData data = computeValues(re.gaze_direction_normalized.x,
                                                re.gaze_direction_normalized.y,
                                                le.gaze_direction_normalized.x,
                                                le.gaze_direction_normalized.y);
            lastData = data;
            emit(newEyeData(data));

        }
    }
}

void VIVEEyePoller::setMaxWidthAndHeight(qreal W, qreal H){
    MaxX = W;
    MaxY = H;
    bX = MaxX/2;
    mX = -MaxX/2;
    bY = MaxY/2;
    mY = -MaxY/2;
}

EyeTrackerData VIVEEyePoller::computeValues(qreal xr, qreal yr, qreal xl, qreal yl){
    EyeTrackerData input;
    input.xLeft = static_cast<qint32>(mX*xl+ bX);
    input.xRight = static_cast<qint32>(mX*xr+ bX);
    input.yLeft = static_cast<qint32>(mY*yl+ bY);
    input.yRight = static_cast<qint32>(mY*yr+ bY);
    return input;
}
