#include <QCoreApplication>
#include <QJsonValue>
#include <QJsonArray>
#include <QJsonObject>
#include <QProcess>

#include "../../../CommonClasses/RawDataContainer/datfiletorawdatacontainer.h"
#include "../../../VMSoftwareSuite/EyeExperimenter/src/qualitycontrol.h"

#include <iostream>

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);


//    QString dat_file = "/home/ariel/repos/viewmind_projects/VMSoftwareSuite/EyeReportGenerator/bin/res/test_scripts/reference_data/nbackrt/nbackrt_2_2020_06_17_21_03.dat";
//    QString eye_rep_gen_conf = "/home/ariel/repos/viewmind_projects/VMSoftwareSuite/EyeReportGenerator/bin/res/test_scripts/reference_data/nbackrt/eye_rep_gen_conf";

    //QString dat_file = "/home/ariel/repos/viewmind_projects/VMSoftwareSuite/EyeReportGenerator/bin/res/test_scripts/reference_data/binding2/binding_bc_2_l_2_2020_05_17_23_03.dat";
    //QString dat_file = "/home/ariel/repos/viewmind_projects/VMSoftwareSuite/EyeReportGenerator/bin/res/test_scripts/reference_data/binding2/binding_uc_2_l_2_2020_05_17_23_11.dat";
    //QString eye_rep_gen_conf = "/home/ariel/repos/viewmind_projects/VMSoftwareSuite/EyeReportGenerator/bin/res/test_scripts/reference_data/binding2/eye_rep_gen_conf";


    QString dir = "/home/ariel/repos/viewmind_projects/Scripts/R/test_scripts/reference_data/binding3";

    ViewMindDataContainer vmdc;
    QStringList filter; filter << "binding*.dat*";
    QStringList allfiles = QDir(dir).entryList(filter,QDir::Files|QDir::NoDotAndDotDot);

    QString eye_rep_gen_conf = dir + "/eye_rep_gen_conf";
    ConfigurationManager configmng;
    if (!configmng.loadConfiguration(eye_rep_gen_conf,"UTF-8")){
        qDebug() << "Could not load rep gen conf: " + configmng.getError();
        return 0;
    }

    QString jsonFileName;

    for (qint32 i = 0; i < allfiles.size(); i++){

        qDebug() << "Doing file" << allfiles.at(i);
        DatFileToViewMindDataContainer dfrdc;
        dfrdc.fromDatFile(dir + "/" + allfiles.at(i),configmng,&vmdc);

        jsonFileName = dfrdc.getFinalStudyFileName();

        if (dfrdc.getError() != ""){
           std::cout << "There was an error " << dfrdc.getError().toStdString() << std::endl;
           return 0;
        }

    }

    if (vmdc.saveJSONFile(jsonFileName,true)){
        std::cout << "File saved. Doing QC ... "<< std::endl;

        // File is saved, doing Quality Control.
        QualityControl qc;
        qc.setVMContainterFile(jsonFileName);
        qc.run();
        if (!qc.getError().isEmpty()){
            std::cout << "QC Failed: " << qc.getError().toStdString() << std::endl;
            return 0;
        }

        std::cout << "Done, now zipping ... "<< std::endl;

        QString zipfile = jsonFileName.split(".").first() + ".zip";

        QStringList arguments;
        arguments << "-c";
        arguments << "-z";
        arguments << "-f";
        arguments << zipfile;
        arguments << jsonFileName;

        std::cout << "Exit code for zipping: " <<  QProcess::execute("tar",arguments) << std::endl;

    }
    else{
        qDebug() << "File could not be saved: " << vmdc.getError();
    }



    return a.exec();
}
