#ifndef EYE_TRACKER_DEFINES_H
#define EYE_TRACKER_DEFINES_H

#include <QString>
#include "debug.h"

namespace Globals {

   namespace HPReverb {
      static const QString NAME = "HP Omnicept";
      static const QString PROCESSING_PARAMETER_KEY = "hpomnicept";
   }

   namespace EyeTracker{
      extern QString NAME;
      extern QString PROCESSING_PARAMETER_KEY;
   }

   static bool SetUpEyeTrackerNameSpace(const QString &key){
       if (key == HPReverb::PROCESSING_PARAMETER_KEY){
           EyeTracker::NAME = HPReverb::NAME;
           EyeTracker::PROCESSING_PARAMETER_KEY = key;
           return true;
       }
       else{
           return false;
       }
   }


}


#endif // EYE_TRACKER_DEFINES_H
