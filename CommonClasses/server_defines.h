#ifndef SERVER_DEFINES_H
#define SERVER_DEFINES_H

//  Defines that control which of the configuration files is used for the databse. ONLY 1 should be active at the time.
//#define   SERVER_LOCALHOST
#define   SERVER_PRODUCTION

#define   COMMON_PATH_FOR_DB_CONFIGURATIONS             ":/CommonClasses/DBConfigurations/"

#define   SERVER_PROGRAM_NAME                           "EyeSever"
#define   SERVER_PROGRAM_VERSION                        "8.0.0" // 7.3.0

#define   RAW_DATA_SERVER_NAME                          "RawDataServer"
#define   RAW_DATA_SERVER_VERSION                       "4.0.0"

#define   EYE_UPDATE_SERVER_NAME                        "EyeUpdateServer"
#define   EYE_UPDATE_SERVER_VERSION                     "1.0.0"

#define   CONFIG_DBUSER                                 "db_user"
#define   CONFIG_DBNAME                                 "db_name"
#define   CONFIG_DBHOST                                 "db_host"
#define   CONFIG_DBPASSWORD                             "db_passwd"
#define   CONFIG_DBPORT                                 "db_port"

#define   CONFIG_ID_DBUSER                              "db_id_user"
#define   CONFIG_ID_DBNAME                              "db_id_name"
#define   CONFIG_ID_DBHOST                              "db_id_host"
#define   CONFIG_ID_DBPASSWORD                          "db_id_passwd"
#define   CONFIG_ID_DBPORT                              "db_id_port"

#define   CONFIG_PATDATA_DBUSER                         "db_patdata_user"
#define   CONFIG_PATDATA_DBNAME                         "db_patdata_name"
#define   CONFIG_PATDATA_DBHOST                         "db_patdata_host"
#define   CONFIG_PATDATA_DBPASSWORD                     "db_patdata_passwd"
#define   CONFIG_PATDATA_DBPORT                         "db_patdata_port"

#define   CONFIG_S3_ADDRESS                             "s3_address"

#define   DB_NAME_BASE                                  "db_base"
#define   DB_NAME_ID                                    "db_id"
#define   DB_NAME_PATDATA                               "db_patdata"

#ifdef SERVER_LOCALHOST
   #define  ETDIR_PATH              "../../../VMSoftwareSuite/EyeUpdateServer/bin/etdir/"
#else
   //#define  ETDIR_PATH              "../EyeServer/etdir/"  // OLD PATH.
   #define  ETDIR_PATH              "../EyeUpdateServer/etdir/"
#endif

#endif // SERVER_DEFINES_H
