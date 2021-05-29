#ifndef DATFILETORAWDATACONTAINER_H
#define DATFILETORAWDATACONTAINER_H

#include <QFile>
#include <QFileInfo>
#include <QTextStream>
#include <QProcess>
#include <iostream>
#include "rawdatacontainer.h"

#include "../DatFileInfo/datfileinfoindir.h"
#include "../ConfigurationManager/configurationmanager.h"
#include "../Experiments/fieldingparser.h"
#include "../Experiments/gonogoparser.h"


class DatFileToRawDataContainer
{
public:
    DatFileToRawDataContainer();

    RawDataContainer fromDatFile(const QString &dat_file, const ConfigurationManager &configmng);

    QString getError() const;


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

    RawDataContainer parseStudyData(const QString &raw_data, const QString &exp, RawDataContainer::StudyType, RawDataContainer rdc, RawDataContainer::TrialListType tlt);
    RawDataContainer parseReading(const QString &raw_data, const QString &exp, RawDataContainer rdc);
    RawDataContainer parseNBackRT(const QString &raw_data, const QString &exp, RawDataContainer rdc);
    RawDataContainer parseBinding(const QString &raw_data, RawDataContainer rdc, RawDataContainer::TrialListType tlt);
    RawDataContainer parseGoNoGo(const QString &raw_data, RawDataContainer rdc);

    // Used only for the reading experiment.
    QMap<QString,QString> readingSentences;

};



#endif // DATFILETORAWDATACONTAINER_H
