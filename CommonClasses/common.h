#ifndef COMMON_H
#define COMMON_H

#include <QDebug>
#include <QDir>
#include <QDesktopWidget>
#include <QApplication>
#include "LogInterface/loginterface.h"

// Which value is which eye.
#define   EYE_L                                         0
#define   EYE_R                                         1
#define   EYE_BOTH                                      2

// SQL Query type: GET or SET
#define   SQL_QUERY_TYPE_GET                            "get"
#define   SQL_QUERY_TYPE_SET                            "set"

// Configuration key values (cmd names in the configuration file)
/// TODO : Clean up unused defines.
#define   CONFIG_PATIENT_DIRECTORY                      "patient_directory"
#define   CONFIG_DOCTOR_NAME                            "doctor_name"
#define   CONFIG_DOCTOR_UID                             "doctor_uid"
#define   CONFIG_DOCTOR_EMAIL                           "doctor_email"
#define   CONFIG_MOVING_WINDOW_DISP                     "moving_window_max_dispersion"
#define   CONFIG_MIN_FIXATION_LENGTH                    "minimum_fixation_length"
#define   CONFIG_SAMPLE_FREQUENCY                       "sample_frequency"
#define   CONFIG_DISTANCE_2_MONITOR                     "distance_to_monitor"
#define   CONFIG_XPX_2_MM                               "x_px_mm"
#define   CONFIG_YPX_2_MM                               "y_px_mm"
#define   CONFIG_LATENCY_ESCAPE_RAD                     "latency_escape_radious"
#define   CONFIG_MARGIN_TARGET_HIT                      "margin_target_hit"
#define   CONFIG_EXP_LIST                               "experiments"
#define   CONFIG_OUTPUT_DIR                             "output_directory"
#define   CONFIG_RAW_DATA_REPO                          "raw_data_repo"
#define   CONFIG_READING_PX_TOL                         "reading_px_tol_for_target"
#define   CONFIG_EYEPROCESSOR_PATH                      "eyeprocessor_path"
#define   CONFIG_DUAL_MONITOR_MODE                      "dual_monitor_mode"
#define   CONFIG_EXP_CONFIG_FILE                        "exp_config_file"
#define   CONFIG_DATA_OUTPUT_FILE                       "data_output_file"
#define   CONFIG_PATIENT_NAME                           "patient_name"
#define   CONFIG_PATIENT_UID                            "patient_uid"
#define   CONFIG_PATIENT_AGE                            "patient_age"
#define   CONFIG_ADD_TO_REPORT                          "add_to_report"
#define   CONFIG_REPORT_LANGUAGE                        "report_language"
#define   CONFIG_REPORT_NO_LOGO                         "no_logo_in_report"
#define   CONFIG_DEMO_MODE                              "demo_mode"
#define   CONFIG_DAT_TIME_FILTER_THRESHOLD              "time_filter_threshold"
#define   CONFIG_SERVER_ADDRESS                         "server_address"
#define   CONFIG_CONNECTION_TIMEOUT                     "connection_time_out"
#define   CONFIG_DATA_REQUEST_TIMEOUT                   "data_request_time_out"
#define   CONFIG_WAIT_REPORT_TIMEOUT                    "wait_report_timeout"
#define   CONFIG_WAIT_DBDATA_TIMEOUT                    "wait_db_data_timeout"
#define   CONFIG_NUMBER_OF_PARALLEL_PROCESSES           "number_of_paralell_processes"
#define   CONFIG_RESOLUTION_WIDTH                       "config_resolution_width"
#define   CONFIG_RESOLUTION_HEIGHT                      "config_resolution_height"
#define   CONFIG_SELECTED_ET                            "selected_eyetracker"
#define   CONFIG_PATIENT_REPORT_DIR                     "patient_report_dir"
#define   CONFIG_REPORT_DATE                            "report_date"
#define   CONFIG_SSLSERVER_PATH                         "ssl_server_path"
#define   CONFIG_REPORT_PATH                            "report_path"
#define   CONFIG_IMAGE_REPORT_PATH                      "image_report_path"
#define   CONFIG_VALID_EYE                              "valid_eye"
#define   CONFIG_DBUSER                                 "db_user"
#define   CONFIG_DBNAME                                 "db_name"
#define   CONFIG_DBHOST                                 "db_host"
#define   CONFIG_DBPASSWORD                             "db_passwd"
#define   CONFIG_DBPORT                                 "db_port"
#define   CONFIG_OFFLINE_MODE                           "offline_mode"
#define   CONFIG_DEFAULT_COUNTRY                        "default_country"
#define   CONFIG_BINDING_TARGET_SMALL                   "binding_target_small"
#define   CONFIG_BINDING_NUMBER_OF_TARGETS              "binding_number_of_targets"
#define   CONFIG_USE_MOUSE                              "use_mouse"
#define   CONFIG_EYETRACKER_CONFIGURED                  "eyetracker_configured"
#define   CONFIG_FILE_BIDING_UC                         "file_binding_uc"
#define   CONFIG_FILE_BIDING_BC                         "file_binding_bc"
#define   CONFIG_FILE_READING                           "file_reading"
#define   CONFIG_FILE_FIELDING                          "file_fielding"

// Result values for the Result EyeReport Generator File
#define   CONFIG_RESULTS_ATTENTIONAL_PROCESSES          "attentional_processes"
#define   CONFIG_RESULTS_EXECUTIVE_PROCESSES            "executive_proceseses"
#define   CONFIG_RESULTS_WORKING_MEMORY                 "working_memory"
#define   CONFIG_RESULTS_RETRIEVAL_MEMORY               "retrieval_memory"
#define   CONFIG_RESULTS_MEMORY_ENCODING                "memory_encoding"
#define   CONFIG_RESULTS_BEHAVIOURAL_RESPONSE           "behavioural_response"

// Parameters for some of the configurations
#define   CONFIG_P_EXP_FIELDING                         "fielding"
#define   CONFIG_P_EXP_BIDING_UC                        "binding_uc"
#define   CONFIG_P_EXP_BIDING_BC                        "binding_bc"
#define   CONFIG_P_EXP_READING                          "reading"
#define   CONFIG_P_LANG_ES                              "Spanish"
#define   CONFIG_P_LANG_EN                              "English"
#define   CONFIG_P_ET_MOUSE                             "Mouse"
#define   CONFIG_P_ET_REDM                              "REDm"
#define   CONFIG_P_ET_GP3HD                             "GP3HD"

// Indexes of the selected experiments
#define   EXP_READING                                   1
#define   EXP_BINDING_UC                                2
#define   EXP_BINDING_BC                                3
#define   EXP_FIELDNG                                   4

// Base names for common files shared accross multiple apps.
#define   FILE_CONFIGURATION                            "configuration"
#define   FILE_SETTINGS                                 "settings"
#define   FILE_OUTPUT_READING                           "reading"
#define   FILE_OUTPUT_BINDING_UC                        "binding_uc"
#define   FILE_OUTPUT_BINDING_BC                        "binding_bc"
#define   FILE_OUTPUT_FIELDING                          "fielding"
#define   FILE_REPORT_NAME                              "report"
#define   FILE_DBDATA_FILE                              "dbdata.dbf"

// Headers for the data files identifying the format of the data in that experiment.
#define   HEADER_READING_EXPERIMENT                     "#READING"
#define   HEADER_IMAGE_EXPERIMENT                       "#IMAGE"
#define   HEADER_FIELDING_EXPERIMENT                    "#FIELDING"

// TCP Ports.
static const quint16 TCP_PORT_DB_COMM                    = 54915;
static const quint16 TCP_PORT_DATA_PROCESSING            = 54912;

#endif // COMMON_H
