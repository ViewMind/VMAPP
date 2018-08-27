#include "edpbase.h"

EDPBase::EDPBase(ConfigurationManager *c)
{
    config = c;
    error = "";
    mwa.parameters.maxDispersion = 30;
    mwa.parameters.minimumFixationLength = 50;
    mwa.parameters.sampleFrequency = 120;
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




