#include "inputcontrol.h"

InputControl::InputControl(QObject *parent):QThread(parent)
{

}

void InputControl::run(){

    printGreeting();

    std::string cline;
    while (true){
        std::getline(std::cin, cline);
        QString line = QString::fromStdString(cline);
        if (line == "exit"){
            std::cout << "Exiting" << std::endl;
            break;
        }
        else if (line == "cleanlog"){
            QFile(LOG_FILE_LOG).remove();
            std::cout << "Log file deleteted" << std::endl;
            std::cout << ">> ";
        }
        else if (line.startsWith("log")){
            QStringList logAndLines = line.split(" ");
            if (logAndLines.size() > 2){
                std::cout << "log should 0 or 1 arguments only" << std::endl;
                std::cout << ">> ";
                continue;
            }
            else{
                qint32 nlines = -1;
                if (logAndLines.size() == 2){
                    QString num = logAndLines.last();
                    bool ok = false;
                    nlines = num.toInt(&ok);
                    if (!ok){
                        std::cout << "Log command parameter should be a valid integer. " << num.toStdString() << " is not a valid integer" << std::endl;
                        std::cout << ">> ";
                        continue;
                    }
                }
                QFile file(LOG_FILE_LOG);
                if (!file.open(QFile::ReadOnly)){
                    std::cout << "Cannot read log file" << std::endl;
                    std::cout << ">> ";
                    continue;
                }
                QTextStream reader(&file);
                QString content = reader.readAll();
                QStringList lines = content.split('\n');
                content.clear();
                qint32 start = 0;

                if ((nlines != -1) && (nlines < lines.size())){
                    start = lines.size() - nlines;
                }

                for (qint32 i = start; i < lines.size(); i++){
                    std::cout << lines.at(i).toStdString() << std::endl;
                }

                std::cout << ">> ";

            }
        }
        else{
            std::cout << "UNKNOWN CMD: " << cline << std::endl;
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
