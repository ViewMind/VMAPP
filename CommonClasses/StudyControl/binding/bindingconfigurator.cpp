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

    Q_UNUSED(studyConfig)

    // Now we parse the trial. If it worked the configuration is loaded with the NBack trials.
    if (!parseStudyDescription()) return false;

    // If all good, then we set the number of trials.
    if (this->shortStudies){
        this->configuration[RRS::StudyConfigurationFields::N_OF_TRIALS] = N_TRIALS_IN_SHORT_STUDIES;
    }
    else {
        this->configuration[RRS::StudyConfigurationFields::N_OF_TRIALS] = this->configuration.value(RRS::StudyConfigurationFields::BINDING_TRIALS).toList().size();
    }

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

    for (qint32 i = 0; i < N; i++){

        // Read the next 7 lines.
        QVariantMap trial;
        QList<QStringList> trialMatrix;
        qint32 start = i*7;
        qint32 end   = start + 7;
        for (qint32 j = start; j < end; j++){

            QStringList row = lines.at(j).split(" ",Qt::SkipEmptyParts);


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
        //qDebug() << "Adding the created trial";
        trials << trial;
    }

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

