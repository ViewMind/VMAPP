#include "movingwindowalgorithm.h"

MovingWindowAlgorithm::MovingWindowAlgorithm()
{

}

void MovingWindowAlgorithm::setOnlineFixationAnalysisFileOutput(const QString &filename){
    onlineFixationLogFile = filename;
}


Fixation MovingWindowAlgorithm::calculateFixationsOnline(qreal x, qreal y, qreal timeStamp, qreal pupil){
    Fixation fixation;

    // Creating the data point and adding it to the list.
    Fixation::DataPoint p; p.x = x; p.y = y; p.timestamp = timeStamp; p.pupil = pupil;
    onlinePointsForFixation << p;

    QVariantMap logItem;
    logItem["type"] = "POINT";
    logItem["x"] = p.x;
    logItem["y"] = p.y;
    logItem["time"] = p.timestamp;

    // If there are not enough points to consider a fixation, then there is no fixation.
    if (onlinePointsForFixation.size() < parameters.getStartWindowSize()) {

        // If the log file was set then the point is saved with the rest of the parameters as is.
        if (!onlineFixationLogFile.isEmpty()){
            logItem["online_list_size"] = onlinePointsForFixation.size();
            logItem["min_x"] = onlineMMMX.min;
            logItem["max_x"] = onlineMMMX.max;
            logItem["min_y"] = onlineMMMY.min;
            logItem["max_y"] = onlineMMMY.max;
            logItem["dispersion"] = onlineMMMX.diff() + onlineMMMY.diff();
            logItem["max_dispersion"] = parameters.maxDispersion;
            logItem["sample_frequency"] = parameters.sampleFrequency;
            logItem["minimum_window_size"] = parameters.getStartWindowSize();
            onlineFixationLog << logItem;
        }

        return fixation;
    }

    if (onlinePointsForFixation.size() == parameters.getStartWindowSize()){

        // This is the case where the dispersion is first computed.
        onlineFindDispLimits();
        if ((onlineMMMX.diff() + onlineMMMY.diff()) > parameters.maxDispersion){
            // This means that this set of data cannot form a fixation, so the first value is discarded.
            onlinePointsForFixation.removeFirst();
        }

        // If the log file was set then the point is saved with the rest of the parameters as is.
        if (!onlineFixationLogFile.isEmpty()){
            logItem["online_list_size"] = onlinePointsForFixation.size();
            logItem["min_x"] = onlineMMMX.min;
            logItem["max_x"] = onlineMMMX.max;
            logItem["min_y"] = onlineMMMY.min;
            logItem["max_y"] = onlineMMMY.max;
            logItem["dispersion"] = onlineMMMX.diff() + onlineMMMY.diff();
            logItem["max_dispersion"] = parameters.maxDispersion;
            onlineFixationLog << logItem;
        }

        return fixation;
    }

    // If the code got here the number of online points for fixations is greater than the minimum to form a fixation.
    // So first we update the dispersion structure.
    onlineMMMX.max = qMax(onlineMMMX.max,x);
    onlineMMMY.max = qMax(onlineMMMY.max,y);
    onlineMMMX.min = qMin(onlineMMMX.min,x);
    onlineMMMY.min = qMin(onlineMMMY.min,y);

    // At this point, no other changes will ocurr with the variables, unles a fixation is formed. so we can save the log, if it was enabled.
    if (!onlineFixationLogFile.isEmpty()){
        logItem["online_list_size"] = onlinePointsForFixation.size();
        logItem["min_x"] = onlineMMMX.min;
        logItem["max_x"] = onlineMMMX.max;
        logItem["min_y"] = onlineMMMY.min;
        logItem["max_y"] = onlineMMMY.max;
        logItem["dispersion"] = onlineMMMX.diff() + onlineMMMY.diff();
        logItem["max_dispersion"] = parameters.maxDispersion;
        onlineFixationLog << logItem;
    }

    // We now check if the maximum dispersion allowed was breached. At this point a fixation is formed. It's just a matter of how long it is.

    if ((onlineMMMX.diff() + onlineMMMY.diff()) > parameters.maxDispersion){

        // If it was then that means that the last point should removed and the fixation has ended.
        onlinePointsForFixation.removeLast();
        fixation.construct(onlinePointsForFixation,true);
        onlinePointsForFixation.clear();
        // The last point might be the start of a new fixation.
        onlinePointsForFixation << p;

        if (!onlineFixationLogFile.isEmpty()){
            QVariantMap logItem;
            logItem["type"] = "FIX";
            logItem["x"] = fixation.getX();
            logItem["y"] = fixation.getY();
            logItem["time"] = fixation.getTime();
            logItem["duration"] = fixation.getDuration();
            logItem["status"] = "finished";
            onlineFixationLog << logItem;
        }

    }
    else {
        // The fixation is formed but it has not ended.
        fixation.construct(onlinePointsForFixation,false);
        if (!onlineFixationLogFile.isEmpty()){
            QVariantMap logItem;
            logItem["type"] = "FIX";
            logItem["x"] = fixation.getX();
            logItem["y"] = fixation.getY();
            logItem["time"] = fixation.getTime();
            logItem["duration"] = fixation.getDuration();
            logItem["status"] = "started";
            onlineFixationLog << logItem;
        }
    }

    return fixation;
}

void MovingWindowAlgorithm::finalizeOnlineFixationLog(){
    if (onlineFixationLogFile.isEmpty()) return;
    QFile file(onlineFixationLogFile);
    file.open(QFile::WriteOnly);
    QTextStream writer(&file);
    for (qint32 i = 0; i < onlineFixationLog.length(); i++){
        QJsonDocument json = QJsonDocument::fromVariant(onlineFixationLog.at(i));
        QByteArray data  = json.toJson(QJsonDocument::Compact);
        writer << QString(data) + "\n";
    }
    file.close();
}

Fixation MovingWindowAlgorithm::finalizeOnlineFixationCalculation(const QString &logfileIdentier){
    // Computing the final fixation if there was one.
    Fixation fixation;

    if (parameters.getStartWindowSize() <= 0) {
        qDebug() << "Online fixation algorithm with zero starting window size";
        return fixation;
    }

    if (onlinePointsForFixation.size() >= parameters.getStartWindowSize()){
        fixation.construct(onlinePointsForFixation,true);
    }

    // If the log file was set the fixation is saved and
    if (!onlineFixationLogFile.isEmpty()){

        // Adding the final fixation log line if it was valid.
        if (!fixation.isInvalid()){
            QVariantMap logItem;
            logItem["type"] = "FIX";
            logItem["x"] = fixation.getX();
            logItem["y"] = fixation.getY();
            logItem["time"] = fixation.getTime();
            logItem["duration"] = fixation.getDuration();
            logItem["status"] = "finished";
            onlineFixationLog << logItem;
        }

        // Adding a new log line that separates the trial
        QVariantMap temp;
        temp["newtrial"] = logfileIdentier;
        onlineFixationLog << temp;

    }

    onlinePointsForFixation.clear();
    return fixation;
}

void MovingWindowAlgorithm::onlineFindDispLimits(){

    onlineMMMX.max = 0;
    onlineMMMX.min = 0;
    onlineMMMY.max = 0;
    onlineMMMY.min = 0;

    if (parameters.getStartWindowSize() <= 0) {
        qDebug() << "Online fixation algorithm with zero starting window size";
        return;
    }

    // This check should not be necessary, but done just in case.
    if (onlinePointsForFixation.size() < parameters.getStartWindowSize()) return;

    onlineMMMX.max = onlinePointsForFixation.first().x;
    onlineMMMX.min = onlinePointsForFixation.last().x;
    onlineMMMY.max = onlinePointsForFixation.first().y;
    onlineMMMY.min = onlinePointsForFixation.last().y;

    for (qint32 i = 0; i < onlinePointsForFixation.size(); i++){
        onlineMMMX.max = qMax(onlineMMMX.max,onlinePointsForFixation.at(i).x);
        onlineMMMX.min = qMin(onlineMMMX.min,onlinePointsForFixation.at(i).x);
        onlineMMMY.max = qMax(onlineMMMY.max,onlinePointsForFixation.at(i).y);
        onlineMMMY.min = qMin(onlineMMMY.min,onlinePointsForFixation.at(i).y);
    }

}
