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

    QStringList filters;
    filters << QString(FILE_OUTPUT_READING) + "*.dat";
    filters << QString(FILE_OUTPUT_BINDING_BC) + "*.dat";
    filters << QString(FILE_OUTPUT_BINDING_UC) + "*.dat";
    filters << QString(FILE_OUTPUT_FIELDING) + "*.dat";
    QStringList files = dir.entryList(filters,QDir::Files,QDir::Time);

    ui->lwFilesInPatientDir->clear();
    lastFixations.clear();

    for (qint32 i = 0; i < files.size(); i++){
        QListWidgetItem *item = new QListWidgetItem(files.at(i),ui->lwFilesInPatientDir);
        item->setFlags(item->flags() | Qt::ItemIsUserCheckable);
        item->setCheckState(Qt::Unchecked);
    }

    // If patient data file exists, it is loaded.
    QFile patientDataFile(selected + "/" + FILE_PATIENT_INFO_FILE);
    if (patientDataFile.exists()){
        ConfigurationManager pdata;
        if (!pdata.loadConfiguration(patientDataFile.fileName(),COMMON_TEXT_CODEC)){
            log.appendError("Could not load patient file: " + pdata.getError());
            return;
        }
        else{
            ConfigurationManager::setValue(FILE_CONFIGURATION,COMMON_TEXT_CODEC,CONFIG_PATIENT_NAME,pdata.getString(CONFIG_PATIENT_NAME),&configuration);
            ConfigurationManager::setValue(FILE_CONFIGURATION,COMMON_TEXT_CODEC,CONFIG_PATIENT_AGE,pdata.getString(CONFIG_PATIENT_AGE),&configuration);
            ui->lePatientAge->setText(configuration.getString(CONFIG_PATIENT_AGE));
            ui->lePatientName->setText(configuration.getString(CONFIG_PATIENT_NAME));
        }
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
    // First step: Create Work directory.
    QString patientDir = ui->lePatientDir->text();
    QDir pdir(patientDir);
    if (!pdir.exists()){
        log.appendError("Patient dir does not exist " + patientDir);
        return;
    }

    // Second step: Gather test files to analyze.
    processedFiles.clear();
    QStringList files;
    for (qint32 i = 0; i < ui->lwFilesInPatientDir->count(); i++){
        QListWidgetItem *item = ui->lwFilesInPatientDir->item(i);
        if (item->checkState() == Qt::Checked){

            QString fname = item->text();
            QString fullPath = patientDir + "/" + fname;
            files << fullPath;

            if (fname.contains(FILE_OUTPUT_BINDING_BC)){
                if (processedFiles.contains(FILE_OUTPUT_BINDING_BC)){
                    log.appendError("Cannot selecte more than one binding BC file");
                    return;
                }
                else{
                    QFileInfo info(fullPath);
                    processedFiles[FILE_OUTPUT_BINDING_BC] = info.baseName();
                }
            }

            else if (fname.contains(FILE_OUTPUT_BINDING_UC)){
                if (processedFiles.contains(FILE_OUTPUT_BINDING_UC)){
                    log.appendError("Cannot selecte more than one binding UC file");
                    return;
                }
                else{
                    QFileInfo info(fullPath);
                    processedFiles[FILE_OUTPUT_BINDING_UC] = info.baseName();
                }
            }

            else if (fname.contains(FILE_OUTPUT_READING)){
                if (processedFiles.contains(FILE_OUTPUT_READING)){
                    log.appendError("Cannot selecte more than one reading file");
                    return;
                }
                else{
                    QFileInfo info(fullPath);
                    processedFiles[FILE_OUTPUT_READING] = info.baseName();
                }
            }

            else if (fname.contains(FILE_OUTPUT_FIELDING)){
                if (processedFiles.contains(FILE_OUTPUT_FIELDING)){
                    log.appendError("Cannot selecte more than one fielding file");
                    return;
                }
                else{
                    QFileInfo info(fullPath);
                    processedFiles[FILE_OUTPUT_FIELDING] = info.baseName();
                }
            }

        }
    }

    if (files.isEmpty()){
        log.appendError("No files were selected for processing");
        return;
    }

    // Third step, processing the data.
    RawDataProcessor processor(this);
    htmlWriter.reset();
    connect(&processor,SIGNAL(appendMessage(QString,qint32)),this,SLOT(onProcessorMessage(QString,qint32)));
    qWarning() << "Procesando con " << configuration.getString(CONFIG_MOVING_WINDOW_DISP);
    processor.initialize(&configuration,files);
    log.appendStandard("Processing started...");
    processor.run();
    QString outputFile = patientDir + "/output.html";
    htmlWriter.write(outputFile,"Data Analyizer - " + QString(PROGRAM_VERSION));
    log.appendSuccess("Processing done: Output at: " + outputFile);
    lastFixations = processor.getFixations();

}

void EyeDataAnalyzer::onProcessorMessage(const QString &msg, qint32 type){
    if (type == MSG_TYPE_STD) htmlWriter.appendStandard(msg);
    else if (type == MSG_TYPE_SUCC) htmlWriter.appendSuccess(msg);
    else if (type == MSG_TYPE_ERR) htmlWriter.appendError(msg);
    else htmlWriter.appendWarning(msg);
}

void EyeDataAnalyzer::on_pbDrawFixations_clicked()
{

    if (!lastFixations.isEmpty()){

        QString baseDir = ui->lePatientDir->text();
        QDir pdir(baseDir);
        QStringList keys = lastFixations.keys();

        for (qint32 i = 0; i < keys.size(); i++){

            // Make the output directory
            pdir.mkdir(processedFiles.value(keys.at(i)));
            QString workDir = baseDir + "/" + processedFiles.value(keys.at(i));
            if (!QDir(workDir).exists()){
                log.appendError("Could not create output image directory: " + workDir);
                return;
            }

            // Draw fixations.
            FixationDrawer fdrawer;
            fdrawer.prepareToDrawFixations(keys.at(i),&configuration,lastFixations.value(keys.at(i)).experimentDescription,workDir);
            if (!fdrawer.drawFixations(lastFixations.value(keys.at(i)))){
                log.appendError("Error drawing fixation for experiment " + keys.at(i) + ". Error is: " + fdrawer.getError());
                return;
            }

        }
    }
    else{
        log.appendError("Data must be analyzed before fixations can be drawn");
    }
}

void EyeDataAnalyzer::on_pbGeneratePNG_clicked()
{

    QString fileName = getNewestFile(ui->lePatientDir->text(),FILE_REPORT_NAME,"rep");
    if (fileName.isEmpty()){
        log.appendError("No .rep file found in patient dir. Try running a data analysis first");
        return;
    }

    ConfigurationManager dataSet;
    if (!dataSet.loadConfiguration(fileName,COMMON_TEXT_CODEC)){
        log.appendError("Error loading rep file: " + dataSet.getError());
        return;
    }

    log.appendStandard("Generating PNG Report ...");
    ImageReportDrawer reportDrawer;
    configuration.addKeyValuePair(CONFIG_PATIENT_REPORT_DIR,ui->lePatientDir->text());
    reportDrawer.drawReport(&dataSet,&configuration);
    QString output = configuration.getString(CONFIG_IMAGE_REPORT_PATH);
    if (QFile(output).exists()){
        log.appendSuccess("Report successfully generated!");
    }
    else{
        log.appendError("Could not generate PNG report");
    }

}
