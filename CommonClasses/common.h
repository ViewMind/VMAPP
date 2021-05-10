#ifndef COMMON_H
#define COMMON_H

#include "LogInterface/loginterface.h"

// Which value is which eye.
#define   EYE_L                                         0
#define   EYE_R                                         1
#define   EYE_BOTH                                      2

// SQL Query type: GET or SET
#define   SQL_QUERY_TYPE_GET                            "get"
#define   SQL_QUERY_TYPE_SET                            "set"

// The directory name where the raw data will be located
#define   DIRNAME_RAWDATA                               "viewmind_etdata"
#define   DIRNAME_DBBKP                                 "dbbkp"
#define   DIRNAME_LAUNCHER                              "launcher"
#define   DIRNAME_PROCESSED_DATA                        "processed_data"
#define   DIRNAME_ARCHIVE                               "archive"
#define   DIRNAME_ABORTED                               "exp_aborted"
#define   DIRNAME_OLD_REP                               "old_reports"
#define   DIRNAME_SERVER_WORKDIR                        "OUTPUTS"
#define   DIRNAME_SERVER_LOGS                           "LOGDIR"

// The semaphore name. To prevent multiple instances for the same application
#define   SEMAPHORE_NAME                                "viewind_eyeexperimenter_semaphore"
#define   SHAREDMEMORY_NAME                             "viewind_eyeexperimenter_shared_memory"

// Configuration key values (cmd names in the configuration file)
#define   CONFIG_PATIENT_DIRECTORY                      "patient_directory"
#define   CONFIG_DOCTOR_NAME                            "doctor_name"
#define   CONFIG_DOCTOR_UID                             "doctor_uid"
#define   CONFIG_DOCTOR_WORK_UID                        "doctor_work_uid"
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
#define   CONFIG_READING_PX_TOL                         "reading_px_tol_for_target"
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
#define   CONFIG_WAIT_REPORT_TIMEOUT                    "wait_report_timeout"
#define   CONFIG_WAIT_DBDATA_TIMEOUT                    "wait_db_data_timeout"
#define   CONFIG_DATA_REQUEST_TIMEOUT                   "data_request_time_out"
#define   CONFIG_RESOLUTION_WIDTH                       "config_resolution_width"
#define   CONFIG_RESOLUTION_HEIGHT                      "config_resolution_height"
#define   CONFIG_SELECTED_ET                            "selected_eyetracker"
#define   CONFIG_PATIENT_REPORT_DIR                     "patient_report_dir"
#define   CONFIG_REPORT_DATE                            "report_date"
#define   CONFIG_SSLSERVER_PATH                         "ssl_server_path"
#define   CONFIG_REPORT_PATH                            "report_path"
#define   CONFIG_IMAGE_REPORT_PATH                      "image_report_path"
#define   CONFIG_VALID_EYE                              "valid_eye"
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
#define   CONFIG_FILE_NBACKRT                           "file_nbackrt"
#define   CONFIG_FILE_GONOGO                            "file_gonogo"
#define   CONFIG_LIST_DONE_EXPERIMENTS                  "list_done_experiments"
#define   CONFIG_INST_NAME                              "inst_name"
#define   CONFIG_INST_UID                               "inst_uid"
#define   CONFIG_INST_ETSERIAL                          "inst_et_serial"
#define   CONFIG_INST_PASSWORD                          "inst_pass"
#define   CONFIG_TEST_MODE                              "test_mode"
#define   CONFIG_ENABLE_GAZE_FOLLOWING                  "enable_gaze_following"
#define   CONFIG_READING_EXP_LANG                       "reading_exp_lang"
#define   CONFIG_REPORT_FILENAME                        "report_filename"
#define   CONFIG_TOL_MAX_PERIOD_TOL                     "tol_max_allowed_period"
#define   CONFIG_TOL_MIN_PERIOD_TOL                     "tol_min_allowed_period"
#define   CONFIG_TOL_MAX_FGLITECHES_IN_TRIAL            "tol_max_fglitches_in_trial"
#define   CONFIG_TOL_MIN_NUMBER_OF_DATA_ITEMS_IN_TRIAL  "tol_min_number_of_data_items_in_trial"
#define   CONFIG_TOL_MAX_PERCENT_OF_INVALID_VALUES      "tol_max_percent_of_invalid_values"
#define   CONFIG_TOL_NUM_ALLOWED_FAILED_DATA_SETS       "tol_num_allowed_of_failed_data_sets"
#define   CONFIG_TOL_NUM_MIN_PTS_IN_FIELDING_TRIAL      "tol_min_num_of_pts_in_fielding_trial"
#define   CONFIG_PROTOCOL_NAME                          "protocol_name"
#define   CONFIG_EYEEXP_NUMBER                          "eyeexp_number"
#define   CONFIG_ENABLE_LOCAL_DB_BKP                    "enable_local_db_bkp"
#define   CONFIG_CONFIGURE_STRING                       "configure_options"
#define   CONFIG_PATIENT_DISPLAYID                      "patient_displayid"
#define   CONFIG_READING_ALG_VERSION                    "reading_algorithm_version"
#define   CONFIG_BINDING_ALG_VERSION                    "binding_algorithm_version"
#define   CONFIG_FIELDING_ALG_VERSION                   "fielding_algorithm_version"
#define   CONFIG_NBACKRT_ALG_VERSION                    "nbackrt_algorithm_version"
#define   CONFIG_GONOGO_ALG_VERSION                     "gonogo_algorithm_version"
#define   CONFIG_RESULT_ENTRY_ID                        "result_entry_id"
#define   CONFIG_LAUCHER_CONNECT_TIMEOUT                "launcher_connect_timeout"
#define   CONFIG_LAUCHER_DOWNLOAD_TIMEOUT               "launcher_download_timeout"
#define   CONFIG_TRANSACTION_ID                         "transaction_id"
#define   CONFIG_DBMNG_ACTION                           "dbmng_action"
#define   CONFIG_DBMNG_RESULT                           "dbmng_result"
#define   CONFIG_REPROCESS_REQUEST                      "reprocess_request"
#define   CONFIG_DATA_SAVED_TO_DB_FLAG                  "data_saved_to_db_flag"
#define   CONFIG_TIMESTAMP                              "timestamp"
#define   CONFIG_REMAINING_EVALUATIONS                  "remaining_evaluations"
#define   CONFIG_TIMESTRING                             "timestring"
#define   CONFIG_VR_ENABLED                             "vr_enabled"
#define   CONFIG_READING_FONT_SIZE                      "reading_font_size"
#define   CONFIG_READING_FONT_NAME                      "reading_font_name"
#define   CONFIG_READING_ESCAPE_POINT_XY_K              "reading_escape_point_xy_k"
#define   CONFIG_FIELDING_PAUSE_TEXT                    "fielding_pause_text"
#define   CONFIG_FIELDING_XPX_2_MM                      "fielding_xpx_2_mm"
#define   CONFIG_FIELDING_YPX_2_MM                      "fielding_ypx_2_mm"
#define   CONFIG_PRIMARY_MONITOR_WIDTH                  "primary_monitor_width"
#define   CONFIG_PRIMARY_MONITOR_HEIGHT                 "primary_monitor_height"
#define   CONFIG_VR_RECOMMENDED_WIDTH                   "vr_recommended_width"
#define   CONFIG_VR_RECOMMENDED_HEIGHT                  "vr_recommended_height"
#define   CONFIG_RESOLUTION_SCALING                     "resolution_scaling"
#define   CONFIG_LABELLING_MANUFACTURE_DATE             "labeling_manufacture_date"
#define   CONFIG_LABELLING_SERIAL_NUMBER                "labeling_serial_number"
#define   CONFIG_LABELLING_AUTHORIZATION_UID            "labeling_unique_authorization_number"
#define   CONFIG_DEFAULT_READING_LANGUAGE               "default_reading_study_language"
#define   CONFIG_NBACKVS_MIN_HOLD_TIME                  "nbackvs_min_hold_time"
#define   CONFIG_NBACKVS_MAX_HOLD_TIME                  "nbackvs_max_hold_time"
#define   CONFIG_NBACKVS_STEP_HOLD_TIME                 "nbackvs_step_hold_time"
#define   CONFIG_NBACKVS_START_HOLD_TIME                "nbackvs_start_hold_time"
#define   CONFIG_NBACKVS_NTRIAL_FOR_STEP_CHANGE         "nbackvs_ntrial_for_step_change"
#define   CONFIG_NBACKVS_SEQUENCE_LENGTH                "nbackvs_sequence_length"
#define   CONFIG_PERCEPTION_IS_TRAINING                 "perception_is_training"
#define   CONFIG_PERCEPTION_MP_CURRENT_IDENTIFIER       "perception_mulit_part_study_current_id"
#define   CONFIG_PERCEPTION_MP_CURRENT_STUDY_FILE       "perception_mulit_part_study_current_study_file"

// Result values for the Result EyeReport Generator File
#define   CONFIG_RESULTS_ATTENTIONAL_PROCESSES          "attentional_processes"
#define   CONFIG_RESULTS_EXECUTIVE_PROCESSES            "executive_proceseses"
#define   CONFIG_RESULTS_WORKING_MEMORY                 "working_memory"
#define   CONFIG_RESULTS_RETRIEVAL_MEMORY               "retrieval_memory"
#define   CONFIG_RESULTS_MEMORY_ENCODING                "memory_encoding"
#define   CONFIG_RESULTS_BEHAVIOURAL_RESPONSE           "behavioural_response"
#define   CONFIG_RESULTS_FREQ_ERRORS_PRESENT            "freq_errors_present"
#define   CONFIG_RESULTS_BC_PREDICTED_DETERIORATION     "bc_predicted_deterioration"
#define   CONFIG_RESULTS_UC_PREDICTED_DETERIORATION     "uc_predicted_deterioration"
#define   CONFIG_RESULTS_BC_PREDICTED_GROUP             "bc_predicted_group"
#define   CONFIG_RESULTS_UC_PREDICTED_GROUP             "uc_predicted_group"
#define   CONFIG_RESULTS_READ_PREDICTED_DETERIORATION   "reading_predicted_deterioration"
#define   CONFIG_RESULTS_BINDING_CONVERSION_INDEX       "binding_conversion_index"
#define   CONFIG_RESULTS_NBACKRT_NUM_FIX_ENC            "number_of_fixations_enc"
#define   CONFIG_RESULTS_NBACKRT_NUM_FIX_RET            "number_of_fixations_ret"
#define   CONFIG_RESULTS_NBACKRT_INHIBITORY_PROBLEMS    "inhibitory_problems_percent"
#define   CONFIG_RESULTS_NBACKRT_SEQ_COMPLETE           "correct_complete_sequence_percent"
#define   CONFIG_RESULTS_NBACKRT_TARGET_HIT             "correct_target_hit_percent"
#define   CONFIG_RESULTS_NBACKRT_MEAN_RESP_TIME         "mean_response_time"
#define   CONFIG_RESULTS_NBACKRT_MEAN_SAC_AMP           "mean_saccade_amplitude"
#define   CONFIG_RESULTS_GNG_SPEED_PROCESSING           "gonogo_speed_processing"
#define   CONFIG_RESULTS_GNG_DMT_FACILITATE             "gonogo_dmt_facilitate"
#define   CONFIG_RESULTS_GNG_DMT_INTERFERENCE           "gonogo_dmt_interference"
#define   CONFIG_RESULTS_GNG_PIP_FACILITATE             "gonogo_pip_facilitate"
#define   CONFIG_RESULTS_GNG_PIP_INTERFERENCE           "gonogo_pip_interference"

// Parameters for some of the configurations
#define   CONFIG_P_EXP_FIELDING                         "fielding"
#define   CONFIG_P_EXP_NBACKRT                          "nbackrt"
#define   CONFIG_P_EXP_NBACKVS                          "nbackvs"
#define   CONFIG_P_EXP_BIDING_UC                        "binding_uc"
#define   CONFIG_P_EXP_BIDING_BC                        "binding_bc"
#define   CONFIG_P_EXP_READING                          "reading"
#define   CONFIG_P_EXP_PARKINSON                        "parkinson"
#define   CONFIG_P_EXP_GONOGO                           "gonogo"
#define   CONFIG_P_EXP_PERCEPTION                       "perception"
#define   CONFIG_P_LANG_ES                              "Spanish"
#define   CONFIG_P_LANG_EN                              "English"
#define   CONFIG_P_ET_MOUSE                             "Mouse"
#define   CONFIG_P_ET_REDM                              "REDm"
#define   CONFIG_P_ET_GP3HD                             "GP3HD"
#define   CONFIG_P_ET_HTCVIVEEYEPRO                     "HTCVIVEEyePro"
#define   CONFIG_P_PERCEPTION_REHAB                     "R"
#define   CONFIG_P_PERCEPTION_TRAINING                  "T"

// Numeric IDs for the exeperiments
#define   LIST_INDEX_READING                            0
#define   LIST_INDEX_BINDING_BC                         1
#define   LIST_INDEX_BINDING_UC                         2
#define   LIST_INDEX_FIELDING                           3
#define   LIST_INDEX_NBACKRT                            4
#define   LIST_INDEX_GONOGO                             5
#define   LIST_INDEX_NBACKVS                            6
#define   LIST_INDEX_PERCEPTION                         7

// Parameters to pass to the DB Manager and tell it what to do.
#define   CONFIG_P_DMBNG_ACTION_CHECK                   "check"
#define   CONFIG_P_DMBNG_ACTION_STORE                   "store"
#define   CONFIG_P_DBMNG_ACTION_PATDATA                 "patdata"

// Indexes of the selected experiments
#define   EXP_READING                                   1
#define   EXP_BINDING_UC                                2
#define   EXP_BINDING_BC                                3
#define   EXP_FIELDNG                                   4
#define   EXP_NBACKRT                                   5
#define   EXP_PARKINSON                                 6
#define   EXP_GONOGO                                    7
#define   EXP_NBACKVS                                   8
#define   EXP_PERCEPTION                                9

// Base names for common files shared accross multiple apps.
#define   FILE_CONFIGURATION                            "configuration"
#define   FILE_SETTINGS                                 "settings"
#define   FILE_OUTPUT_READING                           "reading"
#define   FILE_OUTPUT_BINDING_UC                        "binding_uc"
#define   FILE_OUTPUT_BINDING_BC                        "binding_bc"
#define   FILE_OUTPUT_FIELDING                          "fielding"
#define   FILE_OUTPUT_NBACKRT                           "nbackrt"
#define   FILE_OUTPUT_NBACK_VARIABLE_SPEED              "nbackvs"
#define   FILE_OUTPUT_PARKINSON                         "parkinson"
#define   FILE_OUTPUT_GONOGO                            "gonogo"
#define   FILE_OUTPUT_PERCEPTION                        "perception"
#define   FILE_REPORT_NAME                              "report"
#define   FILE_DBDATA_FILE                              "dbdata.dbf"
#define   FILE_EYE_REP_GEN_CONFIGURATION                "eye_rep_gen_conf"
#define   FILE_GRAPHS_FILE                              "graphs.html"
#define   FILE_PHP_MAIL                                 "mail.php"
#define   FILE_MAIL_ERROR_LOG                           "ferror_mail_detail.html"
#define   FILE_CHANGELOG_UPDATER                        "changelog"
#define   FILE_NEW_LAUCHER_FILE                         "launcher.exe"
#define   FILE_EYE_LAUCHER_FILE                         "EyeLauncherClient.exe"
#define   FILE_OLD_LAUCHER_FILE                         "EyeLauncherClientPrevious.exe"
#define   FILE_LOCAL_DB_BKP                             "localdb_bkp.db"
#define   FILE_LOCAL_DB                                 "localdb.dat"
#define   FILE_DBMNG_COMM_FILE                          "dbmngcomm"
#define   FILE_PATDATA_DB                               "patdata.db"
#define   FILE_DOCDATA_DB                               "docdata.db"
#define   FILE_MEDRECORD_DB                             "medrecords.db"

// Headers for the data files identifying the format of the data in that experiment.
#define   HEADER_READING_EXPERIMENT                     "#READING"
#define   HEADER_IMAGE_EXPERIMENT                       "#IMAGE"
#define   HEADER_FIELDING_EXPERIMENT                    "#FIELDING"
#define   HEADER_NBACKRT_EXPERIMENT                     "#NBACKRT"
#define   HEADER_NBACK_VARIABLE_SPEED_EXPERIMENT        "#NBACKVS"
#define   HEADER_PARKINSON_EXPERIMENT                   "#PARKINSON"
#define   HEADER_GONOGO_EXPERIMENT                      "#GONOGO"
#define   HEADER_PERCEPTION_EXPERIMENT                  "#PERCEPTION"

// Codes indicating the type of update transaction
#define   UPDATE_CHECK_GP_CODE                           "cgp"
#define   UPDATE_CHECK_SMI_CODE                          "csmi"
#define   UPDATE_GET_SMI_CODE                            "gsmi"
#define   UPDATE_GET_GP_CODE                             "ggp"
#define   UPDATE_FORCE_NO_UPDATE_MSG                     "none"

// Codes for reading languages languages.
#define   READING_LANG_SPANISH                           "es"
#define   READING_LANG_ENGLISH                           "en"
#define   READING_LANG_GERMAN                            "de"
#define   READING_LANG_FRENCH                            "fr"

// ID MAP NAMES
#define   IDMAP_ID_PUID                                 "puid"
#define   IDMAP_ID_UID                                  "uid"

// Versions of the processing algorithm
#define   EYE_REP_GEN_READING_ALGORITHM_VERSION          2
#define   EYE_REP_GEN_BINDING_ALGORITHM_VERSION          4
#define   EYE_REP_GEN_FIELDING_ALGORITHM_VERSION         0
#define   EYE_REP_GEN_NBACKRT_ALGORITHM_VERSION          0
#define   EYE_REP_GEN_GONOGO_ALGORITHM_VERSION           0
#define   EYE_REP_GEN_NBACKVS_ALGORITHM_VERSION          0
#define   EYE_REP_GEN_PERCEPTION_ALGORITHM_VERSION       0

// DB Managers possible answers
#define   EYEDBMNG_ANS_OK                                0
#define   EYEDBMNG_ANS_FILE_ERROR                        1
#define   EYEDBMNG_ANS_DB_ERROR                          2
#define   EYEDBMNG_ANS_PARAM_ERROR                       3
#define   EYEDBMNG_ANS_WRONG_SERIAL                      4
#define   EYEDBMNG_ANS_NOEVALS                           5


// The ACK Results sent from the server to the EyeExperimenter
#define   EYESERVER_RESULT_OK                            0
#define   EYESERVER_RESULT_NOEVALS                       1
#define   EYESERVER_RESULT_WRONG_SERIAL                  2
#define   EYESERVER_RESULT_SERVER_ERROR                  3

// Request codes for data packet
#define   REQUEST_CODE_PROCESS_REPORT                    0
#define   REQUEST_CODE_SAVE_PATIENT_DATA                 1

// Field names in data return from raw data server
#define   RAW_DATA_SERVER_RETURN_PATIENT_DATA            "patient_data"
#define   RAW_DATA_SERVER_RETURN_MEDICAL_RECORDS         "medical_records"


// TCP Ports.
static const quint16 TCP_PORT_RAW_DATA_SERVER            = 54918;
static const quint16 TCP_PORT_UPDATE_SERVER              = 54915;
static const quint16 TCP_PORT_DATA_PROCESSING            = 54912;

#endif // COMMON_H
