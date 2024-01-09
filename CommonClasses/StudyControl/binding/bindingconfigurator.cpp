#include "bindingconfigurator.h"

BindingConfigurator::BindingConfigurator(bool isBC, qint32 ntargets){
    // We need to figure out the proper instruction file.
    if (isBC){
        if (ntargets == 3){
            this->studyDescriptionFile = ":/CommonClasses/StudyControl/binding/descriptions/bc_3.dat";
        }
        else {
            this->studyDescriptionFile = ":/CommonClasses/StudyControl/binding/descriptions/bc.dat";
        }
    }
    else {
        if (ntargets == 3){
            this->studyDescriptionFile = ":/CommonClasses/StudyControl/binding/descriptions/uc_3.dat";
        }
        else {
            this->studyDescriptionFile = ":/CommonClasses/StudyControl/binding/descriptions/uc.dat";
        }
    }
    this->number_of_targets = ntargets;
    this->isBC = isBC;
}

bool BindingConfigurator::studySpecificConfiguration(const QVariantMap &studyConfig){


    // Now we parse the trial. If it worked the configuration is loaded with the NBack trials.
    if (!parseStudyDescription()) return false;

    // THIS IS DEBUG FUNCTION ONLY
    // trialTypeBreakDown();

    // If all good, then we set the number of trials.
    if (this->shortStudies){
        this->configuration[RRS::StudyConfigurationFields::N_OF_TRIALS] = N_TRIALS_IN_SHORT_STUDIES;
    }
    else {

        qint32 ntrials = studyConfig.value(VMDC::StudyParameter::NUM_TRIALS).toInt();
        if (ntrials > 0){
            this->configuration[RRS::StudyConfigurationFields::N_OF_TRIALS] = ntrials;

        }        
        else {
            this->configuration[RRS::StudyConfigurationFields::N_OF_TRIALS] = this->configuration.value(RRS::StudyConfigurationFields::BINDING_TRIALS).toList().size();
        }

    }

    //qDebug() << "Configured number of trials to " << this->configuration[RRS::StudyConfigurationFields::N_OF_TRIALS].toInt();

    // The study name and the number of targets are the only configuration parameters required.
    if (this->isBC){
        this->configuration[RRS::StudyConfigurationFields::STUDY] = VMDC::Study::BINDING_BC;
    }
    else {
        this->configuration[RRS::StudyConfigurationFields::STUDY] = VMDC::Study::BINDING_UC;
    }

    this->configuration[RRS::StudyConfigurationFields::NUMBER_OF_TARGETS] = this->number_of_targets;
    configuration[RRS::StudyConfigurationFields::IS_STUDY_3D] = false;

    return true;

}


bool BindingConfigurator::parseStudyDescription(){
    QStringList lines = this->studyDescriptionContent.split("\n",Qt::SkipEmptyParts);
    QStringList tokens = lines.first().split(" ",Qt::SkipEmptyParts);
    this->versionString = tokens.last();

    //qDebug() << "Loading version string" << this->versionString;

    // Make sure the file matches the number of targets passed.
    if (tokens.first().toInt() != this->number_of_targets){
        this->error = "Loaded a description file for " + tokens.first() + " targets but constructed a configurator for " + QString::number(this->number_of_targets);
        return false;
    }

    lines.removeFirst();

    // Make sure that the remaining number of lines is divisible by 7.
    if ((lines.size() % 7) != 0){
        this->error = "The number of lines in the description file is NOT divisible by 7: " + QString::number(lines.size());
        return false;
    }

    // Now we create the token matrix.
    qint32 N = lines.size()/7;

    QVariantList trials;
    //QStringList all_color_list;

    for (qint32 i = 0; i < N; i++){

        // Read the next 7 lines.
        QVariantMap trial;
        QList<QStringList> trialMatrix;
        qint32 start = i*7;
        qint32 end   = start + 7;
        for (qint32 j = start; j < end; j++){

            QStringList row = lines.at(j).split(" ",Qt::SkipEmptyParts);

//            for (qint32 k = 0; k < row.size(); k++){
//                if (row.at(k).size() == 6){
//                    if (!all_color_list.contains(row.at(k))) all_color_list << row.at(k);
//                }
//            }


            if (j == start){

                // This is the description of the trial.
                if (row.size() != 3){
                    this->error = "Expected trial description line and got: " + lines.at(j);
                    return false;
                }
                trial[RRS::Binding::Trial::NAME] = row.at(0);
                trial[RRS::Binding::Trial::IS_SAME] = (row.at(2) == "s");

            }
            else {

                // All token list sizes should be equal to the number of targets.
                if (row.size() != this->number_of_targets){
                    this->error = "Description line " + lines.at(j) + " contains an invalid number of tokens";
                    return false;
                }

                trialMatrix << row;

            }

        }

        // Now we parse the matrix into the flag list.
        //qDebug() << "Creating Binding Matrix";
        if (!createBindingTrialFromTrialStringMatrix(trialMatrix,&trial)) return false;
        trials << trial;
    }

//    qDebug() << "All unique colors";
//    for (qint32 i = 0; i < all_color_list.size(); i++){
//        qDebug() << all_color_list.at(i);
//    }

    this->configuration[RRS::StudyConfigurationFields::BINDING_TRIALS] = trials;

    return true;

}


bool BindingConfigurator::createBindingTrialFromTrialStringMatrix(const QList<QStringList> &matrix, QVariantMap *trial){

    if (matrix.size() != 6){
        this->error = "Got a trial string matrix of height: " + matrix.size();
        return false;
    }

    QVariantList encoding;
    QVariantList retrieval;

    QVariantMap flag;
    flag = createFlag(matrix.at(0).at(0),matrix.at(1).at(0),matrix.at(2).at(0));
    if (flag.isEmpty()) return false;
    encoding << flag;

    flag = createFlag(matrix.at(0).at(1),matrix.at(1).at(1),matrix.at(2).at(1));
    if (flag.isEmpty()) return false;
    encoding << flag;

    if (this->number_of_targets == 3){
        flag = createFlag(matrix.at(0).at(2),matrix.at(1).at(2),matrix.at(2).at(2));
        if (flag.isEmpty()) return false;
        encoding << flag;
    }


    flag = createFlag(matrix.at(3).at(0),matrix.at(4).at(0),matrix.at(5).at(0));
    if (flag.isEmpty()) return false;
    retrieval << flag;

    flag = createFlag(matrix.at(3).at(1),matrix.at(4).at(1),matrix.at(5).at(1));
    if (flag.isEmpty()) return false;
    retrieval << flag;

    if (this->number_of_targets == 3){
        flag = createFlag(matrix.at(3).at(2),matrix.at(4).at(2),matrix.at(5).at(2));
        if (flag.isEmpty()) return false;
        retrieval << flag;
    }

    trial->insert(RRS::Binding::Trial::ENCODE,encoding);
    trial->insert(RRS::Binding::Trial::RETRIEVAL,retrieval);

    return true;
}

QVariantMap BindingConfigurator::createFlag(const QString &pos, const QString &back, const QString &cross){
    QVariantMap flag;
    if (!parseFlagPositions(pos,&flag)) return flag;
    flag[RRS::Binding::Flag::BACK] = back;
    flag[RRS::Binding::Flag::CROSS] = cross;
    return flag;
}

bool BindingConfigurator::parseFlagPositions(const QString &pos, QVariantMap *flag){
    bool ok = false;
    qint32 pcode = pos.toInt(&ok);
    if (!ok){
        error = "Invalid postion code: " + pos;
        return false;
    }
    qint32 col = qFloor(pcode/10);
    qint32 row = pcode - col*10;

    if (col >= this->number_of_targets){
        error = "The col index in the pcode " + pos + " should be lower than "
                + QString::number(this->number_of_targets);
        return false;
    }
    if (row >= 3){
        error = "The row index in the pcode " + pos + " should be lower than 3";
        return false;
    }

    flag->insert(RRS::Binding::Flag::ROW,row);
    flag->insert(RRS::Binding::Flag::COL,col);
    return true;
}

////////////////////////////////////////////////////////////// DEBUG FUNCTIONS //////////////////////////////////////////////////////

void BindingConfigurator::verifyTrial(const QVariantMap &trial){

    bool issame = trial.value(RRS::Binding::Trial::IS_SAME).toBool();
    QString name = trial.value(RRS::Binding::Trial::NAME).toString();
    QVariantList encode = trial.value(RRS::Binding::Trial::ENCODE).toList();
    QVariantList retrieval = trial.value(RRS::Binding::Trial::RETRIEVAL).toList();

    qint32 n = encode.size();

    if (n != retrieval.size()){
        qDebug() << "Trial " << name << "has " << encode.size() << "in encode and in retrieval it has"  << retrieval.size();
        return;
    }

    qint32 same = 0;
    for (qint32 i = 0; i < n; i++){
        QString back  = retrieval.at(i).toMap().value(RRS::Binding::Flag::BACK).toString();
        QString cross = retrieval.at(i).toMap().value(RRS::Binding::Flag::CROSS).toString();
        for (qint32 j = 0; j < n; j++){
            QString b  = encode.at(j).toMap().value(RRS::Binding::Flag::BACK).toString();
            QString c = encode.at(j).toMap().value(RRS::Binding::Flag::CROSS).toString();
            if ((back == b) && (cross == c)){
                same++;
                break;
            }
        }
    }

    qint32 ndiff = n - same;

    if ((same == n) && (!issame)){
        qDebug() << "Trial" << name << "is not marked as same but it IS same";
        Debug::prettyPrintQVariantMap(trial);
    }
    else if ((same != n) && (issame)){
        qDebug() << "Trial" << name << "is marked as same but it IS DIFFERENT";
        Debug::prettyPrintQVariantMap(trial);
    }
    else if ((!issame) && (ndiff != 2)){
        // Correctness of trial type different. In both 2 and 3 targets at 2 targets must change.
        qDebug() << "Trial" << name << "is DIFFERENT but has diff flag count that is not 2 but " << ndiff;
        Debug::prettyPrintQVariantMap(trial);

    }

}

void BindingConfigurator::trialTypeBreakDown(){

    QString type = "BC";
    if (!this->isBC){
        type = "UC";
    }
    type = type + "-" + QString::number(this->number_of_targets);
    qDebug() << "Binding" << type << "Version" << this->versionString;

    QVariantList trialList = this->configuration.value(RRS::StudyConfigurationFields::BINDING_TRIALS).toList();

    // The breakdown should be. The first 10, the Next 22 and the Last 10, for a total of 42 Trials.
    qint32 N = 42;
    if (trialList.size() != N){
        qDebug() << "Trial List is NOT " << N;
        return;
    }

    qint32 same = 0;
    qint32 number;

    number = 10;
    same = verifyAndCount(trialList,0,number);
    qDebug() << "   First 10 Trials S: " << same << "D: " << (number - same);

    number = 22;
    same = verifyAndCount(trialList,10,number);
    qDebug() << "   Middle 22 Trials S: " << same << "D: " << (number - same);

    number = 10;
    same = verifyAndCount(trialList,32,number);
    qDebug() << "   Last 10 Trials S: " << same << "D: " << (number - same);

}

qint32 BindingConfigurator::verifyAndCount(const QVariantList &list, qint32 start, qint32 number){

    qint32 same = 0;
    for (qint32 i = start; i < (start + number); i++){
        QVariantMap trial = list.at(i).toMap();
        verifyTrial(trial);
        if (trial.value(RRS::Binding::Trial::IS_SAME).toBool()) same++;
    }
    return same;

}
