#include "control.h"

Control::Control(QObject *parent):QThread(parent)
{
    mainMenu.setMenuTitle("What do you want to do?");
    mainMenu.addMenuOption("New institution");
    mainMenu.addMenuOption("Reset institution password");
    mainMenu.addMenuOption("Update institution information");
    mainMenu.addMenuOption("See institution information");
    mainMenu.addMenuOption("Delete test entries");
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
            // Delete test entries
            deleteTestEntries();
            break;
        case 5:
            // Exit
            db.close();
            emit(exitRequested());
            return;
        }

    }

}

bool Control::deleteTestEntries(){

    bool ok;
    QStringList userList = db.getPossibleTestUsers(&ok);
    if (!ok){
        std::cout << "ERROR Getting the user list: " << db.getError().toStdString() << ". Press any key to continue" << std::endl;
        getchar();
        return false;
    }

    ConsoleInputScreen screen;
    screen.setMenuTitle("Select possible test user to delete:");

    for (qint32 i = 0; i < userList.size(); i++){
        if ((i % 2) == 1) continue; // Odd entries are the actual UIDs
        screen.addMenuOption(userList.at(i),userList.at(i+1));
    }

    screen.show();

    if (screen.getAction() == ConsoleInputScreen::CA_BACK) return true;

    QString uid = screen.getSelectedData().toString();

    ConsoleInputScreen confirm;
    confirm.setQuestion("Are you sure you want to delete all entries with user UID: " + uid + "?");
    confirm.show();

    if (confirm.getAction() == ConsoleInputScreen::CA_BACK) return true;

    if (!db.deleteUserInfo(uid)){
        std::cout << "ERROR: " << db.getError().toStdString() <<  ". Press any key to continue" << std::endl;
        getchar();
        return false;
    }

    return true;

}

void Control::newInstitutions(){

    InstDBComm::Institution inst;
    inst = inputInstitutionInfo(inst);

    if (!inst.ok) return;

    qint32 keyid = db.addNewInstitution(inst);

    if (keyid == -1){
        std::cout << "ERROR Adding new institution: " << db.getError().toStdString() << ". Press any key to continue" << std::endl;
        getchar();
        return;
    }
    showInfoScreen(true,QString::number(keyid));
}

void Control::resetPasswInstitution(){

    InstDBComm::Institution inst =  institutionSelection();
    if (!inst.ok) return;

    // Setting the neww password
    if (!db.resetPassword(inst.keyid)){
        std::cout << "ERROR Resetting password: " << db.getError().toStdString() << ". Press any key to continue" << std::endl;
        getchar();
        return;
    }

    // Showing the new password
    showInfoScreen(true,inst.keyid);

}

void Control::updateInstitution(){

    InstDBComm::Institution inst = institutionSelection();
    if (!inst.ok) return;

    inst =  inputInstitutionInfo(inst);
    if (!inst.ok) return;

    if (!db.updateNewInstitution(inst)){
        std::cout << "ERROR: Updating instituion information " << db.getError().toStdString() << ". Press any key to continue" << std::endl;
        getchar();
        return;
    }

    // Showing updated information.
    showInfoScreen(false,inst.keyid);

}

void Control::printInstitutionInfo(){

    InstDBComm::Institution inst = institutionSelection();
    if (!inst.ok) return;

    // Showing the information
    showInfoScreen(false,inst.keyid);

}

InstDBComm::Institution Control::institutionSelection(){

    ConsoleInputScreen screen;
    InstDBComm::Institution inst;
    inst.ok = true;
    inst.keyid = -1;


    // Selecting the institution
    bool ok;
    QList<InstDBComm::Institution> info = db.getAllInstitutions(&ok);
    if (!ok){
        std::cout << "ERROR Getting institution list: " << db.getError().toStdString() << ". Press any key to continue" << std::endl;
        getchar();
        inst.ok = false;
        return inst;
    }

    for (qint32 i = 0; i < info.size(); i++){
        screen.addMenuOption(info.at(i).name,info.at(i).keyid);
    }
    screen.setMenuTitle("Select institution");
    screen.show();

    if (screen.getAction() == ConsoleInputScreen::CA_BACK) {
        return inst;
    }
    else {
        inst = db.getInstitutionInfo(screen.getSelectedData().toString());
        if (!inst.ok){
            std::cout << "ERROR Getting institution information: " << db.getError().toStdString() << ". Press any key to continue" << std::endl;
            getchar();
        }
        return inst;
    }
}



InstDBComm::Institution Control::inputInstitutionInfo(InstDBComm::Institution inst){

    ConsoleInputScreen screen;
    bool update = !inst.uid.isEmpty();
    inst.ok = true;

    if (update){
        screen.addDataEntryPrompt("Name [" + inst.name  + "]");
        screen.addDataEntryPrompt("Number of evaluations [" + inst.numEvals + "]");
        screen.addDataEntryPrompt("EyeTracker Brand [" + inst.etbrand +  "]");
        screen.addDataEntryPrompt("EyeTracker Model [" + inst.etmodel +  "]");
        screen.addDataEntryPrompt("EyeTracker Serial Number [" + inst.etserial +  "]");
    }
    else {
        screen.addDataEntryPrompt("Name");
        screen.addDataEntryPrompt("Number of evaluations");
        screen.addDataEntryPrompt("EyeTracker Brand");
        screen.addDataEntryPrompt("EyeTracker Model");
        screen.addDataEntryPrompt("EyeTracker Serial Number");
    }

    if (update) screen.setMenuTitle("Input institution information (UID: " + inst.uid  + "). Leave empty to leave as is:");
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
                if (!input.at(INPUT_BRAND).isEmpty())
                    inst.etbrand = input.at(INPUT_BRAND);
                if (!input.at(INPUT_MODEL).isEmpty())
                    inst.etmodel = input.at(INPUT_MODEL);
                if (!input.at(INPUT_ETSN).isEmpty())
                    inst.etserial = input.at(INPUT_ETSN);
                if (!input.at(INPUT_NAME).isEmpty())
                    inst.name = input.at(INPUT_NAME);
                if (!input.at(INPUT_NEVAL).isEmpty())
                    inst.numEvals = input.at(INPUT_NEVAL);
                break;
            }
        }
        else {
            inst.ok = false;
            break;
        }
    }

    return inst;
}

void Control::showInfoScreen(bool showPassword, const QString &keyid){


    InstDBComm::Institution inst = db.getInstitutionInfo(keyid);

    // Now I need to show the generated information
    ConsoleInputScreen screen;
    screen.setMenuTitle("Instituion: " + inst.name);
    QStringList info;
    info << "Number of evaluations remaining: " + inst.numEvals;
    info << "ET : " + inst.etbrand + " " + inst.etmodel;
    if (showPassword) {
        info << "Password: " + db.getGeneratedPassword();
    }
    info << "Configuation file snippet:";
    info << QString(CONFIG_INST_PASSWORD) + " = " + inst.password + ";";
    info << QString(CONFIG_INST_ETSERIAL) + " = " + inst.etserial + ";";
    info << QString(CONFIG_INST_NAME) + " = " + inst.name + ";";
    info << QString(CONFIG_INST_UID) + " = " + inst.uid + ";";
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
