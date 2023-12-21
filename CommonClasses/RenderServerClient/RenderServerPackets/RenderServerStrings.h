#ifndef RENDERSERVERSTRINGS_H
#define RENDERSERVERSTRINGS_H

#include <QString>
#include <QVariantMap>

namespace RRS {

namespace PacketType {
static const QString TYPE_LOG_LOCATION             = "log_location";             // Sets up where the logs will be located
static const QString TYPE_DBUG_CONTROL             = "debug_control";            // Enable and disables several debug features.
static const QString TYPE_STUDY_CONTROL            = "study_control";            // Control packet containing generic 3D study commands that tells the server what to do.
static const QString TYPE_CALIB_CONTROL            = "calibration_control";      // Calibration control packet.
static const QString TYPE_HAND_CALIB_CONTROL       = "hand_calibration_control"; // Controls hand calibration.
static const QString TYPE_WAIT_MSG                 = "wait_msg";                 // Renders the wait screen.
static const QString TYPE_FREQ_UPDATE              = "freq_update";              // Periodic updates sent to the client regarding the measured sampling frequency.
}

namespace PacketLogLocation {
static const QString LOG_LOCATION  = "log_location";
static const QString APP_NAME      = "app_designation";
static const QString VERSION       = "rrs_version";
static const QString EYETRACKER    = "eye_tracker_version";
}

namespace PacketFreqUpdate {
static const QString FRQ = "frequency";
static const QString AVG = "average";
static const QString MAX = "maximum";
}

namespace PacketDebugControl {
static const QString JSON_DICT_FIELD           = "dbug";
static const QString ENABLE_HMD_EYE_FOLLOW     = "enable_eye_following";
static const QString ENABLE_FORCE_HAND_CALIB   = "force_hand_calibration_results";
static const QString ENABLE_RRS_PACKET_PRINT   = "enable_printout_of_received_packets";
}

namespace PacketStudyControl {
static const QString STUDY_CNF        = "study_configuration";
static const QString CMD              = "command";
static const QString STUDY_CMD        = "study_command";
static const QString STATUS_UPDATE    = "study_status_updates";
static const QString STUDY_DATA_FILE  = "study_data_filename";
}

namespace PacketWaitMsg{
static const QString MSG = "message";
static const QString RENDER_BORDERS = "render_border_check";
}

namespace PacketStudyData {
static const QString ELEMENTS_3D      = "elements_3d";
static const QString TRIAL_LIST_2D    = "trial_list";
static const QString HITBOXES         = "hitboxes";
static const QString EXPLAIN_LENGTH   = "explanation_phase_duration";
static const QString EXAMPLE_LENGTH   = "example_phase_duration";
static const QString EVAL_LENGTH      = "evaluation_phase_duration";
static const QString PAUSE_LEGTH      = "pause_phase_duration";
static const QString N_DP_DURING_EVAL = "number_of_datapoints_during_evaluation";
static const QString RES_W            = "resolution_width";
static const QString RES_H            = "resolution_height";
static const QString FOV              = "field_of_view";
static const QString MESH_FILES       = "mesh_files";
static const QString MESH_STRUCTS     = "mesh_structs";
static const QString ORIGIN_PTS       = "origin_points";
static const QString SPHERE_R         = "sphere_radius";

}

namespace PacketCalibrationControl {
static const QString COMMAND               = "cmd";      // The command.
static const QString CALIBRATION_TARGETS_X = "x";        // The calibration targets
static const QString CALIBRATION_TARGETS_Y = "y";        // The calibration targets
static const QString CALIBRATION_TARGETS_Z = "z";        // The calibration targets
static const QString CALIB_DATA_LX         = "lx";
static const QString CALIB_DATA_LY         = "ly";
static const QString CALIB_DATA_LZ         = "lz";
static const QString CALIB_DATA_RX         = "rx";
static const QString CALIB_DATA_RY         = "ry";
static const QString CALIB_DATA_RZ         = "rz";
static const QString GATHER_TIME           = "gt";       // Gather Timer
static const QString MOVE_TIME             = "mt";       // Move Time;
static const QString N_CALIB_POINTS        = "n";        // Number of calibration points.
static const QString VALIDATION_R          = "validation_R";       // The validation Radious
static const QString COEFFS_R              = "rcoeff";
static const QString COEFFS_L              = "lcoeff";
static const QString CALIB_DATA_START_IND  = "cp_start"; // The list of indexes which indicates where the actual data to use for computation of coefficients starts.
static const QString RES_W                 = "resolution_width";
static const QString RES_H                 = "resolution_height";
}

namespace PacketHandCalibrationControl {
static const QString VERTICAL       = "vertical";
static const QString HORIZONTAL     = "horizonal";
static const QString HANDS          = "hands";
static const QString H_CALIB_RESULT = "h_calibration_result";
static const QString V_CALIB_RESULT = "v_calibration_result";
}


namespace WrapperPacketFields {

static const QString TYPE    = "type";
static const QString PAYLOAD = "payload";
static const QString COUNTER = "counter";

static bool ArePresent(const QVariantMap &map){
    if (!map.contains(TYPE)) return false;
    if (!map.contains(PAYLOAD)) return false;
    if (!map.contains(COUNTER)) return false;
    return true;
}
}

namespace HandCalibrationRequest {
static const int CALIBRATION_LEFT  = 0;
static const int CALIBRATION_RIGHT = 1;
static const int CALIBRATION_BOTH  = 2;
}

namespace CommandCalibrationControls {
static const QString CMD_START_3D              = "start_3d";
static const QString CMD_START_2D              = "start_2d";
static const QString CMD_COEFFICIENTS          = "start_coefficients";
static const QString CMD_DATA_2D               = "data_2d";
static const QString CMD_DATA_3D               = "data_3d";
}

namespace CommandStudyControl {
static const QString CMD_SETUP_STUDY          = "setup";
static const QString CMD_NEXT_EXPLANATION     = "next_explanation";
static const QString CMD_PREVIOUS_EXPLANATION = "previous_explanation";
static const QString CMD_START_EXAMPLE        = "start_examples";
static const QString CMD_NEXT_EXAMPLE         = "next_example";
static const QString CMD_IN_STUDY_COMMAND     = "in_study_command";
static const QString CMD_STUDY_STATUS_UPDATES = "study_status_updates";
static const QString CMD_STUDY_START          = "study_start";
static const QString CMD_ACK                  = "acknowledge";
static const QString CMD_ERROR                = "error";
static const QString CMD_STUDY_END            = "study_end";
static const QString CMD_SEND_STUDY_DATA      = "send_study_data";
static const QString CMD_STUDY_DATA_STORED   =  "study_data_stored";
}

namespace CommandInStudy {
static const QString CMD_ABORT             = "abort";
static const QString CMD_BINDING_SAME      = "binding_same";
static const QString CMD_BINDING_DIFFERENT = "binding_different";
static const QString CMD_CONTINUE          = "continue";
}


}


#endif // RENDERSERVERSTRINGS_H
