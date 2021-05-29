#include "datfiletorawdatacontainer.h"

DatFileToRawDataContainer::DatFileToRawDataContainer()
{

}

QString DatFileToRawDataContainer::getError() const {
    return error;
}

RawDataContainer DatFileToRawDataContainer::fromDatFile(const QString &dat_file, const ConfigurationManager &configmng){

    error = "";
    RawDataContainer rdc;

    QFileInfo info(dat_file);

    DatFileInfoInDir::DatInfo dat_info = DatFileInfoInDir::getDatFileInformation(info.baseName());
    if (dat_info.category == ""){
        error = "Could not determine the study type for " + dat_file;
        return rdc;
    }

    // Valid Eye Conversion. The code used for the eyes correspond to the string location in this list.
    QList<RawDataContainer::StudyConfigurationValue> eyes; eyes << RawDataContainer::SCV_EYE_LEFT << RawDataContainer::SCV_EYE_RIGHT << RawDataContainer::SCV_EYE_BOTH;

    RawDataContainer::StudyConfiguration study_configuration;
    study_configuration.insert(RawDataContainer::SCP_EYES,eyes.at(dat_info.validEye.toInt()));


    RawDataContainer::SubjectData subject_data;
    subject_data.insert(RawDataContainer::SF_AGE,"");
    subject_data.insert(RawDataContainer::SF_INSTITUTION_PROVIDED_ID,"");
    subject_data.insert(RawDataContainer::SF_BIRHTCOUNTRY,"");
    subject_data.insert(RawDataContainer::SF_BIRTHDATE,"");
    subject_data.insert(RawDataContainer::SF_GENDER,"");
    subject_data.insert(RawDataContainer::SF_LASTNAME,"");
    subject_data.insert(RawDataContainer::SF_NAME,"");
    subject_data.insert(RawDataContainer::SF_YEARS_FORMATION,"");
    subject_data.insert(RawDataContainer::SF_LOCAL_ID,"");

    if (configmng.containsKeyword(CONFIG_PATIENT_AGE)){
        subject_data.insert(RawDataContainer::SF_AGE,configmng.getString(CONFIG_PATIENT_AGE));
    }
    if (!rdc.setSubjectData(subject_data)){
        error = "Could not set subject data" + rdc.getError();
        return rdc;
    }

    QString header;
    RawDataContainer::StudyType st;
    RawDataContainer::TrialListType tlt;

    QString code = dat_info.category.mid(0,2);
    if (code == "RD"){
        if (dat_info.extraInfo == "EN"){
            study_configuration.insert(RawDataContainer::SCP_LANGUAGE,RawDataContainer::SCV_LANG_EN);
        }
        else if (dat_info.extraInfo == "ES"){
            study_configuration.insert(RawDataContainer::SCP_LANGUAGE,RawDataContainer::SCV_LANG_ES);
        }
        else if (dat_info.extraInfo == "DE"){
            study_configuration.insert(RawDataContainer::SCP_LANGUAGE,RawDataContainer::SCV_LANG_DE);
        }
        else if (dat_info.extraInfo == "IS"){
            study_configuration.insert(RawDataContainer::SCP_LANGUAGE,RawDataContainer::SCV_LANG_IS);
        }
        else if (dat_info.extraInfo == "FR"){
            study_configuration.insert(RawDataContainer::SCP_LANGUAGE,RawDataContainer::SCV_LANG_FR);
        }
        else{
            error = "Unrecognized reading languange: " + dat_info.extraInfo;
            return rdc;
        }
        header = HEADER_READING_EXPERIMENT;
        st = RawDataContainer::STUDY_READING;
        tlt = RawDataContainer::TLT_UNIQUE;
    }
    else if ((code == "UC") || (code == "BC")){


        if (code == "BC"){
            study_configuration.insert(RawDataContainer::SCP_BINDING_TYPE,RawDataContainer::SCV_BINDING_TYPE_BOUND);
            tlt = RawDataContainer::TLT_BOUND;
        }
        else{
            study_configuration.insert(RawDataContainer::SCP_BINDING_TYPE,RawDataContainer::SCV_BINDING_TYPE_UNBOUND);
            tlt = RawDataContainer::TLT_UNBOUND;
        }

        if (dat_info.extraInfo.size() != 2){
            error = "Expected extra info of a binding file two have only 2 characters: " + dat_info.extraInfo;
            return rdc;
        }

        if (dat_info.extraInfo.at(0) == '2') study_configuration.insert(RawDataContainer::SCP_NUMBER_OF_TARGETS,RawDataContainer::SCV_BINDING_TARGETS_2);
        else if (dat_info.extraInfo.at(0) == '3') study_configuration.insert(RawDataContainer::SCP_NUMBER_OF_TARGETS,RawDataContainer::SCV_BINDING_TARGETS_3);
        else{
            error = QString("Unknown number of targets from extra info: ") + dat_info.extraInfo.at(0);
            return rdc;
        }

        if (dat_info.extraInfo.at(1) == 'l') study_configuration.insert(RawDataContainer::SCP_TARGET_SIZE,RawDataContainer::SCV_BINDING_TARGETS_LARGE);
        else if (dat_info.extraInfo.at(1) == 's') study_configuration.insert(RawDataContainer::SCP_NUMBER_OF_TARGETS,RawDataContainer::SCV_BINDING_TARGETS_SMALL);
        else{
            error = QString("Unknown target size from extra info: ") + dat_info.extraInfo.at(0);
            return rdc;
        }

        header = HEADER_IMAGE_EXPERIMENT;
        st = RawDataContainer::STUDY_BINDING;

    }
    else if (code == "NB"){
        // NBack RT study.
        st = RawDataContainer::STUDY_NBACKRT;
        header = HEADER_NBACKRT_EXPERIMENT;
        tlt = RawDataContainer::TLT_UNIQUE;
    }
    else if (code == "GN"){
        st = RawDataContainer::STUDY_GONOGO;
        header = HEADER_GONOGO_EXPERIMENT;
        tlt = RawDataContainer::TLT_UNIQUE;
    }
    else{
        error = "Unrecognized code " + code + " from " + dat_file;
        return rdc;
    }

    QStringList more_study_info = separateStudyDescriptionFromData(dat_file,header);
    // Answer is: version << expDescription << resolution << rawData;
    if (more_study_info.size() != 4) {
        return rdc;
    }

    QString resolution = more_study_info.at(2);
    QStringList w_and_h = resolution.split(" ",Qt::SkipEmptyParts);
    if (w_and_h.size() != 2){
        error = "Invalid resolution string " + resolution;
        return rdc;
    }

    // Setting the metadata.
    RawDataContainer::Metadata metadata;
    metadata.insert(RawDataContainer::MF_DATE,dat_info.date);
    metadata.insert(RawDataContainer::MF_HOUR,dat_info.hour);
    metadata.insert(RawDataContainer::MF_INSTITUTION_ID,"");
    metadata.insert(RawDataContainer::MF_INSTITUTION_INSTANCE,"");
    metadata.insert(RawDataContainer::MF_INSTITUTION_KEY,"");
    metadata.insert(RawDataContainer::MF_INSTITUTION_NAME,"");
    metadata.insert(RawDataContainer::MF_PROCESSING_PARAMETER_KEY,"0");
    if (!rdc.setMetadata(metadata)){
        error = "Setting the metadata: " + rdc.getError();
        return rdc;
    }

    RawDataContainer::ProcessingParameters pp;
    pp.insert(RawDataContainer::PP_RESOLUTION_WIDTH,w_and_h.first().trimmed());
    pp.insert(RawDataContainer::PP_RESOLUTION_HEIGHT,w_and_h.last().trimmed());

    // In case they are needed as numbers.
    qreal rw = w_and_h.first().trimmed().toDouble();
    qreal rh = w_and_h.last().trimmed().toDouble();

    if (st == RawDataContainer::STUDY_NBACKRT){
        // We need to get the target boxes.
        bool vr = false;
        qreal xpx2mm = 0.25;
        qreal ypx2mm = 0.25;
        if (configmng.containsKeyword(CONFIG_VR_ENABLED)){
            vr = configmng.getBool(CONFIG_VR_ENABLED);
        }
        if (vr){
            xpx2mm = 0.2;
            ypx2mm = 0.2;
        }

        FieldingParser parser;
        if (!parser.parseFieldingExperiment(more_study_info.at(0) + "\n" + more_study_info.at(1),rw,rh,xpx2mm,ypx2mm)){
            error = "Parsing fielding experiment: " + parser.getError();
            return rdc;
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
            if ((i == TARGET_BOX_2) || (i == TARGET_BOX_5)){
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
        pp.insert(RawDataContainer::PP_NBACK_HITBOXES,modHitBoxes);
    }
    else if (st == RawDataContainer::STUDY_GONOGO){
        GoNoGoParser gngparser;
        if (!gngparser.parseGoNoGoExperiment(more_study_info.at(0) + "\n" + more_study_info.at(1),rw,rh)){
            error = "Error parsing go no go experiment: " + gngparser.getError();
            return rdc;
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

        pp.insert(RawDataContainer::PP_GONOGO_HITBOXES,target_boxes);
    }


    if (!rdc.setProcessingParameters(pp)){
        error = "Setting the processing parameters : " + rdc.getError();
        return rdc;
    }

    if (!rdc.addStudy(st,study_configuration,more_study_info.at(1),more_study_info.at(0))){
        error = "Setting the study: " + rdc.getError();
        return rdc;
    }

    rdc = parseStudyData(more_study_info.at(3),more_study_info.at(1),st,rdc,tlt);

    return rdc;

}

RawDataContainer DatFileToRawDataContainer::parseStudyData(const QString &raw_data,
                                                           const QString &exp,
                                                           RawDataContainer::StudyType st,
                                                           RawDataContainer rdc,
                                                           RawDataContainer::TrialListType tlt){
    switch(st){
    case RawDataContainer::STUDY_READING:
        rdc = parseReading(raw_data,exp,rdc);
        break;
    case RawDataContainer::STUDY_BINDING:
        rdc = parseBinding(raw_data,rdc,tlt);
        break;
    case RawDataContainer::STUDY_GONOGO:
        rdc = parseGoNoGo(raw_data,rdc);
        break;
    case RawDataContainer::STUDY_NBACKMS:
        error = "NBackMS parsing unimplemented";
        break;
    case RawDataContainer::STUDY_NBACKRT:
        rdc = parseNBackRT(raw_data,exp,rdc);
        break;
    case RawDataContainer::STUDY_NBACKVS:
        error = "NBackVS parsing unimplemented";
        break;
    case RawDataContainer::STUDY_PERCEPTION:
        error = "Perception parsing unimplemented";
        break;
    }
    return rdc;
}

////////////////////////////////////////////// PROCESSING FUNCTIONS ////////////////////////////////////////////

RawDataContainer DatFileToRawDataContainer::parseReading(const QString &raw_data, const QString &exp, RawDataContainer rdc){

    // Creating the id -> sentence map.
    QStringList lines = exp.split("\n",Qt::SkipEmptyParts);
    QMap<QString,QString> sentences;
    for (qint32 i = 0; i < lines.size(); i++){
        QStringList parts = lines.at(i).split(":",Qt::SkipEmptyParts);
        if (parts.size() != 2){
            error = "Unexpected sentece in reading experiment: " + lines.at(i);
            return rdc;
        }
        QString id_str = parts.first();
        while (id_str.size() < 4) id_str = "0" + id_str;
        sentences[id_str] = parts.last();
    }

    // Now parsing the actual experiment data.
    rdc.setCurrentStudy(RawDataContainer::STUDY_READING);

    // Setting the trial list type which, for reading, is always unique.
    rdc.setCurrentTrialListType(RawDataContainer::TLT_UNIQUE);

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
                rdc.finalizeDataSet();
                // And we finalize the trial.
                rdc.finalizeTrial();
            }

            // We need to create a new trial.
            RawDataContainer::TrialConfiguration tconfig;
            tconfig.insert(RawDataContainer::TF_SENTENCE,sentences.value(id));
            size = sentences.value(id).split(" ",Qt::SkipEmptyParts).size();

            if (!rdc.addNewTrial(id,tconfig)){
                error = "Error creating new trial for " + id  + " on line " + lines.at(i) + ". Error was: " + rdc.getError();
                return rdc;
            }

            rdc.setCurrentDataSet(RawDataContainer::DST_UNIQUE);
        }

        if (!sentences.contains(id)){
            error = "Found data with id " + id + " which does not belong to a sentence id. Line: " + lines.at(i);
            return rdc;
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
                return rdc;
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
                return rdc;
            }

            // We add the data vector.
            rdc.addNewRawDataVector(RawDataContainer::GenerateReadingRawDataVector(timestamp,xr,yr,xl,yl,pr,pl,char_r,char_l,word_r,word_l));

        }
    }

    // Finalize the last data set and response.
    rdc.finalizeDataSet();
    rdc.finalizeTrial();

    // And then finalizing the study.
    if (!rdc.finalizeStudy()){
        error = "Finalizing Study: " + rdc.getError();
    }

    return rdc;
}

//////////////////////////////////////// NBACKRT

RawDataContainer DatFileToRawDataContainer::parseNBackRT(const QString &raw_data, const QString &exp, RawDataContainer rdc){

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
    rdc.setCurrentStudy(RawDataContainer::STUDY_NBACKRT);

    // Setting the trial list type which, for reading, is always unique.
    rdc.setCurrentTrialListType(RawDataContainer::TLT_UNIQUE);

    QMap<QString,RawDataContainer::DataSetType> trialPartToDataSetType;
    trialPartToDataSetType["1"] = RawDataContainer::DST_ENCODING_1;
    trialPartToDataSetType["2"] = RawDataContainer::DST_ENCODING_2;
    trialPartToDataSetType["3"] = RawDataContainer::DST_ENCODING_3;
    trialPartToDataSetType["4"] = RawDataContainer::DST_RETRIEVAL_1;

    QStringList lines = raw_data.split("\n",Qt::SkipEmptyParts);
    QString current_trial = "";

    for (qint32 i =0; i < lines.size(); i++){

        QStringList tokens = lines.at(i).split(" ",Qt::SkipEmptyParts);

        if (tokens.size() == 2){
            QString trialID = tokens.first().trimmed();
            QString trialPart = tokens.last().trimmed();

            if (current_trial != ""){
                rdc.finalizeDataSet();
            }

            if (trialID != current_trial){

                if (current_trial != ""){
                    rdc.finalizeTrial();
                }

                if (!trialDesc.contains(trialID)){
                    error = "Found trial id in NBackRT not found in description: " + trialID + " on line " + lines.at(i);
                    return rdc;
                }

                RawDataContainer::TrialConfiguration tconfig;
                tconfig.insert(RawDataContainer::TF_TRIAL_TYPE,trialDesc.value(trialID));
                if (!rdc.addNewTrial(trialID,tconfig)){
                    error = "Error creating new trial for " + trialID  + " on line " + lines.at(i) + ". Error was: " + rdc.getError();
                    return rdc;
                }
            }

            if (!trialPartToDataSetType.contains(trialPart)){
                error = "Unrecognized trial part " + trialPart + " for NBack RT: " + lines.at(i);
                return rdc;
            }

            rdc.setCurrentDataSet(trialPartToDataSetType.value(trialPart));

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
            rdc.addNewRawDataVector(RawDataContainer::GenerateStdRawDataVector(timestamp,xr,yr,xl,yl,pr,pl));
        }
        else{
            error = "Badly formed for NBack RT: " + lines.at(i);
            return rdc;
        }
    }

    // Finalize the last data set and response.
    rdc.finalizeDataSet();
    rdc.finalizeTrial();

    // And then finalizing the study.
    if (!rdc.finalizeStudy()){
        error = "Finalizing Study: " + rdc.getError();
    }

    return rdc;

}


//////////////////////////////////////// Binding
RawDataContainer DatFileToRawDataContainer::parseBinding(const QString &raw_data, RawDataContainer rdc, RawDataContainer::TrialListType tlt){

    // Now parsing the actual experiment data.
    if (!rdc.setCurrentStudy(RawDataContainer::STUDY_BINDING)){
        error = "Could not set current study: " + rdc.getError();
        return rdc;
    }

    // Setting the trial list type which, for binding can be either bound or unbound
    rdc.setCurrentTrialListType(tlt);

    QStringList lines = raw_data.split("\n",Qt::SkipEmptyParts);
    QString current_trial = "";
    RawDataContainer::DataSetType current_data_set;
    //current_data_set = RawDataContainer::DST_ENCODING_1;
    bool trial_finalized = false;

    for (qint32 i =0; i < lines.size(); i++){

        trial_finalized = false;

        if (!lines.at(i).contains("->")){
            QStringList tokens = lines.at(i).split(" ",Qt::SkipEmptyParts);

            if (tokens.size() == 2){
                QString trialID = tokens.first().trimmed();
                QString trialPart = tokens.last().trimmed();

                if (trialPart == "0") current_data_set = RawDataContainer::DST_ENCODING_1;
                else if (trialPart == "1") current_data_set = RawDataContainer::DST_RETRIEVAL_1;
                else{
                    error = "Bad trial part in Binding header for data set: " + lines.at(i);
                    return rdc;
                }


                if (current_data_set == RawDataContainer::DST_ENCODING_1){

                    QString type = "";
                    if (trialID.toUpper().contains("SAME")) type = "S";
                    else if (trialID.toUpper().contains("DIFFERENT")) type = "D";
                    else{
                        error = "Binding trial header, could not determine the trial type: " + lines.at(i);
                        return rdc;
                    }

                    // Add a new trial.
                    RawDataContainer::TrialConfiguration tconfig;
                    tconfig.insert(RawDataContainer::TF_TRIAL_TYPE,type);
                    if (!rdc.addNewTrial(trialID,tconfig)){
                        error = "Binding creating a new trial: " + rdc.getError();
                        return rdc;
                    }
                    rdc.setCurrentDataSet(RawDataContainer::DST_ENCODING_1);
                }
                else {
                    rdc.finalizeDataSet(); // Finalize the previous encoding.
                    rdc.setCurrentDataSet(RawDataContainer::DST_RETRIEVAL_1);
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
                rdc.addNewRawDataVector(RawDataContainer::GenerateStdRawDataVector(timestamp,xr,yr,xl,yl,pr,pl));
            }
        }
        // This line contains a ->
        else{
            QStringList tokens = lines.at(i).split("->",Qt::SkipEmptyParts);
            if (tokens.size() != 2){
                error = "Bad answer line in binding data:  " + lines.at(i);
                return rdc;
            }
            rdc.finalizeDataSet(); // Finalize the retrieval data set
            rdc.finalizeTrial(tokens.last()); // Finalize trial.
            trial_finalized = true;
        }

    }

    if (!trial_finalized){
        error = "Something went wrong when parsing binding info. The file did not end with a user response";
    }

    if (!rdc.finalizeStudy()){
        error = "Finalizing Study: " + rdc.getError();
    }

    return rdc;
}


//////////////////////////////////////// GoNoGo
RawDataContainer DatFileToRawDataContainer::parseGoNoGo(const QString &raw_data, RawDataContainer rdc){

    // Now parsing the actual experiment data.
    if (!rdc.setCurrentStudy(RawDataContainer::STUDY_GONOGO)){
        error = "Could not set current study: " + rdc.getError();
        return rdc;
    }

    // Setting the trial list type which, for gonogo is always unique.
    rdc.setCurrentTrialListType(RawDataContainer::TLT_UNIQUE);

    QStringList trialTypes;
    trialTypes << "R<-" << "G<-" << "R->" <<  "G->";

    QStringList lines = raw_data.split("\n",Qt::SkipEmptyParts);
    QString current_trial = "";

    for (qint32 i =0; i < lines.size(); i++){

        QStringList tokens = lines.at(i).split(" ",Qt::SkipEmptyParts);
        if (tokens.size() == 2){
            if (current_trial != ""){
                rdc.finalizeDataSet();
                rdc.finalizeTrial();
            }
            current_trial = tokens.first();

            qint32 index = tokens.last().trimmed().toInt();
            QString type;
            if ((index >= 0) && (index < trialTypes.size())){
                type = trialTypes.at(index);
            }
            else{
                error = "Unknown trial type index: " + tokens.last() + ". From line: " + lines.at(i);
                return rdc;
            }

            RawDataContainer::TrialConfiguration tconfig;
            tconfig.insert(RawDataContainer::TF_TRIAL_TYPE,type);
            if (!rdc.addNewTrial(tokens.first().trimmed(),tconfig)){
                error = "Error creating new GoNoGo trial: " + rdc.getError();
                return rdc;
            }

            rdc.setCurrentDataSet(RawDataContainer::DST_UNIQUE);
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
            rdc.addNewRawDataVector(RawDataContainer::GenerateStdRawDataVector(timestamp,xr,yr,xl,yl,pr,pl));
        }
        else{
            error = "Badly formatted GoNoGo line " + lines.at(i);
            return rdc;
        }

    }

    rdc.finalizeDataSet();
    rdc.finalizeTrial();

    if (!rdc.finalizeStudy()){
        error = "Finalizing Study: " + rdc.getError();
    }

    return rdc;

}

////////////////////////////////////////////// SEPARATE RAW DATA FILE FROM HEADER ////////////////////////////////////////////

QStringList DatFileToRawDataContainer::separateStudyDescriptionFromData(const QString &file_path, const QString &header){

    QString codec = DatFileToRawDataContainer::checkCodecForFile(file_path);
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
QString DatFileToRawDataContainer::checkCodecForFile(const QString &file){

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
