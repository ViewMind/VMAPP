#include <QCoreApplication>
#include <QJsonValue>
#include <QJsonArray>
#include <QJsonObject>

#include "../../../CommonClasses/RawDataContainer/datfiletorawdatacontainer.h"
#include <iostream>

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);


//    QString dat_file = "/home/ariel/repos/viewmind_projects/VMSoftwareSuite/EyeReportGenerator/bin/res/test_scripts/reference_data/nbackrt/nbackrt_2_2020_06_17_21_03.dat";
//    QString eye_rep_gen_conf = "/home/ariel/repos/viewmind_projects/VMSoftwareSuite/EyeReportGenerator/bin/res/test_scripts/reference_data/nbackrt/eye_rep_gen_conf";

    //QString dat_file = "/home/ariel/repos/viewmind_projects/VMSoftwareSuite/EyeReportGenerator/bin/res/test_scripts/reference_data/binding2/binding_bc_2_l_2_2020_05_17_23_03.dat";
    //QString dat_file = "/home/ariel/repos/viewmind_projects/VMSoftwareSuite/EyeReportGenerator/bin/res/test_scripts/reference_data/binding2/binding_uc_2_l_2_2020_05_17_23_11.dat";
    //QString eye_rep_gen_conf = "/home/ariel/repos/viewmind_projects/VMSoftwareSuite/EyeReportGenerator/bin/res/test_scripts/reference_data/binding2/eye_rep_gen_conf";

    ////QString dat_file = "/home/ariel/repos/viewmind_projects/VMSoftwareSuite/EyeReportGenerator/bin/res/test_scripts/reference_data/gonogo/gonogo_2_2020_11_09_15_05.dat";
    QString dat_file ="/home/ariel/Workspace/Viewmind/vm_data/gonogo/ref_test/gonogo_2_2020_11_09_15_05.dat";
    QString eye_rep_gen_conf = "/home/ariel/Workspace/Viewmind/vm_data/gonogo/ref_test/eye_rep_gen_conf";

    ConfigurationManager configmng;
    if (!configmng.loadConfiguration(eye_rep_gen_conf,COMMON_TEXT_CODEC)){
        qDebug() << "Could not load rep gen conf: " + configmng.getError();
        return 0;
    }

    DatFileToRawDataContainer dfrdc;
    RawDataContainer rdc = dfrdc.fromDatFile(dat_file,configmng);

    if (dfrdc.getError() == ""){
        if (rdc.saveJSONFile("test.json",true)){
            std::cout << "File saved "<< std::endl;
        }
        else{
            qDebug() << "File could not be saved: " << rdc.getError();
        }
    }
    else{
        std::cout << "There was an error " << dfrdc.getError().toStdString() << std::endl;
    }

    return a.exec();
}
