#ifndef RENDERSERVERPACKETNAMES_H
#define RENDERSERVERPACKETNAMES_H

#include <QString>
#include <QVariantMap>

namespace RenderServerPacketType {
   static const QString TYPE_LOG_LOCATION             = "log_location";
   static const QString TYPE_2D_RENDER                = "render_2d";             // RRS receives this packet to know where to draw 2D.
   static const QString TYPE_2D_CONTROL               = "render_2d_control";     // RRS sends this packet so that the receiver knows the resolution is getting.  It also receives this packet so that it knows to go into 2D render mode or not.
   static const QString TYPE_2D_DBUG_ENABLE           = "debug_enable";
   static const QString TYPE_IMG_SIZE_REQ             = "img_size_req";          // Requests an image dimensions.
   static const QString TYPE_LOG_MESSAGE              = "log_message";           // Numbered packets with a message for logging. Used almost exclusively on debug.
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

}

namespace RenderServerPacketFields {
   static const QString TYPE = "type";
   static const QString PAYLOAD = "payload";

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
    static const QString RENDER_SERVER_VERSION    = "server_version";

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
