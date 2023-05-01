#ifndef STUDYENDOPERATIONS_H
#define STUDYENDOPERATIONS_H

#include <QThread>

#include "../../CommonClasses/Experiments/binding/bindingexperiment.h"
#include "../../CommonClasses/RawDataContainer/viewminddatacontainer.h"

#include "eyexperimenter_defines.h"

class StudyEndOperations : public QThread
{
public:

    explicit StudyEndOperations();

    void processFile(const QString &file, const QString &idx_file);

    void run() override;

    bool wasOperationSuccssful() const;

private:

    QString dataFile;
    QString idxFile;
    bool operationsSuccess;
    ViewMindDataContainer vmdc;

    qreal computeGoNoGoIndex() const;
    qreal computeBindingIndex(const QString &which_binding) const;
    qreal computeGNGSphereIndex() const;
    qreal computeNBackIndex() const;


};

#endif // STUDYENDOPERATIONS_H
