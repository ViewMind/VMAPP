#include "datfiletorawdatacontainer.h"

DatFileToViewMindDataContainer::DatFileToViewMindDataContainer()
{

}

QString DatFileToViewMindDataContainer::getError() const {
    return error;
}

QString DatFileToViewMindDataContainer::getFinalStudyFileName() const {
    return finalFileStudyName;
}

bool DatFileToViewMindDataContainer::fromDatFile(const QString &dat_file, const ConfigurationManager &configmng, ViewMindDataContainer *rdc){

    error = "";

    QFileInfo info(dat_file);

    DatFileInfoInDir::DatInfo dat_info = DatFileInfoInDir::getDatFileInformation(info.baseName());
    if (dat_info.category == ""){
        error = "Could not determine the study type for " + dat_file;
        return false;
    }

    QString parameter_key = "gazepoint";
    QVariantMap qc; QVariantMap perStudy;

#ifdef EYETRACKER_GAZEPOINT
    parameter_key = "gazepoint";
    qc[VMDC::QCGlobalParameters::MAX_TIMESTAMP_DIFF] = 8;
    qc[VMDC::QCGlobalParameters::MIN_TIMESTAMP_DIFF] = 5;
    qc[VMDC::QCGlobalParameters::MAX_GLITCHES] = 20;

    // Reading
    perStudy[VMDC::QCStudyParameters::MIN_FIXS_PER_TRIAL] = 3;
    perStudy[VMDC::QCStudyParameters::MIN_POINTS_PER_TRIAL] = 150;
    qc.insert(VMDC::Study::READING,perStudy);

    // Binding
    perStudy[VMDC::QCStudyParameters::MIN_FIXS_PER_TRIAL] = 3;
    perStudy[VMDC::QCStudyParameters::MIN_POINTS_PER_TRIAL] = 150;
    qc.insert(VMDC::MultiPartStudyBaseName::BINDING,perStudy);


    // NBack RT
    perStudy[VMDC::QCStudyParameters::MIN_FIXS_PER_TRIAL] = 2;
    perStudy[VMDC::QCStudyParameters::MIN_POINTS_PER_TRIAL] = 30;
    qc.insert(VMDC::Study::NBACKRT,perStudy);

    // NBack MS
    perStudy[VMDC::QCStudyParameters::MIN_FIXS_PER_TRIAL] = 2;
    perStudy[VMDC::QCStudyParameters::MIN_POINTS_PER_TRIAL] = 30;
    qc.insert(VMDC::Study::NBACKMS,perStudy);

    // Go No Go
    perStudy[VMDC::QCStudyParameters::MIN_FIXS_PER_TRIAL] = 2;
    perStudy[VMDC::QCStudyParameters::MIN_POINTS_PER_TRIAL] = 100;
    qc.insert(VMDC::Study::GONOGO,perStudy);

    if (configmng.getInt("sample_frequency") != 150){
        error = "Wrong Sample Frequency " + configmng.getString("sample_frequency") + " expected 150 for GazePoint";
        return false;
    }


#endif
#ifdef EYETRACKER_HTCVIVEPRO

    parameter_key = "htcviveeyepro";

    qc[VMDC::QCGlobalParameters::MAX_TIMESTAMP_DIFF] = 10;
    qc[VMDC::QCGlobalParameters::MIN_TIMESTAMP_DIFF] = 6;
    qc[VMDC::QCGlobalParameters::MAX_GLITCHES] = 20;

    // Reading
    perStudy[VMDC::QCStudyParameters::MIN_FIXS_PER_TRIAL] = 3;
    perStudy[VMDC::QCStudyParameters::MIN_POINTS_PER_TRIAL] = 120;
    qc.insert(VMDC::Study::READING,perStudy);

    // Binding
    perStudy[VMDC::QCStudyParameters::MIN_FIXS_PER_TRIAL] = 3;
    perStudy[VMDC::QCStudyParameters::MIN_POINTS_PER_TRIAL] = 120;
    qc.insert(VMDC::MultiPartStudyBaseName::BINDING,perStudy);

    // NBack RT
    perStudy[VMDC::QCStudyParameters::MIN_FIXS_PER_TRIAL] = 2;
    perStudy[VMDC::QCStudyParameters::MIN_POINTS_PER_TRIAL] = 25;
    qc.insert(VMDC::Study::NBACKRT,perStudy);

    // NBack MS
    perStudy[VMDC::QCStudyParameters::MIN_FIXS_PER_TRIAL] = 2;
    perStudy[VMDC::QCStudyParameters::MIN_POINTS_PER_TRIAL] = 25;
    qc.insert(VMDC::Study::NBACKMS,perStudy);

    // Go No Go
    perStudy[VMDC::QCStudyParameters::MIN_FIXS_PER_TRIAL] = 2;
    perStudy[VMDC::QCStudyParameters::MIN_POINTS_PER_TRIAL] = 80;
    qc.insert(VMDC::Study::GONOGO,perStudy);

    if (configmng.getInt("sample_frequency") != 120){
        error = "Wrong Sample Frequency " + configmng.getString("sample_frequency") + " expected 120 for HTCViveEyePro";
        return false;
    }

#endif

    // Creating the metadata.
    QVariantMap metadata;
    QStringList parts = dat_info.date.split("_");
    metadata.insert(VMDC::MetadataField::DATE,parts.join("-"));
    parts = dat_info.hour.split("_");
    metadata.insert(VMDC::MetadataField::HOUR,parts.join(":") + ":00"); // Need to add the seconds.
    metadata.insert(VMDC::MetadataField::INSTITUTION_ID,1);
    metadata.insert(VMDC::MetadataField::INSTITUTION_INSTANCE,0);
    metadata.insert(VMDC::MetadataField::INSTITUTION_NAME,"");
    metadata.insert(VMDC::MetadataField::MOUSE_USED,"false");
    metadata.insert(VMDC::MetadataField::PROC_PARAMETER_KEY,parameter_key);
    metadata.insert(VMDC::MetadataField::STATUS,VMDC::StatusType::FINALIZED);
    metadata.insert(VMDC::MetadataField::PROTOCOL,configmng.getString("protocol_name"));

    finalFileStudyName = dat_info.date + "_" + dat_info.hour + ".json";


    // Creating the subject data
    QVariantMap subject_data;
    subject_data.insert(VMDC::SubjectField::AGE,configmng.getString("patient_age"));
    subject_data.insert(VMDC::SubjectField::BIRTH_COUNTRY,"");
    subject_data.insert(VMDC::SubjectField::BIRTH_DATE,"");
    subject_data.insert(VMDC::SubjectField::GENDER,"");
    subject_data.insert(VMDC::SubjectField::INSTITUTION_PROVIDED_ID,"");
    subject_data.insert(VMDC::SubjectField::LASTNAME,"");
    subject_data.insert(VMDC::SubjectField::LOCAL_ID,"unknown_id");
    subject_data.insert(VMDC::SubjectField::NAME,"");
    subject_data.insert(VMDC::SubjectField::YEARS_FORMATION,"");

    // Setting the evaluator info. We do it here becuase it is required for the data file comparisons.
    QVariantMap evaluator;
    evaluator.insert(VMDC::AppUserField::EMAIL,"ariel.arelovich@viewmind.ai");
    evaluator.insert(VMDC::AppUserField::NAME,"Ariel");
    evaluator.insert(VMDC::AppUserField::LASTNAME,"Arelovich");
    evaluator.insert(VMDC::AppUserField::LOCAL_ID,"0");
    evaluator.insert(VMDC::AppUserField::VIEWMIND_ID,"0");

    // Setting the selected doctor info.
    QVariantMap medic_to_store;
    medic_to_store.insert(VMDC::AppUserField::EMAIL,"jorozco@gmail.com");
    medic_to_store.insert(VMDC::AppUserField::NAME,"Javier");
    medic_to_store.insert(VMDC::AppUserField::LASTNAME,"Orozco");
    medic_to_store.insert(VMDC::AppUserField::LOCAL_ID,"0");
    medic_to_store.insert(VMDC::AppUserField::VIEWMIND_ID,"1");

    // Setting all the values.
    rdc->setQCParameters(qc);
    rdc->setSubjectData(subject_data);
    rdc->setMetadata(metadata);
    rdc->setApplicationUserData(VMDC::AppUserType::EVALUATOR,evaluator);
    rdc->setApplicationUserData(VMDC::AppUserType::MEDIC,medic_to_store);

    // Loading the processing parameters.
    QVariantMap pp;
    pp.insert(VMDC::ProcessingParameter::SAMPLE_FREQUENCY,configmng.getReal("sample_frequency"));
    pp.insert(VMDC::ProcessingParameter::MAX_DISPERSION_WINDOW,configmng.getReal("moving_window_max_dispersion"));
    pp.insert(VMDC::ProcessingParameter::MIN_FIXATION_DURATION,configmng.getReal("minimum_fixation_length"));
    pp.insert(VMDC::ProcessingParameter::LATENCY_ESCAPE_RADIOUS,configmng.getReal("latency_escape_radious"));

    QVariantMap study_configuration;
    QString selectedEye = VMDC::Eye::fromInt(configmng.getInt("valid_eye"));
    study_configuration.insert(VMDC::StudyParameter::VALID_EYE,selectedEye);
    if (selectedEye == VMDC::Eye::LEFT){
        rightEyeEnabled = false;
        leftEyeEnabled = true;
    }
    else if (selectedEye == VMDC::Eye::RIGHT){
        rightEyeEnabled = true;
        leftEyeEnabled = false;
    }

    QString header;
    QString st;

    QString code = dat_info.category.mid(0,2);
    if (code == "RD"){
        if (dat_info.extraInfo == "EN"){
            study_configuration.insert(VMDC::StudyParameter::LANGUAGE,VMDC::ReadingLanguage::ENGLISH);
        }
        else if (dat_info.extraInfo == "ES"){
            study_configuration.insert(VMDC::StudyParameter::LANGUAGE,VMDC::ReadingLanguage::SPANISH);
        }
        else if (dat_info.extraInfo == "DE"){
            study_configuration.insert(VMDC::StudyParameter::LANGUAGE,VMDC::ReadingLanguage::GERMAN);
        }
        else if (dat_info.extraInfo == "IS"){
            study_configuration.insert(VMDC::StudyParameter::LANGUAGE,VMDC::ReadingLanguage::ISELANDIC);
        }
        else if (dat_info.extraInfo == "FR"){
            study_configuration.insert(VMDC::StudyParameter::LANGUAGE,VMDC::ReadingLanguage::FRENCH);
        }
        else{
            error = "Unrecognized reading languange: " + dat_info.extraInfo;
            return false;
        }
        header = "#READING";
        finalFileStudyName = "reading_" + finalFileStudyName;
        st = VMDC::Study::READING;
    }
    else if ((code == "UC") || (code == "BC")){

        if (code == "BC"){
            st = VMDC::Study::BINDING_BC;
        }
        else{
            st = VMDC::Study::BINDING_UC;
        }

        finalFileStudyName = "binding" + finalFileStudyName;

        if (dat_info.extraInfo.size() != 2){
            error = "Expected extra info of a binding file two have only 2 characters: " + dat_info.extraInfo;
            return false;
        }

        if (dat_info.extraInfo.at(0) == '2') study_configuration.insert(VMDC::StudyParameter::NUMBER_TARGETS,VMDC::BindingTargetCount::TWO);
        else if (dat_info.extraInfo.at(0) == '3') study_configuration.insert(VMDC::StudyParameter::NUMBER_TARGETS,VMDC::BindingTargetCount::THREE);
        else{
            error = QString("Unknown number of targets from extra info: ") + dat_info.extraInfo.at(0);
            return false;
        }

        if (dat_info.extraInfo.at(1) == 'l') study_configuration.insert(VMDC::StudyParameter::TARGET_SIZE,VMDC::BindingTargetSize::LARGE);
        else if (dat_info.extraInfo.at(1) == 's') study_configuration.insert(VMDC::StudyParameter::TARGET_SIZE,VMDC::BindingTargetSize::SMALL);
        else{
            error = QString("Unknown target size from extra info: ") + dat_info.extraInfo.at(0);
            return false;
        }

        header = "#IMAGE";

    }
    else if (code == "NB"){
        // NBack RT study.
        st = VMDC::Study::NBACKRT;
        header = "#NBACKRT";
        finalFileStudyName = "nbackrt_" + finalFileStudyName;
    }
    else if (code == "GN"){
        st = VMDC::Study::GONOGO;
        header = "#GONOGO";
        finalFileStudyName = "gonogo_" + finalFileStudyName;
    }
    else{
        error = "Unrecognized code " + code + " from " + dat_file;
        return false;
    }

    QStringList more_study_info = separateStudyDescriptionFromData(dat_file,header);
    // Answer is: version << expDescription << resolution << rawData;
    if (more_study_info.size() != 4) {
        return false;
    }

    QString resolution = more_study_info.at(2);
    QStringList w_and_h = resolution.split(" ",Qt::SkipEmptyParts);
    if (w_and_h.size() != 2){
        error = "Invalid resolution string " + resolution;
        return false;
    }

    // Setting the metadata.
    pp.insert(VMDC::ProcessingParameter::RESOLUTION_HEIGHT,w_and_h.last().trimmed());
    pp.insert(VMDC::ProcessingParameter::RESOLUTION_WIDTH,w_and_h.first().trimmed());

    // In case they are needed as numbers.
    qreal rw = w_and_h.first().trimmed().toDouble();
    qreal rh = w_and_h.last().trimmed().toDouble();

    if (st == VMDC::Study::NBACKRT){
        // We need to get the target boxes.

        FieldingParser parser;
        if (!parser.parseFieldingExperiment(more_study_info.at(0) + "\n" + more_study_info.at(1),rw,rh)){
            error = "Parsing fielding experiment: " + parser.getError();
            return false;
        }

        // Computing the hitboxes
        QList<QRectF> hitBoxes = parser.getHitTargetBoxes();


        // All other boxes except 2 and 5 get half a square of leeway.
        QVariantList modHitBoxes;
        for (qint32 i = 0; i < hitBoxes.size(); i++){

            qreal x,y,w,h;
            QVariantList hitbox;

            //qDebug() << "HB BEFORE" << hitBoxes.at(i);

            x = hitBoxes.at(i).x();
            w = hitBoxes.at(i).width();
            if ((i == 2) || (i == 5)){
                y = hitBoxes.at(i).y();
                h = hitBoxes.at(i).height();
            }
            else{
                qreal leeway = hitBoxes.at(i).height()/2;
                y = hitBoxes.at(i).y() - leeway;
                h = hitBoxes.at(i).height()*2;
            }

            hitbox << x << y << w << h;
            // Casting is necessary otherwise the operation appends a list to the list.
            // Then the JSON is interpreted as one long list instead of serveral lists of 4 values.
            modHitBoxes << (QVariant) hitbox;
        }

        // Store them as part of the processing parameters.
        pp.insert(VMDC::ProcessingParameter::NBACK_HITBOXES,modHitBoxes);
    }
    else if (st == VMDC::Study::GONOGO){
        GoNoGoParser gngparser;
        if (!gngparser.parseGoNoGoExperiment(more_study_info.at(0) + "\n" + more_study_info.at(1),rw,rh)){
            error = "Error parsing go no go experiment: " + gngparser.getError();
            return false;
        }

        QRectF arrow = gngparser.getArrowTargetBox();
        QList<QRectF> left_and_right = gngparser.getTargetBoxes();

        QVariantList target_box_vector;
        QVariantList target_boxes;
        target_box_vector << arrow.x() << arrow.y() << arrow.width() << arrow.height();
        target_boxes << (QVariant) target_box_vector;
        target_box_vector.clear();
        target_box_vector << left_and_right.at(0).x() << left_and_right.at(0).y() << left_and_right.at(0).width() << left_and_right.at(0).height();
        target_boxes << (QVariant) target_box_vector;
        target_box_vector.clear();
        target_box_vector << left_and_right.at(1).x() << left_and_right.at(1).y() << left_and_right.at(1).width() << left_and_right.at(1).height();
        target_boxes << (QVariant) target_box_vector;

        pp.insert(VMDC::ProcessingParameter::GONOGO_HITBOXES,target_boxes);
    }

    if (!rdc->setProcessingParameters(pp)){
        error = "Setting the processing parameters : " + rdc->getError();
        return false;
    }

    if (!rdc->addStudy(st,study_configuration,more_study_info.at(1),more_study_info.at(0))){
        error = "Setting the study: " + rdc->getError();
        return false;
    }

    // Setup fixation computation.
    MovingWindowParameters mwp;
    mwp.sampleFrequency = pp.value(VMDC::ProcessingParameter::SAMPLE_FREQUENCY).toReal();
    mwp.minimumFixationLength =  pp.value(VMDC::ProcessingParameter::MIN_FIXATION_DURATION).toReal();
    mwp.maxDispersion =  pp.value(VMDC::ProcessingParameter::MAX_DISPERSION_WINDOW).toReal();
    mwp.calculateWindowSize();

    rMWA.parameters = mwp;
    lMWA.parameters = mwp;

    // And resetting just in case.
    rMWA.finalizeOnlineFixationCalculation();
    lMWA.finalizeOnlineFixationCalculation();

    parseStudyData(more_study_info.at(3),more_study_info.at(1),st,rdc);

    return true;

}

void DatFileToViewMindDataContainer::parseStudyData(const QString &raw_data,
                                                                     const QString &exp,
                                                                     QString st,
                                                                     ViewMindDataContainer *rdc){

    if ( st ==  VMDC::Study::READING) {
        parseReading(raw_data,exp,rdc);
    }
    else if ( st ==  VMDC::Study::BINDING_BC) {
        parseBinding(raw_data,rdc,true);
    }
    else if ( st ==  VMDC::Study::BINDING_UC) {
        parseBinding(raw_data,rdc,false);
    }
    else if ( st ==  VMDC::Study::NBACKRT) {
        parseGoNoGo(raw_data,rdc);
    }
    else if ( st ==  VMDC::Study::GONOGO) {
        parseGoNoGo(raw_data,rdc);
    }
    else{
        error = "Unknown study to parse " + st;
    }

}
////////////////////////////////////////////// FIXATION CALCULATION FUNCTIONS ////////////////////////////////////////////

void DatFileToViewMindDataContainer::finalizeOnlineFixations(ViewMindDataContainer *rawdata){
    lastFixationR = rMWA.finalizeOnlineFixationCalculation();
    lastFixationL = lMWA.finalizeOnlineFixationCalculation();
    if (lastFixationR.isValid()){
        rawdata->addFixationVectorR(fixationToVariantMap(lastFixationR));
    }
    if (lastFixationL.isValid()){
        rawdata->addFixationVectorL(fixationToVariantMap(lastFixationL));
    }
}

void DatFileToViewMindDataContainer::computeOnlineFixations(ViewMindDataContainer *rawdata, const QVariantMap &data, qreal l_schar, qreal l_word, qreal r_schar, qreal r_word){

    qreal xr = data.value(VMDC::DataVectorField::X_R).toReal();
    qreal yr = data.value(VMDC::DataVectorField::Y_R).toReal();
    qreal pr = data.value(VMDC::DataVectorField::PUPIL_R).toReal();
    qreal ts = data.value(VMDC::DataVectorField::TIMESTAMP).toReal();
    qreal xl = data.value(VMDC::DataVectorField::X_L).toReal();
    qreal yl = data.value(VMDC::DataVectorField::Y_L).toReal();
    qreal pl = data.value(VMDC::DataVectorField::PUPIL_L).toReal();

    lastFixationR = rMWA.calculateFixationsOnline(xr,yr,ts,pr,r_schar,r_word);
    lastFixationL = lMWA.calculateFixationsOnline(xl,yl,ts,pl,l_schar,l_word);

    if (lastFixationR.isValid() && rightEyeEnabled){
        rawdata->addFixationVectorR(fixationToVariantMap(lastFixationR));
    }

    if (lastFixationL.isValid() && leftEyeEnabled){
        rawdata->addFixationVectorL(fixationToVariantMap(lastFixationL));
    }
}

QVariantMap DatFileToViewMindDataContainer::fixationToVariantMap(const Fixation &f){
    QVariantMap map;
    map.insert(VMDC::FixationVectorField::X,f.x);
    map.insert(VMDC::FixationVectorField::Y,f.y);
    map.insert(VMDC::FixationVectorField::DURATION,f.duration);
    map.insert(VMDC::FixationVectorField::TIME,f.time);
    map.insert(VMDC::FixationVectorField::START_TIME,f.fixStart);
    map.insert(VMDC::FixationVectorField::END_TIME,f.fixEnd);
    map.insert(VMDC::FixationVectorField::START_INDEX,f.indexFixationStart);
    map.insert(VMDC::FixationVectorField::END_INDEX,f.indexFixationEnd);
    map.insert(VMDC::FixationVectorField::PUPIL,f.pupil);
    map.insert(VMDC::FixationVectorField::ZERO_PUPIL,f.pupilZeroCount);
    if (studyType == VMDC::Study::READING){
        map.insert(VMDC::FixationVectorField::CHAR,f.sentence_char);
        map.insert(VMDC::FixationVectorField::WORD,f.sentence_word);
    }
    return map;
}

////////////////////////////////////////////// PROCESSING FUNCTIONS ////////////////////////////////////////////

void DatFileToViewMindDataContainer::parseReading(const QString &raw_data, const QString &exp, ViewMindDataContainer *rdc){

    // Creating the id -> sentence map.
    QStringList lines = exp.split("\n",Qt::SkipEmptyParts);
    QMap<QString,QString> sentences;
    for (qint32 i = 0; i < lines.size(); i++){
        QStringList parts = lines.at(i).split(":",Qt::SkipEmptyParts);
        if (parts.size() != 2){
            error = "Unexpected sentece in reading experiment: " + lines.at(i);
            return;
        }
        QString id_str = parts.first();
        while (id_str.size() < 4) id_str = "0" + id_str;
        sentences[id_str] = parts.last();
    }

    // Now parsing the actual experiment data.
    rdc->setCurrentStudy(VMDC::Study::READING);

    // Setting the trial list type which, for reading, is always unique.

    QString current_trial_id = "";
    lines = raw_data.split("\n",Qt::SkipEmptyParts);
    QString response = "";
    qint32 size = 0;
    for (qint32 i = 0; i < lines.size(); i++){

        QStringList line_parts = lines.at(i).split(" ",Qt::SkipEmptyParts);
        QString id = line_parts.first().trimmed();

        if (id != current_trial_id){
            if (current_trial_id != ""){
                // This is a new trial. So we finalize the dataset.
                finalizeOnlineFixations(rdc);
                rdc->finalizeDataSet();
                // And we finalize the trial.
                rdc->finalizeTrial();
            }

            // We need to create a new trial.
            size = sentences.value(id).split(" ",Qt::SkipEmptyParts).size();

            if (!rdc->addNewTrial(id,sentences.value(id))){
                error = "Error creating new trial for " + id  + " on line " + lines.at(i) + ". Error was: " + rdc->getError();
                return;
            }

            rdc->setCurrentDataSet(VMDC::DataSetType::UNIQUE);

        }

        if (!sentences.contains(id)){
            error = "Found data with id " + id + " which does not belong to a sentence id. Line: " + lines.at(i);
            return;
        }

        current_trial_id = id;

        // Question check
        if (lines.at(i).contains("->")){
            // This is a question with an answer.

            QStringList question_and_respose = lines.at(i).split("->",Qt::SkipEmptyParts);
            response = question_and_respose.last();
        }
        else{

            // Checking on size.
            if (line_parts.size() != 13){
                error = "Badly formatted data line for reading on line: " + lines.at(i);
                return;
            }

            float timestamp,xr,yr,xl,yl,pr,pl,char_r,char_l,word_r,word_l;
            timestamp = line_parts.at(1).toDouble();
            xr = line_parts.at(2).toDouble();
            yr = line_parts.at(3).toDouble();
            xl = line_parts.at(4).toDouble();
            yl = line_parts.at(5).toDouble();
            word_r = line_parts.at(6).toDouble();
            char_r = line_parts.at(7).toDouble();
            word_l = line_parts.at(8).toDouble();
            char_l = line_parts.at(9).toDouble();
            // Value 10 si sentence word in lenght but this is store in the sentence.
            pr = line_parts.at(11).toDouble();
            pl = line_parts.at(12).toDouble();

            if (line_parts.at(10).toInt() != size){
                error = "Length for sentence is set as " + line_parts.at(10) + " however sentence has " + QString::number(size);
                error = error + "\nSENTENCE: " + sentences.value(id);
                return;
            }

            // We add the data vector.
            QVariantMap vector = ViewMindDataContainer::GenerateReadingRawDataVector(timestamp,xr,yr,xl,yl,pr,pl,char_r,char_l,word_r,word_l);
            computeOnlineFixations(rdc,vector,char_l,word_l,char_r,word_r);
            rdc->addNewRawDataVector(vector);

        }
    }

    // Finalize the last data set and response.
    finalizeOnlineFixations(rdc);
    rdc->finalizeDataSet();
    rdc->finalizeTrial();

    // And then finalizing the study.
    if (!rdc->finalizeStudy()){
        error = "Finalizing Study: " + rdc->getError();
    }

    rdc->markFileAsFinalized();

    return;
}

//////////////////////////////////////// NBACKRT

void DatFileToViewMindDataContainer::parseNBackRT(const QString &raw_data, const QString &exp, ViewMindDataContainer *rdc){

    QStringList explines = exp.split("\n",Qt::SkipEmptyParts);
    QMap<QString,QString> trialDesc;
    for (qint32 i = 0; i < explines.size(); i++){
        QStringList tokens = explines.at(i).split(" ",Qt::SkipEmptyParts);
        if (tokens.size() < 4){
            error = "Badly formatted line in NBackRT description: " + explines.at(i);
        }
        QString id = tokens.first().trimmed();
        tokens.removeFirst();
        trialDesc.insert(id,tokens.join(" "));
    }

    // Now parsing the actual experiment data.
    rdc->setCurrentStudy(VMDC::Study::NBACKRT);

    // Setting the trial list type which, for reading, is always unique.

    QMap<QString,QString> trialPartToDataSetType;
    trialPartToDataSetType["1"] = VMDC::DataSetType::ENCODING_1;
    trialPartToDataSetType["2"] = VMDC::DataSetType::ENCODING_2;
    trialPartToDataSetType["3"] = VMDC::DataSetType::ENCODING_3;
    trialPartToDataSetType["4"] = VMDC::DataSetType::RETRIEVAL_1;

    QStringList lines = raw_data.split("\n",Qt::SkipEmptyParts);
    QString current_trial = "";

    for (qint32 i =0; i < lines.size(); i++){

        QStringList tokens = lines.at(i).split(" ",Qt::SkipEmptyParts);

        if (tokens.size() == 2){
            QString trialID = tokens.first().trimmed();
            QString trialPart = tokens.last().trimmed();

            if (current_trial != ""){
                finalizeOnlineFixations(rdc);
                rdc->finalizeDataSet();
            }

            if (trialID != current_trial){

                if (current_trial != ""){
                    rdc->finalizeTrial();
                }

                if (!trialDesc.contains(trialID)){
                    error = "Found trial id in NBackRT not found in description: " + trialID + " on line " + lines.at(i);
                    return;
                }

                if (!rdc->addNewTrial(trialID,trialDesc.value(trialID))){
                    error = "Error creating new trial for " + trialID  + " on line " + lines.at(i) + ". Error was: " + rdc->getError();
                    return;
                }
            }

            if (!trialPartToDataSetType.contains(trialPart)){
                error = "Unrecognized trial part " + trialPart + " for NBack RT: " + lines.at(i);
                return;
            }

            rdc->setCurrentDataSet(trialPartToDataSetType.value(trialPart));

            current_trial = trialID;
        }
        else if (tokens.size() == 7){
            float timestamp,xr,yr,xl,yl,pr,pl;
            timestamp = tokens.at(0).toDouble();
            xr = tokens.at(1).toDouble();
            yr = tokens.at(2).toDouble();
            xl = tokens.at(3).toDouble();
            yl = tokens.at(4).toDouble();
            pr = tokens.at(5).toDouble();
            pl = tokens.at(6).toDouble();
            QVariantMap vector = ViewMindDataContainer::GenerateStdRawDataVector(timestamp,xr,yr,xl,yl,pr,pl);
            computeOnlineFixations(rdc,vector);
            rdc->addNewRawDataVector(vector);
        }
        else{
            error = "Badly formed for NBack RT: " + lines.at(i);
            return;
        }
    }

    // Finalize the last data set and response.
    finalizeOnlineFixations(rdc);
    rdc->finalizeDataSet();
    rdc->finalizeTrial();

    // And then finalizing the study.
    if (!rdc->finalizeStudy()){
        error = "Finalizing Study: " + rdc->getError();
    }

    return;

}


//////////////////////////////////////// Binding
void DatFileToViewMindDataContainer::parseBinding(const QString &raw_data, ViewMindDataContainer *rdc, bool isBC){

    // Now parsing the actual experiment data.
    QString study;
    if (isBC) study = VMDC::Study::BINDING_BC;
    else study = VMDC::Study::BINDING_UC;

    if (!rdc->setCurrentStudy(study)){
        error = "Could not set current study: " + rdc->getError();
        return;
    }

    QStringList lines = raw_data.split("\n",Qt::SkipEmptyParts);
    QString current_trial = "";
    QString current_data_set;
    //current_data_set = VMDC::DataSetType::ENCODING_1;
    bool trial_finalized = false;

    for (qint32 i =0; i < lines.size(); i++){

        trial_finalized = false;

        if (!lines.at(i).contains("->")){
            QStringList tokens = lines.at(i).split(" ",Qt::SkipEmptyParts);

            if (tokens.size() == 2){
                QString trialID = tokens.first().trimmed();
                QString trialPart = tokens.last().trimmed();

                if (trialPart == "0") current_data_set = VMDC::DataSetType::ENCODING_1;
                else if (trialPart == "1") current_data_set = VMDC::DataSetType::RETRIEVAL_1;
                else{
                    error = "Bad trial part in Binding header for data set: " + lines.at(i);
                    return;
                }


                if (current_data_set == VMDC::DataSetType::ENCODING_1){

                    QString type = "";
                    if (trialID.toUpper().contains("SAME")) type = "S";
                    else if (trialID.toUpper().contains("DIFFERENT")) type = "D";
                    else{
                        error = "Binding trial header, could not determine the trial type: " + lines.at(i);
                        return;
                    }

                    // Add a new trial.
                    if (!rdc->addNewTrial(trialID,type)){
                        error = "Binding creating a new trial: " + rdc->getError();
                        return;
                    }
                    rdc->setCurrentDataSet(VMDC::DataSetType::ENCODING_1);
                }
                else {
                    finalizeOnlineFixations(rdc);
                    rdc->finalizeDataSet(); // Finalize the previous encoding.
                    rdc->setCurrentDataSet(VMDC::DataSetType::RETRIEVAL_1);
                }


            }
            // More than two tokens. Data.
            else{
                float timestamp,xr,yr,xl,yl,pr,pl;
                timestamp = tokens.at(0).toDouble();
                xr = tokens.at(1).toDouble();
                yr = tokens.at(2).toDouble();
                xl = tokens.at(3).toDouble();
                yl = tokens.at(4).toDouble();
                pr = tokens.at(5).toDouble();
                pl = tokens.at(6).toDouble();

                QVariantMap vector = ViewMindDataContainer::GenerateStdRawDataVector(timestamp,xr,yr,xl,yl,pr,pl);
                computeOnlineFixations(rdc,vector);

                rdc->addNewRawDataVector(vector);
            }
        }
        // This line contains a ->
        else{
            QStringList tokens = lines.at(i).split("->",Qt::SkipEmptyParts);
            if (tokens.size() != 2){
                error = "Bad answer line in binding data:  " + lines.at(i);
                return;
            }
            finalizeOnlineFixations(rdc);
            rdc->finalizeDataSet(); // Finalize the retrieval data set
            rdc->finalizeTrial(tokens.last()); // Finalize trial.
            trial_finalized = true;
        }

    }

    if (!trial_finalized){
        error = "Something went wrong when parsing binding info. The file did not end with a user response";
    }

    if (!rdc->finalizeStudy()){
        error = "Finalizing Study: " + rdc->getError();
    }

    // TODO Mark as finalized if both files are present

    return;
}


//////////////////////////////////////// GoNoGo
void DatFileToViewMindDataContainer::parseGoNoGo(const QString &raw_data, ViewMindDataContainer *rdc){

    // Now parsing the actual experiment data.
    if (!rdc->setCurrentStudy(VMDC::Study::GONOGO)){
        error = "Could not set current study: " + rdc->getError();
        return;
    }

    // Setting the trial list type which, for gonogo is always unique.

    QStringList trialTypes;
    trialTypes << "R<-" << "G<-" << "R->" <<  "G->";

    QStringList lines = raw_data.split("\n",Qt::SkipEmptyParts);
    QString current_trial = "";

    for (qint32 i =0; i < lines.size(); i++){

        QStringList tokens = lines.at(i).split(" ",Qt::SkipEmptyParts);
        if (tokens.size() == 2){
            if (current_trial != ""){
                this->finalizeOnlineFixations(rdc);
                rdc->finalizeDataSet();
                rdc->finalizeTrial();
            }
            current_trial = tokens.first();

            qint32 index = tokens.last().trimmed().toInt();
            QString type;
            if ((index >= 0) && (index < trialTypes.size())){
                type = trialTypes.at(index);
            }
            else{
                error = "Unknown trial type index: " + tokens.last() + ". From line: " + lines.at(i);
                return;
            }

            if (!rdc->addNewTrial(tokens.first().trimmed(),type)){
                error = "Error creating new GoNoGo trial: " + rdc->getError();
                return;
            }

            rdc->setCurrentDataSet(VMDC::DataSetType::UNIQUE);
        }
        else if (tokens.size() == 7){
            float timestamp,xr,yr,xl,yl,pr,pl;
            timestamp = tokens.at(0).toDouble();
            xr = tokens.at(1).toDouble();
            yr = tokens.at(2).toDouble();
            xl = tokens.at(3).toDouble();
            yl = tokens.at(4).toDouble();
            pr = tokens.at(5).toDouble();
            pl = tokens.at(6).toDouble();
            // We add the data vector.
            QVariantMap rawDataVector = ViewMindDataContainer::GenerateStdRawDataVector(timestamp,xr,yr,xl,yl,pr,pl);
            this->computeOnlineFixations(rdc,rawDataVector);
            rdc->addNewRawDataVector(rawDataVector);
        }
        else{
            error = "Badly formatted GoNoGo line " + lines.at(i);
            return;
        }

    }

    this->finalizeOnlineFixations(rdc);
    rdc->finalizeDataSet();
    rdc->finalizeTrial();

    if (!rdc->finalizeStudy()){
        error = "Finalizing Study: " + rdc->getError();
    }

    return;

}

////////////////////////////////////////////// SEPARATE RAW DATA FILE FROM HEADER ////////////////////////////////////////////

QStringList DatFileToViewMindDataContainer::separateStudyDescriptionFromData(const QString &file_path, const QString &header){

    QString codec = DatFileToViewMindDataContainer::checkCodecForFile(file_path);
    if (codec == "") codec = "UTF-8";

    QFile file(file_path);
    if (!file.open(QFile::ReadOnly)){
        error = "Could not open file for reading: " + file_path;
        return QStringList();
    }

    QTextStream reader(&file);
    if (codec == "UTF-8") reader.setCodec("UTF-8");
    else reader.setCodec("ISO 8859-1");
    QString content = reader.readAll();
    file.close();

    //qDebug() << content;

    QStringList parts = content.split(header,Qt::SkipEmptyParts);

    // There should be either two or three parts.
    if (parts.size() == 3){
        // Assuming that the first part is trash and ignore it.
        parts.removeFirst();
    }
    else if (parts.size() != 2){
        error = "Format error of input file. Split by header parts: " + QString::number(parts.size());
        return QStringList();
    }

    // The first part is the experiment description
    QString expDescription = parts.first();
    QString rawData = parts.last();
    parts.clear();

    // The version is the trimmed first line.
    QStringList lines = expDescription.split("\n",Qt::SkipEmptyParts);
    QString version = lines.first();
    lines.removeFirst();
    version = version.trimmed();

    // Regenerating the experiment again.
    expDescription = lines.join("\n");

    // Splitting by the commentary, if it exists.
    lines = expDescription.split("######",Qt::SkipEmptyParts);

    // Keeping only the last part.
    expDescription = lines.last();

    // Removing the first line one las time, as it is a configuration string.
    lines = expDescription.split("\n",Qt::SkipEmptyParts);
    lines.removeFirst();
    expDescription = lines.join("\n");

    // Now we get the resolution. The firs line of the data part.
    lines = rawData.split("\n",Qt::SkipEmptyParts);
    QString resolution = lines.first();
    lines.removeFirst();
    rawData = lines.join("\n");

    // Now we return the version, experiment description, resolution and rawdata
    QStringList ans; ans << version << expDescription << resolution << rawData;
    return ans;

}


////////////////////////////////////////////////////// CHECK CODEC FOR FILE ////////////////////////////////////////////
QString DatFileToViewMindDataContainer::checkCodecForFile(const QString &file){

    QString ans = "";

    QProcess p;
    QStringList args; args << file;
    p.start("file",args);
    p.setReadChannel(QProcess::StandardOutput);
    p.waitForFinished();

    QString std = p.readAllStandardOutput();
    QStringList parts = std.split(":",Qt::SkipEmptyParts);
    if (parts.size() != 2) return ans;
    QStringList tokens = parts.last().split(" ",Qt::SkipEmptyParts);
    if (tokens.first() == "ISO-8859") return "ISO 8859-1";
    else return "UTF-8";

    return "";
}
