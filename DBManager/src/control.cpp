#include "control.h"

Control::Control(QObject *parent):QThread(parent)
{
    mainMenu.setMenuTitle("What do you want to do?");
    mainMenu.addMenuOption("New institution",MENU_OPTION_NEW_INSTITUTION);
    mainMenu.addMenuOption("Reset institution password",MENU_OPTION_RESET_PASSWD);
    mainMenu.addMenuOption("Update institution information",MENU_OPTION_UPDATE_INSTITUTION);
    mainMenu.addMenuOption("Add product",MENU_OPTION_NEW_PRODUCT);
    mainMenu.addMenuOption("Modify product",MENU_OPTION_UPDATE_PRODUCT);
    mainMenu.addMenuOption("Search for product",MENU_OPTION_SEARCH_PRODUCT);
    mainMenu.addMenuOption("Delete test entries",MENU_DELETE_TEST_ENTRIES);
    mainMenu.addMenuOption("Exit",MENU_OPTION_EXIT);
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
        switch (mainMenu.getSelectedData().toInt()){
        case MENU_OPTION_NEW_INSTITUTION:
            // Create a new institution
            newInstitutions();
            break;
        case MENU_OPTION_RESET_PASSWD:
            // Reset institution password
            resetPasswInstitution();
            break;
        case MENU_OPTION_UPDATE_INSTITUTION:
            // Update institution information.
            updateInstitution();
            break;
        case MENU_OPTION_NEW_PRODUCT:
            // Add a product for a given institution
            addPlacedProductForInstitution();
            break;
        case MENU_OPTION_UPDATE_PRODUCT:
            // Add a product for a given institution
            modifyProduct();
            break;
        case MENU_OPTION_SEARCH_PRODUCT:
            // Search for products
            searchForProducts();
            break;
        case MENU_DELETE_TEST_ENTRIES:
            // Delete test entries
            deleteTestEntries();
            break;
        case MENU_OPTION_EXIT:
            // Exit
            db.close();
            emit(exitRequested());
            return;
        }

    }

}

bool Control::deleteTestEntries(){

    ConsoleInputScreen confirm;
    confirm.setQuestion("This will delete all users with the UID: " + QString(TEST_UID)+  " Do you want to proceed?");
    confirm.show();

    if (confirm.getAction() == ConsoleInputScreen::CA_BACK) return true;

    bool deletedOne;
    if (!db.deleteTestUsers(&deletedOne)){
        std::cout << "ERROR: " << db.getError().toStdString() <<  ". Press any key to continue" << std::endl;
        getchar();
        return false;
    }

    if (!deletedOne){
        std::cout << "No test users were found. Did nothing. Press any key to continue" << std::endl;
        getchar();
    }

    return true;

}

void Control::newInstitutions(){

    DBQueries::StringMap inst;
    bool accepted;
    inst = inputInstitutionInfo(inst,&accepted);

    if (!accepted) return;

    qint32 keyid = db.addNewInstitution(inst);

    if (keyid == -1){
        std::cout << "ERROR Adding new institution: " << db.getError().toStdString() << ". Press any key to continue" << std::endl;
        getchar();
        return;
    }
    showInstitutionInfoScreen(true,QString::number(keyid));
}

void Control::resetPasswInstitution(){

    bool ok;
    DBQueries::StringMap inst =  institutionSelection(&ok);
    if (!ok) return;

    // Setting the neww password
    if (!db.resetPassword(inst.value(TINST_COL_KEYID))){
        std::cout << "ERROR Resetting password: " << db.getError().toStdString() << ". Press any key to continue" << std::endl;
        getchar();
        return;
    }

    // Showing the new password
    showInstitutionInfoScreen(true,inst.keyid);

}

void Control::updateInstitution(){

    bool ok;
    DBQueries::StringMap inst = institutionSelection(&ok);
    if (!inst.ok) return;

    inst =  inputInstitutionInfo(inst,&ok);
    if (!ok) return;

    if (!db.updateNewInstitution(inst)){
        std::cout << "ERROR: Updating instituion information " << db.getError().toStdString() << ". Press any key to continue" << std::endl;
        getchar();
        return;
    }

    // Showing updated information.
    showInstitutionInfoScreen(false,inst.keyid);

}

void Control::addPlacedProductForInstitution(){

}

DBQueries::StringMap Control::institutionSelection(bool *isOk){

    ConsoleInputScreen screen;
    DBQueries::StringMap inst;
    *isOk = true;

    // Selecting the institution
    bool ok;
    QList<DBQueries::StringMap> info = db.getAllInstitutions(&ok);
    if (!ok){
        std::cout << "ERROR Getting institution list: " << db.getError().toStdString() << ". Press any key to continue" << std::endl;
        getchar();
        *isOk = false;
        return inst;
    }

    for (qint32 i = 0; i < info.size(); i++){
        screen.addMenuOption(info.at(i).value(TINST_COL_NAME),info.at(i).value(TINST_COL_KEYID));
    }
    screen.setMenuTitle("Select institution");
    screen.show();

    if (screen.getAction() == ConsoleInputScreen::CA_BACK) {
        *isOk = false;
        return inst;
    }
    else {
        inst = db.getInstitutionInfo(screen.getSelectedData().toString(),&ok);
        if (!ok){
            *isOk = false;
            std::cout << "ERROR Getting institution information: " << db.getError().toStdString() << ". Press any key to continue" << std::endl;
            getchar();
        }
        return inst;
    }
}

DBQueries::StringMap Control::inputInstitutionInfo(DBQueries::StringMap inst, bool *accepted){

    ConsoleInputScreen screen;
    bool update = !inst.value(TINST_COL_UID).isEmpty();
    *accepted = true;

    if (update){
        screen.addDataEntryPrompt("Institution Name [" + inst.value(TINST_COL_NAME)  + "]",TINST_COL_NAME);
        screen.addDataEntryPrompt("Institution Contact First Name [" + inst.value(TINST_COL_FNAME)  + "]",TINST_COL_FNAME);
        screen.addDataEntryPrompt("Institution Contact Last Name [" + inst.value(TINST_COL_LNAME)  + "]",TINST_COL_LNAME);
        screen.addDataEntryPrompt("Institution Address [" + inst.value(TINST_COL_ADDRESS)  + "]",TINST_COL_ADDRESS);
        screen.addDataEntryPrompt("Institution EMail [" + inst.value(TINST_COL_EMAIL)  + "]",TINST_COL_EMAIL);
        screen.addDataEntryPrompt("Institution Phone [" + inst.value(TINST_COL_PHONE)  + "]",TINST_COL_PHONE);
        screen.addDataEntryPrompt("Number of evaluations [" + inst.value(TINST_COL_EVALS) + "]",TINST_COL_EVALS);
    }
    else {
        screen.addDataEntryPrompt("Institution Name",TINST_COL_NAME);
        screen.addDataEntryPrompt("Institution Contact First Name",TINST_COL_FNAME);
        screen.addDataEntryPrompt("Institution Contact Last Name",TINST_COL_LNAME);
        screen.addDataEntryPrompt("Institution Address",TINST_COL_ADDRESS);
        screen.addDataEntryPrompt("Institution EMail",TINST_COL_EMAIL);
        screen.addDataEntryPrompt("Institution Phone",TINST_COL_PHONE);
        screen.addDataEntryPrompt("Number of evaluations",TINST_COL_EVALS);
    }

    if (update) screen.setMenuTitle("Input institution information (UID: " + inst.value(TINST_COL_UID)  + "). Leave empty to leave as is:");
    else screen.setMenuTitle("Input institution information");

    while (true){
        screen.show();
        if (screen.getAction() == ConsoleInputScreen::CA_SUBMIT){

            // Checking the the number of evalution is a valid number. However if this is an update, then it can be empty.
            QStringList values = screen.getInputedData();
            QStringList columns = screen.getMenuEntryIDs();

            if (update){
                // When updating empty means leaving as is.
                for (qint32 i = 0; i < columns.size(); i++){
                    if (!values.isEmpty()){
                        inst[columns.at(i)] = values.at(i);
                    }
                }
            }

            bool ok = true;
            inst.at(TINST_COL_EVALS).toInt(&ok);
            if (!ok){
                std::cout << "ERROR: Number of evaluations should be a integer. Press any key to continue" << std::endl;
                getchar();
                // Should ask for infromation again.
            }
        }
        else {
            *accepted = false;
            break;
        }
    }

    return inst;
}

DBQueries::StringMap Control::inputProductInfo(DBQueries::StringMap product, bool *accepted){

    ConsoleInputScreen screen;
    bool update = !product.value(TPLACED_PROD_COL_PRODSN).isEmpty();
    *accepted = true;

    if (update){
        screen.addDataEntryPrompt("Product Name [" + product.value(TPLACED_PROD_COL_PRODUCT)  + "]",TPLACED_PROD_COL_PRODUCT);
        screen.addDataEntryPrompt("Product Software Version [" + product.value(TPLACED_PROD_COL_SOFTVER)  + "]",TPLACED_PROD_COL_SOFTVER);
        screen.addDataEntryPrompt("PC Model [" + product.value(TPLACED_PROD_COL_PCMODEL)  + "]",TPLACED_PROD_COL_PCMODEL);
        screen.addDataEntryPrompt("ET Brand [" + product.value(TPLACED_PROD_COL_ETBRAND)  + "]",TPLACED_PROD_COL_ETBRAND);
        screen.addDataEntryPrompt("ET Model [" + product.value(TPLACED_PROD_COL_ETMODEL)  + "]",TPLACED_PROD_COL_ETMODEL);
        screen.addDataEntryPrompt("ET S/N [" + product.value(TPLACED_PROD_COL_ETSERIAL)  + "]",TPLACED_PROD_COL_ETSERIAL);
        screen.addDataEntryPrompt("ChinRest Model [" + product.value(TPLACED_PROD_COL_CHINRESTMODEL) + "]",TPLACED_PROD_COL_CHINRESTMODEL);
        screen.addDataEntryPrompt("ChinRest S/N [" + product.value(TPLACED_PROD_COL_CHINRESTSN) + "]",TPLACED_PROD_COL_CHINRESTSN);
    }
    else {
        screen.addDataEntryPrompt("Product Name [" + product.value(TPLACED_PROD_COL_PRODUCT)  + "]",TPLACED_PROD_COL_PRODUCT);
        screen.addDataEntryPrompt("Product Software Version [" + product.value(TPLACED_PROD_COL_SOFTVER)  + "]",TPLACED_PROD_COL_SOFTVER);
        screen.addDataEntryPrompt("PC Model [" + product.value(TPLACED_PROD_COL_PCMODEL)  + "]",TPLACED_PROD_COL_PCMODEL);
        screen.addDataEntryPrompt("ET Brand [" + product.value(TPLACED_PROD_COL_ETBRAND)  + "]",TPLACED_PROD_COL_ETBRAND);
        screen.addDataEntryPrompt("ET Model [" + product.value(TPLACED_PROD_COL_ETMODEL)  + "]",TPLACED_PROD_COL_ETMODEL);
        screen.addDataEntryPrompt("ET S/N [" + product.value(TPLACED_PROD_COL_ETSERIAL)  + "]",TPLACED_PROD_COL_ETSERIAL);
        screen.addDataEntryPrompt("ChinRest Model [" + product.value(TPLACED_PROD_COL_CHINRESTMODEL) + "]",TPLACED_PROD_COL_CHINRESTMODEL);
        screen.addDataEntryPrompt("ChinRest S/N [" + product.value(TPLACED_PROD_COL_CHINRESTSN) + "]",TPLACED_PROD_COL_CHINRESTSN);
    }

    if (update) screen.setMenuTitle("Input Placed Product information (UID: " + product.value(TINST_COL_UID)  + "). Leave empty to leave as is:");
    else screen.setMenuTitle("Input institution information");

    while (true){
        screen.show();
        if (screen.getAction() == ConsoleInputScreen::CA_SUBMIT){

            // Checking the the number of evalution is a valid number. However if this is an update, then it can be empty.
            QStringList values = screen.getInputedData();
            QStringList columns = screen.getMenuEntryIDs();

            if (update){
                // When updating empty means leaving as is.
                for (qint32 i = 0; i < columns.size(); i++){
                    if (!values.isEmpty()){
                        product[columns.at(i)] = values.at(i);
                    }
                }
            }

            bool ok = true;
            product.at(TINST_COL_EVALS).toInt(&ok);
            if (!ok){
                std::cout << "ERROR: Number of evaluations should be a integer. Press any key to continue" << std::endl;
                getchar();
                // Should ask for infromation again.
            }
        }
        else {
            *accepted = false;
            break;
        }
    }

    return product;
}

void Control::showInstitutionInfoScreen(bool showPassword, const QString &keyid){

    bool isOk = true;
    DBQueries::StringMap inst = db.getInstitutionInfo(keyid,&isOk);
    if (!isOk){
        std::cout << "ERROR: Getting instituion info: " << db.getError().toStdString() << ". Press any key to continue" << std::endl;
        getchar();
        return;
    }

    // Now I need to show the generated information
    ConsoleInputScreen screen;
    screen.setMenuTitle("Instituion Name: " + inst.value(TINST_COL_NAME));
    QStringList info;
    info << "Number of evaluations remaining: " + inst.value(TINST_COL_EVALS);
    if (showPassword) {
        info << "Password: " + db.getGeneratedPassword();
    }
    info << "Institution Contact First Name: " <<  inst.value(TINST_COL_FNAME);
    info << "Institution Contact Last Name: "  <<  inst.value(TINST_COL_LNAME);
    info << "Institution Address: "            <<  inst.value(TINST_COL_ADDRESS);
    info << "Institution EMail: "              <<  inst.value(TINST_COL_EMAIL);
    info << "Institution Phone: "              <<  inst.value(TINST_COL_PHONE);

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
