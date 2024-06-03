#include "studyendoperations.h"


StudyEndOperations::StudyEndOperations() {

}

bool StudyEndOperations::wasOperationSuccssful() const {
    return operationsSuccess;
}

QVariantMap StudyEndOperations::getLastQCIData() const {
    return this->qciData;
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

        filesToProcessIDX << directory + "/" + Globals::BaseFileNames::MakeMetdataFileName(basename);

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

            // We have to make sure NOT to add the metada files.
            if (Globals::BaseFileNames::IsMetadataFileName(studyFiles.at(i))) continue;

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

    //QElapsedTimer timer;
    //qDebug() << "Doing file processing"; timer.start();

    QVariantMap pp = vmdc.getProcessingParameters();
    qreal sampling_frequency = pp.value(VMDC::ProcessingParameter::SAMPLE_FREQUENCY).toReal();
    sampling_period = 1000.0/sampling_frequency; // In milliseconds.

    //qDebug() << "Sampling Frequency:" << sampling_frequency << ". The period" << sampling_period;

    // Now we need to know how to compute the QC Index for each study in the file. All studies need to approve individually.
    // If any of them fail then the QC Index should be set to bad.

    fileQCIndex = 101;
    fileQCPass  = true;

    QStringList studyList = vmdc.getStudies();
    qreal ok_threshold = vmdc.getQCParameters().value(VMDC::QCGlobalParameters::DQI_THRESHOLD).toReal();

    // Making sure QC is cleared.
    vmdc.setQCStudyStructClear();

    //qDebug() << "After QC Study Struct Clear" << timer.elapsed();

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
                // This is a legacy study. It's too old and we shouldn't be supporting it.
                StaticThreadLogger::error("StudyEndOperations::run","No study duration field found on  '" + dataFile + "' and it's not a 3D study which means it's an old one");
                return;
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


    // We now need to save to both the JSON file and Metadata Json (Used to be known as IDX Files) files.
    vmdc.clearAndStoreSubjectData(); // We make sure to store the raw data anonimosuly.
    if (!vmdc.saveJSONFile(dataFile)){
        StaticThreadLogger::error("StudyEndOperations::run","Failed is saving JSON data with QC parameters to file '" + dataFile + "'");
        return;
    }

    //qDebug() << "After saving the data file" << timer.elapsed();

    // The metadata MUST include the subject data so it is resotored.
    vmdc.restoreSubjectData();

    //qDebug() << "After restoring subjec data" << timer.elapsed();

    // We only clear the metadata WHEN
    vmdc.clearFieldsForIndexFileCreation();
    if (!vmdc.saveJSONFile(idxFile)){
        StaticThreadLogger::error("StudyEndOperations::run","Failed is saving JSON data with QC parameters to file '" + idxFile + "'");
        return;
    }

    // We need to get the evaluation ID. It's present the in the metadata.
    QString evalID = vmdc.getEvaluationID();

    // Now we generate the compressed file.
    if (!createTarFileAndCleanup()) return;

    //qDebug() << "After creating Tar File" << timer.elapsed();

    // Generate the Local DB Entry.
    if (!createQCIStudyFile(evalID)) return;

    //qDebug() << "After creating QCI File" << timer.elapsed();

    operationsSuccess = true;

}


qreal StudyEndOperations::computeQCI(const QString &study) {

    qreal timeWithNoDataPerTrial = 0;
    qreal numberOfTrials = 0;
    qreal numberOfDataPointsObatained = 0;

    qreal studyDuration = vmdc.getStudyDuration(study);

    if ( (study == VMDC::Study::BINDING_BC_2_SHORT) || (study == VMDC::Study::BINDING_UC_2_SHORT) ||
         (study == VMDC::Study::BINDING_BC_3_SHORT) || (study == VMDC::Study::BINDING_UC_3_SHORT) ) {

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
    else if (study == VMDC::Study::MOVING_DOT){
        QVariantList trials = vmdc.getStudyTrialList(study);
        numberOfDataPointsObatained = computeNumberOfDataPointsIn2DStudy(trials);
        numberOfTrials = trials.size();
        timeWithNoDataPerTrial = 0;
    }
    else if ( (study == VMDC::Study::SPHERES) || (study == VMDC::Study::SPHERES_VS) ){


        QVariant temp  = vmdc.get3DStudyData(study,VMDC::Study3DDataFields::TIME_VECTOR);
        QVariantList frame_numbers = vmdc.get3DStudyData(study,VMDC::Study3DDataFields::FRAME_COUNTER).toList();
        if (frame_numbers.size() > 0){
            numberOfDataPointsObatained = frame_numbers.last().toLongLong() - frame_numbers.first().toLongLong();
        }
        else {
            numberOfDataPointsObatained = temp.toList().size();
        }

        //        qDebug() << "Number of time vector data points" << temp.toList().size() << "Number of frame counter data points" << frame_numbers.size();
        //        qDebug() << "Last value of the frame counter" << frame_numbers.last().toLongLong()  << "First value of the frame counter" << frame_numbers.first().toLongLong();

        //numberOfDataPointsObatained = temp.toList().size();
        numberOfTrials = 1;
        timeWithNoDataPerTrial = 0;


    }
    else if ((study == VMDC::Study::NBACK_3) || (study == VMDC::Study::NBACK_4)) {

        QVariantList trials = vmdc.getStudyTrialList(study);
        QVariantMap config = vmdc.getStudyConfiguration(study);
        numberOfDataPointsObatained = computeNumberOfDataPointsIn2DStudy(trials);
        numberOfTrials = trials.size();
        // Nback studies are not collecting data ONLY when transition from one trial to the next.
        //qDebug() << "For NBack. Transition Time" << config.value(VMDC::StudyParameter::NBACK_TRANSITION).toReal() << "And the Pause Duration" << pauseDuration;
        timeWithNoDataPerTrial = config.value(VMDC::StudyParameter::NBACK_TRANSITION).toReal(); // + pauseDuration/numberOfTrials;

    }

    else {
        StaticThreadLogger::error("StudyEndOperations::computeQCI","Found unexpected study type in data file '" + dataFile + "' of  '" + study + "'. Don't know how to compute time with no acquisition");
        return -1;
    }

    // This is usually during testing like removing the headset for the entire study. Rather have a zero, just in case.
    if (numberOfDataPointsObatained < 0) numberOfDataPointsObatained = 0;

    // Now we compute the final quality control index as the ratio of expected number of points versus the computed/obtained number of points.
    qreal estimatedDataGatheringTime = studyDuration - timeWithNoDataPerTrial*numberOfTrials;
    qreal expectedNumberOfDataPoints = estimatedDataGatheringTime/sampling_period;
    qreal qci = qMin(numberOfDataPointsObatained*100.0/expectedNumberOfDataPoints,100.0);

    qDebug() << "Study duration" << studyDuration << ". Time with No Data PerTrial" << timeWithNoDataPerTrial << ". Number of Trials" << numberOfTrials;
    qDebug() << "Expected Data Gathering Time" << estimatedDataGatheringTime << "Expected Number of Data Points" << expectedNumberOfDataPoints << "Sampling Period" << sampling_period;
    qDebug() << "Number of points obtained" << numberOfDataPointsObatained << "QCI" << qci;

    return qci;

}

bool StudyEndOperations::createTarFileAndCleanup() {

    QFileInfo info(dataFile);

    QString directory   = info.absolutePath();
    QString basename    = info.baseName();

    QString localJSON   = basename + ".json";
    QString localIDX    = Globals::BaseFileNames::MakeMetdataFileName(basename);
    compressedZipFile   = basename + ".zip";
    qciFile             = basename + ".qci";
    qciFile             = directory + "/"  + qciFile;


    QStringList arguments;
    arguments << "-c";
    arguments << "-z";
    arguments << "-f";
    arguments << compressedZipFile;
    arguments << localJSON;
    arguments << localIDX;

    //qDebug() << "jsonFile" << jsonFile << "zip" << zipfile;

    QProcess tar;
    tar.setWorkingDirectory(directory);
    tar.start(Globals::Paths::TAR_EXE,arguments);
    tar.waitForFinished();

    qint32 exit_code = tar.exitCode();

    fullPathCompressedZip = directory + "/" + compressedZipFile;

    if (!QFile(fullPathCompressedZip).exists()){
        StaticThreadLogger::error("StudyEndOperations::createTarFileAndCleanup","Could not zip the input file " + dataFile + ". Exit code for TAR.exe is: " + QString::number(exit_code));
        return false;
    }

    // If the file exists. We delete the .json and .idx files.
    localJSON = directory + "/" + localJSON;
    localIDX = directory + "/" + localIDX;
    QFile::remove(localJSON);
    QFile::remove(localIDX);
    return true;

}

bool StudyEndOperations::createQCIStudyFile(const QString &evalID) {

    QVariantMap dbEvaluationEntry;

    dbEvaluationEntry.clear();
    QString reason = "";
    if (!fileQCPass){
        // We automatically set the study to be discarded.
        reason = "low_dqi";
    }
    dbEvaluationEntry[Globals::QCIFields::QCI_PASS]  = fileQCPass;
    dbEvaluationEntry[Globals::QCIFields::QCI] = fileQCIndex;
    dbEvaluationEntry[VMDC::MetadataField::DISCARD_REASON] = reason;
    dbEvaluationEntry[Globals::QCIFields::EVALUATION_ID] = evalID;
    dbEvaluationEntry[Globals::QCIFields::TARBALL_FILE] = compressedZipFile;
    dbEvaluationEntry[VMDC::MetadataField::COMMENTS] = "";

    QFile file (qciFile);
    if (!file.open(QFile::WriteOnly)){
        StaticThreadLogger::error("StudyEndOperations::createQCIStudyFile","Could not open QCI file '" + qciFile + "' for writing");
        return false;
    }

    // This data is stored only to be returned by it's getter.
    this->qciData = dbEvaluationEntry;

    // Converting to a compact JSON Array
    QJsonDocument json = QJsonDocument::fromVariant(dbEvaluationEntry);
    QByteArray data_to_store = json.toJson(QJsonDocument::Indented);

    // Storing JSON Array in the QCI file.
    QTextStream writer(&file);
    writer << data_to_store;
    file.close();

    return true;

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
        //qDebug() << "Trial" << i;
        for (qint32 j = 0; j < dataset_names.size(); j++){
            QVariantMap dataSet = data.value(dataset_names.at(j)).toMap();
            //qDebug() << "   Dataset: " << dataset_names.at(j) << dataSet.value(VMDC::DataSetField::RAW_DATA).toList().size();

            // We check to see if the counter is present to compute the QCI with the counter.
            QVariantList list = dataSet.value(VMDC::DataSetField::RAW_DATA).toList();
            qint32 temp_npoints = 0;
            if (list.size() > 0){
                QVariantMap fPoint = list.first().toMap();
                QVariantMap lPoint = list.last().toMap();
                QVariantList fIntVector = fPoint.value(VMDC::DataVectorField::INT_VECTOR).toList();
                QVariantList lIntVector = lPoint.value(VMDC::DataVectorField::INT_VECTOR).toList();
                if ((lIntVector.size() == 2) && (fIntVector.size() == 2)){
                    qlonglong first = fIntVector.at(VMDC::DataVectorField::L_CNT).toLongLong();
                    qlonglong last  = lIntVector.at(VMDC::DataVectorField::L_CNT).toLongLong();
                    temp_npoints = last - first + 1;
                }
            }
            //temp_npoints = dataSet.value(VMDC::DataSetField::RAW_DATA).toList().size();

            pointsInTrial = pointsInTrial + temp_npoints;// dataSet.value(VMDC::DataSetField::RAW_DATA).toList().size();
        }

    }

    //return qMin(pointsInTrial*100.0/expectedNumberOfDataPoints,100.0);
    return pointsInTrial;

}

