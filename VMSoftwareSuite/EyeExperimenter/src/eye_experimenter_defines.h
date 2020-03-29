#ifndef EYE_EXPERIMENTER_DEFINES_H
#define EYE_EXPERIMENTER_DEFINES_H

#define   EXPERIMENTER_VERSION_NUMBER                          "13.2.0"  // Previous 13.1.0
#ifdef USE_IVIEW
#define   EXPERIMENTER_VERSION                          EXPERIMENTER_VERSION_NUMBER " - SMI"
#else
#define   EXPERIMENTER_VERSION                          EXPERIMENTER_VERSION_NUMBER
#endif

#endif // EYE_EXPERIMENTER_DEFINES_H
