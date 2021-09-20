#ifndef DATFILETORAWDATACONTAINER_H
#define DATFILETORAWDATACONTAINER_H

#include <QFile>
#include <QFileInfo>
#include <QTextStream>
#include <QProcess>
#include <iostream>
#include "viewminddatacontainer.h"

#include "../DatFileInfo/datfileinfoindir.h"
#include "../DataAnalysis/EyeMatrixGenerator/movingwindowalgorithm.h"
#include "../ConfigurationManager/configurationmanager.h"
#include "../Experiments/fieldingparser.h"
#include "../Experiments/gonogoparser.h"

class DatFileToViewMindDataContainer
{
public:
    DatFileToViewMindDataContainer();

    bool fromDatFile(const QString &dat_file, const ConfigurationManager &configmng, ViewMindDataContainer *rdc);

    QString getError() const;

    QString getFinalStudyFileName() const;

    static QString checkCodecForFile(const QString &file);


private:

    QString error;

    /**
     * @brief separateStudyDescriptionFromData
     * @param file_path The path to the dat or datf, raw data file.
     * @param header    The header for the study in order to search and split
     * @return A list with the study description in the first part and the rest of the file in the second part.
     */
    QStringList separateStudyDescriptionFromData(const QString &file_path, const QString &header);

    void parseStudyData(const QString &raw_data, const QString &exp, QString, ViewMindDataContainer *rdc);
    void parseReading(const QString &raw_data, const QString &exp, ViewMindDataContainer *rdc);
    void parseNBackRT(const QString &raw_data, const QString &exp, ViewMindDataContainer *rdc);
    void parseBinding(const QString &raw_data, ViewMindDataContainer *rdc, bool isBC);
    void parseGoNoGo(const QString &raw_data, ViewMindDataContainer *rdc);

    // Used only for the reading experiment.
    QMap<QString,QString> readingSentences;

    // Actual Study name
    QString finalFileStudyName;

    // Online fixation computation.
    Fixation lastFixationL;
    Fixation lastFixationR;
    MovingWindowAlgorithm rMWA;
    MovingWindowAlgorithm lMWA;

    // Finalizes and restes the state machine for computing online fixations.
    void finalizeOnlineFixations(ViewMindDataContainer *rawdata);

    // Feeds data to the online fixation maching. If a fixation is computed, it is stored in the rawdata structure.
    // The last two parameters are reading ONLY.
    void computeOnlineFixations(ViewMindDataContainer *rawdata, const QVariantMap &data, qreal l_schar = 0, qreal l_word = 0, qreal r_schar = 0, qreal r_word = 0);

    // Transform the fixation struct into a the valid struct expected by the ViewMind Data Container.
    QVariantMap fixationToVariantMap(const Fixation &f);

    bool rightEyeEnabled;
    bool leftEyeEnabled;

    // For the online fixations.
    QString studyType;

};



#endif // DATFILETORAWDATACONTAINER_H
