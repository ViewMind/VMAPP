#include <QApplication>

#include <QDebug>
#include <QElapsedTimer>
#include "../../../CommonClasses/GraphicalReport/imagereportdrawer.h"
#include "../../../CommonClasses/GraphicalReport/reportimagespdf.h"

// To run in headless server and make this work the following paramters needs to be used: qtappname -platform offscreen

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    QElapsedTimer timer;

    timer.start();

    QString output_dir = "report_pages";

    // Creating the basic configuration
    qDebug() << "Creating the configuration";
    ConfigurationManager config;
    config.addKeyValuePair(CONFIG_REPORT_LANGUAGE,CONFIG_P_LANG_EN); // English language for the report
    config.addKeyValuePair(CONFIG_REPORT_NO_LOGO,false); // Adding the viewmind logo.

    // These values are not used as the fixed report in the test is just a reading report.
    // Added for completness. These are the default values, anywways.
    // QString bindingCode = "I";  // This indicates the newer version of the binding report, where the binding conversion index is used.
    // QString bindingDescription = "2"; // Wheter the biding is 2 or 3 targets.

    qDebug() << "Creating the draw report";
    ImageReportDrawer ird;

    qDebug() << "Loading the test report";
    ConfigurationManager report;
    report.loadConfiguration(":/test.rep",COMMON_TEXT_CODEC);
    QVariantMap ds = report.getMap();

    qDebug() << "Drawing the report";

    // Generati images for three report pages.
    QList<qint32> study_pages;
    study_pages << EXP_READING << EXP_BINDING_BC << EXP_GONOGO;


    QDir current(".");
    current.mkdir(output_dir);
    if (!QDir(output_dir).exists()){
        qDebug() << "Output directory could not be created";
        return 0;
    }

    for (qint32 i = 0; i < study_pages.size(); i++){
        qint32 study_page = study_pages.at(i);
        QString output_file_name = output_dir + "/test_"  + QString::number(i) + ".png";
        config.addKeyValuePair(CONFIG_IMAGE_REPORT_PATH,output_file_name);
        qDebug() << "Drawing page for study " << study_page;
        ird.drawReport(ds,&config,study_page);
    }

    // Generating the PDF.
    QString output_pdf = "report_pages/test.pdf";

    ReportImagesPDF pdf_gen;
    if (!pdf_gen.createPDF(output_dir,output_pdf)){
        qDebug() << "PDF ERROR" << pdf_gen.getError();
        qDebug() << "STD OUT" << pdf_gen.getStdOutput();
        qDebug() << "STD ERR" << pdf_gen.getStdError();
        qDebug() << "CMD" << pdf_gen.getCmd();
    }
    else{
        qDebug() << "ALL OK";
    }

    qDebug() << "Finished" << timer.elapsed() << "ms";

    //return a.exec();
    return 0;
}

