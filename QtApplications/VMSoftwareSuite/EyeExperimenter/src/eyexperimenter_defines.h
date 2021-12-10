#ifndef EYE_EXPERIMENTER_DEFINES_H
#define EYE_EXPERIMENTER_DEFINES_H

#include <QString>
#include <QJsonDocument>
#include <QJsonObject>
#include <iostream>
#include "../../../CommonClasses/Experiments/experiment.h"
#include "../../../CommonClasses/eyetracker_defines.h"

namespace Globals{

   const qint32 NUMBER_SECONDS_IN_A_DAY = 86400;
   const qint32 NUMBER_OF_PERCEPTION_PARTS = 8;
   const int    REBOOT_CODE = 568; // Just a random number that I thought of. I doens't have to have any real signficance.

   namespace EU_REGION {
      const QString API_URL = "https://eu-api.viewmind.ai";
      const QString REGION  = "EU";
   }

   namespace DEV_SERVER {
      const QString API_URL = "https://devapi.viewmind.net";
      const QString REGION  = "DEV";
   }

   namespace LOCAL {
      const QString API_URL = "http://192.168.1.12/vmapi";
      const QString REGION  = "DBUG";
   }

   extern QString API_URL;
   extern QString REGION;

   namespace VMAppSpec {
      static const QString ET = "et";
      static const QString Region = "region";
   }

   namespace Labeling{
       static const QString MANUFACTURE_DATE = "07/10/2020";
       static const QString SERIAL_NUMBER = "0-00000000-000000";
       static const QString UNIQUE_CHARACTERIZATION_NUMBER = "XXXX";
       static const QString AUTHORIZATION_UID = "authorization_number";
   }
   
   namespace Paths {
      static const QString WORK_DIRECTORY = "viewmind_etdata";
      static const QString LOCALDB = "viewmind_etdata/localdb.dat";
      static const QString DBBKPDIR = "dbbkp";
      static const QString CONFIGURATION = "vmconfiguration";
      static const QString SETTINGS = "vmsettings";
      static const QString PARTNERS = "vmpartners";
      static const QString APPSPEC  = "vmappspec";
      static const QString PROCESSING_PARAMETERS = "vmpp";
      static const QString UPDATE_PACKAGE = "app.zip";
      static const QString UPDATE_SCRIPT  = "update.bat";
      static const QString CHANGELOG_LOCATION = "changelog";
      static const QString CHANGELOG_BASE = "changelog_";      
      static const QString DEBUG_OPTIONS_FILE = "vmdebug";
      static const QString USER_MANUAL = "report.pdf";
   }
   
   namespace VMConfig {
      static const QString INSTITUTION_ID = "institution_id";
      static const QString INSTANCE_NUMBER = "instance_number";
      static const QString INSTANCE_KEY = "instance_key";
      static const QString INSTANCE_HASH_KEY = "instance_hash_key";
      static const QString INSTITUTION_NAME = "institution_name";
   }
   
   namespace VMPreferences {
      static const QString UI_LANGUAGE = "ui_language";
      static const QString DEMO_MODE = "demo_mode";
      static const QString USE_MOUSE = "use_mouse";
      static const QString DEFAULT_COUNTRY = "default_country";
      static const QString DUAL_MONITOR_MODE = "dual_monitor_mode";
      static const QString DEFAULT_READING_LANGUAGE = "default_reading_study_language";
      static const QString LAST_SELECTED_PROTOCOL = "last_selected_protocol";
   }

   namespace Partners {
      static const QString ORBIT = "Orbit Labs";
   }
      
   namespace StudyConfiguration {
      // These values NEED to match the ViewStudyStart definitions.
      static const QString UNIQUE_STUDY_ID = "unique_study_id";
       
      // Unique value index for each experiment, as selectable by the evaluator
      static const qint32 INDEX_READING = 0;
      static const qint32 INDEX_BINDING_BC = 1;
      static const qint32 INDEX_BINDING_UC = 2;
      static const qint32 INDEX_NBACKMS = 3;
      static const qint32 INDEX_NBACKRT = 4;
      static const qint32 INDEX_NBACKVS = 5;
      static const qint32 INDEX_PERCEPTION = 6;
      static const qint32 INDEX_GONOGO = 7;

   }
         
   namespace UILanguage {
      static const QString EN = "English";
      static const QString ES = "Español";
   } 
   
   namespace Share {
       //static const QString EXPERIMENTER_VERSION_NUMBER = "18.2.2";
   static const QString EXPERIMENTER_VERSION_NUMBER = "19.0.0";
       extern QString EXPERIMENTER_VERSION;
       static const QString SEMAPHORE_NAME = "viewind_eyeexperimenter_semaphore";
       static const QString SHAREDMEMORY_NAME = "viewind_eyeexperimenter_shared_memory";
       static const QString PATIENT_UID = "patient_uid";
       static const QString PATIENT_DIRECTORY = "patient_directory";
       static const QString PATIENT_STUDY_FILE = "patient_study_file";
       static const QString CURRENTLY_LOGGED_EVALUATOR = "evaluator_logged";
       static const QString STUDY_DISPLAY_RESOLUTION_WIDTH = "study_display_resolution_width";
       static const QString STUDY_DISPLAY_RESOLUTION_HEIGHT = "study_display_resolution_height";
       static const QString MONITOR_RESOLUTION_WIDTH = "monitor_resolution_width";
       static const QString MONITOR_RESOLUTION_HEIGHT = "monitor_resolution_height";
       static const QString SELECTED_STUDY = "selected_study";
   }

   static bool SetUpRegion(const QString &reg){

       QString region = reg;
       region = region.toLower();

       if (region == "eu"){
           API_URL = EU_REGION::API_URL;
           REGION = EU_REGION::REGION;
           return true;
       }
       else if (region == "local"){
           API_URL = LOCAL::API_URL;
           REGION = LOCAL::REGION;
           return true;
       }
       else if (region == "dev"){
           API_URL = DEV_SERVER::API_URL;
           REGION = DEV_SERVER::REGION;
           return true;
       }

       return false;
   }

   static void SetExperimenterVersion(const QString institutionDescription) {
       QString dbug_str = Debug::CreateDebugOptionSummary();
       Share::EXPERIMENTER_VERSION = Share::EXPERIMENTER_VERSION_NUMBER + " - " + EyeTracker::NAME + " - " + institutionDescription  + " - " + REGION;
       if (!dbug_str.isEmpty()){
           Share::EXPERIMENTER_VERSION = Share::EXPERIMENTER_VERSION + " - " + dbug_str;
       }
   }

   namespace BaseFileNames {
       static const QString READING = "reading";
       static const QString BINDING = "binding";
       static const QString NBACKVS = "nbackvs";
       static const QString NBACKRT = "nbackrt";
       static const QString NBACKMS = "nbackms";
       static const QString PERCEPTION = "perception";
       static const QString GONOGO = "gonogo";
   }
 
}


#endif // EYE_EXPERIMENTER_DEFINES_H
