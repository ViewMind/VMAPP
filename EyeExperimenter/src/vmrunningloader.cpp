#include "vmrunningloader.h"

VMRunningLoader::VMRunningLoader(QObject *parent)  : QObject{parent} {

    ConfigurationManager language;
    language.loadConfiguration(":/languages/en.lang");
    titles["English"] = language.getString("viewstart_app_is_running_title");
    messages["English"] = language.getString("viewstart_app_is_running");

    language.loadConfiguration(":/languages/es.lang");
    titles["Español"] = language.getString("viewstart_app_is_running_title");
    messages["Español"] = language.getString("viewstart_app_is_running");

}

QVariantMap VMRunningLoader::getTitleMap() {
    return this->titles;
}

QVariantMap VMRunningLoader::getMessageMap() {
    return this->messages;
}
