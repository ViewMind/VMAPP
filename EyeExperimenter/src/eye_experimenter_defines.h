#ifndef EYE_EXPERIMENTER_DEFINES_H
#define EYE_EXPERIMENTER_DEFINES_H

#define   EXPERIMENTER_VERSION_NUMBER                          "7.0.0 Novartis"
#ifdef USE_IVIEW
#define   EXPERIMENTER_VERSION                          EXPERIMENTER_VERSION_NUMBER " - SMI"
#else
#define   EXPERIMENTER_VERSION                          EXPERIMENTER_VERSION_NUMBER " - GP"
#endif

#endif // EYE_EXPERIMENTER_DEFINES_H
