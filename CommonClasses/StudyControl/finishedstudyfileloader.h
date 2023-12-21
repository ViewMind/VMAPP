#ifndef FINISHEDSTUDYFILELOADER_H
#define FINISHEDSTUDYFILELOADER_H

#include <QThread>

#include "../RenderServerClient/RenderServerPackets/RenderServerStrings.h"
#include "../RenderServerClient/RenderServerPackets/renderserverpacket.h"
#include "../RawDataContainer/VMDC.h"
#include "../RawDataContainer/viewminddatacontainer.h"
#include "../debug.h"
#include "eyexperimenter_defines.h"

class FinishedStudyFileLoader : public QThread
{
    Q_OBJECT

public:
    explicit FinishedStudyFileLoader(QObject *parent = nullptr);

    void setRawDataData(const ViewMindDataContainer &vmdc, const QString &study_file);
    void run();

    QString getError() const;
    ViewMindDataContainer getRawData() const;

    void clear();

private:
    ViewMindDataContainer rawdata;
    QString study_data_filename;
    QString error;

    /**
     * @brief processAndStoreStudyData - Stores the study data appropiately in the finalized study file.
     * @param control. A Render server Packet, created by a temporary study file.
     * @return True if there were no issues. False otherwise.
     */
    bool processAndStoreStudyData(const QVariantMap &control);

};

#endif // FINISHEDSTUDYFILELOADER_H
