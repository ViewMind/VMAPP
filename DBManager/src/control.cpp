#include "control.h"

Control::Control(QObject *parent):QThread(parent)
{
    mainMenu.setMenuTitle("What do you want to do?");
    mainMenu.addMenuOption("New institution");
    mainMenu.addMenuOption("Reset institution password");
    mainMenu.addMenuOption("Update institution information");
    mainMenu.addMenuOption("See institution information");
    mainMenu.addMenuOption("Exit");
}

void Control::run(){

    QStringList loadmsg = getGreeting();

    if (!db.initConnection()){
        loadmsg << "Error on DB Initialization " << db.getError();
        loadScreen.setInformationScreen(loadmsg,false);
        loadScreen.show();
        emit(exitRequested());
        return;
    }

    loadmsg << db.getConnectionMsg();
    loadScreen.setInformationScreen(loadmsg,false);
    loadScreen.show();

    // The n was pressed for sure. Printing the main menu.
    while (true){

        mainMenu.show();

        if (mainMenu.getAction() == ConsoleInputScreen::CA_BACK){
            // back and exit are equivalent in this menu.
            db.close();
            emit(exitRequested());
            return;
        }

        // A menu was selected.
        switch (mainMenu.getSelected()){
        case 0:
            newInstitutions();
            break;
        case 1:
            // Reset institution password
            resetPasswInstitution();
            break;
        case 2:
            // Update institution information.
            updateInstitution();
            break;
        case 3:
            // Just show institution information.
            printInstitutionInfo();
            break;
        case 4:
            // Exit
            db.close();
            emit(exitRequested());
            return;
        }

    }

}

void Control::newInstitutions(){

    inputInstitutionInfo(false);

    if (!commTransactionOk) return;

    if (!db.addNewInstitution(commInstitutionInfo)){
        std::cout << "ERROR Adding new institution: " << db.getError().toStdString() << ". Press any key to continue" << std::endl;
        getchar();
        return;
    }
    commInstitutionInfo.uid = QString::number(db.getGeeneratedUID());

    showInfoScreen();

}

void Control::resetPasswInstitution(){

    institutionSelection();
    if ((!commTransactionOk) || (commSelection == -1)) return;

    // Setting the neww password
    if (!db.resetPassword(commKeyid)){
        std::cout << "ERROR Resetting password: " << db.getError().toStdString() << ". Press any key to continue" << std::endl;
        getchar();
        return;
    }

    // Showing the new password
    showInfoScreen();

}

void Control::updateInstitution(){

    institutionSelection();
    if ((!commTransactionOk) || (commSelection == -1)) return;

    inputInstitutionInfo(true);
    if (!commTransactionOk) return;

    commInstitutionInfo.keyid = commKeyid;
    if (!db.updateNewInstitution(commInstitutionInfo)){
        std::cout << "ERROR: Updating instituion information " << db.getError().toStdString() << ". Press any key to continue" << std::endl;
        getchar();
        return;
    }

    // Showing updated information.
    showInfoScreen();

}

void Control::printInstitutionInfo(){

    institutionSelection();
    if ((!commTransactionOk) || (commSelection == -1)) return;

    // Showing the information
    showInfoScreen(false);

}

void Control::institutionSelection(){

    ConsoleInputScreen screen;
    bool ok = false;
    commTransactionOk = true;

    // Selecting the institution  
    QList<InstDBComm::Institution> info = db.getAllInstitutions(&ok);
    if (!ok){
        std::cout << "ERROR Getting institution list: " << db.getError().toStdString() << ". Press any key to continue" << std::endl;
        getchar();
        commTransactionOk = false;
        return;
    }

    for (qint32 i = 0; i < info.size(); i++){
        screen.addMenuOption(info.at(i).name,info.at(i).keyid);
    }
    screen.setMenuTitle("Select institution");
    screen.show();

    if (screen.getAction() == ConsoleInputScreen::CA_BACK) {
        commSelection = -1;
    }
    else {
        commInstitutionInfo = db.getInstitutionInfo(screen.getSelectedData().toString());
        commSelection = screen.getSelected();
        commKeyid = commInstitutionInfo.keyid;
        if (!commInstitutionInfo.ok){
            commTransactionOk = false;
            std::cout << "ERROR Getting institution information: " << db.getError().toStdString() << ". Press any key to continue" << std::endl;
            getchar();
        }
    }
}

void Control::inputInstitutionInfo(bool update){

    ConsoleInputScreen screen;
    commTransactionOk = true;

    if (update){
        screen.addDataEntryPrompt("Name [" + commInstitutionInfo.name  + "]");
        screen.addDataEntryPrompt("Number of evaluations [" + commInstitutionInfo.numEvals + "]");
        screen.addDataEntryPrompt("EyeTracker Brand [" + commInstitutionInfo.etbrand +  "]");
        screen.addDataEntryPrompt("EyeTracker Model [" + commInstitutionInfo.etmodel +  "]");
        screen.addDataEntryPrompt("EyeTracker Serial Number [" + commInstitutionInfo.etserial +  "]");
    }
    else {
        screen.addDataEntryPrompt("Name");
        screen.addDataEntryPrompt("Number of evaluations");
        screen.addDataEntryPrompt("EyeTracker Brand");
        screen.addDataEntryPrompt("EyeTracker Model");
        screen.addDataEntryPrompt("EyeTracker Serial Number");
    }

    if (update) screen.setMenuTitle("Input institution information (UID: " + commInstitutionInfo.uid  + "). Leave empty to leave as is:");
    else screen.setMenuTitle("Input institution information");

    while (true){
        screen.show();
        if (screen.getAction() == ConsoleInputScreen::CA_SUBMIT){
            // Checking the the number of evalution is a valid number. However if this is an update, then it can be empty.
            QStringList input = screen.getInputedData();

            bool ok = true;
            input.at(INPUT_NEVAL).toInt(&ok);
            if ((!ok) && !(update && input.at(INPUT_NEVAL).isEmpty())){
                std::cout << "ERROR: Number of evaluations should be a integer. Press any key to continue" << std::endl;
                getchar();
                // Should ask for infromation again.
            }
            else {
                commInstitutionInfo.etbrand = input.at(INPUT_BRAND);
                commInstitutionInfo.etmodel = input.at(INPUT_MODEL);
                commInstitutionInfo.etserial = input.at(INPUT_ETSN);
                commInstitutionInfo.keyid = "";
                commInstitutionInfo.name = input.at(INPUT_NAME);
                commInstitutionInfo.numEvals = input.at(INPUT_NEVAL);
                commInstitutionInfo.uid = "";
                break;
            }
        }
        else break;
    }
}

void Control::showInfoScreen(bool showPassword){

    // Now I need to show the generated information
    ConsoleInputScreen screen;
    screen.setMenuTitle("Instituion: " + commInstitutionInfo.name);
    QStringList info;
    info << "Number of evaluations remaining: " + commInstitutionInfo.numEvals;
    info << "ET : " + commInstitutionInfo.etbrand + " " + commInstitutionInfo.etmodel;
    if (showPassword) info << "Password: " + db.getGeneratedPassword();
    info << "Configuation file snippet:";
    info << QString(CONFIG_INST_ETSERIAL) + " = " + commInstitutionInfo.etserial + ";";
    info << QString(CONFIG_INST_NAME) + " = " + commInstitutionInfo.name + ";";
    info << QString(CONFIG_INST_UID) + " = " + commInstitutionInfo.uid + ";";
    screen.setInformationScreen(info,false);
    screen.show();

}


QStringList Control::getGreeting() const {
    QStringList list;
    list << "=============== " + QString(PROGRAM_NAME) + " ===============";
    list << "= VERSION: " + QString(PROGRAM_VERSION);
    list << "===========";
    return list;
}
