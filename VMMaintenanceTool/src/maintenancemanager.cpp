#include "maintenancemanager.h"

MaintenanceManager::MaintenanceManager() {
    this->currentAction = ACTION_NONE;
}

void MaintenanceManager::setAction(Action action){
    this->currentAction = action;
}

MaintenanceManager::Action MaintenanceManager::getAction() const {
    return this->currentAction;
}

bool MaintenanceManager::wasLastActionASuccess() const {
    return actionSuccessfull;
}

QString MaintenanceManager::getEyeExpWorkDir() const {
    return eyeExpWorkDir;
}

void MaintenanceManager::run() {

    actionSuccessfull = false;
    eyeExpWorkDir = "";

    switch (currentAction){
    case ACTION_UPDATE:
        updateTheApp();
        break;
    }
}


void MaintenanceManager::updateTheApp(){

    // First thing is first we get the directory of the currently installed app.
    QString path = DebugOptions::DebugString(Globals::DebugOptions::FORCE_EYEEXP_PATH);
    if (path != ""){
        log("DBUG: Using eyeexp path: " + path);
    }
    else {
        path = "../";
    }


    qreal nsteps = 8;
    qreal step_couter = 0;

    emit MaintenanceManager::progressUpdate(0,Langs::getString("action_checking_installation"));

    // We need to check the existance fo the path and of the executable.
    QFileInfo currentEXE(path  + "/" + Globals::Paths::EYEEXP_INSTALL_DIR  + "/" + Globals::Paths::EYEEXP_EXECUTABLE);
    if (!currentEXE.exists()){
        error(Langs::getString("error_no_current_install"));
        log("Unable to find current installation on path: " + currentEXE.absoluteFilePath());
        return;
    }

    step_couter++;
    emit MaintenanceManager::progressUpdate(qRound(step_couter*100/nsteps),Langs::getString("action_renaming_current_dir"));

    // Once the check is done, we now attempt to rename the current directory.
    QDir dirContainingInstallDir(path);

    // Now we try to rename it.
    if (!dirContainingInstallDir.rename(Globals::Paths::EYEEXP_INSTALL_DIR,Globals::Paths::DIR_TEMP_EYEEXP)){
        error(Langs::getString("error_fail_rename"));
        log("Unable to rename installation directory from located in path " + dirContainingInstallDir.absolutePath());
        return;
    }

    step_couter++;
    emit MaintenanceManager::progressUpdate(qRound(step_couter*100/nsteps),Langs::getString("action_uncompress_dir"));

    // The next step is to un compress the app.zip.
    QProcess process;
    QStringList arguments;
    arguments << "-xvzf" << Globals::Paths::APPZIP << "-C"  << dirContainingInstallDir.absolutePath();
    process.setProcessChannelMode(QProcess::MergedChannels);
    process.start(Globals::Paths::TAR_EXE,arguments);

    if (!process.waitForFinished()){
        QString output = QString::fromUtf8(process.readAllStandardOutput());
        error(Langs::getString("error_uncompress"));
        log("Untarring packet failed with " + process.errorString() + ". Tar output:\n" + output);
        return;
    }

    step_couter++;
    emit MaintenanceManager::progressUpdate(qRound(step_couter*100/nsteps),Langs::getString("action_renaming_new_dir"));

    // The directory is now renamed eye experimenter.
    if (!dirContainingInstallDir.rename(Globals::Paths::DIR_TEMP_EYEEXP_NEW,Globals::Paths::EYEEXP_INSTALL_DIR)){
        error(Langs::getString("error_fail_rename_new"));
        log("Unable to rename the new app directory directory located in path " + dirContainingInstallDir.absolutePath());
        return;
    }

    step_couter++;
    emit MaintenanceManager::progressUpdate(qRound(step_couter*100/nsteps),Langs::getString("action_copying_patient_data"));

    // Now we move the patient directory.
    QString source = Globals::Paths::DIR_TEMP_EYEEXP + "/" + Globals::Paths::VMETDATA;
    QString dest   = Globals::Paths::EYEEXP_INSTALL_DIR + "/" + Globals::Paths::VMETDATA;
    if (!dirContainingInstallDir.rename(source,dest)){
        error(Langs::getString("error_move_patdata"));
        log("Unable to move the patdata directory from '" + source + "' to '" + dest+ "' in dir " + dirContainingInstallDir.absolutePath());
        return;
    }

    step_couter++;
    emit MaintenanceManager::progressUpdate(qRound(step_couter*100/nsteps),Langs::getString("action_copying_db_bkps"));

    // Now we copy the database backup directory
    source = Globals::Paths::DIR_TEMP_EYEEXP + "/" + Globals::Paths::DBBKP_DIR;
    dest   = Globals::Paths::EYEEXP_INSTALL_DIR + "/" + Globals::Paths::DBBKP_DIR;
    if (!dirContainingInstallDir.rename(source,dest)){
        error(Langs::getString("error_move_db_bkp"));
        log("Unable to move the dbbkp directory from '" + source + "' to '" + dest+ "' in dir " + dirContainingInstallDir.absolutePath());
        return;
    }

    step_couter++;
    emit MaintenanceManager::progressUpdate(qRound(step_couter*100/nsteps),Langs::getString("action_copying_license"));

    // And finally the licencse file.
    source = Globals::Paths::DIR_TEMP_EYEEXP + "/" + Globals::Paths::VMCONFIG;
    dest   = Globals::Paths::EYEEXP_INSTALL_DIR + "/" + Globals::Paths::VMCONFIG;
    if (!dirContainingInstallDir.rename(source,dest)){
        error(Langs::getString("error_move_db_bkp"));
        log("Unable to move the license file directory from '" + source + "' to '" + dest+ "' in dir " + dirContainingInstallDir.absolutePath());
        return;
    }

    step_couter++;
    emit MaintenanceManager::progressUpdate(qRound(step_couter*100/nsteps),Langs::getString("action_cleaning_up"));

    // Now that we've finished the eyeexp temp dir can be deleted.
    QDir tocleanup(dirContainingInstallDir.absolutePath() + "/" + Globals::Paths::DIR_TEMP_EYEEXP);
    if (!tocleanup.removeRecursively()){
        warning(Langs::getString("warning_cleanup"));
        log("Unable to delete temporary directory " + tocleanup.absolutePath());
    }

    emit MaintenanceManager::progressUpdate(100,Langs::getString("action_desktop_shorcut"));

    eyeExpWorkDir = currentEXE.absolutePath();

    QStringList possiblePaths = QStandardPaths::standardLocations(QStandardPaths::DesktopLocation); // the possible paths for the desktop.
    QString pathToLink = possiblePaths.first() + "/Eye Explorer.lnk"; // The name of the ShortCut
    if (!QFile::link(currentEXE.absoluteFilePath(),pathToLink)){
        warning(Langs::getString("warning_shortcut"));
        log("Unable to create a desktop shorcut from " + currentEXE.absoluteFilePath() + " to " + pathToLink);
    }
    actionSuccessfull = true;

    succes(Langs::getString("success_update"));

}

/////////////////////////////////// Logging functions

void MaintenanceManager::error(const QString &s){
    emit MaintenanceManager::message(MessageLogger::MSG_ERROR,s);
}

void MaintenanceManager::succes(const QString &s){
    emit MaintenanceManager::message(MessageLogger::MSG_SUCCESS,s);
}

void MaintenanceManager::warning(const QString &s){
    emit MaintenanceManager::message(MessageLogger::MSG_WARNING,s);
}

void MaintenanceManager::display(const QString &s){
    emit MaintenanceManager::message(MessageLogger::MSG_DISP,s);
}

void MaintenanceManager::log(const QString &s){
    emit MaintenanceManager::message(MessageLogger::MSG_LOG,s);
}
