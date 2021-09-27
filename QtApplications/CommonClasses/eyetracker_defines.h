#ifndef EYE_TRACKER_DEFINES_H
#define EYE_TRACKER_DEFINES_H

#include <QString>
#include "debug.h"

namespace Globals {


   namespace HTC {
      static const QString NAME = "HTCVive Pro";
      static const bool ENABLE_GAZE_FOLLOW = true;
      static const bool IS_VR = true;
      static const QString PROCESSING_PARAMETER_KEY = "htcviveeyepro";
      static const qreal VRSCALING = 1.0;
   }

   namespace HPReverb {
      static const QString NAME = "HP Omnicept";
      static const bool ENABLE_GAZE_FOLLOW = true;
      static const bool IS_VR = true;
      static const QString PROCESSING_PARAMETER_KEY = "hpomnicept";
      static const qreal VRSCALING = 1.0;
   }


   namespace GP3HD {
      static const QString NAME = "Gazepoint";
      static const bool ENABLE_GAZE_FOLLOW = false;
      static const bool IS_VR = false;
      static const QString PROCESSING_PARAMETER_KEY = "gazepoint";
      static const qreal VRSCALING = 1.0; // UNUSED. Required for compilation
   }

   namespace EyeTracker{
      extern QString NAME;
      extern bool ENABLE_GAZE_FOLLOW;
      extern bool IS_VR;
      extern QString PROCESSING_PARAMETER_KEY;
      extern qreal VRSCALING;
   }

   static bool SetUpEyeTrackerNameSpace(const QString &key){
       if (key == HTC::PROCESSING_PARAMETER_KEY){
           EyeTracker::NAME = HTC::NAME;
           EyeTracker::ENABLE_GAZE_FOLLOW = HTC::ENABLE_GAZE_FOLLOW;
           EyeTracker::IS_VR = HTC::IS_VR;
           EyeTracker::PROCESSING_PARAMETER_KEY = key;
           EyeTracker::VRSCALING = HTC::VRSCALING;
           return true;
       }
       else if (key == GP3HD::PROCESSING_PARAMETER_KEY){
           EyeTracker::NAME = GP3HD::NAME;
           EyeTracker::ENABLE_GAZE_FOLLOW = HTC::ENABLE_GAZE_FOLLOW;
           EyeTracker::IS_VR = GP3HD::IS_VR;
           EyeTracker::PROCESSING_PARAMETER_KEY = key;
           EyeTracker::VRSCALING = GP3HD::VRSCALING;
           return true;
       }
       else if (key == HPReverb::PROCESSING_PARAMETER_KEY){
           EyeTracker::NAME = HPReverb::NAME;
           EyeTracker::ENABLE_GAZE_FOLLOW = HPReverb::ENABLE_GAZE_FOLLOW;
           EyeTracker::IS_VR = HPReverb::IS_VR;
           EyeTracker::PROCESSING_PARAMETER_KEY = key;
           EyeTracker::VRSCALING = HPReverb::VRSCALING;
           return true;
       }
       else{
           return false;
       }
   }


}


#endif // EYE_TRACKER_DEFINES_H
