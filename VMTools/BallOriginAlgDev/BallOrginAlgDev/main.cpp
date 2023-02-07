#include <QCoreApplication>
#include <QFile>
#include <QTextStream>
#include "../../../CommonClasses/BallPathGenerator/ballpathgenerator.h"

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    qDebug() << "Hola Mundo";

    qint32 NSeqs = 100;
    qint32 npasses = 50; // 50

    QStringList fileContent;

    for (qint32 k = 0; k < NSeqs; k++){
        QList< QList<qint32> > sequence = BallPathGenerator::GenerateBallPathSequence(5,5,3,npasses,2.0,3.0);

        for (qint32 i = 0; i < sequence.size(); i++){
            QStringList seq;
            for (qint32 j = 0; j < sequence.at(i).size(); j++){
                seq << QString::number(sequence.at(i).at(j));
            }
            //qDebug().noquote() << seq.join(" ");
            fileContent << seq.join(" ");
        }

        fileContent << "=====";
    }

    QFile file("seq_output.txt");
    file.open(QFile::WriteOnly);
    QTextStream writer(&file);
    writer << fileContent.join("\n");
    file.close();


    qDebug() << "Finished";

    return a.exec();
}
