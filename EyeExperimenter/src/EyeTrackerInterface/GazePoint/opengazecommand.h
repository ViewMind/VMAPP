#ifndef OPENGAZECOMMANDS_H
#define OPENGAZECOMMANDS_H

// The GazePoint Command id list for the GazePoint EyeTracker
#define GPC_ENABLE_SEND_DATA         "ENABLE_SEND_DATA"
#define GPC_ENABLE_SEND_COUNTER      "ENABLE_SEND_COUNTER"
#define GPC_ENABLE_SEND_TIME         "ENABLE_SEND_TIME"
#define GPC_ENABLE_SEND_TIME_TICK    "ENABLE_SEND_TIME_TICK"
#define GPC_ENABLE_SEND_POG_FIX      "ENABLE_SEND_POG_FIX"
#define GPC_ENABLE_SEND_POG_LEFT     "ENABLE_SEND_POG_LEFT"
#define GPC_ENABLE_SEND_POG_RIGHT    "ENABLE_SEND_POG_RIGHT"
#define GPC_ENABLE_SEND_POG_BEST     "ENABLE_SEND_POG_BEST"
#define GPC_ENABLE_SEND_PUPIL_LEFT   "ENABLE_SEND_PUPIL_LEFT"
#define GPC_ENABLE_SEND_PUPIL_RIGHT  "ENABLE_SEND_PUPIL_RIGHT"
#define GPC_ENABLE_SEND_EYE_LEFT     "ENABLE_SEND_EYE_LEFT"
#define GPC_ENABLE_SEND_EYE_RIGHT    "ENABLE_SEND_EYE_RIGHT"
#define GPC_ENABLE_SEND_CURSOR       "ENABLE_SEND_CURSOR"
#define GPC_ENABLE_SEND_USER_DATA    "ENABLE_SEND_USER_DATA"
#define GPC_CALIBRATE_START          "CALIBRATE_START"
#define GPC_CALIB_RESULT             "CALIB_RESULT"
#define GPC_CALIBRATE_SHOW           "CALIBRATE_SHOW"
#define GPC_CALIBRATE_TIMEOUT        "CALIBRATE_TIMEOUT"
#define GPC_CALIBRATE_DELAY          "CALIBRATE_DELAY"
#define GPC_CALIBRATE_RESULT_SUMMARY "CALIBRATE_RESULT_SUMMARY"
#define GPC_CALIBRATE_CLEAR          "CALIBRATE_CLEAR"
#define GPC_CALIBRATE_RESET          "CALIBRATE_RESET"
#define GPC_CALIBRATE_ADDPOINT       "CALIBRATE_ADDPOINT"
#define GPC_USER_DATA                "USER_DATA"
#define GPC_TRACKER_DISPLAY          "TRACKER_DISPLAY"
#define GPC_TIME_TICK_FREQUENCY      "TIME_TICK_FREQUENCY"
#define GPC_SCREEN_SIZE              "SCREEN_SIZE"
#define GPC_CAMERA_SIZE              "CAMERA_SIZE"
#define GPC_PRODUCT_ID               "PRODUCT_ID"
#define GPC_SERIAL_ID                "SERIAL_ID"
#define GPC_COMPANY_ID               "COMPANY_ID"
#define GPC_API_ID                   "API_ID"

// A list of the common GazePoint Fields used.
#define GPF_PTS                      "PTS"
#define GPF_VALUE                    "VALUE"
#define GPF_STATE                    "STATE"
#define GPF_LPOGX                    "LPOGX"
#define GPF_LPOGY                    "LPOGY"
#define GPF_LPOGV                    "LPOGV"
#define GPF_RPOGX                    "RPOGX"
#define GPF_RPOGY                    "RPOGY"
#define GPF_RPOGV                    "RPOGV"
#define GPF_TIME                     "TIME"
#define GPF_LPCX                     "LPCX"
#define GPF_LPCY                     "LPCY"
#define GPF_LPD                      "LPD"
#define GPF_LPS                      "LPS"
#define GPF_LPV                      "LPV"
#define GPF_RPCX                     "RPCX"
#define GPF_RPCY                     "RPCY"
#define GPF_RPD                      "RPD"
#define GPF_RPS                      "RPS"
#define GPF_RPV                      "RPV"

// The GazePoint Command Types
#define GPCT_NACK                    "NACK"
#define GPCT_ACK                     "ACK"
#define GPCT_SET                     "SET"
#define GPCT_GET                     "GET"
#define GPCT_REC                     "REC"
#define GPCT_CAL                     "CAL"

#include <QHash>
#include <QSet>
#include <QByteArray>
#include <QDebug>

class OpenGazeCommand
{
public:   

    OpenGazeCommand();

    // Functions that fill the data of the command
    void setID(const QString &cmd) {id = cmd;}
    void setType(const QString &type){ cmdType = type; }
    void setField(const QString &fieldName, const QString &value){ fields[fieldName] = value; }

    QString getCommandType() const {return cmdType;}
    QString getID() const {return id;}

    // Shorcut functions
    void setEnableCommand(const QString &cmd, bool enable);

    // Getting the internal data of a command.
    QString getField(const QString &fieldName) const;

    // Parse an input buffer. Returns an error if it could nto be interpreted.
    QString fromString(const QString &fullcmd);

    // Transforms to byte buffer.
    QByteArray prepareCommandToSend() const;

    // Clear all data
    void clear();

    // FOR DEBUGGING
    void prettyPrint() const;

private:
    QString cmdType;
    QString id;
    QHash<QString,QString> fields;

};

#endif // OPENGAZECOMMANDS_H
