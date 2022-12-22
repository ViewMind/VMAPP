#include <QCoreApplication>

#include <QFile>
#include <QTextStream>
#include "../../../CommonClasses/LinearLeastSquares/ordinaryleastsquares.h"

QVector<qreal> parseEyeSample(QString sample){
    sample = sample.replace('L',"");
    sample = sample.trimmed();
    sample = sample.replace('(',"");
    sample = sample.replace(')',"");
    QStringList tokens = sample.split(",");
    QVector<qreal> ans;
    for (qint32 j = 0; j < tokens.size(); j++){
        QString tok = tokens.at(j).trimmed();
        ans << tok.toDouble();
    }
    return ans;
}

void RegressionTest() {

    QFile file("calibration_data.txt");
    file.open(QFile::ReadOnly);
    QTextStream reader(&file);
    QString content = reader.readAll();
    file.close();

    QStringList lines = content.split("\n",Qt::SkipEmptyParts);

    OrdinaryLeastSquares ols;
    ols.reset();

    QVector<qreal> currentTarget;

    for (qint32 i = 0; i < lines.size(); i++){
        QString line = lines.at(i);

        if (!line.startsWith("R")){
            // This is the target vector.
            currentTarget = parseEyeSample(line);
            qDebug() << "Current Target" << currentTarget;
        }

        else {

            QStringList tokens = line.split(":",Qt::SkipEmptyParts);
            //qDebug() << tokens;
            if (tokens.size() != 3) continue;
            QString r = tokens.at(1);
            QString l = tokens.at(2);
            QVector<qreal> sr = parseEyeSample(r);
            QVector<qreal> sl = parseEyeSample(l);

            //qDebug() << "R" << sr << "L" << sl;
            sr << 1;
            sl << 1;

            ols.addSample(sr,currentTarget.at(2));

        }

    }

    if (!ols.computeCoefficients()){
        qDebug() << "ERROR" << ols.getError();
    }
    else {
        qDebug().noquote() << "Success. Printing output matrix\n" << ols.getCoefficients().prettyPrint();
        qDebug() << "RSquared" << ols.getRSquared();
    }


}


int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    qDebug() << "Hello World";

    RegressionTest();

    return a.exec();
}
