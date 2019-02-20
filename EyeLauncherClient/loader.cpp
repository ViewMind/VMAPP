#include "loader.h"

Loader::Loader(QObject *parent) : QObject(parent)
{

    // Attempting to load the settings file to figure out the language.
    ConfigurationManager cmng;
    QString langfile = ":/languages/en.lang";
    if (cmng.loadConfiguration(FILE_EYEEXP_SETTINGS,COMMON_TEXT_CODEC)){
        if (cmng.getString(CONFIG_REPORT_LANGUAGE) == CONFIG_P_LANG_ES){
            langfile = ":/languages/es.lang";
        }
    }

    if (!language.loadConfiguration(langfile,COMMON_TEXT_CODEC)){
       logger.appendError("Could not load language file");
       return;
    }

}


QString Loader::getStringForKey(const QString &key){
    if (language.containsKeyword(key)){
        return language.getString(key);
    }
    else return "ERROR: NOT FOUND";
}
