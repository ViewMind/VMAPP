#include "inputcontrol.h"

InputControl::InputControl(QObject *parent):QThread(parent)
{

}

void InputControl::run(){

    printGreeting();

    std::string line;
    while (true){
        std::getline(std::cin, line);
        if (line == "exit"){
            std::cout << "Exiting" << std::endl;
            break;
        }
        else{
            std::cout << "UNKNOWN CMD: " << line << std::endl;
            std::cout << ">> ";
        }
    }
    emit(exitRequested());
}

void InputControl::printGreeting(){

    std::cout << "================" << PROGRAM_NAME << "================" << std::endl;
    std::cout << "| Version: " << PROGRAM_VERSION << std::endl;
    std::cout << "| Log file: " << LOG_FILE_LOG << std::endl;
    std::cout << "| Type exit to kill the server" << std::endl;
    std::cout << "========================================" << std::endl;
    std::cout << ">> ";
}
