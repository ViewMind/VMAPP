#ifndef DEFINES_H
#define DEFINES_H

#include <QString>

namespace Globals {

    static const QString VERSION       = "1.0.0";
    static const QString DIR_LOGS      = "logs";
    static const QString LOG_FILE_BASE = "maitenance_log_";


    namespace Paths {
       static const QString APPZIP              = "app.zip";
       static const QString VMCONFIG            = "vmconfiguration";
       static const QString EYEEXP_INSTALL_DIR  = "EyeExperimenter";
       static const QString DIR_TEMP_EYEEXP     = "EEBKP";
       static const QString DIR_TEMP_EYEEXP_NEW = "EyeExperimenter_";
       static const QString EYEEXP_EXECUTABLE   = "EyeExperimenter.exe";
       static const QString DBBKP_DIR           = "dbbkp";
       static const QString VMETDATA            = "viewmind_etdata";
       static const QString VMTOOLDBUG          = "vmtooldebug";
       static const QString TAR_EXE             = "tar.exe";
       static const QString LICENSE_FILE        = "vmconfiguration";
       static const QString DB_FILE             = "localdb.dat";
       static const QString EYEEXP_SHORTCUT     = "Eye Explorer.lnk";       
    }

    namespace DebugOptions {
       static const QString FORCE_EYEEXP_PATH  = "ee_dir";
       static const QString SAME_LOG = "same_log";
       static const QString OVERIDE_API = "api";
    }


}

#endif // DEFINES_H
