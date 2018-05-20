#ifndef EYE_EXPERIMENTER_DEFINES_H
#define EYE_EXPERIMENTER_DEFINES_H

// Used only for extended binding options.
#define  ENABLE_EXPANDED_BINDING_OPTIONS

// Show the console for debugging messages when cannot find bug by locally running it.
// #define  SHOW_CONSOLE

#define   EXPERIMENTER_VERSION                          "1.0.2"

#ifdef ENABLE_EXPANDED_BINDING_OPTIONS
   #define   EXPANDED_VERSION                           "-EBO"
#else
   #define   EXPANDED_VERSION                           ""
#endif

// Eye Trackers
#define   EYE_TRACKER_REDM                              0
#define   EYE_TRACKER_MOUSE                             1

#endif // EYE_EXPERIMENTER_DEFINES_H
