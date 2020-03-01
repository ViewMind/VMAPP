#include "rdataprocessor.h"

RDataProcessor::RDataProcessor()
{
    workDirectory = "";
}


QString RDataProcessor::processReading(const QString &readingFile){
    error = "";
    QFileInfo rfile (readingFile);
    if (!rfile.exists()){
        error = "RPROCESSOR: Reading file: " + readingFile + "does not exist";
        return "";
    }

    QString outputconf = workDirectory + "/" + QString(FILE_R_OUT_READING);;

    QStringList arguments;
    arguments << RSCRIPT_READING;
    arguments << readingFile;
    arguments << workDirectory + "/" + QString(FILE_R_OUT_TEMPRDA);
    arguments << outputconf;

    QProcess::execute(RSCRIPT_RUNNABLE,arguments);

    if (!checkAndMerge(outputconf)) return "";

    QString report = "<br>READING RESULTS:<br>";

    qint32 total   = results.getReal(CONFIG_RESULTS_ATTENTIONAL_PROCESSES);
    qint32 first   = results.getReal(CONFIG_RESULTS_WORKING_MEMORY)*total/100;
    qint32 multi   = results.getReal(CONFIG_RESULTS_EXECUTIVE_PROCESSES)*total/100;
    qint32 single  = results.getReal(CONFIG_RESULTS_RETRIEVAL_MEMORY)*total/100;

    // Saving the DB Data to the right eye, by default, even if it is the right eye
    dbdata.insert(TEYERES_COL_RDFSTEPFIXR,first);
    dbdata.insert(TEYERES_COL_RDMSTEPFIXR,multi);
    dbdata.insert(TEYERES_COL_RDSSTEPFIXR,single);
    dbdata.insert(TEYERES_COL_RDTOTALFIXR,total);
    dbdata.insert(TEYERES_COL_READ_COG_IMPAIR,results.getReal(CONFIG_RESULTS_READ_PREDICTED_DETERIORATION));

    // Report text for Left Eye
    report = report + "Total number of fixations: " + QString::number(total)  + "<br>";
    report = report + "First Step Fixations: "  + QString::number(first) + " (" + results.getString(CONFIG_RESULTS_WORKING_MEMORY)  + "%)<br>";
    report = report + "Multiple Step Fixations: "  + QString::number(multi) + " (" + results.getString(CONFIG_RESULTS_EXECUTIVE_PROCESSES)  + "%)<br>";
    report = report + "Single Fixations: " + QString::number(single) + " (" + results.getString(CONFIG_RESULTS_RETRIEVAL_MEMORY)  + "%)<br>";
    report = report + "Predicted deterioration: " + " (" + results.getString(CONFIG_RESULTS_READ_PREDICTED_DETERIORATION)  + "%)<br>";
    report = report + "<br>REPORT uses value from <b>RIGHT</b> eye<br>" ;

    return report;

}

QString RDataProcessor::processBinding(const QString &bcfile, const QString &ucfile, qint32 bcright, qint32 ucright){
    error = "";

    QFileInfo bc_file (bcfile);
    if (!bc_file.exists()){
        error = "RPROCESSOR: BC file: " + bcfile + "does not exist";
        return "";
    }

    QStringList bres; bres << QString::number(bcright) << QString::number(ucright);
    results.addKeyValuePair(CONFIG_RESULTS_BEHAVIOURAL_RESPONSE,bres.join("|"));

    QFileInfo uc_file (ucfile);
    if (!uc_file.exists()){
        error = "RPROCESSOR: UC file: " + ucfile + "does not exist";
        return "";
    }

    QString outputconf = workDirectory + "/" + QString(FILE_R_OUT_BINDING);

    // Checking the BC target value to finde which script to call.
    QStringList parts = bc_file.baseName().split("_");
    QString rScriptToCall = "";
    if (parts.size() > 3){
        if (parts.at(2) == "2") rScriptToCall = RSCRIPT_BINDING2;
        else if (parts.at(2) == "3") rScriptToCall = RSCRIPT_BINDING3;
    }

    if (rScriptToCall.isEmpty()){
        error = "RPROCESSOR: Cannot determine Binding script to call based on BC file name " + bcfile;
        return "";
    }

    QStringList arguments;
    arguments << rScriptToCall;
    arguments << bcfile;
    arguments << ucfile;
    arguments << outputconf;

    QProcess::execute(RSCRIPT_RUNNABLE,arguments);

    //qDebug() << "BINDING RUN:" << RSCRIPT_RUNNABLE << rScriptToCall << bcfile << ucfile << outputconf;

    if (!checkAndMerge(outputconf)){
        return "";
    }

    qreal bcindex = results.getReal(CONFIG_RESULTS_BC_PREDICTED_DETERIORATION);
    qreal ucindex = results.getReal(CONFIG_RESULTS_UC_PREDICTED_DETERIORATION);
    dbdata.insert(TEYERES_COL_BIND_INDEX_BC,bcindex);
    dbdata.insert(TEYERES_COL_BIND_INDEX_UC,ucindex);

    QString bcgroup = results.getString(CONFIG_RESULTS_BC_PREDICTED_GROUP);
    QString ucgroup = results.getString(CONFIG_RESULTS_UC_PREDICTED_GROUP);
    qreal conversionIndex;

    if (bcgroup == ucgroup){
        conversionIndex = qMin(bcindex,ucindex);
    }
    else {
        if (bcgroup == "NO") conversionIndex = bcindex;
        else conversionIndex = ucindex;
    }

    qDebug() << "BINDING INDEXES";
    qDebug() << bcgroup << ucgroup << bcindex << ucindex << conversionIndex;

    results.addKeyValuePair(CONFIG_RESULTS_BINDING_CONVERSION_INDEX,conversionIndex);

    QString report = "RESULTS (Always using Right EYE):<br>";
    report = report + "BC Group: " + bcgroup + "BC Index: " + QString::number(bcindex);
    report = report + "UC Group: " + ucindex + "UC Index: " + QString::number(ucindex);
    report = report + "Predicted deterioration: " + QString::number(conversionIndex);

    return report;

}


bool RDataProcessor::checkAndMerge(const QString &outputconf){
    ConfigurationManager c;
    if (!c.loadConfiguration(outputconf,COMMON_TEXT_CODEC)){
        error = "RPROCESSOR: Could not load ouput configuration: " + c.getError();
        return false;
    }
    results.merge(c);
    return true;
}
