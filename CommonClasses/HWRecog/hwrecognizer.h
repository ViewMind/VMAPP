#ifndef HWRECOGNIZER_H
#define HWRECOGNIZER_H

#include <QString>
#include <QMap>
#include <QProcess>
#include <QSysInfo>
#include <QDebug>

namespace HWKeys {
   const QString PC_SN     = "serial_number";  /// wmic bios get serialnumber
   const QString PC_BRAND  = "pc_brand";       /// SystemInfo - System Manufacturer
   const QString PC_MODEL  = "pc_model";       /// SystemInfo - System Model
   const QString CPU_BRAND = "cpu_brand";      /// wmic cput get name
   const QString CPU_MODEL = "cpu_model";      /// wmic cput get name
   const QString GPU_BRAND = "gpu_brand";      /// wmic PATH Win32_videocontroller get name
   const QString GPU_MODEL = "gpu_model";      /// wmic PATH Win32_videocontroller get name
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

    // The only real function necessary. Will return the string values, and empty when it's not able to find anything.
    HardwareMap getHardwareSpecs() const;

    QStringList getErrors() const;

private:

    // The actual information returned, in an standarized manner.
    HardwareMap specs;

    QStringList lastErrors;

    // SystemInfo Parsed data.
    HardwareMap systemInfo;

    // List of Commands.
    const QString WINDOWS_REGISTRY_FOR_SYSINFO                = "HKEY_LOCAL_MACHINE\\HARDWARE\\DESCRIPTION\\System\\BIOS";
    const QString CMD_SYSTEMINFO                              = "systeminfo";

    const QString CMD_GET_DEVICE_SERIAL_NUMBER                = "wmic bios get serialnumber";
    const QString CMD_GET_CPU_NAME                            = "wmic cpu get name";
    const QString CMD_GET_GPU_NAME                            = "wmic PATH Win32_videocontroller get name";

    // Keys into the system info structure.
    const QString SYSINFO_KEY_SYS_MANUFACTURER                = "System Manufacturer";
    const QString SYSINFO_KEY_SYS_MODEL                       = "System Model";

    // Generic motor to run a console command in windows.
    QString runCommand(const QString &command, const QStringList &args, bool *ranOK);

    // Parser of system info with states for the parser.
    typedef enum {ST_KEY,ST_VALUE,ST_WAIT_VALUE,ST_KEY_OR_VALUE} SystemInfoParserState;
    void parseSystemInfo();

    // Struct to instruct how to interpret WMIC outputs, inlcuding the command and where to store the brand and model.
    typedef struct {
        bool onlyFirstLine;
        qint32 n_words_for_brand;
        QString command;
        QStringList brand;
        QStringList model;
    } WMICParseInfo;

    // Function that will parse the WMIC output of running the command stored in the structure and return brands and models specivied in the output.
    // This assumes that the first n words of every line is a brand and the last words are the model.
    bool parseWMICOutput(WMICParseInfo *wmic_struct);

};

#endif // HWRECOGNIZER_H
