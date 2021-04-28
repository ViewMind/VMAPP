#include <QApplication>

#include <QDebug>
#include "../../../CommonClasses/GraphicalReport/imagereportdrawer.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    // Creating the basic configuration
    qDebug() << "Creating the configuration";
    ConfigurationManager config;
    config.addKeyValuePair(CONFIG_REPORT_LANGUAGE,CONFIG_P_LANG_EN); // English language for the report
    config.addKeyValuePair(CONFIG_IMAGE_REPORT_PATH,"test.png");     // Output path.
    config.addKeyValuePair(CONFIG_REPORT_NO_LOGO,false); // Adding the viewmind logo.

    // Create the reading report page.
    qint32 study_page = EXP_READING;

    // These values are not used as the fixed report in the test is just a reading report.
    //QString bindingCode = "I";  // This indicates the newer version of the binding report, where the binding conversion index is used.
    //QString bindingDescription = "3"; // Wheter the biding is 2 or 3 targets.

    qDebug() << "Creating the draw report";
    ImageReportDrawer ird;

    qDebug() << "Loading the test report";
    ConfigurationManager report;
    report.loadConfiguration("test.rep",COMMON_TEXT_CODEC);
    QVariantMap ds = report.getMap();

    qDebug() << "Drawing the report";

    ird.drawReport(ds,&config,study_page);

    qDebug() << "Finished";

    return a.exec();
}
