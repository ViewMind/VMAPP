#ifndef DEBUG_H
#define DEBUG_H

#include <QVariantMap>
#include <QJsonDocument>
#include <QJsonObject>
#include <QVariantMap>
#include <QFile>
#include <iostream>
#include "ConfigurationManager/configurationmanager.h"
#include "LogInterface/staticthreadlogger.h"

#define DBUGSTR(x)   Debug::DEBUG_OPTIONS.getString(x)
#define DBUGINT(x)   Debug::DEBUG_OPTIONS.getInt(x)
#define DBUGBOOL(x)  Debug::DEBUG_OPTIONS.getBool(x)
#define DBUGREAL(x)  Debug::DEBUG_OPTIONS.getReal(x)
#define DBUGSLIST(x) Debug::DEBUG_OPTIONS.getStringList(x)
#define DBUGEXIST(x) Debug::DEBUG_OPTIONS.containsKeyword(x)

namespace Debug {

    static QString QVariantMapToString(const QVariantMap &map){
        QJsonDocument json = QJsonDocument::fromVariant(map);
        QByteArray data  = json.toJson(QJsonDocument::Indented);
        return QString(data);
    }

    static void prettyPrintQVariantMap(const QVariantMap &map){
        std::cout << QVariantMapToString(map).toStdString() << std::endl;
    }

    // Debug options that can be used by any application that include this file.
    namespace Options {
       const QString DISABLE_UPDATE_CHECK       = "disable_update_check";
       const QString LOGIN_USER                 = "login_user";
       const QString LOGIN_PASSWORD             = "login_password";
       const QString DISABLE_DB_CHECKSUM        = "disable_db_checksum";
       const QString NO_RM_STUDIES              = "do_no_remove_studies";
       const QString PRETTY_PRINT_DB            = "pretty_print_db";
       const QString RRS_PACKET_PRINT           = "enable_rrs_packet_printout";
       const QString SHORT_STUDIES              = "short_studies";
       const QString PATIENT_TO_LOAD            = "selected_patient";
       const QString SELECTED_DOCTOR            = "selected_doctor";
       const QString STUDY_CONFIG_MAP           = "study_configutation_map";
       const QString LOAD_PREFIX_CALIB          = "load_calibration_constants";
       const QString ENABLE_GAZE_FOLLOW         = "enable_gaze_follow";
       const QString DBUG_MSG                   = "debug_messages";
       const QString PRINT_PP                   = "print_pp";
       const QString PRINT_QC                   = "print_qc";
       const QString PRINT_SERVER_RESP          = "print_server_response";
       const QString CONFIG_CALIB_VALID         = "config_calib_validation";
       const QString FORCE_N_CALIB_PTS          = "force_n_calibration_points";
       const QString HAND_CALIB_RESULTS         = "hand_calib_results";
       const QString PRETTY_PRINT_STUDIES       = "pretty_print_study_files";
       const QString PRINT_INCOMMING            = "print_incomming_packets";
       const QString OVERRIDE_UPDATE_LINK       = "override_update_link";
       const QString CALIBRATION_RAW_DATA_INPUT = "calibration_gathering_results_packet";
    }

    extern ConfigurationManager DEBUG_OPTIONS;

    static QString LoadOptions(const QString &filepath){
        if (QFile(filepath).exists()){
            if (!DEBUG_OPTIONS.loadConfiguration(filepath)){
                return DEBUG_OPTIONS.getError();
            }
            return "";
        }
        // The purpose of this is approach is that the non existance of a debug file should set all debug options to being disabled. So this is not an error.        
        return "";
    }

    static QString CreateDebugOptionSummary(){
        QStringList allkeys = DEBUG_OPTIONS.getAllKeys();
        QString summary = "";
        for (qint32 i = 0; i < allkeys.size(); i++){
            QString key = allkeys.at(i);
            summary = summary + " - " + key + ": " + DEBUG_OPTIONS.getString(key);
        }
        return summary;
    }

    static QVariantMap parseMultipleDebugOptionLine(const QString &debugValueLine){
        QStringList keyValuePairs = debugValueLine.split("--",Qt::SkipEmptyParts);
        QVariantMap result;
        for (qint32 i = 0; i < keyValuePairs.size(); i++){
            QStringList keyAndValue = keyValuePairs.at(i).split("-",Qt::SkipEmptyParts);
            if (keyAndValue.size() == 2){
                result[keyAndValue.at(0)] = keyAndValue.at(1);
            }
        }
        return result;
    }


}



#endif // DEBUG_H
