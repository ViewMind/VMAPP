#include "loader.h"

Loader::Loader(QObject *parent) : QObject(parent)
{

    // Loading the configuration file
    if (!configuration.loadConfiguration(FILE_CONFIGURATION,COMMON_TEXT_CODEC)){
        qWarning() << "ERRORS LOADING CONFIGURATION FILE";
        return;
    }

    QString lang = configuration.getString(CONFIG_REPORT_LANGUAGE);
    if (lang == CONFIG_P_LANG_ES){
        if (!language.loadConfiguration(":/languages/es.lang",COMMON_TEXT_CODEC)){
            qWarning() << "ERROR LOADING LANG FILE";
        }
    }
    else{
        // Defaults to english
        if (!language.loadConfiguration(":/languages/en.lang",COMMON_TEXT_CODEC)){
            qWarning() << "ERROR LOADING LANG FILE" << language.getError();
        }
    }
}


QString Loader::getStringForKey(const QString &key){
    if (language.containsKeyword(key)){
        return language.getString(key);
    }
    else return "ERROR: NOT FOUND";
}
