#ifndef SERVER_DEFINES_H
#define SERVER_DEFINES_H

#include "ConfigurationManager/configurationmanager.h"
//  Defines that control which of the configuration files is used for the databse. ONLY 1 should be active at the time.
//  #define   SERVER_LOCALHOST
//  #define   SERVER_PRODUCTION


//  Defines that control which of the configuration files is used for the databse. ONLY 1 should be active at the time.
#define   CONFIG_FILE                                   "local_config"

#define   SERVER_PROGRAM_NAME                           "EyeSever"
#define   SERVER_PROGRAM_VERSION                        "8.1.2" // 8.1.0

#define   RAW_DATA_SERVER_NAME                          "RawDataServer"
#define   RAW_DATA_SERVER_VERSION                       "4.2.1" // 4.1.0

#define   EYE_UPDATE_SERVER_NAME                        "EyeUpdateServer"
#define   EYE_UPDATE_SERVER_VERSION                     "1.0.2"

#define   CONFIG_DBUSER                                 "db_user"
#define   CONFIG_DBPASSWORD                             "db_passwd"

#define   CONFIG_DATA_DBNAME                            "db_data_name"
#define   CONFIG_DATA_DBHOST                            "db_data_host"
#define   CONFIG_DATA_DBPORT                            "db_data_port"

#define   CONFIG_ID_DBNAME                              "db_id_name"
#define   CONFIG_ID_DBHOST                              "db_id_host"
#define   CONFIG_ID_DBPORT                              "db_id_port"

#define   CONFIG_PATDATA_DBNAME                         "db_patdata_name"
#define   CONFIG_PATDATA_DBHOST                         "db_patdata_host"
#define   CONFIG_PATDATA_DBPORT                         "db_patdata_port"

#define   CONFIG_DASH_DBNAME                            "db_dash_name"
#define   CONFIG_DASH_DBHOST                            "db_dash_host"
#define   CONFIG_DASH_DBPORT                            "db_dash_port"

#define   CONFIG_S3_ADDRESS                             "s3_address"

#define   CONFIG_ETDIR_PATH                             "etdir_path"

#define   CONFIG_EYEPROCESSOR_PATH                      "eyeprocessor_path"
#define   CONFIG_EYEDBMANAGER_PATH                      "eyedbmanager_path"
#define   CONFIG_DATA_REQUEST_TIMEOUT                   "data_request_time_out"

#define   DB_NAME_BASE                                  "db_base"
#define   DB_NAME_ID                                    "db_id"
#define   DB_NAME_PATDATA                               "db_patdata"
#define   DB_NAME_DASHBOARD                             "db_dashboard"

static ConfigurationManager::CommandVerifications getLocalConfigVerifications(){
    ConfigurationManager::CommandVerifications cv;
    ConfigurationManager::Command cmd;

    cmd.clear();
    cmd.type = ConfigurationManager::VT_INT;

    // DB configuration is all strings.
    cmd.clear();
    cv[CONFIG_DBPASSWORD] = cmd;
    cv[CONFIG_DBUSER] = cmd;

    cv[CONFIG_DATA_DBHOST] = cmd;
    cv[CONFIG_DATA_DBNAME] = cmd;

    cv[CONFIG_ID_DBHOST] = cmd;
    cv[CONFIG_ID_DBNAME] = cmd;

    cv[CONFIG_PATDATA_DBHOST] = cmd;
    cv[CONFIG_PATDATA_DBNAME] = cmd;

    cv[CONFIG_DASH_DBHOST] = cmd;
    cv[CONFIG_DASH_DBNAME] = cmd;

    cv[CONFIG_S3_ADDRESS] = cmd;

    cv[CONFIG_EYEPROCESSOR_PATH] = cmd;
    cv[CONFIG_EYEDBMANAGER_PATH] = cmd;

    cmd.type = ConfigurationManager::VT_INT;
    cv[CONFIG_DATA_DBPORT] = cmd;
    cv[CONFIG_ID_DBPORT] = cmd;
    cv[CONFIG_PATDATA_DBPORT] = cmd;
    cv[CONFIG_DASH_DBPORT] = cmd;
    cv[CONFIG_DATA_REQUEST_TIMEOUT] = cmd;

    return cv;
}

#endif // SERVER_DEFINES_H
