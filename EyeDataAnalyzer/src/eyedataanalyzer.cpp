#include "eyedataanalyzer.h"
#include "ui_eyedataanalyzer.h"

EyeDataAnalyzer::EyeDataAnalyzer(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::EyeDataAnalyzer)
{
    ui->setupUi(this);
    logForProcessing.setGraphicalLogInterface();
    connect(&logForProcessing,SIGNAL(newUiMessage(QString)),this,SLOT(on_newUIMessage(QString)));
    this->setWindowTitle(QString(PROGRAM_NAME) + " - " + QString(PROGRAM_VERSION));

    appViews.append(ui->groupBoxDataBaseView);
    appViews.append(ui->groupBoxProcessingView);

    // Default values
    defaultReportCompletionParameters.addKeyValuePair(CONFIG_DOCTOR_NAME,"No Doctor");
    defaultReportCompletionParameters.addKeyValuePair(CONFIG_PATIENT_AGE,"0");
    defaultReportCompletionParameters.addKeyValuePair(CONFIG_PATIENT_NAME,"Unnamed");

    // Default processing parameters
    defaultValues.addKeyValuePair(CONFIG_MOVING_WINDOW_DISP,100);
    defaultValues.addKeyValuePair(CONFIG_MIN_FIXATION_LENGTH,50);
    defaultValues.addKeyValuePair(CONFIG_SAMPLE_FREQUENCY,150);
    defaultValues.addKeyValuePair(CONFIG_DISTANCE_2_MONITOR,60);
    defaultValues.addKeyValuePair(CONFIG_XPX_2_MM,0.25);
    defaultValues.addKeyValuePair(CONFIG_YPX_2_MM,0.25);
    defaultValues.addKeyValuePair(CONFIG_LATENCY_ESCAPE_RAD,80);
    defaultValues.addKeyValuePair(CONFIG_MARGIN_TARGET_HIT,10);
    defaultValues.addKeyValuePair(CONFIG_VALID_EYE,2);
    defaultValues.addKeyValuePair(CONFIG_DAT_TIME_FILTER_THRESHOLD,0);

    // Frequency analysis paramters
    defaultValues.addKeyValuePair(CONFIG_TOL_MAX_PERIOD_TOL,8);
    defaultValues.addKeyValuePair(CONFIG_TOL_MIN_PERIOD_TOL,4);
    defaultValues.addKeyValuePair(CONFIG_TOL_MAX_FGLITECHES_IN_TRIAL,10);
    defaultValues.addKeyValuePair(CONFIG_TOL_MAX_PERCENT_OF_INVALID_VALUES,3);
    defaultValues.addKeyValuePair(CONFIG_TOL_MIN_NUMBER_OF_DATA_ITEMS_IN_TRIAL,50);
    defaultValues.addKeyValuePair(CONFIG_TOL_NUM_ALLOWED_FAILED_DATA_SETS,0);

    //switchViews(VIEW_1_PROCESSING_VIEW);
    switchViews(VIEW_0_DATABASE_VIEW);
    currentDirectory = "work";
    processDirectory();

    if (!QSslSocket::supportsSsl()){
        QMessageBox::critical(this,"SSL Error","There is no SSL support. The application will be unable to download any information",QMessageBox::Ok);
    }

}

void EyeDataAnalyzer::processDirectory(){

    QStringList filters;
    QDir currentDir(currentDirectory);
    QStringList directories = currentDir.entryList(filters,QDir::Dirs|QDir::NoDotAndDotDot);
    filters.clear(); filters << "*.dat";
    QStringList datFiles = currentDir.entryList(filters,QDir::Files);
    filters.clear(); filters << "*.rep";
    QStringList repFiles = currentDir.entryList(filters,QDir::Files);


    // Check if eye_rep_gen_conf exists
    filters.clear(); filters << "eye_rep_gen*";
    QStringList eyeRepGen = currentDir.entryList(filters,QDir::Files);

    if (eyeRepGen.size() > 0){
        QString filename = currentDirectory + "/" + eyeRepGen.first();
        QFile file(filename);
        if (!file.open(QFile::ReadOnly)){
            logForProcessing.appendError("Could not open " + eyeRepGen.first() + " for reading");
        }
        else{
            QTextStream reader(&file);
            reader.setCodec(COMMON_TEXT_CODEC);
            ui->pteConfig->setPlainText(reader.readAll());
            file.close();
        }
    }

    // Loading the default settings that are nto presesnt
    if (!QFile(FILE_DEFAULT_VALUES).exists()){
        //logForProcessing.appendStandard("Antes");
        overWriteCurrentConfigurationWith(defaultValues,true);
        //logForProcessing.appendStandard("MIDDLE");
        overWriteCurrentConfigurationWith(defaultReportCompletionParameters,true);
        //logForProcessing.appendStandard("Despues");
    }
    else{
        ConfigurationManager config;
        if (!config.loadConfiguration(FILE_DEFAULT_VALUES,COMMON_TEXT_CODEC)){
            logForProcessing.appendError("Error loading default settings file: " + config.getError());
            return;
        }
        overWriteCurrentConfigurationWith(config,true);
    }

    ui->lwDirs->clear();
    ui->lwDatFiles->clear();

    for (qint32 i = 0; i < directories.size(); i++){
        QListWidgetItem *item = new QListWidgetItem(directories.at(i));
        item->setData(ROLE_DATA,currentDirectory + "/" + directories.at(i));
        ui->lwDirs->addItem(item);
    }

    for (qint32 i = 0; i < datFiles.size(); i++){
        QListWidgetItem *item = new QListWidgetItem(datFiles.at(i));
        item->setFlags(item->flags() | Qt::ItemIsUserCheckable);
        //item->setCheckState(Qt::Unchecked);
        item->setData(ROLE_DATA,datFiles.at(i));
        ui->lwDatFiles->addItem(item);
    }

    for (qint32 i = 0; i < repFiles.size(); i++){
        QListWidgetItem *item = new QListWidgetItem(repFiles.at(i));
        item->setData(ROLE_DATA,repFiles.at(i));
        ui->lwRepFiles->addItem(item);
    }

}

void EyeDataAnalyzer::on_newUIMessage(const QString &html){
    ui->pteProcessingOutput->appendHtml(html);
}

void EyeDataAnalyzer::onProcessorMessage(const QString &msg, qint32 type){
    if (type == MSG_TYPE_STD) htmlWriter.appendStandard(msg);
    else if (type == MSG_TYPE_SUCC) htmlWriter.appendSuccess(msg);
    else if (type == MSG_TYPE_ERR) htmlWriter.appendError(msg);
    else htmlWriter.appendWarning(msg);
}



void EyeDataAnalyzer::switchViews(qint32 view){
    for (qint32 i = 0; i < appViews.size(); i++){
        appViews[i]->setVisible(i == view);
    }
}

EyeDataAnalyzer::~EyeDataAnalyzer()
{
    delete ui;
}

void EyeDataAnalyzer::on_actionDataBase_Connection_triggered()
{
    //switchViews(VIEW_0_DATABASE_VIEW);
}

void EyeDataAnalyzer::on_actionProcessing_View_triggered()
{
    switchViews(VIEW_1_PROCESSING_VIEW);
}

void EyeDataAnalyzer::on_lwDirs_itemDoubleClicked(QListWidgetItem *item)
{
    currentDirectory = item->data(ROLE_DATA).toString();
    processDirectory();
}


void EyeDataAnalyzer::on_pushButton_clicked()
{
    if (currentDirectory.contains("/")){
        QStringList parts = currentDirectory.split("/");
        parts.removeLast();
        currentDirectory = parts.join("/");
        processDirectory();
    }
}

ConfigurationManager EyeDataAnalyzer::createProcessingConfiguration(bool *ok){
    ConfigurationManager processingParameters;
    *ok = true;
    if (!ui->pteConfig->toPlainText().isEmpty()){
        if (!processingParameters.loadConfiguration("",COMMON_TEXT_CODEC,ui->pteConfig->toPlainText())){
            logForProcessing.appendError("ERROR in parameters file: " +  processingParameters.getError());
            *ok = false;
            return processingParameters;
        }
    }
    processingParameters.addKeyValuePair(CONFIG_PATIENT_DIRECTORY,currentDirectory);
    return processingParameters;
}

void EyeDataAnalyzer::overWriteCurrentConfigurationWith(const ConfigurationManager &mng, bool addOnlyNonExistant){
    bool ok;
    ConfigurationManager processingParameters = createProcessingConfiguration(&ok);
    if (!ok) return;

    if (addOnlyNonExistant){
        QStringList keys = mng.getAllKeys();
        for (qint32 i = 0; i < keys.size(); i++){
            if (!processingParameters.containsKeyword(keys.at(i))){
                processingParameters.addKeyValuePair(keys.at(i),mng.getVariant(keys.at(i)));
            }
        }
    }
    else processingParameters.merge(mng);


    QStringList keys = processingParameters.getAllKeys();
    QString newText = "";
    for (qint32 i = 0; i < keys.size(); i++){
        newText = newText + keys.at(i) + " = " + processingParameters.getString(keys.at(i)) + ";\n";
    }
    ui->pteConfig->setPlainText(newText);
}

void EyeDataAnalyzer::on_pbGenerateReport_clicked()
{

    bool ok;
    ConfigurationManager processingParameters = createProcessingConfiguration(&ok);
    if (!ok) return;

    // Third step, processing the data.
    RawDataProcessor processor(this);
    htmlWriter.reset();
    connect(&processor,SIGNAL(appendMessage(QString,qint32)),this,SLOT(onProcessorMessage(QString,qint32)));
    processor.initialize(&processingParameters);
    logForProcessing.appendStandard("Processing started...");
    processor.run();
    QString outputFile = currentDirectory + "/output.html";
    htmlWriter.write(outputFile,"Data Analyizer - " + QString(PROGRAM_VERSION));
    logForProcessing.appendSuccess("Processing done: Output at: " + outputFile);
    logForProcessing.appendSuccess("Current Report is: : "  + processor.getReportFileOutput());
}

void EyeDataAnalyzer::on_pbDrawFixations_clicked()
{

    if (ui->lwDatFiles->count() == 0){
        logForProcessing.appendError("No files to process");
        return;
    }

    if (ui->lwDatFiles->currentRow() < 0){
        logForProcessing.appendError("No file is selected");
        return;
    }

    bool ok;
    ConfigurationManager processingParameters = createProcessingConfiguration(&ok);
    if (!ok) return;

    QString fileToProcess = ui->lwDatFiles->currentItem()->text();
    QString fixationHashName = "";
    if (fileToProcess.isEmpty()){
        logForProcessing.appendError("No seleted file: " + fileToProcess);
        return;
    }
    if (fileToProcess.startsWith(FILE_OUTPUT_READING))    {
        processingParameters.addKeyValuePair(CONFIG_FILE_READING,fileToProcess);
        fixationHashName = CONFIG_P_EXP_READING;
    }
    else if (fileToProcess.startsWith(FILE_OUTPUT_BINDING_BC)){
        processingParameters.addKeyValuePair(CONFIG_FILE_BIDING_BC,fileToProcess);
        fixationHashName = CONFIG_P_EXP_BIDING_BC;
    }
    else if (fileToProcess.startsWith(FILE_OUTPUT_BINDING_UC)){
        processingParameters.addKeyValuePair(CONFIG_FILE_BIDING_UC,fileToProcess);
        fixationHashName = CONFIG_P_EXP_BIDING_UC;
    }
    else{
        logForProcessing.appendError("Unrecognized file type: " + fileToProcess);
        return;
    }

    RawDataProcessor processor(this);
    htmlWriter.reset();
    connect(&processor,SIGNAL(appendMessage(QString,qint32)),this,SLOT(onProcessorMessage(QString,qint32)));
    processor.initialize(&processingParameters);
    logForProcessing.appendStandard("Processing started...");
    processor.run();
    FixationList fix = processor.getFixations().value(fixationHashName);

    // Draw fixations.
    FixationDrawer fdrawer;

    QFileInfo info(fileToProcess);
    QDir(currentDirectory).mkdir(info.baseName());
    QString outputDir =  currentDirectory + "/" + info.baseName();
    if (!QDir(outputDir).exists()){
        logForProcessing.appendError("Could not create output directory: " + outputDir);
        return;
    }

    if (!fdrawer.prepareToDrawFixations(fixationHashName,&processingParameters,fix.experimentDescription,outputDir)){
        logForProcessing.appendError("Error drawing fixation for experiment " + fixationHashName + ". Error is: " + fdrawer.getError());
        return;
    }
    if (!fdrawer.drawFixations(fix)){
        logForProcessing.appendError("Error drawing fixation for experiment " + fixationHashName + ". Error is: " + fdrawer.getError());
        return;
    }

    logForProcessing.appendSuccess("Fixations were draw in directory: " + outputDir);

}

void EyeDataAnalyzer::on_pgFrequencyAnalsis_clicked()
{
    if (ui->lwDatFiles->count() == 0){
        logForProcessing.appendError("No files to process");
        return;
    }

    if (ui->lwDatFiles->currentRow() < 0){
        logForProcessing.appendError("No file is selected");
        return;
    }

    QString fileToProcess = currentDirectory + "/" + ui->lwDatFiles->currentItem()->text();

    FreqAnalysis freqChecker;
    FreqAnalysis::FreqAnalysisResult fres;
    fres = freqChecker.analyzeFile(fileToProcess);

    QString freqReport;

    bool ok;
    ConfigurationManager processingParameters = createProcessingConfiguration(&ok);
    if (!ok) return;

    if (!fres.errorList.isEmpty()){
        freqReport = "FREQ ANALYSIS ERROR: \n   " + fres.errorList.join("\n   ");
    }
    else {

        FreqAnalysis::FreqCheckParameters fcp;
        fcp.fexpected                        = processingParameters.getReal(CONFIG_SAMPLE_FREQUENCY);
        fcp.periodMax                        = processingParameters.getReal(CONFIG_TOL_MAX_PERIOD_TOL);
        fcp.periodMin                        = processingParameters.getReal(CONFIG_TOL_MIN_PERIOD_TOL);
        fcp.maxAllowedFreqGlitchesPerTrial   = processingParameters.getReal(CONFIG_TOL_MAX_FGLITECHES_IN_TRIAL);
        fcp.maxAllowedPercentOfInvalidValues = processingParameters.getReal(CONFIG_TOL_MAX_PERCENT_OF_INVALID_VALUES);
        fcp.minNumberOfDataItems             = processingParameters.getReal(CONFIG_TOL_MIN_NUMBER_OF_DATA_ITEMS_IN_TRIAL);
        fcp.maxAllowedFailedTrials           = processingParameters.getReal(CONFIG_TOL_NUM_ALLOWED_FAILED_DATA_SETS);

        fres.analysisValid(fcp);


        freqReport = "FREQ ANALYSIS REPORT: Avg Frequency: " + QString::number(fres.averageFrequency) + "\n   ";
        freqReport = freqReport + fres.errorList.join("\n   ");
        freqReport = freqReport  + "\n   Individual Freq Errors:\n   " + fres.individualErrorList.join("\n   ");

    }

    // Saving the frequency log.
    QString freqLog = fileToProcess + ".flog";
    QFile file(freqLog);
    if (!file.open(QFile::WriteOnly)){
        logForProcessing.appendError("Could not open output file " + freqLog + " for writing");
        return;
    }

    QTextStream writer(&file);
    writer.setCodec(COMMON_TEXT_CODEC);
    writer << freqReport;
    file.close();

    logForProcessing.appendSuccess("Generated frequency log file at: " + freqLog);

}

void EyeDataAnalyzer::on_pteDefConfig_clicked()
{

    if (!QFile(FILE_DEFAULT_VALUES).exists()){
        overWriteCurrentConfigurationWith(defaultValues,false);
        overWriteCurrentConfigurationWith(defaultReportCompletionParameters,true);
    }
    else{
        ConfigurationManager config;
        if (!config.loadConfiguration(FILE_DEFAULT_VALUES,COMMON_TEXT_CODEC)){
            logForProcessing.appendError("Error loading default settings file: " + config.getError());
            return;
        }
        overWriteCurrentConfigurationWith(config,false);
    }
}


void EyeDataAnalyzer::on_pbSaveDefConf_clicked()
{
    bool ok;
    ConfigurationManager processingParameters = createProcessingConfiguration(&ok);
    if (!ok) return;

    QString toSave;
    QStringList keys = defaultValues.getAllKeys();
    for (qint32 i = 0; i < keys.size(); i++){
        toSave = toSave  + keys.at(i) + " = ";
        if (processingParameters.containsKeyword(keys.at(i))) toSave = toSave + processingParameters.getString(keys.at(i)) + ";\n";
        else toSave = toSave + defaultValues.getString(keys.at(i)) + ";\n";
    }

    QFile file(FILE_DEFAULT_VALUES);
    if (!file.open(QFile::WriteOnly)){
        logForProcessing.appendError("Could not open settings file for saving");
        return;
    }

    QTextStream writer(&file);
    writer.setCodec(COMMON_TEXT_CODEC);
    writer << toSave;
    file.close();

    logForProcessing.appendSuccess("Saved new default configuration");

}

void EyeDataAnalyzer::on_pbDATFiles_clicked()
{
    ConfigurationManager config;
    for (qint32 i = 0; i < ui->lwDatFiles->count(); i++){
        QString fileToProcess = ui->lwDatFiles->item(i)->text();
        if (fileToProcess.startsWith(FILE_OUTPUT_READING))    {
            config.addKeyValuePair(CONFIG_FILE_READING,fileToProcess);
        }
        else if (fileToProcess.startsWith(FILE_OUTPUT_BINDING_BC)){
            config.addKeyValuePair(CONFIG_FILE_BIDING_BC,fileToProcess);
        }
        else if (fileToProcess.startsWith(FILE_OUTPUT_BINDING_UC)){
            config.addKeyValuePair(CONFIG_FILE_BIDING_UC,fileToProcess);
        }
    }

    overWriteCurrentConfigurationWith(config,false);
}

void EyeDataAnalyzer::on_pbGenerateReport_2_clicked()
{

    if (ui->lwRepFiles->count() == 0){
        logForProcessing.appendError("No reports generated");
        return;
    }

    if (ui->lwRepFiles->currentRow() < 0){
        logForProcessing.appendError("No report selected");
        return;
    }

    logForProcessing.appendStandard("Generating PNG Report ...");

    bool ok;
    ConfigurationManager processingParameters = createProcessingConfiguration(&ok);
    if (!ok) return;

    QString selectedReport = ui->lwRepFiles->currentItem()->text();

    RepFileInfo repInfoOnDir;
    repInfoOnDir.setDirectory(currentDirectory);
    QVariantMap dataSet = repInfoOnDir.getRepData(selectedReport);
    if (dataSet.isEmpty()){
        logForProcessing.appendError("Could not load data on report file: " + selectedReport);
        return;
    }

    // Setting the image report name
    QFileInfo info(selectedReport);
    QString outputPath = currentDirectory + "/" + info.baseName() + ".png";
    processingParameters.addKeyValuePair(CONFIG_IMAGE_REPORT_PATH,outputPath);

    ImageReportDrawer reportDrawer;
    reportDrawer.drawReport(dataSet,&processingParameters);

    if (QFile(outputPath).exists()){
        logForProcessing.appendSuccess("Generated image report at: " + outputPath);
    }
    else{
        logForProcessing.appendError("Could not generate PNG report");
    }

}
