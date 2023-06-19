#include "studyconfigurator.h"

StudyConfigurator::StudyConfigurator(){

}

QVariantMap StudyConfigurator::getConfiguration() const{
    return this->configuration;
}

QString StudyConfigurator::getError() const{
    return this->error;
}

QString StudyConfigurator::getDefaultStudyEye() const{
    return this->defaultStudyEye;
}

QString StudyConfigurator::getVersionString() const{
    return this->versionString;
}

QString StudyConfigurator::getStudyDescriptionContents() const {
    return this->studyDescriptionContent;
}

bool StudyConfigurator::createStudyConfiguration(const QVariantMap &studyConfig, qreal sample_f, qreal md_percent, const QString &study_type, const bool short_studies){
    error = "";
    configuration[RRS::StudyConfigurationFields::VALID_EYE]  = studyConfig.value(VMDC::StudyParameter::VALID_EYE).toString();
    configuration[RRS::StudyConfigurationFields::MD_PERCENT] = md_percent;
    configuration[RRS::StudyConfigurationFields::SAMPLE_F]   = sample_f;
    shortStudies = short_studies;
    studyType = study_type;
    return this->studySpecificConfiguration(studyConfig);
}

bool StudyConfigurator::openStudyDescriptionFile(const QString &filename){
    QFile file(filename);
    if (!file.open(QFile::ReadOnly)){
        error = "Unable to open file description '" + filename + "'";
        return false;
    }
    QTextStream reader(&file);
    reader.setEncoding(QStringConverter::Utf8);
    studyDescriptionContent = reader.readAll();
    file.close();
    return true;
}


bool StudyConfigurator::studySpecificConfiguration(const QVariantMap &studyConfig){
    Q_UNUSED(studyConfig);
    return true;
}



