#include "rdataprocessor.h"

RDataProcessor::RDataProcessor()
{
    workDirectory = "";
}

QString RDataProcessor::processNBackRT(const QString &nbackrtFile){
    error = "";
    QFileInfo nbackrt_file (nbackrtFile);
    if (!nbackrt_file.exists()){
        error = "RPROCESSOR: NBack RT file: " + nbackrtFile + "does not exist";
        return "";
    }

    QString outputconf = workDirectory + "/" + QString(FILE_R_OUT_NBACKRT);

    QStringList arguments;
    arguments << RSCRIPT_NBACKRT;
    arguments << nbackrtFile;
    arguments << outputconf;

    qint32 processRetCode = QProcess::execute(RSCRIPT_RUNNABLE,arguments);

    if (processRetCode != 0){
        warning = "RPROCESSOR: Running the script with the arguments: "
                + arguments.join(",") + " has FAILED. Ret Code: "
                + QString::number(processRetCode);
        //return "";
        results.addKeyValuePair(CONFIG_RESULTS_NBACKRT_NUM_FIX_ENC,"N/A");
        results.addKeyValuePair(CONFIG_RESULTS_NBACKRT_NUM_FIX_RET,"N/A");
        results.addKeyValuePair(CONFIG_RESULTS_NBACKRT_INHIBITORY_PROBLEMS,"N/A");
        results.addKeyValuePair(CONFIG_RESULTS_NBACKRT_SEQ_COMPLETE,"N/A");
        results.addKeyValuePair(CONFIG_RESULTS_NBACKRT_TARGET_HIT,"N/A");
        results.addKeyValuePair(CONFIG_RESULTS_NBACKRT_MEAN_RESP_TIME,"N/A");
        results.addKeyValuePair(CONFIG_RESULTS_NBACKRT_MEAN_SAC_AMP,"N/A");
    }
    else{
        if (!checkAndMerge(outputconf)) return "";
    }

    QString report = "<br>NBACK RT RESULTS:<br>";

    // Saving the DB Data
    dbdata.insert(TEYERES_COL_NBRT_FIX_ENC,results.getInt(CONFIG_RESULTS_NBACKRT_NUM_FIX_ENC));
    dbdata.insert(TEYERES_COL_NBRT_FIX_RET,results.getInt(CONFIG_RESULTS_NBACKRT_NUM_FIX_RET));
    dbdata.insert(TEYERES_COL_NBRT_INHIB_PROB,results.getReal(CONFIG_RESULTS_NBACKRT_INHIBITORY_PROBLEMS));
    dbdata.insert(TEYERES_COL_NBRT_SEQ_COMPLETE,results.getReal(CONFIG_RESULTS_NBACKRT_SEQ_COMPLETE));
    dbdata.insert(TEYERES_COL_NBRT_TARGET_HIT,results.getReal(CONFIG_RESULTS_NBACKRT_TARGET_HIT));
    dbdata.insert(TEYERES_COL_NBRT_MEAN_RESP_TIME,results.getReal(CONFIG_RESULTS_NBACKRT_MEAN_RESP_TIME));
    dbdata.insert(TEYERES_COL_NBRT_MEAN_SAC_AMP,results.getReal(CONFIG_RESULTS_NBACKRT_MEAN_SAC_AMP));

    // Text report
    report = report + "Number of Fixations in Encoding: " + results.getString(CONFIG_RESULTS_NBACKRT_NUM_FIX_ENC)  + "<br>";
    report = report + "Number of Fixations in Retrieval: " + results.getString(CONFIG_RESULTS_NBACKRT_NUM_FIX_RET)  + "<br>";
    report = report + "Percent of Inhibitory Problems: " + results.getString(CONFIG_RESULTS_NBACKRT_INHIBITORY_PROBLEMS)  + "<br>";
    report = report + "Percent of Complete Sequences: " + results.getString(CONFIG_RESULTS_NBACKRT_SEQ_COMPLETE)  + "<br>";
    report = report + "Percent of Target Hits: " + results.getString(CONFIG_RESULTS_NBACKRT_TARGET_HIT)  + "<br>";
    report = report + "Mean Response Time: " + results.getString(CONFIG_RESULTS_NBACKRT_MEAN_RESP_TIME)  + "<br>";
    report = report + "Mean Saccade Amplitude: " + results.getString(CONFIG_RESULTS_NBACKRT_MEAN_SAC_AMP)  + "<br>";

    return report;

}

QString RDataProcessor::processGoNoGo(const QString &gonogoFile){

    error = "";
    QFileInfo gonogo_file (gonogoFile);
    if (!gonogo_file.exists()){
        error = "RPROCESSOR: GoNoGo file: " + gonogoFile + "does not exist";
        return "";
    }

    QString outputconf = workDirectory + "/" + QString(FILE_R_OUT_GONOGO);

    QStringList arguments;
    arguments << RSCRIPT_GONOGO;
    arguments << gonogoFile;
    arguments << outputconf;

    qint32 processRetCode = QProcess::execute(RSCRIPT_RUNNABLE,arguments);

    if (processRetCode != 0){
        warning = "RPROCESSOR: Running the script with the arguments: "
                + arguments.join(",") + " has FAILED. Ret Code: "
                + QString::number(processRetCode);
        //return "";
        results.addKeyValuePair(CONFIG_RESULTS_GNG_DMT_FACILITATE,"N/A");
        results.addKeyValuePair(CONFIG_RESULTS_GNG_DMT_INTERFERENCE,"N/A");
        results.addKeyValuePair(CONFIG_RESULTS_GNG_PIP_FACILITATE,"N/A");
        results.addKeyValuePair(CONFIG_RESULTS_GNG_PIP_INTERFERENCE,"N/A");
        results.addKeyValuePair(CONFIG_RESULTS_GNG_SPEED_PROCESSING,"N/A");
    }
    else{
        if (!checkAndMerge(outputconf)) return "";
    }

    QString report = "<br>GONOGO RESULTS:<br>";

    // Saving the DB Data
    dbdata.insert(TEYERES_COL_GNG_DMT_FACILITATE,results.getReal(CONFIG_RESULTS_GNG_DMT_FACILITATE));
    dbdata.insert(TEYERES_COL_GNG_DMT_INTERFERENCE,results.getReal(CONFIG_RESULTS_GNG_DMT_INTERFERENCE));
    dbdata.insert(TEYERES_COL_GNG_PIP_FACILIATATE,results.getReal(CONFIG_RESULTS_GNG_PIP_FACILITATE));
    dbdata.insert(TEYERES_COL_GNG_PIP_INTERFERENCE,results.getReal(CONFIG_RESULTS_GNG_PIP_INTERFERENCE));
    dbdata.insert(TEYERES_COL_GNG_SPEED_PROCESSING,results.getReal(CONFIG_RESULTS_GNG_SPEED_PROCESSING));

    // Text report
    report = report + "Decision Making Time - Facilitate: " + results.getString(CONFIG_RESULTS_GNG_DMT_FACILITATE)  + "<br>";
    report = report + "Decision Making Time - Interference: " + results.getString(CONFIG_RESULTS_GNG_DMT_INTERFERENCE)  + "<br>";
    report = report + "Percentage of Inhibitory Processes - Facilitation: " + results.getString(CONFIG_RESULTS_GNG_PIP_FACILITATE)  + "<br>";
    report = report + "Percentage of Inhibitory Processes - Interference: " + results.getString(CONFIG_RESULTS_GNG_PIP_INTERFERENCE)  + "<br>";
    report = report + "Speed Processing: " + results.getString(CONFIG_RESULTS_GNG_SPEED_PROCESSING)  + "<br>";

    return report;


}

QString RDataProcessor::processReading(const QString &readingFile){
    error = "";
    QFileInfo rfile (readingFile);
    if (!rfile.exists()){
        error = "RPROCESSOR: Reading file: " + readingFile + "does not exist";
        return "";
    }

    QString outputconf = workDirectory + "/" + QString(FILE_R_OUT_READING);

    QStringList arguments;
    arguments << RSCRIPT_READING;
    arguments << readingFile;
    arguments << outputconf;

    qint32 processRetCode = QProcess::execute(RSCRIPT_RUNNABLE,arguments);

    if (processRetCode != 0){
        warning = "RPROCESSOR: Running the script with the arguments: "
                + arguments.join(",") + " has FAILED. Ret Code: "
                + QString::number(processRetCode);
        //return "";
        results.addKeyValuePair(CONFIG_RESULTS_ATTENTIONAL_PROCESSES,"N/A");
        results.addKeyValuePair(CONFIG_RESULTS_WORKING_MEMORY,"N/A");
        results.addKeyValuePair(CONFIG_RESULTS_EXECUTIVE_PROCESSES,"N/A");
        results.addKeyValuePair(CONFIG_RESULTS_RETRIEVAL_MEMORY,"N/A");
        results.addKeyValuePair(CONFIG_RESULTS_READ_PREDICTED_DETERIORATION,"N/A");
    }
    else{
        if (!checkAndMerge(outputconf)) return "";
    }

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

    qint32 processRetCode = QProcess::execute(RSCRIPT_RUNNABLE,arguments);

    //qDebug() << "Process returned " << processRetCode;
    if (processRetCode != 0){
        warning = "RPROCESSOR: Running the script with the arguments: "
                + arguments.join(",") + " has FAILED. Ret Code: "
                + QString::number(processRetCode);
        results.addKeyValuePair(CONFIG_RESULTS_BC_PREDICTED_DETERIORATION,"N/A");
        results.addKeyValuePair(CONFIG_RESULTS_UC_PREDICTED_DETERIORATION,"N/A");
        results.addKeyValuePair(CONFIG_RESULTS_BC_PREDICTED_GROUP,"N/A");
        results.addKeyValuePair(CONFIG_RESULTS_UC_PREDICTED_GROUP,"N/A");
    }
    else{
        if (!checkAndMerge(outputconf)){
            return "";
        }
    }

    //qDebug() << "BINDING RUN:" << RSCRIPT_RUNNABLE << rScriptToCall << bcfile << ucfile << outputconf;

    qreal bcindex = results.getReal(CONFIG_RESULTS_BC_PREDICTED_DETERIORATION);
    qreal ucindex = results.getReal(CONFIG_RESULTS_UC_PREDICTED_DETERIORATION);
    dbdata.insert(TEYERES_COL_BIND_INDEX_BC,bcindex);
    dbdata.insert(TEYERES_COL_BIND_INDEX_UC,ucindex);

    QString bcgroup = results.getString(CONFIG_RESULTS_BC_PREDICTED_GROUP);
    QString ucgroup = results.getString(CONFIG_RESULTS_UC_PREDICTED_GROUP);
    qreal conversionIndex;

    conversionIndex = bcindex;

    //qDebug() << "BINDING INDEXES";
    //qDebug() << bcgroup << ucgroup << bcindex << ucindex << conversionIndex;

    results.addKeyValuePair(CONFIG_RESULTS_BINDING_CONVERSION_INDEX,conversionIndex);

    QString report = "RESULTS (Always using Right EYE):<br>";
    report = report + "BC Group: " + bcgroup + "BC Index: " + QString::number(bcindex);
    report = report + "UC Group: " + ucgroup + "UC Index: " + QString::number(ucindex);
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
