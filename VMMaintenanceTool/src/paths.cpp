#include "paths.h"

QString Paths::pathToDirContainingEyeExpInstall = "";
QString Paths::pathToEyeExpInstallDir = "";
QString Paths::pathToUnCompressedNewEyeExpDir = "";
QString Paths::pathToBKPOldEyeInstallDir = "";

QString Paths::pathToBKPOldEyeInstallVMData = "";
QString Paths::pathToBKPOldEyeInstallVMConfig = "";
QString Paths::pathToBKPOldEyeInstallDBBKPDir = "";

QString Paths::pathToUnCompressedNewEyeExpDirVMData = "";
QString Paths::pathToUnCompressedNewEyeExpDirVMConfig = "";
QString Paths::pathToUnCompressedNewEyeExpDirDBBKPDir = "";

QString Paths::pathToCurrentDB = "";
QString Paths::pathToCurrentVMConfig = "";
QString Paths::pathToCurrentDBBkp = "";
QString Paths::pathToCurrentEXE = "";
QString Paths::pathToCurrentVMData = "";


Paths::Paths() {

}

void Paths::FillPaths(){

    // First thing is first we get the directory of the currently installed app.
    QString path = DebugOptions::DebugString(Globals::DebugOptions::FORCE_EYEEXP_PATH);
    if (path == ""){
        path = "../";
    }

    pathToDirContainingEyeExpInstall = path;

    pathToBKPOldEyeInstallDir      = pathToDirContainingEyeExpInstall + "/" + Globals::Paths::DIR_TEMP_EYEEXP;
    pathToBKPOldEyeInstallDBBKPDir = pathToBKPOldEyeInstallDir + "/" + Globals::Paths::DBBKP_DIR;
    pathToBKPOldEyeInstallVMConfig = pathToBKPOldEyeInstallDir + "/" + Globals::Paths::LICENSE_FILE;
    pathToBKPOldEyeInstallVMData   = pathToBKPOldEyeInstallDir + "/" + Globals::Paths::VMETDATA;

    pathToEyeExpInstallDir         = pathToDirContainingEyeExpInstall + "/" + Globals::Paths::EYEEXP_INSTALL_DIR;
    pathToCurrentDB                = pathToEyeExpInstallDir + "/" + Globals::Paths::VMETDATA + "/" + Globals::Paths::DB_FILE;
    pathToCurrentDBBkp             = pathToEyeExpInstallDir + "/" + Globals::Paths::DBBKP_DIR;
    pathToCurrentVMConfig          = pathToEyeExpInstallDir + "/" + Globals::Paths::LICENSE_FILE;
    pathToCurrentVMData            = pathToEyeExpInstallDir + "/" + Globals::Paths::VMETDATA;
    pathToCurrentEXE               = pathToEyeExpInstallDir + "/" + Globals::Paths::EYEEXP_EXECUTABLE;

    pathToUnCompressedNewEyeExpDir          = pathToDirContainingEyeExpInstall + "/" + Globals::Paths::DIR_TEMP_EYEEXP_NEW;
    pathToUnCompressedNewEyeExpDirDBBKPDir  = pathToUnCompressedNewEyeExpDir + "/" + Globals::Paths::DBBKP_DIR;
    pathToUnCompressedNewEyeExpDirVMConfig  = pathToUnCompressedNewEyeExpDir + "/" + Globals::Paths::LICENSE_FILE;
    pathToUnCompressedNewEyeExpDirVMData    = pathToUnCompressedNewEyeExpDir + "/" + Globals::Paths::VMETDATA;


}

QString Paths::Path(PathID p){
    switch (p) {
    case PI_BKP_DBBKP_DIR:
        return pathToBKPOldEyeInstallDBBKPDir;
    case PI_BKP_DIR:
        return pathToBKPOldEyeInstallDir;
    case PI_BKP_VMCONFIG_FILE:
        return pathToBKPOldEyeInstallVMConfig;
    case PI_BKP_VMDATA_DIR:
        return pathToBKPOldEyeInstallVMData;
    case PI_CONTAINER_DIR:
        return pathToDirContainingEyeExpInstall;
    case PI_CURRENT_DBBKP:
        return pathToCurrentDBBkp;
    case PI_CURRENT_DB_FILE:
        return pathToCurrentDB;
    case PI_CURRENT_VMCONFIG_FILE:
        return pathToCurrentVMConfig;
    case PI_CURRENT_VMDATA:
        return pathToCurrentVMData;
    case PI_INSTALL_DIR:
        return pathToEyeExpInstallDir;
    case PI_UNDERSCORE_DIR:
        return pathToUnCompressedNewEyeExpDir;
    case PI_UNDERS_DBBKP_DIR:
        return pathToUnCompressedNewEyeExpDirDBBKPDir;
    case PI_UNDERS_VMCONFIG_FILE:
        return pathToUnCompressedNewEyeExpDirVMConfig;
    case PI_UNDERS_VMDATA_DIR:
        return pathToUnCompressedNewEyeExpDirVMData;
    case PI_CURRENT_EXE_FILE:
        return pathToCurrentEXE;
    default:
        return "";
    }
}

bool Paths::Exists(PathID p){
    QString path = Path(p);
    if (IsFile(p)){
        return QFile(path).exists();
    }
    else {
        return QDir(path).exists();
    }
}

bool Paths::IsFile(PathID p){
    QSet<PathID> filepaths;
    filepaths << PI_BKP_VMCONFIG_FILE
              << PI_UNDERS_VMCONFIG_FILE
              << PI_CURRENT_VMCONFIG_FILE
              << PI_CURRENT_EXE_FILE
              << PI_CURRENT_DB_FILE;
    return filepaths.contains(p);
}
