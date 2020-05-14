#include "edpbase.h"

const qreal MonitorGeometry::VR_VIEW_WIDTH               = 2000.0;
const qreal MonitorGeometry::VR_VIEW_HEIGHT              = 2000.0;
const qreal MonitorGeometry::VR_VIEW_DISTANCE_TO_MONITOR = 2000.0;
const qreal MonitorGeometry::SACADE_NORM_CONSTANT        = 20.0;

EDPBase::EDPBase(ConfigurationManager *c)
{
    config = c;
    error = "";
    mwa.parameters.maxDispersion = 30;
    mwa.parameters.minimumFixationLength = 50;
    mwa.parameters.sampleFrequency = 120;
    if (c->containsKeyword(CONFIG_RESOLUTION_SCALING)){
        resolutionScaling = c->getReal(CONFIG_RESOLUTION_SCALING);
    }
    else{
        resolutionScaling = 1;
    }
    eyeFixations.clear();
}

void EDPBase::configure(const QString &fileName, const QString &exp){

    QFileInfo info (fileName);
    workingDirectory = info.absolutePath();
    QFileInfo pname(workingDirectory);

    // Creating the image output directory if it doesn't exist
    subjectIdentifier = pname.baseName();
    outputFile = workingDirectory + "/" + info.baseName() + ".csv";
    eyeFixations.experimentDescription = exp;

}

/************************** Auxiliary functions*******************************/
qreal EDPBase::averageColumnOfMatrix(const DataMatrix &data, qint32 col,
                                              qint32 startRow, qint32 endRow){
    qreal avg = 0;
    for (qint32 i = startRow; i <= endRow; i++){
        avg = avg + data.at(i).at(col);
    }
    qreal size = endRow - startRow + 1;
    return avg/size;

}

qint32 EDPBase::countZeros(const DataMatrix &data,
                                    qint32 col,
                                    qint32 startRow,
                                    qint32 endRow,qreal tol) const{

    qint32 counter = 0;
    for (qint32 i = startRow; i <= endRow; i++){
        if (data.at(i).at(col) <= tol) counter++;
    }
    return 0;

}

qreal EDPBase::getGaze(const Fixations &fixations){
    qreal gaze = 0;
    for (qint32 i = 0; i < fixations.length(); i++){
        gaze = gaze + fixations.at(i).duration;
    }
    return gaze;
}

qreal EDPBase::calculateSamplingFrequency(const DataMatrix &eventData, qint32 timeCol){

    qreal freqAcc = 0;
    qreal freqCounter = eventData.size()-1;

    for (qint32 i = 1; i < eventData.size(); i++){
        //qWarning() << eventData.at(i).at(timeCol) <<  eventData.at(i-1).at(timeCol) << eventData.at(i).at(timeCol) - eventData.at(i-1).at(timeCol);
        freqAcc = freqAcc + ((qreal)(1000))/(eventData.at(i).at(timeCol) - eventData.at(i-1).at(timeCol));
    }

    return freqAcc/freqCounter;

}

/************************** Saccade amplitude calculator *******************************/

void EDPBase::SacadeAmplitudeCalculator::reset(){
    lastX = -1;
    lastY = -1;
}

qreal EDPBase::SacadeAmplitudeCalculator::calculateSacadeAmplitudeOLD(qreal x, qreal y, const MonitorGeometry &monitorGeometry){
    qreal sacade = 0;
    if ((lastX > -1) && (lastY > -1)){
        // Calculating the sacade
        qreal xINmm = (lastX - x)*monitorGeometry.XmmToPxRatio;
        qreal yINmm = (lastY - y)*monitorGeometry.YmmToPxRatio;
        qreal delta = qSqrt(qPow(xINmm,2) + qPow(yINmm,2));
        sacade = qAtan(delta/monitorGeometry.distanceToMonitorInMilimiters)*180.0/3.141516;
    }
    lastX = x;
    lastY = y;
    return sacade;
}

qreal EDPBase::SacadeAmplitudeCalculator::calculateSacadeAmplitude(qreal x, qreal y, const MonitorGeometry &monitorGeometry){
    qreal sacade = 0;
    if ((lastX > -1) && (lastY > -1)){
        // Calculating the sacade
        qreal normX = (lastX - x)*MonitorGeometry::SACADE_NORM_CONSTANT/monitorGeometry.resolutionWidth;
        qreal normY = (lastY - y)*MonitorGeometry::SACADE_NORM_CONSTANT/monitorGeometry.resolutionHeight;
        sacade = qSqrt(qPow(normX,2) + qPow(normY,2));
        //qDebug() << "SACADE: (" << lastX << "-" << x << ") && (" << lastY << "-" << y << ") RES" << monitorGeometry.resolutionWidth << monitorGeometry.resolutionHeight;
    }
    lastX = x;
    lastY = y;
    return sacade;
}

DataMatrix EDPBase::resolutionScaleDataMatrix(const DataMatrix &matrix, qint32 xl, qint32 yl, qint32 xr, qint32 yr){
    if (resolutionScaling > 1){
        DataMatrix ans;
        for (qint32 i = 0; i < matrix.size(); i++){
            DataRow row = matrix.at(i);
            //qDebug() << "INPUT" << row;

            // Right
            qreal x = row.at(xr)*resolutionScaling/monitorGeometry.resolutionWidth;
            qreal y = row.at(yr)*resolutionScaling/monitorGeometry.resolutionHeight;
            row.replace(xr,x);
            row.replace(yr,y);

            // LEFT
            x = row.at(xl)*resolutionScaling/monitorGeometry.resolutionWidth;
            y = row.at(yl)*resolutionScaling/monitorGeometry.resolutionHeight;
            row.replace(xl,x);
            row.replace(yl,y);

            //qDebug() << "OUTPUT" << row;
            ans << row;
        }
        return ans;
    }
    else return matrix;
}

