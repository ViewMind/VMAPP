#ifndef DEBUG_H
#define DEBUG_H

#include <QVariantMap>
#include <QJsonDocument>
#include <QJsonObject>
#include <QVariantMap>
#include <QFile>
#include <iostream>
#include "ConfigurationManager/configurationmanager.h"

#define DBUGSTR(x)  Debug::DEBUG_OPTIONS.getString(x)
#define DBUGINT(x)  Debug::DEBUG_OPTIONS.getInt(x)
#define DBUGBOOL(x) Debug::DEBUG_OPTIONS.getBool(x)
#define DBUGREAL(x) Debug::DEBUG_OPTIONS.getReal(x)

namespace Debug {

    static QString QVariantMapToString(const QVariantMap &map){
        QJsonDocument json = QJsonDocument::fromVariant(map);
        QByteArray data  = json.toJson(QJsonDocument::Indented);
        return QString(data);
    }

    static void prettpPrintQVariantMap(const QVariantMap &map){
        std::cout << QVariantMapToString(map).toStdString() << std::endl;
    }

    // Debug options that can be used by any application that include this file.
    namespace Options {
       const QString DISABLE_UPDATE_CHECK = "disable_update_check";
       const QString LOGIN_USER           = "login_user";
       const QString LOGIN_PASSWORD       = "login_password";
       const QString DISABLE_DB_CHECKSUM  = "disable_db_checksum";
       const QString NO_RM_STUDIES        = "do_no_remove_studies";
       const QString FIX_LOG_ENABLED      = "enable_fixation_logs";
       const QString SHORT_STUDIES        = "short_studies";
       const QString PATIENT_TO_LOAD      = "selected_patient";
       const QString SELECTED_DOCTOR      = "selected_doctor";
       const QString QC_STUDY_TO_LOAD     = "qc_study_to_load";
       const QString VIEW_INDEX_TO_LOAD   = "view_index_to_show";
       const QString SHOW_EYES_IN_STUDY   = "show_eyes_in_study";
       const QString FIX_QC_SHA_CHECKS    = "fix_qc_sha_checks";
       const QString STUDY_CONFIG_MAP     = "study_configutation_map";
       const QString LOAD_CALIBRATION_K   = "load_calibration_coeffs";
       const QString ENABLE_GAZE_FOLLOW   = "enable_gaze_follow";
       const QString USE_MOUSE            = "use_mouse";
       const QString OVERRIDE_TIME        = "override_time";
       const QString DBUG_MSG             = "debug_messages";
       const QString LIGHTUP_NBACKRT      = "lightup_nback_rt";
       const QString PRINT_PP             = "print_pp";
       const QString PRINT_QC             = "print_qc";
       const QString RENDER_HITBOXES      = "render_hitboxes";
       const QString PRINT_SERVER_RESP    = "print_server_response";
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


}



#endif // DEBUG_H
