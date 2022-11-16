#ifndef RENDERSERVERPACKETNAMES_H
#define RENDERSERVERPACKETNAMES_H

#include <QString>
#include <QVariantMap>

namespace RenderServerPacketType {
   static const QString TYPE_LOG_LOCATION             = "log_location";
   static const QString TYPE_CURRENT_EYE_POSITION     = "current_eye_position";
   static const QString TYPE_GONOGO_SPHERE            = "gonogo_sphere";
   static const QString TYPE_2D_RENDER                = "render_2d";             // RRS receives this packet to know where to draw 2D.
   static const QString TYPE_2D_CONTROL               = "render_2d_control";     // RRS sends this packet so that the receiver knows the resolution is getting.  It also receives this packet so that it knows to go into 2D render mode or not.
   static const QString TYPE_SERVER_ACK               = "ack";
}

namespace GL2DItemType {

   static const int TYPE_RECT     = 0;
   static const int TYPE_CIRCLE   = 1;
   static const int TYPE_LINE     = 2;
   static const int TYPE_TRIANGLE = 3;
   static const int TYPE_ARROW    = 4;
   static const int TYPE_TEXT     = 5;

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

namespace PacketEyeTrackingData {

   namespace EyeData  {
      static const QString X = "x";
      static const QString Y = "y";
   }

   static const QString LEFT = "left";
   static const QString RIGHT = "right";

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
    static const QString SPEC_LIST                = "s";
    static const QString FONT_SIZE                = "s";
    static const QString TEXT                     = "text";
    static const QString BG_COLOR                 = "bg";
    static const QString TEXT_ALIGNMENT           = "a";

    // Some extras used fro the item value map.
    static const QString Z                        = "z";
    static const QString TF_ORIGIN                = "tfo";
    static const QString VISIBLE                  = "v";
    static const QString LINE_SPACING             = "ls";
    static const QString REF_Y                    = "ref_y";
}

#endif // RENDERSERVERPACKETNAMES_H
