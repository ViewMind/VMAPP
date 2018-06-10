#include "loader.h"

Loader::Loader(QObject *parent, ConfigurationManager *c) : QObject(parent)
{

    // Loading the configuration file and checking for the must have configuration
    ConfigurationManager::CommandVerifications cv;
    ConfigurationManager::Command cmd;

    loadingError = false;
    configuration = c;

    cmd.clear();
    cmd.type = ConfigurationManager::VT_REAL;
    cv[CONFIG_XPX_2_MM] = cmd;
    cv[CONFIG_YPX_2_MM] = cmd;
    cv[CONFIG_READING_PX_TOL] = cmd;
    cv[CONFIG_TCP_PORT] = cmd;

    cmd.clear();
    cv[CONFIG_SERVER_ADDRESS] = cmd;

    cmd.clear();
    cmd.type = ConfigurationManager::VT_BOOL;
    cmd.optional = true;
    cv[CONFIG_DUAL_MONITOR_MODE] = cmd;

    // Optional strings
    cmd.clear();
    cmd.optional = true;
    cv[CONFIG_DOCTOR_NAME] = cmd;
    cv[CONFIG_DOCTOR_EMAIL] = cmd;
    cv[CONFIG_OUTPUT_DIR] = cmd;
    cv[CONFIG_REPORT_LANGUAGE] = cmd;
    cv[CONFIG_SELECTED_ET] = cmd;
    cv[CONFIG_SSLSERVER_PATH] = cmd;

    // Optional booleans
    cmd.clear();
    cmd.optional = true;
    cmd.type = ConfigurationManager::VT_BOOL;
    cv[CONFIG_DEMO_MODE] = cmd;
    cv[CONFIG_BINDING_DEFAULT] = cmd;
    cv[CONFIG_BINDING_NUM_TARGETS] = cmd;
    cv[CONFIG_BINDING_USE_NUMBERS] = cmd;

    configuration->setupVerification(cv);

    if (!configuration->loadConfiguration(FILE_CONFIGURATION,COMMON_TEXT_CODEC)){
        logger.appendError("Errors loading the configuration file: " + configuration->getError());
        loadingError = true;
    }

    if (!loadingError){
        loadDefaultConfigurations();
    }

    changeLanguage();

}

QRect Loader::frameSize(QObject *window)
{
    QQuickWindow *qw = qobject_cast<QQuickWindow *>(window);
    if(qw)
        return qw->frameGeometry();
    return QRect();
}

QString Loader::hasValidOutputRepo(const QString &dirToCheck){

    if (dirToCheck != ""){
        QUrl url(dirToCheck);
        QString fileloc = url.toLocalFile();

        if (fileloc.isEmpty()){
            // This means that it is a stright directroy path and not an URL.
            fileloc = dirToCheck;
        }

        configuration->addKeyValuePair(CONFIG_OUTPUT_DIR,fileloc);
    }

    // Checking if the directory for the outputs exists and is valid. If there are any problems the user is asked for a directory instead.
    if (!configuration->containsKeyword(CONFIG_OUTPUT_DIR)) return "";
    QDir dir(configuration->getString(CONFIG_OUTPUT_DIR));
    if (!dir.exists()) return "";

    QDir reports(dir.path() + "/" + QString(DIRNAME_REPORTS));
    QDir rawdata(dir.path() + "/" + QString(DIRNAME_RAWDATA));
    if (!reports.exists()){
        if (!dir.mkdir(DIRNAME_REPORTS)){
            logger.appendError("Cannot create reports directory in selected output directory");
            return "";
        }
    }
    if (!rawdata.exists()){
        if (!dir.mkdir(DIRNAME_RAWDATA)){
            logger.appendError("Cannot create etdata directory in selected output directory");
            return "";
        }
    }

    // Saving it to disk
    ConfigurationManager::setValue(FILE_CONFIGURATION,COMMON_TEXT_CODEC,CONFIG_OUTPUT_DIR,configuration->getString(CONFIG_OUTPUT_DIR));

    // Returning the new dir.
    return configuration->getString(CONFIG_OUTPUT_DIR);

}


QString Loader::getStringForKey(const QString &key){
    if (language.containsKeyword(key)){
        return language.getString(key);
    }
    else return "ERROR: NOT FOUND";
}

QString Loader::getConfigurationString(const QString &key){
    if (configuration->containsKeyword(key)){
        return configuration->getString(key);
    }
    else return "";
}

bool Loader::getConfigurationBoolean(const QString &key){
    if (configuration->containsKeyword(key)){
        return configuration->getBool(key);
    }
    else return "";
}

void Loader::changeLanguage(){
    QString lang = configuration->getString(CONFIG_REPORT_LANGUAGE);
    if (lang == CONFIG_P_LANG_ES){
        if (!language.loadConfiguration(":/languages/es.lang",COMMON_TEXT_CODEC)){
            // In a stable program this should NEVER happen.
            logger.appendError("CANNOT LOAD ES LANG FILE: " + language.getError());
            loadingError = true;
        }
    }
    else{
        // Defaults to english
        if (!language.loadConfiguration(":/languages/en.lang",COMMON_TEXT_CODEC)){
            // In a stable program this should NEVER happen.
            logger.appendError("CANNOT LOAD EN LANG FILE: " + language.getError());
            loadingError = true;
        }
    }
}

void Loader::setConfigurationString(const QString &key, const QString &value){
    ConfigurationManager::setValue(FILE_CONFIGURATION,COMMON_TEXT_CODEC,key,value,configuration);
}

void Loader::setConfigurationBoolean(const QString &key, bool value){
    QString boolText = value ? "true" : "false";
    ConfigurationManager::setValue(FILE_CONFIGURATION,COMMON_TEXT_CODEC,key,boolText,configuration);
}


bool Loader::createPatientDirectory(const QString &patient, const QString &age, const QString &email){

    // Creating the doctor directory.
    QString baseDir = configuration->getString(CONFIG_OUTPUT_DIR) + "/" + QString(DIRNAME_RAWDATA);
    QString repDir = configuration->getString(CONFIG_OUTPUT_DIR) + "/" + QString(DIRNAME_REPORTS);
    QString drname = configuration->getString(CONFIG_DOCTOR_NAME);
    QString pname = patient;

    if (!createDirectorySubstructure(drname,patient,baseDir,CONFIG_PATIENT_DIRECTORY)) return false;
    if (!createDirectorySubstructure(drname,patient,repDir,CONFIG_PATIENT_REPORT_DIR)) return false;


    // The patient info file
    QString pInfoFile = configuration->getString(CONFIG_PATIENT_DIRECTORY) + "/" + QString(FILE_PATIENT_INFO_FILE);

    if (email.isEmpty()){
        ConfigurationManager::setValue(pInfoFile,COMMON_TEXT_CODEC,CONFIG_PATIENT_EMAIL,"noone@somewhere.nan");
    }

    ConfigurationManager::setValue(pInfoFile,COMMON_TEXT_CODEC,CONFIG_PATIENT_AGE,age);
    ConfigurationManager::setValue(pInfoFile,COMMON_TEXT_CODEC,CONFIG_PATIENT_NAME,patient);
    return true;

}

bool Loader::createDirectorySubstructure(QString drname, QString pname, QString baseDir, QString saveAs){

    drname = drname.replace(" ","_");
    drname = drname.replace(".","_");

    pname = pname.replace(" ","_");
    pname = pname.replace(".","_");

    QString drdir = baseDir + "/" + drname;
    QString pdir = drdir + "/" + pname;

    // Creating the the doctor's dir
    if (!QDir(drdir).exists()){
        QDir base(baseDir);
        if (!base.mkdir(drname)){
            logger.appendError("Could not create doctor dir: " + drdir + " in " + baseDir);
            return false;
        }
    }

    QDir dirdr(drdir);
    if (!QDir(pdir).exists()){
        if (!dirdr.mkdir(pname)){
            logger.appendError("Could not create patient dir: " + pname + " in " + drdir);
            return false;
        }
    }

    // Al is good and pdir is the final working directory so it is set in the configuration->
    configuration->addKeyValuePair(saveAs,pdir);

    return true;
}

void Loader::loadDefaultConfigurations(){
    if (!configuration->containsKeyword(CONFIG_DEMO_MODE)){
        configuration->addKeyValuePair(CONFIG_DEMO_MODE,false);
    }
    if (!configuration->containsKeyword(CONFIG_DUAL_MONITOR_MODE)){
        configuration->addKeyValuePair(CONFIG_DUAL_MONITOR_MODE,false);
    }
    // This should allways be true in the std version of the software
    if (!configuration->containsKeyword(CONFIG_BINDING_DEFAULT)){
        configuration->addKeyValuePair(CONFIG_BINDING_DEFAULT,true);
    }
    if (!configuration->containsKeyword(CONFIG_SELECTED_ET)){
        configuration->addKeyValuePair(CONFIG_SELECTED_ET,CONFIG_P_ET_MOUSE);
    }
    if (!configuration->containsKeyword(CONFIG_REPORT_LANGUAGE)){
        configuration->addKeyValuePair(CONFIG_REPORT_LANGUAGE,CONFIG_P_LANG_EN);
    }
    if (!configuration->containsKeyword(CONFIG_DOCTOR_NAME)){
        configuration->addKeyValuePair(CONFIG_DOCTOR_NAME,"DOCTOR");
    }
}
