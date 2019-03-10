#include "repfileinfo.h"

RepFileInfo::RepFileInfo()
{

}


void RepFileInfo::setDirectory(const QString &directory){

    repFileInfo.clear();
    repData.clear();

    // Getting all rep files
    QStringList filters;
    filters << "*.rep";
    QStringList repFiles = QDir(directory).entryList(filters,QDir::Files);
    if (repFiles.isEmpty()) return;

    for (qint32 i = 0; i < repFiles.size(); i++){
        QString repfile = repFiles.at(i);

        if (!repfile.startsWith(FILE_REPORT_NAME)) continue;

        // Splitting at the dot.
        QStringList parts = repfile.split(".");
        QString basename = parts.at(0);
        parts = basename.split("_");

        if ((parts.size() < 7) || (parts.size() > 8)){
            logger.appendWarning("Unrecognized rep file format: " + repfile + ". Old format?");
            continue;
        }

        QString date;
        QString reading;
        QString binding;

        // Setting the date, time, reading and binding message
        switch (parts.size()){
        case 7:
            // Information can be reading or binding
            if (parts.at(1).contains('r')){
                reading = "OK";
                binding = "N/A";
            }
            else{
                reading = "N/A";
                binding = parts.at(1);
            }
            date = parts.at(4) + "/" + parts.at(3) + "/" + parts.at(2) + " " + parts.at(5) + ":" + parts.at(6);
            break;
        case 8:
            // The first is the report and the second one is the binding
            reading = "OK";
            binding = parts.at(2);
            date = parts.at(5) + "/" + parts.at(4) + "/" + parts.at(3) + " " + parts.at(6) + ":" + parts.at(7);
            break;
        }

        // Parsing the results file.
        ConfigurationManager config;
        if (!config.loadConfiguration(directory + "/" + repfile,COMMON_TEXT_CODEC)){
            logger.appendError("Could not load report file: " + repfile + " due to error: " + config.getError());
            continue;
        }

        QVariantMap data = config.getMap();

        // Checking the validity of the information, starting with reading.
        if (data.contains(CONFIG_RESULTS_ATTENTIONAL_PROCESSES)){
            QString attproc = data.value(CONFIG_RESULTS_ATTENTIONAL_PROCESSES).toString();
            if ((attproc == "0") || (attproc == "nan")){
                // All reading results are invalid in this case.
                reading = "N/A";
            }
        }

        // Now binding.
        if (data.contains(CONFIG_RESULTS_MEMORY_ENCODING)){
            QString memenc = data.value(CONFIG_RESULTS_MEMORY_ENCODING).toString();
            if ((memenc == "0") || (memenc == "nan")){
                // All reading results are invalid in this case.
                binding = "N/A";
            }
        }

        //qWarning() << "Adding info from" << repFiles.at(i)  << " and date is" << date;

        QVariantMap info;
        info[KEY_BINDING] = binding;
        info[KEY_DATE] = date;
        info[KEY_INDEX] = i;
        info[KEY_READING] = reading;
        info[KEY_REPNAME] = repfile;
        info[KEY_SELFLAG] = false;

        repFileInfo << info;
        repData << data;

    }

}

QVariantMap RepFileInfo::nextReportInfo() {
    if (currentIterValue < repFileInfo.size()){
        qint32 now = currentIterValue;
        currentIterValue++;
        return repFileInfo.at(now);
    }
    else return QVariantMap();
}

QVariantMap RepFileInfo::getRepData(const QString &fileName) const{

    QFileInfo info(fileName);
    QString toSearch = info.baseName() + "." + info.suffix();

    qint32 index = -1;
    for (qint32 i = 0; i < repFileInfo.size(); i++){
        if (repFileInfo.at(i).value(KEY_REPNAME).toString() == toSearch){
            index = i;
            break;
        }
    }

    if (index == -1) return QVariantMap();
    return repData.at(index);

}
