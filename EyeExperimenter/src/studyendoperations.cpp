#include "studyendoperations.h"


StudyEndOperations::StudyEndOperations() {

}

bool StudyEndOperations::wasOperationSuccssful() const {
    return operationsSuccess;
}

void StudyEndOperations::setFileListToProcess(const QStringList &fileList){

    filesToProcessIDX.clear();
    filesToProcessJSON.clear();

    for (qint32 i = 0; i < fileList.size(); i++){
        QString jsonFile = fileList.at(i);

        filesToProcessJSON << jsonFile;

        QFileInfo info(jsonFile);
        QString directory   = info.absolutePath();
        QString basename    = info.baseName();

        filesToProcessIDX << directory + "/" + basename + ".idx";

    }

}

void StudyEndOperations::setFilesToProcessFromViewMindDataDirectory(){

    QDir vmetdata(Globals::Paths::WORK_DIRECTORY);

    // We now scan for directories inside the work directory.
    QStringList listOfDirectories = vmetdata.entryList(QStringList(),QDir::Dirs | QDir::NoDotAndDotDot);

    // We now scan each directory and add it to the path.
    QStringList filters;
    filters << "*.json";

    QStringList jsonFiles;

    for (qint32 i = 0; i < listOfDirectories.size(); i++){

        QString path = vmetdata.absolutePath() + "/" + listOfDirectories.at(i);
        QDir subjectDir(path);
        QStringList studyFiles = subjectDir.entryList(filters,QDir::Files);

        for (qint32 j = 0; j < studyFiles.size(); j++){
            jsonFiles << path + "/" + studyFiles.at(j);
            //qDebug() << jsonFiles.last();
        }

    }

    this->setFileListToProcess(jsonFiles);

}

void StudyEndOperations::setFileToProcess(const QString &file, const QString &idx_file){
    filesToProcessIDX.clear();
    filesToProcessJSON.clear();
    filesToProcessIDX << idx_file;
    filesToProcessJSON << file;
}

void StudyEndOperations::run(){

    for (qint32 i = 0; i < filesToProcessJSON.size(); i++){
        operationsSuccess = false;
        idxFile = filesToProcessIDX.at(i);
        dataFile = filesToProcessJSON.at(i);
        doStudyFileProcessing();
    }

}

void StudyEndOperations::doStudyFileProcessing(){

    // First we load the file and make sure it's ok.
    if (!vmdc.loadFromJSONFile(dataFile)){
        StaticThreadLogger::error("StudyEndOperations::run","Could not load the raw data file: '" + dataFile + "'. Reason: " + vmdc.getError());
        return;
    }

    QVariantMap pp = vmdc.getProcessingParameters();
    qreal sampling_frequency = pp.value(VMDC::ProcessingParameter::SAMPLE_FREQUENCY).toReal();
    sampling_period = 1000.0/sampling_frequency; // In milliseconds.

    // Now we need to know how to compute the QC Index for each study in the file. All studies need to approve individually.
    // If any of them fail then the QC Index should be set to bad.

    fileQCIndex = 101;
    fileQCPass  = true;

    QStringList studyList = vmdc.getStudies();
    qreal ok_threshold = vmdc.getQCParameters().value(VMDC::QCGlobalParameters::DQI_THRESHOLD).toReal();

    // Making sure QC is cleared.
    vmdc.setQCStudyStructClear();

    for (qint32 i = 0; i < studyList.size(); i++){

        QString study = studyList.at(i);

        qreal qci;

        //qDebug() << "Study Code" << study;

        // Get study duration returns -1 if the field does not exist
        if (vmdc.getStudyDuration(study) > 0){
            // Current study.
            //qDebug() << "Computing QCI";
            qci = computeQCI(study);
        }
        else {
            // Now it depends on whether it's a 3D study or a 2D one.
            if (vmdc.isStudy3D(study)){
                //qDebug() << "3D Legacy";
                QVariantMap studyData3D = vmdc.get3DStudyData(study,VMDC::StudyField::STUDY_DATA).toMap();
                QVariantList timevector = studyData3D.value(VMDC::Study3DDataFields::TIME_VECTOR).toList();
                qci = computeQCIOnLegacy3DStudy(timevector);
            }
            else {
                //qDebug() << "2D Legacy";
                QVariantList list = vmdc.getStudyTrialList(study);
                qci = computeQCIOnLegacy2DStudy(list);
            }
        }

        bool  qci_ok = (qci >= ok_threshold);

        // We set the quality control to the actual file.
        vmdc.setQCValue(study,VMDC::QCFields::DQI,qci);
        vmdc.setQCValue(study,VMDC::QCFields::DQI_OK,qci_ok);

        // Each file is represented by a single row in the Upload Studies Table.
        // Hence if a file has multiple studies the QC Index is the index of the lowest QC and if that is not passing, then the files does not pass.
        fileQCIndex  = qMin(qci,fileQCIndex);
        fileQCPass   = fileQCPass && qci_ok;

    }

    // We now need to save to both the JSON file and ICX files.
    if (!vmdc.saveJSONFile(dataFile)){
        StaticThreadLogger::error("StudyEndOperations::run","Failed is saving JSON data with QC parameters to file '" + dataFile + "'");
        return;
    }

    vmdc.clearFieldsForIndexFileCreation();
    if (!vmdc.saveJSONFile(idxFile)){
        StaticThreadLogger::error("StudyEndOperations::run","Failed is saving JSON data with QC parameters to file '" + idxFile + "'");
        return;
    }

    // The next steps are done ONLY if the study is finalized.
    if (vmdc.getMetadaStatus() == VMDC::StatusType::FINALIZED) {

        // Now we generate the compressed file.
        if (!createTarFileAndCleanup()) return;

        // Generate the Local DB Entry.
        if (!createQCIStudyFile()) return;

    }

    operationsSuccess = true;


}


qreal StudyEndOperations::computeQCI(const QString &study) {

    qreal timeWithNoDataPerTrial = 0;
    qreal numberOfTrials = 0;
    qreal numberOfDataPointsObatained = 0;

    qreal studyDuration = vmdc.getStudyDuration(study);
    qreal pauseDuration = vmdc.getStudyPauseDuration(study);

    if ((study == VMDC::Study::BINDING_BC) || (study == VMDC::Study::BINDING_UC)) {

        QVariantList trials = vmdc.getStudyTrialList(study);
        numberOfDataPointsObatained = computeNumberOfDataPointsIn2DStudy(trials);
        numberOfTrials = trials.size();
        timeWithNoDataPerTrial = StudyConstTimes::Binding::TIME_FINISH
                + StudyConstTimes::Binding::TIME_START_CROSS
                + StudyConstTimes::Binding::TIME_WHITE_TRANSITION;

    }
    else if (study == VMDC::Study::GONOGO){

        QVariantList trials = vmdc.getStudyTrialList(study);
        numberOfDataPointsObatained = computeNumberOfDataPointsIn2DStudy(trials);
        numberOfTrials = trials.size();
        timeWithNoDataPerTrial = StudyConstTimes::GoNoGo::TIME_CROSS;

    }
    else if ( (study == VMDC::Study::GONOGO_SPHERE) || (study == VMDC::Study::PASSBALL) ){


        QVariant temp = vmdc.get3DStudyData(study,VMDC::Study3DDataFields::TIME_VECTOR);
        numberOfDataPointsObatained = temp.toList().size();
        numberOfTrials = 1;
        timeWithNoDataPerTrial = 0;


    }
    else if (study == VMDC::Study::NBACK) {

        QVariantList trials = vmdc.getStudyTrialList(study);
        QVariantMap config = vmdc.getStudyConfiguration(study);
        numberOfDataPointsObatained = computeNumberOfDataPointsIn2DStudy(trials);
        numberOfTrials = trials.size();
        // Nback studies are not collecting data ONLY when transition from one trial to the next.
        timeWithNoDataPerTrial = config.value(VMDC::StudyParameter::NBACK_TRANSITION).toReal() + pauseDuration/numberOfTrials;

    }

    else if (study == VMDC::Study::NBACKVS) {

        QVariantList trials = vmdc.getStudyTrialList(study);
        numberOfDataPointsObatained = computeNumberOfDataPointsIn2DStudy(trials);
        numberOfTrials = trials.size();
        // Nback studies are not collecting data ONLY when transition from one trial to the next.
        timeWithNoDataPerTrial = StudyConstTimes::NBack::TRANSITION_TIME + pauseDuration/numberOfTrials;

    }

    else {
        StaticThreadLogger::error("StudyEndOperations::computeQCI","Found unexpected study type in data file '" + dataFile + "' of  '" + study + "'. Don't know how to compute time with no acquisition");
        return -1;
    }

    // Now we compute the final quality control index as the ratio of expected number of points versus the computed/obtained number of points.
    qreal estimatedDataGatheringTime = studyDuration - timeWithNoDataPerTrial*numberOfTrials;
    qreal expectedNumberOfDataPoints = estimatedDataGatheringTime/sampling_period;
    qreal qci = qMin(numberOfDataPointsObatained*100.0/expectedNumberOfDataPoints,100.0);

//    qDebug() << "Study duration" << studyDuration << ". Time with No Data PerTrial" << timeWithNoDataPerTrial << ". Number of Trials" << numberOfTrials;
//    qDebug() << "Expected Data Gathering Time" << estimatedDataGatheringTime << "Expected Number of Data Points" << expectedNumberOfDataPoints;
//    qDebug() << "Number of points obtained" << numberOfDataPointsObatained << "QCI" << qci;

    return qci;

}

bool StudyEndOperations::createTarFileAndCleanup() {

    QFileInfo info(dataFile);

    QString directory   = info.absolutePath();
    QString basename    = info.baseName();

    QString localJSON   = basename + ".json";
    QString localIDX    = basename + ".idx";
    compressedFileName  = basename + ".zip";
    qciFile             = basename + ".qci";
    qciFile             = directory + "/"  + qciFile;


    QStringList arguments;
    arguments << "-c";
    arguments << "-z";
    arguments << "-f";
    arguments << compressedFileName;
    arguments << localJSON;
    arguments << localIDX;

    //qDebug() << "jsonFile" << jsonFile << "zip" << zipfile;

    QProcess tar;
    tar.setWorkingDirectory(directory);
    tar.start(Globals::Paths::TAR_EXE,arguments);
    tar.waitForFinished();

    qint32 exit_code = tar.exitCode();

    compressedFileName = directory + "/" + compressedFileName;

    if (!QFile(compressedFileName).exists()){
        StaticThreadLogger::error("StudyEndOperations::createTarFileAndCleanup","Could not zip the input file " + dataFile + ". Exit code for TAR.exe is: " + QString::number(exit_code));
        return false;
    }

    // If the file exists. We delete the .json and .idx files.
    localJSON = directory + "/" + localJSON;
    localIDX = directory + "/" + localIDX;
    bool ans = QFile::remove(localJSON);
    //qDebug() << "Removed file" << localJSON << ". Result" << ans;
    ans = QFile::remove(localIDX);
    //qDebug() << "Removed file" << localIDX << ". Result" << ans;
    return true;

}

bool StudyEndOperations::createQCIStudyFile() {

    QVariantMap dbEvaluationEntry;


    dbEvaluationEntry.clear();

    QStringList dateData = vmdc.getMetaDataDateTime();

    dbEvaluationEntry[Globals::QCIFields::DATE] = dateData.first();
    dbEvaluationEntry[Globals::QCIFields::DATE_ORDERCODE]  = dateData.last();
    dbEvaluationEntry[Globals::QCIFields::EVALUATOR] =
    dbEvaluationEntry[Globals::QCIFields::PASS]  = fileQCPass;
    dbEvaluationEntry[Globals::QCIFields::INDEX] = fileQCIndex;

    QVariantMap medic = vmdc.getApplicationUserData(VMDC::AppUserType::MEDIC);
    QVariantMap evaluator = vmdc.getApplicationUserData(VMDC::AppUserType::EVALUATOR);

    dbEvaluationEntry[Globals::QCIFields::MEDIC] = medic.value(VMDC::AppUserField::NAME).toString() + " " + medic.value(VMDC::AppUserField::LASTNAME).toString();
    dbEvaluationEntry[Globals::QCIFields::EVALUATOR] = evaluator.value(VMDC::AppUserField::EMAIL).toString();

    QStringList studylist = vmdc.getStudies();
    QString study_type;
    if (studylist.size() > 1){
        study_type = VMDC::MultiPartStudyBaseName::BINDING;
    }
    else {
        study_type = studylist.first();
    }

    dbEvaluationEntry[Globals::QCIFields::STUDY_TYPE] = study_type;

    QFile file (qciFile);
    if (!file.open(QFile::WriteOnly)){
        StaticThreadLogger::error("StudyEndOperations::createQCIStudyFile","Could not open QCI file '" + qciFile + "' for writing");
        return false;
    }

    // Converting to a compact JSON Array
    QJsonDocument json = QJsonDocument::fromVariant(dbEvaluationEntry);
    QByteArray data_to_store = json.toJson(QJsonDocument::Indented);

    // Storing JSON Array in the QCI file.
    QTextStream writer(&file);
    writer << data_to_store;
    file.close();

    return true;

}

qreal StudyEndOperations::computeQCIOnLegacy2DStudy(const QVariantList &trials) const {

    qreal pointsInTrial = 0;
    qreal estimated_gather_time = 0;
    qreal expected_number_of_points = 0;

    // Now that we have how many points we should get. We count how many points we have.
    for (qint32 i = 0; i < trials.size(); i++){

        QVariantMap trial = trials.at(i).toMap();
        QVariantMap data = trial.value(VMDC::TrialField::DATA).toMap();
        QStringList dataset_names = data.keys();

        for (qint32 j = 0; j < dataset_names.size(); j++){

            QVariantMap dataSet = data.value(dataset_names.at(j)).toMap();

            QVariantList rawdata = dataSet.value(VMDC::DataSetField::RAW_DATA).toList();

            pointsInTrial = pointsInTrial + rawdata.size();

            for (qint32 k = 0; k < rawdata.size(); k++){

                if (rawdata.size() < 2) continue;
                qreal time_diff = rawdata.last().toMap().value(VMDC::DataVectorField::TIMESTAMP).toReal() - rawdata.first().toMap().value(VMDC::DataVectorField::TIMESTAMP).toReal();
                estimated_gather_time = estimated_gather_time + time_diff;

            }

        }

    }

    // Now that we have an estimate of the study length, we do the same as in the regular QCI.
    expected_number_of_points = estimated_gather_time/sampling_period;
    return qMin(pointsInTrial*100.0/expected_number_of_points,100.0);

}

qreal StudyEndOperations::computeQCIOnLegacy3DStudy(const QVariantList &timevector) const {

    qreal pointsInTrial = 0;
    qreal estimated_gather_time = 0;
    qreal expected_number_of_points = 0;

    pointsInTrial = timevector.size();
    if (timevector.size() >= 2){
        estimated_gather_time = timevector.last().toReal() - timevector.first().toReal();
    }

    // Now that we have an estimate of the study length, we do the same as in the regular QCI.
    expected_number_of_points = estimated_gather_time/sampling_period;
    return qMin(pointsInTrial*100.0/expected_number_of_points,100.0);

}

qreal StudyEndOperations::computeNumberOfDataPointsIn2DStudy(const QVariantList &trials) const{

    qreal pointsInTrial = 0;

    // Now that we have how many points we should get. We count how many points we have.
    for (qint32 i = 0; i < trials.size(); i++){

        QVariantMap trial = trials.at(i).toMap();
        QVariantMap data = trial.value(VMDC::TrialField::DATA).toMap();
        QStringList dataset_names = data.keys();
        for (qint32 j = 0; j < dataset_names.size(); j++){
            QVariantMap dataSet = data.value(dataset_names.at(j)).toMap();
            pointsInTrial = pointsInTrial + dataSet.value(VMDC::DataSetField::RAW_DATA).toList().size();
        }

    }

    //return qMin(pointsInTrial*100.0/expectedNumberOfDataPoints,100.0);
    return pointsInTrial;

}

