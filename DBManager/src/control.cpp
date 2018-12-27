#include "control.h"

Control::Control(QObject *parent):QThread(parent)
{
    mainMenu.setMenuTitle("What do you want to do?");
    mainMenu.addMenuOption("New institution",MENU_OPTION_NEW_INSTITUTION);
    mainMenu.addMenuOption("Reset institution password",MENU_OPTION_RESET_PASSWD);
    mainMenu.addMenuOption("Update institution information",MENU_OPTION_UPDATE_INSTITUTION);
    mainMenu.addMenuOption("Add placed product in an institution",MENU_OPTION_NEW_PRODUCT);
    mainMenu.addMenuOption("Modify product",MENU_OPTION_UPDATE_PRODUCT);
    mainMenu.addMenuOption("Search for product",MENU_OPTION_SEARCH_PRODUCT);
    mainMenu.addMenuOption("Delete test entries",MENU_DELETE_TEST_ENTRIES);
    mainMenu.addMenuOption("Exit",MENU_OPTION_EXIT);
}

void Control::run(){

    QStringList loadmsg = getGreeting();

    if (!db.initConnection()){
        loadmsg << "Error on DB Initialization: " + db.getError();
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
            modifyProducts();
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
    showInstitutionInfoScreen(true,inst.value(TINST_COL_KEYID));

}

void Control::updateInstitution(){

    bool ok;
    DBQueries::StringMap inst = institutionSelection(&ok);

    if (!ok) return;

    inst =  inputInstitutionInfo(inst,&ok);
    if (!ok) return;

    if (!db.updateNewInstitution(inst)){
        std::cout << "ERROR: Updating instituion information " << db.getError().toStdString() << ". Press any key to continue" << std::endl;
        getchar();
        return;
    }

    // Showing updated information.
    showInstitutionInfoScreen(false,inst.value(TINST_COL_KEYID));

}

void Control::addPlacedProductForInstitution(){

    bool ok;
    DBQueries::StringMap inst = institutionSelection(&ok);
    if (!ok) return;

    DBQueries::StringMap product = db.getEmptyStringMap(DBQueries::BSMT_PLACED_PRODUCT);
    product = inputProductInfo(product,&ok);
    if (!ok) return;

    product[TPLACED_PROD_COL_INSTITUTION] = inst.value(TINST_COL_UID);

    qint32 keyid = db.addNewProduct(product);

    if (keyid == -1){
        std::cout << "ERROR Adding new placed product: " << db.getError().toStdString() << ". Press any key to continue" << std::endl;
        getchar();
        return;
    }

    // Showing the information
    showPlacedProductInfoScreen(QString::number(keyid));

}

void Control::modifyProducts(){

    bool ok;
    DBQueries::StringMap inst = institutionSelection(&ok);
    if (!ok) return;

    DBQueries::StringMap product = productSelection(&ok,inst.value(TINST_COL_UID));
    if (!ok) return;

    product =  inputProductInfo(product,&ok);
    if (!ok) return;

    if (!db.updateProduct(product)){
        std::cout << "ERROR: Updating product information " << db.getError().toStdString() << ". Press any key to continue" << std::endl;
        getchar();
        return;
    }

    showPlacedProductInfoScreen(product.value(TPLACED_PROD_COL_KEYID));

}

void Control::searchForProducts(){
    ConsoleInputScreen screen;
    screen.addDataEntryPrompt("Input search string","");
    screen.setMenuTitle("Search in placed products");
    while (true){
        screen.show();
        if (screen.getAction() == ConsoleInputScreen::CA_SUBMIT){
            QStringList searchstrlist = screen.getInputedData();

            bool ok;
            QStringList ans = db.searchForPlacedProducts(searchstrlist.first(),&ok);

            if (!ok){
                std::cout << "ERROR: While searching for products " << db.getError().toStdString() << ". Press any key to continue" << std::endl;
                getchar();
                return;
            }

            QFile file("searchoutput");
            if (!file.open(QFile::WriteOnly)){
                std::cout << "ERROR: Could not opern file for search output for writing. Press any key to continue" << std::endl;
                getchar();
                return;
            }

            QTextStream writer(&file);
            writer.setCodec(COMMON_TEXT_CODEC);

            writer << "SEARCH RESULTS FOR: " + searchstrlist.first() + "\n";

            for (qint32 i = 0; i < ans.size(); i++){
                QStringList writeData;
                showPlacedProductInfoScreen(ans.at(i),&writeData);
                writer << ">>>>>>>>>>>>>>><< RESULT " + QString::number(i) + ":\n";
                writer << writeData.join("\n");
            }
            file.close();

            std::cout << "Output written to file searchoutput. Press any key to continue" << std::endl;
            getchar();
            return;


        }
        else break;
    }
}

DBQueries::StringMap Control::productSelection(bool *isOk, const QString &instUID){

    ConsoleInputScreen screen;
    DBQueries::StringMap product;
    *isOk = true;

    // Selecting the institution
    bool ok;
    QList<DBQueries::StringMap> info = db.getAllProductsForInstitutions(instUID,&ok);
    if (!ok){
        std::cout << "ERROR Getting product list: " << db.getError().toStdString() << ". Press any key to continue" << std::endl;
        getchar();
        *isOk = false;
        return product;
    }

    for (qint32 i = 0; i < info.size(); i++){
        screen.addMenuOption(info.at(i).value(TPLACED_PROD_COL_PRODUCT) + " - S/N: " + info.at(i).value(TPLACED_PROD_COL_PRODSN) ,info.at(i).value(TPLACED_PROD_COL_KEYID));
    }
    screen.setMenuTitle("Select product");
    screen.show();

    if (screen.getAction() == ConsoleInputScreen::CA_BACK) {
        *isOk = false;
        return product;
    }
    else {
        product = db.getProductInformation(screen.getSelectedData().toString(),&ok);
        if (!ok){
            *isOk = false;
            std::cout << "ERROR Getting product information: " << db.getError().toStdString() << ". Press any key to continue" << std::endl;
            getchar();
        }
        return product;
    }

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

            // When updating empty means leaving as is.
            for (qint32 i = 0; i < columns.size(); i++){
                if (!values.at(i).isEmpty() || !update){
                    inst[columns.at(i)] = values.at(i);
                }
            }

            bool ok = true;
            //qWarning() << "Converting value |" + inst.value(TINST_COL_EVALS) + "|";
            inst.value(TINST_COL_EVALS).toInt(&ok);

            // This is ok if 1) the number of evaluations is a valid integer. OR 2) We are updating AND no number evaluation has been entered.
            // ok = ok || (update && !inst.contains(TINST_COL_EVALS));

            if (!ok){
                std::cout << "ERROR: Number of evaluations should be a integer. Press any key to continue" << std::endl;
                getchar();
                // Should ask for infromation again.
            }
            else break;
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
        screen.addDataEntryPrompt("Product Name",TPLACED_PROD_COL_PRODUCT);
        screen.addDataEntryPrompt("Product Software Version",TPLACED_PROD_COL_SOFTVER);
        screen.addDataEntryPrompt("PC Model",TPLACED_PROD_COL_PCMODEL);
        screen.addDataEntryPrompt("ET Brand",TPLACED_PROD_COL_ETBRAND);
        screen.addDataEntryPrompt("ET Model" ,TPLACED_PROD_COL_ETMODEL);
        screen.addDataEntryPrompt("ET S/N",TPLACED_PROD_COL_ETSERIAL);
        screen.addDataEntryPrompt("ChinRest Model" ,TPLACED_PROD_COL_CHINRESTMODEL);
        screen.addDataEntryPrompt("ChinRest S/N",TPLACED_PROD_COL_CHINRESTSN);
    }

    if (update) screen.setMenuTitle("Input Placed Product information (UID: " + product.value(TPLACED_PROD_COL_INSTITUTION)  + "). Leave empty to leave as is:");
    else screen.setMenuTitle("Input product information information");

    while (true){
        screen.show();
        if (screen.getAction() == ConsoleInputScreen::CA_SUBMIT){

            // Checking the the number of evalution is a valid number. However if this is an update, then it can be empty.
            QStringList values = screen.getInputedData();
            QStringList columns = screen.getMenuEntryIDs();

            // When updating empty means leaving as is.
            for (qint32 i = 0; i < columns.size(); i++){
                if (!values.at(i).isEmpty()){
                    product[columns.at(i)] = values.at(i);
                }
            }

            break;
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
    info << "Institution Contact First Name: " +  inst.value(TINST_COL_FNAME);
    info << "Institution Contact Last Name: "  +  inst.value(TINST_COL_LNAME);
    info << "Institution Address: "            +  inst.value(TINST_COL_ADDRESS);
    info << "Institution EMail: "              +  inst.value(TINST_COL_EMAIL);
    info << "Institution Phone: "              +  inst.value(TINST_COL_PHONE);

    screen.setInformationScreen(info,false);
    screen.show();

}

void Control::showPlacedProductInfoScreen(const QString & productKeyid, QStringList *result){

    bool isOk = true;
    DBQueries::StringMap product = db.getProductInformation(productKeyid,&isOk);
    if (!isOk){
        std::cout << "ERROR: Getting placed product info: " << db.getError().toStdString() << ". Press any key to continue" << std::endl;
        getchar();
        return;
    }

    // Now I need to show the generated information
    ConsoleInputScreen screen;
    screen.setMenuTitle("Placed Product Information");
    QStringList info;
    info << "Product Name: " + product.value(TPLACED_PROD_COL_PRODUCT);
    info << "Product S/N: " + product.value(TPLACED_PROD_COL_PRODSN);
    info << "Institution: " + product.value(TPLACED_PROD_COL_INSTITUTION);
    info << "Product Software Version: " + product.value(TPLACED_PROD_COL_SOFTVER);
    info << "PC Model: " + product.value(TPLACED_PROD_COL_PCMODEL);
    info << "ET Brand: " + product.value(TPLACED_PROD_COL_ETBRAND);
    info << "ET Model: "  + product.value(TPLACED_PROD_COL_ETMODEL);
    info << "ET S/N: " + product.value(TPLACED_PROD_COL_ETSERIAL);
    info << "ChinRest Model: " + product.value(TPLACED_PROD_COL_CHINRESTMODEL);
    info << "ChinRest S/N: " + product.value(TPLACED_PROD_COL_CHINRESTSN);

    if (result == nullptr){
        info << "====================== CONFIGURATIION CODE SNIPPET: ";
        info << QString(CONFIG_INST_PASSWORD) + " = " + product.value(TINST_COL_HASHPASS) + ";";
        info << QString(CONFIG_INST_UID) + " = " + product.value(TPLACED_PROD_COL_INSTITUTION) + ";";
        info << QString(CONFIG_INST_NAME) + " = " + product.value(TINST_COL_NAME) + ";";
        info << QString(CONFIG_INST_ETSERIAL) + " = " + product.value(TPLACED_PROD_COL_ETSERIAL) + ";";

        screen.setInformationScreen(info,false);
        screen.show();
    }
    else{
        for (qint32 i = 0; i < info.size(); i++){
            result->append(info.at(i));
        }
    }

}

QStringList Control::getGreeting() const {
    QStringList list;
    list << "=============== " + QString(PROGRAM_NAME) + " ===============";
    list << "= VERSION: " + QString(PROGRAM_VERSION);
    list << "===========";
    return list;
}
