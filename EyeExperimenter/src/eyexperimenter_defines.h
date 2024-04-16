#ifndef EYE_EXPERIMENTER_DEFINES_H
#define EYE_EXPERIMENTER_DEFINES_H

#include <QString>
#include <QJsonDocument>
#include <QJsonObject>
#include <iostream>
#include "../../CommonClasses/eyetracker_defines.h"

namespace Globals{

const qint32 NUMBER_SECONDS_IN_A_DAY = 86400;
const int    REBOOT_CODE = 568; // Just a random number that I thought of. I doens't have to have any real signficance.

namespace DEV_SERVER {
const QString API_URL = "https://devapi.viewmind.net";
const QString REGION  = "DEV";
}

namespace LOCAL {
const QString API_URL = "http://192.168.0.8/vmapi";
//const QString API_URL = "http://10.1.12.73/vmapi";
const QString REGION  = "DBUG";
}

namespace GLOBAL {
const QString API_URL = "https://api.viewmind.ai";
const QString REGION  = "APP";
}

extern QString API_URL;
extern QString REGION;

namespace VMAppSpec {
static const QString ET = "et";
static const QString Region = "region";
}

namespace Paths {
static const QString WORK_DIRECTORY          = "viewmind_etdata";
static const QString LOCALDB                 = "viewmind_etdata/localdb.dat";
static const QString EVALDB                  = "viewmind_etdata/evaldb.dat";
static const QString DBBKPDIR                = "dbbkp";
static const QString CONFIGURATION           = "vmconfiguration";
static const QString APPSPEC                 = "vmappspec";
static const QString VMINSTID                = "vminstid";
static const QString UPDATE_PACKAGE          = "app.zip";
static const QString VMTOOLDIR               = "../VMMaintenanceTool";
static const QString VMTOOLEXE               = "../VMMaintenanceTool/VMMaintenanceTool.exe";
static const QString CHANGELOG_LOCATION      = "changelog";
static const QString CHANGELOG_BASE          = "changelog_";
static const QString DEBUG_OPTIONS_FILE      = "vmdebug";
static const QString TAR_EXE                 = "tar.exe";
static const QString LOGFILE                 = "logfile.log";
static const QString MANUAL_DIR              = "manual";
static const QString FAILED_CALIBRATION_DIR  = "failed_calibrations";
static const QString SUPPORT_EMAIL_FILE      = "support_email.html";
static const QString VM_UPDATE_LINK          = "ViewMind Update.lnk";
static const QString LOGDIR                  = "logs";
static const QString TASKKILL                = "taskkill";
static const QString SUBJECT_DIR_ABORTED     = "exp_aborted";
static const QString SUBJECT_DIR_SENT        = "sent";
static const QString EVAL_DB                 = "evals.json";
}

namespace SupportEmailPlaceHolders {
static const QString APPVERSION = "||APP_VERSION||";
static const QString EVALUATOR  = "||EVALUATOR||";
static const QString EVAL_EMAIL = "||EVAL_EMAIL||";
static const QString INST_NAME  = "||INSTITUTION||";
static const QString INST_ID    = "||INSTITUTION_ID||";
static const QString INSTANCE   = "||INSTANCE||";
static const QString HWSPECS    = "||HW_SPECS||";
static const QString ISSUE      = "||ISSUE||";
}

namespace QCIFields {
// Quality Control Index Fields
static const QString QCI                 = "qci";
static const QString QCI_PASS            = "qci_pass";
static const QString EVALUATION_ID       = "evaluation_id";
static const QString TARBALL_FILE        = "tarballfile";
}

namespace VMConfig {
static const QString INSTITUTION_ID = "institution_id";
static const QString INSTANCE_NUMBER = "instance_number";
static const QString INSTANCE_KEY = "instance_key";
static const QString INSTANCE_HASH_KEY = "instance_hash_key";
static const QString INSTITUTION_NAME = "institution_name";
}


namespace RemoteRenderServerParameters {
static const QString DIR = "render_server";
static const QString EXE = "RRS.exe";
static const QString PORT_FILE = "selected_port";
}

namespace UILanguage {
static const QString EN = "English";
static const QString ES = "Español";
}

namespace ExplanationLanguage {
static QVariantMap GetNameCodeMap() {
    QVariantMap map;
    map["English"]    = "en";
    map["Español"]    = "es";
    //map["中国人"]    = "cn"; Chinese was a test and is not fully available yet.
    map["Deutsch"]    = "de";
    map["Français "]  = "fr";
    map["Português "] = "pt";
    return map;
}
}

namespace Share {
static const QString APP_NAME = "ViewMind Atlas";
static const QString EXPERIMENTER_VERSION_NUMBER = "27.0.0.dev.36.rc.0";
static const QString SEMAPHORE_NAME              = "viewind_eyeexperimenter_semaphore";
static const QString SHAREDMEMORY_NAME           = "viewind_eyeexperimenter_shared_memory";
static const QString PATIENT_UID                 = "patient_uid";
static const QString PATIENT_DIRECTORY           = "patient_directory";
static const QString PATIENT_STUDY_FILE          = "patient_study_file";
static const QString CURRENTLY_LOGGED_EVALUATOR  = "evaluator_logged";
static const QString SELECTED_STUDY              = "selected_study";
static const QString HAND_CALIB_RES              = "hand_calibration_results";
static const QString SELECTED_EVALUATION         = "selected_evaluation";
static const QString API_PARAMETER_KEY           = "api_parameter_key";
static const QString EYEEXPLORER_SYSTEM_VERSION  = "27.8.5";
static const QString APP_RELEASE_DATE            = "DD-MM-YYYY";
}

static bool SetUpRegion(const QString &reg){

    QString region = reg;
    region = region.toLower();

    if (region == "local"){
        API_URL = LOCAL::API_URL;
        REGION = LOCAL::REGION;
        return true;
    }
    else if (region == "dev"){
        API_URL = DEV_SERVER::API_URL;
        REGION = DEV_SERVER::REGION;
        return true;
    }
    else {
        API_URL = GLOBAL::API_URL;
        REGION = GLOBAL::REGION;
        return true;
    }
    return false;
}

namespace BaseFileNames {

static QString MakeMetdataFileName(const QString &basename){
    return basename + "_metadata.json";
}
static bool IsMetadataFileName(const QString &filename){
    return filename.endsWith("_metadata.json");
}

}

namespace EvalStruct {

   static const QString NAME = "name";
   static const QString LIST = "list";

}

// These are keys for notification that come from flow control (which is the object conneted to the RRS) to the Loader.
// For now there is only one thing that the Loader needs to be notified.
// There might be more in the future.
namespace FCL {
   static const QString HMD_KEY_RECEIVED = "hmd_key_received";
   static const QString UPDATE_SAMP_FREQ = "update_sampling_frequecy";
   static const QString UPDATE_AVG_FREQ  = "update_avg_frequecy";
   static const QString UPDATE_MAX_FREQ  = "update_max_frequecy";
}

static QVariantMap SortMapListByStringValue(QList<QVariantMap> list, const QString &index){

    // First we sort by order code.
    bool swapDone = true;
    while (swapDone){
        swapDone = false;
        for (qint32 i = 0; i < list.size()-1; i++){
            if (list.at(i).value(index).toString() > list.at(i+1).value(index).toString()){
                QVariantMap map = list.at(i);
                list.replace(i,list.at(i+1));
                list.replace(i+1,map);
                swapDone = true;
            }
        }
    }

    // Then we form a map. When iteratin through the map, it should do it in order code, since it's alphebetical.
    QVariantMap ans;
    //qDebug() << "After sorting list size" << list.size();
    for (qint32 i = 0; i < list.size(); i++){
        ans.insert(list.at(i).value(index).toString(),list.at(i));
    }
    //qDebug() << "Returning a map of size" << ans.size();
    return ans;
}

}


#endif // EYE_EXPERIMENTER_DEFINES_H
