#ifndef STUDYCONFIGURATOR_H
#define STUDYCONFIGURATOR_H

/**
 * @brief The StudyConfigurator class
 * @details The point of the class is to act as a generic base to implement custom configuration
 * for each type of study. These implementation will be called ty the study control class.
 */

#include <QVariantMap>
#include <QFile>
#include <QTextStream>
#include "StudyConfiguration.h"
#include "../RawDataContainer/VMDC.h"

class StudyConfigurator
{
public:
    StudyConfigurator();
    /**
     * @brief createStudyConfiguration - Creates a configuration for render packet for the implemented study.
     * @param studyConfig - The configuration of the study according to user selection augmented with required processing parameters.
     * @param sample_f - The sample frequency for the headset.
     * @param md_percent - The maximum dispersion used for finding online fixations on a 2D plane online.
     * @param study_type - the name of the study. Some studies of similar types used the same configurator.
     * @param short_studies - Debug option to use a reduced set of studies. Crucial for testing.
     * @return True if no issues were found. False otherwise.
     */
    bool createStudyConfiguration(const QVariantMap &studyConfig,
                                  qreal sample_f,
                                  qreal md_percent,
                                  const QString &study_type,
                                  const bool short_studies);

    /**
     * @brief getConfiguration
     * @return - The configuration payload for a given study, ready to be a render server packet
     */
    QVariantMap getConfiguration() const;

    /**
     * @brief getError
     * @return The last error message
     */
    QString getError() const;

    /**
     * @brief getVersionString
     * @return The version string of the parsed study fiel.
     */
    QString getVersionString() const;

    /**
     * @brief getDefaultStudyEye
     * @return - Returns the preferred eye defined to use with fixation logic. Varies per study.
     */
    QString getDefaultStudyEye() const;

    /**
     * @brief getStudyDescriptionVersion
     * @return - The study description version.
     */
    QString getStudyDescriptionVersion() const;

    /**
     * @brief getStudyDescriptionContents
     * @return - The content parsed in order to generate the study structure.
     */
    QString getStudyDescriptionContents() const;

protected:

    QVariantMap configuration;
    QString studyType;
    QString error;
    QString versionString;
    QString studyDescriptionContent;
    QString studyDescriptionFile;
    QString defaultStudyEye;
    bool shortStudies;

    const qint32 N_TRIALS_IN_SHORT_STUDIES = 4;

    /**
     * @brief openStudyDescriptionFile - Opens and description file loading the contents.
     * @param filename - The internal path to the file name to open. The file is part of the binary.
     * @return True if successfull, false otherwise
     */
    bool openStudyDescriptionFile(const QString &filename);

    /**
     * @brief studySpecificConfiguration - Create teh full study configuration. Implemented by each particular study.
     * @param studyCofig - The study configuration.
     * @return True if no issues were found, false otherwise.
     */
    virtual bool studySpecificConfiguration(const QVariantMap &studyCofig);

};

#endif // STUDYCONFIGURATOR_H
