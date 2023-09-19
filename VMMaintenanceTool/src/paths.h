#ifndef PATHS_H
#define PATHS_H

#include <QString>
#include <QFileInfo>
#include <QDir>
#include <QSet>

#include "defines.h"
#include "debugoptions.h"

/**
 * @brief The PathsManager class - A collection of static variables and functions that will behave as a global repository of all the paths that the application needs.
 *
 */

class Paths
{
public:

    typedef enum {
        PI_CONTAINER_DIR,
        PI_INSTALL_DIR,
        PI_UNDERSCORE_DIR,
        PI_BKP_DIR,
        PI_BKP_VMDATA_DIR,
        PI_BKP_VMCONFIG_FILE,
        PI_BKP_DBBKP_DIR,
        PI_UNDERS_VMDATA_DIR,
        PI_UNDERS_VMCONFIG_FILE,
        PI_UNDERS_DBBKP_DIR,
        PI_CURRENT_DB_FILE,
        PI_CURRENT_VMCONFIG_FILE,
        PI_CURRENT_DBBKP,
        PI_CURRENT_EXE_FILE
    } PathID;

    Paths();

    /**
     * @brief FillPaths - Needs to be called as soon as the application starts up so that I can fill out all the variables.
     */
    static void FillPaths();

    /**
     * @brief Path - Gets the one of the paths used by the maintainer.
     * @param p - The path desired.
     * @return - The full path.
     */
    static QString Path(PathID p);

    /**
     * @brief Exists - Returns if the file or dir exists.
     * @param p - The path id enum.
     * @return - True if it exists, false otherwise.
     */
    static bool Exists(PathID p);

private:

    static QString pathToDirContainingEyeExpInstall;
    static QString pathToEyeExpInstallDir;
    static QString pathToUnCompressedNewEyeExpDir;
    static QString pathToBKPOldEyeInstallDir;

    static QString pathToBKPOldEyeInstallVMData;
    static QString pathToBKPOldEyeInstallVMConfig;
    static QString pathToBKPOldEyeInstallDBBKPDir;

    static QString pathToUnCompressedNewEyeExpDirVMData;
    static QString pathToUnCompressedNewEyeExpDirVMConfig;
    static QString pathToUnCompressedNewEyeExpDirDBBKPDir;

    static QString pathToCurrentDB;
    static QString pathToCurrentVMConfig;
    static QString pathToCurrentDBBkp;
    static QString pathToCurrentEXE;

    static bool IsFile(PathID p);

};

#endif // PATHS_H
