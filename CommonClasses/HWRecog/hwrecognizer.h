#ifndef HWRECOGNIZER_H
#define HWRECOGNIZER_H

#include <QString>
#include <QMap>
#include <QProcess>
#include <QSysInfo>
#include <QDebug>
#include <QFile>
#include <QTextStream>
#include <QVariantMap>
#include "tableoutputparser.h"

namespace HWKeys {
   const QString PC_SN        = "serial_number";  /// wmic bios get serialnumber
   const QString PC_BRAND     = "pc_brand";       /// SystemInfo - System Manufacturer
   const QString PC_MODEL     = "pc_model";       /// SystemInfo - System Model
   const QString CPU_MODEL    = "cpu_model";      /// wmic cput get name
   const QString CPU_BRAND    = "cpu_brand";
   const QString GPU_MODEL    = "gpu_model";      /// wmic PATH Win32_videocontroller get name
   const QString GPU_BRAND    = "gpu_brand";
   const QString DISK_MODEL   = "hdd_model";      /// wmic diskdrive get model, serialNumber, size, mediaType
   const QString DISK_SN      = "hdd_sn";         /// wmic diskdrive get model, serialNumber, size, mediaType
   const QString DISK_SIZE    = "hdd_size";       /// wmic diskdrive get model, serialNumber, size, mediaType
   const QString TOTAL_RAM    = "total_ram";      /// SystemInfo - Available Physical Memory
   const QString HMD_BRAND    = "hmd_brand";      /// Depeding on which serial number we are able to find, we kown the hmd brand.
   const QString HMD_SN       = "hmd_sn";         /// If available, the serial number of the headset
}

/**
 * @brief The HWRecognizer class is an abstract class
 * Information getting might be very specific to each different computer we work with
 * So This abstract class will provide a common interface and then we can have different implementations if necessary.
 */

class HWRecognizer
{
public:
    HWRecognizer();

    // Hardware Map For Convenience.
    typedef QMap<QString,QString> HardwareMap;

    // Get all the information as a string map.
    HardwareMap getHardwareSpecs() const;

    // Get the information as a VariantMap.
    QVariantMap getHardwareSpecsAsVariantMap() const;

    // Get errors, if any.
    QStringList getErrors() const;

    // Get warnings, if any.
    QStringList getWarnings() const;

    // Get information as a string, either coded (values separated by a | and key value pairs by a ||) or pretty printed.
    QString toString(bool prettyPrint) const;

private:

    // The actual information returned, in an standarized manner.
    HardwareMap specs;

    QList< QMap<QString,QString> > pnputilinfo;

    // Errors.
    QStringList lastErrors;

    // Warnings.
    QStringList lastWarnings;

    // SystemInfo Parsed data.
    HardwareMap systemInfo;

    // List of Commands.
    const QString WINDOWS_REGISTRY_FOR_SYSINFO                = "HKEY_LOCAL_MACHINE\\HARDWARE\\DESCRIPTION\\System\\BIOS";
    const QString CMD_SYSTEMINFO                              = "systeminfo";

    const QString CMD_GET_DEVICE_SERIAL_NUMBER                = "wmic bios get serialnumber";
    const QString CMD_GET_CPU_NAME                            = "wmic cpu get name";
    const QString CMD_GET_GPU_NAME                            = "wmic PATH Win32_videocontroller get name"; //
    const QString CMD_GET_HDD_INFO                            = "wmic diskdrive get model, serialNumber, size";
    const QString CMD_PNPINFO                                 = "pnputil /enum-devices /connected";
    const QString CMD_GET_DEVICE_PROPERTY                     = "powershell \"Get-PnpDeviceProperty -InstanceId '<<DEVICE_ID>>'\"";

    // Keys into the system info structure.
    const QString SYSINFO_KEY_SYS_MANUFACTURER                = "System Manufacturer";
    const QString SYSINFO_KEY_SYS_MODEL                       = "System Model";
    const QString SYSINFO_KEY_RAM                             = "Total Physical Memory";

    // WMIC Keys
    const QString WMIC_KEY_NAME                               = "Name";
    const QString WMIC_KEY_MODEL                              = "Model";
    const QString WMIC_KEY_SN                                 = "SerialNumber";
    const QString WMIC_KEY_SIZE                               = "Size";

    // PNP INFO KEYS
    const QString PNP_KEY_INSTID                              = "Instance ID";
    const QString PNP_KEY_DESC                                = "Device Description";
    const QString PNP_KEY_CLASS                               = "Class Name";
    const QString PNP_KEY_CLASS_ID                            = "Class GUID";
    const QString PNP_KEY_BRAND                               = "Manufacturer Name";
    const QString PNP_KEY_STATUS                              = "Status";
    const QString PNP_KEY_DRIVER                              = "Driver Name";
    const QString PNP_KEY_EXT_DRIVER                          = "Extension Driver Names";

    // PNP DEVICE PROPERTY_KEYS
    const QString PNP_DEV_PROP_KEY_KEYNAME                    = "KeyName";
    const QString PNP_DEV_PROP_KEY_INSTANCEID                 = "InstanceId";
    const QString PNP_DEV_PROP_KEY_TYPE                       = "Type";
    const QString PNP_DEV_PROP_KEY_DATA                       = "Data";

    // HP Omnicept related constants.
    const QString HP_DEVICE_DESCRIPTION                       = "HP Reverb G2 Omnicept";
    const QString HP_DEVICE_SN_PNP_PROPERTY_KEY               = "{6D166322-FA1D-4223-9463-201AFD540BC8} 0";

    // Generic motor to run a console command in windows.
    QString runCommand(const QString &command, const QStringList &args, bool *ranOK);

    // Parser of system info with states for the parser.
    typedef enum {ST_KEY,ST_VALUE,ST_WAIT_VALUE,ST_KEY_OR_VALUE} SystemInfoParserState;
    void parseSystemInfo();

    // The as table printed information is parsed
    TableOutputParser::TableColumnList parseWMICOutputAsTable(const QString &cmd);
    // PNP Command functions.
    void parsePNPUtilInfo();
    QMap<QString,QString> parseSinglePNPInfoEntry(const QStringList &lines);
    QList< QMap<QString,QString> >  searchPNPInfo(const QString &key, const QString &value);

    // Get a device property
    TableOutputParser::ParseTableOutput getDevicePropertiesByID(const QString &deviceID);

    // Device specific functios.
    QString findHPOmniceptSN();

};

#endif // HWRECOGNIZER_H
