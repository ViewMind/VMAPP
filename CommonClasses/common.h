#ifndef COMMON_H
#define COMMON_H


// Used only for extended binding options.
#define  ENABLE_EXPANDED_BINDING_OPTIONS

// Show the console for debugging messages when cannot find bug by locally running it.
// #define  SHOW_CONSOLE

#include <QDebug>
#include <QHash>
#include <QVariantHash>
#include <QGraphicsScene>
#include <QPainter>
#include <QImage>

// Common classes
#include "ConfigurationManager/configurationmanager.h"

// Typedef used for convenience.
typedef QHash<qint32,qreal> ProcessingResults;

// Typedefs for state machines
typedef enum {QS_POINT, QS_PHRASE, QS_QUESTION, QS_INFORMATION} QuestionState;                          // State machine for the reading trials
typedef enum {DS_CROSS, DS_CROSS_TARGET, DS_1, DS_2, DS_3} DrawState;                                   // Draw states for the Fielding experiments
typedef enum {TSB_CENTER_CROSS,TSB_SHOW,TSB_TRANSITION,TSB_TEST,TSB_FINISH} TrialStateBinding;          // State machine states for Binding trials
typedef enum {TSF_START,
              TSF_SHOW_DOT_1,
              TSF_SHOW_DOT_2,
              TSF_SHOW_DOT_3,
              TSF_TRANSITION,
              TSF_SHOW_BLANK_3,
              TSF_SHOW_BLANK_2,
              TSF_SHOW_BLANK_1} TrialStateFielding;                                                       // State machine states for Fielding

// Typedefs for the type of information in a data packet.
typedef enum {DPFT_FILE = 0, DPFT_REAL_VALUE = 1, DPFT_STRING = 2} DataPacketFieldType;

#define   EXPERIMENTER_VERSION                          "1.0.1"
#define   EYE_REP_GEN_VERSION                           "1.1"
#define   SERVER_MANAGER_VERSION                        "1.0"

#ifdef ENABLE_EXPANDED_BINDING_OPTIONS   
   #define   EXPANDED_VERSION                           "-EBO"
#else
   #define   EXPANDED_VERSION                           ""
#endif

// Constant sizes for information
#define   BYTES_FOR_SIZE                                4
#define   BYTES_FOR_REAL                                8

#define   COMMON_TEXT_CODEC                             "UTF-8"

#define   EYE_L                                         0
#define   EYE_R                                         1

// Headers for the data files identifying the format of the data in that experiment.
#define   HEADER_READING_EXPERIMENT                     "#READING"
#define   HEADER_IMAGE_EXPERIMENT                       "#IMAGE"
#define   HEADER_FIELDING_EXPERIMENT                    "#FIELDING"

// Name of Key files for the program
#define   FILE_CONFIGURATION                            "configuration"
#define   FILE_OUTPUT_READING                           "reading"
#define   FILE_OUTPUT_BINDING_UC                        "binding_uc"
#define   FILE_OUTPUT_BINDING_BC                        "binding_bc"
#define   FILE_OUTPUT_FIELDING                          "fielding"
#define   FILE_REPORT_NAME                              "report"
#define   FILE_PATIENT_INFO_FILE                        "patient_info"
 
// Defines that identify values in hash map and how to replace the string in the generated report
#define   STAT_ID_TOTAL_FIXATIONS                       0
#define   STAT_ID_FIRST_STEP_FIXATIONS                  1
#define   STAT_ID_MULTIPLE_FIXATIONS                    2
#define   STAT_ID_SINGLE_FIXATIONS                      3
#define   STAT_ID_UC_CORRECT                            4
#define   STAT_ID_UC_WRONG                              5
#define   STAT_ID_BC_WRONG                              6
#define   STAT_ID_BC_CORRECT                            7
#define   STAT_ID_BC_PUPIL_R                            8
#define   STAT_ID_UC_PUPIL_R                            9
#define   STAT_TEXT_DOCTOR                              12
#define   STAT_TEXT_DATE                                13
#define   STAT_TEXT_PATIENT                             14
#define   STAT_TEXT_OBSERVATIONS                        15
#define   STAT_TEXT_OUTPUTFILENAME                      16
#define   STAT_ID_FIELDING_HIT_COUNT                    17
#define   STAT_ID_FIELDING_HIT_POSSIBLE                 18
#define   STAT_ID_ENCODING_MEM_VALUE                    19
#define   STAT_ID_BC_PUPIL_L                            20
#define   STAT_ID_UC_PUPIL_L                            21


// Settings key values
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
#define   CONFIG_OUTPUT_DIR                             "output_dir"
#define   CONFIG_OUTPUT_REPO                            "output_directory"
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
#define   CONFIG_TCP_PORT                               "TCPPort"

// Parameters for some of the configurations
#define   CONFIG_P_EXP_FIELDING                         "fielding"
#define   CONFIG_P_EXP_BIDING_UC                        "binding_uc"
#define   CONFIG_P_EXP_BIDING_BC                        "binding_bc"
#define   CONFIG_P_EXP_READING                          "reading"
#define   CONFIG_P_LANG_ES                              "Spanish"
#define   CONFIG_P_LANG_EN                              "English"


// Processing parameters
#define   READING_N_TO_FILTER_FROM_START                10
#define   READING_MIN_VALID_FIXATION                    51
#define   READING_MAX_VALID_FIXATION                    1750
#define   READING_COMMENT_STRING                        "######"
#define   READING_WARNING_NUM_DATAPOINTS                10

// Binding target dimensions (in mm)
#define   BINDING_LARGE_TARGET_MARK                     "LARGE_TARGETS"
#define   BINDING_TARGET_SIDE                           10
#define   BINDING_TARGET_HS                             0.25
#define   BINDING_TARGET_HL                             2.6
#define   BINDING_TARGET_VS                             1.33
#define   BINDING_TARGET_VL                             3.54
#define   BINDING_TARGET_GRID                           105
#define   BINDING_WARNING_NUM_DATAPOINTS                10

// Defines for the indexes of columns in the Reading CSV files
#define   CSV_READING_SUBJECT_COL                       0
#define   CSV_READING_SENTENCE_COL                      2
#define   CSV_READING_WORD_INDEX_COL                    6
#define   CSV_READING_FIXATION_LENGHT_COL               8
#define   CSV_READING_EYE_ID_COL                        10
#define   CSV_READING_SIZE_CHECK                        12

// Defines for the indexes of columns in the Binding CSV files
#define   CSV_BINDING_SUBJECT_COL                       0
#define   CSV_BINDING_TRIALID_COL                       1
#define   CSV_BINDING_ISTRIAL_COL                       2
#define   CSV_BINDING_TRIALNAME_COL                     3
#define   CSV_BINDING_FIXATION_LENGHT_COL               6
#define   CSV_BINDING_EYE_ID_COL                        7
#define   CSV_BINDING_TRIALTYPE_COL                     4
#define   CSV_BINDING_TRIALRESPONSE_COL                 5
#define   CSV_BINDING_SACADE_COL                        9
#define   CSV_BINDING_PUPIL_COL                         10
#define   CSV_BINDING_SIZE_CHECK                        11

// Defines for the indexes of columns in the Fielding CSV files
#define   CSV_FIELDING_SUBJECT_COL                      0
#define   CSV_FIELDING_TRIALID_COL                      1
#define   CSV_FIELDING_IMGNUMBER_COL                    2
#define   CSV_FIELDING_TARGET_HIT_COL                   3
#define   CSV_FIELDING_EYE_ID_COL                       5
#define   CSV_FIELDING_FIX_LENGTH_COL                   4
#define   CSV_FIELDING_TRIALRESPONSE_COL                5
#define   CSV_FIELDING_LAT_SAC_COL                      6
#define   CSV_FIELDING_ROW_SIZE                         7


// Defines for the reading experiment raw data file.
#define   READ_TI                                       0
#define   READ_XR                                       1
#define   READ_YR                                       2
#define   READ_XL                                       3
#define   READ_YL                                       4
#define   READ_WR                                       5
#define   READ_CR                                       6
#define   READ_WL                                       7
#define   READ_CL                                       8
#define   READ_SL                                       9
#define   READ_PR                                       10
#define   READ_PL                                       11

// Defines for the image experiment raw data file
#define   IMAGE_TI                                      0
#define   IMAGE_XR                                      1
#define   IMAGE_YR                                      2
#define   IMAGE_XL                                      3
#define   IMAGE_YL                                      4
#define   IMAGE_PR                                      5
#define   IMAGE_PL                                      6
          
#define   STR_DIFFERENT                                 "different"
#define   STR_SAME                                      "same"

// Timer constants
#define   TIME_START_CROSS                              250
#define   TIME_IMAGE_1                                  2000
#define   TIME_WHITE_TRANSITION                         900
#define   TIME_IMAGE_2_TIMEOUT                          10000
#define   TIME_FINISH                                   1000

// Defines for fielding experiment raw data file
#define   FIELDING_TI                                   0
#define   FIELDING_XR                                   1
#define   FIELDING_YR                                   2
#define   FIELDING_XL                                   3
#define   FIELDING_YL                                   4
#define   FIELDING_PR                                   5
#define   FIELDING_PL                                   6

// Timer timess
#define   TIME_TRANSITION                               50
#define   TIME_TARGET                                   25
#define   TIME_CONDITION                                50
#define   TIMER_TIME_INTERVAL                           10

// Constants for drawing the squares, cross and target on the screen for fielding
#define   AREA_WIDTH                                    1024
#define   AREA_HEIGHT                                   768
#define   RECT_WIDTH                                    163
#define   RECT_HEIGHT                                   155
#define   RECT_0_X                                      200
#define   RECT_0_Y                                      76
#define   RECT_1_X                                      660
#define   RECT_1_Y                                      76
#define   RECT_2_X                                      830
#define   RECT_2_Y                                      306
#define   RECT_3_X                                      660
#define   RECT_3_Y                                      536
#define   RECT_4_X                                      200
#define   RECT_4_Y                                      536
#define   RECT_5_X                                      30
#define   RECT_5_Y                                      306
#define   TARGET_R                                      42
#define   TARGET_OFFSET_X                               39
#define   TARGET_OFFSET_Y                               35
#define   CROSS_P0_X                                    511
#define   CROSS_P0_Y                                    362
#define   CROSS_P1_X                                    533
#define   CROSS_P1_Y                                    383
#define   CROSS_P2_X                                    511
#define   CROSS_P2_Y                                    405
#define   CROSS_P3_X                                    490
#define   CROSS_P3_Y                                    383

// Threshold for warning value
#define   FIELDING_WARNING_NUM_DATAPOINTS               6

// Possible pauses for the fielding experiment
#define   PAUSE_TRIAL_1                                 32
#define   PAUSE_TRIAL_2                                 64

// Screen resolution is fixed.
#define   SCREEN_W                                      1024
#define   SCREEN_H                                      768

// Scripts for calling eye processor
#define   SCRIPT_CALL_PROCESSOR                         "callprocessor.bat"
#define   SCRIPT_EYEPROCESSOR_EXE                       "EyeReportGen.exe"

// Indexes of the selected experiments
#define   EXP_SEQ_READING_IMAGES_UC_BC                  0
#define   EXP_READING                                   1
#define   EXP_BINDING_UC                                2
#define   EXP_BINDING_BC                                3
#define   EXP_FIELDNG                                   4

// Eye Trackers
#define   EYE_TRACKER_REDM                              0
#define   EYE_TRACKER_MOUSE                             1

#endif // COMMON_H
