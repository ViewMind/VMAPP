#ifndef RENDERSERVERPACKETNAMES_H
#define RENDERSERVERPACKETNAMES_H

#include <QString>
#include <QVariantMap>

namespace RenderServerPacketType {
   static const QString TYPE_IMAGE                    = "image";
   static const QString TYPE_REMOTE_TARGET_RESOLUTION = "remote_target_resolution";
   static const QString TYPE_CURRENT_EYE_POSITION     = "current_eye_position";
   static const QString TYPE_GONOGO_SPHERE            = "gonogo_sphere";
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

namespace PacketFrame {

   static const QString IMAGE  = "base64EncodedFrame";
   static const QString WIDTH  = "frameWidth";
   static const QString HEIGHT = "frameHeight";
   static const QString FRAME  = "frameNumber";

}

namespace PacketResolution {

   static const QString WIDTH  = "width";
   static const QString HEIGHT = "height";

}

namespace PacketEyeTrackingData {

   namespace EyeData  {
      static const QString X = "x";
      static const QString Y = "y";
   }

   static const QString LEFT = "left";
   static const QString RIGHT = "right";

}

#endif // RENDERSERVERPACKETNAMES_H
