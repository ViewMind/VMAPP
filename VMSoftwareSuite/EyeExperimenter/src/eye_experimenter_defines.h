#ifndef EYE_EXPERIMENTER_DEFINES_H
#define EYE_EXPERIMENTER_DEFINES_H

#define   EXPERIMENTER_VERSION_NUMBER                          "15.6.4"  // Previous 15.5.1
#ifdef USE_IVIEW
#define   EXPERIMENTER_VERSION                          EXPERIMENTER_VERSION_NUMBER " - SMI"
#else
#define   EXPERIMENTER_VERSION                          EXPERIMENTER_VERSION_NUMBER
#endif

#endif // EYE_EXPERIMENTER_DEFINES_H
