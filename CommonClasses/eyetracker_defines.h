#ifndef EYE_TRACKER_DEFINES_H
#define EYE_TRACKER_DEFINES_H

//#define EYETRACKER_HPOMNICENT
//#define EYETRACKER_GAZEPOINT
#define EYETRACKER_HTCVIVEPRO

#include <QString>

namespace Globals {

#ifdef EYETRACKER_HTCVIVEPRO
   namespace EyeTracker {
      static const QString NAME = "HTCVive Pro";
      static const bool ENABLE_GAZE_FOLLOW = true;
      static const bool IS_VR = true;
      static const QString PROCESSING_PARAMETER_KEY = "htcviveeyepro";
   }
#endif

#ifdef EYETRACKER_HPOMNICENT
   namespace EyeTracker {
      static const QString NAME = "HP Omnicent";
      static const bool ENABLE_GAZE_FOLLOW = true;
      static const bool IS_VR = true;
      static const QString PROCESSING_PARAMETER_KEY = "hpomniscent";
   }
#endif

#ifdef EYETRACKER_GAZEPOINT
   namespace EyeTracker {
      static const QString NAME = "Gazepoint";
      static const bool ENABLE_GAZE_FOLLOW = false;
      static const bool IS_VR = false;
      static const QString PROCESSING_PARAMETER_KEY = "gazepoint";
   }
#endif

}


#endif // EYE_TRACKER_DEFINES_H
