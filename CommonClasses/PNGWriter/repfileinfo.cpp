#include "repfileinfo.h"

RepFileInfo::RepFileInfo()
{

}


void RepFileInfo::setDirectory(const QString &directory, AlgorithmVersions alg_ver){

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

        if ((parts.size() < 5) || (parts.size() > 8)){
            logger.appendWarning("Unrecognized rep file format: " + repfile + ". Old format?");
            continue;
        }

        QString date;
        QString reading;
        QString binding;
        QString reading_code = "";
        QString binding_code = "";

        // Setting the date, time, reading and binding message
        switch (parts.size()){
        case 5:
            // OLD FORMAT JUST READING OR BINDING BUT NOT BOTH
            if (parts.at(1).toUpper().contains('R')){
                reading = parts.at(1);
                binding = "N/A";
                // This is very old. Before ther could be more than one study per day. So just the date is used a code.
                reading_code = parts.at(2) + "_" + parts.at(3) + "_" + parts.at(4);
            }
            else{
                reading = "N/A";
                binding = parts.at(1);
                // This is very old. Before ther could be more than one study per day. So just the date is used a code.
                binding_code = parts.at(2) + "_" + parts.at(3) + "_" + parts.at(4);
            }
            date = parts.at(4) + "/" + parts.at(3) + "/" + parts.at(2);
            break;
        case 6:
            // OLD FORMAT READING AND BINDING
            reading = parts.at(1);
            binding = parts.at(2);
            date = parts.at(5) + "/" + parts.at(4) + "/" + parts.at(3);
            reading_code = parts.at(3) + "_" + parts.at(4) + "_" + parts.at(5);
            binding_code = reading_code;
            break;
        case 7:
            // Information can be reading or binding
            if (parts.at(1).contains('R')){
                reading = parts.at(1);
                binding = "N/A";
                reading_code = parts.at(2) + "_" + parts.at(3) + "_" + parts.at(4) + "_" + parts.at(5) + "_" + parts.at(6);
                // Checking the languange and the eye count.
                if (reading.size() == 4){
                    QString lang = reading.mid(1,2);
                    lang = lang.toLower();
                    QString eyes = reading.mid(3,1);
                    reading_code = lang + "_" + eyes + "_" + reading_code;
                }
            }
            else{
                reading = "N/A";
                binding = parts.at(1);
                binding_code = parts.at(2) + "_" + parts.at(3) + "_" + parts.at(4);

                // Checking the binding parameters type.
                if (binding.size() == 4){
                    QString num_targets = binding.mid(1,1);
                    QString size = binding.mid(2,1);
                    QString eyes = binding.mid(3,1);
                    binding_code = num_targets + "_" + size + "_" + eyes + "_" + binding_code;
                }
            }
            date = parts.at(4) + "/" + parts.at(3) + "/" + parts.at(2) + " " + parts.at(5) + ":" + parts.at(6);
            break;
        case 8:
            // The first is the report and the second one is the binding
            reading = parts.at(1);
            binding = parts.at(2);
            date    = parts.at(5) + "/" + parts.at(4) + "/" + parts.at(3) + " " + parts.at(6) + ":" + parts.at(7);

            reading_code = parts.at(3) + "_" + parts.at(4) + "_" + parts.at(5);
            binding_code = reading_code;

            // Checking the languange and the eye count.
            if (reading.size() == 4){
                QString lang = reading.mid(1,2);
                lang = lang.toLower();
                QString eyes = reading.mid(3,1);
                reading_code = lang + "_" + eyes + "_" + reading_code;
            }

            // Checking the binding parameters type.
            if (binding.size() == 4){
                QString num_targets = binding.mid(1,1);
                QString size = binding.mid(2,1);
                QString eyes = binding.mid(3,1);
                binding_code = num_targets + "_" + size + "_" + eyes + "_" + binding_code;
            }

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

        alg_ver.binding_code = binding_code;
        alg_ver.reading_code = reading_code;
        FileList flist = isReportUpToDate(directory, repfile,alg_ver);

        QVariantMap info;
        info[KEY_BINDING] = binding;
        info[KEY_DATE] = date;
        info[KEY_INDEX] = i;
        info[KEY_READING] = reading;
        info[KEY_REPNAME] = repfile;
        info[KEY_SELFLAG] = false;
        info[KEY_ISUPTODATE] = flist.isUpToDate;
        info[KEY_FILELIST] = flist.fileList.join("|");

        //qWarning() << "REP FILE: " << repfile << "FILE LIST: " << flist.fileList;

        repFileInfo << info;
        repData << data;

    }

}

RepFileInfo::FileList RepFileInfo::isReportUpToDate(const QString &directory, const QString &report_file, const AlgorithmVersions &algver){

    FileList ans;
    ans.isUpToDate = true;

    ConfigurationManager repfile;
    QString repFileName = directory + "/" + report_file;
    if (!repfile.loadConfiguration(repFileName,COMMON_TEXT_CODEC)){
        logger.appendError("While trying to find out if report " + report_file +  " is up to date: " + repfile.getError());
        return ans; // Any error and the report will be up to date.
    }

    /////// READING
    // Checking if the reports contains binding or reading version information
    bool done = false;
    if (repfile.containsKeyword(CONFIG_READING_ALG_VERSION)){
        done = true;
        qint32 version = repfile.getInt(CONFIG_READING_ALG_VERSION);
        if (version < algver.readingAlg){
            ans.fileList << repfile.getString(CONFIG_FILE_READING);
            ans.isUpToDate = false;
        }
    }
    else{
        // This is an old file. Eventually this else code section will become obsolete
        bool fileSearchRequired = false;
        bool addAlgVersion = false;
        if (repfile.containsKeyword(CONFIG_RESULTS_READ_PREDICTED_DETERIORATION)){
            // This is an old file that does NOT need updating right now. So the version is added to the file.
            fileSearchRequired = true;
            addAlgVersion = true;
            //ConfigurationManager::setValue(repFileName,COMMON_TEXT_CODEC,CONFIG_READING_ALG_VERSION,1);
        }
        else{
            if (repfile.containsKeyword(CONFIG_RESULTS_ATTENTIONAL_PROCESSES)){
                // This is, indeed, contains reading information. And it needs to be updated
                fileSearchRequired = true;
                ans.isUpToDate = false;
            }
        }
        if (fileSearchRequired){
            // No info available so the file list needs to be constructed from the codes.
            QString processed_data_dir = directory + "/" + DIRNAME_PROCESSED_DATA;
            QStringList filters; filters << "reading*.dat" << "reading*.datf";
            QStringList processedFiles = QDir(processed_data_dir).entryList(filters,QDir::Files|QDir::NoDotAndDotDot,QDir::Time);
            QStringList matches;
            if (processedFiles.isEmpty()){
                logger.appendError("Searching for processed files to match reading code: " + algver.reading_code
                                   + " but found no files in processed directory " + processed_data_dir);
            }
            else{
                for (qint32 i = 0; i < processedFiles.size(); i++){
                    if (processedFiles.at(i).contains(algver.reading_code)) matches << processedFiles.at(i);
                }
                if (matches.size() >= 1){

                    if (matches.size() > 1){
                        logger.appendWarning("Searching for processed files to match reading code: " + algver.reading_code
                                            + " but found more than one match in processed directory " + processed_data_dir
                                            + ". Matches were: " + matches.join(", ") + ". Will use newest file: " + matches.first());
                    }

                    // All good.
                    ans.fileList << matches.first();
                    // Adding it to the file
                    if (addAlgVersion) ConfigurationManager::setValue(repFileName,COMMON_TEXT_CODEC,CONFIG_READING_ALG_VERSION,"1");
                    ConfigurationManager::setValue(repFileName,COMMON_TEXT_CODEC,CONFIG_FILE_READING,matches.first());
                }
                else{
                    logger.appendError("Searching for processed files to match reading code: " + algver.reading_code
                                                                              + " but found no matches in processed directory " + processed_data_dir);
                    ans.isUpToDate = true;
                }
            }
        }
    }


    /////// BINDING
    if (repfile.containsKeyword(CONFIG_BINDING_ALG_VERSION)){
        done = true;
        qint32 version = repfile.getInt(CONFIG_BINDING_ALG_VERSION);
        if (version < algver.bindingAlg){
            ans.fileList << repfile.getString(CONFIG_FILE_BIDING_BC);
            ans.fileList << repfile.getString(CONFIG_FILE_BIDING_UC);
            ans.isUpToDate = false;
        }
    }
    else{
        // This is an old file. Eventually this else code section will become obsolete
        bool fileSearchRequired = false;
        bool addAlgVersion = false;
        if (repfile.containsKeyword(CONFIG_RESULTS_BINDING_CONVERSION_INDEX)){
            // This is an old file that does NOT need updating right now. So the version is added to the file.
            fileSearchRequired = true;
            addAlgVersion = true;
        }
        else{
            if (repfile.containsKeyword(CONFIG_RESULTS_BEHAVIOURAL_RESPONSE)){
                // This is, indeed, contains binding information. And it needs to be updated
                fileSearchRequired = true;
                ans.isUpToDate = false;
            }
        }

        if (fileSearchRequired){
            // No info available so the file list needs to be constructed from the codes.
            QString processed_data_dir = directory + "/" + DIRNAME_PROCESSED_DATA;
            QStringList filters; filters << "binding*.dat" << "binding*.datf";
            QStringList processedFiles = QDir(processed_data_dir).entryList(filters,QDir::Files|QDir::NoDotAndDotDot,QDir::Time);
            QStringList matches;
            if (processedFiles.isEmpty()){
                logger.appendError("Searching for processed files to match binding code: " + algver.binding_code
                                   + " but found no files in processed directory " + processed_data_dir);
            }
            else{
                for (qint32 i = 0; i < processedFiles.size(); i++){
                    if (processedFiles.at(i).contains(algver.binding_code)) matches << processedFiles.at(i);
                }
                if (matches.size() >= 2){

                    if (matches.size() > 2){
                        // Sorting to UC and BC
                        QStringList bclist;
                        QStringList uclist;
                        QStringList temp;
                        for (qint32 j = 0; j < matches.size(); j++){
                            if (matches.at(j).startsWith(FILE_OUTPUT_BINDING_BC)) bclist << matches.at(j);
                            else if (matches.at(j).startsWith(FILE_OUTPUT_BINDING_UC)) uclist << matches.at(j);
                        }

                        temp << bclist.first() << uclist.first();
                        logger.appendError("Searching for processed files to match binding code: " + algver.binding_code
                                                                    + " but found more than two matches in processed directory " + processed_data_dir
                                                                    + ". Matches were: " + matches.join(", ") + ". Will use: " + temp.join(", "));
                        matches.clear();
                        matches = temp;
                    }

                    // Must check that one is a UC file and the other is a BC file.
                    QString bc = "";
                    QString uc = "";
                    for (qint32 j = 0; j < 2; j++){
                        if (matches.at(j).startsWith(FILE_OUTPUT_BINDING_BC)) bc = matches.at(j);
                        else if (matches.at(j).startsWith(FILE_OUTPUT_BINDING_UC)) uc = matches.at(j);
                    }
                    if (!bc.isEmpty() && !uc.isEmpty()){
                        ans.fileList << matches.first() << matches.last();
                        // Adding it to the file
                        if (addAlgVersion) ConfigurationManager::setValue(repFileName,COMMON_TEXT_CODEC,CONFIG_BINDING_ALG_VERSION,"1");
                        ConfigurationManager::setValue(repFileName,COMMON_TEXT_CODEC,CONFIG_FILE_BIDING_BC,bc);
                        ConfigurationManager::setValue(repFileName,COMMON_TEXT_CODEC,CONFIG_FILE_BIDING_UC,uc);
                    }
                    else{
                        if (bc.isEmpty()) logger.appendError("Searching for processed files to match binding code: " + algver.binding_code
                                                             + " in processed directory " + processed_data_dir
                                                             + ". Found 2 matches but could not determine BC: " + matches.join(", "));
                        if (uc.isEmpty()) logger.appendError("Searching for processed files to match binding code: " + algver.binding_code
                                                             + " in processed directory " + processed_data_dir
                                                             + ". Found 2 matches but could not determine UC: " + matches.join(", "));
                        ans.isUpToDate = true;

                    }

                }
                else{
                    logger.appendError("Searching for processed files to match binding code: " + algver.binding_code
                                                                              + " but found no matches in processed directory " + processed_data_dir);
                    ans.isUpToDate = true;
                }
            }
        }
    }

    return ans;

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
