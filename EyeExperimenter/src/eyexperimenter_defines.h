#ifndef EYE_EXPERIMENTER_DEFINES_H
#define EYE_EXPERIMENTER_DEFINES_H

#include <QString>
#include <QJsonDocument>
#include <QJsonObject>
#include <iostream>
#include "../../CommonClasses/Experiments/experiment.h"
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
//const QString API_URL = "http://192.168.96.24/vmapi";
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
static const QString DBBKPDIR                = "dbbkp";
static const QString CONFIGURATION           = "vmconfiguration";
static const QString APPSPEC                 = "vmappspec";
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
static const QString DATE                = "date";
static const QString INDEX               = "qci";
static const QString MEDIC               = "medic_name";
static const QString STUDY_TYPE          = "type";
static const QString SUBJECT             = "subject_name";
static const QString SUBJECT_INST_ID     = "subject_insitution_id";
static const QString STUDY_FILE          = "file";
static const QString PASS                = "qci_pass";
static const QString DATE_ORDERCODE      = "order_code";
static const QString EVALUATOR           = "evaluator";
static const QString SUBJECT_VM_ID       = "subject_vm_id";

}

namespace VMConfig {
static const QString INSTITUTION_ID = "institution_id";
static const QString INSTANCE_NUMBER = "instance_number";
static const QString INSTANCE_KEY = "instance_key";
static const QString INSTANCE_HASH_KEY = "instance_hash_key";
static const QString INSTITUTION_NAME = "institution_name";
}

namespace VMUILanguages {
static const QString ES = "Español";
static const QString EN = "English";
}

namespace StudyConfiguration {
// These values NEED to match the ViewStudyStart definitions.
static const QString UNIQUE_STUDY_ID = "unique_study_id";

// Will be used when multiple studies need to be in the same file.
static const QString ONGOING_STUDY_FILE = "ongoing_study_file";

// Unique value index for each experiment, as selectable by the evaluator
static const qint32 INDEX_READING = 0;
static const qint32 INDEX_BINDING_BC = 1;
static const qint32 INDEX_BINDING_UC = 2;
static const qint32 INDEX_NBACKRT = 4;
static const qint32 INDEX_NBACKVS = 5;
static const qint32 INDEX_PASSBALL = 6;
static const qint32 INDEX_GONOGO = 7;
static const qint32 INDEX_GNG_SPHERE = 8;
static const qint32 INDEX_NBACK = 9;

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
    map["English"]   = "en";
    map["Español"]   = "es";
    //map["中国人"]   = "cn"; Chinese was a test and is not fully available yet.
    //map["Deutsch"]   = "de";
    //map["Français "] = "fr";s
    return map;
}
}

namespace Share {
static const QString APP_NAME = "ViewMind Atlas";
static const QString EXPERIMENTER_VERSION_NUMBER = "27.0.0.experimental.14";
static const QString SEMAPHORE_NAME = "viewind_eyeexperimenter_semaphore";
static const QString SHAREDMEMORY_NAME = "viewind_eyeexperimenter_shared_memory";
static const QString PATIENT_UID = "patient_uid";
static const QString PATIENT_DIRECTORY = "patient_directory";
static const QString PATIENT_STUDY_FILE = "patient_study_file";
static const QString CURRENTLY_LOGGED_EVALUATOR = "evaluator_logged";
static const QString SELECTED_STUDY = "selected_study";
static const QString HAND_CALIB_RES = "hand_calibration_results";
static const QString NBACK_WAIT_MSG = "nback_wait_msg";
static const QString EYEEXPLORER_SYSTEM_VERSION = "27.6.5";
static const QString APP_RELEASE_DATE = "DD-MM-YYYY";
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
static const QString READING     = "reading";
static const QString BINDING     = "binding";
static const QString NBACKVS     = "nbackvs";
static const QString NBACKRT     = "nbackrt";
static const QString NBACK       = "nback";
static const QString GONOGO_3D   = "gonogo_spheres";
static const QString GONOGO      = "gonogo";
static const QString PASSBALL    = "passball";
static QString MakeMetdataFileName(const QString &basename){
    return basename + "_metadata.json";
}
static bool IsMetadataFileName(const QString &filename){
    return filename.endsWith("_metadata.json");
}

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

}


#endif // EYE_EXPERIMENTER_DEFINES_H
