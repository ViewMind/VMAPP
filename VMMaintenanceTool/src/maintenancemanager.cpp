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

MaintenanceManager::Action MaintenanceManager::getRecommendedAction() const {
    return this->recommendedAction;
}

bool MaintenanceManager::wasLastActionASuccess() const {
    return actionSuccessfull;
}

QString MaintenanceManager::getEyeExpWorkDir() const {
    return eyeExpWorkDir;
}

void MaintenanceManager::onDirCompareProgress(double p, QString filename){

    QString msg = Langs::getString("action_checking_file");
    msg = msg.replace("<F>",filename);
    p = qRound(p);
    emit MaintenanceManager::progressUpdate(p,msg);

}

void MaintenanceManager::run() {

    actionSuccessfull = false;
    eyeExpWorkDir = "";

    switch (currentAction){
    case ACTION_UPDATE:
        doActionUpdate();
        break;
    case ACTION_DIAGNOSE:
        doActionRunDiagnostics();
        break;
    }
}

/////////////////////////////////// Update Sequence

void MaintenanceManager::doActionUpdate(){

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
    QString output = "";
    QString errorString;
    if (!uncompressAppPackage(dirContainingInstallDir.absolutePath(),&output,&errorString)){
        error(Langs::getString("error_uncompress"));
        log("Untarring packet failed with " + errorString + ". Tar output:\n" + output);
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

/////////////////////////////////// Diagnostics

void MaintenanceManager::doActionRunDiagnostics() {

    anyDiagnosisStepsFailed = false;

    // First thing is first we get the directory where the installation directory should be located.
    QString eeInstallDirPath = DebugOptions::DebugString(Globals::DebugOptions::FORCE_EYEEXP_PATH);
    if (eeInstallDirPath == ""){
        eeInstallDirPath = "../";
    }

    QDir eeInstallDir(eeInstallDirPath);
    eeInstallDirPath = eeInstallDir.absolutePath();
    log("Running diagnostics. Installation Directory Location: '" + eeInstallDirPath + "'");

    QString tempRenamedOldInstallDirPath = eeInstallDirPath + "/" + Globals::Paths::DIR_TEMP_EYEEXP;
    QString tempUncompressedNewInstallDirPath = eeInstallDirPath + "/" + Globals::Paths::DIR_TEMP_EYEEXP_NEW;
    QString normalInstallDirPath = eeInstallDirPath + "/" + Globals::Paths::EYEEXP_INSTALL_DIR;

    // Now we check if the original installation renamed directory exists.
    eebkpExists = QDir(tempRenamedOldInstallDirPath).exists();

    // Now we check if the copied, newly unpacked ee directory exists.
    eyeExpUnderScoreExists = QDir(tempUncompressedNewInstallDirPath).exists();

    // Now we check if a current installation directory exists.
    currentInstallExists = QDir(normalInstallDirPath).exists();

    emit MaintenanceManager::progressUpdate(0,Langs::getString("action_checking_installation"));

    eebkpContainsETData = false;
    eyeExpUnderScoreContainsETData = false;
    if (eebkpExists){
        eebkpContainsETData = QDir(tempRenamedOldInstallDirPath + "/" + Globals::Paths::VMETDATA).exists();
    }
    if (eyeExpUnderScoreExists){
        eyeExpUnderScoreContainsETData = QDir(tempUncompressedNewInstallDirPath + "/" + Globals::Paths::VMETDATA).exists();
    }


    if (!currentInstallExists){
        // If the current installation doesn't exist we can't run any further diagnostics. And the decisions need to be made the the available information.
        missingFilesInInstall.clear();
        corruptedFilesInInstall.clear();
        currentDBCorrupted = false;
        currentLicenseFileCorrupted = false;
        getRecommendedActionFromDiagnosisResults(eeInstallDirPath);
        return;
    }

    log("Dirctory exist flags.\n   EEBKP: " + QString::number(eebkpExists)
        + ".\n   EyeExperimenter_: " + QString::number(eyeExpUnderScoreExists)
        + ".\n   EyeExperimenter_/viewmind_etdata: " + QString::number(eyeExpUnderScoreContainsETData)
        + ".\n   EEBKP/viewmind_etdata: " + QString::number(eebkpContainsETData)
        + ".\n   EyeExperimenter: " + QString::number(currentInstallExists)
    );

    emit MaintenanceManager::progressUpdate(0,Langs::getString("action_uncompress_dir"));

    // If we got here, the current installation exists and we verify it. For that we need to uncompress to the current working dir.
    QString output = "";
    QString errorString = "";
    if (!uncompressAppPackage(".",&output,&errorString)){
        error(Langs::getString("error_uncompress"));
        log("Untarring packet failed with " + errorString + ". Tar output:\n" + output);
        anyDiagnosisStepsFailed = true;
        getRecommendedActionFromDiagnosisResults(eeInstallDirPath);
        return;
    }


    // We check that the uncompressed directory exists.
    QDir refInstallDir(Globals::Paths::DIR_TEMP_EYEEXP_NEW);
    if (!refInstallDir.exists()){
        error(Langs::getString("error_uncompress"));
        log("After untarring, was unable to locate the reference install dir at location: " + refInstallDir.absolutePath());
        anyDiagnosisStepsFailed = true;
        getRecommendedActionFromDiagnosisResults(eeInstallDirPath);
        return;
    }

    emit MaintenanceManager::progressUpdate(0,Langs::getString("action_verify_install"));

    // Now we check the directories integrity.
    DirCompare dirCompare;
    connect(&dirCompare,&DirCompare::updateProgress,this,&MaintenanceManager::onDirCompareProgress);
    dirCompare.setDirs(refInstallDir.absolutePath(),normalInstallDirPath);
    dirCompare.runInThread();

    missingFilesInInstall = dirCompare.getFileList(DirCompare::FLT_NOT_IN_CHECK);
    corruptedFilesInInstall = dirCompare.getFileList(DirCompare::FLT_BAD_CHECSUM);

    log("List of files missing in install\n   " + missingFilesInInstall.join("\n   "));
    log("List of files corrupted in install\n   " + corruptedFilesInInstall.join("\n   "));

    emit MaintenanceManager::progressUpdate(100,Langs::getString("action_finishing"));


}

/////////////////////////////////// Recommended Action logic
void MaintenanceManager::getRecommendedActionFromDiagnosisResults(const QString &eeInstallDirPath) {

    // First coherence check
    if ((eebkpExists) && (currentInstallExists)){

        // When installing the current installation is renamed to the temporary location they can't both exists at the same time. Requires a human to take a look.
        log("Both the EEBKP and EyeExperimenter directory exists at location '" + eeInstallDirPath + "'. Unknonw situation. Recommending contact");
        error(Langs::getString("error_incosistent_situation"));
        this->recommendedAction = ACTION_NONE;
        return;

    }

    if ((!eebkpExists) && (!currentInstallExists)){

        // When installing the current installation is renamed to the temporary location they can't both not exist.
        log("Neither the EEBKP and EyeExperimenter directory exists at location '" + eeInstallDirPath + "'. Unknonw situation. Recommending contact");
        error(Langs::getString("error_incosistent_situation"));
        this->recommendedAction = ACTION_NONE;
        return;

    }

}

/////////////////////////////////// Sub Actions.
bool MaintenanceManager::uncompressAppPackage(const QString &destination, QString *output, QString *errorString){
    QProcess process;
    QStringList arguments;
    arguments << "-xvzf" << Globals::Paths::APPZIP << "-C"  << destination; //dirContainingInstallDir.absolutePath();
    process.setProcessChannelMode(QProcess::MergedChannels);
    process.start(Globals::Paths::TAR_EXE,arguments);
    bool ans = process.waitForFinished();
    *errorString = process.errorString();
    *output = QString::fromUtf8(process.readAllStandardOutput());
    return ans;
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
