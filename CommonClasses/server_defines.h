#ifndef SERVER_DEFINES_H
#define SERVER_DEFINES_H

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

#define   CONFIG_PHP_MAILER_LOCAL                       "php_mailer_local"

#define   DB_NAME_BASE                                  "db_base"
#define   DB_NAME_ID                                    "db_id"
#define   DB_NAME_PATDATA                               "db_patdata"
#define   DB_NAME_DASHBOARD                             "db_dashboard"

// This file is only present in developing environment as a flag to change some code.
#define   LOCAL_FLAG_FILE                               "local_flag_file"

#endif // SERVER_DEFINES_H
