#ifndef RENDERSERVERPACKETNAMES_H
#define RENDERSERVERPACKETNAMES_H

#include <QString>
#include <QVariantMap>

namespace RenderServerPacketType {
   static const QString TYPE_LOG_LOCATION             = "log_location";
   static const QString TYPE_2D_RENDER                = "render_2d";             // RRS receives this packet to know where to draw 2D.
   static const QString TYPE_2D_CONTROL               = "render_2d_control";     // RRS sends this packet so that the receiver knows the resolution is getting.  It also receives this packet so that it knows to go into 2D render mode or not.
   static const QString TYPE_IMG_SIZE_REQ             = "img_size_req";          // Requests an image dimensions.
   static const QString TYPE_LOG_MESSAGE              = "log_message";           // Numbered packets with a message for logging. Used almost exclusively on debug.
   static const QString TYPE_3DSTUDY_CONTROL          = "3D_study_control";      // Control packet containing generic 3D study commands that tells the server what to do.
   static const QString TYPE_STUDY_DESCRIPTION        = "study_description";     // The contentes of the packet represent the study description for a given 3D Study.
   static const QString TYPE_STUDY_DATA               = "study_data";            // Study data which signals the end of the study.
   static const QString TYPE_CALIB_CONTROL            = "calibration_control";   // Calibration control packet.
   static const QString TYPE_HAND_CALIB_CONTROL       = "hand_calibration_control";   // Controls hand calibration.
   static const QString TYPE_DBUG_CONTROL             = "debug_control";              // Enable and disables several debug features.
}

namespace CalibrationControlPacketFields {
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
   static const QString COEFFS_l              = "lcoeff";
}

namespace RemoteRenderServerDebugControls {
   static const QString JSON_DICT_FIELD            = "dbug";
   static const QString ENABLE_2D_RENDER_LOG      = "enable_2d_render_log";
   static const QString ENABLE_3D_HMD_LASER_SIGHT = "enable_3d_hmd_laser_sight";
}

namespace HandCalibrationRequest {
   static const int CALIBRATION_LEFT  = 0;
   static const int CALIBRATION_RIGHT = 1;
   static const int CALIBRATION_BOTH  = 2;
}

namespace PacketDebugControl {
   static const QString JSON_DICT_FIELD           = "dbug";
   static const QString ENABLE_RENDER_2D_LOG      = "enable_2d_render_log";
   static const QString ENABLE_3D_HMD_LASER_SIGHT = "enable_3d_hmd_laser_sight";
   static const QString ENABLE_FORCE_HAND_CALIB   = "force_hand_calibration_results";
}

namespace Study3DNames {

   static const QString GO_NO_GO_SPHERE = "gonogo_spheres";

}

namespace CalibrationControlCommands {
   static const int CMD_SETUP        = 0;
   static const int CMD_CALIB_DATA   = 1;
   static const int CMD_COEFFICIENTS = 2;
}

namespace Study3DControlCommands {

   static const int CMD_UPDATE_MESSAGES              = 0;
   static const int CMD_NEXT_EXPLANATION             = 1;
   static const int CMD_PREVIOUS_EXPLANATION         = 2;
   static const int CMD_STUDY_START                  = 3;
   static const int CMD_START_EXAMPLES               = 4;
   static const int CMD_NEXT_EXAMPLES                = 5;
   static const int CMD_REQUEST_STUDY_DESCRIPTION    = 6;

}

namespace Render2DControlPacketFields {
   static const QString WIDTH   = "w";
   static const QString HEIGHT  = "h";
   static const QString ENABLE  = "enable";
   static const QString VERSION = "string server_version";
   static const QString VFOV    = "vfov";
   static const QString HFOV    = "hfov";
}

namespace RenderServerImageNames {
    static const QString BACKGROUND_LOGO              = "logo";
}

namespace GL2DItemType {

   static const int TYPE_RECT     = 0;
   static const int TYPE_CIRCLE   = 1;
   static const int TYPE_LINE     = 2;
   static const int TYPE_TRIANGLE = 3;
   static const int TYPE_ARROW    = 4;
   static const int TYPE_TEXT     = 5;
   static const int TYPE_IMAGE    = 6;

   static QString Name(qint32 type){
       switch (type){
       case TYPE_ARROW: return "Arrow";
       case TYPE_CIRCLE: return "Circle";
       case TYPE_IMAGE: return "Image";
       case TYPE_LINE: return "Line";
       case TYPE_RECT: return "Rect";
       case TYPE_TEXT: return "Text";
       case TYPE_TRIANGLE: return "Tri";
       default: return "Unknown " + QString::number(type);
       }
   }

}

namespace RenderServerPacketFields {
   static const QString TYPE = "type";
   static const QString PAYLOAD = "payload";
   static const QString SKIPPABLE = "s";

   static bool ArePresent(const QVariantMap &map){
       if (!map.contains(TYPE)) return false;
       if (!map.contains(PAYLOAD)) return false;
       return true;
   }
}

namespace PacketLogLocation {

   static const QString LOG_LOCATION  = "log_location";
   static const QString APP_NAME      = "app_designation";

}

namespace PacketNumberedLog {
   static const QString NUMBER  = "packet_number";
   static const QString MESSAGE = "message";
}

namespace PacketImgSizeRequest {
   static const QString WIDTH = "w";
   static const QString HEIGHT = "h";
   static const QString NAME = "name";
}

namespace PacketStudyDescription {

   static const QString FOV        = "field_of_view";
   static const QString TYPE       = "study_type";
   static const QString HAND_MESH  = "hand_mesh";
   static const QString STUDY_DESC = "study_description";
   static const QString ELEMENTS   = "elements_to_track";

}

namespace PacketHandCalibrationControl {
  static const QString VERTICAL       = "vertical";
  static const QString HORIZONTAL     = "horizonal";
  static const QString HANDS          = "hands";
  static const QString H_CALIB_RESULT = "h_calibration_result";
  static const QString V_CALIB_RESULT = "v_calibration_result";
}

namespace Packet3DStudyControl  {

   static const QString COMMAND           = "cmd";
   static const QString COMMAND_ARG       = "cmd_arg";
   static const QString STUDY_MESSAGES    = "s_msg";
   static const QString SHORT_STUDIES     = "ss";
   static const QString DESCRIPTION       = "sdesc";

}

namespace PacketEndStudyData {
   static const QString TIME          = "time_vector";
   static const QString TIME_RECEIVED = "elements";
   static const QString PACKET_NUMBER = "sample_interval";
}

/**
 * Actual string packet name are really short so as to avoid sending unnecessary data.
 */

namespace RenderControlPacketFields {
    static const QString TYPE                     = "type";
    static const QString WIDTH                    = "w";
    static const QString HEIGHT                   = "h";
    static const QString TOP                      = "t";
    static const QString LEFT                     = "l";
    static const QString COLOR                    = "c";
    static const QString BORDER_COLOR             = "bc";
    static const QString BORDER_WIDTH             = "bw";
    static const QString X                        = "x";
    static const QString Y                        = "y";
    static const QString RADIOUS                  = "R";
    static const QString USE_ROUND_CAPS           = "rc";
    static const QString ARROW_HEAD_LENGTH        = "hl";
    static const QString ARROW_HEAD_HEIGHT        = "hh";
    static const QString ARROW_TYPE_TRIANGLE      = "isT";
    static const QString ENABLE_2D_RENDER         = "enable";
    static const QString ENABLE_RENDER_P_DBUG     = "enable";
    static const QString SPEC_LIST                = "s";
    static const QString FONT_SIZE                = "s";
    static const QString TEXT                     = "text";
    static const QString BG_COLOR                 = "bg";
    static const QString TEXT_ALIGNMENT           = "a";
    static const QString DISPLAY_ONLY_FLAG        = "od";
    static const QString FIT_TO_WIDTH             = "fW";
    static const QString FILE_NAME                = "fname";    

    // Some extras used for the value map.
    static const QString ID                       = "id";
    static const QString Z                        = "z";
    static const QString TF_ORIGIN                = "tfo";
    static const QString VISIBLE                  = "v";
    static const QString LINE_SPACING             = "ls";
    static const QString REF_Y                    = "ref_y";
    static const QString VALUE_TO_FIT             = "v2f";
    static const QString TYPE_NAME                = "type_name";
    static const QString IMG_W                    = "imgW";
    static const QString IMG_H                    = "imgH";
    static const QString RENDER_LIST              = "render_list";

    static const QString PARAMETER_ANIMATION_LIST = "parameter_animation_list";

}

namespace AnimationVars {

   static QString PARAMETER        = "parameter";
   static QString INDEX            = "index";
   static QString STEPS            = "steps";
   static QString START            = "start";
   static QString END              = "end";
   static QString STEPCOUNTER      = "stepcounter";
   static QString DELAY_STEPS      = "delay_steps";
   static QString DELTA            = "delta";

}

#endif // RENDERSERVERPACKETNAMES_H
