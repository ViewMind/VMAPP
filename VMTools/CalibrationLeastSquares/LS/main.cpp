#include <QCoreApplication>
#include "calibrationleastsquares.h"
#include <QFile>
#include <QTextStream>

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    QFile file("example_calibration_data");
    if (!file.open(QFile::ReadOnly)) {
        qDebug() << "Could not read calibration data";
        return 0;
    }

    QTextStream reader(&file);
    QStringList lines = reader.readAll().split("\n");
    QList<CalibrationLeastSquares::CalibrationData> cdata;
    CalibrationLeastSquares::CalibrationData currentData;

    for (qint32 i = 0; i < lines.size(); i++){
        QString line = lines.at(i);
        if (line.startsWith("#")){
            if (currentData.xl.size() > 0){
                cdata << currentData;
            }
            currentData.clear();
        }
        else{
            QStringList values = line.split(" ");
            if (values.size() != 4){
                qDebug() << "Skipping line" << i+1 << line;
                continue;
            }
            else{
                // Order is xr yr xl yl
                currentData.xr << values.at(0).toDouble();
                currentData.yr << values.at(1).toDouble();
                currentData.xl << values.at(2).toDouble();
                currentData.yl << values.at(3).toDouble();
            }
        }
    }
    if (currentData.xl.size() > 0){
        cdata << currentData;
    }


    qDebug() << "Done loading data. Number of calibration points: " << cdata.size();
    QString sizes;
    for (qint32 i = 0; i < cdata.size(); i++){
        sizes = sizes + QString::number(cdata.at(i).xl.size()) + " ";
    }
    qDebug() << sizes;

    CalibrationLeastSquares cls;
    if (!cls.computeCalibrationCoeffs(cdata)){
        qDebug() << "Calibration computation failed";
    }
    else{
        qDebug() << "Computation finished.";
        CalibrationLeastSquares::EyeCorrectionCoeffs ecc =  cls.getCalculatedCoeficients();
        qDebug() << "XR" << ecc.xr.m << ecc.xr.b;
        qDebug() << "YR" << ecc.yr.m << ecc.yr.b;
        qDebug() << "XL" << ecc.xl.m << ecc.xl.b;
        qDebug() << "YL" << ecc.yl.m << ecc.yl.b;
    }

    return a.exec();
}
