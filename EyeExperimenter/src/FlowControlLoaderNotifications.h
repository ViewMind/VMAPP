#ifndef FLOWCONTROLLOADERNOTIFICATIONS_H
#define FLOWCONTROLLOADERNOTIFICATIONS_H

#include <QString>

// For now there is only one thing that the Loader needs to be notified.
// There might be more in the future.
namespace FCL {
   static const QString HMD_KEY_RECEIVED = "hmd_key_received";
   static const QString UPDATE_SAMP_FREQ = "update_sampling_frequecy";
   static const QString UPDATE_AVG_FREQ  = "update_avg_frequecy";
   static const QString UPDATE_MAX_FREQ  = "update_max_frequecy";
}

#endif // FLOWCONTROLLOADERNOTIFICATIONS_H
