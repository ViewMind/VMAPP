#include <QCoreApplication>

#include "dataextractor.h"
#include "dataplotter.h"
#include "datrecursivefinder.h"

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

//    DatRecursiveFinder drf;
//    drf.startFrom("/home/ariela/repos/viewmind_projects/Scripts/server_scripts/05InformationDownload/rawdata");
//    for (qint32 i = 0; i < drf.getDatFileList().size(); i++){
//        qWarning() << drf.getDatFileList().at(i);
//    }

    DataExtractor de;
    QString badFile = "/home/ariela/repos/viewmind_projects/Scripts/server_scripts/05InformationDownload/rawdata/00000091/2019_03_12_12_28_20/binding_bc_2_l_2_2019_03_12_12_19.dat";
    QString goodFile = "/home/ariela/Workspace/Viewmind/grace_worked/binding_bc_2_l_2_2019_03_14_17_56.dat";
    DataExtractor::DataPoints plotData = de.analyzeFile(badFile);

    qWarning() << "Saving the data";
    DataPlotter plotter;
    plotter.plotPoints("bad",plotData);

    plotData = de.analyzeFile(goodFile);
    plotter.plotPoints("good",plotData);

    qWarning() << "DONE";

////    for (qint32 i = 0; i < plotData.datapoints.size(); i++){
////        qWarning() << "DATA SET" << i;
////        for (qint32 j = 0; j < plotData.datapoints.at(i).size(); j++){
////            qWarning() << "   ROW" << j << plotData.datapoints.at(i).at(j);
////        }
////    }

    return a.exec();
}
