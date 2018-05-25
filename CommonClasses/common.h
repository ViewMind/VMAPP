#ifndef COMMON_H
#define COMMON_H

#include <QDebug>
#include <QDir>
#include <QDesktopWidget>
#include <QApplication>

// Defining text codec for all text files.
#define   COMMON_TEXT_CODEC                             "UTF-8"

// Which value is which eye.
#define   EYE_L                                         0
#define   EYE_R                                         1

// Configuration key values (cmd names in the configuration file)
#define   CONFIG_PATIENT_DIRECTORY                      "patient_directory"
#define   CONFIG_DOCTOR_NAME                            "doctor_name"
#define   CONFIG_MOVING_WINDOW_DISP                     "moving_window_max_dispersion"
#define   CONFIG_MIN_FIXATION_LENGTH                    "minimum_fixation_length"
#define   CONFIG_SAMPLE_FREQUENCY                       "sample_frequency"
#define   CONFIG_DISTANCE_2_MONITOR                     "distance_to_monitor"
#define   CONFIG_XPX_2_MM                               "x_px_mm"
#define   CONFIG_YPX_2_MM                               "y_px_mm"
#define   CONFIG_LATENCY_ESCAPE_RAD                     "latency_escape_radious"
#define   CONFIG_MARGIN_TARGET_HIT                      "margin_target_hit"
#define   CONFIG_FAST_PROCESSING                        "fast_processing"
#define   CONFIG_EXP_LIST                               "experiments"
#define   CONFIG_OUTPUT_DIR                             "output_directory"
#define   CONFIG_RAW_DATA_REPO                          "raw_data_repo"
#define   CONFIG_READING_PX_TOL                         "reading_px_tol_for_target"
#define   CONFIG_EYEPROCESSOR_PATH                      "eyeprocessor_path"
#define   CONFIG_OVERWRITE_BEHAVIOUR                    "overwrite_exp_files"
#define   CONFIG_DUAL_MONITOR_MODE                      "dual_monitor_mode"
#define   CONFIG_EXP_CONFIG_FILE                        "exp_config_file"
#define   CONFIG_DATA_OUTPUT_FILE                       "data_output_file"
#define   CONFIG_PATIENT_AGE                            "patient_age"
#define   CONFIG_PATIENT_NAME                           "patient_name"
#define   CONFIG_ADD_TO_REPORT                          "add_to_report"
#define   CONFIG_REPORT_LANGUAGE                        "report_language"
#define   CONFIG_ENABLE_REPORT_GEN                      "enable_report_generation"
#define   CONFIG_PATIENT_EMAIL                          "patient_email"
#define   CONFIG_REPORT_NO_LOGO                         "no_logo_in_report"
#define   CONFIG_DEMO_MODE                              "demo_mode"
#define   CONFIG_BINDING_NUM_TARGETS                    "binding_num_targets"
#define   CONFIG_BINDING_USE_NUMBERS                    "binding_use_numbers"
#define   CONFIG_BINDING_DEFAULT                        "binding_default"
#define   CONFIG_BC_FILE_FILTER                         "bc_file_filter"
#define   CONFIG_UC_FILE_FILTER                         "uc_file_filter"
#define   CONFIG_DAT_TIME_FILTER_THRESHOLD              "time_filter_threshold"
#define   CONFIG_TCP_PORT                               "tcp_port"
#define   CONFIG_SERVER_ADDRESS                         "server_address"
#define   CONFIG_CONNECTION_TIMEOUT                     "connection_time_out"
#define   CONFIG_DATA_REQUEST_TIMEOUT                   "data_request_time_out"
#define   CONFIG_RUN_AND_QUIT                           "run_and_quit"
#define   CONFIG_NUMBER_OF_PARALLEL_PROCESSES           "number_of_paralell_processes"
#define   CONFIG_WAIT_DATA_TIMEOUT                      "wait_data_timeout"
#define   CONFIG_RESOLUTION_WIDTH                       "config_resolution_width"
#define   CONFIG_RESOLUTION_HEIGHT                      "config_resolution_height"

// Parameters for some of the configurations
#define   CONFIG_P_EXP_FIELDING                         "fielding"
#define   CONFIG_P_EXP_BIDING_UC                        "binding_uc"
#define   CONFIG_P_EXP_BIDING_BC                        "binding_bc"
#define   CONFIG_P_EXP_READING                          "reading"
#define   CONFIG_P_LANG_ES                              "Spanish"
#define   CONFIG_P_LANG_EN                              "English"

// Indexes of the selected experiments
#define   EXP_SEQ_READING_IMAGES_UC_BC                  0
#define   EXP_READING                                   1
#define   EXP_BINDING_UC                                2
#define   EXP_BINDING_BC                                3
#define   EXP_FIELDNG                                   4

// Base names for common files shared accross multiple apps.
#define   FILE_CONFIGURATION                            "configuration"
#define   FILE_OUTPUT_READING                           "reading"
#define   FILE_OUTPUT_BINDING_UC                        "binding_uc"
#define   FILE_OUTPUT_BINDING_BC                        "binding_bc"
#define   FILE_OUTPUT_FIELDING                          "fielding"
#define   FILE_REPORT_NAME                              "report"
#define   FILE_PATIENT_INFO_FILE                        "patient_info"

// Headers for the data files identifying the format of the data in that experiment.
#define   HEADER_READING_EXPERIMENT                     "#READING"
#define   HEADER_IMAGE_EXPERIMENT                       "#IMAGE"
#define   HEADER_FIELDING_EXPERIMENT                    "#FIELDING"

/****************************************************************************************
 * Function that returns the newest dat file in a directory for a given
 * base name.
 * **************************************************************************************/

static inline QString getNewestFile(const QString &directory, const QString &baseName){
    // Getting all the files with the correct based name and sorted by modification time.
    QDir dir(directory);
    QStringList filter;
    filter << baseName + "_*.dat";
    QStringList allfiles = dir.entryList(filter,QDir::Files,QDir::Time);

    // This is used to simply look for simple bc uc files when no expanded versions are desired.
    qint32 i = 0;
    qint32 targetFileNameSize = baseName.size() + 9; // 9 = 1 underscore, 4 numbers, 1 dot, and 3 because of the dat

    // qWarning() << "Searching for files with the base" << baseName;

    while (i < allfiles.size()){
        if (allfiles.at(i).size() != targetFileNameSize){
            allfiles.removeAt(i);
        }
        else i++;
    }


    if (allfiles.isEmpty()) return "";
    return directory + "/" + allfiles.first();
}

#endif // COMMON_H
