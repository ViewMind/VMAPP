#ifndef DEBUGOPTIONS_H
#define DEBUGOPTIONS_H

#include "../../CommonClasses/ConfigurationManager/configurationmanager.h"
#include "defines.h"

/**
 * @brief The DebugOptions class - Simply creates a global class to access whatever debug options are set.
 */

class DebugOptions
{
public:
    DebugOptions();

    static bool LoadDebugOptions();
    static QString DebugString(const QString &key);
    static bool DebugBool(const QString &key);

private:
    static ConfigurationManager debug;

};

#endif // DEBUGOPTIONS_H
