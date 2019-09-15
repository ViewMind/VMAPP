#ifndef LOADER_H
#define LOADER_H

#include <QObject>
#include <QUrl>
#include <QQuickWindow>
#include <QSystemSemaphore>
#include <QSharedMemory>
#include <QCoreApplication>

#include "../../../CommonClasses/LocalInformationManager/localinformationmanager.h"
#include "Experiments/experiment.h"
#include "eye_experimenter_defines.h"
#include "countries.h"
#include "uiconfigmap.h"


class Loader : public QObject
{
    Q_OBJECT
public:
    explicit Loader(QObject *parent = nullptr, ConfigurationManager *c = nullptr, CountryStruct *cs = nullptr, UIConfigMap *ui = nullptr);
    ~Loader();

    //******************** UI Functions ***************************
    Q_INVOKABLE QString getStringForKey(const QString &key);
    Q_INVOKABLE QStringList getStringListForKey(const QString &key);
    Q_INVOKABLE bool getLoaderError() const {return loadingError; }
    Q_INVOKABLE bool checkETChange();
    Q_INVOKABLE QString getWindowTilteVersion(){ return EXPERIMENTER_VERSION; }
    Q_INVOKABLE QRect frameSize(QObject *window);
    Q_INVOKABLE QStringList getCountryList() {return countries->getCountryList();}
    Q_INVOKABLE QStringList getCountryCodeList() {return countries->getCodeList();}
    Q_INVOKABLE int getDefaultCountry(bool offset = true);
    Q_INVOKABLE QString getCountryCodeForCountry(const QString &country) { return countries->getCodeForCountry(country); }
    Q_INVOKABLE int getCountryIndexFromCode(const QString &code) { return countries->getIndexFromCode(code); }
    Q_INVOKABLE QString loadTextFile(const QString &fileName);
    Q_INVOKABLE QStringList getErrorMessageForCode(quint8 code);
    Q_INVOKABLE QStringList getFileListForPatient(QString patuid, qint32 type);
    Q_INVOKABLE QStringList getFileListCompatibleWithSelectedBC(QString patuid, qint32 selectedBC);

    //******************** Configuration Functions ***************************
    Q_INVOKABLE QString getConfigurationString(const QString &key);
    Q_INVOKABLE bool getConfigurationBoolean(const QString &key);
    Q_INVOKABLE void setSettingsValue(const QString& key, const QVariant &var);
    Q_INVOKABLE void setValueForConfiguration(const QString &key, const QVariant &var) {configuration->addKeyValuePair(key,var);}
    Q_INVOKABLE void setAgeForCurrentPatient();

    //******************** Local DB Functions ***************************
    Q_INVOKABLE bool createPatientDirectory();
    Q_INVOKABLE QStringList generatePatientLists(const QString &filter = "", bool showAll = false);
    Q_INVOKABLE QStringList getPatientUIDLists() {return nameInfoList.patientUIDs;}
    Q_INVOKABLE QStringList getPatientIsOKList() {return nameInfoList.patientISOKList; }
    Q_INVOKABLE QStringList getPatientMedRecUpToDateList() {return nameInfoList.patientMedRecsUpToDateList;}
    Q_INVOKABLE void loadDoctorSelectionInformation() { nameInfoList = lim.getDoctorList(); }
    Q_INVOKABLE QStringList getDoctorNameList() {return nameInfoList.doctorNames; }
    Q_INVOKABLE QStringList getDoctorUIDList() {return nameInfoList.doctorUIDs; }
    Q_INVOKABLE QStringList getPatientDisplayIDList() { return nameInfoList.patientDisplayIDs; }
    Q_INVOKABLE QString getDoctorUIDByIndex(qint32 selectedIndex);
    Q_INVOKABLE qint32 getIndexOfDoctor(QString uid);
    Q_INVOKABLE bool isDoctorValidated(qint32 selectedIndex);
    Q_INVOKABLE bool isDoctorPasswordEmpty(qint32 selectedIndex);
    Q_INVOKABLE bool isDoctorPasswordCorrect(const QString &password);
    Q_INVOKABLE bool doesCurrentDoctorHavePassword() { return !lim.getDoctorPassword(configuration->getString(CONFIG_DOCTOR_UID)).isEmpty(); }
    Q_INVOKABLE QVariantMap getCurrentDoctorInformation() {return lim.getDoctorInfo(configuration->getString(CONFIG_DOCTOR_UID));}
    Q_INVOKABLE QVariantMap getCurrentPatientInformation() {return lim.getPatientInfo(configuration->getString(CONFIG_PATIENT_UID));}
    Q_INVOKABLE void addNewDoctorToDB(QVariantMap dbdata, QString password, bool hide);
    Q_INVOKABLE void addNewPatientToDB(QVariantMap dbdata);
    Q_INVOKABLE void addPatientMedicalRecord(QVariantMap medRecord, qint32 recordIndex);
    Q_INVOKABLE bool requestDrValidation(const QString &instPassword, qint32 selectedDr);
    Q_INVOKABLE bool verifyInstitutionPassword(const QString &instPass);
    Q_INVOKABLE QString getWorkingDirectory() const {return lim.getWorkDirectory();}
    Q_INVOKABLE bool getViewAllFlag() const {return lim.getViewAllFlag(); }
    Q_INVOKABLE void setViewAllFlag(bool flag) {lim.setViewAllFlag(flag); }
    Q_INVOKABLE void updateCurrentDoctorAndPatientDBFiles();
    Q_INVOKABLE void generateIDTable(const QString &urlPath);
    Q_INVOKABLE QString getNumberOfEvalsString(bool onlyEvals = false);
    Q_INVOKABLE void setNumberOfEvaluations(qint32 numevals);
    Q_INVOKABLE bool prepareMedicalRecordFiles(const QString &patid);

    //******************** Protocol related functions ***************************
    Q_INVOKABLE bool addProtocol(const QString &p) { return lim.addProtocol(p); }
    Q_INVOKABLE void deleteProtocol(const QString &p) { lim.deleteProtocol(p); }
    Q_INVOKABLE QStringList getProtocolList(bool full) { return lim.getProtocolList(full); }

    //******************** Report Related Functions ***************************
    Q_INVOKABLE QString getEvaluationID(const QString &existingFile);
    Q_INVOKABLE void operateOnRepGenStruct(qint32 index, qint32 type);
    Q_INVOKABLE QString getDatFileNameFromIndex(qint32 index, QString patuid, qint32 type);
    Q_INVOKABLE void reloadPatientDatInformation();

    //******************** Updater Related Functions **************************
    Q_INVOKABLE bool clearChangeLogFile();
    Q_INVOKABLE QString checkForChangeLog();
    Q_INVOKABLE void replaceEyeLauncher();

signals:
    void synchDone();

    // Signal to FlowControl, indicating the next file set to process.
    void fileSetReady(const QStringList &fileSet, const QString &evaluationID);


public slots:
    // Request of the flow control for the next set of files to process.
    void onFileSetRequested();

private:

    LogInterface logger;
    bool loadingError;
    ConfigurationManager *configuration;
    ConfigurationManager language;

    // To control data
    LocalInformationManager lim;

    // UI Config Map
    UIConfigMap *uimap;

    // The list of countries and their codes.
    CountryStruct *countries;

    // The list that holds list names and corresponding uids
    LocalInformationManager::DisplayLists nameInfoList;

    // Stores the data selected for processing.
    DatFileInfoInDir::ReportGenerationStruct reportGenerationStruct;

    // Loads default configurations when they don't exist.
    void loadDefaultConfigurations();

    // Sets the language for program.
    void changeLanguage();

};

#endif // LOADER_H
