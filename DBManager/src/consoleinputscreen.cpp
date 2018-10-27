#include "consoleinputscreen.h"

ConsoleInputScreen::ConsoleInputScreen()
{
    screenType = CIT_MENU;
    addedNumbers = false;
}


void ConsoleInputScreen::addDataEntryPrompt(const QString &prompt){
    if (screenType != CIT_DATA_INPUT) clearAll();
    screenType = CIT_DATA_INPUT;
    menuText << prompt;
}

void ConsoleInputScreen::addMenuOption(const QString &option, const QVariant &data){
    if (screenType != CIT_MENU) clearAll();
    screenType = CIT_MENU;
    dataForMenu << data;
    menuText << option;
}

void ConsoleInputScreen::setInformationScreen(const QStringList &information, bool isGoBackAnOption){
    if (screenType != CIT_INFORMATION) clearAll();
    screenType = CIT_INFORMATION;
    menuText << information;
    useGoBack = isGoBackAnOption;
}

void ConsoleInputScreen::setQuestion(const QString &question){
    clearAll();
    screenType = CIT_QUESTION;
    menuText << question;
}

void ConsoleInputScreen::clearAll(){
    selected = 0;
    inputData.clear();
    menuText.clear();
    menuTitle = "";
    dataForMenu.clear();
    action = CA_BACK;
    addedNumbers = false;
}


void ConsoleInputScreen::show(bool noInput){
    switch(screenType){
    case CIT_DATA_INPUT: showPrompts();
        break;
    case CIT_INFORMATION: showInformation(noInput);
        break;
    case CIT_MENU: showMenu();
        break;
    case CIT_QUESTION: showQuestion();
        break;
    }
}

void ConsoleInputScreen::showQuestion(){
    std::string input;
    std::string message;

    while (true){
        // Clearing the screen
        QProcess::execute("clear");

        if (!message.empty()) std::cout << "MESSAGE: " << message << std::endl;

        for (qint32 i = 0; i < menuText.size(); i++){
            std::cout << menuText.at(i).toStdString() << std::endl;
        }

        std::cout << "Press y to accept. Press g to go back and cancel" << std::endl;
        std::cout << ">> ";

        // Getting the navigation.
        std::getline(std::cin,input);

        // Two options g for going back or n for next.
        QString selection = QString::fromStdString(input);
        if (selection == "g") {
            action = CA_BACK;
            break;
        }
        else if (selection == "y"){
            action = CA_SUBMIT;
            break;
        }
        else {
            message = "Invalid input: " + input;
        }
    }
}

void ConsoleInputScreen::showPrompts(){

    std::string input;

    while (true){
        // Clearing the screen
        QProcess::execute("clear");

        std::cout << menuTitle.toStdString() << std::endl;

        // Getting each fo the prompts
        inputData.clear();
        for (qint32 i = 0; i < menuText.size(); i++){
            std::cout << menuText.at(i).toStdString() << ": ";
            std::getline(std::cin,input);
            inputData << QString::fromStdString(input);
        }

        // Asking for navigation.
        std::cout << "Press s to go to Submit" << std::endl;
        std::cout << "Press r to refill the form" << std::endl;
        std::cout << "Press g to Go back." << std::endl;
        std::cout << ">> ";
        bool refill = false;
        while (true){
            std::getline(std::cin,input);
            QString selection = QString::fromStdString(input);
            if ((selection == "g")) {
                action = CA_BACK;
                break;
            }
            else if (selection == "s"){
                action = CA_SUBMIT;
                break;
            }
            else if (selection == "r"){
                refill = true;
                break;
            }
        }

        if (!refill) break;
    }

}

void ConsoleInputScreen::showInformation(bool noInput){

    std::string input;
    std::string message;

    while (true){
        // Clearing the screen
        QProcess::execute("clear");

        if (!message.empty()) std::cout << "MESSAGE: " << message << std::endl;

        for (qint32 i = 0; i < menuText.size(); i++){
            std::cout << menuText.at(i).toStdString() << std::endl;
        }

        if (noInput) return;

        std::cout << "Press n to go to the next screen.";
        if (useGoBack) {
            std::cout << "Press g to go back." << std::endl;
        }
        else std::cout << std::endl;
        std::cout << ">> ";

        // Getting the navigation.
        std::getline(std::cin,input);

        // Two options g for going back or n for next.
        QString selection = QString::fromStdString(input);
        if ((selection == "g") && (useGoBack)) {
            action = CA_BACK;
            break;
        }
        else if (selection == "n"){
            action = CA_NEXT;
            break;
        }
        else {
            message = "Invalid input: " + input;
        }
    }
}

void ConsoleInputScreen::showMenu(){

    qint32 sizeOpt = menuText.size();
    QString sizeAsString = QString::number(sizeOpt);
    sizeOpt = sizeAsString.size();

    // Which set of options to show.
    qint32 numberOfOptionScreens = menuText.size()/MAX_ALLOWED_OPTIONS;
    if (menuText.size() - numberOfOptionScreens*MAX_ALLOWED_OPTIONS > 0) numberOfOptionScreens++;
    qint32 currentScreen = 0;

    if (!addedNumbers){
        // Preparing the options
        for (qint32 i = 0; i < menuText.size(); i++){
            QString opt = QString::number(i);
            while (opt.size() < sizeOpt) opt = " " + opt;
            menuText[i] = opt + ") " + menuText.at(i);
        }
        addedNumbers = true;
    }

    std::string input;
    std::string message;

    while (true){

        // Clearing the screen
        QProcess::execute("clear");

        // Drawing the title
        std::cout << menuTitle.toStdString() << std::endl;

        // Drawing the navigation instructions
        if ((numberOfOptionScreens > 1) && (currentScreen < numberOfOptionScreens)) std::cout << "NAVIGATION: Select Option to processed. Press g to Go back. Press n for Next set of options" << std::endl;
        else std::cout << "NAVIGATION: Select Option to proceed. Press g to Go back" << std::endl;

        if (!message.empty()) std::cout << "MESSAGE: " << message << std::endl;

        // Drawing the menu
        qint32 start = currentScreen*MAX_ALLOWED_OPTIONS;
        qint32 end = start + MAX_ALLOWED_OPTIONS;
        if (end > menuText.size()) end = menuText.size();

        for (qint32 i = start; i < end; i++){
            std::cout << menuText.at(i).toStdString() << std::endl;
        }

        // The prompt
        std::cout << ">> ";

        // Getting the option.
        std::getline(std::cin,input);

        // Three options: valid number input. g for going back or n for next.
        QString selection = QString::fromStdString(input);

        bool ok = false;
        qint32 sel = selection.toInt(&ok);
        if (!ok){
            if (selection == "g") {
                action = CA_BACK;
                break;
            }
            else if ( (selection == "n") && (numberOfOptionScreens > 1) && (currentScreen < numberOfOptionScreens + 1) ){
                currentScreen++;
            }
            else{
                message = "Invalid input: " + input;
            }
        }
        else{
            if ((sel >= 0) && (sel < menuText.size())){
                selected = sel;
                action = CA_SUBMIT;
                break;
            }
        }

    }
}
