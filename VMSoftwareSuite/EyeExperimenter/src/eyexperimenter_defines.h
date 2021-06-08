#ifndef EYE_EXPERIMENTER_DEFINES_H
#define EYE_EXPERIMENTER_DEFINES_H

#include <QString>
#include <QJsonDocument>
#include <QJsonObject>
#include <iostream>

///////////////////////// ONLY ONE OF THESE SHOULD BE ENABLED: EYE TRACKER USED BY THE SYSTEM SELECTION //////////////////////////
#include "../../../CommonClasses/eyetracker_defines.h"

namespace Globals{

   namespace Labeling{
       static const QString MANUFACTURE_DATE = "07/10/2020";
       static const QString SERIAL_NUMBER = "0-00000000-000000";
       static const QString UNIQUE_CHARACTERIZATION_NUMBER = "XXXX";
       static const QString AUTHORIZATION_UID = "authorization_number";
   }
   
   namespace Paths {
      static const QString WORK_DIRECTORY = "viewmind_etdata";
      static const QString LOCALDB = "viewmind_etdata/localdb.dat";
      static const QString DBBKPDIR = "viewmind_etdata/dbbkp";
      static const QString CONFIGURATION = "vmconfiguration";
      static const QString SETTINGS = "vmsettings";
      static const QString PROCESSING_PARAMETERS = "vmpp";
      static const QString SUBJECT_DIR_ABORTED = "unfinished";
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
   }
   
   
   namespace StudyConfiguration {
      // These values NEED to match the ViewStudyStart definitions.
      static const QString UNIQUE_STUDY_ID = "unique_study_id";
       
      // Unique value index for each experiment = 
      static const qint32 INDEX_READING = 0;
      static const qint32 INDEX_BINDING_BC = 1;
      static const qint32 INDEX_BINDING_UC = 2;
      static const qint32 INDEX_NBACKMS = 3;
      static const qint32 INDEX_NBACKRT = 4;
      static const qint32 INDEX_NBACKVS = 5;
      static const qint32 INDEX_PERCEPTION = 6;
      static const qint32 INDEX_GONOGO = 7;
   }
   
   namespace Debug {
      static const bool PRETTY_PRINT_JSON     = true;
      static const bool SHORT_STUDIES         = true;
      static const bool DISABLE_DB_CHECKSUM   = true;
      static const bool USE_LOCAL_API_ADDRESS = true;
      static void prettpPrintQVariantMap(const QVariantMap &map){
          QJsonDocument json = QJsonDocument::fromVariant(map);
          QByteArray data  = json.toJson(QJsonDocument::Indented);
          std::cout << QString(data).toStdString() << std::endl;
      }
   }
      
   namespace UILanguage {
      static const QString EN = "en";
      static const QString ES = "en";
   } 
   
   namespace Share {
       static const QString EXPERIMENTER_VERSION_NUMBER = "16.0.0";
       static const QString EXPERIMENTER_VERSION = EXPERIMENTER_VERSION_NUMBER + " - " + EyeTracker::NAME;
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
       static const QString MAX_DISPERSION_SIZE = "max_dispersion_size";
       static const QString MINIMUM_FIX_DURATION = "minimum_fix_duration";
       static const QString SAMPLE_FREQUENCY = "sample_frequency";
       static const char *  TEXT_CODEC = "UTF-8";
   }
 
}


#endif // EYE_EXPERIMENTER_DEFINES_H
