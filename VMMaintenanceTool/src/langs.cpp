#include "langs.h"

ConfigurationManager Langs::language;

Langs::Langs(){

}

bool Langs::LoadLanguageFile(const QString &langcode){
    Q_UNUSED(langcode)
    language.clear();
    bool ans = language.loadConfiguration(":/langs/en.lang");
    //qDebug() << language.getMap();
    return ans;
}

QString Langs::getString(const QString &key){
    if (language.containsKeyword(key)) return language.getString(key);
    else return "";
}

QStringList Langs::getStringList(const QString &key){
    if (language.containsKeyword(key)) return language.getStringList(key);
    else return QStringList();
}
