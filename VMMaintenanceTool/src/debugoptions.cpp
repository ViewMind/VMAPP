#include "debugoptions.h"

ConfigurationManager DebugOptions::debug;

DebugOptions::DebugOptions() {

}

bool DebugOptions::LoadDebugOptions(){
    if (QFile(Globals::Paths::VMTOOLDBUG).exists()){
        return debug.loadConfiguration(Globals::Paths::VMTOOLDBUG);
    }
    return false;
}

QString DebugOptions::DebugString(const QString &key){
    if (debug.containsKeyword(key)) return debug.getString(key);
    return "";
}

bool DebugOptions::DebugBool(const QString &key){
    if (debug.containsKeyword(key)) return debug.getBool(key);
    return false;
}
