#ifndef DATFILETORAWDATACONTAINER_H
#define DATFILETORAWDATACONTAINER_H

#include <QFile>
#include <QTextStream>
#include "../DatFileInfo/datfileinfoindir.h"
#include "rawdatacontainer.h"


class DatFileToRawDataContainer
{
public:
    DatFileToRawDataContainer();

    RawDataContainer fromDatFile(const QString &dat_file);

    QString getError() const;

private:

    QString error;

    /**
     * @brief separateStudyDescriptionFromData
     * @param file_path The path to the dat or datf, raw data file.
     * @param header    The header for the study in order to search and split
     * @return A list with the study description in the first part and the rest of the file in the second part.
     */
    QStringList separateStudyDescriptionFromData(const QString &file_path, const QString &header);

};



#endif // DATFILETORAWDATACONTAINER_H
