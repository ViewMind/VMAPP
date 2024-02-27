#include <QCoreApplication>

#include "../../../EyeExperimenter/src/studyendoperations.h"

namespace Debug {
   ConfigurationManager DEBUG_OPTIONS;
}

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    StaticThreadLogger::StartLogger("StudyVerifTestes","logfile.log");

    StudyEndOperations seo;

    QStringList ftoProcess; ftoProcess << "C:/Users/ViewMind/Documents/temp/StudyEndFiles/sphers_2024_02_22/gonogo_spheres_2024_02_22_11_24.json";

    seo.setFileListToProcess(ftoProcess);

    //seo.doStudyFileProcessing();
    seo.run();

    qDebug() << "Done";

    return a.exec();
}
