#ifndef EYE_TRACKER_DEFINES_H
#define EYE_TRACKER_DEFINES_H

#include <QString>
#include "debug.h"

namespace Globals {



   namespace HPReverb {
      static const QString NAME = "HP Omnicept";
      static const bool IS_VR = true;
      static const QString PROCESSING_PARAMETER_KEY = "hpomnicept";
   }

   namespace EyeTracker{
      extern QString NAME;
      extern bool ENABLE_GAZE_FOLLOW;
      extern bool IS_VR;
      extern QString PROCESSING_PARAMETER_KEY;
   }

   static bool SetUpEyeTrackerNameSpace(const QString &key){
       if (key == HPReverb::PROCESSING_PARAMETER_KEY){
           EyeTracker::NAME = HPReverb::NAME;
           EyeTracker::IS_VR = HPReverb::IS_VR;
           EyeTracker::PROCESSING_PARAMETER_KEY = key;
           return true;
       }
       else{
           return false;
       }
   }


}


#endif // EYE_TRACKER_DEFINES_H
