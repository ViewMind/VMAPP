#include "maintenancemanager.h"

MaintenanceManager::MaintenanceManager() {
    this->currentAction = ACTION_DIAGNOSE;
}

void MaintenanceManager::setAction(Action action){
    this->currentAction = action;
}

void MaintenanceManager::setDryRun(bool dr){
    this->dryRun = dr;
}

MaintenanceManager::Action MaintenanceManager::getLastPerformedAction() const {
    return this->currentAction;
}

void MaintenanceManager::setRecommendedAction(Action action){
    this->recommendedAction = action;
}

MaintenanceManager::Action MaintenanceManager::getRecommendedAction() const {
    return this->recommendedAction;
}

bool MaintenanceManager::wasLastActionASuccess() const {
    return actionSuccessfull;
}


void MaintenanceManager::onDirCompareProgress(double p, QString filename){

    QString msg = Langs::getString("action_checking_file");
    msg = msg.replace("<F>",filename);
    p = qRound(p);
    emit MaintenanceManager::progressUpdate(p,msg);

}

void MaintenanceManager::run() {

    actionSuccessfull = false;

    switch (currentAction){
    case ACTION_UPDATE:
        doActionUpdate();
        break;
    case ACTION_DIAGNOSE:
        doActionRunDiagnostics();
        break;
    case ACTION_CORRECT:
        qDebug() << "Performing corrective actions. Number of actions: " << correctiveActions.size() << " Dry Run: " << dryRun;
        if (performCorrectiveActions(dryRun)){
            actionSuccessfull = true;
        }
        else {
            actionSuccessfull = false;
        }
        break;
    }
}

/////////////////////////////////// Update Sequence

void MaintenanceManager::doActionUpdate(){

    correctiveActions.clear();

    emit MaintenanceManager::progressUpdate(0,Langs::getString("action_checking_installation"));

    // We need to check the existance fo the path and of the executable.
    if (!Paths::Exists(Paths::PI_CURRENT_EXE_FILE)){
        error(Langs::getString("error_no_current_install"));
        log("Unable to find current installation on path: " + Paths::Path(Paths::PI_CURRENT_EXE_FILE));
        return;
    }

    // Fill all the actions.
    fillCorrectiveActionsForUpdateProcess();


    // Runn all the corrective actions.
    if (performCorrectiveActions()){
        actionSuccessfull = true;
        succes(Langs::getString("success_update"));
    }

}

/////////////////////////////////// Diagnostics

void MaintenanceManager::doActionRunDiagnostics() {

    anyDiagnosisStepsFailed = false;

    log("Running diagnostics. Installation Directory Location: '" + Paths::Path(Paths::PI_CONTAINER_DIR) + "'");

    // Now we check if the original installation renamed directory exists.
    eebkpExists = Paths::Exists(Paths::PI_BKP_DIR); //QDir(tempRenamedOldInstallDirPath).exists();

    // Now we check if the copied, newly unpacked ee directory exists.
    eyeExpUnderScoreExists = Paths::Exists(Paths::PI_UNDERSCORE_DIR) ; //QDir(tempUncompressedNewInstallDirPath).exists();

    // Now we check if a current installation directory exists.
    currentInstallExists = Paths::Exists(Paths::PI_INSTALL_DIR); // QDir(normalInstallDirPath).exists();

    emit MaintenanceManager::progressUpdate(0,Langs::getString("action_checking_installation"));

    eebkpContainsETData = false;
    eyeExpUnderScoreContainsETData = false;
    if (eebkpExists){
        eebkpContainsETData = Paths::Exists(Paths::PI_BKP_VMDATA_DIR); //QDir(tempRenamedOldInstallDirPath + "/" + Globals::Paths::VMETDATA).exists();
    }
    if (eyeExpUnderScoreExists){
        eyeExpUnderScoreContainsETData = Paths::Exists(Paths::PI_UNDERS_VMDATA_DIR);  // QDir(tempUncompressedNewInstallDirPath + "/" + Globals::Paths::VMETDATA).exists();
    }

    log("Dirctory exist flags.\n   EEBKP: " + QString::number(eebkpExists)
        + ".\n   EyeExperimenter_: " + QString::number(eyeExpUnderScoreExists)
        + ".\n   EyeExperimenter_/viewmind_etdata: " + QString::number(eyeExpUnderScoreContainsETData)
        + ".\n   EEBKP/viewmind_etdata: " + QString::number(eebkpContainsETData)
        + ".\n   EyeExperimenter: " + QString::number(currentInstallExists)
        );

    if (!currentInstallExists){
        // If the current installation doesn't exist we can't run any further diagnostics. And the decisions need to be made the the available information.
        missingFilesInInstall.clear();
        corruptedFilesInInstall.clear();
        currentDBCorrupted = false;
        currentLicenseFileCorrupted = false;
        emit MaintenanceManager::progressUpdate(100,Langs::getString("action_finishing"));
        getRecommendedActionFromDiagnosisResults();
        return;
    }

    emit MaintenanceManager::progressUpdate(0,Langs::getString("action_uncompress_dir"));

    // If we got here, the current installation exists and we verify it. For that we need to uncompress to the current working dir.
    QString output = "";
    QString errorString = "";
    if (!uncompressAppPackage(".",&output,&errorString)){
        error(Langs::getString("error_uncompress"));
        log("Untarring packet failed with " + errorString + ". Tar output:\n" + output);
        anyDiagnosisStepsFailed = true;
        getRecommendedActionFromDiagnosisResults();
        return;
    }


    // We check that the uncompressed directory exists.
    QDir refInstallDir(Globals::Paths::DIR_TEMP_EYEEXP_NEW);
    if (!refInstallDir.exists()){
        error(Langs::getString("error_uncompress"));
        log("After untarring, was unable to locate the reference install dir at location: " + refInstallDir.absolutePath());
        anyDiagnosisStepsFailed = true;
        getRecommendedActionFromDiagnosisResults();
        return;
    }

    // Before we start verifying the installs we must verify the DB AND license file.
    currentDBCorrupted = !isLocalDBOK();

    if (QFile(Globals::Paths::LICENSE_FILE).exists()){
        QString lic_file_checksum = computeMD5ForFile(Paths::Path(Paths::PI_CURRENT_VMCONFIG_FILE));
        QString bkp_lic_file_checksum = computeMD5ForFile(Globals::Paths::LICENSE_FILE);
        currentLicenseFileCorrupted = (lic_file_checksum != bkp_lic_file_checksum);
    }
    else {
        // The backup does not exist.
        warning(Langs::getString("warning_no_lic_backup"));
        currentLicenseFileCorrupted = false;
    }

    log("DB Corrupted Flag: " + QString::number(currentDBCorrupted));
    log("License File Corrupted Flag: " + QString::number(currentLicenseFileCorrupted));

    emit MaintenanceManager::progressUpdate(0,Langs::getString("action_verify_install"));

    // Now we check the directories integrity.
    DirCompare dirCompare;
    connect(&dirCompare,&DirCompare::updateProgress,this,&MaintenanceManager::onDirCompareProgress);
    dirCompare.setDirs(refInstallDir.absolutePath(),Paths::Path(Paths::PI_INSTALL_DIR));
    dirCompare.runInThread();

    missingFilesInInstall = dirCompare.getFileList(DirCompare::FLT_NOT_IN_CHECK);
    corruptedFilesInInstall = dirCompare.getFileList(DirCompare::FLT_BAD_CHECSUM);

    log("List of files missing in install\n   " + missingFilesInInstall.join("\n   "));       
    log("List of files corrupted in install\n   " + corruptedFilesInInstall.join("\n   "));

    // After all the diagnostics are done, we figure out the recommended action.
    getRecommendedActionFromDiagnosisResults();

    emit MaintenanceManager::progressUpdate(100,Langs::getString("action_finishing"));

}

/////////////////////////////////// Recommended Action logic
void MaintenanceManager::getRecommendedActionFromDiagnosisResults() {

    qDebug() << "Getting the recommended action from dia results";

    if (anyDiagnosisStepsFailed){
        error(Langs::getString("error_diagnostic_failed"));
        this->recommendedAction = ACTION_CONTACT_SUPPORT;
        return;
    }

    correctiveActions.clear();
    this->recommendedAction = ACTION_NONE;

    // Directory existance coherence check.
    if (currentInstallExists){

        if (eebkpExists){
            // Cant't be.
            log("Both the current installation and the BKP exist at location '" + Paths::Path(Paths::PI_INSTALL_DIR) + "'. Unknonw situation. Recommending contact");
            error(Langs::getString("error_incosistent_situation"));
            this->recommendedAction = ACTION_CONTACT_SUPPORT;
            return;
        }
        else if (eyeExpUnderScoreExists){
            // Can't be either.
            log("Both the current installation and the New underscore directory exist at location '" + Paths::Path(Paths::PI_INSTALL_DIR) + "'. Unknonw situation. Recommending contact");
            error(Langs::getString("error_incosistent_situation"));
            this->recommendedAction = ACTION_CONTACT_SUPPORT;
            return;
        }

        // We chekc if  we need to restore the DB.
        if (currentDBCorrupted) {
            // We need to restore the current DB prior to update.
            log("The Database is corrupted adding action to restore it to a previous version");
            addCorrectiveAction(CA_RESTORE_DB,"","");
        }


        // We check if we need to restore the license file
        if (currentLicenseFileCorrupted){
            // We need to restore the current licencse file prior to update, assuming we have the current backup.

            log("The License File is corrupted adding action to restore it from backup");

            // This is assuming the licence file exists.
            if (Paths::Exists(Paths::PI_CURRENT_VMCONFIG_FILE)){                
                addCorrectiveAction(CA_DELETE_FILE,Paths::Path(Paths::PI_CURRENT_VMCONFIG_FILE),"");
            }

            // If we got here we KNOW the backup exists.
            addCorrectiveAction(CA_COPY_FILE,Globals::Paths::LICENSE_FILE,Paths::Path(Paths::PI_CURRENT_VMCONFIG_FILE));
        }


        //  We check the install coherence.
        if ((missingFilesInInstall.size() > 0) || (corruptedFilesInInstall.size() > 0)){

            // We need to recommend a re updated. But we need to make sure that everything else is safe.
            if (!Paths::Exists(Paths::PI_CURRENT_VMDATA)){
                // Something is wrong. Can't do anything.
                log("The current install directory does not contain the VM Data directory. Unknonw situation. Recommending contact");
                error(Langs::getString("error_incosistent_situation"));
                this->recommendedAction = ACTION_CONTACT_SUPPORT;
                return;
            }

            log("Installation seems to be corrupted. Adding actions for the update process");

            // Basicall we re do the update process to establish all to where it was.
            fillCorrectiveActionsForUpdateProcess();

        }

    }
    else if (eebkpExists) {

        // Two paths depending if underscore exists or not.
        if (eyeExpUnderScoreExists) {
            // Now it depends on where is the data.

            if (eyeExpUnderScoreContainsETData){
                // The new dir contains the et data.
                if (eebkpContainsETData){
                    // So does the backup. Unknown situation.
                    log("ERROR: Will not be procededing. Both Underscore and EEBKP contain VMETData");
                    error(Langs::getString("error_diagnostic_failed"));
                    this->recommendedAction = ACTION_CONTACT_SUPPORT;
                    return;
                }

                // The else here is to do nothing. the new directory already contains the et data so nothing needs tobe copied.
            }
            else {
                if (eebkpContainsETData){
                    // We add the corrective action.
                    log("Adding action to move VMDATA from the BKP To the Underscore directory");
                    addCorrectiveAction(CA_MOVE_DIR,Paths::Path(Paths::PI_BKP_VMDATA_DIR),Paths::Path(Paths::PI_UNDERS_VMDATA_DIR));
                }
                else {
                    // Neither directory has the ViewMind ET Data. Somehting is wrong.
                    log("ERROR: Will not be procededing. Both Neither and EEBKP contain VMETData");
                    error(Langs::getString("error_diagnostic_failed"));
                    this->recommendedAction = ACTION_CONTACT_SUPPORT;
                    return;
                }
            }

            // Similar logic for the DBBKP. But if not present it's not critical.
            if (Paths::Exists(Paths::PI_BKP_DBBKP_DIR) && !Paths::Exists(Paths::PI_UNDERS_DBBKP_DIR) ){
                // We add the corrective action.
                log("Adding action to move DBBKP from the BKP To the Underscore directory");
                addCorrectiveAction(CA_MOVE_DIR,Paths::Path(Paths::PI_BKP_VMDATA_DIR),Paths::Path(Paths::PI_UNDERS_VMDATA_DIR));
            }

            log("Adding actions to copy license file from backup to underscore, rename underscore to regular install dir, remove backup and create EyeExp shortcut");

            // And then we add the finalize update steps.
            addCorrectiveAction(CA_COPY_FILE,Paths::Path(Paths::PI_BKP_VMCONFIG_FILE),Paths::Path(Paths::PI_UNDERS_VMCONFIG_FILE));
            // We create a local copy of the license file.
            addCorrectiveAction(CA_COPY_FILE,Paths::Path(Paths::PI_BKP_VMCONFIG_FILE),Globals::Paths::VMCONFIG);
            // The directory is now renamed eye experimenter.
            addCorrectiveAction(CA_MOVE_DIR,Paths::Path(Paths::PI_UNDERSCORE_DIR),Paths::Path(Paths::PI_INSTALL_DIR));
            // Now that we've finished the eyeexp temp dir can be deleted.
            addCorrectiveAction(CA_REMOVE_DIR,Paths::Path(Paths::PI_BKP_DIR),"");
            // Finally we create a desktop shortcut.
            addCorrectiveAction(CA_CREATE_SHORTCUT,Paths::Path(Paths::PI_CURRENT_EXE_FILE),Globals::Paths::EYEEXP_SHORTCUT);

        }
        else {

            // The underscore does not exist. But the bkp does. We make sure it's complete.
            if (!eebkpContainsETData){
                // This is a problem.
                log("ERROR: Will not be procededing. The  EEBKP exists and there is no underscore direcotry. However EEBKP does not contain the VM Data directory. Recommending contact support");
                error(Langs::getString("error_diagnostic_failed"));
                this->recommendedAction = ACTION_CONTACT_SUPPORT;
                return;
            }

            // We have the etdata. We need to make sure that we have license file.
            if (!Paths::Exists(Paths::PI_BKP_VMCONFIG_FILE)){
                // This is another problem.
                log("ERROR: Will not be procededing. The  EEBKP exists but there is no licencse file. Recommending contact support");
                error(Langs::getString("error_diagnostic_failed"));
                this->recommendedAction = ACTION_CONTACT_SUPPORT;
                return;
            }

            // Best to just rename the directory and the restart the update.
            log("Adding action to restore regular install dir from BKP and then do the update process");
            addCorrectiveAction(CA_MOVE_DIR, Paths::Path(Paths::PI_BKP_DIR), Paths::Path(Paths::PI_INSTALL_DIR));
            fillCorrectiveActionsForUpdateProcess();

        }

    }
    else {

        // Makes no sense either the backup or the current install must exit.
        // When installing the current installation is renamed to the temporary location they can't both exists at the same time. Requires a human to take a look.
        log("Neither the EEBKP and EyeExperimenter directory exists at location '" + Paths::Path(Paths::PI_INSTALL_DIR) + "'. Unknonw situation. Recommending contact");
        error(Langs::getString("error_incosistent_situation"));
        this->recommendedAction = ACTION_CONTACT_SUPPORT;
        return;


    }

    // If any corrective actions are indicated, then correct is the recommend action
    if (correctiveActions.size() > 0){
        this->recommendedAction = ACTION_CORRECT;
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

bool MaintenanceManager::isLocalDBOK(const QString &override_path)  {

    QString path = Paths::Path(Paths::PI_CURRENT_DB_FILE);
    if (override_path != "") path = override_path;

    QFile file(path);
    if (!file.open(QFile::ReadOnly)) return false;

    // Loading the data.
    QJsonParseError json_error;
    QString val = file.readAll();
    file.close();

    QJsonDocument doc = QJsonDocument::fromJson(val.toUtf8(),&json_error);
    if (doc.isNull()) return false;
    QVariantMap data = doc.object().toVariantMap();
    QString checksum = data.value("hash_checksum").toString();


    data["hash_checksum"] = "";
    QJsonDocument json = QJsonDocument::fromVariant(data);
    QString hash = QString(QCryptographicHash::hash(json.toJson(QJsonDocument::Compact),QCryptographicHash::Sha3_512).toHex());

    bool ans = (hash == checksum);
    QString anstext = "Same";
    if (!ans) anstext = "Different";

    log("DB Checksum Comparison: '" + path + "'\n   DB Stored: " + checksum + "\n   Computed: " + hash + "\n   Result: " + anstext);

    return ans;

}

bool MaintenanceManager::restoreDBToPreviousValidPoint() {

    QString main_bkp_path = Paths::Path(Paths::PI_CURRENT_DBBKP);

    QStringList dbbkps = QDir(main_bkp_path).entryList(QStringList(),QDir::NoDotAndDotDot|QDir::Files,QDir::Time);
    if (dbbkps.empty()){
        log("ERROR: Unable to get any files from directory: " + Paths::Path(Paths::PI_CURRENT_DBBKP));
        return false;
    }

    QString pathBkpToRestore = "";

    for (qint32 i = 0; i < dbbkps.size(); i++){

        QString bkp_path = main_bkp_path + "/" + dbbkps.at(i);
        if (isLocalDBOK(bkp_path)){
            pathBkpToRestore = bkp_path;
            break;
        }

    }

    if (pathBkpToRestore == ""){
        log("ERROR: None of the files in: " + main_bkp_path + " can be verified as non-corrupted");
        return false;
    }

    // Now that we got the final path, we need to copy this file to the current path.
    QString destination = Paths::Path(Paths::PI_CURRENT_VMDATA) + "/" + Globals::Paths::DB_FILE;
    QFile destFile(destination);
    destFile.remove();
    if (destFile.exists()){
        log("ERROR: Failed in removing the local database at: " + destination);
        return false;
    }

    QFile::copy(pathBkpToRestore,destination);

    if (!destFile.exists()){
        log("ERROR: Failed in copying backup file '" + pathBkpToRestore + "' to DB Local file: " + destination);
        return false;
    }

    return true;

}

QString MaintenanceManager::computeMD5ForFile(const QString &filename) {
    QFile file(filename);
    if (!file.open(QFile::ReadOnly)) {
        log("ERROR: Failed to open file to compute MD5: '" + filename + "'");
        return "";
    }

    QCryptographicHash hash(QCryptographicHash::Md5);
    hash.addData(&file);
    QString ans(hash.result().toHex());
    file.close();
    return ans;
}

void MaintenanceManager::addCorrectiveAction(CorrectiveActionType type,
                                             const QString &source,
                                             const QString &destination,
                                             const QString &error_key,
                                             const QString &warning_key,
                                             const QString &progress_label){
    CorrectiveAction ca;
    ca.type = type;
    ca.source = source;
    ca.destination = destination;
    ca.error_key = error_key;
    ca.warning_key = warning_key;
    ca.progress_label = progress_label;
    correctiveActions << ca;
}

bool MaintenanceManager::createDesktopShortcut(const QString &pathToEXE, const QString &shortcutName){
    QStringList possiblePaths = QStandardPaths::standardLocations(QStandardPaths::DesktopLocation); // the possible paths for the desktop.
    if (possiblePaths.size() == 0) return false; // Unable to find desktop.
    QString pathToLink = possiblePaths.first() + "/" + shortcutName; // The name of the ShortCut
    return QFile::link(pathToEXE,pathToLink);
}

/////////////////////////////////// Run corrective actions.

bool MaintenanceManager::performCorrectiveActions(bool sayNotDo) {

    qreal total = correctiveActions.size();

    for (qint32 i = 0; i < correctiveActions.size(); i++){

        qint32 progress = qRound((i+1)*100.0/total);

        CorrectiveAction ca = correctiveActions.at(i);

        bool printWarning = false;
        bool printError = false;

        if (ca.type == CA_COPY_FILE){

            log("CorrectiveAction: Copying file " + ca.source + " to " + ca.destination + "'");
            if (sayNotDo) continue;

            if (!QFile::copy(ca.source,ca.destination)){
                // We need to check that this didn't fail simply because the file already exists.
                if (!QFile(ca.destination).exists()){
                    log("ERROR: Failed to copy file from '" + ca.source + " to " + ca.destination);
                    printError = true;
                }
            }
        }
        else if (ca.type == CA_CREATE_SHORTCUT){

            log("CorrectiveAction: Creating shortcut of file '" + ca.source + "' under name '" + ca.destination + "'");
            if (sayNotDo) continue;

            if (!createDesktopShortcut(ca.source,ca.destination)){
                log("ERROR: Unable to create shortcut from '" + ca.source + "' to '" + ca.destination);
                printWarning = true;
            }
        }
        else if (ca.type == CA_MOVE_DIR){

            log("CorrectiveAction: Moving directory from '" + ca.source + "' to '" + ca.destination + "'");
            if (sayNotDo) continue;

            QDir dir;
            if (!dir.rename(ca.source,ca.destination)){
                log("ERROR: Unable move directories");
                printError = true;
            }
        }
        else if (ca.type == CA_REMOVE_DIR){

            log("CorrectiveAction: Deleting directory: '" + ca.source +  "'");
            if (sayNotDo) continue;

            QDir dir(ca.source);
            if (!dir.removeRecursively()){
                log("ERROR: Unable to delete directory '" + ca.source + "'");
                printError = true;
            }
        }
        else if (ca.type == CA_UNTAR_APP){

            log("CorrectiveAction: Uncompressing the app.zip to '" + ca.destination +  "'");
            if (sayNotDo) continue;

            QString output = "";
            QString errorString = "";
            if (!uncompressAppPackage(ca.destination,&output,&errorString)){
                log("ERROR: Failed in unpacking app.zip. Reason: " + errorString + ". Command output:\n " + output);
                printError = true;
            }

        }
        else if (ca.type == CA_DELETE_FILE){

            log("CorrectiveAction: Deleting file '" + ca.source +  "'");
            if (sayNotDo) continue;

            if (!QFile(ca.source).remove()){
                log("ERROR: Failed in deleting file " + ca.source);
                printError = true;
            }
        }
        else if (ca.type == CA_RESTORE_DB){
            log("CorrectiveAction: Restoring the local DB from the last known good backup");
            if (!restoreDBToPreviousValidPoint()){
                printError = true;
            }
        }
        else {
            log("ERROR: Unknown corrective action");
            return false;
        }

        if (printError){
            if (ca.error_key != ""){
                error(Langs::getString(ca.error_key));
            }
            return false;
        }

        if ((ca.warning_key != "") && (printWarning)){
            warning(Langs::getString(ca.warning_key));
        }


        emit MaintenanceManager::progressUpdate(progress,Langs::getString(ca.progress_label));


    }

    return true;

}

void MaintenanceManager::fillCorrectiveActionsForUpdateProcess(){

    // Once the check is done, we now attempt to rename the current directory.
    addCorrectiveAction(CA_MOVE_DIR,Paths::Path(Paths::PI_INSTALL_DIR),Paths::Path(Paths::PI_BKP_DIR),
                        "error_fail_rename","",
                        "action_renaming_current_dir");

    // The next step is to un compress the app.zip.

    addCorrectiveAction(CA_UNTAR_APP,"",Paths::Path(Paths::PI_CONTAINER_DIR),
                        "error_uncompress","",
                        "action_uncompress_dir");

    // Now we move the patient directory.

    addCorrectiveAction(CA_MOVE_DIR,Paths::Path(Paths::PI_BKP_VMDATA_DIR),Paths::Path(Paths::PI_UNDERS_VMDATA_DIR),
                        "error_move_patdata","",
                        "action_copying_patient_data");

    // Now we copy the database backup directory

    addCorrectiveAction(CA_MOVE_DIR,Paths::Path(Paths::PI_BKP_DBBKP_DIR),Paths::Path(Paths::PI_UNDERS_DBBKP_DIR),
                        "error_move_db_bkp","",
                        "action_copying_db_bkps");

    // And finally the licencse file.

    addCorrectiveAction(CA_COPY_FILE,Paths::Path(Paths::PI_BKP_VMCONFIG_FILE),Paths::Path(Paths::PI_UNDERS_VMCONFIG_FILE),
                        "error_fail_copy_lic","",
                        "action_copying_license");

    // We create a local copy of the license file.

    addCorrectiveAction(CA_COPY_FILE,Paths::Path(Paths::PI_BKP_VMCONFIG_FILE),Globals::Paths::VMCONFIG,
                        "error_fail_bkp_copy","",
                        "action_copying_license");

    // The directory is now renamed eye experimenter.

    addCorrectiveAction(CA_MOVE_DIR,Paths::Path(Paths::PI_UNDERSCORE_DIR),Paths::Path(Paths::PI_INSTALL_DIR),
                        "error_fail_rename_new","",
                        "action_renaming_new_dir");

    // Now that we've finished the eyeexp temp dir can be deleted.

    addCorrectiveAction(CA_REMOVE_DIR,Paths::Path(Paths::PI_BKP_DIR),"",
                        "error_cleanup","",
                        "action_cleaning_up");


    // Finally we create a desktop shortcut.

    addCorrectiveAction(CA_CREATE_SHORTCUT,Paths::Path(Paths::PI_CURRENT_EXE_FILE),Globals::Paths::EYEEXP_SHORTCUT,
                        "","warning_shortcut",
                        "action_cleaning_up");
}

QString MaintenanceManager::findCommonPath(const QString &dir1, const QString &dir2){
    QDir d1(dir1);
    QDir d2(dir2);
    if (d1.absolutePath() == d2.absolutePath()) return d1.absolutePath();
    QStringList pathPartsD1 = d1.absolutePath().split("/");
    QStringList pathPartsD2 = d2.absolutePath().split("/");
    QStringList path;
    qint32 M = qMin(pathPartsD1.size(),pathPartsD2.size());
    for (qint32 i = 0; i < M; i++){
        if (pathPartsD1.at(i) == pathPartsD2.at(i)){
            path << pathPartsD1.at(i);
        }
        else break;
    }
    if (path.size() > 0) return path.join("/");
    else return "";
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
