#include "eyedataanalyzer.h"
#include "ui_eyedataanalyzer.h"

EyeDataAnalyzer::EyeDataAnalyzer(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::EyeDataAnalyzer)
{
    ui->setupUi(this);
    log.setLogInterface(ui->pteLog);
    this->setWindowTitle(QString(PROGRAM_NAME) + " - " + QString(PROGRAM_VERSION));
    if (!configuration.loadConfiguration(FILE_CONFIGURATION,COMMON_TEXT_CODEC)){
        log.appendError("ERROR Loading configuration: " + configuration.getError());
    }
    fillUi();
    enableControlButtons(false);

}


//******************************************************************* AUX Functions *************************************************************

void EyeDataAnalyzer::listDatFilesInPatientDirectory(){

    QString selected = ui->lePatientDir->text();

    // Listing all possible data files
    QDir dir(selected);

    if (!dir.exists()){
        log.appendWarning("Patient directory does not exist: " + selected);
        ui->lePatientDir->setText("");
        ConfigurationManager::setValue(FILE_CONFIGURATION,COMMON_TEXT_CODEC,CONFIG_PATIENT_DIRECTORY,"",&configuration);
        return;
    }


    infoInDir.setDatDirectory(selected,true);

    ui->lwReportsThatCanBeGenerated->clear();
    lastFixations.clear();

    infoInDir.prepareToInterateOverPendingReportFileSets();
    while (true){
        QStringList fileSet = infoInDir.nextPendingReportFileSet();
        if (fileSet.isEmpty()) break;
        // The first value is the report name
        QListWidgetItem *item = new QListWidgetItem(fileSet.first(),ui->lwReportsThatCanBeGenerated);
        item->setData(DATA_ROLE,fileSet);
    }
}


QString EyeDataAnalyzer::validNumber(const QString &str, bool positive, bool isReal){
    if (isReal){
        bool ok = true;
        qreal value = str.toDouble(&ok);
        if (!ok){
            return str + " is not a valid real number";
        }
        if (positive && (value < 0)){
            return str + " is not a positive real number";
        }
    }
    else{
        bool ok = true;
        qint64 value = str.toLongLong(&ok);
        if (!ok){
            return str + " is not a valid integer";
        }
        if (positive && (value < 0)){
            return str + " is not a positive integer";
        }
    }
    return "";
}

void EyeDataAnalyzer::validateAndSave(QLineEdit *le, const QString &parameter, bool positive, bool real){
    QString e = validNumber(le->text(),positive,real);
    if (!e.isEmpty()){
        log.appendError(e);
    }
    else{
        ConfigurationManager::setValue(FILE_CONFIGURATION,COMMON_TEXT_CODEC,parameter,le->text(),&configuration);
    }
}

void EyeDataAnalyzer::fillUi(){

    // Fill with default values all missing configuration
    QStringList params;
    params << CONFIG_DAT_TIME_FILTER_THRESHOLD << CONFIG_MIN_FIXATION_LENGTH << CONFIG_DISTANCE_2_MONITOR << CONFIG_MOVING_WINDOW_DISP
           << CONFIG_SAMPLE_FREQUENCY << CONFIG_LATENCY_ESCAPE_RAD << CONFIG_MARGIN_TARGET_HIT << CONFIG_XPX_2_MM << CONFIG_YPX_2_MM
           << CONFIG_PATIENT_DIRECTORY << CONFIG_REPORT_NO_LOGO << CONFIG_REPORT_LANGUAGE
           << CONFIG_DOCTOR_NAME << CONFIG_PATIENT_AGE << CONFIG_PATIENT_NAME;
    QStringList values;
    values << "0" << "50" << "60" << "30"
           << "150" << "80" << "10" << "0.25" << "0.25"
           << "" << "false" << CONFIG_P_LANG_EN
           << "Miles Genius" << "0" << "Some Guy";

    for (qint32 i = 0; i < params.size(); i++){
        if (!configuration.containsKeyword(params.at(i)))
            ConfigurationManager::setValue(FILE_CONFIGURATION,COMMON_TEXT_CODEC,params.at(i),values.at(i),&configuration);
    }

    // Fill the UI.
    ui->leDistanceToMonitor->setText(configuration.getString(CONFIG_DISTANCE_2_MONITOR));
    ui->leLatencyEscapeRadious->setText(configuration.getString(CONFIG_LATENCY_ESCAPE_RAD));
    ui->leMarginTargetHit->setText(configuration.getString(CONFIG_MARGIN_TARGET_HIT));
    ui->leMinimumFixationLength->setText(configuration.getString(CONFIG_MIN_FIXATION_LENGTH));
    ui->leMovingWindowMaxDispersion->setText(configuration.getString(CONFIG_MOVING_WINDOW_DISP));
    ui->leSampleFrequency->setText(configuration.getString(CONFIG_SAMPLE_FREQUENCY));
    ui->leTimeFilterThreshold->setText(configuration.getString(CONFIG_DAT_TIME_FILTER_THRESHOLD));
    ui->leXToMM->setText(configuration.getString(CONFIG_XPX_2_MM));
    ui->leYToMM->setText(configuration.getString(CONFIG_YPX_2_MM));

    ui->lePatientAge->setText(configuration.getString(CONFIG_PATIENT_AGE));
    ui->lePatientName->setText(configuration.getString(CONFIG_PATIENT_NAME));
    ui->leDoctorsName->setText(configuration.getString(CONFIG_DOCTOR_NAME));

    ui->lePatientDir->setText(configuration.getString(CONFIG_PATIENT_DIRECTORY));
    if (!ui->lePatientDir->text().isEmpty()) listDatFilesInPatientDirectory();

    ui->cbUseLogo->setChecked(!configuration.getBool(CONFIG_REPORT_NO_LOGO));

    for (qint32 i = 0; i < ui->comboLang->count(); i++){
        if (ui->comboLang->itemText(i) == configuration.getString(CONFIG_REPORT_LANGUAGE)){
            ui->comboLang->setCurrentIndex(i);
            break;
        }
    }

    ui->comboEyeSelection->setCurrentIndex(configuration.getInt(CONFIG_VALID_EYE));

}

EyeDataAnalyzer::~EyeDataAnalyzer()
{
    delete ui;
}


//******************************************************************* Basic Slot Fucntions *************************************************************


void EyeDataAnalyzer::on_pbBrowsePatientDir_clicked()
{

    QString selected = QFileDialog::getExistingDirectory(this,"Select patient directory",configuration.getString(CONFIG_PATIENT_DIRECTORY));
    if (selected.isEmpty()) return;

    // The path value should be visible
    ui->lePatientDir->setText(selected);

    // Saving the selection
    ConfigurationManager::setValue(FILE_CONFIGURATION,COMMON_TEXT_CODEC,CONFIG_PATIENT_DIRECTORY,selected,&configuration);

    listDatFilesInPatientDirectory();

}

void EyeDataAnalyzer::on_leMovingWindowMaxDispersion_editingFinished()
{
    validateAndSave(ui->leMovingWindowMaxDispersion,CONFIG_MOVING_WINDOW_DISP,true,false);
}

void EyeDataAnalyzer::on_leTimeFilterThreshold_editingFinished()
{
    validateAndSave(ui->leTimeFilterThreshold,CONFIG_DAT_TIME_FILTER_THRESHOLD,true,false);
}

void EyeDataAnalyzer::on_leMinimumFixationLength_editingFinished()
{
    validateAndSave(ui->leMinimumFixationLength,CONFIG_MIN_FIXATION_LENGTH,true,false);
}

void EyeDataAnalyzer::on_leSampleFrequency_editingFinished()
{
    validateAndSave(ui->leSampleFrequency,CONFIG_SAMPLE_FREQUENCY,true,true);
}

void EyeDataAnalyzer::on_leLatencyEscapeRadious_editingFinished()
{
    validateAndSave(ui->leLatencyEscapeRadious,CONFIG_LATENCY_ESCAPE_RAD,true,false);
}

void EyeDataAnalyzer::on_leDistanceToMonitor_editingFinished()
{
    validateAndSave(ui->leDistanceToMonitor,CONFIG_DISTANCE_2_MONITOR,true,false);
}

void EyeDataAnalyzer::on_leMarginTargetHit_editingFinished()
{
    validateAndSave(ui->leMarginTargetHit,CONFIG_MARGIN_TARGET_HIT,true,false);
}

void EyeDataAnalyzer::on_leXToMM_editingFinished()
{
    validateAndSave(ui->leXToMM,CONFIG_XPX_2_MM,true,false);
}

void EyeDataAnalyzer::on_leYToMM_editingFinished()
{
    validateAndSave(ui->leYToMM,CONFIG_YPX_2_MM,true,false);
}

void EyeDataAnalyzer::on_comboEyeSelection_currentIndexChanged(int index)
{
    ConfigurationManager::setValue(FILE_CONFIGURATION,COMMON_TEXT_CODEC,CONFIG_VALID_EYE,QString::number(index),&configuration);
}

void EyeDataAnalyzer::on_leDoctorsName_editingFinished()
{
    ConfigurationManager::setValue(FILE_CONFIGURATION,COMMON_TEXT_CODEC,CONFIG_DOCTOR_NAME,ui->leDoctorsName->text(),&configuration);
}

void EyeDataAnalyzer::on_lePatientName_editingFinished()
{
    ConfigurationManager::setValue(FILE_CONFIGURATION,COMMON_TEXT_CODEC,CONFIG_PATIENT_NAME,ui->lePatientName->text(),&configuration);
}

void EyeDataAnalyzer::on_lePatientAge_editingFinished()
{
    ConfigurationManager::setValue(FILE_CONFIGURATION,COMMON_TEXT_CODEC,CONFIG_PATIENT_AGE,ui->lePatientAge->text(),&configuration);
}

void EyeDataAnalyzer::on_comboLang_currentIndexChanged(const QString &arg1)
{
    ConfigurationManager::setValue(FILE_CONFIGURATION,COMMON_TEXT_CODEC,CONFIG_REPORT_LANGUAGE,arg1,&configuration);
}

void EyeDataAnalyzer::on_cbUseLogo_stateChanged(int arg1)
{
    Q_UNUSED(arg1);
    QString p = ui->cbUseLogo->isChecked() ? "false" : "true";
    ConfigurationManager::setValue(FILE_CONFIGURATION,COMMON_TEXT_CODEC,CONFIG_REPORT_NO_LOGO,p,&configuration);
}


void EyeDataAnalyzer::on_pbAnalyzeData_clicked()
{
    // First step: Check the patient dir.
    QString patientDir = ui->lePatientDir->text();
    QDir pdir(patientDir);
    if (!pdir.exists()){
        log.appendError("Patient dir does not exist " + patientDir);
        return;
    }

    // Getting the file set
    QListWidgetItem *selectedItem = ui->lwReportsThatCanBeGenerated->currentItem();
    if (selectedItem == nullptr){
        log.appendError("A report must be selected");
        return;
    }

    configuration.removeKey(CONFIG_FILE_BIDING_BC);
    configuration.removeKey(CONFIG_FILE_BIDING_UC);
    configuration.removeKey(CONFIG_FILE_READING);
    configuration.removeKey(CONFIG_FILE_FIELDING);

    QStringList files = selectedItem->data(DATA_ROLE).toStringList();

    for (qint32 i = 0; i < files.size(); i++){
        QString fname = files.at(i);
        if (fname.contains(FILE_OUTPUT_BINDING_BC)){
            configuration.addKeyValuePair(CONFIG_FILE_BIDING_BC,fname);
        }

        else if (fname.contains(FILE_OUTPUT_BINDING_UC)){
            configuration.addKeyValuePair(CONFIG_FILE_BIDING_UC,fname);
        }

        else if (fname.contains(FILE_OUTPUT_READING)){
            configuration.addKeyValuePair(CONFIG_FILE_READING,fname);

        }

        else if (fname.contains(FILE_OUTPUT_FIELDING)){
            configuration.addKeyValuePair(CONFIG_FILE_FIELDING,fname);
        }

    }

    // Third step, processing the data.
    RawDataProcessor processor(this);
    htmlWriter.reset();
    connect(&processor,SIGNAL(appendMessage(QString,qint32)),this,SLOT(onProcessorMessage(QString,qint32)));
    processor.initialize(&configuration);
    log.appendStandard("Processing started...");
    processor.run();
    QString outputFile = patientDir + "/output.html";
    htmlWriter.write(outputFile,"Data Analyizer - " + QString(PROGRAM_VERSION));
    log.appendSuccess("Processing done: Output at: " + outputFile);
    log.appendSuccess("Current Report is: : "  + processor.getReportFileOutput());
    configuration.addKeyValuePair(CONFIG_REPORT_PATH,processor.getReportFileOutput());
    lastFixations = processor.getFixations();
    enableControlButtons(true);

}

void EyeDataAnalyzer::onProcessorMessage(const QString &msg, qint32 type){
    if (type == MSG_TYPE_STD) htmlWriter.appendStandard(msg);
    else if (type == MSG_TYPE_SUCC) htmlWriter.appendSuccess(msg);
    else if (type == MSG_TYPE_ERR) htmlWriter.appendError(msg);
    else htmlWriter.appendWarning(msg);
}

void EyeDataAnalyzer::on_pbDrawFixations_clicked()
{

    // Adding the Target size to the configuration
    configuration.addKeyValuePair(CONFIG_BINDING_TARGET_SMALL,(ui->comboTargetSize->currentIndex() == 1));
    QStringList list;
    QListWidgetItem *selected = ui->lwReportsThatCanBeGenerated->currentItem();
    if (selected == nullptr) return;
    list = selected->data(DATA_ROLE).toStringList();

    if (!lastFixations.isEmpty()){

        QString baseDir = ui->lePatientDir->text();
        QDir pdir(baseDir);
        QStringList keys = lastFixations.keys();

        // Seeing which key belongs to whcih file.
        for (qint32 i = 0; i < keys.size(); i++){

            QString currentFile;
            for (qint32 j = 1; j < list.size(); j++){
                if (list.at(j).startsWith(keys.at(i))){
                    currentFile = list.at(j);
                    break;
                }
            }

            // Make the output directory
            QFileInfo info(currentFile);
            pdir.mkdir(info.baseName());
            QString workDir = baseDir + "/" + info.baseName();
            if (!QDir(workDir).exists()){
                log.appendError("Could not create output image directory: " + workDir);
                return;
            }

            // Draw fixations.
            FixationDrawer fdrawer;
            if (!fdrawer.prepareToDrawFixations(keys.at(i),&configuration,lastFixations.value(keys.at(i)).experimentDescription,workDir)){
                log.appendError("Error drawing fixation for experiment " + keys.at(i) + ". Error is: " + fdrawer.getError());
                return;
            }
            if (!fdrawer.drawFixations(lastFixations.value(keys.at(i)))){
                log.appendError("Error drawing fixation for experiment " + keys.at(i) + ". Error is: " + fdrawer.getError());
                return;
            }

        }

        log.appendSuccess("Fixations Drawn!!!");
    }
    else{
        log.appendError("Data must be analyzed before fixations can be drawn");
    }
}

void EyeDataAnalyzer::on_pbGeneratePNG_clicked()
{
    QString fileName = configuration.getString(CONFIG_REPORT_PATH);
    if (fileName.isEmpty()){
        log.appendError("No report file set. Need to run a analysis first");
        return;
    }
    log.appendStandard("Generating PNG Report ...");

    RepFileInfo repInfoOnDir;
    repInfoOnDir.setDirectory(configuration.getString(CONFIG_PATIENT_DIRECTORY));
    QVariantMap dataSet = repInfoOnDir.getRepData(fileName);
    if (dataSet.isEmpty()){
        log.appendError("Could not load data on report file: " + fileName);
        return;
    }

    // Setting the image report name
    QFileInfo info(fileName);
    QString outputPath = info.absolutePath() + "/" + info.baseName() + ".png";
    configuration.addKeyValuePair(CONFIG_IMAGE_REPORT_PATH,outputPath);

    ImageReportDrawer reportDrawer;
    reportDrawer.drawReport(dataSet,&configuration);

    if (QFile(outputPath).exists()){
        log.appendSuccess("Generated image report at: " + outputPath);
    }
    else{
        log.appendError("Could not generate PNG report");
    }
}

void EyeDataAnalyzer::on_lwReportsThatCanBeGenerated_itemClicked(QListWidgetItem *item)
{
    ui->lwFilesInReport->clear();
    QStringList files = item->data(DATA_ROLE).toStringList();

    for (qint32 i = 1; i < files.size(); i++){
        ui->lwFilesInReport->addItem(new QListWidgetItem(files.at(i)));
    }

    enableControlButtons(false);
}

void EyeDataAnalyzer::enableControlButtons(bool enable){
    ui->pbDrawFixations->setEnabled(enable);
    ui->pbGeneratePNG->setEnabled(enable);
}
